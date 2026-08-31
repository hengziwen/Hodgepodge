#include "ALS/HodgeAlsAnimationInstanceProxy.h"

#include "ALS/HodgeAlsAnimationInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAlsAnimationInstanceProxy)

FHodgeAlsAnimationInstanceProxy::FHodgeAlsAnimationInstanceProxy(UAnimInstance* AnimationInstance): FAnimInstanceProxy{AnimationInstance} {}

void FHodgeAlsAnimationInstanceProxy::PostUpdate(UAnimInstance* AnimationInstance) const
{
	FAnimInstanceProxy::PostUpdate(AnimationInstance);

	// Epic does not allow to override the UAnimInstance::PostUpdateAnimation()
	// function in child classes, so we have to resort to this workaround.

	auto* HodgeAlsAnimationInstance{Cast<UHodgeAlsAnimationInstance>(AnimationInstance)};
	if (IsValid(HodgeAlsAnimationInstance))
	{
		HodgeAlsAnimationInstance->NativePostUpdateAnimation();
	}
}
