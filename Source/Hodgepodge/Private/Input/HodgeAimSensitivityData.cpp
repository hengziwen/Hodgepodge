// Copyright Epic Games, Inc. All Rights Reserved.

// 引入项目自定义的瞄准灵敏度数据资产。
#include "Input/HodgeAimSensitivityData.h"

//#include "Settings/HodgeSettingsShared.h"

// 引入当前 cpp 对应的 UE 内联生成代码。
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAimSensitivityData)

// 构造瞄准灵敏度数据资产。
UHodgeAimSensitivityData::UHodgeAimSensitivityData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// // 初始化灵敏度档位与实际浮点倍率之间的映射。
	// SensitivityMap =
	// {
	// 	// 较慢的灵敏度档位，对应 0.5 倍输入倍率。
	// 	// {EHodgeGamepadSensitivity::Slow, 0.5f},
	//
	// 	// 稍慢的灵敏度档位，对应 0.75 倍输入倍率。
	// 	// {EHodgeGamepadSensitivity::SlowPlus, 0.75f},
	//
	// 	// 较慢偏正常的灵敏度档位，对应 0.9 倍输入倍率。
	// 	// {EHodgeGamepadSensitivity::SlowPlusPlus, 0.9f},
	//
	// 	// 默认正常灵敏度，对应 1.0 倍输入倍率。
	// 	// {EHodgeGamepadSensitivity::Normal, 1.0f},
	//
	// 	// 稍快的正常灵敏度，对应 1.1 倍输入倍率。
	// 	// {EHodgeGamepadSensitivity::NormalPlus, 1.1f},
	//
	// 	// 更快的正常灵敏度，对应 1.25 倍输入倍率。
	// 	// {EHodgeGamepadSensitivity::NormalPlusPlus, 1.25f},
	//
	// 	// 快速灵敏度，对应 1.5 倍输入倍率。
	// 	// {EHodgeGamepadSensitivity::Fast, 1.5f},
	//
	// 	// 更快的灵敏度，对应 1.75 倍输入倍率。
	// 	// {EHodgeGamepadSensitivity::FastPlus, 1.75f},
	//
	// 	// 非常快的灵敏度，对应 2.0 倍输入倍率。
	// 	// {EHodgeGamepadSensitivity::FastPlusPlus, 2.0f},
	//
	// 	// 极高灵敏度，对应 2.5 倍输入倍率。
	// 	// {EHodgeGamepadSensitivity::Insane, 2.5f},
	// };
}
//
// // 将手柄灵敏度枚举转换为实际的浮点倍率。
// const float UHodgeAimSensitivityData::SensitivtyEnumToFloat(const EHodgeGamepadSensitivity InSensitivity) const
// {
// 	// 根据灵敏度枚举在映射表中查找对应的倍率。
// 	if (const float* Sens = SensitivityMap.Find(InSensitivity))
// 	{
// 		// 找到对应配置时返回实际灵敏度倍率。
// 		return *Sens;
// 	}
//
// 	// 找不到配置时使用 1.0 倍，表示不修改原始输入。
// 	return 1.0f;
// }
