// Copyright Epic Games, Inc. All Rights Reserved.

// 项目输入系统的可映射按键 Profile。
#pragma once

// 引入 Enhanced Input 的玩家可映射按键配置基类。
#include "UserSettings/EnhancedInputUserSettings.h"

// 当前类的 UE 反射代码生成头文件。
#include "HodgePlayerMappableKeyProfile.generated.h"

// 项目自定义的玩家可映射按键配置 Profile。
UCLASS()
class HODGEPODGE_API UHodgePlayerMappableKeyProfile : public UEnhancedPlayerMappableKeyProfile
{
	GENERATED_BODY()

protected:
	//~ Begin UEnhancedPlayerMappableKeyProfile interface
	// 装备当前按键 Profile，将该 Profile 设置为当前生效的配置。
	virtual void EquipProfile() override;

	// 卸载当前按键 Profile，取消该 Profile 的生效状态。
	virtual void UnEquipProfile() override;
	//~ End UEnhancedPlayerMappableKeyProfile interface
};
