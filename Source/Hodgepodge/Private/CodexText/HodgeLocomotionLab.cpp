#include "CodexText/HodgeLocomotionLab.h"
#include "CodexText/HodgeALSLocomotion.h"
#include "CodexText/HodgeGroundedLocomotion.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/AssetManager.h"
#include "Data/HodgeExperienceDefinition.h"
#include "Engine/Engine.h"
#include "InputCoreTypes.h"

#if WITH_EDITOR
#include "Animation/AnimBlueprint.h"
#include "Animation/BlendSpace.h"
#include "AnimGraphNode_BlendSpacePlayer.h"
#include "AnimGraphNode_Root.h"
#include "K2Node_VariableGet.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Serialization/ArchiveReplaceObjectRef.h"
#include "UObject/UObjectHash.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeLocomotionLab)

UHodgeLocomotionLabComponent::UHodgeLocomotionLabComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;
}

void UHodgeLocomotionLabComponent::BeginPlay()
{
    Super::BeginPlay();
    SetCombatFacing(bCombatFacing);
    SetWalking(bWalking);
    if (auto* Character = Cast<ACharacter>(GetOwner()))
    {
        Character->GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = false;
        // Apply facing after movement, before converting world foot anchors into mesh space.
        AddTickPrerequisiteComponent(Character->GetCharacterMovement());
        Character->GetMesh()->AddTickPrerequisiteComponent(this);
    }
}

void UHodgeLocomotionLabComponent::SetCombatFacing(bool bEnabled)
{
    bCombatFacing = bEnabled;
    if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
    {
        Character->bUseControllerRotationYaw = false;
        UCharacterMovementComponent* Movement = Character->GetCharacterMovement();
        Movement->bOrientRotationToMovement = !bEnabled;
        Movement->bUseControllerDesiredRotation = false;
        Movement->RotationRate = FRotator(0.f, 540.f, 0.f);
    }
}

void UHodgeLocomotionLabComponent::SetWalking(bool bEnabled)
{
    bWalking = bEnabled;
    if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
    {
        Character->GetCharacterMovement()->MaxWalkSpeed = bEnabled ? WalkSpeed : RunSpeed;
    }
}

void UHodgeLocomotionLabComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    APlayerController* PC = Character ? Cast<APlayerController>(Character->GetController()) : nullptr;
    if (!PC || !PC->IsLocalController()) return;
    if (bKeyboardControlsEnabled)
    {
        if (PC->WasInputKeyJustPressed(EKeys::V)) SetCombatFacing(!bCombatFacing);
        if (PC->WasInputKeyJustPressed(EKeys::O))
            if (auto* Grounded = Cast<UHodgeGroundedLocomotion>(Character->GetMesh()->GetAnimInstance()))
                Grounded->SetOverlayEnabled(Grounded->OverlayAlpha < 0.5f);
        const bool bShift = PC->IsInputKeyDown(EKeys::LeftShift) || PC->IsInputKeyDown(EKeys::RightShift);
        if (bShift != bWalking) SetWalking(bShift);
    }
    if (bCombatFacing && Character->GetCharacterMovement()->IsMovingOnGround()
        && !Character->GetCharacterMovement()->GetDeltaRotation(DeltaTime).IsNearlyZero())
    {
        const auto* Anim = Cast<UHodgeALSLocomotion>(Character->GetMesh()->GetAnimInstance());
        const auto* Grounded = Cast<UHodgeGroundedLocomotion>(Anim);
        if (Grounded && Character->GetVelocity().Size2D() < 10.f
            && Character->GetCharacterMovement()->GetCurrentAcceleration().IsNearlyZero())
        {
            if (Grounded->IsTurningInPlace())
                Character->SetActorRotation(FRotator(0.f, Grounded->GetGroundedRotationTarget(), 0.f));
        }
        else
        {
            const float Offset = Anim ? Anim->LabYawOffset : 0.f;
            const FRotator Target(0.f, PC->GetControlRotation().Yaw + Offset, 0.f);
            Character->SetActorRotation(FMath::RInterpTo(Character->GetActorRotation(), Target, DeltaTime, 12.f));
        }
    }
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(0xC0DE, 0.f, FColor::Cyan,
            FString::Printf(TEXT("LOCOMOTION LAB | WASD: move | Mouse: look | Shift: walk | V: facing\n%s | %s | Speed %.0f"),
                bCombatFacing ? TEXT("Combat / camera facing") : TEXT("Free / movement facing"),
                bWalking ? TEXT("Walk") : TEXT("Run"), Character->GetVelocity().Size2D()));
    }
}

void AHodgeLocomotionLabMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    UAssetManager::Get().ScanPathsForPrimaryAssets(FPrimaryAssetType(TEXT("HodgeExperienceDefinition")),
        {TEXT("/Game/CodexText/Locomotion")}, UHodgeExperienceDefinition::StaticClass(), true, false, true);
    Super::InitGame(MapName, Options + TEXT("?Experience=Exp_LocomotionLab"), ErrorMessage);
}

