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
#include "AnimGraphNode_LayeredBoneBlend.h"
#include "AnimGraph/AnimGraphNode_StrideWarping.h"
#include "AnimGraph/AnimGraphNode_FootPlacement.h"
#include "AnimStateNode.h"
#include "AnimStateEntryNode.h"
#include "AnimStateTransitionNode.h"
#include "AnimationStateMachineGraph.h"
#include "K2Node_VariableGet.h"
#include "K2Node_CallFunction.h"
#include "Kismet/KismetMathLibrary.h"
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

    void PreserveKneeDirection(FAnimNode_TwoBoneIK& IK, int32 Side)
    {
        IK.JointTargetLocationSpace = BCS_BoneSpace;
        IK.JointTarget.BoneReference.BoneName = Side == 0 ? TEXT("Bip001LCalf") : TEXT("Bip001RCalf");
        IK.JointTargetLocation = FVector::ZeroVector;
    }
}
#endif

bool UHodgeGroundedAuthoring::RepairGroundedLegIK(UObject* Blueprint)
{
#if WITH_EDITOR
    using namespace HodgeGroundedGraph;
    auto* BP = Cast<UAnimBlueprint>(Blueprint);
    if (!BP || !BP->GetPathName().StartsWith(TEXT("/Game/CodexText/")) || BP->ParentClass != UHodgeGroundedLocomotion::StaticClass()) return false;
    TArray<UEdGraph*> Graphs; BP->GetAllGraphs(Graphs);
    UAnimGraphNode_FootPlacement* Terrain = nullptr;
    for (auto* G : Graphs) for (UEdGraphNode* N : G->Nodes)
        if (auto* T = Cast<UAnimGraphNode_FootPlacement>(N)) Terrain = T;
    if (!Terrain || Terrain->Node.LegDefinitions.Num() != 2) return false;
    UEdGraph* Graph = Terrain->GetGraph();
    auto* Input = Terrain->FindPin(TEXT("ComponentPose"));
    if (!Input || Input->LinkedTo.Num() != 1) return false;
    BP->Modify(); Graph->Modify(); Terrain->Modify();
    bool Good = true;
    // 用独立目标骨承接地形修正，保留真实腿链的骨长供 IK 求解。
    if (Terrain->Node.LegDefinitions[0].IKFootBone.BoneName == TEXT("Bip001LFoot"))
    {
        auto* Previous = Input->LinkedTo[0]; Input->BreakAllPinLinks();
        for (int32 I = 0; I < 2; ++I)
        {
            auto* Copy = Node<UAnimGraphNode_CopyBone>(Graph, 1100 + I * 180, 850);
            Copy->Node.SourceBone.BoneName = I == 0 ? TEXT("Bip001LFoot") : TEXT("Bip001RFoot");
            Copy->Node.TargetBone.BoneName = I == 0 ? TEXT("VB ik_foot_left") : TEXT("VB ik_foot_right");
            Copy->Node.bCopyTranslation = Copy->Node.bCopyRotation = Copy->Node.bCopyScale = true;
            Copy->Node.ControlSpace = BCS_ComponentSpace;
            Good &= Graph->GetSchema()->TryCreateConnection(Previous, Copy->FindPin(TEXT("ComponentPose")));
            Previous = Output(Copy);
        }
        Good &= Graph->GetSchema()->TryCreateConnection(Previous, Input);
    }
    for (int32 I = 0; I < 2; ++I)
        Terrain->Node.LegDefinitions[I].IKFootBone.BoneName = I == 0 ? TEXT("VB ik_foot_left") : TEXT("VB ik_foot_right");
    for (UEdGraphNode* N : Graph->Nodes) if (auto* IK = Cast<UAnimGraphNode_TwoBoneIK>(N))
    {
        const FName Foot = IK->Node.IKBone.BoneName;
        if (Foot != TEXT("Bip001LFoot") && Foot != TEXT("Bip001RFoot")) continue;
        IK->Modify(); const int32 Side = Foot == TEXT("Bip001LFoot") ? 0 : 1;
        PreserveKneeDirection(IK->Node, Side);
        if (IK->Node.EffectorLocationSpace == BCS_BoneSpace && IK->Node.EffectorTarget.BoneReference.BoneName == Foot)
            IK->Node.EffectorTarget.BoneReference.BoneName = Side == 0 ? TEXT("VB ik_foot_left") : TEXT("VB ik_foot_right");
    }
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP); FKismetEditorUtilities::CompileBlueprint(BP);
    return Good && BP->Status != BS_Error;
