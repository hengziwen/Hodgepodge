# Input 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeAimSensitivityData.cpp

瞄准灵敏度数据映射。

源码：[Source/Hodgepodge/Private/Input/HodgeAimSensitivityData.cpp](../../../Source/Hodgepodge/Private/Input/HodgeAimSensitivityData.cpp)

项目内直接 include（不是运行调用关系）：[Input/HodgeAimSensitivityData.h](../../../Source/Hodgepodge/Public/Input/HodgeAimSensitivityData.h)

定义候选（多行签名仅展示首行）：

- L12: `UHodgeAimSensitivityData::UHodgeAimSensitivityData(const FObjectInitializer& ObjectInitializer)`

## HodgeInputComponent.cpp

基于 Tag 的 Native/Ability Action 绑定和句柄移除；映射辅助函数仍占位。

源码：[Source/Hodgepodge/Private/Input/HodgeInputComponent.cpp](../../../Source/Hodgepodge/Private/Input/HodgeInputComponent.cpp)

定义候选（多行签名仅展示首行）：


## HodgeInputConfig.cpp

NativeInputActions / AbilityInputActions 的 IA 与 Tag 数据配置及查询。

源码：[Source/Hodgepodge/Private/Input/HodgeInputConfig.cpp](../../../Source/Hodgepodge/Private/Input/HodgeInputConfig.cpp)

定义候选（多行签名仅展示首行）：


## HodgeInputModifiers.cpp

输入数值处理扩展；具体启用情况由 IA/IMC 资产决定。

源码：[Source/Hodgepodge/Private/Input/HodgeInputModifiers.cpp](../../../Source/Hodgepodge/Private/Input/HodgeInputModifiers.cpp)

项目内直接 include（不是运行调用关系）：[Input/HodgeInputModifiers.h](../../../Source/Hodgepodge/Public/Input/HodgeInputModifiers.h)、[Input/HodgeAimSensitivityData.h](../../../Source/Hodgepodge/Public/Input/HodgeAimSensitivityData.h)、[Core/LocalPlayer/HodgeLocalPlayerBase.h](../../../Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h)

定义候选（多行签名仅展示首行）：

- L55: `FInputActionValue UHodgeSettingBasedScalar::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput,`
- L146: `FInputActionValue UHodgeInputModifierDeadZone::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput,`
- L231: `FLinearColor UHodgeInputModifierDeadZone::GetVisualizationColor_Implementation(`
- L253: `FInputActionValue UHodgeInputModifierGamepadSensitivity::ModifyRaw_Implementation(`
- L292: `FInputActionValue UHodgeInputModifierAimInversion::ModifyRaw_Implementation(`

## HodgeInputUserSettings.cpp

Enhanced Input 用户设置派生入口；须核对实际设置类配置。

源码：[Source/Hodgepodge/Private/Input/HodgeInputUserSettings.cpp](../../../Source/Hodgepodge/Private/Input/HodgeInputUserSettings.cpp)

项目内直接 include（不是运行调用关系）：[Input/HodgeInputUserSettings.h](../../../Source/Hodgepodge/Public/Input/HodgeInputUserSettings.h)

定义候选（多行签名仅展示首行）：

- L9: `void UHodgeInputUserSettings::ApplySettings()`

## HodgePlayerMappableKeyProfile.cpp

玩家键位 Profile 扩展。

源码：[Source/Hodgepodge/Private/Input/HodgePlayerMappableKeyProfile.cpp](../../../Source/Hodgepodge/Private/Input/HodgePlayerMappableKeyProfile.cpp)

项目内直接 include（不是运行调用关系）：[Input/HodgePlayerMappableKeyProfile.h](../../../Source/Hodgepodge/Public/Input/HodgePlayerMappableKeyProfile.h)

定义候选（多行签名仅展示首行）：

- L9: `void UHodgePlayerMappableKeyProfile::EquipProfile()`
- L19: `void UHodgePlayerMappableKeyProfile::UnEquipProfile()`

## HodgeAimSensitivityData.h

瞄准灵敏度数据映射。

