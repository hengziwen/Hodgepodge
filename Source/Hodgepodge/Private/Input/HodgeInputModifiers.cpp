// Copyright Epic Games, Inc. All Rights Reserved.

// 引入项目自定义的输入 Modifier。
#include "Input/HodgeInputModifiers.h"

// Enhanced Input 玩家输入相关类型。
#include "EnhancedPlayerInput.h"

// PlayerController，用于从输入对象找到对应的 LocalPlayer。
#include "GameFramework/PlayerController.h"

// 手柄瞄准灵敏度数据。
#include "Input/HodgeAimSensitivityData.h"

// 项目自定义 LocalPlayer，用于访问玩家本地配置。
#include "Core/LocalPlayer/HodgeLocalPlayerBase.h"
//#include "Settings/HodgeSettingsShared.h"

// 引入当前 cpp 对应的内联生成代码。
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeInputModifiers)

// 当前输入 Modifier 文件的日志分类。
DEFINE_LOG_CATEGORY_STATIC(LogHodgeInputModifiers, Log, All);

//////////////////////////////////////////////////////////////////////
// HodgeInputModifiersHelpers

// 输入 Modifier 的辅助函数集合。
namespace HodgeInputModifiersHelpers
{
	/** Returns the owning HodgeLocalPlayer of an Enhanced Player Input pointer */
	// 从 Enhanced Player Input 中获取其所属的 HodgeLocalPlayer。
	static UHodgeLocalPlayerBase* GetLocalPlayer(const UEnhancedPlayerInput* PlayerInput)
	{
		// 确保输入对象有效。
		if (PlayerInput)
		{
			// EnhancedPlayerInput 的 Outer 通常是对应的 PlayerController。
			if (APlayerController* PC = Cast<APlayerController>(PlayerInput->GetOuter()))
			{
				// 从 PlayerController 获取 LocalPlayer，并转换成项目自定义类型。
				return Cast<UHodgeLocalPlayerBase>(PC->GetLocalPlayer());
			}
		}

		// 无法找到对应 LocalPlayer 时返回空。
		return nullptr;
	}
}

//////////////////////////////////////////////////////////////////////
// UHodgeSettingBasedScalar

// 根据玩家设置中的倍率修改当前输入值。
FInputActionValue UHodgeSettingBasedScalar::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput,
                                                                     FInputActionValue CurrentValue, float DeltaTime)
{
	// Setting Based Scalar 不支持 Boolean 类型的输入。
	if (ensureMsgf(CurrentValue.GetValueType() != EInputActionValueType::Boolean,
	               TEXT("Setting Based Scalar modifier doesn't support boolean values.")))
	{
		// 获取当前输入所属的 LocalPlayer。
		// if (UHodgeLocalPlayerBase* LocalPlayer = HodgeInputModifiersHelpers::GetLocalPlayer(PlayerInput))
		// {
		// 获取共享设置对应的 UClass，用于通过反射查找属性。
		// 	const UClass* SettingsClass = UHodgeSettingsShared::StaticClass();

		// 获取玩家当前的共享设置。
		// 	UHodgeSettingsShared* SharedSettings = LocalPlayer->GetSharedSettings();

		// 判断属性缓存是否已经完整缓存 X/Y/Z 三个属性。
		//
		// 	const bool bHasCachedProperty = PropertyCache.Num() == 3;

		// 如果已经缓存属性，则直接使用缓存，否则通过属性名称进行反射查找。
		// 	const FProperty* XAxisValue = bHasCachedProperty
		// 		                              ? PropertyCache[0]
		// 		                              : SettingsClass->FindPropertyByName(XAxisScalarSettingName);

		// 查找 Y 轴对应的设置属性。
		// 	const FProperty* YAxisValue = bHasCachedProperty
		// 		                              ? PropertyCache[1]
		// 		                              : SettingsClass->FindPropertyByName(YAxisScalarSettingName);

		// 查找 Z 轴对应的设置属性。
		// 	const FProperty* ZAxisValue = bHasCachedProperty
		// 		                              ? PropertyCache[2]
		// 		                              : SettingsClass->FindPropertyByName(ZAxisScalarSettingName);

		// 第一次查找完成后，将三个属性保存下来，避免后续每帧重复反射查找。
		// 	if (PropertyCache.IsEmpty())
		// 	{
		// 		PropertyCache.Emplace(XAxisValue);
		// 		PropertyCache.Emplace(YAxisValue);
		// 		PropertyCache.Emplace(ZAxisValue);
		// 	}

		// 默认三个轴都使用 1.0 倍率，也就是不改变输入。
		// 	FVector ScalarToUse = FVector(1.0, 1.0, 1.0);

		// 根据输入值类型决定需要读取哪些轴的设置。
		// 	switch (CurrentValue.GetValueType())
		// 	{
		// Axis3D 同时使用 X/Y/Z 三个轴。
		// 	case EInputActionValueType::Axis3D:
		// 		ScalarToUse.Z = ZAxisValue ? *ZAxisValue->ContainerPtrToValuePtr<double>(SharedSettings) : 1.0;

		// 继续向下处理 Axis2D 的 X/Y 轴。
		// 	//[[fallthrough]];

		// Axis2D 使用 X/Y 两个轴。
		// 	case EInputActionValueType::Axis2D:
		// 		ScalarToUse.Y = YAxisValue ? *YAxisValue->ContainerPtrToValuePtr<double>(SharedSettings) : 1.0;

		// 继续向下处理 Axis1D 的 X 轴。
		// 	//[[fallthrough]];

		// Axis1D 只使用 X 轴。
		// 	case EInputActionValueType::Axis1D:
		// 		ScalarToUse.X = XAxisValue ? *XAxisValue->ContainerPtrToValuePtr<double>(SharedSettings) : 1.0;
		// 		break;
		// 	}

		// 将 X 轴倍率限制在配置的最小值和最大值之间。
		// 	ScalarToUse.X = FMath::Clamp(ScalarToUse.X, MinValueClamp.X, MaxValueClamp.X);

		// 将 Y 轴倍率限制在配置的最小值和最大值之间。
		// 	ScalarToUse.Y = FMath::Clamp(ScalarToUse.Y, MinValueClamp.Y, MaxValueClamp.Y);

		// 将 Z 轴倍率限制在配置的最小值和最大值之间。
		// 	ScalarToUse.Z = FMath::Clamp(ScalarToUse.Z, MinValueClamp.Z, MaxValueClamp.Z);

		// 将输入值与最终倍率相乘。
		// 	return CurrentValue.Get<FVector>() * ScalarToUse;
		// }
	}

	// 当前实现暂时不修改输入，直接返回原始值。
	return CurrentValue;
}

