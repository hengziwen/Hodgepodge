/** @file HodgeAnimNotifyState_CinematicCamera.h @brief 用原生蒙太奇通知条定义电影镜头窗口。 */
#pragma once
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "HodgeAnimNotifyState_CinematicCamera.generated.h"
class UHodgeCinematicCameraProfile;

/** 通知仅保存配置，角色会话由Hero按实际蒙太奇位置协调。 */
UCLASS(meta=(DisplayName="Hodge Cinematic Camera Window"))
class HODGEPODGE_API UHodgeAnimNotifyState_CinematicCamera : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	/** 电影镜头分镜和安全参数，缺失时保留正常游戏镜头。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Camera") TObjectPtr<UHodgeCinematicCameraProfile> Profile;
	/** 用于蒙太奇编辑器识别实际引用的电影配置。 */
	virtual FString GetNotifyName_Implementation() const override;
};
