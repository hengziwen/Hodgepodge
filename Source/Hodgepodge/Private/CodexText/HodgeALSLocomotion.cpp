#include "CodexText/HodgeALSLocomotion.h"
#include "CodexText/HodgeLocomotionLab.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Curves/CurveFloat.h"

#if WITH_EDITOR
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimSequence.h"
#include "AnimGraphNode_Root.h"
#include "AnimGraphNode_StateMachine.h"
#include "AnimGraphNode_SequencePlayer.h"
#include "AnimGraphNode_TwoWayBlend.h"
#include "AnimGraphNode_MultiWayBlend.h"
#include "AnimGraphNode_Slot.h"
#include "AnimGraphNode_TransitionResult.h"
#include "AnimStateNode.h"
#include "AnimStateEntryNode.h"
#include "AnimStateTransitionNode.h"
#include "AnimationStateMachineGraph.h"
#include "K2Node_VariableGet.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeALSLocomotion)

FName UHodgeALSLocomotion::GetLabActiveState()
{
    const int32 Index = GetStateMachineIndex(TEXT("ALS_Locomotion"));
    return Index != INDEX_NONE ? GetCurrentStateName(Index) : NAME_None;
}

void UHodgeALSLocomotion::NativeUpdateAnimation(float Dt)
{
    Super::NativeUpdateAnimation(Dt);
    const ACharacter* Character = Cast<ACharacter>(TryGetPawnOwner());
    if (!Character || Dt <= 0.f) return;
    const auto* Lab = Character->FindComponentByClass<UHodgeLocomotionLabComponent>();
    const FVector Velocity = Character->GetVelocity();
    LabSpeed = Velocity.Size2D();
    bLabFall = Character->GetCharacterMovement()->IsFalling();
    bLabIdle = !bLabFall && LabSpeed < 5.f;
    const bool Moving = !bLabIdle && !bLabFall;
    LabViewAngle = FRotator::NormalizeAxis(Velocity.Rotation().Yaw - Character->GetBaseAimRotation().Yaw);
    LabFeetCrossing = GetCurveValue(TEXT("FeetCrossing"));
    DirectionHoldRemaining = FMath::Max(0.f, DirectionHoldRemaining - Dt);
    auto IsLeft = [](EHodgeALSDirection D) { return D == EHodgeALSDirection::LeftForward || D == EHodgeALSDirection::LeftBackward; };
    auto IsRight = [](EHodgeALSDirection D) { return D == EHodgeALSDirection::RightForward || D == EHodgeALSDirection::RightBackward; };
    if (Moving)
    {
        const EHodgeALSDirection Previous = LabDirection;
        const float AbsAngle = FMath::Abs(LabViewAngle);
        // Hysteresis in view space: actor yaw correction must not change the selected direction.
        const float ForwardLimit = Previous == EHodgeALSDirection::Forward ? 75.f : 65.f;
        const float BackwardLimit = Previous == EHodgeALSDirection::Backward ? 105.f : 115.f;
        if ((Lab && !Lab->bCombatFacing) || AbsAngle < ForwardLimit) LabDirection = EHodgeALSDirection::Forward;
        else if (AbsAngle > BackwardLimit) LabDirection = EHodgeALSDirection::Backward;
        else if (LabViewAngle < 0.f)
        {
            if (IsRight(Previous))
            {
                LabDirection = Previous == EHodgeALSDirection::RightForward ? EHodgeALSDirection::LeftBackward : EHodgeALSDirection::LeftForward;
                DirectionHoldRemaining = 0.2f;
            }
            else if (!IsLeft(Previous)) LabDirection = HipOrientationBias > 0.5f ? EHodgeALSDirection::LeftBackward : EHodgeALSDirection::LeftForward;
            else if (DirectionHoldRemaining <= 0.f && LabFeetCrossing < 0.2f)
                LabDirection = HipOrientationBias > 0.5f ? EHodgeALSDirection::LeftBackward : EHodgeALSDirection::LeftForward;
        }
        else
        {
            if (IsLeft(Previous))
            {
                LabDirection = Previous == EHodgeALSDirection::LeftForward ? EHodgeALSDirection::RightBackward : EHodgeALSDirection::RightForward;
                DirectionHoldRemaining = 0.2f;
            }
            else if (!IsRight(Previous)) LabDirection = HipOrientationBias < -0.5f ? EHodgeALSDirection::RightBackward : EHodgeALSDirection::RightForward;
            else if (DirectionHoldRemaining <= 0.f && LabFeetCrossing < 0.2f)
                LabDirection = HipOrientationBias < -0.5f ? EHodgeALSDirection::RightBackward : EHodgeALSDirection::RightForward;
        }
    }
    const UCurveFloat* Curve = IsLeft(LabDirection) ? LeftYawCurve : IsRight(LabDirection) ? RightYawCurve : ForwardBackwardYawCurve;
    const float TargetOffset = Moving && (!Lab || Lab->bCombatFacing) && Curve ? Curve->GetFloatValue(LabViewAngle) : 0.f;
    const float Alpha = 1.f - FMath::Exp(-12.f * Dt);
    LabYawOffset = FMath::Lerp(LabYawOffset, TargetOffset, Alpha);
    if (Moving)
    {
        const FVector Local = Character->GetActorRotation().UnrotateVector(Velocity);
        const float Sum = FMath::Max(1.f, FMath::Abs(Local.X) + FMath::Abs(Local.Y));
        LabForward = FMath::Lerp(LabForward, FMath::Max(0.f, float(Local.X)) / Sum, Alpha);
        LabBackward = FMath::Lerp(LabBackward, FMath::Max(0.f, float(-Local.X)) / Sum, Alpha);
        LabLeft = FMath::Lerp(LabLeft, FMath::Max(0.f, float(-Local.Y)) / Sum, Alpha);
        LabRight = FMath::Lerp(LabRight, FMath::Max(0.f, float(Local.Y)) / Sum, Alpha);
    }
    const float WalkSpeed = Lab ? Lab->WalkSpeed : 180.f;
    const float RunSpeed = Lab ? Lab->RunSpeed : 420.f;
    LabGait = FMath::Clamp((LabSpeed - WalkSpeed) / FMath::Max(1.f, RunSpeed - WalkSpeed), 0.f, 1.f);
    LabPlayRate = FMath::Clamp(LabSpeed / FMath::Max(1.f, FMath::Lerp(WalkSpeed, RunSpeed, LabGait)), 0.15f, 1.4f);
    bLabF = Moving && LabDirection == EHodgeALSDirection::Forward;
    bLabB = Moving && LabDirection == EHodgeALSDirection::Backward;
    bLabLF = Moving && LabDirection == EHodgeALSDirection::LeftForward;
    bLabLB = Moving && LabDirection == EHodgeALSDirection::LeftBackward;
    bLabRF = Moving && LabDirection == EHodgeALSDirection::RightForward;
    bLabRB = Moving && LabDirection == EHodgeALSDirection::RightBackward;
}

