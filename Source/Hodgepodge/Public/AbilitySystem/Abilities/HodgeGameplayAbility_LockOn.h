/** @file HodgeGameplayAbility_LockOn.h @brief GAS 预测命令传输，不拥有持续锁定。 */
#pragma once
#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "HodgeGameplayAbility_LockOn.generated.h"
class UHodgeLockOnComponent;
/** 每次激活仅处理一条 TargetData，服务器业务处理完成才复制能力结束。 */
UCLASS()
class HODGEPODGE_API UHodgeGameplayAbility_LockOn : public UHodgeGameplayAbility
{
	GENERATED_BODY()
public:
	UHodgeGameplayAbility_LockOn();
	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* RelevantTags = nullptr) const override;
protected:
	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* Event) override;
	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate, bool bCancelled) override;
private:
	/** 必须先复制载荷再消费 GAS 缓存，防止回调引用失效。 */
	void OnTargetData(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag Tag);
	/** 缺失数据或回执的有限等待，不无限维持活动命令。 */
	void OnTimeout();
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeLockOnComponent> Component;
	FDelegateHandle DataHandle;
	FTimerHandle TimeoutHandle;
	int32 Sequence = 0;
	// 防止同 Pawn 的 ASC 重绑后，旧激活回调影响新生命周期。
	int32 ComponentGeneration = 0;
};