源码：[Source/Hodgepodge/Public/Input/HodgeAimSensitivityData.h](../../../Source/Hodgepodge/Public/Input/HodgeAimSensitivityData.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   7: #include "Engine/DataAsset.h"
  10: #include "HodgeAimSensitivityData.generated.h"
  16: class UObject;
  20: UCLASS(BlueprintType, Const,
  21: 	Meta = (DisplayName = "Hodge Aim Sensitivity Data", ShortTooltip =
  22: 		"Data asset used to define a map of Gamepad Sensitivty to a float value."))
  23: class HODGEPODGE_API UHodgeAimSensitivityData : public UPrimaryDataAsset
  24: {
  26: 	GENERATED_BODY()
  28: public:
  30: 	UHodgeAimSensitivityData(const FObjectInitializer& ObjectInitializer);
  40: };
```

## HodgeInputComponent.h

基于 Tag 的 Native/Ability Action 绑定和句柄移除；映射辅助函数仍占位。

源码：[Source/Hodgepodge/Public/Input/HodgeInputComponent.h](../../../Source/Hodgepodge/Public/Input/HodgeInputComponent.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  10: #pragma once
  12: #include "CoreMinimal.h"
  13: #include "EnhancedInputComponent.h"
  14: #include "GameplayTagContainer.h"
  15: #include "HodgeInputConfig.h"
  16: #include "HodgeInputComponent.generated.h"
  18: class UEnhancedInputLocalPlayerSubsystem;
  44: UCLASS(Config = Input)
  45: class HODGEPODGE_API UHodgeInputComponent : public UEnhancedInputComponent
  46: {
  47: 	GENERATED_BODY()
  49: public:
  55: 	UHodgeInputComponent();
  67: 	void AddInputMappings(const UHodgeInputConfig* InputConfig,
  68: 	                      UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
  79: 	void RemoveInputMappings(const UHodgeInputConfig* InputConfig,
  80: 	                         UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
  99: 	template <class UserClass, typename FuncType>
 100: 	void BindNativeAction(const UHodgeInputConfig* InputConfig, const FGameplayTag& InputTag,
 101: 	                      ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound);
 121: 	template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
 122: 	void BindAbilityActions(const UHodgeInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc,
 123: 	                        ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);
 133: 	void RemoveBinds(TArray<uint32>& BindHandles);
 134: };
 137: template <class UserClass, typename FuncType>
 138: void UHodgeInputComponent::BindNativeAction(const UHodgeInputConfig* InputConfig, const FGameplayTag& InputTag,
 139:                                             ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func,
 140:                                             bool bLogIfNotFound)
 141: {
 142: 	check(InputConfig);
 143: 	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
 144: 	{
 145: 		BindAction(IA, TriggerEvent, Object, Func);
 146: 	}
 147: }
 149: template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
 150: void UHodgeInputComponent::BindAbilityActions(const UHodgeInputConfig* InputConfig, UserClass* Object,
 151:                                               PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc,
 152:                                               TArray<uint32>& BindHandles)
 153: {
 154: 	check(InputConfig);
 156: 	for (const FHodgeInputAction& Action : InputConfig->AbilityInputActions)
 157: 	{
 158: 		if (Action.InputAction && Action.InputTag.IsValid())
 159: 		{
 160: 			if (PressedFunc)
 161: 			{
 162: 				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc,
 163: 				                           Action.InputTag).GetHandle());
 164: 			}
 166: 			if (ReleasedFunc)
 167: 			{
 168: 				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc,
 169: 				                           Action.InputTag).GetHandle());
 170: 			}
 171: 		}
 172: 	}
 173: }
```

## HodgeInputConfig.h

NativeInputActions / AbilityInputActions 的 IA 与 Tag 数据配置及查询。

源码：[Source/Hodgepodge/Public/Input/HodgeInputConfig.h](../../../Source/Hodgepodge/Public/Input/HodgeInputConfig.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  10: #pragma once
  12: #include "CoreMinimal.h"
  13: #include "GameplayTagContainer.h"
  14: #include "Engine/DataAsset.h"
  15: #include "HodgeInputConfig.generated.h"
  17: class UInputAction;
  29: USTRUCT(BlueprintType)
  30: struct FHodgeInputAction
  31: {
  32: 	GENERATED_BODY()
  34: public:
  41: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  42: 	TObjectPtr<const UInputAction> InputAction = nullptr;
  51: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
  52: 	FGameplayTag InputTag;
  53: };
  70: UCLASS(BlueprintType, Blueprintable, Const)
  71: class HODGEPODGE_API UHodgeInputConfig : public UDataAsset
  72: {
  73: 	GENERATED_BODY()
  75: public:
  81: 	UHodgeInputConfig(const FObjectInitializer& ObjectInitializer);
  93: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Pawn")
  94: 	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;
 106: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Pawn")
 107: 	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;
 109: public:
 117: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
 118: 	TArray<FHodgeInputAction> NativeInputActions;
 128: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
 129: 	TArray<FHodgeInputAction> AbilityInputActions;
 130: };
```

## HodgeInputModifiers.h

输入数值处理扩展；具体启用情况由 IA/IMC 资产决定。

源码：[Source/Hodgepodge/Public/Input/HodgeInputModifiers.h](../../../Source/Hodgepodge/Public/Input/HodgeInputModifiers.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   5: #pragma once
   8: #include "InputModifiers.h"
  11: #include "UObject/UnrealType.h"
  14: #include "HodgeInputModifiers.generated.h"
  17: struct FInputActionValue;
  20: class FProperty;
  23: class UEnhancedPlayerInput;
  26: class UHodgeAimSensitivityData;
  29: class UObject;
  35: UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "Setting Based Scalar"))
  36: class UHodgeSettingBasedScalar : public UInputModifier
  37: {
  38: 	GENERATED_BODY()
  40: public:
  43: 	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings)
  44: 	FName XAxisScalarSettingName = NAME_None;
  48: 	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings)
  49: 	FName YAxisScalarSettingName = NAME_None;
  53: 	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings)
  54: 	FName ZAxisScalarSettingName = NAME_None;
  58: 	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings)
  59: 	FVector MaxValueClamp = FVector(10.0, 10.0, 10.0);
  63: 	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings)
  64: 	FVector MinValueClamp = FVector::ZeroVector;
  66: protected:
  68: 	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput,
  69: 	                                                   FInputActionValue CurrentValue, float DeltaTime) override;
  73: 	TArray<const FProperty*> PropertyCache;
  74: };
  78: UENUM()
  79: enum class EDeadzoneStick : uint8
  80: {
  83: 	MoveStick = 0,
  87: 	LookStick = 1,
  88: };
  94: UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "Hodge Settings Driven Dead Zone"))
  95: class UHodgeInputModifierDeadZone : public UInputModifier
  96: {
  97: 	GENERATED_BODY()
  99: public:
 101: 	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings, Config)
 102: 	EDeadZoneType Type = EDeadZoneType::Radial;
 106: 	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings, Config)
 107: 	float UpperThreshold = 1.0f;
 111: 	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings, Config)
 112: 	EDeadzoneStick DeadzoneStick = EDeadzoneStick::MoveStick;
 114: protected:
 116: 	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput,
 117: 	                                                   FInputActionValue CurrentValue, float DeltaTime) override;
 122: 	virtual FLinearColor GetVisualizationColor_Implementation(FInputActionValue SampleValue,
 123: 	                                                          FInputActionValue FinalValue) const override;
 124: };
 128: UENUM()
 129: enum class EHodgeTargetingType : uint8
 130: {
 133: 	Normal = 0,
 137: 	ADS = 1,
 138: };
 142: UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "Hodge Gamepad Sensitivity"))
 143: class UHodgeInputModifierGamepadSensitivity : public UInputModifier
 144: {
 145: 	GENERATED_BODY()
 147: public:
 150: 	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings, Config)
 151: 	EHodgeTargetingType TargetingType = EHodgeTargetingType::Normal;
 155: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AssetBundles="Client,Server"))
 156: 	TObjectPtr<const UHodgeAimSensitivityData> SensitivityLevelTable;
 158: protected:
 160: 	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput,
 161: 	                                                   FInputActionValue CurrentValue, float DeltaTime) override;
 162: };
 166: UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "Hodge Aim Inversion Setting"))
 167: class UHodgeInputModifierAimInversion : public UInputModifier
 168: {
 169: 	GENERATED_BODY()
 171: protected:
 173: 	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput,
 174: 	                                                   FInputActionValue CurrentValue, float DeltaTime) override;
 175: };
```

## HodgeInputUserSettings.h

Enhanced Input 用户设置派生入口；须核对实际设置类配置。

源码：[Source/Hodgepodge/Public/Input/HodgeInputUserSettings.h](../../../Source/Hodgepodge/Public/Input/HodgeInputUserSettings.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   7: #include "UserSettings/EnhancedInputUserSettings.h"
  10: #include "PlayerMappableKeySettings.h"
  13: #include "HodgeInputUserSettings.generated.h"
  22: UCLASS()
  23: class HODGEPODGE_API UHodgeInputUserSettings : public UEnhancedInputUserSettings
  24: {
  25: 	GENERATED_BODY()
  27: public:
  30: 	virtual void ApplySettings() override;
  49: };
  58: UCLASS()
  59: class HODGEPODGE_API UHodgePlayerMappableKeySettings : public UPlayerMappableKeySettings
  60: {
  61: 	GENERATED_BODY()
  63: public:
  66: 	const FText& GetTooltipText() const;
  68: protected:
  71: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta=(AllowPrivateAccess=true))
  72: 	FText Tooltip = FText::GetEmpty();
  73: };
```

## HodgePlayerMappableKeyProfile.h

玩家键位 Profile 扩展。

源码：[Source/Hodgepodge/Public/Input/HodgePlayerMappableKeyProfile.h](../../../Source/Hodgepodge/Public/Input/HodgePlayerMappableKeyProfile.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   7: #include "UserSettings/EnhancedInputUserSettings.h"
  10: #include "HodgePlayerMappableKeyProfile.generated.h"
  13: UCLASS()
  14: class HODGEPODGE_API UHodgePlayerMappableKeyProfile : public UEnhancedPlayerMappableKeyProfile
  15: {
  16: 	GENERATED_BODY()
  18: protected:
  21: 	virtual void EquipProfile() override;
  24: 	virtual void UnEquipProfile() override;
  26: };
```
