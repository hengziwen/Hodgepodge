#include "CodexText/HodgeGroundedLocomotion.h"
#if WITH_EDITOR
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimSequence.h"
#include "Engine/SkeletalMesh.h"
#include "AnimGraphNode_Root.h"
#include "AnimGraphNode_StateMachine.h"
#include "AnimGraphNode_Slot.h"
#include "AnimGraphNode_SequencePlayer.h"
#include "AnimGraphNode_SequenceEvaluator.h"
#include "AnimGraphNode_TransitionResult.h"
#include "AnimGraphNode_LocalToComponentSpace.h"
#include "AnimGraphNode_ComponentToLocalSpace.h"
#include "AnimGraphNode_TwoBoneIK.h"
#include "AnimGraphNode_ModifyBone.h"
#include "AnimGraphNode_CopyBone.h"
#include "AnimStateNode.h"
#include "AnimStateEntryNode.h"
#include "AnimStateTransitionNode.h"
#include "AnimationStateMachineGraph.h"
#include "K2Node_VariableGet.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"

namespace HodgeGroundedGraph
{
    template<class T> T* Node(UEdGraph* Graph, int32 X, int32 Y)
    {
        FGraphNodeCreator<T> C(*Graph); auto* N=C.CreateNode(); N->NodePosX=X; N->NodePosY=Y; C.Finalize(); return N;
    }
    UEdGraphPin* Output(UEdGraphNode* Node)
    {
        for (auto* P:Node->Pins) if(P->Direction==EGPD_Output) return P;
        return nullptr;
    }
}
#endif

