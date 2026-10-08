// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// ControllerComponent 是专门依附于 AController 的组件基类。
// 当前 IndicatorManager 因此以 Controller 为宿主，负责管理该 Controller 对应玩家的 Indicator 数据。
#include "Components/ControllerComponent.h"

#include "HodgeIndicatorManagerComponent.generated.h"

// 前向声明 Controller。
// UHodgeIndicatorManagerComponent 会挂载在 Controller 上。
class AController;

// 前向声明 Indicator 描述对象。
// 每个 UIndicatorDescriptor 用于描述一个需要被 Indicator 系统展示的目标。
class UIndicatorDescriptor;

class UObject;
struct FFrame;

/**
 * @class UHodgeIndicatorManagerComponent
 *
 * Hodge Indicator 系统的核心管理组件。
 *
 * 挂载在 Controller 上，用于维护当前 Controller / 玩家所拥有的所有 IndicatorDescriptor。
 *
 * Gameplay 层可以通过 AddIndicator / RemoveIndicator 注册或移除 Indicator，
 * UI 层则可以通过 OnIndicatorAdded / OnIndicatorRemoved 事件监听 Indicator 数据变化，
 * 并根据 Descriptor 创建、更新或移除对应的屏幕 UI。
 */
UCLASS(BlueprintType, Blueprintable)
class HODGEPODGE_API UHodgeIndicatorManagerComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	// 构造当前 Controller 对应的 Indicator 管理组件。
	UHodgeIndicatorManagerComponent(const FObjectInitializer& ObjectInitializer);

	// 从指定 Controller 上获取 UHodgeIndicatorManagerComponent。
	// 为 IndicatorLibrary 以及其他 C++ 系统提供统一的组件查找入口。
	static UHodgeIndicatorManagerComponent* GetComponent(AController* Controller);

	// 向当前 Controller 的 Indicator 系统注册一个 IndicatorDescriptor。
	// 注册后，该 Descriptor 会进入 Indicators 数组，
	// 并可以通过 OnIndicatorAdded 通知 UI 层创建对应的 Indicator 表现。
	UFUNCTION(BlueprintCallable, Category = Indicator)
	void AddIndicator(UIndicatorDescriptor* IndicatorDescriptor);

	// 从当前 Controller 的 Indicator 系统移除一个 IndicatorDescriptor。
	// 移除后，可以通过 OnIndicatorRemoved 通知 UI 层删除对应的 Indicator 表现。
	UFUNCTION(BlueprintCallable, Category = Indicator)
	void RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor);

	// Indicator 增加 / 移除事件类型。
	// 每次事件广播时都会携带发生变化的 UIndicatorDescriptor。
	DECLARE_EVENT_OneParam(UHodgeIndicatorManagerComponent, FIndicatorEvent, UIndicatorDescriptor* Descriptor)

	// 当新的 IndicatorDescriptor 被添加到当前 Manager 时广播。
	FIndicatorEvent OnIndicatorAdded;

	// 当 IndicatorDescriptor 从当前 Manager 中移除时广播。
	FIndicatorEvent OnIndicatorRemoved;

	// 获取当前 Manager 正在管理的所有 IndicatorDescriptor。
	// 返回只读数组引用，外部可以遍历，但不能直接通过该接口修改内部数组。
	const TArray<UIndicatorDescriptor*>& GetIndicators() const { return Indicators; }

private:
	// 当前 Controller / 玩家正在管理的所有 IndicatorDescriptor。
	// Manager 负责保存这些 Descriptor，
	// UI 层可以根据这些描述数据生成对应的 Indicator Widget。
	UPROPERTY()
	TArray<TObjectPtr<UIndicatorDescriptor>> Indicators;
};
