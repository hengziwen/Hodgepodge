# Input 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeAimSensitivityData.cpp

瞄准灵敏度数据映射。

源码：[Source/Hodgepodge/Private/Input/HodgeAimSensitivityData.cpp](../../../Source/Hodgepodge/Private/Input/HodgeAimSensitivityData.cpp)

项目内直接 include（不是运行调用关系）：[Input/HodgeAimSensitivityData.h](../../../Source/Hodgepodge/Public/Input/HodgeAimSensitivityData.h)

定义候选（多行签名仅展示首行）：

- L15: `UHodgeAimSensitivityData::UHodgeAimSensitivityData(const FObjectInitializer& ObjectInitializer)`

## HodgeInputComponent.cpp

基于 Tag 的 Native/Ability Action 绑定和句柄移除；映射辅助函数仍占位。

源码：[Source/Hodgepodge/Private/Input/HodgeInputComponent.cpp](../../../Source/Hodgepodge/Private/Input/HodgeInputComponent.cpp)

项目内直接 include（不是运行调用关系）：[Input/HodgeInputComponent.h](../../../Source/Hodgepodge/Public/Input/HodgeInputComponent.h)

定义候选（多行签名仅展示首行）：

- L26: `UHodgeInputComponent::UHodgeInputComponent()`
- L42: `void UHodgeInputComponent::AddInputMappings(const UHodgeInputConfig* InputConfig,`
- L62: `void UHodgeInputComponent::RemoveInputMappings(const UHodgeInputConfig* InputConfig,`
- L84: `void UHodgeInputComponent::RemoveBinds(TArray<uint32>& BindHandles)`

## HodgeInputConfig.cpp

NativeInputActions / AbilityInputActions 的 IA 与 Tag 数据配置及查询。

源码：[Source/Hodgepodge/Private/Input/HodgeInputConfig.cpp](../../../Source/Hodgepodge/Private/Input/HodgeInputConfig.cpp)

项目内直接 include（不是运行调用关系）：[Input/HodgeInputConfig.h](../../../Source/Hodgepodge/Public/Input/HodgeInputConfig.h)

定义候选（多行签名仅展示首行）：

- L25: `UHodgeInputConfig::UHodgeInputConfig(const FObjectInitializer& ObjectInitializer)`
- L40: `const UInputAction* UHodgeInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag,`
- L71: `const UInputAction* UHodgeInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag,`

## HodgeInputModifiers.cpp

输入数值处理扩展；具体启用情况由 IA/IMC 资产决定。

源码：[Source/Hodgepodge/Private/Input/HodgeInputModifiers.cpp](../../../Source/Hodgepodge/Private/Input/HodgeInputModifiers.cpp)

项目内直接 include（不是运行调用关系）：[Input/HodgeInputModifiers.h](../../../Source/Hodgepodge/Public/Input/HodgeInputModifiers.h)、[Input/HodgeAimSensitivityData.h](../../../Source/Hodgepodge/Public/Input/HodgeAimSensitivityData.h)、[Core/LocalPlayer/HodgeLocalPlayerBase.h](../../../Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h)

定义候选（多行签名仅展示首行）：

- L58: `FInputActionValue UHodgeSettingBasedScalar::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput,`
- L149: `FInputActionValue UHodgeInputModifierDeadZone::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput,`
- L234: `FLinearColor UHodgeInputModifierDeadZone::GetVisualizationColor_Implementation(`
- L256: `FInputActionValue UHodgeInputModifierGamepadSensitivity::ModifyRaw_Implementation(`
- L295: `FInputActionValue UHodgeInputModifierAimInversion::ModifyRaw_Implementation(`

## HodgeInputUserSettings.cpp

Enhanced Input 用户设置派生入口；须核对实际设置类配置。

源码：[Source/Hodgepodge/Private/Input/HodgeInputUserSettings.cpp](../../../Source/Hodgepodge/Private/Input/HodgeInputUserSettings.cpp)

项目内直接 include（不是运行调用关系）：[Input/HodgeInputUserSettings.h](../../../Source/Hodgepodge/Public/Input/HodgeInputUserSettings.h)

