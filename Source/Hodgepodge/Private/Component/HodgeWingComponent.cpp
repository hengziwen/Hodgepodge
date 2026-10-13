/** @file HodgeWingComponent.cpp
 * @brief 实现翅膀表现与采样组件相关的类型、接口与现有处理流程。
 */
#include "Component/HodgeWingComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "Animation/AnimSequence.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Materials/MaterialInstanceDynamic.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeWingComponent)

/** 构造翅膀表现与采样组件并初始化其默认状态。 */
UHodgeWingComponent::UHodgeWingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
	bTickInEditor = true;
}

/** 进入世界运行阶段并接入本对象的生命周期依赖。 */
void UHodgeWingComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeWing();
}

/** 初始化翅膀。 */
bool UHodgeWingComponent::InitializeWing()
{
	if (bInitialized && (IsValid(WingMesh) || (GetNetMode() == NM_DedicatedServer && Profile && Profile->PlaybackMode == EHodgeWingPlaybackMode::AnimationAsset && !Profile->bEnableAuthorityHitSampling))) { return true; }
	bInitializationFailed = false;
	Character = Cast<ACharacter>(GetOwner());
	// 即使配置错误，明确指定的自有翼网格也必须保持隐藏。
	if (Character.IsValid() && !ExistingMeshComponentName.IsNone())
	{
		TArray<USkeletalMeshComponent*> ExistingMeshes;
		GetOwner()->GetComponents(ExistingMeshes);
		for (auto* Existing : ExistingMeshes)
		{
			if (Existing->GetFName() != ExistingMeshComponentName || Existing == Character->GetMesh()) { continue; }
			Existing->SetHiddenInGame(true);
			Existing->SetVisibility(false);
			Existing->SetComponentTickEnabled(false);
		}
	}
	TArray<FText> Errors;
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!Character.IsValid() || !Profile || !Profile->Validate(Errors) ||
		!Character->GetMesh()->DoesSocketExist(Profile->AttachSocket))
	{
		UE_LOG(LogTemp, Error, TEXT("Wing %s: invalid profile/owner/socket; initialization stopped"), *GetNameSafe(GetOwner()));
		for (const FText& Error : Errors) { UE_LOG(LogTemp, Error, TEXT("Wing profile %s: %s"), *GetNameSafe(Profile), *Error.ToString()); }
		bInitializationFailed = true;
		SetComponentTickEnabled(false);
		return false;
	}
	if (Profile->PlaybackMode == EHodgeWingPlaybackMode::AnimationAsset && GetNetMode() == NM_DedicatedServer && !Profile->bEnableAuthorityHitSampling)
	{
		bInitialized = true;
		SetComponentTickEnabled(false);
		return true;
	}
	if (!ExistingMeshComponentName.IsNone())
	{
		TArray<USkeletalMeshComponent*> Meshes;
		GetOwner()->GetComponents(Meshes);
		for (auto* Candidate : Meshes)
		{
			if (Candidate->GetFName() == ExistingMeshComponentName && Candidate != Character->GetMesh()) { WingMesh = Candidate; break; }
		}
		if (!WingMesh || WingMesh->GetSkeletalMeshAsset() != Profile->Mesh || WingMesh->GetAttachParent() != Character->GetMesh())
		{
			UE_LOG(LogTemp, Error, TEXT("Wing %s: existing wing component is missing or incompatible"), *GetNameSafe(GetOwner()));
			WingMesh = nullptr;
			bInitializationFailed = true;
			SetComponentTickEnabled(false);
			return false;
		}
	}
	else if (FindObject<UObject>(GetOwner(), TEXT("WingMesh")))
	{
		UE_LOG(LogTemp, Error, TEXT("Wing %s: WingMesh component name already owned; refusing duplicate"), *GetNameSafe(GetOwner()));
		bInitializationFailed = true;
		SetComponentTickEnabled(false);
		return false;
	}
	else
	{
		WingMesh = NewObject<USkeletalMeshComponent>(GetOwner(), TEXT("WingMesh"));
		bOwnsWingMesh = true;
		WingMesh->SetSkeletalMeshAsset(Profile->Mesh);
		WingMesh->SetupAttachment(Character->GetMesh(), Profile->AttachSocket);
		WingMesh->SetRelativeTransform(Profile->MeshOffset);
		GetOwner()->AddInstanceComponent(WingMesh);
	}
	WingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WingMesh->SetGenerateOverlapEvents(false);
	WingMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	WingMesh->bEnableUpdateRateOptimizations = false;
	if (Profile->PlaybackMode == EHodgeWingPlaybackMode::Procedural) { WingMesh->SetAnimInstanceClass(Profile->AnimClass); }
	else { WingMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode); }
	WingMesh->bCastHiddenShadow = false;
	WingMesh->PrimaryComponentTick.bAllowTickOnDedicatedServer = true;
	WingMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	AddTickPrerequisiteComponent(Character->GetMesh());
	WingMesh->AddTickPrerequisiteComponent(this);
	GetOwner()->AddInstanceComponent(WingMesh);
	if (!WingMesh->IsRegistered()) { WingMesh->RegisterComponent(); }
	DynamicMaterials.Reset();
	OriginalMaterials.Reset();
	if (Profile->PlaybackMode == EHodgeWingPlaybackMode::AnimationAsset && GetNetMode() != NM_DedicatedServer)
	{
		for (int32 Index = 0; Index < WingMesh->GetNumMaterials(); ++Index)
		{
			OriginalMaterials.Add(WingMesh->GetMaterial(Index));
			if (Profile->Materials.IsValidIndex(Index) && Profile->Materials[Index]) { WingMesh->SetMaterial(Index, Profile->Materials[Index]); }
			if (auto* Material = WingMesh->CreateDynamicMaterialInstance(Index)) { DynamicMaterials.Add(Material); }
		}
	}
	bInitialized = true;
	Fade.Reset();
	ApplyFadeVisibility();
	// 专服只在命中任务请求时求值，不运行显示窗口或材质淡出。
	SetComponentTickEnabled(GetNetMode() != NM_DedicatedServer || Profile->PlaybackMode == EHodgeWingPlaybackMode::Procedural);
	return true;
}