#else
    return false;
#endif
}

bool UHodgeGroundedAuthoring::RefineGroundedTransitions(UObject* Blueprint)
{
#if WITH_EDITOR
    using namespace HodgeGroundedGraph;
    UAnimBlueprint* BP = Cast<UAnimBlueprint>(Blueprint);
    if (!BP || !BP->GetPathName().StartsWith(TEXT("/Game/CodexText/")) || BP->ParentClass != UHodgeGroundedLocomotion::StaticClass()) return false;
    TArray<UEdGraph*> Graphs; BP->GetAllGraphs(Graphs);
    UAnimationStateMachineGraph* SM = nullptr;
    for (auto* G : Graphs) if (G->GetFName() == TEXT("Grounded")) SM = Cast<UAnimationStateMachineGraph>(G);
    if (!SM) return false;
    TArray<UAnimStateTransitionNode*> Stops;
    for (UEdGraphNode* N : SM->Nodes) if (auto* T = Cast<UAnimStateTransitionNode>(N))
    {
        if (T->GetPreviousState() && T->GetNextState() && T->GetPreviousState()->GetStateName() == TEXT("Move") && T->GetNextState()->GetStateName().StartsWith(TEXT("Stop"))) Stops.Add(T);
    }
    if (Stops.Num() == 8)
    {
        BP->Modify();
        for (auto* T : Stops) for (UEdGraphNode* N : T->BoundGraph->Nodes)
            if (auto* V = Cast<UK2Node_VariableGet>(N))
            {
                const FName Name = V->VariableReference.GetMemberName();
                if (Name == TEXT("bStopPlanted") || Name == TEXT("bStopAirborne"))
                { T->Modify(); T->CrossfadeDuration = Name == TEXT("bStopPlanted") ? 0.1f : 0.16f; }
            }
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP); FKismetEditorUtilities::CompileBlueprint(BP);
        return BP->Status != BS_Error;
    }
    if (Stops.Num() != 4) return false;
    BP->Modify(); SM->Modify(); bool Good = true;
    for (auto* Old : Stops)
    {
        auto* From = Old->GetPreviousState(); auto* To = Old->GetNextState();
        FName Flag;
        for (UEdGraphNode* N : Old->BoundGraph->Nodes) if (auto* V = Cast<UK2Node_VariableGet>(N)) Flag = V->VariableReference.GetMemberName();
        if (Flag.IsNone()) return false;
        Old->DestroyNode();
        for (int32 I = 0; I < 2; ++I)
        {
            auto* T = Node<UAnimStateTransitionNode>(SM, 0, 0);
            // 现有收脚素材与循环首帧不同：脚锁立即生效，身体姿态仍需短暂混合。
            T->CrossfadeDuration = I == 0 ? 0.1f : 0.16f; T->CreateConnections(From, To);
            UEdGraph* G = T->BoundGraph;
            FGraphNodeCreator<UK2Node_CallFunction> C(*G); auto* And = C.CreateNode();
            And->SetFromFunction(UKismetMathLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UKismetMathLibrary, BooleanAND))); C.Finalize();
            auto Wire = [&](UEdGraphPin* A, UEdGraphPin* B) { Good &= A && B && G->GetSchema()->TryCreateConnection(A, B); };
            for (int32 J = 0; J < 2; ++J)
            {
                FGraphNodeCreator<UK2Node_VariableGet> VC(*G); auto* V = VC.CreateNode();
                const FName Name = J == 0 ? Flag : FName(I == 0 ? TEXT("bStopPlanted") : TEXT("bStopAirborne"));
                V->VariableReference.SetSelfMember(Name); V->NodePosX = -450; V->NodePosY = J * 140; VC.Finalize();
                Wire(V->FindPin(Name), And->FindPin(J == 0 ? TEXT("A") : TEXT("B")));
            }
            for (UEdGraphNode* N : G->Nodes) if (auto* R = Cast<UAnimGraphNode_TransitionResult>(N)) Wire(And->FindPin(TEXT("ReturnValue")), R->FindPin(TEXT("bCanEnterTransition")));
        }
    }
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP); FKismetEditorUtilities::CompileBlueprint(BP);
    return Good && BP->Status != BS_Error;