//////////////////////////////////////////////////////////////////////
// UHodgeInputModifierDeadZone

// 根据玩家设置处理摇杆 DeadZone。
FInputActionValue UHodgeInputModifierDeadZone::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput,
                                                                        FInputActionValue CurrentValue, float DeltaTime)
{
	// 获取当前输入值的类型。
	EInputActionValueType ValueType = CurrentValue.GetValueType();

	// 获取当前输入所属的 LocalPlayer。
	UHodgeLocalPlayerBase* LocalPlayer = HodgeInputModifiersHelpers::GetLocalPlayer(PlayerInput);

	// Boolean 输入不需要 DeadZone，没有 LocalPlayer 时也无法读取玩家设置。
	if (ValueType == EInputActionValueType::Boolean || !LocalPlayer)
	{
		// 条件不满足时保持原始输入。
		return CurrentValue;
	}

	// 获取玩家共享设置。
	// UHodgeSettingsShared* Settings = LocalPlayer->GetSharedSettings();

	// 确保共享设置有效。
	// ensure(Settings);

	// 根据当前处理的是移动摇杆还是视角摇杆，选择对应的 DeadZone。
	//
	// float LowerThreshold =
	// 	(DeadzoneStick == EDeadzoneStick::MoveStick)
	// 		? Settings->GetGamepadMoveStickDeadZone()
	// 		: Settings->GetGamepadLookStickDeadZone();

	// 将 DeadZone 限制在 0~1 范围。
	// LowerThreshold = FMath::Clamp(LowerThreshold, 0.0f, 1.0f);

	// DeadZone 的实际计算函数。
	// auto DeadZoneLambda = [LowerThreshold, this](const float AxisVal)
	// {
	// 输入绝对值先减去 DeadZone，再重新映射到 0~1。
	// 	// We need to translate and scale the input to the +/- 1 range after removing the dead zone.
	// 	return FMath::Min(
	// 			1.f, (FMath::Max(0.f, FMath::Abs(AxisVal) - LowerThreshold) / (UpperThreshold - LowerThreshold))) *
	// 		FMath::Sign(AxisVal);
	// };

	// 将输入转换成 Vector，方便处理 X/Y/Z 轴。
	// FVector NewValue = CurrentValue.Get<FVector>();

	// 根据 DeadZone 类型决定使用轴向 DeadZone 还是径向 DeadZone。
	// switch (Type)
	// {
	// 逐轴独立处理 DeadZone。
	// case EDeadZoneType::Axial:
	// 	NewValue.X = DeadZoneLambda(NewValue.X);
	// 	NewValue.Y = DeadZoneLambda(NewValue.Y);
	// 	NewValue.Z = DeadZoneLambda(NewValue.Z);
	// 	break;

	// 按输入向量整体长度计算 DeadZone。
	// case EDeadZoneType::Radial:
	// 如果是三维输入，则使用三维向量长度。
	// 	if (ValueType == EInputActionValueType::Axis3D)
	// 	{
	// 		NewValue = NewValue.GetSafeNormal() * DeadZoneLambda(NewValue.Size());

	// 如果是二维输入，则使用二维向量长度。
	// 	}
	// 	else if (ValueType == EInputActionValueType::Axis2D)
	// 	{
	// 		NewValue = NewValue.GetSafeNormal2D() * DeadZoneLambda(NewValue.Size2D());

	// 一维输入直接处理 X 轴。
	// 	}
	// 	else
	// 	{
	// 		NewValue.X = DeadZoneLambda(NewValue.X);
	// 	}
	// 	break;
	// }
	//
	// 返回处理后的输入值。
	// return NewValue;

	// 当前 DeadZone 的实际修改逻辑暂时被注释掉。
	return FInputActionValue();
}