void UHodgeWingComponent::SetWingVisible(bool bVisible)
{
	const bool bTickPose = Profile && Profile->PlaybackMode == EHodgeWingPlaybackMode::Procedural && bVisible;
	bVisible = bVisible && GetNetMode() != NM_DedicatedServer;
	Presentation.bVisible = bVisible;
	if (!IsValid(WingMesh)) { return; }
	WingMesh->SetHiddenInGame(!bVisible);
	WingMesh->SetVisibility(bVisible);
	// 新模式完全由主时钟求值，禁止网格自动推进第二套时钟。
	WingMesh->SetComponentTickEnabled(bTickPose);
}

void UHodgeWingComponent::ApplyFadeVisibility()
{
	for (const auto& Material : DynamicMaterials)
	{
		if (Material && Profile) { Material->SetScalarParameterValue(Profile->VisibilityParameter, Fade.Amount); }
	}
	SetWingVisible(Fade.Amount > 0.f);
}

void UHodgeWingComponent::BeginFadeOut()
{
	Fade.BeginFade(Profile ? Profile->FadeOutSeconds : 0.f);
	ApplyFadeVisibility();
}

bool UHodgeWingComponent::IsOwnerPresentationBlocked() const
{
	const auto* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner(), true);
	return ASC && (ASC->GetAvatarActor() != GetOwner() || ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death));
}

bool UHodgeWingComponent::ResolveSource(int32& BindingIndex, int32& InstanceId, float& Position, UAnimInstance*& SourceInstance)
{
	SourceInstance = Character.IsValid() ? Character->GetMesh()->GetAnimInstance() : nullptr;
	if (!SourceInstance || !Profile) { return false; }
	for (int32 Index = 0; Index < Profile->AnimationBindings.Num(); ++Index)
	{
		const auto& Binding = Profile->AnimationBindings[Index];
		if (auto* Montage = Cast<UAnimMontage>(Binding.SourceAnimation))
		{
			auto* Instance = SourceInstance->GetActiveInstanceForMontage(Montage);
			if (!Instance || !Instance->IsActive()) { continue; }
			BindingIndex = Index;
			InstanceId = Instance->GetInstanceID();
			Position = Instance->GetPosition();
			return FMath::IsFinite(Position);
		}
		if (auto* Single = Character->GetMesh()->GetSingleNodeInstance(); Single && Single->GetCurrentAsset() == Binding.SourceAnimation)
		{
			Position = Single->GetCurrentTime();
			if (!FMath::IsFinite(Position)) { return false; }
			if (PreviewSource != Binding.SourceAnimation || Position < LastPreviewPosition - KINDA_SMALL_NUMBER)
			{
				PreviewEpoch = PreviewEpoch < MAX_int32 ? PreviewEpoch + 1 : 1;
				PreviewSource = Binding.SourceAnimation;
			}
			LastPreviewPosition = Position;
			BindingIndex = Index;
			InstanceId = PreviewEpoch;
			return true;
		}
	}
	return false;
}

