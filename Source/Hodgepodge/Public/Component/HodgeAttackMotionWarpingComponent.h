/** @file HodgeAttackMotionWarpingComponent.h @brief 为获准攻击提供根运动目标，不管理选敌或伤害。 */
#pragma once
#include "MotionWarpingComponent.h"
#include "HodgeAttackMotionWarpingComponent.generated.h"
struct FHodgeCharacterRotationState;
class UAnimMontage;

/** 同时支持正常求值与 SavedMove 回放，清理只作用于本组件的攻击目标。 */
UCLASS(ClassGroup=Movement, meta=(BlueprintSpawnableComponent))
class HODGEPODGE_API UHodgeAttackMotionWarpingComponent : public UMotionWarpingComponent
{
	GENERATED_BODY()
public:
	virtual void InitializeComponent() override;
	/** GA 暂停、取消或更换执行后立即移除本次攻击辅助。 */
	void ClearAttackWarp();
	/** 用本次快照准备目标；正常移动记录实际Yaw，SavedMove回放只消费历史输入。 */
	void PrepareAttackTarget(const FHodgeCharacterRotationState& State, const UAnimMontage* Montage, float Position);
	void ClearAttackWarpForRequest(FGuid Request) { if (EvaluationRequest==Request) { ClearAttackWarp(); } }
private:
	UFUNCTION() void PrepareAttackEvaluation(UMotionWarpingComponent* Component);
	FGuid EvaluationRequest;
	FName OwnedTargetName;
	float PreviousPosition = -1.f;
};
