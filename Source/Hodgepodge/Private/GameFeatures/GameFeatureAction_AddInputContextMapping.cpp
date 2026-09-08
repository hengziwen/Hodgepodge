// Copyright Epic Games, Inc. All Rights Reserved.

// 引入 GameFeature 添加输入映射上下文的具体实现。
#include "GameFeatures/GameFeatureAction_AddInputContextMapping.h"
// 引入 GameFrameworkComponentManager，用于监听 PlayerController 的扩展事件。
#include "Components/GameFrameworkComponentManager.h"
// 引入 GameInstance。
#include "Engine/GameInstance.h"
// 引入 PlayerController。
#include "GameFramework/PlayerController.h"
// 引入 LocalPlayer。
#include "Engine/LocalPlayer.h"
// 引入 World。
#include "Engine/World.h"
// 引入 Enhanced Input 的 LocalPlayer Subsystem。
#include "EnhancedInputSubsystems.h"
// 引入 GameFeature WorldAction 基类。
#include "GameFeatures/GameFeatureAction_WorldActionBase.h"
// 引入 Input Mapping Context。
#include "InputMappingContext.h"
//#include "Character/HodgeHeroComponent.h"
// 引入 Enhanced Input 用户设置。
#include "UserSettings/EnhancedInputUserSettings.h"
// 引入项目自己的 AssetManager，用于加载软引用资源。
#include "Data/HodgeAssetManager.h"
#if WITH_EDITOR
// 编辑器下引入 Data Validation 数据验证功能。
#include "Misc/DataValidation.h"
#endif
// 引入当前 CPP 对应的内联生成代码。
#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddInputContextMapping)
// 本文件使用 GameFeatures 作为本地化文本命名空间。
#define LOCTEXT_NAMESPACE "GameFeatures"

//////////////////////////////////////////////////////////////////////
// UGameFeatureAction_AddInputContextMapping

// GameFeature 注册阶段。
void UGameFeatureAction_AddInputContextMapping::OnGameFeatureRegistering()
{
	// 先执行父类的注册逻辑。
	Super::OnGameFeatureRegistering();

	// 注册当前 GameFeature 所拥有的输入映射上下文。
	RegisterInputMappingContexts();
}

// GameFeature 激活阶段。
void UGameFeatureAction_AddInputContextMapping::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	// 获取当前这次 GameFeature 激活对应的运行时数据。
	FPerContextData& ActiveData = ContextData.FindOrAdd(Context);

	// 正常情况下激活前不应该残留上一次的运行时数据。
	if (!ensure(ActiveData.ExtensionRequestHandles.IsEmpty()) ||
		!ensure(ActiveData.ControllersAddedTo.IsEmpty()))
	{
		// 如果发现残留数据，则先进行清理。
		Reset(ActiveData);
	}

	// 执行父类的 GameFeature 激活逻辑。
	Super::OnGameFeatureActivating(Context);
}

// GameFeature 停用阶段。
void UGameFeatureAction_AddInputContextMapping::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	// 先执行父类的停用逻辑。
	Super::OnGameFeatureDeactivating(Context);

	// 找到当前这次激活对应的运行时数据。
	FPerContextData* ActiveData = ContextData.Find(Context);

	// 确保对应的运行时数据存在。
	if (ensure(ActiveData))
	{
		// 停用 GameFeature 时撤销该 Feature 添加的所有输入映射。
		Reset(*ActiveData);
	}
}

// GameFeature 注销阶段。
void UGameFeatureAction_AddInputContextMapping::OnGameFeatureUnregistering()
{
	// 先执行父类的注销逻辑。
	Super::OnGameFeatureUnregistering();

	// 注销当前 GameFeature 注册的输入映射上下文。
	UnregisterInputMappingContexts();
}

// 注册当前 GameFeature 所拥有的输入映射上下文。
void UGameFeatureAction_AddInputContextMapping::RegisterInputMappingContexts()
{
	// 监听新的 GameInstance 启动，以便 GameFeature 在之后启动的世界中也能注册输入映射。
	RegisterInputContextMappingsForGameInstanceHandle = FWorldDelegates::OnStartGameInstance.AddUObject(
		this, &UGameFeatureAction_AddInputContextMapping::RegisterInputContextMappingsForGameInstance);

	// 获取当前引擎中的所有 WorldContext。
	const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();

	// 遍历当前已经存在的所有 GameInstance。
	for (TIndirectArray<FWorldContext>::TConstIterator WorldContextIterator = WorldContexts.CreateConstIterator();
	     WorldContextIterator; ++WorldContextIterator)
	{
		// 为已经存在的 GameInstance 注册输入映射。
		RegisterInputContextMappingsForGameInstance(WorldContextIterator->OwningGameInstance);
	}
}

