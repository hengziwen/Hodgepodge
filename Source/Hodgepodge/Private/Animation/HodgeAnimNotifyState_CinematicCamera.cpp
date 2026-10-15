/** @file HodgeAnimNotifyState_CinematicCamera.cpp @brief 原生窗口只提供配置，不依赖通知Begin或End建立运行时事实。 */
#include "Animation/HodgeAnimNotifyState_CinematicCamera.h"
#include "Data/HodgeCinematicCameraProfile.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAnimNotifyState_CinematicCamera)

FString UHodgeAnimNotifyState_CinematicCamera::GetNotifyName_Implementation() const
{
	return Profile ? FString::Printf(TEXT("Cinematic: %s"), *Profile->GetName()) : TEXT("Cinematic: Select Profile");
}
