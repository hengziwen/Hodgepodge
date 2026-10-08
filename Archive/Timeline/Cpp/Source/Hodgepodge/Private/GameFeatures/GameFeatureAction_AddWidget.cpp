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
void UGameFeatureAction_AddWidgets::Reset(FPerContextData& ActiveData)
{
	// 清空 ComponentRequest Handle。
	// FComponentRequestHandle 的生命周期结束后，对应注册到 GameFrameworkComponentManager 的 Extension Handler也会随之失效 / 注销。
	ActiveData.ComponentRequests.Empty();

	// 遍历当前 Context 下所有曾经处理过的 Actor。
	for (TPair<FObjectKey, FPerActorData>& Pair : ActiveData.ActorData)
	{
		// 注销当前 Actor 对应的所有 UIExtension。
		for (FUIExtensionHandle& Handle : Pair.Value.ExtensionHandles)
		{
			// 通知 UUIExtensionSubsystem 移除对应 Extension，对应的 UUIExtensionPointWidget 会收到 Removed并移除之前创建的动态 Widget。
			Handle.Unregister();
		}
	}

	// 清空所有 Actor 对应的运行时 UI 数据。
	ActiveData.ActorData.Empty();
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
	// 当前 Extension Handler 只针对 AHodgeHUD 注册，因此这里要求传入 Actor 必须是 AHodgeHUD。
	AHodgeHUD* HUD = CastChecked<AHodgeHUD>(Actor);

	// HUD 必须已经拥有对应 PlayerController。
	if (!HUD->GetOwningPlayerController())
	{
		return;//如果当前 HUD 还没有 OwningPlayerController就无法确定 UI 应该属于哪个玩家，因此直接返回。
	}

	// 从 HUD 的 OwningPlayerController 中取得真正的 ULocalPlayer。只有本地玩家才需要创建客户端 HUD UI。
	if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(HUD->GetOwningPlayerController()->Player))
	{
		// 获取当前 HUD Actor 对应的运行时 UI 记录。后续所有新增 Layout 和 Extension Handle都应该记录在这个 ActorData 中，以便精确撤销。
		FPerActorData& ActorData = ActiveData.ActorData.FindOrAdd(HUD);

		// 遍历当前 Action 配置的所有 HUD Layout。
		for (const FHodgeHUDLayoutRequest& Entry : Layout)
		{
			// SoftClass 必须已经能够解析出实际 Widget Class。
			if (TSubclassOf<UCommonActivatableWidget> ConcreteWidgetClass = Entry.LayoutClass.Get())
			{
				// 设计意图：
				// 将 Layout Widget Push 到当前 LocalPlayer对应的 CommonUI Layer 中，并把实际创建的 Layout Widget 保存到 LayoutsAdded，方便之后 RemoveWidgets 时 Deactivate。
				//
				// 当前 CommonUIExtensions 调用被注释，因此目前 Layout[] 实际不会创建 HUD Layout。
				//ActorData.LayoutsAdded.Add(UCommonUIExtensions::PushContentToLayer_ForPlayer(LocalPlayer, Entry.LayerID, ConcreteWidgetClass));
			}
		}

		// 从当前 HUD 所属 World 获取 UIExtensionSubsystem。普通 HUD Element 不直接 AddToViewport，而是通过 UIExtensionSystem 注册到对应 Slot。
		UUIExtensionSubsystem* ExtensionSubsystem = HUD->GetWorld()->GetSubsystem<UUIExtensionSubsystem>();

		// 遍历当前 Action 配置的所有 HUD Element。
		for (const FHodgeHUDElementEntry& Entry : Widgets)
		{
			// 将 WidgetClass 注册为一条带 LocalPlayer Context 的 UIExtension。
			// SlotID： 决定这条 Widget Extension 应该匹配哪个 UUIExtensionPointWidget。
			// LocalPlayer： 保证这条 UI 只会被当前本地玩家对应的 ExtensionPoint 接收。
			// WidgetClass： 是真正需要动态创建的 UUserWidget 类型。
			// Priority = -1： 当前没有额外指定优先级。
			// 返回的 FUIExtensionHandle 会保存下来供 RemoveWidgets / Reset 时精确注销。
			ActorData.ExtensionHandles.Add(ExtensionSubsystem->RegisterExtensionAsWidgetForContext(Entry.SlotID, LocalPlayer, Entry.WidgetClass.Get(), -1));
		}
	}
}

// 移除之前为指定 AHodgeHUD 添加的所有 UI。
void UGameFeatureAction_AddWidgets::RemoveWidgets(AActor* Actor, FPerContextData& ActiveData)
{
	// 当前 Handler 只应该接收 AHodgeHUD。
	AHodgeHUD* HUD = CastChecked<AHodgeHUD>(Actor);

	// Only unregister if this is the same HUD actor that was registered, there can be multiple active at once on the client
	// 只清理当前这个 HUD Actor 自己对应的数据。客户端同一时间可能存在多个活动 HUD Actor，因此不能简单地把当前 Context 下的所有 UI 全部清空。
	FPerActorData* ActorData = ActiveData.ActorData.Find(HUD);

	// 当前 HUD 确实曾经由这个 Action 添加过 UI 时才进行清理。
	if (ActorData)
	{
		// 遍历当前 Action 为这个 HUD 添加过的所有 Layout。
		for (TWeakObjectPtr<UCommonActivatableWidget>& AddedLayout : ActorData->LayoutsAdded)
		{
			// Layout 实例仍然有效时，通过 CommonActivatableWidget 生命周期将其停用。
			if (AddedLayout.IsValid())
			{
				AddedLayout->DeactivateWidget();
			}
		}

		// 注销当前 HUD 对应的所有 UIExtension。
		for (FUIExtensionHandle& Handle : ActorData->ExtensionHandles)
		{
			// Extension 注销后，UUIExtensionSubsystem 会向匹配的 ExtensionPoint,广播 EUIExtensionAction::Removed,最终对应动态 Widget 会被移除。
			Handle.Unregister();
		}

		// 当前 HUD 对应的 UI 已经全部清理完成，从 ActorData Map 中移除它的运行时记录。
		ActiveData.ActorData.Remove(HUD);
	}
}

// 结束当前文件的本地化文本命名空间。
#undef LOCTEXT_NAMESPACE
