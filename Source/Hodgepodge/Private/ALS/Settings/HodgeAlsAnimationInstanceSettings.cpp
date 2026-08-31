#include "ALS/Settings/HodgeAlsAnimationInstanceSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAlsAnimationInstanceSettings)

UHodgeAlsAnimationInstanceSettings::UHodgeAlsAnimationInstanceSettings()
{
	InAir.GroundPredictionResponseChannels =
	{
		ECC_WorldStatic,
		ECC_WorldDynamic,
		ECC_Destructible
	};

	InAir.GroundPredictionSweepResponses.WorldStatic = ECR_Block;
	InAir.GroundPredictionSweepResponses.WorldDynamic = ECR_Block;
	InAir.GroundPredictionSweepResponses.Destructible = ECR_Block;
}

#if WITH_EDITOR
void UHodgeAlsAnimationInstanceSettings::PostEditChangeProperty(FPropertyChangedEvent& ChangedEvent)
{
	if (ChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_STRING_VIEW_CHECKED(ThisClass, InAir))
	{
		InAir.PostEditChangeProperty(ChangedEvent);
	}

	Super::PostEditChangeProperty(ChangedEvent);
}
#endif
