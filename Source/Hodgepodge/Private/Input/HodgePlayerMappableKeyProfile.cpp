// Copyright Epic Games, Inc. All Rights Reserved.

// 引入项目自定义的玩家可映射按键 Profile。
#include "Input/HodgePlayerMappableKeyProfile.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgePlayerMappableKeyProfile)

// 装备当前按键 Profile 时调用。
void UHodgePlayerMappableKeyProfile::EquipProfile()
{
	// 先执行父类的 Profile 装备逻辑。
	Super::EquipProfile();

	// Do anything you may want to when a new key profile is equipped
	// 可以在这里添加按键 Profile 装备完成后的自定义逻辑。
}

// 卸载当前按键 Profile 时调用。
void UHodgePlayerMappableKeyProfile::UnEquipProfile()
{
	// 先执行父类的 Profile 卸载逻辑。
	Super::UnEquipProfile();

	// Do anything you may want to when a new key profile is unequipped
	// 可以在这里添加按键 Profile 卸载完成后的自定义逻辑。
}
