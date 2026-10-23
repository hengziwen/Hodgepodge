#include "Combat/HodgeHitReactionMotion.h"
#include "Curves/CurveFloat.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeHitReactionMotion)

/** 预测与校正复制完整父类状态，不能退回单点采样的父类实例。 */
FRootMotionSource* FHodgeRootMotionSource_Knockback::Clone() const
{
	return new FHodgeRootMotionSource_Knockback(*this);
}

/** 原生 RMS 网络序列化按真实结构重建客户端对象。 */
UScriptStruct* FHodgeRootMotionSource_Knockback::GetScriptStruct() const
{
	return StaticStruct();
}

/** 固定分段积分覆盖当前物理步，跨过结束时间时只计算仍有效的面积。 */
void FHodgeRootMotionSource_Knockback::PrepareRootMotion(float SimulationTime, float MovementTickTime,
	const ACharacter& Character, const UCharacterMovementComponent& MoveComponent)
{
	if (!StrengthOverTime || Duration <= 0.f || SimulationTime <= 0.f)
	{
		FRootMotionSource_ConstantForce::PrepareRootMotion(SimulationTime, MovementTickTime, Character, MoveComponent);
		return;
	}
	const float Start = FMath::Clamp(GetTime() / Duration, 0.f, 1.f);
	const float End = FMath::Clamp((GetTime() + SimulationTime) / Duration, 0.f, 1.f);
	constexpr int32 Steps = 16;
	float Area = 0.f;
	for (int32 Index = 0; Index <= Steps; ++Index)
	{
		Area += StrengthOverTime->GetFloatValue(FMath::Lerp(Start, End, float(Index) / Steps)) *
			((Index == 0 || Index == Steps) ? 0.5f : 1.f) * (End - Start) / Steps;
	}
	// 父类保留时间推进、模拟追赶和结束策略；只替换本物理步的平均速度。
	const FVector SavedForce = Force;
	UCurveFloat* Curve = StrengthOverTime;
	Force *= Area * Duration / SimulationTime;
	StrengthOverTime = nullptr;
	FRootMotionSource_ConstantForce::PrepareRootMotion(SimulationTime, MovementTickTime, Character, MoveComponent);
	Force = SavedForce;
	StrengthOverTime = Curve;
}
