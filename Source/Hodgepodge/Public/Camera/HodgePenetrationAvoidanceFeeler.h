// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "HodgePenetrationAvoidanceFeeler.generated.h"

/**
 * 用于摄像机防穿模的探针射线配置，每个 Feeler 代表一条用于检测障碍物的射线
 */
USTRUCT()
struct FHodgePenetrationAvoidanceFeeler
{
	GENERATED_BODY()

	// 探针相对于主射线方向的旋转偏移，用于让不同探针检测不同方向
	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
	FRotator AdjustmentRot;

	// 探针检测到世界场景物体时，对最终摄像机位置调整的影响权重
	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
	float WorldWeight;

	// 探针检测到 Pawn 时，对最终摄像机位置调整的影响权重，设置为 0 表示不检测 Pawn
	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
	float PawnWeight;

	// 探针进行碰撞检测时使用的碰撞范围大小
	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
	float Extent;

	// 如果上一帧没有检测到碰撞，至少间隔多少帧后才再次使用该探针进行检测
	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
	int32 TraceInterval;

	// 距离下一次允许使用该探针进行检测还剩多少帧
	UPROPERTY(transient)
	int32 FramesUntilNextTrace;


	// 使用默认值初始化探针配置
	FHodgePenetrationAvoidanceFeeler()
	   : AdjustmentRot(ForceInit)
	   , WorldWeight(0)
	   , PawnWeight(0)
	   , Extent(0)
	   , TraceInterval(0)
	   , FramesUntilNextTrace(0)
	{
	}

	// 使用指定参数初始化探针配置
	FHodgePenetrationAvoidanceFeeler(const FRotator& InAdjustmentRot,
							const float& InWorldWeight,
							const float& InPawnWeight,
							const float& InExtent,
							const int32& InTraceInterval = 0,
							const int32& InFramesUntilNextTrace = 0)
	   : AdjustmentRot(InAdjustmentRot)
	   , WorldWeight(InWorldWeight)
	   , PawnWeight(InPawnWeight)
	   , Extent(InExtent)
	   , TraceInterval(InTraceInterval)
	   , FramesUntilNextTrace(InFramesUntilNextTrace)
	{
	}
};