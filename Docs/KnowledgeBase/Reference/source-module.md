# Module 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeAbilityEditor.Build.cs

模块或基础类型入口。

源码：[Source/HodgeAbilityEditor/HodgeAbilityEditor.Build.cs](../../../Source/HodgeAbilityEditor/HodgeAbilityEditor.Build.cs)

定义候选（多行签名仅展示首行）：


## HodgeAbilityEditorModule.cpp

模块或基础类型入口。

源码：[Source/HodgeAbilityEditor/Private/HodgeAbilityEditorModule.cpp](../../../Source/HodgeAbilityEditor/Private/HodgeAbilityEditorModule.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)

定义候选（多行签名仅展示首行）：


## HodgeAnimationAuthoringLibrary.cpp

Editor 动画文本导入、属性/引用修正、编译诊断与实验 PIE 配置。

源码：[Source/HodgeAbilityEditor/Private/HodgeAnimationAuthoringLibrary.cpp](../../../Source/HodgeAbilityEditor/Private/HodgeAnimationAuthoringLibrary.cpp)

定义候选（多行签名仅展示首行）：

- L97: `UAnimBlueprint* UHodgeAnimationAuthoringLibrary::ImportAnimationBlueprintText(const FString& AssetPath, const FString& TextFilename, UClass* ParentClass, USkeleton* TargetSkeleton)`
- L163: `bool UHodgeAnimationAuthoringLibrary::SetAnimationDefault(UBlueprint* Blueprint, const FString& PropertyName, const FString& Value)`
- L172: `bool UHodgeAnimationAuthoringLibrary::SetAnimationNodeProperty(UBlueprint* Blueprint, const FString& NodePath, const FString& PropertyPath, const FString& Value)`
- L183: `bool UHodgeAnimationAuthoringLibrary::RemapAnimationReferences(UBlueprint* Blueprint, const TArray<UObject*>& Sources, const TArray<UObject*>& Destinations)`
- L206: `FString UHodgeAnimationAuthoringLibrary::CompileAnimationBlueprint(UBlueprint* Blueprint)`
- L223: `TArray<FName> UHodgeAnimationAuthoringLibrary::GetSkeletonBoneNames(USkeleton* Skeleton)`
- L233: `bool UHodgeAnimationAuthoringLibrary::ConfigureAnimationLabPIE(int32 PlayerCount, bool bListenServer)`
- L243: `bool UHodgeAnimationAuthoringLibrary::ConfigureAnimationLabGameMode(UBlueprint* Blueprint, TSubclassOf<APawn> PawnClass)`

## HodgeAnimationAuthoringLibrary.h

Editor 动画文本导入、属性/引用修正、编译诊断与实验 PIE 配置。

