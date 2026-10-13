/** @file HodgeWeaponInstance.cpp
 * @brief 实现武器运行实例相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#include "Equipment/HodgeWeaponInstance.h"
#include "Component/HodgeCombatComponentBase.h"

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
	NextHandRequestOrder = 0;
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

/** 获取表现时间。 */
double UHodgeWeaponInstance::GetPresentationTime() const
{
	// 本次操作所在的世界。
	const auto* World = GetWorld();
	const auto* State = World ? World->GetGameState() : nullptr;
	return State ? State->GetServerWorldTimeSeconds() : (World ? World->GetTimeSeconds() : 0.0);
}

/** 检查是否允许Drive表现。 */
bool UHodgeWeaponInstance::CanDrivePresentation() const
{
	// 当前操作关联的Pawn角色。
	const APawn* Pawn = GetPawn();
	const auto* Health = Pawn ? UHodgeHealthComponent::FindHealthComponent(Pawn) : nullptr;
	return bPresentationEquipped && !bPresentationDisabled && PresentationProfile && Pawn
		&& (Pawn->HasAuthority() || Pawn->IsLocallyControlled()) && (!Health || !Health->IsDeadOrDying());
}

/** 解析表现武器。 */
UHodgeWeaponInstance* UHodgeWeaponInstance::ResolvePresentationWeapon(APawn* Pawn, UObject* SourceObject)
{
	const auto* Manager = Pawn ? Pawn->FindComponentByClass<UHodgeEquipmentManagerComponent>() : nullptr;
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!Manager) { return nullptr; }
	const auto Instances = Manager->GetEquipmentInstancesOfType(StaticClass());
	if (auto* Source = Cast<UHodgeWeaponInstance>(SourceObject))
	{
		return Instances.Contains(Source) && Source->PresentationProfile && Source->bPresentationEquipped && !Source->
		       bPresentationDisabled
			       ? Source
			       : nullptr;
	}
	UHodgeWeaponInstance* Result = nullptr;
	for (auto* Instance : Instances)
	{
		auto* Weapon = CastChecked<UHodgeWeaponInstance>(Instance);
		if (!Weapon->PresentationProfile || !Weapon->bPresentationEquipped || Weapon->bPresentationDisabled)
		{
			continue;
		}
		if (Result) { return nullptr; }
		Result = Weapon;
	}
	return Result;
}

/** 初始化表现。 */
void UHodgeWeaponInstance::InitializePresentation()
{
	if (bPresentationEquipped)
	{
		RefreshPresentationActors();
		return;
	}
	bPresentationEquipped = true;
	bPresentationDisabled = false;
	// 解除本对象的监听，避免旧生命周期回调继续进入。
	if (auto* Health = UHodgeHealthComponent::FindHealthComponent(GetPawn()))
	{
		Health->OnDeathStarted.RemoveAll(this);
		Health->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
		bPresentationDisabled = Health->IsDeadOrDying();
	}
	LocalPresentation = ReplicatedPresentation;
	// 按服务器权限区分本次处理分支。
	if (GetPawn() && GetPawn()->HasAuthority()) { SetPresentationPhase(EHodgeWeaponPresentationPhase::Hidden, 0); }
	RefreshPresentationActors();
}

/** 清空表现计时器。 */
void UHodgeWeaponInstance::ClearPresentationTimer()
{
	if (UWorld* World = GetWorld()) { World->GetTimerManager().ClearTimer(PresentationTimer); }
}

/** 清理当前武器表现的计时器与来源记录。 */
void UHodgeWeaponInstance::ShutdownPresentation()
{
	// 解除本对象的监听，避免旧生命周期回调继续进入。
	if (auto* Health = UHodgeHealthComponent::FindHealthComponent(GetPawn()))
	{
		Health->OnDeathStarted.RemoveAll(this);
	}
	HandRequests.Reset();
	UpdateOwnerPosePolicy();
	ClearPresentationTimer();
	bPresentationEquipped = false;
	bPresentationDisabled = true;
	bPredictedPresentation = false;
	SetPresentationPhase(EHodgeWeaponPresentationPhase::Hidden, 0);
}

/** 按执行身份申请武器手持窗口。 */
FGuid UHodgeWeaponInstance::AcquireHandUse(FGuid ExecutionId, int32 OccurrenceId, int32 ActivationKey, EHodgeWeaponHandEndBehavior EndBehavior)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!ExecutionId.IsValid() || !CanDrivePresentation() || NextHandRequestOrder == MAX_uint64 ||
		(EndBehavior != EHodgeWeaponHandEndBehavior::ReturnToBack && EndBehavior != EHodgeWeaponHandEndBehavior::FadeAtHand)) { return {}; }
	for (const auto& Entry : HandRequests)
	{
		if (Entry.Value.ExecutionId == ExecutionId && Entry.Value.OccurrenceId == OccurrenceId) { return Entry.Key; }
	}
	const FGuid Handle = FGuid::NewGuid();
	HandRequests.Add(Handle, {ExecutionId, OccurrenceId, ActivationKey, EndBehavior, ++NextHandRequestOrder});
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
		if (GetPawn()->HasAuthority())
		{
			ReplicatedPresentation = LocalPresentation;
			GetPawn()->ForceNetUpdate();
		}
	}
	RefreshPresentationActors();
	return Handle;
}

