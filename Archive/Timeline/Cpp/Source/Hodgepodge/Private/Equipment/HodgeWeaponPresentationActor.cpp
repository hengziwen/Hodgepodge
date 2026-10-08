#include "Equipment/HodgeWeaponPresentationActor.h"
#include "Equipment/HodgeWeaponInstance.h"
#include "Equipment/HodgeWeaponPresentationProfile.h"
#include "Equipment/HodgeEquipmentManagerComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Curves/CurveFloat.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeWeaponPresentationActor)

AHodgeWeaponPresentationActor::AHodgeWeaponPresentationActor()
{
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
	SetReplicateMovement(false);
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);
	SkeletalMesh->SetVisibility(false);
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SkeletalMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	WeaponVisualMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponVisualMesh"));
	WeaponVisualMesh->SetupAttachment(RootComponent);
	WeaponVisualMesh->SetVisibility(false);
	WeaponVisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AddTickPrerequisiteComponent(SkeletalMesh);
}

void AHodgeWeaponPresentationActor::BeginPlay()
{
	Super::BeginPlay();
	SkeletalMesh->SetComponentTickEnabled(false);
	TryBindWeapon();
	if (!Weapon.IsValid()) { GetWorldTimerManager().SetTimer(BindRetryTimer, this, &ThisClass::TryBindWeapon, .05f, true); }
}

void AHodgeWeaponPresentationActor::TryBindWeapon()
{
	if (const auto* Manager = GetOwner() ? GetOwner()->FindComponentByClass<UHodgeEquipmentManagerComponent>() : nullptr)
	{
		for (auto* Instance : Manager->GetEquipmentInstancesOfType(UHodgeWeaponInstance::StaticClass()))
		{
			if (Instance->GetSpawnedActors().Contains(this)) { BindWeapon(CastChecked<UHodgeWeaponInstance>(Instance)); break; }
		}
	}
	if (Weapon.IsValid() || ++BindAttempts >= 100) { GetWorldTimerManager().ClearTimer(BindRetryTimer); }
}

void AHodgeWeaponPresentationActor::ConfigureProfile(const UHodgeWeaponPresentationProfile* Profile)
{
	if (!Profile || Profile == AppliedProfile) { return; }
	AppliedProfile = Profile;
	SkeletalMesh->SetSkeletalMeshAsset(Profile->WeaponMesh);
	SkeletalMesh->SetRelativeTransform(Profile->HandOffset);
	WeaponVisualMesh->SetSkeletalMeshAsset(Profile->WeaponMesh);
	WeaponVisualMesh->SetLeaderPoseComponent(SkeletalMesh);
	DynamicMaterials.Reset();
	if (GetNetMode() == NM_DedicatedServer) { return; }
	for (int32 Index = 0; Index < WeaponVisualMesh->GetNumMaterials(); ++Index)
	{
		if (Profile->Materials.IsValidIndex(Index) && Profile->Materials[Index]) { WeaponVisualMesh->SetMaterial(Index, Profile->Materials[Index]); }
		if (auto* Material = WeaponVisualMesh->CreateDynamicMaterialInstance(Index)) { DynamicMaterials.Add(Material); }
	}
}

void AHodgeWeaponPresentationActor::BindWeapon(UHodgeWeaponInstance* Instance)
{
	Weapon = Instance;
	GetWorldTimerManager().ClearTimer(BindRetryTimer);
	ConfigureProfile(Instance ? Instance->GetPresentationProfile() : nullptr);
	if (Instance && AppliedProfile)
	{
		if (auto* Character = Cast<ACharacter>(Instance->GetPawn()))
		{
			if (auto* PreviousMesh = Cast<USkeletalMeshComponent>(RootComponent->GetAttachParent())) { RemoveTickPrerequisiteComponent(PreviousMesh); }
			AddTickPrerequisiteComponent(Character->GetMesh());
			if (RootComponent->GetAttachParent() != Character->GetMesh() || RootComponent->GetAttachSocketName() != AppliedProfile->HandSocket)
			{
				AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, AppliedProfile->HandSocket);
			}
			if (Character->GetMesh()->GetSkeletalMeshAsset() && !Character->GetMesh()->DoesSocketExist(AppliedProfile->BackSocket))
			{
				UE_LOG(LogTemp, Warning, TEXT("Weapon %s: character Mesh has no back socket %s; using the hand pose"), *GetName(), *AppliedProfile->BackSocket.ToString());
			}
		}
	}
	RefreshPresentation();
}

void AHodgeWeaponPresentationActor::RefreshPresentation()
{
	if (!Weapon.IsValid() || !AppliedProfile) { WeaponVisualMesh->SetVisibility(false); SetActorTickEnabled(false); return; }
	const bool bHand = Weapon->GetPresentationState().Phase == EHodgeWeaponPresentationPhase::Hand;
	SkeletalMesh->SetComponentTickEnabled(bHand);
	EvaluatePresentation();
}

FTransform AHodgeWeaponPresentationActor::GetVisualTransform() const
{
	return GetNetMode() == NM_DedicatedServer ? SkeletalMesh->GetComponentTransform() : WeaponVisualMesh->GetComponentTransform();
}

