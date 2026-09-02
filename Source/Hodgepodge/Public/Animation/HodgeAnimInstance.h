// 111 屎山代码来袭

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Animation/AnimInstance.h"
#include "HodgeAnimInstance.generated.h"

/**
 * 角色动画实例基类，负责将角色的 Gameplay 状态同步给动画系统
 */
UCLASS(Config = Game)
class HODGEPODGE_API UHodgeAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// 构造动画实例并进行默认属性初始化
	UHodgeAnimInstance(const FObjectInitializer& ObjectInitializer);

	// 将动画实例与角色的 AbilitySystemComponent 绑定，使动画可以读取 GAS 状态
	virtual void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);

protected:
#if WITH_EDITOR
	// 编辑器中验证动画实例配置是否正确
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif // WITH_EDITOR

	// 动画实例初始化时调用，适合获取 Owner、Character 等基础引用
	virtual void NativeInitializeAnimation() override;

	// 每帧更新动画实例数据，供动画蓝图读取最新角色状态
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	// 将 GameplayTag 自动映射到动画蓝图变量，Tag 添加或移除时会自动更新对应变量
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;

	// 角色与地面的距离，-1 表示当前还没有有效的地面距离数据
	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
	float GroundDistance = -1.0f;
};
