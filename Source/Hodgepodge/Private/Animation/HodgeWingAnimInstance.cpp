/** @file HodgeWingAnimInstance.cpp
 * @brief 实现翅膀姿势动画实例相关的类型、接口与现有处理流程。
 */
#include "Animation/HodgeWingAnimInstance.h"
#include "Component/HodgeWingComponent.h"
#include "GameFramework/Actor.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeWingAnimInstance)

/** 在游戏线程采集当前角色的动画表现数据。 */
void UHodgeWingAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	const auto* Wing = GetOwningActor() ? GetOwningActor()->FindComponentByClass<UHodgeWingComponent>() : nullptr;
	WingRootRotation = Wing ? Wing->Pose.Root : FRotator::ZeroRotator;
	WingMidRotation = Wing ? Wing->Pose.Mid : FRotator::ZeroRotator;
	WingTipRotation = Wing ? Wing->Pose.Tip : FRotator::ZeroRotator;
}
