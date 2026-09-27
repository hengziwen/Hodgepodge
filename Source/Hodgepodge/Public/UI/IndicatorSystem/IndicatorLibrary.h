// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// UE 蓝图函数库基类。
// 继承该类后，可以向蓝图暴露不依赖具体 UObject 实例的静态工具函数。
#include "Kismet/BlueprintFunctionLibrary.h"

#include "IndicatorLibrary.generated.h"

// 前向声明 Controller。
// Indicator 系统通常会以玩家 Controller 作为查找对应 IndicatorManager 的入口。
class AController;

// 前向声明 Hodge 项目的 Indicator 管理组件。
// 该组件负责管理当前 Controller 对应的各种 Indicator。
class UHodgeIndicatorManagerComponent;

class UObject;
struct FFrame;

// Hodge Indicator 系统的蓝图工具函数库。
// 用于给蓝图提供访问 Indicator 系统相关对象的统一入口。
UCLASS()
class HODGEPODGE_API UIndicatorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// 构造 Indicator 蓝图函数库。
	UIndicatorLibrary();

	/**  */
	// 根据指定 Controller 获取其对应的 UHodgeIndicatorManagerComponent。
	// 蓝图可以通过这个函数快速取得当前玩家的 Indicator 管理组件，
	// 然后进一步执行添加、移除或查询 Indicator 等操作。
	UFUNCTION(BlueprintCallable, Category = Indicator)
	static UHodgeIndicatorManagerComponent* GetIndicatorManagerComponent(AController* Controller);
};
