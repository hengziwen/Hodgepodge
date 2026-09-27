// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// Unreal 基础类型、常用宏等核心定义。
#include "CoreMinimal.h"

// UObject / UCLASS / UINTERFACE 等反射相关宏定义。
#include "UObject/ObjectMacros.h"

// Unreal Interface 基类定义。
#include "UObject/Interface.h"

#include "IActorIndicatorWidget.generated.h"

// Actor 前向声明。
class AActor;

// Indicator 描述对象前向声明。
// Widget 会通过该 Descriptor 获取当前 Indicator 对应的数据、目标组件以及显示配置。
class UIndicatorDescriptor;

// Indicator Widget 的 UObject 接口声明。
// BlueprintType 允许蓝图 Widget 实现并使用该接口。
UINTERFACE(BlueprintType)
class HODGEPODGE_API UIndicatorWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

// Indicator Widget 实际需要实现的接口。
// 用于统一规定 Indicator UI 与 UIndicatorDescriptor 之间的绑定 / 解绑方式。
class IIndicatorWidgetInterface
{
	GENERATED_BODY()

public:
	// 将当前 Widget 与指定 IndicatorDescriptor 绑定。
	// Indicator UI 系统创建 / 获取 Widget 后，可以通过该接口把对应 Descriptor 传递给 Widget，
	// Widget 随后可以从 Descriptor 中取得 DataObject 等数据并刷新自己的显示内容。
	//
	// BlueprintNativeEvent 允许该函数由 C++ 提供默认实现，
	// 同时也允许蓝图 Widget 重写对应事件。
	UFUNCTION(BlueprintNativeEvent, Category = "Indicator")
	void BindIndicator(UIndicatorDescriptor* Indicator);

	// 解除当前 Widget 与指定 IndicatorDescriptor 的绑定。
	// Indicator 被移除或 Widget 被回收时，可以通过该接口通知 Widget 清理
	// 与 Descriptor / DataObject 相关的事件监听、引用或其他运行时状态。
	//
	// BlueprintNativeEvent 允许 C++ 与蓝图共同实现该接口。
	UFUNCTION(BlueprintNativeEvent, Category = "Indicator")
	void UnbindIndicator(const UIndicatorDescriptor* Indicator);
};
