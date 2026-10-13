/** @file HodgeSprintAbilityProfile.h
 * @brief 声明Dash/Sprint参数配置相关的类型、接口与现有处理流程。
 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "HodgeSprintAbilityProfile.generated.h"

class UAnimMontage;
class UAnimSequence;
class UGameplayEffect;

/** 同一输入的 Dash 与 Sprint 共用不可变配置；没有冷却字段。 */
UCLASS(BlueprintType, Const)
class HODGEPODGE_API UHodgeSprintAbilityProfile : public UDataAsset
{
	GENERATED_BODY()
public:
	/** 仅控制 RMS 位移，不缩放或终止蒙太奇。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash") float Duration = .35f;
	// 距离的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash") float Distance = 280.f;
	// 蒙太奇Play速率的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash|Animation", meta=(ClampMin="0.1")) float MontagePlayRate = 1.f;
	/** 以下窗口均为成功播放后的实际秒数，不包含朝向准备时间。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash|Recovery") bool bAllowMoveCancel = true;
	// 移动取消打开时间的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash|Recovery", meta=(Units="s")) float MoveCancelOpenTime = .35f;
	// Handoff打开时间的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint|Handoff", meta=(Units="s")) float HandoffOpenTime = .35f;
	// Handoff关闭时间的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint|Handoff", meta=(Units="s")) float HandoffCloseTime = 1.2f;
	// Handoff网络Grace的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint|Handoff", meta=(Units="s")) float HandoffNetworkGrace = .5f;
	// 移动意图阈值的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement", meta=(ClampMin="0.01", ClampMax="1")) float MoveIntentThreshold = .1f;
	// 控制是否启用后向Variant的状态标记。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash") bool bEnableBackwardVariant = true;
	// 后向Cone角度的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash", meta=(ClampMin="0", ClampMax="89")) float BackwardConeAngle = 45.f;
	// 即时对齐后同次激活播放蒙太奇，避免锁定侧移时先停步等待转身。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash") bool bInstantFacingAtStart = true;
	// 朝向PreparationTimeout的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash") float FacingPreparationTimeout = .5f;
	// 前向蒙太奇的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash") TObjectPtr<UAnimMontage> ForwardMontage;
	// 后向蒙太奇的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash") TObjectPtr<UAnimMontage> BackwardMontage;
	// 攻击取消窗口的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash") FGameplayTag AttackCancelWindow;
	// Invulnerability起始的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Defense") float InvulnerabilityStart = .04f;
	// Invulnerability结束的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Defense") float InvulnerabilityEnd = .22f;
	// 完美闪避起始的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Defense") float PerfectStart = .04f;
	// 完美闪避结束的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Defense") float PerfectEnd = .10f;
	// 完美闪避Reward效果的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Defense") TSubclassOf<UGameplayEffect> PerfectRewardEffect;
	// 按住阈值的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint") float HoldThreshold = .22f;
	// Sprint奔跑最大速度的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint") float SprintMaxSpeed = 720.f;
	// Sprint奔跑加速度的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint") float SprintAcceleration = 2400.f;
	// Sprint奔跑Braking的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint") float SprintBraking = 2000.f;
	// Sprint奔跑转向速率的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint") float SprintTurnRate = 540.f;
	// No移动意图Grace的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint") float NoMoveIntentGrace = .08f;
	// 阻塞退出Delay的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint") float BlockedExitDelay = .4f;
	// 控制是否AllowSprint奔跑掉头的状态标记。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint") bool bAllowSprintPivot = true;
	// Sprint奔跑循环的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation") TObjectPtr<UAnimSequence> SprintCycle;
	// Sprint奔跑循环Play速率的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation", meta=(ClampMin="0.1", ClampMax="3")) float SprintCyclePlayRate = 1.f;
	/** 循环参考速度来自 Root 轨迹或原地素材的步幅估算，用于匹配胶囊速度。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation", meta=(ClampMin="1", Units="cm/s")) float SprintCycleReferenceSpeed = 720.f;
	// Sprint奔跑循环最小Play速率的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation", meta=(ClampMin="0.01")) float SprintCycleMinPlayRate = .5f;
	// Sprint奔跑循环最大Play速率的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation", meta=(ClampMin="0.01")) float SprintCycleMaxPlayRate = 1.5f;
	// Sprint奔跑转向的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation") TObjectPtr<UAnimSequence> SprintTurn;
	/** 地面掉头使用独立根运动蒙太奇，避免普通加速与脚步轨迹竞争。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint|Pivot") TObjectPtr<UAnimMontage> SprintPivotMontage;
	// 掉头Trigger角度的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint|Pivot", meta=(ClampMin="90", ClampMax="180")) float PivotTriggerAngle = 150.f;
	// 掉头最小速度的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint|Pivot", meta=(ClampMin="0")) float PivotMinimumSpeed = 250.f;
	// 掉头ReentryDelay的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint|Pivot", meta=(ClampMin="0", Units="s")) float PivotReentryDelay = .25f;
	/** 素材时间：制动和反向起步结束后混出，尾部跑步交给循环。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint|Pivot", meta=(ClampMin="0.01", Units="s")) float PivotRecoveryEndTime = .9f;
	// Sprint奔跑转向Play速率的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation", meta=(ClampMin="0.1", ClampMax="3")) float SprintTurnPlayRate = 1.f;
	// Sprint奔跑转向混合输出时间的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation", meta=(ClampMin="0", Units="s")) float SprintTurnBlendOutTime = .12f;
	/** 判断Handoff窗口。
	 * @param DashAge Dash冲刺Age，由调用方传入。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsHandoffWindow(double DashAge) const;
	/** 判断权威Handoff窗口。
	 * @param DashAge Dash冲刺Age，由调用方传入。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsAuthorityHandoffWindow(double DashAge) const;
	/** 检查是否允许移动取消。
	 * @param DashAge Dash冲刺Age，由调用方传入。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool CanMoveCancel(double DashAge) const;
	/** 检查当前配置并收集诊断。
	 * @param Errors 收集配置或执行失败的诊断。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool Validate(TArray<FText>& Errors) const;
#if WITH_EDITOR
	/** 在编辑器校验当前配置并收集诊断。
	 * @param Context 本次操作的上下文。
	 * @return 当前配置的校验结果。
	 */
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