// 为指定 GameInstance 注册输入映射相关监听。
void UGameFeatureAction_AddInputContextMapping::RegisterInputContextMappingsForGameInstance(UGameInstance* GameInstance)
{
	// 确保 GameInstance 有效，并且当前对象还没有绑定该 GameInstance 的 LocalPlayer 事件。
	if (GameInstance != nullptr && !GameInstance->OnLocalPlayerAddedEvent.IsBoundToObject(this))
	{
		// 监听 LocalPlayer 添加事件。
		GameInstance->OnLocalPlayerAddedEvent.AddUObject(
			this, &UGameFeatureAction_AddInputContextMapping::RegisterInputMappingContextsForLocalPlayer);

		// 监听 LocalPlayer 移除事件。
		GameInstance->OnLocalPlayerRemovedEvent.AddUObject(
			this, &UGameFeatureAction_AddInputContextMapping::UnregisterInputMappingContextsForLocalPlayer);

		// 遍历当前 GameInstance 中已经存在的 LocalPlayer。
		for (TArray<ULocalPlayer*>::TConstIterator LocalPlayerIterator = GameInstance->GetLocalPlayerIterator();
		     LocalPlayerIterator; ++LocalPlayerIterator)
		{
			// 为已经存在的 LocalPlayer 注册输入映射上下文。
			RegisterInputMappingContextsForLocalPlayer(*LocalPlayerIterator);
		}
	}
}

