// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFeatures/GameFeatureAction_AddWidget.h"

// GameFrameworkComponentManager。
// 用于监听 AHodgeHUD 这类 GameFramework Receiver 的创建、Ready、移除等扩展事件。
#include "Components/GameFrameworkComponentManager.h"

// GameInstance。
// 用于获取挂载在 GameInstance 上的 GameFrameworkComponentManager Subsystem。
#include "Engine/GameInstance.h"

// 面向 World 的 GameFeatureAction 基类。
#include "GameFeatures/GameFeatureAction_WorldActionBase.h"

// GameFeature Subsystem 配置。
// 用于取得客户端资源加载对应的 AssetBundle 名称。
#include "GameFeaturesSubsystemSettings.h"

// CommonUI 扩展工具。
// 当前用于 Push / Pop CommonUI Layer 的相关代码暂时被注释。
//#include "CommonUIExtensions.h"

// Hodge HUD Actor。
// 当前 GameFeatureAction 会监听 AHodgeHUD 的生命周期，
// 并以 HUD Actor 作为给玩家添加 / 移除 UI 的生命周期锚点。
#include "Core/HUD/HodgeHUD.h"

#if WITH_EDITOR

// UE 数据校验系统。
// 编辑器下用于检查 Layout / Widget 配置是否合法。
#include "Misc/DataValidation.h"

#endif

#include "UI/Subsystem/HodgeUIManagerSubsystem.h"
#include "UI/Foundation/HodgePrimaryGameLayout.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddWidget)

// 当前文件本地化文本使用的命名空间。
#define LOCTEXT_NAMESPACE "HodgeGameFeatures"

//////////////////////////////////////////////////////////////////////
// UGameFeatureAction_AddWidgets

// 当前 GameFeature 开始停用时调用。
void UGameFeatureAction_AddWidgets::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	// 先执行父类 GameFeatureAction 的停用逻辑。
	Super::OnGameFeatureDeactivating(Context);

	// 根据当前 GameFeature 的状态切换 Context，找到这个 Context 激活期间保存的全部运行时 UI 数据。
	FPerContextData* ActiveData = ContextData.Find(Context);

	// 正常情况下当前 Context 应该存在对应的 ActiveData。
	if ensure(ActiveData)
	{
		// 清理当前 Context 注册的 ComponentRequest以及通过 UIExtensionSystem 注册的所有 Extension。
		Reset(*ActiveData);
	}
}

#if WITH_EDITORONLY_DATA

// 收集当前 GameFeatureAction 额外依赖的 AssetBundle 资源。
void UGameFeatureAction_AddWidgets::AddAdditionalAssetBundleData(FAssetBundleData& AssetBundleData)
{
	// 遍历当前 Action 配置的所有 HUD Element。
	for (const FHodgeHUDElementEntry& Entry : Widgets)
	{
		// 将 WidgetClass 对应的资源加入客户端加载 Bundle。
		AssetBundleData.AddBundleAsset(UGameFeaturesSubsystemSettings::LoadStateClient, Entry.WidgetClass.ToSoftObjectPath().GetAssetPath());
	}
}

#endif

#if WITH_EDITOR

