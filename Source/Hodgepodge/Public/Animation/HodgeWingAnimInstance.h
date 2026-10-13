/** @file HodgeWingAnimInstance.h
 * @brief 声明翅膀姿势动画实例相关的类型、接口与现有处理流程。
 */
#pragma once

#include "Animation/AnimInstance.h"
#include "HodgeWingAnimInstance.generated.h"

/** 从协调组件读取旋转；动画图在参考姿势上对三段翼骨骼作局部加法。 */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeWingAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	// 翅膀根节点旋转的几何或变换数据。
	UPROPERTY(BlueprintReadOnly, Transient) FRotator WingRootRotation = FRotator::ZeroRotator;
	// 翅膀中段旋转的几何或变换数据。
	UPROPERTY(BlueprintReadOnly, Transient) FRotator WingMidRotation = FRotator::ZeroRotator;
	// 翅膀末端旋转的几何或变换数据。
	UPROPERTY(BlueprintReadOnly, Transient) FRotator WingTipRotation = FRotator::ZeroRotator;
protected:
	/** 在游戏线程采集当前角色的动画表现数据。
	 * @param DeltaSeconds 本次更新的时间步长，单位秒。
	 */
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