#else
    return false;
#endif
}

bool UHodgeGroundedAuthoring::AddStrideLayer(UObject* Blueprint)
{
#if WITH_EDITOR
    using namespace HodgeGroundedGraph;
    auto* BP = Cast<UAnimBlueprint>(Blueprint);
    if (!BP || !BP->GetPathName().StartsWith(TEXT("/Game/CodexText/")) || BP->ParentClass != UHodgeGroundedLocomotion::StaticClass()) return false;
    TArray<UEdGraph*> Graphs; BP->GetAllGraphs(Graphs); UEdGraph* Graph = nullptr;
    for (auto* G : Graphs) if (G->GetFName() == TEXT("AnimGraph")) Graph = G;
    if (!Graph) return false;
    UAnimGraphNode_CopyBone* LastCopy = nullptr;
    for (UEdGraphNode* N : Graph->Nodes)
    {
        if (Cast<UAnimGraphNode_StrideWarping>(N)) return false;
        if (auto* C = Cast<UAnimGraphNode_CopyBone>(N)) if (C->Node.TargetBone.BoneName == TEXT("VB ik_foot_right")) LastCopy = C;
    }
    if (!LastCopy || Output(LastCopy)->LinkedTo.Num() != 1) return false;
    BP->Modify(); Graph->Modify(); bool Good = true;
    UEdGraphPin* Next = Output(LastCopy)->LinkedTo[0]; Output(LastCopy)->BreakAllPinLinks();
    auto Wire = [&](UEdGraphPin* A, UEdGraphPin* B) { Good &= A && B && Graph->GetSchema()->TryCreateConnection(A, B); };
    auto Variable = [&](UAnimGraphNode_Base* N, FName Property, FName Name)
    {
        for (auto& P : N->ShowPinForProperties) if (P.PropertyName == Property) P.bShowPin = true;
        N->ReconstructNode();
        FGraphNodeCreator<UK2Node_VariableGet> C(*Graph); auto* V = C.CreateNode(); V->VariableReference.SetSelfMember(Name); C.Finalize();
        Wire(V->FindPin(Name), N->FindPin(Property));
    };
    FGraphNodeCreator<UAnimGraphNode_StrideWarping> C(*Graph); auto* Warp = C.CreateNode();
    Warp->Node.Mode = EWarpingEvaluationMode::Manual;
    Warp->Node.PelvisBone.BoneName = TEXT("Bip001Pelvis"); Warp->Node.IKFootRootBone.BoneName = TEXT("root");
    Warp->Node.bDisableIfMissingRootMotion = false;
    for (int32 I = 0; I < 2; ++I)
    {
        FStrideWarpingFootDefinition Foot;
        Foot.IKFootBone.BoneName = I == 0 ? TEXT("VB ik_foot_left") : TEXT("VB ik_foot_right");
        Foot.FKFootBone.BoneName = I == 0 ? TEXT("Bip001LFoot") : TEXT("Bip001RFoot");
        Foot.ThighBone.BoneName = I == 0 ? TEXT("Bip001LThigh") : TEXT("Bip001RThigh");
        Warp->Node.FootDefinitions.Add(Foot);
    }
    Warp->NodePosY = 600; C.Finalize();
    Variable(Warp, TEXT("StrideScale"), TEXT("StrideScale")); Variable(Warp, TEXT("StrideDirection"), TEXT("StrideDirection")); Variable(Warp, TEXT("Alpha"), TEXT("StrideAlpha"));
    Wire(Output(LastCopy), Warp->FindPin(TEXT("ComponentPose"))); UEdGraphPin* Previous = Output(Warp);
    for (int32 I = 0; I < 2; ++I)
    {
        FGraphNodeCreator<UAnimGraphNode_TwoBoneIK> IC(*Graph); auto* IK = IC.CreateNode();
        IK->Node.IKBone.BoneName = I == 0 ? TEXT("Bip001LFoot") : TEXT("Bip001RFoot");
        IK->Node.EffectorLocationSpace = BCS_BoneSpace;
        IK->Node.EffectorTarget.BoneReference.BoneName = I == 0 ? TEXT("VB ik_foot_left") : TEXT("VB ik_foot_right");
        PreserveKneeDirection(IK->Node, I);
        IK->Node.bMaintainEffectorRelRot = true; IK->NodePosY = 600; IK->NodePosX = 350 + I * 350; IC.Finalize();
        Variable(IK, TEXT("Alpha"), TEXT("StrideAlpha")); Wire(Previous, IK->FindPin(TEXT("ComponentPose"))); Previous = Output(IK);
    }
    Wire(Previous, Next);
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP); FKismetEditorUtilities::CompileBlueprint(BP);
    return Good && BP->Status != BS_Error;