/** 释放手部Use。 */
void UHodgeWeaponInstance::ReleaseHandUse(FGuid Handle)
{
	FHandRequest Released;
	if (!HandRequests.RemoveAndCopyValue(Handle, Released)) { return; }
	UpdateOwnerPosePolicy();
	if (HandRequests.IsEmpty()) { BeginIdlePresentation(Released.EndBehavior); }
}

/** 释放手部Uses关联执行。 */
void UHodgeWeaponInstance::ReleaseHandUsesForExecution(const FGuid& ExecutionId)
{
	TArray<FGuid> Handles;
	for (const auto& Entry : HandRequests) { if (Entry.Value.ExecutionId == ExecutionId) { Handles.Add(Entry.Key); } }
	Handles.Sort([this](const FGuid& A, const FGuid& B) { return HandRequests.FindChecked(A).Order < HandRequests.FindChecked(B).Order; });
	for (const FGuid& Handle : Handles) { ReleaseHandUse(Handle); }
}

/** 开始Idle表现。 */
void UHodgeWeaponInstance::BeginIdlePresentation(EHodgeWeaponHandEndBehavior EndBehavior)
{
	if (!CanDrivePresentation()) { return; }
	if (EndBehavior == EHodgeWeaponHandEndBehavior::FadeAtHand)
	{
		SetPresentationPhase(EHodgeWeaponPresentationPhase::Fading, LocalPresentation.ActivationKey, EndBehavior);
	}
	else { SchedulePresentationPhase(EHodgeWeaponPresentationPhase::Returning, PresentationProfile->ReturnGraceSeconds, EndBehavior); }
}

/** 按配置安排下一次武器表现阶段切换。 */
void UHodgeWeaponInstance::SchedulePresentationPhase(EHodgeWeaponPresentationPhase Phase, float Seconds, EHodgeWeaponHandEndBehavior EndBehavior)
{
	ClearPresentationTimer();
	const int32 Version = LocalPresentation.Revision;
	FTimerDelegate Callback = FTimerDelegate::CreateWeakLambda(this, [this, Version, Phase, EndBehavior]()
	{
		if (CanDrivePresentation() && HandRequests.IsEmpty() && LocalPresentation.Revision == Version)
		{
			SetPresentationPhase(Phase, LocalPresentation.ActivationKey, EndBehavior);
		}
	});
	if (Seconds <= 0.f) { Callback.ExecuteIfBound(); }
	else if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(PresentationTimer, Callback, Seconds, false);
	}
}

/** 设置表现Phase。 */
void UHodgeWeaponInstance::SetPresentationPhase(EHodgeWeaponPresentationPhase Phase, int32 ActivationKey, EHodgeWeaponHandEndBehavior EndBehavior)
{
	ClearPresentationTimer();
	const auto Previous = LocalPresentation;
	LocalPresentation.Phase = Phase;
	LocalPresentation.EndBehavior = EndBehavior;
	LocalPresentation.StartTime = GetPresentationTime();
	LocalPresentation.ActivationKey = ActivationKey;
	LocalPresentation.Revision = Previous.Revision < MAX_int32 ? Previous.Revision + 1 : 1;
	LocalPresentation.StartVisibility = Phase == EHodgeWeaponPresentationPhase::Hand
		                                    ? (Previous.Phase == Phase ? 1.f : 0.f)
		                                    : 1.f;
	if (PresentationProfile && ((Phase == EHodgeWeaponPresentationPhase::Fading && EndBehavior == EHodgeWeaponHandEndBehavior::FadeAtHand) ||
		(Phase == EHodgeWeaponPresentationPhase::Hand && Previous.Phase == EHodgeWeaponPresentationPhase::Fading && Previous.EndBehavior == EHodgeWeaponHandEndBehavior::FadeAtHand)))
	{
		const float Elapsed = float(FMath::Max(0.0, GetPresentationTime() - Previous.StartTime));
		// 直接淡出和紧接的新攻击都从当前透明度开始，不闪回零或一。
		LocalPresentation.StartVisibility = Previous.Phase == EHodgeWeaponPresentationPhase::Hand
			? FMath::Lerp(Previous.StartVisibility, 1.f, PresentationProfile->DrawSeconds > 0.f ? FMath::Clamp(Elapsed / PresentationProfile->DrawSeconds, 0.f, 1.f) : 1.f)
			: Previous.StartVisibility * (1.f - FMath::Clamp(Elapsed / PresentationProfile->FadeSeconds, 0.f, 1.f));
	}
	if (Phase == EHodgeWeaponPresentationPhase::Returning && GetPawn())
	{
		for (AActor* Actor : GetSpawnedActors())
		{
			if (auto* Visual = Cast<AHodgeWeaponPresentationActor>(Actor))
			{
				LocalPresentation.StartRelativeTransform = Visual->GetVisualTransform().GetRelativeTransform(
					GetPawn()->GetRootComponent()->GetComponentTransform());
				break;
			}
		}
	}
	// 按服务器权限区分本次处理分支。
	if (GetPawn() && GetPawn()->HasAuthority())
	{
		ReplicatedPresentation = LocalPresentation;
		GetPawn()->ForceNetUpdate();
	}
	RefreshPresentationActors();
	if (!CanDrivePresentation() || !HandRequests.IsEmpty()) { return; }
	if (Phase == EHodgeWeaponPresentationPhase::Returning)
	{
		SchedulePresentationPhase(EHodgeWeaponPresentationPhase::Hovering, PresentationProfile->ReturnSeconds, EndBehavior);
	}
	else if (Phase == EHodgeWeaponPresentationPhase::Hovering)
	{
		SchedulePresentationPhase(EHodgeWeaponPresentationPhase::Fading, PresentationProfile->HoverSeconds, EndBehavior);
	}
	else if (Phase == EHodgeWeaponPresentationPhase::Fading)
	{
		SchedulePresentationPhase(EHodgeWeaponPresentationPhase::Hidden, PresentationProfile->FadeSeconds, EndBehavior);
	}
}

