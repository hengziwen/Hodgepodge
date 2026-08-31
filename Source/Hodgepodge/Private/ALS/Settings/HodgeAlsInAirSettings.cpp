#include "ALS/Settings/HodgeAlsInAirSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAlsInAirSettings)

#if WITH_EDITOR
void FHodgeAlsInAirSettings::PostEditChangeProperty(const FPropertyChangedEvent& ChangedEvent)
{
	if (ChangedEvent.GetPropertyName() == GET_MEMBER_NAME_STRING_VIEW_CHECKED(FHodgeAlsInAirSettings, GroundPredictionResponseChannels))
	{
		GroundPredictionSweepResponses.SetAllChannels(ECR_Ignore);

		for (const auto CollisionChannel : GroundPredictionResponseChannels)
		{
			GroundPredictionSweepResponses.SetResponse(CollisionChannel, ECR_Block);
		}
	}
}
#endif
