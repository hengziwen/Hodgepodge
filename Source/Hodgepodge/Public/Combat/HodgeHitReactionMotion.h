/** 在既有恒力 RMS 内积分速度曲线，避免单点采样让击退距离随帧长漂移。 */
#pragma once

#include "GameFramework/RootMotionSource.h"
#include "HodgeHitReactionMotion.generated.h"

USTRUCT()
struct HODGEPODGE_API FHodgeRootMotionSource_Knockback : public FRootMotionSource_ConstantForce
{
	GENERATED_BODY()
	/** CMC 复制、保存帧与校正均保留本类型，其他状态沿用 ConstantForce。 */
	virtual FRootMotionSource* Clone() const override;
	virtual UScriptStruct* GetScriptStruct() const override;
	/** 以当前 SimulationTime 的平均强度交给原生 RMS，不另行移动胶囊。 */
	virtual void PrepareRootMotion(float SimulationTime, float MovementTickTime, const ACharacter& Character,
		const UCharacterMovementComponent& MoveComponent) override;
};

/** 沿用父类网络载荷和拷贝语义，不引入第二份运动时钟。 */
template<>
struct TStructOpsTypeTraits<FHodgeRootMotionSource_Knockback> : TStructOpsTypeTraitsBase2<FHodgeRootMotionSource_Knockback>
{
	enum { WithNetSerializer = true, WithCopy = true };
};