// 获取 DeadZone 在 Enhanced Input 调试可视化中的颜色。
FLinearColor UHodgeInputModifierDeadZone::GetVisualizationColor_Implementation(
	FInputActionValue SampleValue, FInputActionValue FinalValue) const
{
	// Taken from UInputModifierDeadZone::GetVisualizationColor_Implementation
	// 该可视化逻辑参考 UE 原生 UInputModifierDeadZone 的实现。
	if (FinalValue.GetValueType() == EInputActionValueType::Boolean || FinalValue.GetValueType() ==
		EInputActionValueType::Axis1D)
	{
		// 一维输入为 0 时显示红色，否则显示黑色。
		return FLinearColor(FinalValue.Get<float>() == 0.f ? 1.f : 0.f, 0.f, 0.f);
	}

	// 二维输入根据 X/Y 轴是否为 0 分别计算红色强度。
	return FLinearColor(
		(FinalValue.Get<FVector2D>().X == 0.f ? 0.5f : 0.f) + (FinalValue.Get<FVector2D>().Y == 0.f ? 0.5f : 0.f), 0.f,
		0.f);
}

//////////////////////////////////////////////////////////////////////
// UHodgeInputModifierGamepadSensitivity

// 根据当前手柄灵敏度设置缩放输入。
FInputActionValue UHodgeInputModifierGamepadSensitivity::ModifyRaw_Implementation(
	const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	// You can't scale a boolean action type
	// Boolean 类型没有数值大小，因此不能进行灵敏度缩放。
	UHodgeLocalPlayerBase* LocalPlayer = HodgeInputModifiersHelpers::GetLocalPlayer(PlayerInput);

	// Boolean 输入、LocalPlayer 无效或灵敏度数据表不存在时保持原始输入。
	if (CurrentValue.GetValueType() == EInputActionValueType::Boolean || !LocalPlayer || !SensitivityLevelTable)
	{
		return CurrentValue;
	}

	// 获取玩家共享设置。
	// UHodgeSettingsShared* Settings = LocalPlayer->GetSharedSettings();

	// 确保共享设置有效。
	// ensure(Settings);

	// 根据当前是否为 ADS，选择普通视角灵敏度或瞄准灵敏度。
	//
	// const EHodgeGamepadSensitivity Sensitivity = (TargetingType == EHodgeTargetingType::Normal)
	// 	                                             ? Settings->GetGamepadLookSensitivityPreset()
	// 	                                             : Settings->GetGamepadTargetingSensitivityPreset();

	// 将灵敏度枚举转换成实际的浮点缩放倍率。
	// const float Scalar = SensitivityLevelTable->SensitivtyEnumToFloat(Sensitivity);

	// 将当前输入值乘以灵敏度倍率。
	// return CurrentValue.Get<FVector>() * Scalar;

	// 当前灵敏度的实际修改逻辑暂时被注释掉。
	return FInputActionValue();
}

//////////////////////////////////////////////////////////////////////
// UHodgeInputModifierAimInversion

// 根据玩家的瞄准反转设置修改输入轴。
FInputActionValue UHodgeInputModifierAimInversion::ModifyRaw_Implementation(
	const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	// 获取当前输入所属的 LocalPlayer。
	UHodgeLocalPlayerBase* LocalPlayer = HodgeInputModifiersHelpers::GetLocalPlayer(PlayerInput);

	// 无法获取 LocalPlayer 时无法读取玩家输入设置。
	if (!LocalPlayer)
	{
		// 保持原始输入。
		return CurrentValue;
	}

	// 获取玩家共享设置。
	// UHodgeSettingsShared* Settings = LocalPlayer->GetSharedSettings();

	// 确保共享设置有效。
	// ensure(Settings);

	// 将当前输入转换成 Vector，方便修改 X/Y 轴。
	//
	// FVector NewValue = CurrentValue.Get<FVector>();

	// 根据垂直轴反转设置决定是否反转 Y 轴。
	//
	// if (Settings->GetInvertVerticalAxis())
	// {
	// 	NewValue.Y *= -1.0f;
	// }

	// 根据水平轴反转设置决定是否反转 X 轴。
	//
	// if (Settings->GetInvertHorizontalAxis())
	// {
	// 	NewValue.X *= -1.0f;
	// }

	// 返回处理后的输入值。
	//
	// return NewValue;

	// 当前瞄准反转的实际修改逻辑暂时被注释掉。
	return FInputActionValue();
}
