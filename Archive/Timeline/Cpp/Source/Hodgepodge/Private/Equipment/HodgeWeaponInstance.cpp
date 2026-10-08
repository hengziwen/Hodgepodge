// Copyright Epic Games, Inc. All Rights Reserved.

#include "Equipment/HodgeWeaponInstance.h"

// 提供 APawn，用于获取武器所属 Pawn 及玩家控制状态。
#include "GameFramework/Pawn.h"

// 提供 UWorld，用于获取当前世界运行时间。
#include "Engine/World.h"

// 提供 FMath 数学工具，用于计算距离最近一次武器交互的时间。
#include "Math/UnrealMathUtility.h"

// 提供 check 等运行时断言宏。
#include "Misc/AssertionMacros.h"

// 提供输入设备子系统，用于激活和移除设备属性。
#include "GameFramework/InputDeviceSubsystem.h"

// 提供输入设备属性及其激活参数。
#include "GameFramework/InputDeviceProperties.h"

// 提供 HodgeHealthComponent，用于监听所属 Pawn 的死亡事件。
#include "Component/HodgeHealthComponent.h"

#include "Equipment/HodgeWeaponPresentationActor.h"
#include "Equipment/HodgeWeaponPresentationProfile.h"
#include "Equipment/HodgeEquipmentManagerComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeWeaponInstance)

class UAnimInstance;
struct FGameplayTagContainer;

// 构造武器运行时实例，并尝试监听玩家 Pawn 的死亡事件。
UHodgeWeaponInstance::UHodgeWeaponInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Listen for death of the owning pawn so that any device properties can be removed if we
	// die and can't unequip

	// 获取当前武器实例所属的 Pawn。
	if (APawn* Pawn = GetPawn())
	{
		// We only need to do this for player controlled pawns, since AI and others won't have input devices on the client

		// 只有玩家控制的 Pawn 才需要处理输入设备效果，AI 不需要客户端输入设备反馈。
		if (Pawn->IsPlayerControlled())
		{
			// 查找所属 Pawn 的生命组件，用于监听死亡开始事件。
			if (UHodgeHealthComponent* HealthComponent = UHodgeHealthComponent::FindHealthComponent(GetPawn()))
			{
				// Pawn 开始死亡时通知当前武器清理仍在运行的输入设备效果。
				HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
			}
		}
	}
}

// 武器装备时更新装备时间并启用对应的输入设备效果。
void UHodgeWeaponInstance::OnEquipped()
{
	const bool bAlreadyEquipped = bPresentationEquipped;
	InitializePresentation();
	if (bAlreadyEquipped) { return; }
	// 保留基础 EquipmentInstance 的通用装备生命周期逻辑。
	Super::OnEquipped();

	// 获取当前武器实例所属的 World。
	UWorld* World = GetWorld();

	// 武器装备时必须存在有效 World。
	check(World);

	// 记录最近一次装备该武器的世界时间。
	TimeLastEquipped = World->GetTimeSeconds();

	// 应用武器装备期间需要持续生效的输入设备属性。
	ApplyDeviceProperties();
}

// 武器卸下时移除由该武器启用的输入设备效果。
void UHodgeWeaponInstance::OnUnequipped()
{
	ShutdownPresentation();
	// 保留基础 EquipmentInstance 的通用卸装生命周期逻辑。
	Super::OnUnequipped();

	// 清除当前武器激活的输入设备属性。
	RemoveDeviceProperties();
}

// 更新最近一次武器开火或使用的时间。
void UHodgeWeaponInstance::UpdateFiringTime()
{
	// 获取当前武器实例所属的 World。
	UWorld* World = GetWorld();

	// 更新开火时间时必须存在有效 World。
	check(World);

	// 记录当前世界时间作为最近一次开火时间。
	TimeLastFired = World->GetTimeSeconds();
}