FHodgeWingActionHandle UHodgeWingComponent::BeginWingAction(int32 BindingIndex, int32 InstanceId, FGuid ExecutionId)
{
	if (!Profile || Profile->PlaybackMode != EHodgeWingPlaybackMode::AnimationAsset || !Profile->AnimationBindings.IsValidIndex(BindingIndex)) { return {}; }
	return Presentation.Begin(BindingIndex, InstanceId, ExecutionId);
}

bool UHodgeWingComponent::EndWingAction(FHodgeWingActionHandle Handle)
{
	const int32 EndingInstance = Presentation.SourceInstanceId;
	const int32 EndingBinding = Presentation.BindingIndex;
	if (!Presentation.End(Handle)) { return false; }
	SuppressedSourceInstance = ActiveSourceInstance;
	SuppressedInstanceId = EndingInstance;
	SuppressedBindingIndex = EndingBinding;
	BeginFadeOut();
	SourceInstanceId = INDEX_NONE;
	SourcePosition = 0.f;
	return true;
}

void UHodgeWingComponent::EndWingForExecution(FGuid ExecutionId)
{
	if (ExecutionId.IsValid() && Presentation.ExecutionId == ExecutionId) { EndWingAction(Presentation.Handle); }
}

void UHodgeWingComponent::ResetWingPresentation()
{
	if (Presentation.Handle.IsValid()) { EndWingAction(Presentation.Handle); }
	Presentation.Reset();
	Fade.Reset();
	ApplyFadeVisibility();
	Pose = {};
	SourcePosition = 0.f;
	SourceInstanceId = INDEX_NONE;
}

bool UHodgeWingComponent::EvaluateAnimationPose(int32 BindingIndex, float Position)
{
	if (!IsValid(WingMesh) || !Profile || !Profile->AnimationBindings.IsValidIndex(BindingIndex) || !FMath::IsFinite(Position)) { return false; }
	const auto& Binding = Profile->AnimationBindings[BindingIndex];
	if (!Binding.WingAnimation || !WingMesh->GetSkeletalMeshAsset() ||
		Binding.WingAnimation->GetSkeleton() != WingMesh->GetSkeletalMeshAsset()->GetSkeleton() ||
		!FMath::IsFinite(Binding.SourceStartTime) || !FMath::IsFinite(Binding.WingStartTime) ||
		!FMath::IsFinite(Binding.PlayRateRatio) || Binding.PlayRateRatio <= 0.f) { return false; }
	WingMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	auto* Single = WingMesh->GetSingleNodeInstance();
	if (!Single) { return false; }
	const float WingTime = Binding.EvaluateWingTime(Position);
	// 主时钟暂停时复用已经求值的局部姿势，挂接仍由父网格更新。
	if (Presentation.PoseEvaluations > 0 && Single->GetCurrentAsset() == Binding.WingAnimation && FMath::IsNearlyEqual(Single->GetCurrentTime(), WingTime, SMALL_NUMBER)) { return true; }
	if (Single->GetCurrentAsset() != Binding.WingAnimation) { WingMesh->SetAnimation(Binding.WingAnimation); }
	Single->SetPlaying(false);
	WingMesh->SetPosition(WingTime, false);
	// 空闲时网格不 Tick，因此显现前必须同步完成姿势求值。
	WingMesh->TickAnimation(0.f, false);
	WingMesh->RefreshBoneTransforms();
	++Presentation.PoseEvaluations;
	return true;
}

