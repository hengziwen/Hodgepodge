/** @file HodgeEquipmentStatProfile.h
 * @brief 声明装备等级属性配置相关的类型、接口与现有处理流程。
 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ScalableFloat.h"
#include "HodgeEquipmentStatProfile.generated.h"

class UGameplayEffect;

/** 每件装备按自身等级计算的可撤销加成。 */
UCLASS(BlueprintType, Const)
class HODGEPODGE_API UHodgeEquipmentStatProfile : public UDataAsset
{
	GENERATED_BODY()
public:
	/** 构造装备等级属性配置并初始化其默认状态。 */
	UHodgeEquipmentStatProfile();
	// 最小等级的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="1000")) int32 MinLevel = 1;
	// 最大等级的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="1000")) int32 MaxLevel = 90;
	// 最大生命值Bonus的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FScalableFloat MaxHealthBonus = FScalableFloat(0.f);
	// 基础伤害Bonus的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FScalableFloat BaseDamageBonus = FScalableFloat(0.f);
	// 属性效果的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSubclassOf<UGameplayEffect> AttributeEffect;
	/** 计算当前配置结果。
	 * @param Level 配置或运行实例的等级。
	 * @param Health 当前生命值。
	 * @param Damage 伤害，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool Evaluate(int32 Level, float& Health, float& Damage) const;
	/** 检查当前配置并收集诊断。
	 * @param Errors 收集配置或执行失败的诊断。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool Validate(TArray<FText>& Errors) const;
#if WITH_EDITOR
	/** 在编辑器校验当前配置并收集诊断。
	 * @param Context 本次操作的上下文。
	 * @return 当前配置的校验结果。
	 */
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
