/** @file UIExtensionPointWidget.h
 * @brief 声明UI扩展插槽控件相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// 动态 Entry 容器基类。
// UUIExtensionPointWidget 继承它来动态创建、保存和移除实际的 UUserWidget。
#include "Components/DynamicEntryBoxBase.h"

// UI Extension 系统定义。
// 包含 ExtensionPoint、Extension、Handle、Request 等核心数据结构。
#include "UIExtensionSystem.h"

#include "UIExtensionPointWidget.generated.h"

// Widget 蓝图编译检查日志接口。
// 编辑器下用于检查 ExtensionPoint 配置是否合法。
class IWidgetCompilerLog;

// Hodge 项目的 LocalPlayer 类型。
// 用于按当前本地玩家注册对应 Context 的 ExtensionPoint。
class UHodgeLocalPlayerBase;

// 玩家状态。
// 可以作为 ContextObject，使 ExtensionPoint 只接收特定玩家相关的 Extension。
class APlayerState;

/**
 * A slot that defines a location in a layout, where content can be added later
 *
 * 定义 UI 布局中的一个“扩展插槽”。
 *
 * 这个 Widget 本身主要负责占据 HUD 中的某个位置，
 * 后续其他系统可以通过 UUIExtensionSubsystem
 * 向这个 ExtensionPoint 动态添加 UI 内容。
 */
UCLASS()
class HODGEPODGE_API UUIExtensionPointWidget : public UDynamicEntryBoxBase
{
	GENERATED_BODY()

public:
	// 在原生树构建阶段配置插槽；运行后的修改应通过重建控件进行。
	/** 设置扩展Point标签。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 */
	void SetExtensionPointTag(FGameplayTag Tag);
	// 当 Extension 携带的是普通 Data，而不是直接携带 WidgetClass 时，
	// 通过这个委托根据 DataItem 决定应该创建哪一种 UUserWidget。
	//
	// 输入：Extension 携带的 DataItem。
	// 输出：用于展示该 DataItem 的 Widget Class。
	DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(TSubclassOf<UUserWidget>, FOnGetWidgetClassForData, UObject*, DataItem);

	// 当根据 Data 创建出 Widget 后，
	// 通过这个委托将 DataItem 交给 Widget 做进一步初始化和数据绑定。
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnConfigureWidgetForData, UUserWidget*, Widget, UObject*, DataItem);

	// 构造函数。
	/** 构造UUI扩展Point控件并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	UUIExtensionPointWidget(const FObjectInitializer& ObjectInitializer);

	//~UWidget interface

	// 释放当前 Widget 持有的 Slate 资源。
	// ExtensionPointWidget 从 UI Tree 中释放时也可以在这里处理对应的注册状态。
	/** 释放Slate界面Resources。
	 * @param bReleaseChildren 是否释放子项的控制标记。
	 */
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	// 重建当前 UMG Widget 对应的底层 Slate Widget。
	// Widget 真正进入 UI Tree 时会经过这里。
	/** 根据当前属性创建对应的Slate控件。
	 * @return 本次计算、查询或创建得到的T共享RefS控件。
	 */
	virtual TSharedRef<SWidget> RebuildWidget() override;

#if WITH_EDITOR
	// 编辑器编译 Widget Blueprint 时检查当前 ExtensionPoint 的默认配置是否合法。
	/** 校验已编译Defaults。
	 * @param CompileLog 编译Log，由调用方传入。
	 */
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;
#endif

	//~End of UWidget interface