bool UHodgeALSAuthoring::BuildLocomotionGraph(UObject* Blueprint, const TArray<UObject*>& Animations, UObject* Idle, UObject* Fall)
{
#if WITH_EDITOR
    UAnimBlueprint* BP = Cast<UAnimBlueprint>(Blueprint);
    if (!BP || !BP->GetPathName().StartsWith(TEXT("/Game/CodexText/")) || !BP->ParentClass->IsChildOf(UHodgeALSLocomotion::StaticClass()) || Animations.Num() != 12) return false;
    for (UObject* A : Animations) if (!Cast<UAnimSequence>(A) || !A->GetPathName().StartsWith(TEXT("/Game/CodexText/"))) return false;
    if (!Cast<UAnimSequence>(Idle) || !Cast<UAnimSequence>(Fall)) return false;
    TArray<UEdGraph*> Graphs; BP->GetAllGraphs(Graphs);
    UEdGraph* Graph = nullptr;
    for (auto* G : Graphs) if (G->GetFName() == TEXT("AnimGraph")) { Graph = G; break; }
    if (!Graph || Graph->Nodes.Num() != 1) return false;
    auto* Root = Cast<UAnimGraphNode_Root>(Graph->Nodes[0]);
    if (!Root) return false;
    BP->Modify(); Graph->Modify();
    bool Good = true;
    auto Wire = [&](UEdGraph* G, UEdGraphPin* A, UEdGraphPin* B)
    {
        if (!A || !B || !G->GetSchema()->TryCreateConnection(A, B)) { Good = false; UE_LOG(LogTemp, Error, TEXT("ALS graph connection failed in %s"), *G->GetName()); }
    };
    auto Variable = [&](UEdGraph* G, FName Name, UEdGraphPin* Pin, int32 X, int32 Y)
    {
        FGraphNodeCreator<UK2Node_VariableGet> C(*G); auto* N = C.CreateNode();
        N->VariableReference.SetSelfMember(Name); N->NodePosX = X; N->NodePosY = Y; C.Finalize();
        Wire(G, N->FindPin(Name), Pin);
    };
    auto Sequence = [&](UEdGraph* G, UObject* A, int32 X, int32 Y, bool Cycle)
    {
        FGraphNodeCreator<UAnimGraphNode_SequencePlayer> C(*G); auto* N = C.CreateNode();
        N->Node.SetSequence(Cast<UAnimSequence>(A));
        if (Cycle) { N->Node.SetGroupName(TEXT("ALS_Locomotion")); N->Node.SetGroupMethod(EAnimSyncMethod::SyncGroup); }
        N->NodePosX = X; N->NodePosY = Y; C.Finalize();
        if (Cycle)
        {
            for (auto& P : N->ShowPinForProperties) if (P.PropertyName == TEXT("PlayRate")) P.bShowPin = true;
            N->ReconstructNode(); Variable(G, TEXT("LabPlayRate"), N->FindPin(TEXT("PlayRate")), X - 230, Y + 60);
        }
        return N;
    };
    FGraphNodeCreator<UAnimGraphNode_StateMachine> MachineCreator(*Graph);
    auto* Machine = MachineCreator.CreateNode(); Machine->NodePosX = -700; MachineCreator.Finalize();
    UAnimationStateMachineGraph* SM = Machine->EditorStateMachineGraph;
    FBlueprintEditorUtils::RenameGraph(SM, TEXT("ALS_Locomotion"));
    const TCHAR* Names[] = {TEXT("Idle"),TEXT("Forward"),TEXT("Backward"),TEXT("LeftForward"),TEXT("LeftBackward"),TEXT("RightForward"),TEXT("RightBackward"),TEXT("Fall")};
    const TCHAR* Flags[] = {TEXT("bLabIdle"),TEXT("bLabF"),TEXT("bLabB"),TEXT("bLabLF"),TEXT("bLabLB"),TEXT("bLabRF"),TEXT("bLabRB"),TEXT("bLabFall")};
    const TCHAR* Weights[] = {TEXT("LabForward"),TEXT("LabBackward"),TEXT("LabLeft"),TEXT("LabRight")};
    TArray<UAnimStateNode*> States;
    for (int32 I = 0; I < 8; ++I)
    {
        FGraphNodeCreator<UAnimStateNode> C(*SM); auto* State = C.CreateNode();
        State->NodePosX = (I % 4) * 380; State->NodePosY = (I / 4) * 350;
        State->bAlwaysResetOnEntry = false; C.Finalize();
        FBlueprintEditorUtils::RenameGraph(State->BoundGraph, Names[I]); States.Add(State);
        UEdGraph* G = State->BoundGraph;
        if (I == 0 || I == 7) Wire(G, Sequence(G, I == 0 ? Idle : Fall, -400, 0, false)->FindPin(TEXT("Pose")), State->GetPoseSinkPinInsideState());
        else
        {
            FGraphNodeCreator<UAnimGraphNode_MultiWayBlend> BC(*G); auto* Blend = BC.CreateNode();
            Blend->NodePosX = -350; BC.Finalize();
            while (Blend->Node.Poses.Num() < 4) Blend->AddPinToBlendNode();
            // L/R pose variants preserve hip orientation during reversals; they are not diagonal samples.
            const int32 Left = (I == 2 || I == 4 || I == 5) ? 3 : 2;
            const int32 Right = (I == 2 || I == 3 || I == 6) ? 5 : 4;
            const int32 PoseIndices[] = {0, 1, Left, Right};
            for (int32 P = 0; P < 4; ++P)
            {
                FGraphNodeCreator<UAnimGraphNode_TwoWayBlend> GC(*G); auto* Gait = GC.CreateNode();
                Gait->NodePosX = -800; Gait->NodePosY = P * 350; GC.Finalize();
                Wire(G, Sequence(G, Animations[PoseIndices[P]], -1350, P * 350, true)->FindPin(TEXT("Pose")), Gait->FindPin(TEXT("A")));
                Wire(G, Sequence(G, Animations[6 + PoseIndices[P]], -1350, P * 350 + 140, true)->FindPin(TEXT("Pose")), Gait->FindPin(TEXT("B")));
                Variable(G, TEXT("LabGait"), Gait->FindPin(TEXT("Alpha")), -1070, P * 350 + 250);
                Wire(G, Gait->FindPin(TEXT("Pose")), Blend->FindPin(FName(*FString::Printf(TEXT("Poses_%d"), P))));
                Variable(G, Weights[P], Blend->FindPin(FName(*FString::Printf(TEXT("DesiredAlphas_%d"), P))), -600, 1400 + P * 90);
            }
            Wire(G, Blend->FindPin(TEXT("Pose")), State->GetPoseSinkPinInsideState());
        }
    }
    Wire(SM, SM->EntryNode->Pins[0], States[0]->GetInputPin());
    for (int32 From = 0; From < 8; ++From) for (int32 To = 0; To < 8; ++To)
    {
        if (From == To) continue;
        FGraphNodeCreator<UAnimStateTransitionNode> TC(*SM); auto* T = TC.CreateNode(); TC.Finalize();
        T->CrossfadeDuration = To == 7 ? 0.12f : 0.18f; T->CreateConnections(States[From], States[To]);
        for (UEdGraphNode* N : T->BoundGraph->Nodes) if (auto* Result = Cast<UAnimGraphNode_TransitionResult>(N))
        { Variable(T->BoundGraph, Flags[To], Result->FindPin(TEXT("bCanEnterTransition")), -250, 0); break; }
    }
    FGraphNodeCreator<UAnimGraphNode_Slot> SC(*Graph); auto* Slot = SC.CreateNode();
    Slot->Node.SlotName = TEXT("DefaultSlot"); Slot->NodePosX = -350; SC.Finalize();
    Wire(Graph, Machine->FindPin(TEXT("Pose")), Slot->FindPin(TEXT("Source")));
    Wire(Graph, Slot->FindPin(TEXT("Pose")), Root->FindPin(TEXT("Result")));
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
    FKismetEditorUtilities::CompileBlueprint(BP);
    return Good && BP->Status != BS_Error;
#else
    return false;
#endif
}
