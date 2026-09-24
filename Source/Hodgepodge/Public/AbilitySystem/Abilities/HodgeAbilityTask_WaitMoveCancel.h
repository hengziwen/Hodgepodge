/**
 * @file HodgeAbilityTask_WaitMoveCancel.h
 * @brief 等待"移动取消"条件成立：取消窗口已开放 + 玩家确实有移动意图
 *
 * 职责边界（严格）：
 *   - 观察两个信号：ASC 上的**取消窗口标签**（由 UHodgeAbilityTask_PlayTimeline 按区间授予）
 *     与 UHodgeHeroComponent 的**移动意图**（输入层的原始输入量）；
 *   - 两者同时成立时广播 OnMoveCancel **一次**，然后结束自己。
 * 不负责：停止 Montage、结束 Ability、决定"取消之后做什么"。那些属于调用它的 Ability，
 * 而且必须写成可替换的策略点（第一版是 EndAbility，以后连招会变成"转段"）。
 *
 * 三层职责的分工（这就是它存在的理由）：
 *   Timeline      → 允许不允许移动取消（授权，由资产里的 Window 区间决定）
 *   Input / Hero  → 玩家到底有没有想移动（意图）
 *   Attack Ability→ 满足两个条件后这次攻击应该怎么结束（策略）
 *
 * 标签和输入变化立即重判；本地 Tick 补充检查自定义摇杆阈值，避免只监听布尔翻转漏判。
 *
 * ⚠️ 本地控制端语义：移动意图只存在于本地控制的玩家身上，服务器（对远端玩家的 Pawn）
 * 永远看不到它 —— 这不是缺陷：取消由 Ability 的 EndAbility 经 GAS 的预测复制收敛到服务器。
 * 不要为了"两端一致"把移动意图复制上去，那会造出两个决策源。
 * 推论：找不到 HeroComponent（AI、模拟代理）时本任务只订阅标签，条件因此永不成立。
 *
 * 设计依据：Docs/Design/ability-timeline-stage1.md（"移动取消后摇"的消费方示例）
 */

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GameplayTagContainer.h"

#include "HodgeAbilityTask_WaitMoveCancel.generated.h"

class UAbilitySystemComponent;
class UHodgeHeroComponent;

// 移动取消条件成立（窗口开放 + 有移动意图）。监听者通常在回调里结束这次攻击。
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHodgeMoveCancelDelegate);

UCLASS()
class HODGEPODGE_API UHodgeAbilityTask_WaitMoveCancel : public UAbilityTask
{
	GENERATED_BODY()

public:
	UHodgeAbilityTask_WaitMoveCancel(const FObjectInitializer& ObjectInitializer);
	// 条件成立时广播，只会广播一次。
	UPROPERTY(BlueprintAssignable)
	FHodgeMoveCancelDelegate OnMoveCancel;

	/**
	 * 开始等待移动取消。
	 *
	 * @param CancelWindowTag      允许取消的窗口标签（如 Status.Attack.Cancel.Move），由 Timeline 的 Window 授予
	 * @param MoveIntentThreshold  判定"有移动意图"的输入量阈值（摇杆死区）
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks",
		meta = (DisplayName = "Wait Move Cancel", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility",
			BlueprintInternalUseOnly = "TRUE"))
	static UHodgeAbilityTask_WaitMoveCancel* WaitMoveCancel(UGameplayAbility* OwningAbility,
	                                                        FGameplayTag CancelWindowTag,
	                                                        float MoveIntentThreshold = 0.1f);

protected:
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	// 重新判定条件。任一信号变化都会调用它。
	void Evaluate(const TCHAR* Source);

	// 移动意图变化的转发（绑定到 UHodgeHeroComponent::OnMoveIntentChanged）。
	UFUNCTION()
	void HandleMoveIntentChanged(bool bHasMoveIntent);

	// 窗口标签加减的转发。
	void HandleWindowTagChanged(FGameplayTag Tag, int32 NewCount);

	// 允许取消的窗口标签。
	FGameplayTag WindowTag;

	// 移动意图阈值。
	float IntentThreshold = 0.1f;

	// 条件是否已经成立过。成立过就不再判定，也保证不会二次广播。
	bool bSucceeded = false;

	// 是否已经进入 OnDestroy。广播之后用它判断"监听者是不是已经同步结束了 Ability"。
	bool bInOnDestroy = false;

	// 订阅用的弱引用：任务比 ASC / HeroComponent 短命，销毁时必须解绑。
	TWeakObjectPtr<UAbilitySystemComponent> SubscribedASC;
	TWeakObjectPtr<UHodgeHeroComponent> SubscribedHeroComponent;

	// 窗口标签事件的解绑凭据。
	FDelegateHandle WindowTagDelegateHandle;
};
