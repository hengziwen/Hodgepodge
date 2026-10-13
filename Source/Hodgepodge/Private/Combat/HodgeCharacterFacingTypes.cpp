/** @file HodgeCharacterFacingTypes.cpp
 * @brief 实现朝向请求与移动意图数据相关的类型、接口与现有处理流程。
 */
#include "Combat/HodgeCharacterFacingTypes.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeCharacterFacingTypes)

/** 校验并归一化方向，输出用于后续方向计算。 */
bool HodgeFacing::NormalizeDirection(const FVector& Input, FVector& Output)
{
	Output = FVector::ZeroVector;
	if (Input.ContainsNaN()) { return false; }
	Output = Input.GetSafeNormal2D();
	return !Output.IsNearlyZero();
}

/** 构造移动意图。 */
FHodgeMoveIntentSnapshot HodgeFacing::MakeMoveIntent(FVector2D Input, float ControlYaw, double Time, int32 Generation)
{
	FHodgeMoveIntentSnapshot Result;
	Result.AvatarGeneration = Generation;
	Result.SampleTime = FMath::IsFinite(Time) ? Time : 0.;
	if (Input.ContainsNaN() || !FMath::IsFinite(ControlYaw)) { return Result; }
	Result.RawInput2D = Input;
	Result.ControllerYawAtSample = FRotator::NormalizeAxis(ControlYaw);
	Result.InputMagnitude = Input.Size();
	// 本次操作所在的世界。
	const FVector World = FRotator(0.f, Result.ControllerYawAtSample, 0.f).RotateVector(FVector(Input.Y, Input.X, 0.f));
	NormalizeDirection(World, Result.DesiredDirectionWorld);
	return Result;
}

/** 判断基础驱动。 */
bool HodgeFacing::IsBaseDriver(EHodgeCharacterFacingDriver Driver)
{
	return Driver == EHodgeCharacterFacingDriver::Movement || Driver == EHodgeCharacterFacingDriver::Controller;
}
