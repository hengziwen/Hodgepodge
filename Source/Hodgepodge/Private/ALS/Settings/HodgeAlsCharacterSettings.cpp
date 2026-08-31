#include "ALS/Settings/HodgeAlsCharacterSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAlsCharacterSettings)

UHodgeAlsCharacterSettings::UHodgeAlsCharacterSettings()
{
	Mantling.MantlingTraceResponseChannels =
	{
		ECC_WorldStatic,
		ECC_WorldDynamic,
		ECC_Destructible
	};

	Mantling.MantlingTraceResponses.WorldStatic = ECR_Block;
	Mantling.MantlingTraceResponses.WorldDynamic = ECR_Block;
	Mantling.MantlingTraceResponses.Destructible = ECR_Block;
}

#if WITH_EDITOR
void UHodgeAlsCharacterSettings::PostEditChangeProperty(FPropertyChangedEvent& ChangedEvent)
{
	if (ChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_STRING_VIEW_CHECKED(ThisClass, Mantling))
	{
		Mantling.PostEditChangeProperty(ChangedEvent);
	}

	Super::PostEditChangeProperty(ChangedEvent);
}
#endif