bool UHodgeGroundedAuthoring::AddGroundedLayer(UObject* Blueprint, const TArray<UObject*>& Actions, UObject* Idle, UObject* Fall, UObject* PreviewMesh)
{
#if WITH_EDITOR
    using namespace HodgeGroundedGraph;
    UAnimBlueprint* BP=Cast<UAnimBlueprint>(Blueprint);
    if(!BP || !BP->GetPathName().StartsWith(TEXT("/Game/CodexText/")) || BP->ParentClass!=UHodgeGroundedLocomotion::StaticClass() || Actions.Num()!=6) return false;
    for(auto* A:Actions) if(!Cast<UAnimSequence>(A) || !A->GetPathName().StartsWith(TEXT("/Game/CodexText/"))) return false;
    if(!Cast<UAnimSequence>(Idle) || !Cast<UAnimSequence>(Fall) || !Cast<USkeletalMesh>(PreviewMesh)) return false;
    TArray<UEdGraph*> Graphs; BP->GetAllGraphs(Graphs);
    UEdGraph* Graph=nullptr;
    for(auto* G:Graphs) { if(G->GetFName()==TEXT("Grounded")) return false; if(G->GetFName()==TEXT("AnimGraph")) Graph=G; }
    if(!Graph) return false;
    UAnimGraphNode_Root* Root=nullptr; UAnimGraphNode_StateMachine* Cycle=nullptr; UAnimGraphNode_Slot* Slot=nullptr;
    for(UEdGraphNode* N:Graph->Nodes)
    {
        if(auto* R=Cast<UAnimGraphNode_Root>(N)) Root=R;
        if(auto* C=Cast<UAnimGraphNode_StateMachine>(N)) Cycle=C;
        if(auto* S=Cast<UAnimGraphNode_Slot>(N)) Slot=S;
    }
    if(!Root || !Cycle || !Slot) return false;
    BP->Modify(); Graph->Modify(); bool Good=true;
    auto Wire=[&](UEdGraph* G,UEdGraphPin* A,UEdGraphPin* B)
    {
        if(!A || !B || !G->GetSchema()->TryCreateConnection(A,B)) { Good=false; UE_LOG(LogTemp,Error,TEXT("Grounded graph connection failed: %s (%s -> %s)"),*G->GetName(),A?*A->PinName.ToString():TEXT("null"),B?*B->PinName.ToString():TEXT("null")); }
    };
    auto Variable=[&](UEdGraph* G,FName Name,UEdGraphPin* Pin,int32 X,int32 Y)
    {
        FGraphNodeCreator<UK2Node_VariableGet> C(*G);auto* V=C.CreateNode();V->VariableReference.SetSelfMember(Name);V->NodePosX=X;V->NodePosY=Y;C.Finalize();Wire(G,V->FindPin(Name),Pin);
    };
    auto Expose=[](UAnimGraphNode_Base* N,FName Name)
    { for(auto& P:N->ShowPinForProperties) if(P.PropertyName==Name) P.bShowPin=true; N->ReconstructNode(); };
    Root->FindPin(TEXT("Result"))->BreakAllPinLinks();Slot->FindPin(TEXT("Source"))->BreakAllPinLinks();
    auto* Outer=Node<UAnimGraphNode_StateMachine>(Graph,-1600,0);
    UAnimationStateMachineGraph* SM=Outer->EditorStateMachineGraph;
    FBlueprintEditorUtils::RenameGraph(SM,TEXT("Grounded"));
    const TCHAR* Names[]={TEXT("Idle"),TEXT("Move"),TEXT("StopWalkLeft"),TEXT("StopWalkRight"),TEXT("StopRunLeft"),TEXT("StopRunRight"),TEXT("TurnLeft"),TEXT("TurnRight"),TEXT("Fall")};
    const TCHAR* Flags[]={TEXT("bGroundIdle"),TEXT("bGroundMove"),TEXT("bGroundStopWalkL"),TEXT("bGroundStopWalkR"),TEXT("bGroundStopRunL"),TEXT("bGroundStopRunR"),TEXT("bGroundTurnL"),TEXT("bGroundTurnR"),TEXT("bLabFall")};
    TArray<UAnimStateNode*> States;
    for(int32 I=0;I<9;++I)
    {
        auto* S=Node<UAnimStateNode>(SM,(I%3)*450,(I/3)*320);FBlueprintEditorUtils::RenameGraph(S->BoundGraph,Names[I]);States.Add(S);
        UEdGraph* G=S->BoundGraph;
        if(I==0 || I==8)
        {
            FGraphNodeCreator<UAnimGraphNode_SequencePlayer> C(*G);auto* P=C.CreateNode();P->Node.SetSequence(Cast<UAnimSequence>(I==0?Idle:Fall));P->NodePosX=-350;C.Finalize();Wire(G,Output(P),S->GetPoseSinkPinInsideState());
        }
        else if(I>=2)
        {
            FGraphNodeCreator<UAnimGraphNode_SequenceEvaluator> C(*G);auto* P=C.CreateNode();P->Node.SetSequence(Cast<UAnimSequence>(Actions[I-2]));P->NodePosX=-350;C.Finalize();
            Expose(P,TEXT("ExplicitTime"));Variable(G,TEXT("GroundActionTime"),P->FindPin(TEXT("ExplicitTime")),-650,100);Wire(G,Output(P),S->GetPoseSinkPinInsideState());
        }
    }
    // Move the existing, tested directional graph under the Move state, retaining its asset players.
    Graph->Nodes.Remove(Cycle);Graph->SubGraphs.Remove(Cycle->EditorStateMachineGraph);
    Cycle->Rename(nullptr,States[1]->BoundGraph,REN_DontCreateRedirectors);
    States[1]->BoundGraph->Nodes.Add(Cycle);States[1]->BoundGraph->SubGraphs.Add(Cycle->EditorStateMachineGraph);
    Cycle->NodePosX=-400;Cycle->NodePosY=0;Wire(States[1]->BoundGraph,Output(Cycle),States[1]->GetPoseSinkPinInsideState());
    UAnimationStateMachineGraph* Inner=Cycle->EditorStateMachineGraph;
    TArray<UAnimStateNode*> Remove;UAnimStateNode* Forward=nullptr;
    for(UEdGraphNode* N:Inner->Nodes) if(auto* S=Cast<UAnimStateNode>(N))
    { if(S->BoundGraph->GetFName()==TEXT("Idle") || S->BoundGraph->GetFName()==TEXT("Fall")) Remove.Add(S); if(S->BoundGraph->GetFName()==TEXT("Forward")) Forward=S; }
    const auto OldNodes=Inner->Nodes;
    for(UEdGraphNode* N:OldNodes) if(auto* T=Cast<UAnimStateTransitionNode>(N))
    { if(Remove.Contains(Cast<UAnimStateNode>(T->GetPreviousState())) || Remove.Contains(Cast<UAnimStateNode>(T->GetNextState()))) T->DestroyNode(); }
    Inner->EntryNode->Pins[0]->BreakAllPinLinks();for(auto* S:Remove) S->DestroyNode();
    if(!Forward) return false;Wire(Inner,Inner->EntryNode->Pins[0],Forward->GetInputPin());
    Wire(SM,SM->EntryNode->Pins[0],States[0]->GetInputPin());
    for(int32 From=0;From<9;++From) for(int32 To=0;To<9;++To)
    {
        if(From==To) continue;
        const bool Allowed=To==8 || To==0 || To==1 || (From==0 && To>=6 && To<=7) || (From==1 && To>=2 && To<=5);
        if(!Allowed) continue;
        auto* T=Node<UAnimStateTransitionNode>(SM,0,0);T->CrossfadeDuration=(To==8?0.08f:0.12f);T->CreateConnections(States[From],States[To]);
        for(UEdGraphNode* N:T->BoundGraph->Nodes) if(auto* R=Cast<UAnimGraphNode_TransitionResult>(N))
        { Variable(T->BoundGraph,Flags[To],R->FindPin(TEXT("bCanEnterTransition")),-250,0);break; }
    }
    Wire(Graph,Output(Outer),Slot->FindPin(TEXT("Source")));Slot->NodePosX=-1250;
    auto* ToCS=Node<UAnimGraphNode_LocalToComponentSpace>(Graph,-900,0);Wire(Graph,Output(Slot),ToCS->FindPin(TEXT("LocalPose")));
    UEdGraphPin* Previous=Output(ToCS);
    // Existing virtual bones target toes. Cache ankle transforms explicitly before deforming either leg.
    for(int32 I=0;I<2;++I)
    {
        FGraphNodeCreator<UAnimGraphNode_CopyBone> CC(*Graph);auto* Copy=CC.CreateNode();
        Copy->Node.SourceBone.BoneName=I==0?TEXT("Bip001LFoot"):TEXT("Bip001RFoot");
        Copy->Node.TargetBone.BoneName=I==0?TEXT("VB ik_foot_left"):TEXT("VB ik_foot_right");
        Copy->Node.bCopyTranslation=true;Copy->Node.bCopyRotation=true;Copy->Node.bCopyScale=true;
        Copy->Node.ControlSpace=BCS_ComponentSpace;Copy->NodePosX=-800+I*150;Copy->NodePosY=-200;CC.Finalize();
        Wire(Graph,Previous,Copy->FindPin(TEXT("ComponentPose")));Previous=Output(Copy);
    }
    for(int32 I=0;I<2;++I)
    {
        FGraphNodeCreator<UAnimGraphNode_TwoBoneIK> IC(*Graph);auto* IK=IC.CreateNode();
        IK->Node.IKBone.BoneName=I==0?TEXT("Bip001LFoot"):TEXT("Bip001RFoot");
        IK->Node.EffectorLocationSpace=BCS_ComponentSpace;IK->Node.JointTargetLocationSpace=BCS_ComponentSpace;
        IK->Node.JointTargetLocation=FVector(I==0?25.f:-25.f,120.f,45.f);IK->Node.bAllowStretching=false;
        IK->Node.bMaintainEffectorRelRot=true;IK->NodePosX=-550+I*850;IC.Finalize();
        Expose(IK,TEXT("Alpha"));
        Wire(Graph,Previous,IK->FindPin(TEXT("ComponentPose")));
        Variable(Graph,I==0?TEXT("FootTargetL"):TEXT("FootTargetR"),IK->FindPin(TEXT("EffectorLocation")),IK->NodePosX-200,200);
        Variable(Graph,I==0?TEXT("FootAlphaL"):TEXT("FootAlphaR"),IK->FindPin(TEXT("Alpha")),IK->NodePosX-200,320);
        FGraphNodeCreator<UAnimGraphNode_ModifyBone> RC(*Graph);auto* R=RC.CreateNode();
        R->Node.BoneToModify.BoneName=IK->Node.IKBone.BoneName;R->Node.RotationMode=BMM_Replace;R->Node.RotationSpace=BCS_ComponentSpace;R->NodePosX=IK->NodePosX+400;RC.Finalize();
        Expose(R,TEXT("Rotation"));Expose(R,TEXT("Alpha"));Wire(Graph,Output(IK),R->FindPin(TEXT("ComponentPose")));
        Variable(Graph,I==0?TEXT("FootRotationL"):TEXT("FootRotationR"),R->FindPin(TEXT("Rotation")),R->NodePosX-150,220);
        Variable(Graph,I==0?TEXT("FootAlphaL"):TEXT("FootAlphaR"),R->FindPin(TEXT("Alpha")),R->NodePosX-150,350);
        Previous=Output(R);
    }
    auto* ToLS=Node<UAnimGraphNode_ComponentToLocalSpace>(Graph,1200,0);Wire(Graph,Previous,ToLS->FindPin(TEXT("ComponentPose")));Root->NodePosX=1550;Wire(Graph,Output(ToLS),Root->FindPin(TEXT("Result")));
    BP->SetPreviewMesh(Cast<USkeletalMesh>(PreviewMesh));
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);FKismetEditorUtilities::CompileBlueprint(BP);
    return Good && BP->Status!=BS_Error;
#else
    return false;
#endif
}
