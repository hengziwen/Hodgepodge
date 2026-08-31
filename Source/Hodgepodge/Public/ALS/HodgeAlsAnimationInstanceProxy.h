#pragma once

#include "Animation/AnimInstanceProxy.h"
#include "HodgeAlsAnimationInstanceProxy.generated.h"

USTRUCT()
struct HODGEPODGE_API FHodgeAlsAnimationInstanceProxy : public FAnimInstanceProxy
{
	GENERATED_BODY()

public:
	FHodgeAlsAnimationInstanceProxy() = default;

	explicit FHodgeAlsAnimationInstanceProxy(UAnimInstance* AnimationInstance);

protected:
	virtual void PostUpdate(UAnimInstance* AnimationInstance) const override;
};