bool UHodgeWingComponent::PrepareAuthorityHitPose(FGuid ExecutionId)
{
	if (!ExecutionId.IsValid() || !GetOwner() || !GetOwner()->HasAuthority() || !Profile ||
		Profile->PlaybackMode != EHodgeWingPlaybackMode::AnimationAsset || !Profile->bEnableAuthorityHitSampling ||
		IsOwnerPresentationBlocked() || !InitializeWing() || !IsValid(WingMesh)) { return false; }
	auto* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	const auto* Ability = ASC ? Cast<UHodgeGameplayAbility_Definition>(ASC->GetAnimatingAbility()) : nullptr;
	if (!Ability || !Ability->IsActive() || Ability->IsExecutionEnding() || Ability->GetExecutionId() != ExecutionId) { return false; }
	auto* Body = Character.IsValid() ? Character->GetMesh() : nullptr;
	if (!Body) { return false; }
	if (Body->VisibilityBasedAnimTickOption != EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones &&
		(GetNetMode() == NM_DedicatedServer || !Body->WasRecentlyRendered(.2f)) && LastAuthorityBodyRefreshFrame != GFrameCounter)
	{
		// AlwaysTickPose 只保证动画时钟，命中需要当前角色挂点的骨骼矩阵。
		Body->RefreshBoneTransforms();
		LastAuthorityBodyRefreshFrame = GFrameCounter;
	}
	WingMesh->UpdateComponentToWorld();
	int32 BindingIndex = INDEX_NONE;
	int32 InstanceId = INDEX_NONE;
	float Position = 0.f;
	UAnimInstance* SourceInstance = nullptr;
	if (!ResolveSource(BindingIndex, InstanceId, Position, SourceInstance) ||
		Profile->AnimationBindings[BindingIndex].SourceAnimation != Ability->GetCurrentMontage() ||
		!EvaluateAnimationPose(BindingIndex, Position)) { return false; }
	// 命中准备不打开显示窗口，攻击结束后的材质尾迹也不产生伤害。
	SourcePosition = Presentation.SourceTime = Position;
	SourceInstanceId = Presentation.SourceInstanceId = InstanceId;
	Presentation.WingTime = Profile->AnimationBindings[BindingIndex].EvaluateWingTime(Position);
	return true;
}

void UHodgeWingComponent::RequestPresentationRefresh()
{
	if (!bInitializationFailed && !(Profile && Profile->PlaybackMode == EHodgeWingPlaybackMode::AnimationAsset && GetNetMode() == NM_DedicatedServer))
	{
		SetComponentTickEnabled(true);
	}
}

void UHodgeWingComponent::RefreshPresentation()
{
	if (bRefreshing || bInitializationFailed) { return; }
	TGuardValue<bool> Guard(bRefreshing, true);
	if (!bInitialized && !InitializeWing()) { return; }
	if (!Profile || Profile->PlaybackMode != EHodgeWingPlaybackMode::AnimationAsset || GetNetMode() == NM_DedicatedServer) { return; }
	if (!Character.IsValid() || !IsValid(WingMesh) || IsOwnerPresentationBlocked())
	{
		if (Presentation.Handle.IsValid() || Presentation.bVisible) { ResetWingPresentation(); }
		return;
	}
	int32 BindingIndex = INDEX_NONE;
	int32 InstanceId = INDEX_NONE;
	float Position = 0.f;
	UAnimInstance* SourceInstance = nullptr;
	if (!ResolveSource(BindingIndex, InstanceId, Position, SourceInstance))
	{
		Presentation.End(Presentation.Handle);
		BeginFadeOut();
		SourceInstanceId = INDEX_NONE;
		SourcePosition = 0.f;
		return;
	}
	if (SuppressedSourceInstance == SourceInstance && SuppressedInstanceId == InstanceId && SuppressedBindingIndex == BindingIndex)
	{
		BeginFadeOut();
		return;
	}
	FGuid Execution = Presentation.ExecutionId;
	if (ActiveSourceInstance != SourceInstance || Presentation.SourceInstanceId != InstanceId || Presentation.BindingIndex != BindingIndex || !Execution.IsValid())
	{
		Execution = FGuid::NewGuid();
	}
	if (auto* ASC = Cast<UHodgeAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner())))
	{
		const auto* Ability = Cast<UHodgeGameplayAbility_Definition>(ASC->GetAnimatingAbility());
		if (Ability && Ability->GetExecutionId().IsValid() && Ability->GetCurrentMontage() == Profile->AnimationBindings[BindingIndex].SourceAnimation)
		{
			Execution = Ability->GetExecutionId();
		}
	}
	ActiveSourceInstance = SourceInstance;
	if (!BeginWingAction(BindingIndex, InstanceId, Execution).IsValid()) { ResetWingPresentation(); return; }
	Presentation.SourceTime = SourcePosition = Position;
	Presentation.WingTime = Profile->AnimationBindings[BindingIndex].EvaluateWingTime(Position);
	SourceInstanceId = InstanceId;
	if (!Profile->AnimationBindings[BindingIndex].IsWindowOpen(Position)) { BeginFadeOut(); return; }
	if (!EvaluateAnimationPose(BindingIndex, Position)) { ResetWingPresentation(); return; }
	Fade.Show();
	ApplyFadeVisibility();
}