/** 获取表现状态。 */
FHodgeWeaponPresentationState UHodgeWeaponInstance::GetPresentationState() const
{
	if (!bPresentationEquipped || bPresentationDisabled) { return {}; }
	return GetPawn() && (GetPawn()->HasAuthority() || bPredictedPresentation)
		       ? LocalPresentation
		       : ReplicatedPresentation;
}

/** 收到表现状态复制后更新本地状态。 */
void UHodgeWeaponInstance::OnRep_PresentationState()
{
	if (!bPresentationEquipped)
	{
		RefreshPresentationActors();
		return;
	}
	if (bPredictedPresentation)
	{
		if (!HandRequests.IsEmpty() || ReplicatedPresentation.ActivationKey != LocalPresentation.ActivationKey)
		{
			return;
		}
		if (ReplicatedPresentation.Phase == EHodgeWeaponPresentationPhase::Hand) { return; }
		ClearPresentationTimer();
		bPredictedPresentation = false;
		LocalPresentation = ReplicatedPresentation;
	}
	RefreshPresentationActors();
}

/** 撤销被服务器拒绝的预测手持请求。 */
void UHodgeWeaponInstance::RejectPredictedHandUse(int32 ActivationKey)
{
	// 按服务器权限区分本次处理分支。
	if (!GetPawn() || GetPawn()->HasAuthority()) { return; }
	TArray<FGuid> Handles;
	for (const auto& Entry : HandRequests)
	{
		if (Entry.Value.ActivationKey == ActivationKey) { Handles.Add(Entry.Key); }
	}
	for (const FGuid& Handle : Handles) { ReleaseHandUse(Handle); }
	if (HandRequests.IsEmpty() && LocalPresentation.ActivationKey == ActivationKey)
	{
		ClearPresentationTimer();
		bPredictedPresentation = false;
		LocalPresentation = ReplicatedPresentation;
		RefreshPresentationActors();
	}
}

/** 按当前武器表现状态更新关联Actor。 */
void UHodgeWeaponInstance::RefreshPresentationActors()
{
	if (!PresentationProfile) { return; }
	for (AActor* Actor : GetSpawnedActors())
	{
		if (auto* Visual = Cast<AHodgeWeaponPresentationActor>(Actor)) { Visual->BindWeapon(this); }
	}
}

/** 处理SpawnedActors变化。 */
void UHodgeWeaponInstance::OnSpawnedActorsChanged()
{
	Super::OnSpawnedActorsChanged();
	RefreshPresentationActors();
}

/** 在UObject销毁前清理仍由本对象持有的资源。 */
void UHodgeWeaponInstance::BeginDestroy()
{
	ClearPresentationTimer();
	Super::BeginDestroy();
}


/** 更新所属对象姿势策略。 */
void UHodgeWeaponInstance::UpdateOwnerPosePolicy()
{
	// 按服务器权限区分本次处理分支。
	if (GetPawn() && GetPawn()->HasAuthority() && !HandRequests.IsEmpty())
	{
		if (!CombatPoseLease.IsValid())
		{
			PoseCombat = UHodgeCombatComponentBase::FindCombatComponent(GetPawn());
			if (PoseCombat.IsValid()) { CombatPoseLease = PoseCombat->AcquirePoseLease(); }
		}
	}
	else
	{
		if (PoseCombat.IsValid()) { PoseCombat->ReleasePoseLease(CombatPoseLease); }
		CombatPoseLease.Invalidate();
		PoseCombat.Reset();
	}
}
