/** @file HodgeCharacterStatProfile.h
 * @brief 声明角色等级属性曲线配置相关的类型、接口与现有处理流程。
 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ScalableFloat.h"
#include "HodgeCharacterStatProfile.generated.h"

class UGameplayEffect;

/** 角色等级到固有基础属性的不可变映射。 */
UCLASS(BlueprintType, Const)
class HODGEPODGE_API UHodgeCharacterStatProfile : public UDataAsset
{
	GENERATED_BODY()
public:
	/** 构造角色等级属性曲线配置并初始化其默认状态。 */
	UHodgeCharacterStatProfile();
	// 最小等级的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="1000")) int32 MinLevel = 1;
	// 最大等级的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="1000")) int32 MaxLevel = 90;
	// Configuration版本的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1")) int32 ConfigurationVersion = 1;
	// 最大生命值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FScalableFloat MaxHealth = FScalableFloat(100.f);
	// 基础伤害的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FScalableFloat BaseDamage = FScalableFloat(20.f);
	// 初始化效果的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSubclassOf<UGameplayEffect> InitializationEffect;
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