// 编辑器下校验当前 AddWidgets Action 的配置是否合法。
EDataValidationResult UGameFeatureAction_AddWidgets::IsDataValid(FDataValidationContext& Context) const
{
	// 先执行父类的数据校验，再将当前类自己的校验结果合并进来。
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);
	{
		// 当前正在检查的 Layout 配置索引。
		int32 EntryIndex = 0;

		// 检查所有 HUD Layout 请求。
		for (const FHodgeHUDLayoutRequest& Entry : Layout)
		{
			// Layout 必须配置有效的 LayoutClass。
			if (Entry.LayoutClass.IsNull())
			{
				// 标记整个 Action 配置无效。
				Result = EDataValidationResult::Invalid;

				// 输出具体是哪一个 Layout Entry 没有配置 WidgetClass。
				Context.AddError(FText::Format(LOCTEXT("LayoutHasNullClass", "Null WidgetClass at index {0} in Layout"), FText::AsNumber(EntryIndex)));
			}

			// Layout 必须配置有效的 CommonUI Layer GameplayTag。
			if (!Entry.LayerID.IsValid())
			{
				// 标记配置无效。
				Result = EDataValidationResult::Invalid;

				// 输出具体是哪一个 Layout Entry 没有配置 LayerID。
				Context.AddError(FText::Format(LOCTEXT("LayoutHasNoTag", "LayerID is not set at index {0} in Widgets"), FText::AsNumber(EntryIndex)));
			}

			// 继续检查下一个 Layout Entry。
			++EntryIndex;
		}
	}

	{
		// 当前正在检查的 HUD Element 配置索引。
		int32 EntryIndex = 0;

		// 检查所有通过 UIExtensionSystem 注入的 Widget 配置。
		for (const FHodgeHUDElementEntry& Entry : Widgets)
		{
			// 每一个 HUD Element 都必须配置有效 WidgetClass。
			if (Entry.WidgetClass.IsNull())
			{
				// 标记整个 Action 配置无效。
				Result = EDataValidationResult::Invalid;

				// 输出具体是哪一个 Widget Entry 没有配置 WidgetClass。
				Context.AddError(FText::Format(LOCTEXT("EntryHasNullClass", "Null WidgetClass at index {0} in Widgets"), FText::AsNumber(EntryIndex)));
			}

			// 每一个 HUD Element 都必须配置有效的 UIExtension SlotID。
			if (!Entry.SlotID.IsValid())
			{
				// 标记配置无效。
				Result = EDataValidationResult::Invalid;

				// 输出具体是哪一个 Widget Entry 没有配置 SlotID。
				Context.AddError(FText::Format(LOCTEXT("EntryHasNoTag", "SlotID is not set at index {0} in Widgets"), FText::AsNumber(EntryIndex)));
			}

			// 继续检查下一个 Widget Entry。
			++EntryIndex;
		}
	}

	// 返回最终数据校验结果。
	return Result;
}

#endif

// 当前 GameFeatureAction 被应用到一个 World 时调用。
void UGameFeatureAction_AddWidgets::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
	// 从 WorldContext 获取当前实际 World。
	UWorld* World = WorldContext.World();

	// 获取当前 WorldContext 所属的 GameInstance。
	UGameInstance* GameInstance = WorldContext.OwningGameInstance;

	// 为当前 GameFeature State Change Context，查找或创建对应的运行时状态记录。
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	// 只有存在有效 GameInstance、World并且当前 World 是真正的游戏 World 时才注册 HUD 扩展监听。
	if ((GameInstance != nullptr) && (World != nullptr) && World->IsGameWorld())
	{
		// 从 GameInstance 获取 GameFrameworkComponentManager。后续通过它监听 AHodgeHUD 的 Receiver / Extension 生命周期事件。
		if (UGameFrameworkComponentManager* ComponentManager = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance))
		{
			// 指定当前 Action 感兴趣的 Actor 类型为 AHodgeHUD。使用 SoftClassPtr 形式传给 ComponentManager。
			TSoftClassPtr<AActor> HUDActorClass = AHodgeHUD::StaticClass();

			// 注册针对 AHodgeHUD 的 Extension Handler。
			// 当 AHodgeHUD 被注册为 Receiver、进入 Ready、被移除等事件发生时， ComponentManager 会回调 HandleActorExtension。
			// ChangeContext 被一并绑定到回调中，这样收到事件时可以找到属于当前 GameFeature Context 的运行时数据。
			TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle = ComponentManager->AddExtensionHandler(
				HUDActorClass,
				UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &ThisClass::HandleActorExtension, ChangeContext));

			// 保存 Request Handle，维持这次 Extension Handler 注册的生命周期，同时方便 GameFeature 停用时统一释放。
			ActiveData.ComponentRequests.Add(ExtensionRequestHandle);
		}
	}
}

// 清理某一个 GameFeature Context 对应的运行时数据。
void UGameFeatureAction_AddWidgets::ClearActorContents(FPerActorData& Data)
{
    Data.bAdded = false;
    auto Layouts = MoveTemp(Data.LayoutsAdded);
    auto Extensions = MoveTemp(Data.ExtensionHandles);
    for (auto& Handle : Extensions) { Handle.Unregister(); }
    if (auto* Root = Data.Root.Get())
    { for (const auto& AddedLayout : Layouts) { if (AddedLayout.IsValid()) { Root->Pop(AddedLayout.Get()); } } }
    Data.Root.Reset();
}

void UGameFeatureAction_AddWidgets::Reset(FPerContextData& ActiveData)
{
    auto Records = MoveTemp(ActiveData.ActorData);
    for (auto& Pair : Records)
    {
        auto& Data = Pair.Value;
        if (auto* UI = Data.Manager.Get()) { UI->OnRootLayoutChanged.Remove(Data.RootChangedDelegate); }
        ClearActorContents(Data);
    }
    ActiveData.ComponentRequests.Empty();
}

