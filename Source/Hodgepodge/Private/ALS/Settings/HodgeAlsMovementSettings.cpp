#include "ALS/Settings/HodgeAlsMovementSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAlsMovementSettings)

#if WITH_EDITOR
void UHodgeAlsMovementSettings::PostEditChangeProperty(FPropertyChangedEvent& ChangedEvent)
{
	if (ChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_STRING_VIEW_CHECKED(ThisClass, VelocityAngleToSpeedInterpolationRange))
	{
		VelocityAngleToSpeedInterpolationRange.Y = FMath::Max(VelocityAngleToSpeedInterpolationRange.X,
		                                                      VelocityAngleToSpeedInterpolationRange.Y);
	}

	Super::PostEditChangeProperty(ChangedEvent);
}
#endif
