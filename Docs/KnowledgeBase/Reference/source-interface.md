# Interface 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeAbilitySourceInterface.cpp

技能来源与相关计算契约。

源码：[Source/Hodgepodge/Private/Interface/HodgeAbilitySourceInterface.cpp](../../../Source/Hodgepodge/Private/Interface/HodgeAbilitySourceInterface.cpp)

项目内直接 include（不是运行调用关系）：[Interface/HodgeAbilitySourceInterface.h](../../../Source/Hodgepodge/Public/Interface/HodgeAbilitySourceInterface.h)

定义候选（多行签名仅展示首行）：

- L11: `UHodgeAbilitySourceInterface::UHodgeAbilitySourceInterface(const FObjectInitializer& ObjectInitializer)`

## LoadingProcessInterface.cpp

加载状态/原因查询契约；不是独立加载界面。

源码：[Source/Hodgepodge/Private/Interface/LoadingProcessInterface.cpp](../../../Source/Hodgepodge/Private/Interface/LoadingProcessInterface.cpp)

项目内直接 include（不是运行调用关系）：[Interface/LoadingProcessInterface.h](../../../Source/Hodgepodge/Public/Interface/LoadingProcessInterface.h)

定义候选（多行签名仅展示首行）：


## HodgeAbilitySourceInterface.h

技能来源与相关计算契约。

源码：[Source/Hodgepodge/Public/Interface/HodgeAbilitySourceInterface.h](../../../Source/Hodgepodge/Public/Interface/HodgeAbilitySourceInterface.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "UObject/Interface.h"
  10: #include "HodgeAbilitySourceInterface.generated.h"
  12: class UObject;
  13: class UPhysicalMaterial;
  14: struct FGameplayTagContainer;
  20: UINTERFACE()
  21: class UHodgeAbilitySourceInterface : public UInterface
  22: {
  23: 	GENERATED_UINTERFACE_BODY()
  24: };
  30: class IHodgeAbilitySourceInterface
  31: {
  32: 	GENERATED_IINTERFACE_BODY()
  48: 	virtual float GetDistanceAttenuation(float Distance, const FGameplayTagContainer* SourceTags = nullptr,
  49: 	                                     const FGameplayTagContainer* TargetTags = nullptr) const = 0;
  66: 	virtual float GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial,
  67: 	                                             const FGameplayTagContainer* SourceTags = nullptr,
  68: 	                                             const FGameplayTagContainer* TargetTags = nullptr) const = 0;
  69: };
```

## HodgeLockOnTargetInterface.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Interface/HodgeLockOnTargetInterface.h](../../../Source/Hodgepodge/Public/Interface/HodgeLockOnTargetInterface.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "UObject/Interface.h"
   4: #include "HodgeLockOnTargetInterface.generated.h"
   5: UINTERFACE(BlueprintType)
   6: class HODGEPODGE_API UHodgeLockOnTargetInterface : public UInterface { GENERATED_BODY() };
   8: class HODGEPODGE_API IHodgeLockOnTargetInterface
   9: {
  10: 	GENERATED_BODY()
  11: public:
  13: 	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Hodge|LockOn") bool CanBeLockedBy(APawn* SourcePawn) const;
  15: 	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Hodge|LockOn") bool GetLockPoint(FName LockPointId, FVector& OutLocation) const;
  17: 	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Hodge|LockOn") float GetLockPriority() const;
  18: };
```

## LoadingProcessInterface.h

加载状态/原因查询契约；不是独立加载界面。

源码：[Source/Hodgepodge/Public/Interface/LoadingProcessInterface.h](../../../Source/Hodgepodge/Public/Interface/LoadingProcessInterface.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "CoreMinimal.h"
   9: #include "UObject/Interface.h"
  10: #include "LoadingProcessInterface.generated.h"
  13: UINTERFACE(MinimalAPI)
  14: class ULoadingProcessInterface : public UInterface
  15: {
  16: 	GENERATED_BODY()
  17: };
  24: class HODGEPODGE_API ILoadingProcessInterface
  25: {
  26: 	GENERATED_BODY()
  28: public:
  35: 	static bool ShouldShowLoadingScreen(UObject* TestObject, FString& OutReason);
  43: 	virtual bool ShouldShowLoadingScreen(FString& OutReason) const
  44: 	{
  46: 		return false;
  47: 	}
  48: };
```
