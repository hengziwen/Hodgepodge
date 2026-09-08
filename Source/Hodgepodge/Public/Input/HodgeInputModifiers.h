
// Copyright Epic Games, Inc. All Rights Reserved.

// 项目输入修饰器相关定义。
#pragma once

// Enhanced Input 的输入 Modifier 基类和相关类型。
#include "InputModifiers.h"

// FProperty 等反射属性相关类型。
#include "UObject/UnrealType.h"

// 当前头文件对应的 UE 反射代码生成文件。
#include "HodgeInputModifiers.generated.h"

// 输入动作当前传递的值，例如 Vector2D / Vector3D 等。
struct FInputActionValue;

// UE 反射属性基类。
class FProperty;

// Enhanced Input 玩家输入对象。
class UEnhancedPlayerInput;

// 项目瞄准灵敏度数据。
class UHodgeAimSensitivityData;

// UE 基础对象类型。
class UObject;

/** 
*  Scales input basedon a double property in the SharedUserSettings
*/
// 根据 SharedUserSettings 中的数值设置，对输入值进行缩放。
UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "Setting Based Scalar"))
class UHodgeSettingBasedScalar : public UInputModifier
{
	GENERATED_BODY()

public:
	/** Name of the property that will be used to clamp the X Axis of this value */
	// X 轴对应的用户设置属性名称。
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings)
	FName XAxisScalarSettingName = NAME_None;

	/** Name of the property that will be used to clamp the Y Axis of this value */
	// Y 轴对应的用户设置属性名称。
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings)
	FName YAxisScalarSettingName = NAME_None;

	/** Name of the property that will be used to clamp the Z Axis of this value */
	// Z 轴对应的用户设置属性名称。
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings)
	FName ZAxisScalarSettingName = NAME_None;

	/** Set the maximium value of this setting on each axis. */
	// 限制用户设置在各个轴上的最大允许值。
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings)
	FVector MaxValueClamp = FVector(10.0, 10.0, 10.0);

	/** Set the minimum value of this setting on each axis. */
	// 限制用户设置在各个轴上的最小允许值。
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings)
	FVector MinValueClamp = FVector::ZeroVector;

protected:
	// 根据玩家设置中的数值修改当前输入值。
	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput,
	                                                   FInputActionValue CurrentValue, float DeltaTime) override;

	/** FProperty Cache that will be populated with any found FProperty's on the settings class so that we don't need to look them up each frame */
	// 缓存已经找到的设置属性，避免每帧通过反射重新查找 FProperty。
	TArray<const FProperty*> PropertyCache;
};

/** Represents which stick that this deadzone is for, either the move or the look stick */
// 表示当前 DeadZone 修饰器作用于哪个摇杆。
UENUM()
enum class EDeadzoneStick : uint8
{
	/** Deadzone for the movement stick */
	// 移动摇杆的 DeadZone。
	MoveStick = 0,

	/** Deadzone for the looking stick */
	// 视角/镜头摇杆的 DeadZone。
	LookStick = 1,
};

/**
 * This is a deadzone input modifier that will have it's thresholds driven by what is in the Hodge Shared game settings. 
 */
// 根据 Hodge Shared Game Settings 中的配置动态决定摇杆 DeadZone。
UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "Hodge Settings Driven Dead Zone"))
class UHodgeInputModifierDeadZone : public UInputModifier
{
	GENERATED_BODY()

public:
	// DeadZone 的计算类型，例如 Radial 等。
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings, Config)
	EDeadZoneType Type = EDeadZoneType::Radial;

	// Threshold above which input is clamped to 1
	// 超过该阈值后，输入值会被限制为最大值 1。
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings, Config)
	float UpperThreshold = 1.0f;

	/** Which stick this deadzone is for. This controls which setting will be used when calculating the deadzone */
	// 指定当前 DeadZone 使用移动摇杆还是视角摇杆对应的用户设置。
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings, Config)
	EDeadzoneStick DeadzoneStick = EDeadzoneStick::MoveStick;

protected:
	// 根据用户设置计算 DeadZone，并修改原始输入值。
	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput,
	                                                   FInputActionValue CurrentValue, float DeltaTime) override;

	// Visualize as black when unmodified. Red when blocked (with differing intensities to indicate axes)
	// 根据输入是否被 DeadZone 阻挡，为 Enhanced Input 编辑器提供可视化颜色。
	// Mirrors visualization in https://www.gamasutra.com/blogs/JoshSutphin/20130416/190541/Doing_Thumbstick_Dead_Zones_Right.php.
	virtual FLinearColor GetVisualizationColor_Implementation(FInputActionValue SampleValue,
	                                                          FInputActionValue FinalValue) const override;
};

/** The type of targeting sensitity that should be considered */
// 表示当前应该使用哪一种瞄准灵敏度配置。
UENUM()
enum class EHodgeTargetingType : uint8
{
	/** Sensitivity to be applied why normally looking around */
	// 玩家正常转动镜头时使用的灵敏度。
	Normal = 0,

	/** The sensitivity that should be applied while Aiming Down Sights */
	// 玩家进入 ADS 瞄准状态时使用的灵敏度。
	ADS = 1,
};

/** Applies a scalar modifier based on the current gamepad settings in Hodge Shared game settings.  */
// 根据 Hodge Shared Game Settings 中的手柄灵敏度设置，对输入值进行缩放。
UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "Hodge Gamepad Sensitivity"))
class UHodgeInputModifierGamepadSensitivity : public UInputModifier
{
	GENERATED_BODY()

public:
	/** The type of targeting to use for this Sensitivity */
	// 当前使用 Normal 还是 ADS 灵敏度。
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Settings, Config)
	EHodgeTargetingType TargetingType = EHodgeTargetingType::Normal;

	/** Asset that gives us access to the float scalar value being used for sensitivty */
	// 灵敏度数据表，用于根据配置获取实际的灵敏度缩放值。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AssetBundles="Client,Server"))
	TObjectPtr<const UHodgeAimSensitivityData> SensitivityLevelTable;

protected:
	// 根据当前手柄灵敏度设置修改原始输入值。
	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput,
	                                                   FInputActionValue CurrentValue, float DeltaTime) override;
};

/** Applies an inversion of axis values based on a setting in the Hodge Shared game settings */
// 根据 Hodge Shared Game Settings 中的设置，对瞄准输入轴进行反转。
UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "Hodge Aim Inversion Setting"))
class UHodgeInputModifierAimInversion : public UInputModifier
{
	GENERATED_BODY()

protected:
	// 根据玩家的瞄准反转设置修改原始输入值。
	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput,
	                                                   FInputActionValue CurrentValue, float DeltaTime) override;
};
