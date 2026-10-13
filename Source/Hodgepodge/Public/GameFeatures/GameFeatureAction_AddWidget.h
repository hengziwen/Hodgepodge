/** @file GameFeatureAction_AddWidget.h
 * @brief 声明GameFeature界面注入动作相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// CommonUI 可激活 Widget 基类。
// HUD Layout 会以 UCommonActivatableWidget 的形式加入指定 CommonUI Layer。
#include "CommonActivatableWidget.h"

// 面向 World 的 GameFeatureAction 基类。
// 提供 GameFeature 激活后针对对应 World 执行 AddToWorld 的基础机制。
#include "GameFeatureAction_WorldActionBase.h"

// GameplayTag。
// 用于描述 CommonUI Layer ID 和 UIExtension Slot ID。
#include "GameplayTagContainer.h"

// UIExtension 系统。
// 动态 HUD Element 会通过 FUIExtensionHandle 注册到指定 ExtensionPoint。
#include "UI/Extension/UIExtensionSystem.h"

#include "GameFeatureAction_AddWidget.generated.h"

// World Context 前向声明。
// AddToWorld 时用于描述当前 GameFeature 正在作用的 World。
struct FWorldContext;

// GameFrameworkComponentManager 组件扩展请求 Handle 前向声明。
// 用于保存当前 GameFeature 注册的 Actor Extension 请求生命周期。
struct FComponentRequestHandle;

/**
 * 描述一条 HUD Layout 注入请求。
 *
 * 表示：
 * 将哪个 CommonActivatableWidget Layout，
 * 添加到哪个 CommonUI Layer 中。
 */
USTRUCT()
struct FHodgeHUDLayoutRequest
{
	GENERATED_BODY()

	// The layout widget to spawn

	// 需要创建并加入 HUD 的 Layout Widget Class。
	// 使用 SoftClass 引用，避免配置该 GameFeatureAction 时
	// 直接形成对 Layout Widget 资源的硬引用。
	// AssetBundles="Client" 表示该资源属于客户端资源 Bundle，
	// 方便 AssetManager / Experience 加载阶段收集客户端所需资源。
	UPROPERTY(EditAnywhere, Category=UI, meta=(AssetBundles="Client"))
	TSoftClassPtr<UCommonActivatableWidget> LayoutClass;

	// The layer to insert the widget in

	// Layout 需要插入的 CommonUI Layer ID。
	// Categories="UI.Layer" 限制编辑器中选择的 GameplayTag
	// 应属于 UI.Layer Tag 层级。
	// 例如： UI.Layer.Game
	UPROPERTY(EditAnywhere, Category=UI, meta=(Categories="UI.Layer"))
	FGameplayTag LayerID;
};


/**
 * 描述一条普通 HUD Element 注入请求。
 *
 * 与 FHodgeHUDLayoutRequest 不同，
 * 这里不是向 CommonUI Layer 中添加一个根 Layout，
 * 而是通过 UIExtensionSystem
 * 将 Widget 动态注册到指定的 UI Extension Slot。
 */
USTRUCT()
struct FHodgeHUDElementEntry
{
	GENERATED_BODY()

	// The widget to spawn
	// 需要注入 HUD 的具体 Widget Class。
	UPROPERTY(EditAnywhere, Category=UI, meta=(AssetBundles="Client"))
	TSoftClassPtr<UUserWidget> WidgetClass;

	// The slot ID where we should place this widget
	// 该 Widget 要注入的 UIExtension Slot ID。这个 Tag 会与 UUIExtensionPointWidget 配置的ExtensionPointTag 进行匹配。
	// 例如：UI.HUD.Right
	UPROPERTY(EditAnywhere, Category = UI)
	FGameplayTag SlotID;
};

//////////////////////////////////////////////////////////////////////
// UGameFeatureAction_AddWidget

/**
 * GameFeatureAction responsible for granting abilities (and attributes) to actors of a specified type.
 * 用于在 GameFeature 激活期间向玩家 HUD 动态添加 UI。
 * 主要负责两类 UI：
 * 1. Layout
 *    将 UCommonActivatableWidget 类型的 HUD Layout
 *    添加到指定 CommonUI Layer。
 * 2. Widget
 *    将普通 UUserWidget 通过 UIExtensionSystem
 *    注册到指定的 UI Extension Slot。
 * 当 GameFeature 停用时，
 * 该 Action 会利用保存的 Layout 实例和 ExtensionHandle
 * 将之前添加的 UI 完整清理掉。
 */
UCLASS(MinimalAPI, meta = (DisplayName = "Add Widgets"))
class UGameFeatureAction_AddWidgets final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()
#if WITH_EDITOR
    friend class UHodgeUIAuthoringLibrary;
#endif

public:
	//~ Begin UGameFeatureAction interface

	// GameFeature 开始停用时调用。
	// 用于清理由当前 GameFeature 添加的 HUD Layout、 UIExtension 以及 Actor Extension 请求。
	/** 处理游戏功能扩展Deactivating。
	 * @param Context 本次操作的上下文。
	 */
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

#if WITH_EDITORONLY_DATA

	// 向当前 GameFeature 的 AssetBundle 中追加额外资源。
	// LayoutClass / WidgetClass 使用 SoftClass 引用，因此这里可以把这些 UI 资源加入对应 Bundle，让 GameFeature / Experience 的资源加载阶段提前收集它们。
	/** 添加Additional资产资源包数据。
	 * @param AssetBundleData 资产资源包数据，由调用方传入。
	 */
	virtual void AddAdditionalAssetBundleData(FAssetBundleData& AssetBundleData) override;

#endif

	//~ End UGameFeatureAction interface

	//~ Begin UObject interface

