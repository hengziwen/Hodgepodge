/** @file HodgeResonanceSet.h @brief PlayerState持有的共鸣解放充能属性，独立于生命和冷却。 */
#pragma once
#include "AbilitySystem/AttributeSet/HodgeAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"
#include "HodgeResonanceSet.generated.h"

/** 原生标签只有一个事实来源，资产无需另建同义标签。 */
namespace HodgeResonanceTags
{
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(EnergyDelta);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown);
}

/** 能量只通过GAS属性修改，默认等待初始化GE提供正容量。 */
UCLASS(BlueprintType)
class HODGEPODGE_API UHodgeResonanceSet : public UHodgeAttributeSet
{
	GENERATED_BODY()
public:
	/** 新PlayerState从零能量开始，不在重新绑定Pawn时重置。 */
	UHodgeResonanceSet();
	ATTRIBUTE_ACCESSORS(UHodgeResonanceSet, ResonanceEnergy);
	ATTRIBUTE_ACCESSORS(UHodgeResonanceSet, MaxResonanceEnergy);
	/** 当前属性有限且容量为正才表示资源可用。 */
	bool IsResourceReady() const;
	/** 复制通知沿用Lyra的GAS聚合器修正路径。 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	/** 基础值写入前限制能量与容量。 */
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	/** 最终值和预测聚合值也受同样范围约束。 */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	/** 降低容量时同时压低已有能量，不补充能量。 */
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
private:
	/** 复制给拥有者；旁观客户端无需读取玩家的个人充能。 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_ResonanceEnergy, Category="Resonance", meta=(AllowPrivateAccess="true")) FGameplayAttributeData ResonanceEnergy;
	/** 容量由初始化GE配置，不与攻击伤害倍率绑定。 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxResonanceEnergy, Category="Resonance", meta=(AllowPrivateAccess="true")) FGameplayAttributeData MaxResonanceEnergy;
	/** 修正本地预测并触发ASC属性变化委托。 */
	UFUNCTION() void OnRep_ResonanceEnergy(const FGameplayAttributeData& Old);
	UFUNCTION() void OnRep_MaxResonanceEnergy(const FGameplayAttributeData& Old);
	/** 非有限数值安全归零，容量为零时禁止充能和释放。 */
	void Clamp(const FGameplayAttribute& Attribute, float& Value) const;
};
