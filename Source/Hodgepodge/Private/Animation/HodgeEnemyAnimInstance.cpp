/** @file HodgeEnemyAnimInstance.cpp
 * @brief 实现怪物移动动画实例相关的类型、接口与现有处理流程。
 */
#include "Animation/HodgeEnemyAnimInstance.h"
#include "GameFramework/Pawn.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeEnemyAnimInstance)

/** 在游戏线程采集当前角色的动画表现数据。 */
void UHodgeEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	// 当前操作关联的所属对象。
	const APawn* Owner = TryGetPawnOwner();
	EnemyGroundSpeed = Owner ? Owner->GetVelocity().Size2D() : 0.f;
	bEnemyMoving = EnemyGroundSpeed > 5.f;
}