// 返回距离最近一次装备或开火交互已经过去的时间。
float UHodgeWeaponInstance::GetTimeSinceLastInteractedWith() const
{
	// 获取当前武器实例所属的 World。
	UWorld* World = GetWorld();

	// 计算交互时间时必须存在有效 World。
	check(World);

	// 获取当前世界时间。
	const double WorldTime = World->GetTimeSeconds();

	// 默认先计算距离最近一次装备该武器已经过去多久。
	double Result = WorldTime - TimeLastEquipped;

	// 只有武器至少记录过一次开火时间时才参与最近交互时间比较。
	if (TimeLastFired > 0.0)
	{
		// 计算距离最近一次开火已经过去多久。
		const double TimeSinceFired = WorldTime - TimeLastFired;

		// 取更短的时间，代表装备和开火两个事件中距离现在最近的一次交互。
		Result = FMath::Min(Result, TimeSinceFired);
	}

	// 返回距离最近一次武器交互已经过去的秒数。
	return Result;
}

// 根据装备状态和外观标签选择最合适的武器动画层，目前动画层选择逻辑暂未启用。
TSubclassOf<UAnimInstance> UHodgeWeaponInstance::PickBestAnimLayer(bool bEquipped,
                                                                   const FGameplayTagContainer& CosmeticTags) const
{
	//const FHodgeAnimLayerSelectionSet& SetToQuery = (bEquipped ? EquippedAnimSet : UneuippedAnimSet);
	//return SetToQuery.SelectBestLayer(CosmeticTags);

	// 当前没有启用动画层配置，因此返回空的 AnimInstance 类型。
	return TSubclassOf<UAnimInstance>();
}

// 获取当前武器所属 Pawn 对应的平台用户 ID。
const FPlatformUserId UHodgeWeaponInstance::GetOwningUserId() const
{
	// 通过 EquipmentInstance 的 Outer 获取当前武器所属 Pawn。
	if (const APawn* Pawn = GetPawn())
	{
		// 返回该 Pawn 对应的平台用户 ID。
		return Pawn->GetPlatformUserId();
	}

	// 没有有效 Pawn 时返回无效的平台用户 ID。
	return PLATFORMUSERID_NONE;
}

// 激活当前武器配置的所有输入设备属性，并保存对应句柄。
void UHodgeWeaponInstance::ApplyDeviceProperties()
{
	// 获取当前武器所属玩家的平台用户 ID。
	const FPlatformUserId UserId = GetOwningUserId();

	// 只有有效的平台用户才能应用输入设备效果。
	if (UserId.IsValid())
	{
		// 获取全局输入设备子系统。
		if (UInputDeviceSubsystem* InputDeviceSubsystem = UInputDeviceSubsystem::Get())
		{
			// 遍历当前武器配置的所有输入设备属性。
			for (TObjectPtr<UInputDeviceProperty>& DeviceProp : ApplicableDeviceProperties)
			{
				// 创建当前设备属性的激活参数。
				FActivateDevicePropertyParams Params = {};

				// 指定设备效果所属的平台用户。
				Params.UserId = UserId;

				// By default, the device property will be played on the Platform User's Primary Input Device.
				// If you want to override this and set a specific device, then you can set the DeviceId parameter.
				//Params.DeviceId = <some specific device id>;

				// Don't remove this property it was evaluated. We want the properties to be applied as long as we are holding the 
				// weapon, and will remove them manually in OnUnequipped

				// 让设备属性持续循环生效，直到武器卸下或角色死亡时主动移除。
				Params.bLooping = true;

				// 激活设备属性，并保存返回句柄以便后续精确移除。
				DevicePropertyHandles.Emplace(InputDeviceSubsystem->ActivateDeviceProperty(DeviceProp, Params));
			}
		}
	}
}

// 移除当前武器曾经激活的所有输入设备属性。
void UHodgeWeaponInstance::RemoveDeviceProperties()
{
	// 获取当前武器所属玩家的平台用户 ID。
	const FPlatformUserId UserId = GetOwningUserId();

	// 只有用户有效且确实存在已激活设备属性时才执行清理。
	if (UserId.IsValid() && !DevicePropertyHandles.IsEmpty())
	{
		// Remove any device properties that have been applied

		// 获取输入设备子系统，用于根据保存的句柄移除设备属性。
		if (UInputDeviceSubsystem* InputDeviceSubsystem = UInputDeviceSubsystem::Get())
		{
			// 一次性移除当前武器记录的所有设备属性句柄。
			InputDeviceSubsystem->RemoveDevicePropertyHandles(DevicePropertyHandles);

			// 清空已经失效的设备属性句柄记录。
			DevicePropertyHandles.Empty();
		}
	}
}