void AHodgeWeaponPresentationActor::EvaluatePresentation()
{
	if (!Weapon.IsValid() || !AppliedProfile || !Weapon->GetPawn()) { return; }
	const auto& P = *AppliedProfile;
	const auto State = Weapon->GetPresentationState();
	const FTransform PawnTransform = Weapon->GetPawn()->GetRootComponent()->GetComponentTransform();
	auto* Character = Cast<ACharacter>(Weapon->GetPawn());
	auto* CharacterMesh = Character ? Character->GetMesh() : nullptr;
	const bool bHasBackSocket = CharacterMesh && !P.BackSocket.IsNone() && CharacterMesh->DoesSocketExist(P.BackSocket);
	const FTransform BackWorldTransform = bHasBackSocket
		? P.BackTransform * CharacterMesh->GetSocketTransform(P.BackSocket, RTS_World)
		: SkeletalMesh->GetComponentTransform();
	const FTransform BackPose = BackWorldTransform.GetRelativeTransform(PawnTransform);
	double Elapsed = FMath::Max(0.0, Weapon->GetPresentationTime() - State.StartTime);
	EHodgeWeaponPresentationPhase Phase = State.Phase;
	// 复制可能迟到，观察者可从时间戳补算闲置阶段。
	if (Phase == EHodgeWeaponPresentationPhase::Returning && Elapsed >= P.ReturnSeconds)
	{
		Elapsed -= P.ReturnSeconds;
		Phase = EHodgeWeaponPresentationPhase::Hovering;
	}
	if (Phase == EHodgeWeaponPresentationPhase::Hovering && Elapsed >= P.HoverSeconds)
	{
		Elapsed -= P.HoverSeconds;
		Phase = EHodgeWeaponPresentationPhase::Fading;
	}
	if (Phase == EHodgeWeaponPresentationPhase::Fading && Elapsed >= P.FadeSeconds) { Phase = EHodgeWeaponPresentationPhase::Hidden; }
	FTransform Pose = BackPose;
	VisibilityAmount = 1.f;
	if (Phase == EHodgeWeaponPresentationPhase::Hand)
	{
		Pose = SkeletalMesh->GetComponentTransform().GetRelativeTransform(PawnTransform);
		const float Alpha = P.DrawSeconds > 0.f ? FMath::Clamp(static_cast<float>(Elapsed) / P.DrawSeconds, 0.f, 1.f) : 1.f;
		VisibilityAmount = FMath::Lerp(State.StartVisibility, 1.f, Alpha);
	}
	else if (Phase == EHodgeWeaponPresentationPhase::Returning)
	{
		float Alpha = FMath::Clamp(static_cast<float>(Elapsed) / P.ReturnSeconds, 0.f, 1.f);
		Alpha = P.ReturnCurve ? FMath::Clamp(P.ReturnCurve->GetFloatValue(Alpha), 0.f, 1.f) : Alpha * Alpha * (3.f - 2.f * Alpha);
		Pose.Blend(State.StartRelativeTransform, BackPose, Alpha);
		Pose.AddToTranslation(P.ReturnArcOffset * (4.f * Alpha * (1.f - Alpha)));
	}
	else if (Phase == EHodgeWeaponPresentationPhase::Hovering)
	{
		if (bHasBackSocket) { Pose.AddToTranslation(FVector(0.f, 0.f, P.HoverAmplitude * FMath::Sin(Elapsed * UE_TWO_PI * P.HoverFrequency))); }
	}
	else if (Phase == EHodgeWeaponPresentationPhase::Fading)
	{
		if (bHasBackSocket) { Pose.AddToTranslation(FVector(0.f, 0.f, P.HoverAmplitude * FMath::Sin((Elapsed + P.HoverSeconds) * UE_TWO_PI * P.HoverFrequency))); }
		VisibilityAmount = 1.f - FMath::Clamp(static_cast<float>(Elapsed) / P.FadeSeconds, 0.f, 1.f);
	}
	else { VisibilityAmount = 0.f; }
	// 只切换可见 Mesh 的挂接，逻辑命中来源始终保持手部挂接。
	const bool bOnBack = bHasBackSocket && (Phase == EHodgeWeaponPresentationPhase::Hovering || Phase == EHodgeWeaponPresentationPhase::Fading || Phase == EHodgeWeaponPresentationPhase::Hidden);
	USceneComponent* VisualParent = bOnBack ? CharacterMesh : (Phase == EHodgeWeaponPresentationPhase::Returning ? Weapon->GetPawn()->GetRootComponent() : SkeletalMesh.Get());
	const FName VisualSocket = bOnBack ? P.BackSocket : NAME_None;
	if (WeaponVisualMesh->GetAttachParent() != VisualParent || WeaponVisualMesh->GetAttachSocketName() != VisualSocket)
	{
		WeaponVisualMesh->AttachToComponent(VisualParent, FAttachmentTransformRules::KeepWorldTransform, VisualSocket);
	}
	WeaponVisualMesh->SetWorldTransform(Pose * PawnTransform);
	WeaponVisualMesh->SetVisibility(VisibilityAmount > 0.f && GetNetMode() != NM_DedicatedServer);
	for (const auto& Material : DynamicMaterials) { Material->SetScalarParameterValue(P.VisibilityParameter, VisibilityAmount); }
	if (VisualPhase != Phase)
	{
		VisualPhase = Phase;
		if (GetNetMode() != NM_DedicatedServer) { OnPresentationPhaseChanged(Phase); }
	}
	const bool bAnimate = Phase != EHodgeWeaponPresentationPhase::Hidden;
	SetActorTickEnabled(bAnimate && GetNetMode() != NM_DedicatedServer);
}

void AHodgeWeaponPresentationActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	EvaluatePresentation();
}

void AHodgeWeaponPresentationActor::EndPlay(const EEndPlayReason::Type Reason)
{
	GetWorldTimerManager().ClearTimer(BindRetryTimer);
	Weapon.Reset();
	Super::EndPlay(Reason);
}
