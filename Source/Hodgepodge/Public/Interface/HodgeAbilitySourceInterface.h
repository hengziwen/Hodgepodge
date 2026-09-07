// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"

#include "HodgeAbilitySourceInterface.generated.h"

class UObject;
class UPhysicalMaterial;
struct FGameplayTagContainer;

/**
 * 能力效果来源接口：用于描述能够为 Ability Effect 计算提供额外修正的对象。
 * 例如武器、技能、投射物等都可以实现该接口，根据距离、物理材质等因素计算效果衰减。
 */
UINTERFACE()
class UHodgeAbilitySourceInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 能力效果来源接口的 C++ 实际实现部分。
 * 只定义计算规则，不关心具体由什么对象实现。
 */
class IHodgeAbilitySourceInterface
{
	GENERATED_IINTERFACE_BODY()
	/**
	 * 根据距离计算 Ability 效果的衰减倍率。
	 *
	 * @param Distance       Source 到 Target 的距离，例如枪械子弹飞行的距离。
	 * @param SourceTags     Ability Source 当前聚合得到的 GameplayTags，可用于判断来源状态。
	 * @param TargetTags     Target 当前拥有的 GameplayTags，可用于判断目标状态。
	 *
	 * @return 根据距离计算得到的倍率，通常会与基础属性值相乘。
	 */
	virtual float GetDistanceAttenuation(float Distance, const FGameplayTagContainer* SourceTags = nullptr,
	                                     const FGameplayTagContainer* TargetTags = nullptr) const = 0;

	/**
	 * 根据目标物理材质计算 Ability 效果的衰减倍率。
	 *
	 * @param PhysicalMaterial Target 当前命中的物理材质，例如金属、木材、角色皮肤等。
	 * @param SourceTags       Ability Source 当前聚合得到的 GameplayTags。
	 * @param TargetTags       Target 当前拥有的 GameplayTags。
	 *
	 * @return 根据物理材质计算得到的倍率，通常用于调整最终伤害或其他 Ability 效果。
	 */
	virtual float GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial,
	                                             const FGameplayTagContainer* SourceTags = nullptr,
	                                             const FGameplayTagContainer* TargetTags = nullptr) const = 0;
};