// 所属 Pawn 开始死亡时清理当前武器仍然激活的输入设备效果。
void UHodgeWeaponInstance::OnDeathStarted(AActor* OwningActor)
{
	bPresentationDisabled = true;
	HandRequests.Reset();
	UpdateOwnerPosePolicy();
	ClearPresentationTimer();
	SetPresentationPhase(EHodgeWeaponPresentationPhase::Hidden, LocalPresentation.ActivationKey);
	// Remove any possibly active device properties when we die to make sure that there aren't any lingering around

	// 即使死亡时没有正常执行卸装流程，也确保设备效果不会继续残留。
	RemoveDeviceProperties();
}

// 武器表现、请求和复制

void UHodgeWeaponInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, ReplicatedPresentation);
}

double UHodgeWeaponInstance::GetPresentationTime() const
{
	const auto* World = GetWorld();
	const auto* State = World ? World->GetGameState() : nullptr;
	return State ? State->GetServerWorldTimeSeconds() : (World ? World->GetTimeSeconds() : 0.0);
}

bool UHodgeWeaponInstance::CanDrivePresentation() const
{
	const APawn* Pawn = GetPawn();
	const auto* Health = Pawn ? UHodgeHealthComponent::FindHealthComponent(Pawn) : nullptr;
	return bPresentationEquipped && !bPresentationDisabled && PresentationProfile && Pawn
		&& (Pawn->HasAuthority() || Pawn->IsLocallyControlled()) && (!Health || !Health->IsDeadOrDying());
}

UHodgeWeaponInstance* UHodgeWeaponInstance::ResolvePresentationWeapon(APawn* Pawn, UObject* SourceObject)
{
	const auto* Manager = Pawn ? Pawn->FindComponentByClass<UHodgeEquipmentManagerComponent>() : nullptr;
	if (!Manager) { return nullptr; }
	const auto Instances = Manager->GetEquipmentInstancesOfType(StaticClass());
	if (auto* Source = Cast<UHodgeWeaponInstance>(SourceObject))
	{
		return Instances.Contains(Source) && Source->PresentationProfile && Source->bPresentationEquipped && !Source->bPresentationDisabled ? Source : nullptr;
	}
	UHodgeWeaponInstance* Result = nullptr;
	for (auto* Instance : Instances)
	{
		auto* Weapon = CastChecked<UHodgeWeaponInstance>(Instance);
		if (!Weapon->PresentationProfile || !Weapon->bPresentationEquipped || Weapon->bPresentationDisabled) { continue; }
		if (Result) { return nullptr; }
		Result = Weapon;
	}
	return Result;
}

void UHodgeWeaponInstance::InitializePresentation()
{
	if (bPresentationEquipped) { RefreshPresentationActors(); return; }
	bPresentationEquipped = true;
	bPresentationDisabled = false;
	if (auto* Health = UHodgeHealthComponent::FindHealthComponent(GetPawn()))
	{
		Health->OnDeathStarted.RemoveAll(this);
		Health->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
		bPresentationDisabled = Health->IsDeadOrDying();
	}
	LocalPresentation = ReplicatedPresentation;
	if (GetPawn() && GetPawn()->HasAuthority()) { SetPresentationPhase(EHodgeWeaponPresentationPhase::Hidden, 0); }
	RefreshPresentationActors();
}

void UHodgeWeaponInstance::ClearPresentationTimer()
{
	if (UWorld* World = GetWorld()) { World->GetTimerManager().ClearTimer(PresentationTimer); }
}

void UHodgeWeaponInstance::ShutdownPresentation()
{
	if (auto* Health = UHodgeHealthComponent::FindHealthComponent(GetPawn())) { Health->OnDeathStarted.RemoveAll(this); }
	HandRequests.Reset();
	UpdateOwnerPosePolicy();
	ClearPresentationTimer();
	bPresentationEquipped = false;
	bPresentationDisabled = true;
	bPredictedPresentation = false;
	SetPresentationPhase(EHodgeWeaponPresentationPhase::Hidden, 0);
}