private:
	// 重置当前 ExtensionPoint。
	//
	// 负责清理已经注册到 UUIExtensionSubsystem 的 ExtensionPoint Handle，
	// 以及当前由 Extension 创建出来的相关 UI 状态。
	/** 重置扩展Point。 */
	void ResetExtensionPoint();

	// 根据当前 ExtensionPointWidget 的配置，
	// 向 UUIExtensionSubsystem 注册对应的 ExtensionPoint。
	/** 注册扩展Point。 */
	void RegisterExtensionPoint();

	// 为指定 LocalPlayer / PlayerState 注册带玩家 Context 的 ExtensionPoint。
	//
	// 这样即使多个玩家使用相同 ExtensionPointTag，
	// 也可以通过 ContextObject 区分具体属于哪个玩家。
	/** 注册扩展Point关联玩家状态。
	 * @param LocalPlayer 本地玩家，由调用方传入。
	 * @param PlayerState 玩家持有的持久状态与ASC入口。
	 */
	void RegisterExtensionPointForPlayerState(UHodgeLocalPlayerBase* LocalPlayer, APlayerState* PlayerState);

	// UUIExtensionSubsystem 的 ExtensionPoint 回调。
	//
	// 当满足当前 ExtensionPoint 契约的 Extension 被添加或移除时进入这里。
	//
	// Added：
	// 根据 Request.Data 创建对应 Widget，并加入当前动态 Entry 容器。
	//
	// Removed：
	// 根据 Request.ExtensionHandle 找到之前创建的 Widget，并将其移除。
	/** 处理添加Or移除扩展。
	 * @param Action 动作，由调用方传入。
	 * @param Request 请求，由调用方传入。
	 */
	void OnAddOrRemoveExtension(EUIExtensionAction Action, const FUIExtensionRequest& Request);

protected:
	/** The tag that defines this extension point */

	// 当前 Widget 所代表的 UI ExtensionPoint Tag。
	//
	// 例如：
	// UI.HUD.Left
	// UI.HUD.Right
	// UI.HUD.Bottom.Skill
	//
	// 其他系统注册到匹配 Tag 的 Extension，
	// 才有机会被添加到当前 Widget 中。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Extension")
	FGameplayTag ExtensionPointTag;

	/** How exactly does the extension need to match the extension point tag. */

	// 当前 ExtensionPoint 使用的 GameplayTag 匹配方式。
	//
	// ExactMatch：
	// 只接受与 ExtensionPointTag 完全相同的 Extension。
	//
	// PartialMatch：
	// 还可以接受该 Tag 子层级下注册的 Extension。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Extension")
	EUIExtensionPointMatch ExtensionPointTagMatch = EUIExtensionPointMatch::ExactMatch;

	// 当前 ExtensionPoint 允许接收的 Data 类型。
	//
	// Extension 携带的数据必须属于这里声明的 Class，
	// 或满足对应接口契约，才能通过 UUIExtensionSubsystem 的匹配检查。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Extension")
	TArray<TObjectPtr<UClass>> DataClasses;

	// 当 Extension.Data 不是直接可创建的 WidgetClass，
	// 而是普通业务 Data 时，
	// 通过该委托把 Data 转换成对应的 Widget Class。
	//
	// 例如：
	// QuestData → WBP_QuestEntry
	// TeamMemberData → WBP_TeamMemberEntry
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Extension", meta=( IsBindableEvent="True" ))
	FOnGetWidgetClassForData GetWidgetClassForData;

	// 根据 Data 创建 Widget 后调用的配置委托。
	//
	// 用于把 Extension 携带的业务数据继续交给新创建的 Widget，
	// 完成文本、图标、事件等具体初始化。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Extension", meta=( IsBindableEvent="True" ))
	FOnConfigureWidgetForData ConfigureWidgetForData;

	// 当前 Widget 已经注册到 UUIExtensionSubsystem 的所有 ExtensionPoint Handle。
	//
	// 之所以是数组，是因为同一个 ExtensionPointWidget
	// 可能根据不同 Context 注册多个 ExtensionPoint。
	//
	// Widget 销毁 / 重建时可以通过这些 Handle 精确注销。
	TArray<FUIExtensionPointHandle> ExtensionPointHandles;

	// Extension 与实际创建出来的 UUserWidget 之间的映射关系。
	//
	// Key：
	// 哪一条 Extension。
	//
	// Value：
	// 这条 Extension 对应创建出来的 Widget。
	//
	// 当收到 Removed 通知时，
	// 可以根据 ExtensionHandle 精确找到并删除对应 Widget。
	UPROPERTY(Transient)
	TMap<FUIExtensionHandle, TObjectPtr<UUserWidget>> ExtensionMapping;
	// 已绑定本地玩家的弱引用，不延长目标对象生命周期。
	TWeakObjectPtr<UHodgeLocalPlayerBase> BoundLocalPlayer;
	// 玩家状态委托对应的事件通知。
	FDelegateHandle PlayerStateDelegate;
	// 玩家状态Point对应操作的配对句柄。
	FUIExtensionPointHandle PlayerStatePoint;
};