// 为指定 LocalPlayer 注册输入映射上下文。
void UGameFeatureAction_AddInputContextMapping::RegisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer)
{
	// 确保 LocalPlayer 有效。
	if (ensure(LocalPlayer))
	{
		// 获取项目自定义 AssetManager，用于解析输入映射的软引用。
		UHodgeAssetManager& AssetManager = UHodgeAssetManager::Get();

		// 获取该 LocalPlayer 对应的 Enhanced Input Subsystem。
		if (UEnhancedInputLocalPlayerSubsystem* EISubsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
		{
			// 获取 Enhanced Input 用户设置对象。
			if (UEnhancedInputUserSettings* Settings = EISubsystem->GetUserSettings())
			{
				// 遍历当前 GameFeature 配置的所有输入映射。
				for (const FInputMappingContextAndPriority& Entry : InputMappings)
				{
					// Skip entries that don't want to be registered
					// 如果该输入映射不需要注册到用户设置，则跳过。
					if (!Entry.bRegisterWithSettings)
					{
						continue;
					}

					// Register this IMC with the settings!
					// 将当前 Input Mapping Context 注册到 Enhanced Input 用户设置。
					if (UInputMappingContext* IMC = AssetManager.GetAsset(Entry.InputMapping))
					{
						Settings->RegisterInputMappingContext(IMC);
					}
				}
			}
		}
	}
}

// 注销当前 GameFeature 注册的所有输入映射上下文。
void UGameFeatureAction_AddInputContextMapping::UnregisterInputMappingContexts()
{
	// 移除 GameInstance 启动事件监听。
	FWorldDelegates::OnStartGameInstance.Remove(RegisterInputContextMappingsForGameInstanceHandle);

	// 清空事件句柄，表示当前已经解除绑定。
	RegisterInputContextMappingsForGameInstanceHandle.Reset();

	// 获取当前引擎中的所有 WorldContext。
	const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();

	// 遍历当前所有 GameInstance。
	for (TIndirectArray<FWorldContext>::TConstIterator WorldContextIterator = WorldContexts.CreateConstIterator();
	     WorldContextIterator; ++WorldContextIterator)
	{
		// 注销指定 GameInstance 下的输入映射。
		UnregisterInputContextMappingsForGameInstance(WorldContextIterator->OwningGameInstance);
	}
}

// 为指定 GameInstance 注销输入映射相关监听。
void UGameFeatureAction_AddInputContextMapping::UnregisterInputContextMappingsForGameInstance(
	UGameInstance* GameInstance)
{
	// 确保 GameInstance 有效。
	if (GameInstance != nullptr)
	{
		// 移除 LocalPlayer 添加事件监听。
		GameInstance->OnLocalPlayerAddedEvent.RemoveAll(this);

		// 移除 LocalPlayer 移除事件监听。
		GameInstance->OnLocalPlayerRemovedEvent.RemoveAll(this);

		// 遍历当前 GameInstance 中已经存在的 LocalPlayer。
		for (TArray<ULocalPlayer*>::TConstIterator LocalPlayerIterator = GameInstance->GetLocalPlayerIterator();
		     LocalPlayerIterator; ++LocalPlayerIterator)
		{
			// 为每个 LocalPlayer 注销输入映射。
			UnregisterInputMappingContextsForLocalPlayer(*LocalPlayerIterator);
		}
	}
}

// 为指定 LocalPlayer 注销输入映射上下文。
void UGameFeatureAction_AddInputContextMapping::UnregisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer)
{
	// 确保 LocalPlayer 有效。
	if (ensure(LocalPlayer))
	{
		// 获取该 LocalPlayer 对应的 Enhanced Input Subsystem。
		if (UEnhancedInputLocalPlayerSubsystem* EISubsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
		{
			// 获取 Enhanced Input 用户设置对象。
			if (UEnhancedInputUserSettings* Settings = EISubsystem->GetUserSettings())
			{
				// 遍历当前 GameFeature 配置的所有输入映射。
				for (const FInputMappingContextAndPriority& Entry : InputMappings)
				{
					// Skip entries that don't want to be registered
					// 如果该输入映射没有注册到用户设置，则无需注销。
					if (!Entry.bRegisterWithSettings)
					{
						continue;
					}

					// Register this IMC with the settings!
					// 获取已经加载的 Input Mapping Context。
					if (UInputMappingContext* IMC = Entry.InputMapping.Get())
					{
						// 从 Enhanced Input 用户设置中注销该 Input Mapping Context。
						Settings->UnregisterInputMappingContext(IMC);
					}
				}
			}
		}
	}
}


#if WITH_EDITOR
// 编辑器下验证当前 GameFeature 输入配置是否有效。
EDataValidationResult UGameFeatureAction_AddInputContextMapping::IsDataValid(FDataValidationContext& Context) const
{
	// 先执行父类的数据验证，再合并当前类自己的验证结果。
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context),
	                                                            EDataValidationResult::Valid);

	// 当前正在检查的 InputMapping 数组索引。
	int32 Index = 0;

	// 遍历所有输入映射配置。
	for (const FInputMappingContextAndPriority& Entry : InputMappings)
	{
		// 检查输入映射软引用是否为空。
		if (Entry.InputMapping.IsNull())
		{
			// 空输入映射会导致配置无效。
			Result = EDataValidationResult::Invalid;

			// 在编辑器数据验证窗口中报告具体错误位置。
			Context.AddError(FText::Format(LOCTEXT("NullInputMapping", "Null InputMapping at index {0}."), Index));
		}

		// 移动到下一个配置索引。
		++Index;
	}

	// 返回最终的数据验证结果。
	return Result;
}
#endif

// 将该 GameFeature Action 添加到指定 World。
void UGameFeatureAction_AddInputContextMapping::AddToWorld(const FWorldContext& WorldContext,
                                                           const FGameFeatureStateChangeContext& ChangeContext)
{
	// 获取当前 World。
	UWorld* World = WorldContext.World();

	// 获取当前 World 所属的 GameInstance。
	UGameInstance* GameInstance = WorldContext.OwningGameInstance;

	// 获取当前 GameFeature 激活上下文对应的运行时数据。
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	// 只在有效的 GameInstance、World，并且是真正的 GameWorld 时处理。
	if ((GameInstance != nullptr) && (World != nullptr) && World->IsGameWorld())
	{
		// 获取 GameFrameworkComponentManager，用于监听 Actor 扩展事件。
		if (UGameFrameworkComponentManager* ComponentManager = UGameInstance::GetSubsystem<
			UGameFrameworkComponentManager>(GameInstance))
		{
			// 创建 PlayerController 扩展事件回调。
			UGameFrameworkComponentManager::FExtensionHandlerDelegate AddAbilitiesDelegate =
				UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(
					this, &ThisClass::HandleControllerExtension, ChangeContext);

			// 注册 PlayerController 扩展监听。
			TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle =
				ComponentManager->AddExtensionHandler(APlayerController::StaticClass(), AddAbilitiesDelegate);

			// 保存监听句柄，方便 GameFeature 停用时撤销。
			ActiveData.ExtensionRequestHandles.Add(ExtensionRequestHandle);
		}
	}
}