FGuid UHodgeWeaponInstance::AcquireHandUse(FGuid ExecutionId, int32 EventIndex, int32 ActivationKey)
{
	if (!ExecutionId.IsValid() || !CanDrivePresentation()) { return {}; }
	for (const auto& Entry : HandRequests)
	{
		if (Entry.Value.ExecutionId == ExecutionId && Entry.Value.EventIndex == EventIndex) { return Entry.Key; }
	}
	const FGuid Handle = FGuid::NewGuid();
	HandRequests.Add(Handle, {ExecutionId, EventIndex, ActivationKey});
	UpdateOwnerPosePolicy();
	ClearPresentationTimer();
	bPredictedPresentation = !GetPawn()->HasAuthority();
	if (LocalPresentation.Phase != EHodgeWeaponPresentationPhase::Hand)
	{
		SetPresentationPhase(EHodgeWeaponPresentationPhase::Hand, ActivationKey);
	}
	else
	{
		// 更新身份但不重播连续攻击的显现效果。
		LocalPresentation.ActivationKey = ActivationKey;
		if (GetPawn()->HasAuthority()) { ReplicatedPresentation = LocalPresentation; GetPawn()->ForceNetUpdate(); }
	}
	RefreshPresentationActors();
	return Handle;
}

void UHodgeWeaponInstance::ReleaseHandUse(FGuid Handle)
{
	if (HandRequests.Remove(Handle) == 0) { return; }
	UpdateOwnerPosePolicy();
	if (HandRequests.IsEmpty()) { BeginIdlePresentation(); }
}

void UHodgeWeaponInstance::ReleaseHandUsesForExecution(const FGuid& ExecutionId)
{
	TArray<FGuid> Handles;
	for (const auto& Entry : HandRequests) { if (Entry.Value.ExecutionId == ExecutionId) { Handles.Add(Entry.Key); } }
	for (const FGuid& Handle : Handles) { ReleaseHandUse(Handle); }
}

void UHodgeWeaponInstance::BeginIdlePresentation()
{
	if (!CanDrivePresentation()) { return; }
	SchedulePresentationPhase(EHodgeWeaponPresentationPhase::Returning, PresentationProfile->ReturnGraceSeconds);
}

void UHodgeWeaponInstance::SchedulePresentationPhase(EHodgeWeaponPresentationPhase Phase, float Seconds)
{
	ClearPresentationTimer();
	const int32 Version = LocalPresentation.Revision;
	FTimerDelegate Callback = FTimerDelegate::CreateWeakLambda(this, [this, Version, Phase]()
	{
		if (CanDrivePresentation() && HandRequests.IsEmpty() && LocalPresentation.Revision == Version)
		{
			SetPresentationPhase(Phase, LocalPresentation.ActivationKey);
		}
	});
	if (Seconds <= 0.f) { Callback.ExecuteIfBound(); }
	else if (UWorld* World = GetWorld()) { World->GetTimerManager().SetTimer(PresentationTimer, Callback, Seconds, false); }
}

void UHodgeWeaponInstance::SetPresentationPhase(EHodgeWeaponPresentationPhase Phase, int32 ActivationKey)
{
	ClearPresentationTimer();
	const auto Previous = LocalPresentation;
	LocalPresentation.Phase = Phase;
	LocalPresentation.StartTime = GetPresentationTime();
	LocalPresentation.ActivationKey = ActivationKey;
	LocalPresentation.Revision = Previous.Revision < MAX_int32 ? Previous.Revision + 1 : 1;
	LocalPresentation.StartVisibility = Phase == EHodgeWeaponPresentationPhase::Hand ? (Previous.Phase == Phase ? 1.f : 0.f) : 1.f;
	if (Phase == EHodgeWeaponPresentationPhase::Returning && GetPawn())
	{
		for (AActor* Actor : GetSpawnedActors())
		{
			if (auto* Visual = Cast<AHodgeWeaponPresentationActor>(Actor))
			{
				LocalPresentation.StartRelativeTransform = Visual->GetVisualTransform().GetRelativeTransform(GetPawn()->GetRootComponent()->GetComponentTransform());
				break;
			}
		}
	}
	if (GetPawn() && GetPawn()->HasAuthority()) { ReplicatedPresentation = LocalPresentation; GetPawn()->ForceNetUpdate(); }
	RefreshPresentationActors();
	if (!CanDrivePresentation() || !HandRequests.IsEmpty()) { return; }
	if (Phase == EHodgeWeaponPresentationPhase::Returning) { SchedulePresentationPhase(EHodgeWeaponPresentationPhase::Hovering, PresentationProfile->ReturnSeconds); }
	else if (Phase == EHodgeWeaponPresentationPhase::Hovering) { SchedulePresentationPhase(EHodgeWeaponPresentationPhase::Fading, PresentationProfile->HoverSeconds); }
	else if (Phase == EHodgeWeaponPresentationPhase::Fading) { SchedulePresentationPhase(EHodgeWeaponPresentationPhase::Hidden, PresentationProfile->FadeSeconds); }
}

