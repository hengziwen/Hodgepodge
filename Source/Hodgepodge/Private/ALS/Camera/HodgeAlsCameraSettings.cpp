#include "ALS/Camera/HodgeAlsCameraSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAlsCameraSettings)

#if WITH_EDITORONLY_DATA
void UHodgeAlsCameraSettings::Serialize(FArchive& Archive)
{
	UObject::Serialize(Archive);

	if (Archive.IsLoading())
	{
		PostProcess.OnAfterLoad();
	}
}
#endif