源码：[Source/HodgeAbilityEditor/Private/HodgeAnimationAuthoringLibrary.h](../../../Source/HodgeAbilityEditor/Private/HodgeAnimationAuthoringLibrary.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   3: #include "CoreMinimal.h"
   4: #include "Kismet/BlueprintFunctionLibrary.h"
   6: #include "HodgeAnimationAuthoringLibrary.generated.h"
   8: class UAnimBlueprint;
   9: class UBlueprint;
  10: class USkeleton;
  11: class APawn;
  14: UCLASS()
  15: class HODGEABILITYEDITOR_API UHodgeAnimationAuthoringLibrary : public UBlueprintFunctionLibrary
  16: {
  18: 	GENERATED_BODY()
  20: public:
  22: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
  23: 	static UAnimBlueprint* ImportAnimationBlueprintText(const FString& AssetPath, const FString& TextFilename, UClass* ParentClass = nullptr, USkeleton* TargetSkeleton = nullptr);
  26: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
  27: 	static bool SetAnimationDefault(UBlueprint* Blueprint, const FString& PropertyName, const FString& Value);
  30: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
  31: 	static bool SetAnimationNodeProperty(UBlueprint* Blueprint, const FString& NodePath, const FString& PropertyPath, const FString& Value);
  34: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
  35: 	static bool RemapAnimationReferences(UBlueprint* Blueprint, const TArray<UObject*>& Sources, const TArray<UObject*>& Destinations);
  38: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
  39: 	static FString CompileAnimationBlueprint(UBlueprint* Blueprint);
  42: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
  43: 	static bool ConfigureAnimationLabGameMode(UBlueprint* Blueprint, TSubclassOf<APawn> PawnClass);
  46: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
  47: 	static bool ConfigureAnimationLabPIE(int32 PlayerCount, bool bListenServer);
  49: 	UFUNCTION(BlueprintPure, Category = "Hodge|Editor|Animation")
  50: 	static TArray<FName> GetSkeletonBoneNames(USkeleton* Skeleton);
  51: };
```

## HodgeCombatValidationLibrary.cpp

模块或基础类型入口。

源码：[Source/HodgeAbilityEditor/Private/HodgeCombatValidationLibrary.cpp](../../../Source/HodgeAbilityEditor/Private/HodgeCombatValidationLibrary.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)

定义候选（多行签名仅展示首行）：

- L13: `bool UHodgeCombatValidationLibrary::QueueAbilityAction(UHodgeAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle Handle, bool bCancel)`
- L75: `bool UHodgeCombatValidationLibrary::AddHitNotify(UAnimMontage* Montage, float Start, float End, const FHodgeAnimHitConfig& Config, bool bSingle)`
- L89: `bool UHodgeCombatValidationLibrary::AddStateNotify(UAnimMontage* Montage, float Start, float End, FGameplayTag Tag)`
- L97: `bool UHodgeCombatValidationLibrary::AddWeaponHandNotify(UAnimMontage* Montage, float Start, float End)`
- L103: `FGameplayTagContainer UHodgeCombatValidationLibrary::InspectAbilityWindows(UHodgeGameplayAbility_Definition* Ability)`
- L107: `int32 UHodgeCombatValidationLibrary::InspectHitSessions(AActor* Avatar)`
- L112: `int32 UHodgeCombatValidationLibrary::InspectPoseLeases(AActor* Avatar)`
- L118: `bool UHodgeCombatValidationLibrary::ConfigureValidationPIE(int32 Players, bool bDedicated)`
- L128: `bool UHodgeCombatValidationLibrary::ConfigureValidationSections(UAnimMontage* Montage)`
- L146: `bool UHodgeCombatValidationLibrary::NormalizeCombatNotifies(UAnimMontage* Montage)`

## HodgeCombatValidationLibrary.h

模块或基础类型入口。

源码：[Source/HodgeAbilityEditor/Private/HodgeCombatValidationLibrary.h](../../../Source/HodgeAbilityEditor/Private/HodgeCombatValidationLibrary.h)

项目内直接 include（不是运行调用关系）：[Animation/HodgeCombatAnimNotifies.h](../../../Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   2: #include "Kismet/BlueprintFunctionLibrary.h"
   3: #include "GameplayAbilitySpecHandle.h"
   4: #include "Animation/HodgeCombatAnimNotifies.h"
   5: #include "HodgeCombatValidationLibrary.generated.h"
   7: class UHodgeAbilitySystemComponent;
   8: class UHodgeGameplayAbility_Definition;
   9: class UAnimMontage;
  12: UCLASS()
  13: class HODGEABILITYEDITOR_API UHodgeCombatValidationLibrary : public UBlueprintFunctionLibrary
  14: {
  15: 	GENERATED_BODY()
  16: public:
  17: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
  18: 	static bool QueueAbilityAction(UHodgeAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle Handle, bool bCancel = false);
  19: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
  20: 	static bool AddHitNotify(UAnimMontage* Montage, float Start, float End, const FHodgeAnimHitConfig& Config, bool bSingle = false);
  21: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
  22: 	static bool AddStateNotify(UAnimMontage* Montage, float Start, float End, FGameplayTag Tag);
  23: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
  24: 	static bool AddWeaponHandNotify(UAnimMontage* Montage, float Start, float End);
  25: 	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation")
  26: 	static FGameplayTagContainer InspectAbilityWindows(UHodgeGameplayAbility_Definition* Ability);
  27: 	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation")
  28: 	static int32 InspectHitSessions(AActor* Avatar);
  29: 	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation")
  30: 	static int32 InspectPoseLeases(AActor* Avatar);
  31: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
  32: 	static bool ConfigureValidationPIE(int32 Players, bool bDedicated = false);
  33: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
  34: 	static bool ConfigureValidationSections(UAnimMontage* Montage);
  35: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
  36: 	static bool NormalizeCombatNotifies(UAnimMontage* Montage);
  37: };
```

## HodgeUIAuthoringLibrary.cpp

仅 Editor 的 Main UI 资产制作、原生 PIE 命令调度、Slate 截图与只读检查入口。

源码：[Source/HodgeAbilityEditor/Private/HodgeUIAuthoringLibrary.cpp](../../../Source/HodgeAbilityEditor/Private/HodgeUIAuthoringLibrary.cpp)

项目内直接 include（不是运行调用关系）：[UI/Foundation/HodgePrimaryGameLayout.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgePrimaryGameLayout.h)、[UI/Subsystem/HodgeUIManagerSubsystem.h](../../../Source/Hodgepodge/Public/UI/Subsystem/HodgeUIManagerSubsystem.h)、[UI/Extension/UIExtensionPointWidget.h](../../../Source/Hodgepodge/Public/UI/Extension/UIExtensionPointWidget.h)、[Data/HodgeExperienceActionSet.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceActionSet.h)、[Data/HodgeExperienceDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceDefinition.h)、[GameFeatures/GameFeatureAction_AddWidget.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddWidget.h)、[Component/HodgeExperienceManagerComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h)、[Component/HodgeHealthComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHealthComponent.h)、[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[Core/PlayerController/HodgePlayerController.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h)、[UI/Data/HodgeGameplayUIDataSource.h](../../../Source/Hodgepodge/Public/UI/Data/HodgeGameplayUIDataSource.h)、[UI/HodgeHUDLayout.h](../../../Source/Hodgepodge/Public/UI/HodgeHUDLayout.h)、[UI/HodgeActivatableWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeActivatableWidget.h)

定义候选（多行签名仅展示首行）：

- L125: `FString UHodgeUIAuthoringLibrary::CreateFoundationAssets()`
- L190: `FString UHodgeUIAuthoringLibrary::InspectPlayerUI(APlayerController* Player)`
- L263: `bool UHodgeUIAuthoringLibrary::QueueUIAction(APlayerController* Player, FName Action, float Value)`
- L822: `FString UHodgeUIAuthoringLibrary::MigrateDesignerAssets()`
- L916: `FString UHodgeUIAuthoringLibrary::InspectDesignerAssets()`
- L931: `FString UHodgeUIAuthoringLibrary::ConfigureDesignerPreviews()`

## HodgeUIAuthoringLibrary.h

仅 Editor 的 Main UI 资产制作、原生 PIE 命令调度、Slate 截图与只读检查入口。

源码：[Source/HodgeAbilityEditor/Private/HodgeUIAuthoringLibrary.h](../../../Source/HodgeAbilityEditor/Private/HodgeUIAuthoringLibrary.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   2: #include "Kismet/BlueprintFunctionLibrary.h"
   3: #include "HodgeUIAuthoringLibrary.generated.h"
   4: class APlayerController;
   7: UCLASS()
   8: class UHodgeUIAuthoringLibrary : public UBlueprintFunctionLibrary
   9: {
  10:     GENERATED_BODY()
  11: public:
  12:     UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static FString CreateFoundationAssets();
  13:     UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static FString MigrateDesignerAssets();
  14:     UFUNCTION(BlueprintPure, Category="Hodge|Editor|UI") static FString InspectDesignerAssets();
  15:     UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static FString ConfigureDesignerPreviews();
  16:     UFUNCTION(BlueprintPure, Category="Hodge|Editor|UI") static FString InspectPlayerUI(APlayerController* Player);
  17:     UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static bool QueueUIAction(APlayerController* Player, FName Action, float Value = 0);
  18: };
```

## Hodgepodge.Build.cs

模块或基础类型入口。

源码：[Source/Hodgepodge/Hodgepodge.Build.cs](../../../Source/Hodgepodge/Hodgepodge.Build.cs)

定义候选（多行签名仅展示首行）：


## Hodgepodge.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Hodgepodge.cpp](../../../Source/Hodgepodge/Hodgepodge.cpp)

定义候选（多行签名仅展示首行）：


## Hodgepodge.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Hodgepodge.h](../../../Source/Hodgepodge/Hodgepodge.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  10: #pragma once
  12: #include "CoreMinimal.h"
```

## Hodgepodge.Target.cs

模块或基础类型入口。

源码：[Source/Hodgepodge.Target.cs](../../../Source/Hodgepodge.Target.cs)

定义候选（多行签名仅展示首行）：


## HodgepodgeEditor.Target.cs

模块或基础类型入口。

源码：[Source/HodgepodgeEditor.Target.cs](../../../Source/HodgepodgeEditor.Target.cs)

定义候选（多行签名仅展示首行）：
