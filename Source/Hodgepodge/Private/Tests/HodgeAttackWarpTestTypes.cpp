/** @file HodgeAttackWarpTestTypes.cpp @brief 仅隔离动画启动，保留真实 GAS 的活动身份和来源。 */
#include "HodgeAttackWarpTestTypes.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAttackWarpTestTypes)
bool UHodgeAttackWarpTestAbility::CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
	const FGameplayTagContainer* Source, const FGameplayTagContainer* Target, FGameplayTagContainer* Relevant) const
{ return UGameplayAbility::CanActivateAbility(Handle,Info,Source,Target,Relevant); }
void UHodgeAttackWarpTestAbility::ActivateAbility(FGameplayAbilitySpecHandle, const FGameplayAbilityActorInfo*, FGameplayAbilityActivationInfo, const FGameplayEventData*) {}

/** 不在朝向协议测试中额外要求装备和检测来源，播放仍由 Definition 负责。 */
void UHodgeAttackWarpTestAbility::OnExecutionReady() {}

/** 默认点沿目标位置移动，备用点刻意偏离以暴露预测点混入。 */
bool AHodgeAttackWarpTestTarget::GetLockPoint_Implementation(FName Point, FVector& OutLocation) const
{
	if (!Point.IsNone() && Point != TEXT("Alternate")) { return false; }
	OutLocation = GetActorLocation() + (Point.IsNone() ? FVector::ZeroVector : FVector(0.f, 500.f, 0.f));
	return !OutLocation.ContainsNaN();
}

/** 保留 PlayerController 身份，只隔离独立测试世界自动认定本地玩家的条件。 */
bool AHodgeDeferredFacingTestController::IsLocalController() const
{
	return false;
}
