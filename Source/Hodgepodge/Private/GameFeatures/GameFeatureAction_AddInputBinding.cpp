// Copyright Epic Games, Inc. All Rights Reserved.

// 引入当前 GameFeature 输入绑定 Action。
#include "GameFeatures/GameFeatureAction_AddInputBinding.h"

// 引入 GameFrameworkComponentManager，用于监听 Pawn 的扩展生命周期。
#include "Components/GameFrameworkComponentManager.h"

// 引入 GameInstance。
#include "Engine/GameInstance.h"

// 引入 PlayerController。
#include "GameFramework/PlayerController.h"

// 引入 World。
#include "Engine/World.h"

// 引入 Pawn。
#include "GameFramework/Pawn.h"

// 引入 Enhanced Input LocalPlayer Subsystem。
#include "EnhancedInputSubsystems.h"

// 引入 LocalPlayer。
#include "Engine/LocalPlayer.h"

//#include "Character/HodgeHeroComponent.h"

// 引入 GameFeature WorldAction 基类。
#include "GameFeatures/GameFeatureAction_WorldActionBase.h"

// 引入项目自定义的输入配置。
#include "Input/HodgeInputConfig.h"

#if WITH_EDITOR
// 编辑器下引入数据验证功能。
#include "Misc/DataValidation.h"
#endif

// 引入当前 CPP 对应的内联生成代码。
#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddInputBinding)

// 本文件使用 GameFeatures 作为本地化文本命名空间。
#define LOCTEXT_NAMESPACE "GameFeatures"

//////////////////////////////////////////////////////////////////////
// UGameFeatureAction_AddInputBinding

// GameFeature 激活阶段。
void UGameFeatureAction_AddInputBinding::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	// 获取当前 GameFeature 激活上下文对应的运行时数据。
	FPerContextData& ActiveData = ContextData.FindOrAdd(Context);

	// 正常情况下激活前不应该残留之前的监听句柄和 Pawn 记录。
	if (!ensure(ActiveData.ExtensionRequestHandles.IsEmpty()) ||
		!ensure(ActiveData.PawnsAddedTo.IsEmpty()))
	{
		// 如果发现残留数据，则先清理之前的运行时状态。
		Reset(ActiveData);
	}

	// 执行父类的 GameFeature 激活逻辑。
	Super::OnGameFeatureActivating(Context);
}

// GameFeature 停用阶段。
void UGameFeatureAction_AddInputBinding::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	// 先执行父类的 GameFeature 停用逻辑。
	Super::OnGameFeatureDeactivating(Context);

	// 查找当前 GameFeature 激活上下文对应的运行时数据。
	FPerContextData* ActiveData = ContextData.Find(Context);

	// 确保对应的运行时数据存在。
	if (ensure(ActiveData))
	{
		// 停用 GameFeature 时撤销该 Feature 添加的输入绑定。
		Reset(*ActiveData);
	}
}

#if WITH_EDITOR
// 编辑器下验证当前 GameFeature 的输入配置是否有效。
EDataValidationResult UGameFeatureAction_AddInputBinding::IsDataValid(FDataValidationContext& Context) const
{
	// 先执行父类验证，再合并当前类自己的验证结果。
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context),
	                                                            EDataValidationResult::Valid);

	// 当前正在检查的 InputConfig 数组索引。
	int32 Index = 0;

	// 遍历当前 GameFeature 配置的所有输入配置。
	for (const TSoftObjectPtr<const UHodgeInputConfig>& Entry : InputConfigs)
	{
		// 检查当前输入配置软引用是否为空。
		if (Entry.IsNull())
		{
			// 空输入配置会导致整个数据配置无效。
			Result = EDataValidationResult::Invalid;

			// 在编辑器数据验证窗口中报告具体错误位置。
			Context.AddError(FText::Format(LOCTEXT("NullInputConfig", "Null InputConfig at index {0}."), Index));
		}

		// 移动到下一个输入配置索引。
		++Index;
	}

	// 返回最终的数据验证结果。
	return Result;
}
#endif

// 将当前 GameFeature Action 添加到指定 World。
void UGameFeatureAction_AddInputBinding::AddToWorld(const FWorldContext& WorldContext,
                                                    const FGameFeatureStateChangeContext& ChangeContext)
{
	// 获取当前 World。
	UWorld* World = WorldContext.World();

	// 获取当前 World 所属的 GameInstance。
	UGameInstance* GameInstance = WorldContext.OwningGameInstance;

	// 获取当前 GameFeature 激活上下文对应的运行时数据。
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	// 只在有效的 GameInstance、World，并且是真正的 GameWorld 中注册 Pawn 扩展监听。
	if ((GameInstance != nullptr) && (World != nullptr) && World->IsGameWorld())
	{
		// 获取 GameFrameworkComponentManager，用于监听 Pawn 生命周期扩展事件。
		if (UGameFrameworkComponentManager* ComponentManager = UGameInstance::GetSubsystem<
			UGameFrameworkComponentManager>(GameInstance))
		{
			// 创建 Pawn 扩展事件的回调委托。
			UGameFrameworkComponentManager::FExtensionHandlerDelegate AddAbilitiesDelegate =
				UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(
					this, &ThisClass::HandlePawnExtension, ChangeContext);

			// 注册对所有 Pawn 的扩展事件监听。
			TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle =
				ComponentManager->AddExtensionHandler(APawn::StaticClass(), AddAbilitiesDelegate);

			// 保存监听句柄，方便 GameFeature 停用时解除监听。
			ActiveData.ExtensionRequestHandles.Add(ExtensionRequestHandle);
		}
	}
}