#else
    return false;
#endif
}

bool UHodgeGroundedAuthoring::AddTerrainLayer(UObject* Blueprint)
{
#if WITH_EDITOR
    using namespace HodgeGroundedGraph;
    auto* BP = Cast<UAnimBlueprint>(Blueprint);
    if (!BP || !BP->GetPathName().StartsWith(TEXT("/Game/CodexText/")) || BP->ParentClass != UHodgeGroundedLocomotion::StaticClass()) return false;
    TArray<UEdGraph*> Graphs; BP->GetAllGraphs(Graphs); UEdGraph* Graph = nullptr;
    for (auto* G : Graphs) if (G->GetFName() == TEXT("AnimGraph")) Graph = G;
    if (!Graph) return false;
    UAnimGraphNode_ComponentToLocalSpace* ToLocal = nullptr;
    for (UEdGraphNode* N : Graph->Nodes)
    {
        if (Cast<UAnimGraphNode_FootPlacement>(N)) return false;
        if (auto* L = Cast<UAnimGraphNode_ComponentToLocalSpace>(N)) ToLocal = L;
    }
    if (!ToLocal || ToLocal->FindPin(TEXT("ComponentPose"))->LinkedTo.Num() != 1) return false;
    BP->Modify(); Graph->Modify(); bool Good = true;
    auto* Input = ToLocal->FindPin(TEXT("ComponentPose")); auto* Previous = Input->LinkedTo[0]; Input->BreakAllPinLinks();
    auto Wire = [&](UEdGraphPin* A, UEdGraphPin* B) { Good &= A && B && Graph->GetSchema()->TryCreateConnection(A, B); };
    auto Alpha = [&](UAnimGraphNode_Base* N)
    {
        for (auto& P : N->ShowPinForProperties) if (P.PropertyName == TEXT("Alpha")) P.bShowPin = true;
        N->ReconstructNode(); FGraphNodeCreator<UK2Node_VariableGet> C(*Graph); auto* V = C.CreateNode();
        V->VariableReference.SetSelfMember(TEXT("TerrainAlpha")); C.Finalize(); Wire(V->FindPin(TEXT("TerrainAlpha")), N->FindPin(TEXT("Alpha")));
    };
    FGraphNodeCreator<UAnimGraphNode_FootPlacement> C(*Graph); auto* Terrain = C.CreateNode();
    Terrain->Node.PelvisBone.BoneName = TEXT("Bip001Pelvis"); Terrain->Node.IKFootRootBone.BoneName = TEXT("root");
    Terrain->Node.PlantSpeedMode = EWarpingEvaluationMode::Manual;
    Terrain->Node.PlantSettings.LockType = EFootPlacementLockType::Unlocked;
    Terrain->Node.PlantSettings.SeparatingDistance = 2.f;
    Terrain->Node.PelvisSettings.MaxOffset = 35.f; Terrain->Node.PelvisSettings.HorizontalRebalancingWeight = 0.f;
    Terrain->Node.PelvisSettings.ActorMovementCompensationMode = EActorMovementCompensationMode::ComponentSpace;
    Terrain->Node.TraceSettings.StartOffset = -50.f; Terrain->Node.TraceSettings.EndOffset = 60.f;
    for (int32 I = 0; I < 2; ++I)
    {
        FFootPlacemenLegDefinition Leg;
        Leg.FKFootBone.BoneName = I == 0 ? TEXT("Bip001LFoot") : TEXT("Bip001RFoot");
        Leg.IKFootBone = Leg.FKFootBone;
        Leg.BallBone.BoneName = I == 0 ? TEXT("Bip001LToe0") : TEXT("Bip001RToe0");
        Leg.SpeedCurveName = I == 0 ? TEXT("FootSpeed_L") : TEXT("FootSpeed_R");
        Terrain->Node.LegDefinitions.Add(Leg);
    }
    Terrain->NodePosX = 1500; Terrain->NodePosY = 650; C.Finalize(); Alpha(Terrain);
    Wire(Previous, Terrain->FindPin(TEXT("ComponentPose"))); Previous = Output(Terrain);
    for (int32 I = 0; I < 2; ++I)
    {
        FGraphNodeCreator<UAnimGraphNode_TwoBoneIK> IC(*Graph); auto* IK = IC.CreateNode();
        IK->Node.IKBone.BoneName = I == 0 ? TEXT("Bip001LFoot") : TEXT("Bip001RFoot");
        IK->Node.EffectorLocationSpace = BCS_BoneSpace; IK->Node.EffectorTarget.BoneReference = IK->Node.IKBone;
        IK->Node.bTakeRotationFromEffectorSpace = true;
        PreserveKneeDirection(IK->Node, I);
        IK->NodePosX = 1900 + I * 350; IK->NodePosY = 650; IC.Finalize(); Alpha(IK);
        Wire(Previous, IK->FindPin(TEXT("ComponentPose"))); Previous = Output(IK);
    }
    Wire(Previous, Input);
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP); FKismetEditorUtilities::CompileBlueprint(BP);
    return Good && BP->Status != BS_Error && RepairGroundedLegIK(BP);