FHodgeWeaponPresentationState UHodgeWeaponInstance::GetPresentationState() const
{
	if (!bPresentationEquipped || bPresentationDisabled) { return {}; }
	return GetPawn() && (GetPawn()->HasAuthority() || bPredictedPresentation) ? LocalPresentation : ReplicatedPresentation;
}

void UHodgeWeaponInstance::OnRep_PresentationState()
{
	if (!bPresentationEquipped) { RefreshPresentationActors(); return; }
	if (bPredictedPresentation)
	{
		if (!HandRequests.IsEmpty() || ReplicatedPresentation.ActivationKey != LocalPresentation.ActivationKey) { return; }
		if (ReplicatedPresentation.Phase == EHodgeWeaponPresentationPhase::Hand) { return; }
		ClearPresentationTimer();
		bPredictedPresentation = false;
		LocalPresentation = ReplicatedPresentation;
	}
	RefreshPresentationActors();
}

void UHodgeWeaponInstance::RejectPredictedHandUse(int32 ActivationKey)
{
	if (!GetPawn() || GetPawn()->HasAuthority()) { return; }
	TArray<FGuid> Handles;
	for (const auto& Entry : HandRequests) { if (Entry.Value.ActivationKey == ActivationKey) { Handles.Add(Entry.Key); } }
	for (const FGuid& Handle : Handles) { ReleaseHandUse(Handle); }
	if (HandRequests.IsEmpty() && LocalPresentation.ActivationKey == ActivationKey)
	{
		ClearPresentationTimer();
		bPredictedPresentation = false;
		LocalPresentation = ReplicatedPresentation;
		RefreshPresentationActors();
	}
}

void UHodgeWeaponInstance::RefreshPresentationActors()
{
	if (!PresentationProfile) { return; }
	for (AActor* Actor : GetSpawnedActors()) { if (auto* Visual = Cast<AHodgeWeaponPresentationActor>(Actor)) { Visual->BindWeapon(this); } }
}

void UHodgeWeaponInstance::OnSpawnedActorsChanged()
{
	Super::OnSpawnedActorsChanged();
	RefreshPresentationActors();
}

void UHodgeWeaponInstance::BeginDestroy()
{
	ClearPresentationTimer();
	Super::BeginDestroy();
}

void UHodgeWeaponInstance::UpdateOwnerPosePolicy()
{
	if (GetPawn() && GetPawn()->HasAuthority() && !HandRequests.IsEmpty())
	{
		if (!bPosePolicyOverridden)
		{
			if (auto* Character = Cast<ACharacter>(GetPawn()))
			{
				PoseMesh = Character->GetMesh();
				SavedPosePolicy = static_cast<uint8>(PoseMesh->VisibilityBasedAnimTickOption);
				bPosePolicyOverridden = true;
				PoseMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
			}
		}
	}
	else if (bPosePolicyOverridden)
	{
		if (PoseMesh.IsValid() && PoseMesh->VisibilityBasedAnimTickOption == EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones)
		{
			PoseMesh->VisibilityBasedAnimTickOption = static_cast<EVisibilityBasedAnimTickOption>(SavedPosePolicy);
		}
		bPosePolicyOverridden = false;
		PoseMesh.Reset();
	}
}
