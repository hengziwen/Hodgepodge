// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// UE 原生 HUD Actor 基类。
// AHUD 与 PlayerController 关联，每个拥有 HUD 的玩家控制器通常对应一个 AHUD 实例。
#include "GameFramework/HUD.h"

#include "HodgeHUD.generated.h"

// Actor EndPlay 原因枚举前向声明。
// 用于 EndPlay() 生命周期函数参数。
namespace EEndPlayReason
{
	enum Type : int;
}

// Actor 前向声明。
// GetDebugActorList() 会收集需要参与 HUD Debug 显示的 Actor。
class AActor;

// UObject 前向声明。
class UObject;

/**
 * AHodgeHUD
 *
 * Hodge 项目的 HUD Actor 基类。
 *
 * Note that you typically do not need to extend or modify this class, instead you would
 * use an "Add Widget" action in your experience to add a HUD layout and widgets to it
 *
 * 通常不需要通过继承或修改 AHodgeHUD 来添加游戏 UI。
 *
 * 在 Experience / GameFeature 驱动的 UI 架构中，
 * 应该通过类似 “Add Widget” 的 GameFeature Action，
 * 根据当前 Experience 动态添加 HUD Layout 以及具体 Widget。
 *
 * This class exists primarily for debug rendering
 *
 * 因此这个类本身主要保留 AHUD 的基础职责，
 * 尤其用于游戏中的 Debug HUD / Debug Actor 绘制，
 * 而不是承担具体业务 UI 的创建和管理。
 */
UCLASS(Config = Game)
class HODGEPODGE_API AHodgeHUD : public AHUD
{
	GENERATED_BODY()

public:
	// 构造函数。
	// 使用 FObjectInitializer 完成 AHUD 基础对象初始化。
	AHodgeHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	//~UObject interface

	// UObject / Actor 组件预初始化阶段。
	// 在 BeginPlay 之前调用，可用于完成 HUD Actor 进入游戏前的初始化工作。
	virtual void PreInitializeComponents() override;

	//~End of UObject interface

	//~AActor interface

	// HUD Actor 正式进入游戏时调用。
	virtual void BeginPlay() override;

	// HUD Actor 离开游戏或所属 World 销毁时调用。
	// EndPlayReason 表示 Destroy、LevelTransition、EndPlayInEditor 等具体结束原因。
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//~End of AActor interface

	//~AHUD interface

	// 收集参与 HUD Debug 显示的 Actor。
	//
	// AHUD 的 Debug 系统可以利用这个 Actor 列表，
	// 配合 ShowDebug 等机制显示指定 Actor 的调试信息。
	virtual void GetDebugActorList(TArray<AActor*>& InOutList) override;

	//~End of AHUD interface
};
