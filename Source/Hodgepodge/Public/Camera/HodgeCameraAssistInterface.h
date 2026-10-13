/** @file HodgeCameraAssistInterface.h
 * @brief 声明相机避障协作接口相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "HodgeCameraAssistInterface.generated.h"

// 摄像机辅助接口的 Unreal 反射包装，用于支持蓝图类型识别
UINTERFACE(BlueprintType)
class UHodgeCameraAssistInterface : public UInterface
{
	GENERATED_BODY()
};

// 摄像机辅助接口，提供摄像机防穿透相关的扩展能力
class IHodgeCameraAssistInterface
{
	GENERATED_BODY()

public:
	// 获取允许摄像机穿透的 Actor 列表，例如自身角色、载具或其他不希望挡住摄像机的目标
	/** 获取IgnoredActors关联相机Pentration。
	 * @param OutActorsAllowPenetration 输出ActorsAllowPenetration。
	 */
	virtual void GetIgnoredActorsForCameraPentration(TArray<const AActor*>& OutActorsAllowPenetration) const
	{
	}

	// 获取摄像机需要重点防止穿透的目标 Actor，通常就是当前 ViewTarget
	/** 获取相机PreventPenetration目标。
	 * @return 相机PreventPenetration目标的当前值或引用。
	 */
	virtual TOptional<AActor*> GetCameraPreventPenetrationTarget() const
	{
		// 返回空值表示使用摄像机默认的 ViewTarget 作为防穿透目标
		return TOptional<AActor*>();
	}

	// 当摄像机穿入防穿透目标时调用，可用于隐藏目标模型等额外处理
	/** 处理相机Penetrating目标。 */
	virtual void OnCameraPenetratingTarget()
	{
	}
};