#else
    return false;
#endif
}

bool UHodgeGroundedAuthoring::AddCombatLayer(UObject* Blueprint, UObject* Overlay)
{
#if WITH_EDITOR
    using namespace HodgeGroundedGraph;
    auto* BP = Cast<UAnimBlueprint>(Blueprint); auto* Sequence = Cast<UAnimSequence>(Overlay);
    if (!BP || !Sequence || !BP->GetPathName().StartsWith(TEXT("/Game/CodexText/")) || !Sequence->GetPathName().StartsWith(TEXT("/Game/CodexText/"))
        || BP->ParentClass != UHodgeGroundedLocomotion::StaticClass() || BP->TargetSkeleton != Sequence->GetSkeleton()) return false;
    TArray<UEdGraph*> Graphs; BP->GetAllGraphs(Graphs); UEdGraph* Graph = nullptr;
    for (auto* G : Graphs) if (G->GetFName() == TEXT("AnimGraph")) Graph = G;
    if (!Graph) return false;
    UAnimGraphNode_Slot* Slot = nullptr; UAnimGraphNode_LocalToComponentSpace* ToCS = nullptr;
    for (UEdGraphNode* N : Graph->Nodes)
    {
        if (Cast<UAnimGraphNode_LayeredBoneBlend>(N)) return false;
        if (auto* S = Cast<UAnimGraphNode_Slot>(N)) Slot = S;
        if (auto* CS = Cast<UAnimGraphNode_LocalToComponentSpace>(N)) ToCS = CS;
    }
    if (!Slot || !ToCS || Slot->FindPin(TEXT("Source"))->LinkedTo.Num() != 1 || Output(ToCS)->LinkedTo.Num() != 1) return false;
    BP->Modify(); Graph->Modify(); bool Good = true;
    auto Wire = [&](UEdGraphPin* A, UEdGraphPin* B) { Good &= A && B && Graph->GetSchema()->TryCreateConnection(A, B); };
    auto Variable = [&](UAnimGraphNode_Base* N, FName Property, FName Name)
    {
        for (auto& P : N->ShowPinForProperties) if (P.PropertyName == Property) P.bShowPin = true;
        N->ReconstructNode(); FGraphNodeCreator<UK2Node_VariableGet> C(*Graph); auto* V = C.CreateNode();
        V->VariableReference.SetSelfMember(Name); C.Finalize(); Wire(V->FindPin(Name), N->FindPin(Property));
    };
    auto* Base = Slot->FindPin(TEXT("Source"))->LinkedTo[0]; Slot->FindPin(TEXT("Source"))->BreakAllPinLinks();
    FGraphNodeCreator<UAnimGraphNode_LayeredBoneBlend> LC(*Graph); auto* Layer = LC.CreateNode();
    Layer->Node.LayerSetup[0].BranchFilters.Add(FBranchFilter(TEXT("Bip001Spine"), 3));
    Layer->Node.bMeshSpaceRotationBlend = true; Layer->Node.CurveBlendOption = ECurveBlendOption::UseBasePose;
    Layer->NodePosX = -1500; Layer->NodePosY = -600; LC.Finalize();
    FGraphNodeCreator<UAnimGraphNode_SequencePlayer> PC(*Graph); auto* Pose = PC.CreateNode(); Pose->Node.SetSequence(Sequence); Pose->NodePosX = -1900; Pose->NodePosY = -500; PC.Finalize();
    Variable(Pose, TEXT("Sequence"), TEXT("OverlayPose"));
    FGraphNodeCreator<UK2Node_VariableGet> VC(*Graph); auto* Weight = VC.CreateNode(); Weight->VariableReference.SetSelfMember(TEXT("OverlayAlpha")); VC.Finalize();
    Wire(Weight->FindPin(TEXT("OverlayAlpha")), Layer->FindPin(TEXT("BlendWeights_0")));
    Wire(Base, Layer->FindPin(TEXT("BasePose"))); Wire(Output(Pose), Layer->FindPin(TEXT("BlendPoses_0"))); Wire(Output(Layer), Slot->FindPin(TEXT("Source")));
    auto* Next = Output(ToCS)->LinkedTo[0]; Output(ToCS)->BreakAllPinLinks();
    FGraphNodeCreator<UAnimGraphNode_TwoBoneIK> IC(*Graph); auto* IK = IC.CreateNode();
    IK->Node.IKBone.BoneName = TEXT("Bip001LHand"); IK->Node.EffectorLocationSpace = BCS_BoneSpace; IK->Node.EffectorTarget.BoneReference.BoneName = TEXT("Bip001RHand");
    IK->Node.bTakeRotationFromEffectorSpace = true; IK->Node.JointTargetLocationSpace = BCS_BoneSpace; IK->Node.JointTarget.BoneReference.BoneName = TEXT("Bip001LForearm");
    IK->Node.JointTargetLocation = FVector::ZeroVector; IK->NodePosX = -1000; IK->NodePosY = -650; IC.Finalize();
    Variable(IK, TEXT("Alpha"), TEXT("HandIKAlpha")); Variable(IK, TEXT("EffectorLocation"), TEXT("HandGripLocation"));
    Wire(Output(ToCS), IK->FindPin(TEXT("ComponentPose")));
    FGraphNodeCreator<UAnimGraphNode_ModifyBone> RC(*Graph); auto* Rotation = RC.CreateNode(); Rotation->Node.BoneToModify.BoneName = TEXT("Bip001LHand");
    Rotation->Node.RotationMode = BMM_Additive; Rotation->Node.RotationSpace = BCS_BoneSpace; Rotation->NodePosX = -650; Rotation->NodePosY = -650; RC.Finalize();
    Variable(Rotation, TEXT("Alpha"), TEXT("HandIKAlpha")); Variable(Rotation, TEXT("Rotation"), TEXT("HandGripRotation"));
    Wire(Output(IK), Rotation->FindPin(TEXT("ComponentPose"))); Wire(Output(Rotation), Next);
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP); FKismetEditorUtilities::CompileBlueprint(BP);
    if (BP->Status == BS_Error || !BP->GeneratedClass) return false;
    if (auto* CDO = Cast<UHodgeGroundedLocomotion>(BP->GeneratedClass->GetDefaultObject())) CDO->OverlayPose = Sequence;
    return Good && BP->Status != BS_Error;
#else
    return false;
#endif
}

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
        IK->Node.EffectorLocationSpace=BCS_ComponentSpace;
        PreserveKneeDirection(IK->Node, I);
        IK->Node.bAllowStretching=true;IK->Node.StartStretchRatio=1.f;IK->Node.MaxStretchScale=1.03f;
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