bool UHodgeLocomotionLabAuthoring::RemapCopy(UObject* Asset, const TArray<UObject*>& Sources, const TArray<UObject*>& Copies)
{
#if WITH_EDITOR
    if (!Asset || !Asset->GetPathName().StartsWith(TEXT("/Game/CodexText/")) || Sources.Num() != Copies.Num()) return false;
    TMap<UObject*, UObject*> Map;
    for (int32 Index = 0; Index < Sources.Num(); ++Index)
    {
        if (!Sources[Index] || !Copies[Index] || !Copies[Index]->GetPathName().StartsWith(TEXT("/Game/CodexText/"))) return false;
        Map.Add(Sources[Index], Copies[Index]);
        UBlueprint* From = Cast<UBlueprint>(Sources[Index]);
        UBlueprint* To = Cast<UBlueprint>(Copies[Index]);
        if (From && To)
        {
            if (From->GeneratedClass && To->GeneratedClass) Map.Add(From->GeneratedClass, To->GeneratedClass);
            if (From->SkeletonGeneratedClass && To->SkeletonGeneratedClass) Map.Add(From->SkeletonGeneratedClass, To->SkeletonGeneratedClass);
        }
    }
    TArray<UObject*> Objects;
    GetObjectsWithOuter(Asset->GetOutermost(), Objects, true);
    for (UObject* Object : Objects)
    {
        Object->Modify();
        FArchiveReplaceObjectRef<UObject> Replace(Object, Map, EArchiveReplaceObjectFlags::IgnoreOuterRef | EArchiveReplaceObjectFlags::IgnoreArchetypeRef);
    }
    Asset->MarkPackageDirty();
    if (UBlueprint* BP = Cast<UBlueprint>(Asset))
    {
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
        FKismetEditorUtilities::CompileBlueprint(BP);
        return BP->Status != BS_Error;
    }
    return true;
#else
    return false;
#endif
}

bool UHodgeLocomotionLabAuthoring::ConfigureGroundBlend(UObject* Blueprint, UObject* BlendSpace)
{
#if WITH_EDITOR
    UAnimBlueprint* BP = Cast<UAnimBlueprint>(Blueprint);
    UBlendSpace* BS = Cast<UBlendSpace>(BlendSpace);
    if (!BP || !BS || !BP->GetPathName().StartsWith(TEXT("/Game/CodexText/")) || !BS->GetPathName().StartsWith(TEXT("/Game/CodexText/"))) return false;
    TArray<UEdGraph*> Graphs;
    BP->GetAllGraphs(Graphs);
    UEdGraph* Graph = nullptr;
    for (UEdGraph* Candidate : Graphs) if (Candidate->GetFName() == TEXT("AnimGraph")) { Graph = Candidate; break; }
    if (!Graph) return false;
    UAnimGraphNode_Root* Root = nullptr;
    for (UEdGraphNode* Node : Graph->Nodes) if (auto* Candidate = Cast<UAnimGraphNode_Root>(Node)) { Root = Candidate; break; }
    if (!Root) return false;
    BP->Modify(); Graph->Modify();
    FGraphNodeCreator<UAnimGraphNode_BlendSpacePlayer> Creator(*Graph);
    auto* Player = Creator.CreateNode();
    Player->Node.SetBlendSpace(BS);
    Player->NodePosX = Root->NodePosX - 400;
    Player->NodePosY = Root->NodePosY + 400;
    Creator.Finalize();
    auto AddVariable = [&](FName Name, FName Pin, int32 Y)
    {
        FGraphNodeCreator<UK2Node_VariableGet> VarCreator(*Graph);
        auto* Variable = VarCreator.CreateNode();
        Variable->VariableReference.SetSelfMember(Name);
        Variable->NodePosX = Player->NodePosX - 250; Variable->NodePosY = Player->NodePosY + Y;
        VarCreator.Finalize();
        return Graph->GetSchema()->TryCreateConnection(Variable->FindPin(Name), Player->FindPin(Pin));
    };
    if (!AddVariable(TEXT("LocalVelocityDirectionAngle"), TEXT("X"), 0) || !AddVariable(TEXT("Speed"), TEXT("Y"), 100)) return false;
    UEdGraphPin* Result = Root->FindPin(TEXT("Result"));
    if (!Result) return false;
    Result->BreakAllPinLinks();
    if (!Graph->GetSchema()->TryCreateConnection(Player->FindPin(TEXT("Pose")), Result)) return false;
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
    FKismetEditorUtilities::CompileBlueprint(BP);
    return BP->Status != BS_Error;
#else
    return false;
#endif
}
