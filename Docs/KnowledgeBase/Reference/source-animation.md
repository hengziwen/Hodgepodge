# Animation 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeAnimInstance.cpp

ASC GameplayTag 属性映射和 GroundDistance 更新。

源码：[Source/Hodgepodge/Private/Animation/HodgeAnimInstance.cpp](../../../Source/Hodgepodge/Private/Animation/HodgeAnimInstance.cpp)

项目内直接 include（不是运行调用关系）：[Animation/HodgeAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimInstance.h)、[Character/HodgeCharacterBase.h](../../../Source/Hodgepodge/Public/Character/HodgeCharacterBase.h)、[Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)

定义候选（多行签名仅展示首行）：

- L13: `UHodgeAnimInstance::UHodgeAnimInstance(const FObjectInitializer& ObjectInitializer)`
- L18: `void UHodgeAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)`
- L29: `EDataValidationResult UHodgeAnimInstance::IsDataValid(class FDataValidationContext& Context) const`
- L43: `void UHodgeAnimInstance::NativeInitializeAnimation()`
- L61: `void UHodgeAnimInstance::NativeUpdateAnimation(float DeltaSeconds)`

## HodgeAnimInstance.h

ASC GameplayTag 属性映射和 GroundDistance 更新。

源码：[Source/Hodgepodge/Public/Animation/HodgeAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimInstance.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CoreMinimal.h"
   6: #include "GameplayEffectTypes.h"
   7: #include "Animation/AnimInstance.h"
   8: #include "HodgeAnimInstance.generated.h"
  13: UCLASS(Config = Game)
  14: class HODGEPODGE_API UHodgeAnimInstance : public UAnimInstance
  15: {
  16: 	GENERATED_BODY()
  18: public:
  20: 	UHodgeAnimInstance(const FObjectInitializer& ObjectInitializer);
  23: 	virtual void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);
  25: protected:
  26: #if WITH_EDITOR
  28: 	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
  29: #endif
  32: 	virtual void NativeInitializeAnimation() override;
  35: 	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
  37: protected:
  39: 	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
  40: 	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;
  43: 	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
  44: 	float GroundDistance = -1.0f;
  45: };
```
