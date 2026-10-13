/** @file HodgeWeaponPresentationProfile.cpp
 * @brief 实现武器显隐与手持配置相关的类型、接口与现有处理流程。
 */
#include "Equipment/HodgeWeaponPresentationProfile.h"
#include "Engine/SkeletalMesh.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeWeaponPresentationProfile)

/** 检查当前配置并收集诊断。 */
bool UHodgeWeaponPresentationProfile::Validate(TArray<FText>& Errors) const
{
	const int32 Before = Errors.Num();
	if (!WeaponMesh || VisibilityParameter.IsNone()) { Errors.Add(FText::FromString(TEXT("WeaponMesh and VisibilityParameter are required"))); }
	if (BackSocket.IsNone()) { Errors.Add(FText::FromString(TEXT("BackSocket must name a socket on the character Mesh"))); }
	for (float Value : {DrawSeconds, ReturnGraceSeconds, HoverSeconds, HoverAmplitude, HoverFrequency})
	{
		if (!FMath::IsFinite(Value) || Value < 0.f) { Errors.Add(FText::FromString(TEXT("Presentation values must be finite and nonnegative"))); }
	}
	if (!FMath::IsFinite(ReturnSeconds) || ReturnSeconds <= 0.f || !FMath::IsFinite(FadeSeconds) || FadeSeconds <= 0.f
		|| HandOffset.ContainsNaN() || BackTransform.ContainsNaN() || ReturnArcOffset.ContainsNaN())
	{
		Errors.Add(FText::FromString(TEXT("Invalid presentation transform or transition duration")));
	}
	return Before == Errors.Num();
}
#if WITH_EDITOR
/** 在编辑器校验当前配置并收集诊断。 */
EDataValidationResult UHodgeWeaponPresentationProfile::IsDataValid(FDataValidationContext& Context) const
{
	TArray<FText> Errors;
	Validate(Errors);
	for (const FText& Error : Errors) { Context.AddError(Error); }
	return Errors.IsEmpty() ? EDataValidationResult::Valid : EDataValidationResult::Invalid;
}
#endif