// 处理 GameFrameworkComponentManager 发来的 AHodgeHUD Extension 生命周期事件。
void UGameFeatureAction_AddWidgets::HandleActorExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext)
{
	// 找到或创建当前 GameFeature Context 对应的运行时状态。
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	// Extension 被移除或整个 Receiver 被从 GameFrameworkComponentManager 中移除时，清理当前 HUD Actor 对应的 UI。
	if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) || (EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved))
	{
		RemoveWidgets(Actor, ActiveData);
	}

	// Extension 刚刚加入或 AHodgeHUD 已经广播 NAME_GameActorReady 时，为这个 HUD Actor 添加当前 Action 配置的 UI。
	else if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (EventName == UGameFrameworkComponentManager::NAME_GameActorReady))
	{
		AddWidgets(Actor, ActiveData);
	}
}

// 为指定 AHodgeHUD 对应的本地玩家添加当前 GameFeature 配置的 UI。
void UGameFeatureAction_AddWidgets::AddWidgets(AActor* Actor, FPerContextData& ActiveData)
{
    auto* HUD = Cast<AHodgeHUD>(Actor);
    auto* PC = HUD ? HUD->GetOwningPlayerController() : nullptr;
    ULocalPlayer* Player = PC ? PC->GetLocalPlayer() : nullptr;
    auto* Instance = Player ? Player->GetGameInstance() : nullptr;
    auto* UI = Instance ? Instance->GetSubsystem<UHodgeUIManagerSubsystem>() : nullptr;
    if (!UI) { return; }
    auto& Data = ActiveData.ActorData.FindOrAdd(HUD);
    Data.Actor = HUD;
    Data.Player = Player;
    Data.Manager = UI;
    if (!Data.RootChangedDelegate.IsValid())
    {
        TWeakObjectPtr<AActor> WeakActor = Actor;
        FGameFeatureStateChangeContext Context;
        for (const auto& Pair : ContextData) { if (&Pair.Value == &ActiveData) { Context = Pair.Key; break; } }
        Data.RootChangedDelegate = UI->OnRootLayoutChanged.AddWeakLambda(this,
            [this, WeakActor, Context](ULocalPlayer* Changed, UHodgePrimaryGameLayout* Root)
        {
            auto* Active = ContextData.Find(Context);
            auto* Record = Active && WeakActor.IsValid() ? Active->ActorData.Find(WeakActor.Get()) : nullptr;
            if (!Record || Record->Player != Changed) { return; }
            ClearActorContents(*Record);
            if (Root) { AddWidgets(WeakActor.Get(), *Active); }
        });
    }
    auto* Root = UI->GetRootLayout(Player);
    if (!Root || Data.bAdded) { return; }
    Data.Root = Root;
    Data.bAdded = true;
    for (const auto& Entry : Layout)
    {
        if (auto* Class = Entry.LayoutClass.Get())
        { if (auto* Added = Root->Push(Entry.LayerID, Class)) { Data.LayoutsAdded.Add(Added); } }
        else { UE_LOG(LogTemp, Error, TEXT("[Hodge UI] Layout not loaded in Client bundle: %s"), *Entry.LayoutClass.ToString()); }
    }
    if (auto* Extensions = HUD->GetWorld()->GetSubsystem<UUIExtensionSubsystem>())
    {
        for (const auto& Entry : Widgets)
        {
            if (auto* Class = Entry.WidgetClass.Get())
            { Data.ExtensionHandles.Add(Extensions->RegisterExtensionAsWidgetForContext(Entry.SlotID, Player, Class, -1)); }
            else { UE_LOG(LogTemp, Error, TEXT("[Hodge UI] Extension not loaded in Client bundle: %s"), *Entry.WidgetClass.ToString()); }
        }
    }
}

// 移除之前为指定 AHodgeHUD 添加的所有 UI。
void UGameFeatureAction_AddWidgets::RemoveWidgets(AActor* Actor, FPerContextData& ActiveData)
{
    FPerActorData Data;
    if (!ActiveData.ActorData.RemoveAndCopyValue(Actor, Data)) { return; }
    if (auto* UI = Data.Manager.Get()) { UI->OnRootLayoutChanged.Remove(Data.RootChangedDelegate); }
    ClearActorContents(Data);
}

// 结束当前文件的本地化文本命名空间。
#undef LOCTEXT_NAMESPACE
