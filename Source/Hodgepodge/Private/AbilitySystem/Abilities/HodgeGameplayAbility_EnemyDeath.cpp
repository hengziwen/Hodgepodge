/** @file HodgeGameplayAbility_EnemyDeath.cpp
 * @brief 实现玩法能力敌人死亡相关的类型、接口与现有处理流程。
 */
#include "AbilitySystem/Abilities/HodgeGameplayAbility_EnemyDeath.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Component/HodgeCombatComponentBase.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayAbility_EnemyDeath)

/** 启动本次技能，按现有配置提交消耗并建立执行资源。 */
void UHodgeGameplayAbility_EnemyDeath::ActivateAbility(FGameplayAbilitySpecHandle Handle,
                                                       const FGameplayAbilityActorInfo* Info,
                                                       FGameplayAbilityActivationInfo ActivationInfo,
                                                       const FGameplayEventData* Event)
{
	Super::ActivateAbility(Handle, Info, ActivationInfo, Event);
	// 根据当前生命周期状态决定是否继续执行。
	if (!IsActive()) { return; }
	Combat = UHodgeCombatComponentBase::FindCombatComponent(GetAvatarActorFromActorInfo());
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (Combat.IsValid()) { PoseLease = Combat->AcquirePoseLease(); }
	if (!DeathMontage)
	{
		FinishMontage();
		return;
	}
	Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, DeathMontage, 1.f, NAME_None, true);
	Task->OnCompleted.AddDynamic(this, &ThisClass::FinishMontage);
	Task->OnInterrupted.AddDynamic(this, &ThisClass::FinishMontage);
	Task->OnCancelled.AddDynamic(this, &ThisClass::FinishMontage);
	Task->ReadyForActivation();
	if (IsActive() && !DeathMontage->bEnableAutoBlendOut)
	{
		GetWorld()->GetTimerManager().SetTimer(HeldPoseTimer, this, &ThisClass::CheckHeldDeathPose, .05f, true);
	}
}

/** 检查按住死亡姿势。 */
void UHodgeGameplayAbility_EnemyDeath::CheckHeldDeathPose()
{
	const auto* Info = GetCurrentActorInfo();
	// 当前网格使用的动画实例。
	auto* Anim = Info ? Info->GetAnimInstance() : nullptr;
	const auto* Instance = Anim && DeathMontage ? Anim->GetActiveInstanceForMontage(DeathMontage) : nullptr;
	// 末帧保持不会发出蒙太奇完成事件，以实际播放进度结束死亡生命周期。
	if (!Instance || (!Instance->IsPlaying() && Instance->GetPosition() >= DeathMontage->GetPlayLength() - .001f))
	{
		FinishMontage();
	}
}

/** 结束蒙太奇。 */
void UHodgeGameplayAbility_EnemyDeath::FinishMontage()
{
	// 根据当前生命周期状态决定是否继续执行。
	if (IsActive())
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
	}
}

/** 结束本次技能并释放其拥有的资源，再交还GAS生命周期。 */
void UHodgeGameplayAbility_EnemyDeath::EndAbility(FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* Info,
                                                  FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate,
                                                  bool bCancelled)
{
	if (GetWorld()) { GetWorld()->GetTimerManager().ClearTimer(HeldPoseTimer); }
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (Combat.IsValid()) { Combat->ReleasePoseLease(PoseLease); }
	PoseLease.Invalidate();
	Combat.Reset();
	Super::EndAbility(Handle, Info, ActivationInfo, bReplicate, bCancelled);
}
