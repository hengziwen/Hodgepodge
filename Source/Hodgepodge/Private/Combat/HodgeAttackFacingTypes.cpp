/** @file HodgeAttackFacingTypes.cpp @brief 固定攻击方向的边界检查和 GAS 序列化。 */
#include "Combat/HodgeAttackFacingTypes.h"
#include "Engine/PackageMapClient.h"
#include "GameFramework/Actor.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAttackFacingTypes)

bool FHodgeAttackFacingConfig::Validate(FString& Error) const
{
	// 固定方向与确认目标追踪共用同一组角度、速率和通知窗口约束。
	if (!bEnabled) { Error.Reset(); return true; }
	const float Positive[] = {MaxCorrectionAngle,MaxRotationRate,RotationTimeMultiplier,TargetDataTimeout};
	for (float V : Positive) { if (!FMath::IsFinite(V) || V <= 0.f) { Error=TEXT("Attack facing thresholds must be finite and positive."); return false; } }
	if (WarpTargetName.IsNone() || MaxCorrectionAngle >= 180.f || !FMath::IsFinite(StartYawTolerance) || StartYawTolerance<0.f || StartYawTolerance>90.f || !FMath::IsFinite(TargetYawTolerance) || TargetYawTolerance<0.f || TargetYawTolerance>90.f)
	{ Error=TEXT("Attack facing target name or angular bounds are invalid."); return false; }
	Error.Reset(); return true;
}
bool HodgeAttackFacing::ResolveYaw(float StartYaw, float DesiredYaw, float MaxAngle, float& OutYaw)
{
	if (!FMath::IsFinite(StartYaw) || !FMath::IsFinite(DesiredYaw) || !FMath::IsFinite(MaxAngle) || MaxAngle<=0.f || MaxAngle>=180.f) { return false; }
	OutYaw=FRotator::NormalizeAxis(StartYaw+FMath::Clamp(FMath::FindDeltaAngleDegrees(StartYaw,DesiredYaw),-MaxAngle,MaxAngle)); return true;
}
bool FHodgeAttackFacingTargetData::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	if (!Map) { bOutSuccess=false; return false; }
	UObject* Avatar=SourceAvatar.Get(); UObject* Target=LockedTarget.Get();
	const bool bAvatarMapped=Map->SerializeObject(Ar,AActor::StaticClass(),Avatar);
	const bool bTargetMapped=Map->SerializeObject(Ar,AActor::StaticClass(),Target);
	uint8 Kind=static_cast<uint8>(Source); Ar << StartYaw << DesiredYaw << Kind;
	if (Ar.IsLoading()) { SourceAvatar=Cast<AActor>(Avatar); LockedTarget=Cast<AActor>(Target); Source=static_cast<EHodgeAttackFacingSource>(Kind); }
	bOutSuccess=!Ar.IsError(); return bAvatarMapped && bTargetMapped;
}
