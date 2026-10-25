// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "TimerManager.h"
#include "HodgeGameplayAbility_AutoRespawn.generated.h"

class AHodgePlayerState;
class UHodgeHealthComponent;
class UHodgePawnData;
class UHodgePawnExtensionComponent;

/** 复用 Lyra 跨 Pawn 被动能力的监听职责，只在服务器调度正式 Hero 的死亡后重生。 */
UCLASS()
class HODGEPODGE_API UHodgeGameplayAbility_AutoRespawn : public UHodgeGameplayAbility
{
	GENERATED_BODY()
public:
	UHodgeGameplayAbility_AutoRespawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
protected:
	/** 基础授予只激活一次；换 Pawn 时由 OnPawnAvatarSet 重新绑定。 */
	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* Event) override;
	virtual void OnPawnAvatarSet() override;
	/** 来源撤销与主动结束都先退休异步请求，避免旧回调生成新身体。 */
	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate, bool bCancelled) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* Info, const FGameplayAbilitySpec& Spec) override;
private:
	friend struct FHodgeAutoRespawnTestAccess;
	/** 弱引用不延长尸体、Controller 或 PlayerState 的生命周期。 */
	TWeakObjectPtr<APawn> BoundAvatar;
	TWeakObjectPtr<AHodgePlayerState> BoundPlayerState;
	TWeakObjectPtr<APlayerController> BoundController;
	TWeakObjectPtr<UHodgeAbilitySystemComponent> BoundASC;
	TWeakObjectPtr<UHodgeHealthComponent> BoundHealth;
	TWeakObjectPtr<UHodgePawnExtensionComponent> BoundExtension;
	TWeakObjectPtr<const UHodgePawnData> BoundData;
	FTimerHandle RespawnTimer;
	/** 同一实例跨 Pawn 存活；代际在解绑前递增，使重入中的旧调用栈失效。 */
	uint64 Generation = 0;
	int32 Attempts = 0;
	int32 MaxAttempts = 0;
	float Delay = 0.f;
	float RetryInterval = 0.f;
	bool bDeathCompleted = false;
	bool bTeardownObserved = false;
	bool bRequestStarted = false;
	void BindAvatar();
	void Unbind();
	/** 死亡完成和旧 Avatar 清理共同开启一次调度。 */
	void TryQueueRespawn();
	void ScheduleAttempt(float Seconds);
	void AttemptRespawn(uint64 ForGeneration);
	bool IsRequestCurrent(uint64 ForGeneration) const;
	/** 动态死亡通知仍核对来源 Actor，拒绝旧 Pawn 的迟到事件。 */
	UFUNCTION() void OnDeathFinished(AActor* Actor);
	UFUNCTION() void OnAvatarEndPlay(AActor* Actor, EEndPlayReason::Type Reason);
	void OnAvatarUninitialized(TWeakObjectPtr<APawn> Avatar);
};