// 清理当前 GameFeature 激活上下文产生的所有运行时数据。
void UGameFeatureAction_AddInputContextMapping::Reset(FPerContextData& ActiveData)
{
	// 清空所有 Component Extension 请求句柄，从而解除相关监听。
	ActiveData.ExtensionRequestHandles.Empty();

	// 逐个处理曾经添加过输入映射的 PlayerController。
	while (!ActiveData.ControllersAddedTo.IsEmpty())
	{
		// 获取当前列表顶部的 Controller。
		TWeakObjectPtr<APlayerController> ControllerPtr = ActiveData.ControllersAddedTo.Top();

		// 如果 Controller 仍然有效，则移除其输入映射。
		if (ControllerPtr.IsValid())
		{
			RemoveInputMapping(ControllerPtr.Get(), ActiveData);
		}
		else
		{
			// 如果 Controller 已经失效，则直接移除对应记录。
			ActiveData.ControllersAddedTo.Pop();
		}
	}
}

// 处理 PlayerController 的 GameFrameworkComponentManager 扩展事件。
void UGameFeatureAction_AddInputContextMapping::HandleControllerExtension(
	AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext)
{
	// GameFrameworkComponentManager 保证这里的 Actor 是 PlayerController，因此直接强制转换。
	APlayerController* AsController = CastChecked<APlayerController>(Actor);

	// 获取当前 GameFeature 激活上下文对应的运行时数据。
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	// TODO Why does this code mix and match controllers and local players? ControllersAddedTo is never modified
	// TODO：这里混用了 Controller 和 LocalPlayer，需要确认实际生命周期关系；当前 ControllersAddedTo 似乎没有被添加记录。

	// 当 Controller 对应的扩展或接收者被移除时，需要撤销输入映射。
	if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) || (EventName ==
		UGameFrameworkComponentManager::NAME_ReceiverRemoved))
	{
		// 移除该 PlayerController 对应的输入映射。
		RemoveInputMapping(AsController, ActiveData);
	}
	// else if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (EventName == UHodgeHeroComponent::NAME_BindInputsNow))
	// {
	//     AddInputMappingForPlayer(AsController->GetLocalPlayer(), ActiveData);
	// }
}

// 为指定 Player 添加当前 GameFeature 的输入映射。
void UGameFeatureAction_AddInputContextMapping::AddInputMappingForPlayer(UPlayer* Player, FPerContextData& ActiveData)
{
	// 确保传入的 Player 实际上是 LocalPlayer。
	if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player))
	{
		// 获取 LocalPlayer 对应的 Enhanced Input Subsystem。
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>())
		{
			// 遍历当前 GameFeature 配置的所有输入映射。
			for (const FInputMappingContextAndPriority& Entry : InputMappings)
			{
				// 获取已经加载的 Input Mapping Context。
				if (const UInputMappingContext* IMC = Entry.InputMapping.Get())
				{
					// 将输入映射上下文添加到 Enhanced Input 系统，并使用配置的优先级。
					InputSystem->AddMappingContext(IMC, Entry.Priority);
				}
			}
		}
		else
		{
			// 如果找不到 Enhanced Input Subsystem，则输出错误日志。
			UE_LOG(LogGameFeatures, Error,
			       TEXT(
				       "Failed to find `UEnhancedInputLocalPlayerSubsystem` for local player. Input mappings will not be added. Make sure you're set to use the EnhancedInput system via config file."
			       ));
		}
	}
}

// 移除指定 PlayerController 对应的输入映射。
void UGameFeatureAction_AddInputContextMapping::RemoveInputMapping(APlayerController* PlayerController,
                                                                   FPerContextData& ActiveData)
{
	// 获取 PlayerController 对应的 LocalPlayer。
	if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
	{
		// 获取 LocalPlayer 对应的 Enhanced Input Subsystem。
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>())
		{
			// 遍历当前 GameFeature 配置的所有输入映射。
			for (const FInputMappingContextAndPriority& Entry : InputMappings)
			{
				// 获取已经加载的 Input Mapping Context。
				if (const UInputMappingContext* IMC = Entry.InputMapping.Get())
				{
					// 从 Enhanced Input 系统中移除该输入映射上下文。
					InputSystem->RemoveMappingContext(IMC);
				}
			}
		}
	}

	// 从运行时记录中移除该 PlayerController。
	ActiveData.ControllersAddedTo.Remove(PlayerController);
}

// 恢复本地化文本命名空间。
#undef LOCTEXT_NAMESPACE
