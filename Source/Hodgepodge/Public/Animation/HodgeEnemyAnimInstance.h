/** @file HodgeEnemyAnimInstance.h
 * @brief 声明怪物移动动画实例相关的类型、接口与现有处理流程。
 */
#pragma once
#include "Animation/HodgeAnimInstance.h"
#include "HodgeEnemyAnimInstance.generated.h"

/** 普通怪物的最小移动表现数据，沿用共用 ASC 与朝向绑定。 */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeEnemyAnimInstance : public UHodgeAnimInstance
{
	GENERATED_BODY()
public:
	// 水平速度是否超过移动表现阈值。
	UPROPERTY(BlueprintReadOnly, Transient, Category="Hodge|Enemy") bool bEnemyMoving = false;
	// 怪物在水平面的实际移动速度。
	UPROPERTY(BlueprintReadOnly, Transient, Category="Hodge|Enemy") float EnemyGroundSpeed = 0.f;
protected:
	/** 在游戏线程采集当前角色的动画表现数据。
	 * @param DeltaSeconds 本次更新的时间步长，单位秒。
	 */
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
