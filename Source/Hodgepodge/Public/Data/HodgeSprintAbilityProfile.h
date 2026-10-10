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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash") float Distance = 280.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash|Animation", meta=(ClampMin="0.1")) float MontagePlayRate = 1.f;
	/** 以下窗口均为成功播放后的实际秒数，不包含朝向准备时间。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash|Recovery") bool bAllowMoveCancel = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash|Recovery", meta=(Units="s")) float MoveCancelOpenTime = .35f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint|Handoff", meta=(Units="s")) float HandoffOpenTime = .35f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint|Handoff", meta=(Units="s")) float HandoffCloseTime = 1.2f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint|Handoff", meta=(Units="s")) float HandoffNetworkGrace = .5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement", meta=(ClampMin="0.01", ClampMax="1")) float MoveIntentThreshold = .1f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash") bool bEnableBackwardVariant = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash", meta=(ClampMin="0", ClampMax="89")) float BackwardConeAngle = 45.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash") bool bInstantFacingAtStart = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash") float FacingPreparationTimeout = .5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash") TObjectPtr<UAnimMontage> ForwardMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash") TObjectPtr<UAnimMontage> BackwardMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash") FGameplayTag AttackCancelWindow;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Defense") float InvulnerabilityStart = .04f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Defense") float InvulnerabilityEnd = .22f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Defense") float PerfectStart = .04f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Defense") float PerfectEnd = .10f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Defense") TSubclassOf<UGameplayEffect> PerfectRewardEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint") float HoldThreshold = .22f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint") float SprintMaxSpeed = 720.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint") float SprintAcceleration = 2400.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint") float SprintBraking = 2000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint") float SprintTurnRate = 540.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint") float NoMoveIntentGrace = .08f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint") float BlockedExitDelay = .4f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint") bool bAllowSprintPivot = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation") TObjectPtr<UAnimSequence> SprintCycle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation", meta=(ClampMin="0.1", ClampMax="3")) float SprintCyclePlayRate = 1.f;
	/** 循环参考速度来自 Root 轨迹或原地素材的步幅估算，用于匹配胶囊速度。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation", meta=(ClampMin="1", Units="cm/s")) float SprintCycleReferenceSpeed = 720.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation", meta=(ClampMin="0.01")) float SprintCycleMinPlayRate = .5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation", meta=(ClampMin="0.01")) float SprintCycleMaxPlayRate = 1.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation") TObjectPtr<UAnimSequence> SprintTurn;
	/** 地面掉头使用独立根运动蒙太奇，避免普通加速与脚步轨迹竞争。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint|Pivot") TObjectPtr<UAnimMontage> SprintPivotMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint|Pivot", meta=(ClampMin="90", ClampMax="180")) float PivotTriggerAngle = 150.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint|Pivot", meta=(ClampMin="0")) float PivotMinimumSpeed = 250.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint|Pivot", meta=(ClampMin="0", Units="s")) float PivotReentryDelay = .25f;
	/** 素材时间：制动和反向起步结束后混出，尾部跑步交给循环。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint|Pivot", meta=(ClampMin="0.01", Units="s")) float PivotRecoveryEndTime = .9f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation", meta=(ClampMin="0.1", ClampMax="3")) float SprintTurnPlayRate = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation", meta=(ClampMin="0", Units="s")) float SprintTurnBlendOutTime = .12f;
	bool IsHandoffWindow(double DashAge) const;
	bool IsAuthorityHandoffWindow(double DashAge) const;
	bool CanMoveCancel(double DashAge) const;
	bool Validate(TArray<FText>& Errors) const;
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