#if WITH_EDITOR

	// 编辑器数据校验。
	// 用于检查 LayoutClass、LayerID、WidgetClass、SlotID 等配置是否合法。
	/** 在编辑器校验当前配置并收集诊断。
	 * @param Context 本次操作的上下文。
	 * @return 当前配置的校验结果。
	 */
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;

#endif

	//~ End UObject interface

private:
	// Layout to add to the HUD

	// 当前 GameFeature 激活后需要加入玩家 HUD 的 Layout 列表。
	// 每一项描述：LayerID -> LayoutClass
	// 例如： UI.Layer.Game -> WBP_HodgeHUDLayout
	UPROPERTY(EditAnywhere, Category=UI, meta=(TitleProperty="{LayerID} -> {LayoutClass}"))
	// 布局的记录集合。
	TArray<FHodgeHUDLayoutRequest> Layout;

	// Widgets to add to the HUD

	// 当前 GameFeature 激活后需要动态注入 HUD ExtensionPoint 的 Widget 列表。
	// 每一项描述：SlotID -> WidgetClass
	//例如： UI.HUD.Right -> WBP_QuestTracker
	UPROPERTY(EditAnywhere, Category=UI, meta=(TitleProperty="{SlotID} -> {WidgetClass}"))
	// 控件集合的记录集合。
	TArray<FHodgeHUDElementEntry> Widgets;

private:
	/**
	 * 保存针对某一个目标 Actor 添加 UI 后产生的运行时数据。
	 * 主要用于在 Actor 被移除或 GameFeature 停用时，
	 * 精确撤销之前由当前 Action 添加的 UI。
	 */
	struct FPerActorData
	{
		// 本次操作关联的Actor。
		TWeakObjectPtr<AActor> Actor;
		// 玩家的弱引用，不延长目标对象生命周期。
		TWeakObjectPtr<ULocalPlayer> Player;
		// 当前操作使用的根对象。
		TWeakObjectPtr<class UHodgePrimaryGameLayout> Root;
		// 管理器的弱引用，不延长目标对象生命周期。
		TWeakObjectPtr<class UHodgeUIManagerSubsystem> Manager;
		// 根节点变化委托对应的事件通知。
		FDelegateHandle RootChangedDelegate;
		// 控制是否Added的状态标记。
		bool bAdded = false;
		// 当前 Action 为这个 Actor 添加过的 HUD Layout 实例。
		TArray<TWeakObjectPtr<UCommonActivatableWidget>> LayoutsAdded;

		// 当前 Action 为这个 Actor 注册到 UIExtensionSystem 的 Extension Handle。
		TArray<FUIExtensionHandle> ExtensionHandles;
	};

	/**
	 * 保存一次 GameFeature State Change Context
	 * 对应的全部运行时 UI 扩展数据。
	 *
	 * 一个 Context 下可能存在多个目标 Actor，
	 * 因此这里继续按 Actor 分组保存。
	 */
	struct FPerContextData
	{
		// 当前 Context 注册到 GameFrameworkComponentManager 的扩展请求。
		TArray<TSharedPtr<FComponentRequestHandle>> ComponentRequests;

		// Actor -> 当前 Action 为该 Actor 添加的 UI 数据。
		TMap<FObjectKey, FPerActorData> ActorData;
	};

	// 按 GameFeatureStateChangeContext 保存当前 Action 的运行时状态。
	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

	//~ Begin UGameFeatureAction_WorldActionBase interface

	// 当前 GameFeature 被应用到某个 World 时调用。
	// 通常会在这里通过 GameFrameworkComponentManager注册针对目标 Actor（例如 AHodgeHUD）的 Extension Handler，
	// 等待对应 Actor 出现 / Ready，再执行真正的 AddWidgets。
	/** 添加目标世界。
	 * @param WorldContext 世界上下文，由调用方传入。
	 * @param ChangeContext 变更上下文，由调用方传入。
	 */
	virtual void AddToWorld(const FWorldContext& WorldContext,
	                        const FGameFeatureStateChangeContext& ChangeContext) override;

	//~ End UGameFeatureAction_WorldActionBase interface

	// 清理某个 Context 下由当前 Action 创建的全部运行时状态。
	// 包括：Actor 对应的 Layout、UIExtension、ComponentRequest
	/** 恢复本对象的初始状态。
	 * @param ActiveData 活动数据，由调用方传入。
	 */
	void Reset(FPerContextData& ActiveData);
	/** 清空Actor对象Contents。
	 * @param Data 当前操作使用的数据。
	 */
	void ClearActorContents(FPerActorData& Data);

	// GameFrameworkComponentManager 的 Actor Extension 事件处理函数。
	// 当目标 Actor 被添加、Ready、移除等事件发生时，根据 EventName 决定添加还是移除对应 UI。
	/** 消费GameFramework扩展事件并配对管理注入内容。
	 * @param Actor 本次操作关联的Actor。
	 * @param EventName 事件名称，由调用方传入。
	 * @param ChangeContext 变更上下文，由调用方传入。
	 */
	void HandleActorExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);

	// 为指定 Actor 添加当前 Action 配置的 HUD Layout 和 Widget Extension。
	/** 添加控件集合。
	 * @param Actor 本次操作关联的Actor。
	 * @param ActiveData 活动数据，由调用方传入。
	 */
	void AddWidgets(AActor* Actor, FPerContextData& ActiveData);

	// 移除当前 Action 之前为指定 Actor 添加的 HUD Layout 和 Widget Extension。
	/** 移除控件集合。
	 * @param Actor 本次操作关联的Actor。
	 * @param ActiveData 活动数据，由调用方传入。
	 */
	void RemoveWidgets(AActor* Actor, FPerContextData& ActiveData);
};
