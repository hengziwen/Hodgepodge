#include "ALS/Settings/HodgeAlsMantlingSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAlsMantlingSettings)

#if WITH_EDITOR
void FHodgeAlsGeneralMantlingSettings::PostEditChangeProperty(const FPropertyChangedEvent& ChangedEvent)
{
	if (ChangedEvent.GetPropertyName() == GET_MEMBER_NAME_STRING_VIEW_CHECKED(FHodgeAlsGeneralMantlingSettings, SlopeAngleThreshold))
	{
		SlopeAngleThresholdCos = FMath::Cos(FMath::DegreesToRadians(SlopeAngleThreshold));
	}
	else if (ChangedEvent.GetPropertyName() ==
	         GET_MEMBER_NAME_STRING_VIEW_CHECKED(FHodgeAlsGeneralMantlingSettings, MantlingTraceResponseChannels))
	{
		MantlingTraceResponses.SetAllChannels(ECR_Ignore);

		for (const auto CollisionChannel : MantlingTraceResponseChannels)
		{
			MantlingTraceResponses.SetResponse(CollisionChannel, ECR_Block);
		}
	}
}
#endif
