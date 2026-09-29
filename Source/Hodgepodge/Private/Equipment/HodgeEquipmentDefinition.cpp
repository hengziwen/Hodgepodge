// Copyright Epic Games, Inc. All Rights Reserved.

#include "Equipment/HodgeEquipmentDefinition.h"

// 引入装备运行时实例类，用于设置默认的 InstanceType。
#include "Equipment/HodgeEquipmentInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeEquipmentDefinition)

// 构造装备定义对象，并设置默认的装备运行时实例类型。
UHodgeEquipmentDefinition::UHodgeEquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 默认使用基础 UHodgeEquipmentInstance，具体装备定义可以配置为其派生类型。
	InstanceType = UHodgeEquipmentInstance::StaticClass();
}