定义候选（多行签名仅展示首行）：

- L12: `void UHodgeInputUserSettings::ApplySettings()`

## HodgePlayerMappableKeyProfile.cpp

玩家键位 Profile 扩展。

源码：[Source/Hodgepodge/Private/Input/HodgePlayerMappableKeyProfile.cpp](../../../Source/Hodgepodge/Private/Input/HodgePlayerMappableKeyProfile.cpp)

项目内直接 include（不是运行调用关系）：[Input/HodgePlayerMappableKeyProfile.h](../../../Source/Hodgepodge/Public/Input/HodgePlayerMappableKeyProfile.h)

定义候选（多行签名仅展示首行）：

- L12: `void UHodgePlayerMappableKeyProfile::EquipProfile()`
- L22: `void UHodgePlayerMappableKeyProfile::UnEquipProfile()`

## HodgeAimSensitivityData.h

瞄准灵敏度数据映射。

源码：[Source/Hodgepodge/Public/Input/HodgeAimSensitivityData.h](../../../Source/Hodgepodge/Public/Input/HodgeAimSensitivityData.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   7: #pragma once
  10: #include "Engine/DataAsset.h"
  13: #include "HodgeAimSensitivityData.generated.h"
  19: class UObject;
  23: UCLASS(BlueprintType, Const,
  24: 	Meta = (DisplayName = "Hodge Aim Sensitivity Data", ShortTooltip =
  25: 		"Data asset used to define a map of Gamepad Sensitivty to a float value."))
  27: class HODGEPODGE_API UHodgeAimSensitivityData : public UPrimaryDataAsset
  28: {
  30: 	GENERATED_BODY()
  32: public:
  37: 	UHodgeAimSensitivityData(const FObjectInitializer& ObjectInitializer);
  47: };
```

## HodgeInputComponent.h

基于 Tag 的 Native/Ability Action 绑定和句柄移除；映射辅助函数仍占位。

源码：[Source/Hodgepodge/Public/Input/HodgeInputComponent.h](../../../Source/Hodgepodge/Public/Input/HodgeInputComponent.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  13: #pragma once
  15: #include "CoreMinimal.h"
  16: #include "EnhancedInputComponent.h"
  17: #include "GameplayTagContainer.h"
  18: #include "HodgeInputConfig.h"
  19: #include "HodgeInputComponent.generated.h"
  21: class UEnhancedInputLocalPlayerSubsystem;
  47: UCLASS(Config = Input)
  48: class HODGEPODGE_API UHodgeInputComponent : public UEnhancedInputComponent
  49: {
  50: 	GENERATED_BODY()
  52: public:
  59: 	UHodgeInputComponent();
  75: 	void AddInputMappings(const UHodgeInputConfig* InputConfig,
  76: 	                      UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
  91: 	void RemoveInputMappings(const UHodgeInputConfig* InputConfig,
  92: 	                         UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
 111: 	template <class UserClass, typename FuncType>
 120: 	void BindNativeAction(const UHodgeInputConfig* InputConfig, const FGameplayTag& InputTag,
 121: 	                      ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound);
 141: 	template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
 149: 	void BindAbilityActions(const UHodgeInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc,
 150: 	                        ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);
 163: 	void RemoveBinds(TArray<uint32>& BindHandles);
 164: };
 167: template <class UserClass, typename FuncType>
 176: void UHodgeInputComponent::BindNativeAction(const UHodgeInputConfig* InputConfig, const FGameplayTag& InputTag,
 177:                                             ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func,
 178:                                             bool bLogIfNotFound)
 179: {
 180: 	check(InputConfig);
 181: 	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
 182: 	{
 183: 		BindAction(IA, TriggerEvent, Object, Func);
 184: 	}
 185: }
 187: template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
 195: void UHodgeInputComponent::BindAbilityActions(const UHodgeInputConfig* InputConfig, UserClass* Object,
 196:                                               PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc,
 197:                                               TArray<uint32>& BindHandles)
 198: {
 199: 	check(InputConfig);
 201: 	for (const FHodgeInputAction& Action : InputConfig->AbilityInputActions)
 202: 	{
 203: 		if (Action.InputAction && Action.InputTag.IsValid())
 204: 		{
 205: 			if (PressedFunc)
 206: 			{
 207: 				const bool bPressOnce = Action.InputTag == FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Dash")) ||
 208: 					Action.InputTag == FGameplayTag::RequestGameplayTag(TEXT("InputTag.Sprint")) ||
 209: 					Action.InputTag == FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.LockOn")) ||
 210: 					Action.InputTag == FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Melee"));
 212: 				BindHandles.Add(BindAction(Action.InputAction, bPressOnce ? ETriggerEvent::Started : ETriggerEvent::Triggered, Object, PressedFunc,
 213: 				                           Action.InputTag).GetHandle());
 214: 			}
 216: 			if (ReleasedFunc)
 217: 			{
 218: 				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc,
 219: 				                           Action.InputTag).GetHandle());
 220: 				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Canceled, Object, ReleasedFunc,
 221: 				                           Action.InputTag).GetHandle());
 222: 			}
 223: 		}
 224: 	}
 225: }
```

## HodgeInputConfig.h

NativeInputActions / AbilityInputActions 的 IA 与 Tag 数据配置及查询。

源码：[Source/Hodgepodge/Public/Input/HodgeInputConfig.h](../../../Source/Hodgepodge/Public/Input/HodgeInputConfig.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  13: #pragma once
  15: #include "CoreMinimal.h"
  16: #include "GameplayTagContainer.h"
  17: #include "Engine/DataAsset.h"
  18: #include "HodgeInputConfig.generated.h"
  20: class UInputAction;
  32: USTRUCT(BlueprintType)
  33: struct FHodgeInputAction
  34: {
  35: 	GENERATED_BODY()
  37: public:
  44: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  45: 	TObjectPtr<const UInputAction> InputAction = nullptr;
  55: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
  56: 	FGameplayTag InputTag;
  57: };
  74: UCLASS(BlueprintType, Blueprintable, Const)
  75: class HODGEPODGE_API UHodgeInputConfig : public UDataAsset
  76: {
  77: 	GENERATED_BODY()
  79: public:
  88: 	UHodgeInputConfig(const FObjectInitializer& ObjectInitializer);
 105: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Pawn")
 106: 	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;
 123: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Pawn")
 124: 	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;
 126: public:
 134: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
 135: 	TArray<FHodgeInputAction> NativeInputActions;
 145: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
 146: 	TArray<FHodgeInputAction> AbilityInputActions;
 147: };
```

## HodgeInputModifiers.h

输入数值处理扩展；具体启用情况由 IA/IMC 资产决定。

源码：[Source/Hodgepodge/Public/Input/HodgeInputModifiers.h](../../../Source/Hodgepodge/Public/Input/HodgeInputModifiers.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   8: #pragma once
  11: #include "InputModifiers.h"
  14: #include "UObject/UnrealType.h"
  17: #include "HodgeInputModifiers.generated.h"
  20: struct FInputActionValue;
  23: class FProperty;
  26: class UEnhancedPlayerInput;
  29: class UHodgeAimSensitivityData;
  32: class UObject;
  38: UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "Setting Based Scalar"))
  39: class UHodgeSettingBasedScalar : public UInputModifier
  40: {
  41: 	GENERATED_BODY()
  43: public:
  46: 	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings)
  47: 	FName XAxisScalarSettingName = NAME_None;
  51: 	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings)
  52: 	FName YAxisScalarSettingName = NAME_None;
  56: 	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings)
  57: 	FName ZAxisScalarSettingName = NAME_None;
  61: 	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings)
  62: 	FVector MaxValueClamp = FVector(10.0, 10.0, 10.0);
  66: 	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings)
  67: 	FVector MinValueClamp = FVector::ZeroVector;
  69: protected:
  77: 	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput,
  78: 	                                                   FInputActionValue CurrentValue, float DeltaTime) override;
  82: 	TArray<const FProperty*> PropertyCache;
  83: };
  87: UENUM()
  88: enum class EDeadzoneStick : uint8
  89: {
  92: 	MoveStick = 0,
  96: 	LookStick = 1,
  97: };
 103: UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "Hodge Settings Driven Dead Zone"))
 104: class UHodgeInputModifierDeadZone : public UInputModifier
 105: {
 106: 	GENERATED_BODY()
 108: public:
 110: 	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings, Config)
 111: 	EDeadZoneType Type = EDeadZoneType::Radial;
 115: 	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings, Config)
 116: 	float UpperThreshold = 1.0f;
 120: 	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings, Config)
 121: 	EDeadzoneStick DeadzoneStick = EDeadzoneStick::MoveStick;
 123: protected:
 131: 	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput,
 132: 	                                                   FInputActionValue CurrentValue, float DeltaTime) override;
 142: 	virtual FLinearColor GetVisualizationColor_Implementation(FInputActionValue SampleValue,
 143: 	                                                          FInputActionValue FinalValue) const override;
 144: };
 148: UENUM()
 149: enum class EHodgeTargetingType : uint8
 150: {
 153: 	Normal = 0,
 157: 	ADS = 1,
 158: };
 162: UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "Hodge Gamepad Sensitivity"))
 163: class UHodgeInputModifierGamepadSensitivity : public UInputModifier
 164: {
 165: 	GENERATED_BODY()
 167: public:
 170: 	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings, Config)
 171: 	EHodgeTargetingType TargetingType = EHodgeTargetingType::Normal;
 175: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AssetBundles="Client,Server"))
 176: 	TObjectPtr<const UHodgeAimSensitivityData> SensitivityLevelTable;
 178: protected:
 186: 	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput,
 187: 	                                                   FInputActionValue CurrentValue, float DeltaTime) override;
 188: };
 192: UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "Hodge Aim Inversion Setting"))
 193: class UHodgeInputModifierAimInversion : public UInputModifier
 194: {
 195: 	GENERATED_BODY()
 197: protected:
 205: 	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput,
 206: 	                                                   FInputActionValue CurrentValue, float DeltaTime) override;
 207: };
```

## HodgeInputUserSettings.h

Enhanced Input 用户设置派生入口；须核对实际设置类配置。

源码：[Source/Hodgepodge/Public/Input/HodgeInputUserSettings.h](../../../Source/Hodgepodge/Public/Input/HodgeInputUserSettings.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   7: #pragma once
  10: #include "UserSettings/EnhancedInputUserSettings.h"
  13: #include "PlayerMappableKeySettings.h"
  16: #include "HodgeInputUserSettings.generated.h"
  25: UCLASS()
  26: class HODGEPODGE_API UHodgeInputUserSettings : public UEnhancedInputUserSettings
  27: {
  28: 	GENERATED_BODY()
  30: public:
  34: 	virtual void ApplySettings() override;
  53: };
  62: UCLASS()
  63: class HODGEPODGE_API UHodgePlayerMappableKeySettings : public UPlayerMappableKeySettings
  64: {
  65: 	GENERATED_BODY()
  67: public:
  73: 	const FText& GetTooltipText() const;
  75: protected:
  78: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta=(AllowPrivateAccess=true))
  79: 	FText Tooltip = FText::GetEmpty();
  80: };
```

## HodgePlayerMappableKeyProfile.h

玩家键位 Profile 扩展。

源码：[Source/Hodgepodge/Public/Input/HodgePlayerMappableKeyProfile.h](../../../Source/Hodgepodge/Public/Input/HodgePlayerMappableKeyProfile.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   7: #pragma once
  10: #include "UserSettings/EnhancedInputUserSettings.h"
  13: #include "HodgePlayerMappableKeyProfile.generated.h"
  16: UCLASS()
  17: class HODGEPODGE_API UHodgePlayerMappableKeyProfile : public UEnhancedPlayerMappableKeyProfile
  18: {
  19: 	GENERATED_BODY()
  21: protected:
  25: 	virtual void EquipProfile() override;
  29: 	virtual void UnEquipProfile() override;
  31: };
```