/** 更新姿势。 */
void UHodgeWingComponent::UpdatePose(float Position, int32 InstanceId, bool bActive, float DeltaSeconds)
{
	if (!Profile || !Profile->HasValidTiming()) { Pose = {}; bWasActive = false; SourceInstanceId = INDEX_NONE; return; }
	const bool bValid = bActive && InstanceId != INDEX_NONE && FMath::IsFinite(Position);
	if (bValid)
	{
		Pose = Profile->Evaluate(Position);
		SourcePosition = Position;
		SourceInstanceId = InstanceId;
		ReturnElapsed = 0.f;
	}
	else
	{
		if (bWasActive) { ReturnFrom = Pose; ReturnElapsed = 0.f; }
		ReturnElapsed += FMath::IsFinite(DeltaSeconds) ? FMath::Max(DeltaSeconds, 0.f) : 0.f;
		const float T = FMath::Clamp(ReturnElapsed / Profile->CancelReturnSeconds, 0.f, 1.f);
		Pose = FHodgeWingPose::Blend(ReturnFrom, {}, T * T * (3.f - 2.f * T));
		SourceInstanceId = INDEX_NONE;
		SourcePosition = 0.f;
	}
	bWasActive = bValid;
}

/** 更新组件。 */
void UHodgeWingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* Tick)
{
	Super::TickComponent(DeltaTime, TickType, Tick);
	if (Profile && Profile->PlaybackMode == EHodgeWingPlaybackMode::AnimationAsset)
	{
		RefreshPresentation();
		Fade.Advance(DeltaTime);
		ApplyFadeVisibility();
		return;
	}
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!WingMesh || !Character.IsValid() || !Profile) { return; }
	// 当前网格使用的动画实例。
	UAnimInstance* Anim = Character->GetMesh()->GetAnimInstance();
	FAnimMontageInstance* Instance = Anim ? Anim->GetActiveInstanceForMontage(Profile->AttackMontage) : nullptr;
	const bool bActive = Instance && !Instance->IsStopped();
	UpdatePose(bActive ? Instance->GetPosition() : 0.f,
		bActive ? Instance->GetInstanceID() : INDEX_NONE, bActive, DeltaTime);
	SetWingVisible(bActive && SourcePosition >= Profile->WindupStart && SourcePosition < Profile->RecoveryEnd && !IsOwnerPresentationBlocked());
}

/** 离开世界运行阶段并解除本对象持有的监听或资源。 */
void UHodgeWingComponent::EndPlay(const EEndPlayReason::Type Reason)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (Character.IsValid()) { RemoveTickPrerequisiteComponent(Character->GetMesh()); }
	ResetWingPresentation();
	if (WingMesh)
	{
		WingMesh->RemoveTickPrerequisiteComponent(this);
		if (!bOwnsWingMesh)
		{
			for (int32 Index = 0; Index < OriginalMaterials.Num(); ++Index) { WingMesh->SetMaterial(Index, OriginalMaterials[Index]); }
		}
		if (bOwnsWingMesh) { WingMesh->DestroyComponent(); }
		WingMesh = nullptr;
	}
	bInitialized = false;
	LastAuthorityBodyRefreshFrame = MAX_uint64;
	DynamicMaterials.Reset();
	OriginalMaterials.Reset();
	ActiveSourceInstance.Reset();
	SuppressedSourceInstance.Reset();
	Character.Reset();
	Pose = {};
	bWasActive = false;
	SourceInstanceId = INDEX_NONE;
	Super::EndPlay(Reason);
}
