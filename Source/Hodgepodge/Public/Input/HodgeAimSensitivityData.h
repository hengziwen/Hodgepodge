// Copyright Epic Games, Inc. All Rights Reserved.

// 防止头文件被重复包含。
#pragma once

// UDataAsset 和 UPrimaryDataAsset 数据资产基类。
#include "Engine/DataAsset.h"

// 当前头文件对应的 UE 反射代码生成文件。
#include "HodgeAimSensitivityData.generated.h"

// 手柄灵敏度枚举类型的前置声明。
//enum class EHodgeGamepadSensitivity : uint8;

// UE 基础对象类型。
class UObject;

/** Defines a set of gamepad sensitivity to a float value. */
// 定义手柄灵敏度枚举与实际浮点倍率之间的映射。
UCLASS(BlueprintType, Const,
	Meta = (DisplayName = "Hodge Aim Sensitivity Data", ShortTooltip =
		"Data asset used to define a map of Gamepad Sensitivty to a float value."))
class HODGEPODGE_API UHodgeAimSensitivityData : public UPrimaryDataAsset
{
	// 开启 UE 反射、序列化和 Primary Asset 支持。
	GENERATED_BODY()

public:
	// 构造瞄准灵敏度数据资产。
	UHodgeAimSensitivityData(const FObjectInitializer& ObjectInitializer);

	// // 将手柄灵敏度枚举转换为实际使用的浮点倍率。
	// const float SensitivtyEnumToFloat(const EHodgeGamepadSensitivity InSensitivity) const;
	//
	// protected:
	// /** Map of SensitivityMap settings to their corresponding float */
	// // 保存每个手柄灵敏度档位对应的实际浮点倍率。
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// TMap<EHodgeGamepadSensitivity, float> SensitivityMap;
};
