/** @file HodgeHavocSet.h @brief 共鸣回路暗流资源；与共鸣解放能量独立。 */
#pragma once
#include "AbilitySystem/AttributeSet/HodgeAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"
#include "HodgeHavocSet.generated.h"

/** 标签由原生注册，蓝图与伤害执行共用同一身份。 */
namespace HodgeHavocTags
{
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(DarkFlowDelta);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(DarkSurge);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HavocElement);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CircuitInput);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CircuitAbility);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CastCommitted);
}

/** PlayerState持有资源，初始化GE只设置容量，不在重复绑定时补满。 */
UCLASS(BlueprintType)
class HODGEPODGE_API UHodgeHavocSet : public UHodgeAttributeSet
{
	GENERATED_BODY()
public:
	/** 默认零容量表示尚未完成资源初始化。 */
	UHodgeHavocSet();
	ATTRIBUTE_ACCESSORS(UHodgeHavocSet, DarkFlow);
	ATTRIBUTE_ACCESSORS(UHodgeHavocSet, MaxDarkFlow);
	/** 有限正容量才允许充能或尝试入态。 */
	bool IsResourceReady() const;
	/** 拥有者复制沿用Lyra属性聚合器通知。 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	/** 约束基础值及聚合值，拒绝NaN和越界燃料。 */
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	/** 降低容量不允许保留超过容量的燃料。 */
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
private:
	/** 实际燃料是显示及归零退出的唯一数值来源。 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_DarkFlow, meta=(AllowPrivateAccess="true")) FGameplayAttributeData DarkFlow;
	/** 容量由初始化GE配置。 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxDarkFlow, meta=(AllowPrivateAccess="true")) FGameplayAttributeData MaxDarkFlow;
	/** 接收服务器数据后修正预测聚合器。 */
	UFUNCTION() void OnRep_DarkFlow(const FGameplayAttributeData& Old);
	UFUNCTION() void OnRep_MaxDarkFlow(const FGameplayAttributeData& Old);
	/** 只处理数值约束，不在属性预处理里移除GE。 */
	void Clamp(const FGameplayAttribute& Attribute, float& Value) const;
};