// 清理当前 GameFeature 激活上下文产生的所有运行时数据。
void UGameFeatureAction_AddInputBinding::Reset(FPerContextData& ActiveData)
{
	// 清空所有 Pawn 扩展监听请求。
	ActiveData.ExtensionRequestHandles.Empty();

	// 逐个处理当前 GameFeature 曾经添加过输入绑定的 Pawn。
	while (!ActiveData.PawnsAddedTo.IsEmpty())
	{
		// 获取当前列表顶部的 Pawn。
		TWeakObjectPtr<APawn> PawnPtr = ActiveData.PawnsAddedTo.Top();

		// 如果 Pawn 仍然有效，则撤销它的输入配置。
		if (PawnPtr.IsValid())
		{
			// 移除当前 GameFeature 添加的输入配置。
			RemoveInputMapping(PawnPtr.Get(), ActiveData);
		}
		else
		{
			// 如果 Pawn 已经失效，则直接移除对应的记录。
			ActiveData.PawnsAddedTo.Pop();
		}
	}
}

// 处理 Pawn 的 GameFrameworkComponentManager 扩展事件。
void UGameFeatureAction_AddInputBinding::HandlePawnExtension(AActor* Actor, FName EventName,
                                                             FGameFeatureStateChangeContext ChangeContext)
{
	// GameFrameworkComponentManager 保证这里监听的是 Pawn，因此直接强制转换。
	APawn* AsPawn = CastChecked<APawn>(Actor);

	// 获取当前 GameFeature 激活上下文对应的运行时数据。
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	// Pawn 的扩展或接收者被移除时，需要撤销当前 Feature 添加的输入配置。
	if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) || (EventName ==
		UGameFrameworkComponentManager::NAME_ReceiverRemoved))
	{
		// 移除该 Pawn 上当前 GameFeature 添加的输入配置。
		RemoveInputMapping(AsPawn, ActiveData);
	}
	// Pawn 的扩展被添加，或者 Pawn 已经准备好绑定输入时，添加输入配置。
	// else if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (EventName == UHodgeHeroComponent::NAME_BindInputsNow))
	// {
	// 	// 为该 Pawn 添加当前 GameFeature 提供的输入配置。
	// 	AddInputMappingForPlayer(AsPawn, ActiveData);
	// }
}

// 为指定 Pawn 添加当前 GameFeature 的输入配置。
void UGameFeatureAction_AddInputBinding::AddInputMappingForPlayer(APawn* Pawn, FPerContextData& ActiveData)
{
	// 获取 Pawn 当前拥有的 PlayerController。
	APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());

	// 只有 Pawn 属于本地 PlayerController 时，才能找到对应的 LocalPlayer。
	if (ULocalPlayer* LocalPlayer = PlayerController ? PlayerController->GetLocalPlayer() : nullptr)
	{
		// 获取 LocalPlayer 对应的 Enhanced Input Subsystem。
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>())
		{
			// 获取 Pawn 上的 HeroComponent。
			// UHodgeHeroComponent* HeroComponent = Pawn->FindComponentByClass<UHodgeHeroComponent>();
			//
			// // 只有 HeroComponent 存在并且已经准备好绑定输入时，才真正添加 InputConfig。
			// if (HeroComponent && HeroComponent->IsReadyToBindInputs())
			// {
			// 	// 遍历当前 GameFeature 配置的所有输入配置。
			// 	for (const TSoftObjectPtr<const UHodgeInputConfig>& Entry : InputConfigs)
			// 	{
			// 		// 获取已经加载的输入配置资源。
			// 		if (const UHodgeInputConfig* BindSet = Entry.Get())
			// 		{
			// 			// 将该输入配置添加到 HeroComponent 的额外输入配置列表。
			// 			HeroComponent->AddAdditionalInputConfig(BindSet);
			// 		}
			// 	}
			// }

			// 记录该 Pawn 已经处理过输入绑定，方便后续停用时进行清理。
			ActiveData.PawnsAddedTo.AddUnique(Pawn);
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

// 移除指定 Pawn 上当前 GameFeature 添加的输入配置。
void UGameFeatureAction_AddInputBinding::RemoveInputMapping(APawn* Pawn, FPerContextData& ActiveData)
{
	// 获取 Pawn 当前拥有的 PlayerController。
	APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());

	// 只有 Pawn 属于本地 PlayerController 时，才能找到对应的 LocalPlayer。
	if (ULocalPlayer* LocalPlayer = PlayerController ? PlayerController->GetLocalPlayer() : nullptr)
	{
		// 获取 LocalPlayer 对应的 Enhanced Input Subsystem。
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>())
		{
			// 查找 Pawn 上的 HeroComponent。
			// if (UHodgeHeroComponent* HeroComponent = Pawn->FindComponentByClass<UHodgeHeroComponent>())
			// {
			// 	// 遍历当前 GameFeature 配置的所有输入配置。
			// 	for (const TSoftObjectPtr<const UHodgeInputConfig>& Entry : InputConfigs)
			// 	{
			// 		// 获取已经加载的输入配置资源。
			// 		if (const UHodgeInputConfig* InputConfig = Entry.Get())
			// 		{
			// 			// 从 HeroComponent 中移除该额外输入配置。
			// 			HeroComponent->RemoveAdditionalInputConfig(InputConfig);
			// 		}
			// 	}
			// }
		}
	}

	// 从运行时记录中移除该 Pawn。
	ActiveData.PawnsAddedTo.Remove(Pawn);
}

// 恢复本地化文本命名空间。
#undef LOCTEXT_NAMESPACE
