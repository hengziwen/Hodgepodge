// Copyright Epic Games, Inc. All Rights Reserved.

// 防止头文件被重复包含。
#pragma once

// GameFeature 的 WorldAction 基类。
#include "GameFeatureAction_WorldActionBase.h"

// UE SoftObjectPtr 软引用类型。
#include "UObject/SoftObjectPtr.h"

// 当前类对应的 UE 反射代码生成文件。
#include "GameFeatureAction_AddInputContextMapping.generated.h"

// Actor 基类。
class AActor;

// Enhanced Input 输入映射上下文。
class UInputMappingContext;

// 玩家对象基类。
class UPlayer;

// 玩家控制器。
class APlayerController;

// GameFeature 组件请求句柄，用于撤销组件扩展请求。
struct FComponentRequestHandle;

// 一组输入映射上下文及其优先级配置。
USTRUCT()
struct FInputMappingContextAndPriority
{
	// 开启 UE 反射和序列化支持。
	GENERATED_BODY()

	// 输入映射上下文的软引用。
	UPROPERTY(EditAnywhere, Category="Input", meta=(AssetBundles="Client,Server"))
	TSoftObjectPtr<UInputMappingContext> InputMapping;

	// Higher priority input mappings will be prioritized over mappings with a lower priority.
	// 数值越大的输入映射优先级越高。
	UPROPERTY(EditAnywhere, Category="Input")
	int32 Priority = 0;

	/** If true, then this mapping context will be registered with the settings when this game feature action is registered. */
	// 是否在 GameFeature 注册时将该输入映射注册到玩家输入设置。
	UPROPERTY(EditAnywhere, Category="Input")
	bool bRegisterWithSettings = true;
};

/**
 * Adds InputMappingContext to local players' EnhancedInput system. 
 * Expects that local players are set up to use the EnhancedInput system.
 */
// 将配置的 InputMappingContext 添加到本地玩家的 Enhanced Input 系统。
// 前提是 LocalPlayer 已经配置为使用 Enhanced Input。
UCLASS(MinimalAPI, meta = (DisplayName = "Add Input Mapping"))
class UGameFeatureAction_AddInputContextMapping final : public UGameFeatureAction_WorldActionBase
{
	// 开启 UE 反射支持。
	GENERATED_BODY()

public:
	//~UGameFeatureAction interface
	// GameFeature 开始注册时执行。
	virtual void OnGameFeatureRegistering() override;

	// GameFeature 开始激活时执行。
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;

	// GameFeature 开始停用时执行。
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

	// GameFeature 注销时执行。
	virtual void OnGameFeatureUnregistering() override;
	//~End of UGameFeatureAction interface

	//~UObject interface
#if WITH_EDITOR
	// 编辑器中验证当前 GameFeature Action 的配置是否合法。
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~End of UObject interface

	// 当前 GameFeature Action 需要添加的所有输入映射上下文。
	UPROPERTY(EditAnywhere, Category="Input")
	TArray<FInputMappingContextAndPriority> InputMappings;

private:
	// 保存某一次 GameFeature 激活上下文对应的运行时数据。
	struct FPerContextData
	{
		// 保存向 Actor 扩展系统请求的组件句柄，用于停用时撤销请求。
		TArray<TSharedPtr<FComponentRequestHandle>> ExtensionRequestHandles;

		// 保存已经添加过输入映射的 PlayerController。
		TArray<TWeakObjectPtr<APlayerController>> ControllersAddedTo;
	};

	// 按 GameFeature 状态变化上下文保存对应的运行时数据。
	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

	/** Delegate for when the game instance is changed to register IMC's */
	// GameInstance 发生变化时，用于重新注册 InputMappingContext 的委托句柄。
	FDelegateHandle RegisterInputContextMappingsForGameInstanceHandle;

	/** Registers owned Input Mapping Contexts to the Input Registry Subsystem. Also binds onto the start of GameInstances and the adding/removal of Local Players. */
	// 将当前 Action 拥有的输入映射注册到 Input Registry Subsystem。
	// 同时监听 GameInstance 启动以及 LocalPlayer 添加/移除事件。
	void RegisterInputMappingContexts();

	/** Registers owned Input Mapping Contexts to the Input Registry Subsystem for a specified GameInstance. This also gets called by a GameInstance Start. */
	// 为指定 GameInstance 注册当前 Action 的输入映射。
	// GameInstance 启动时也会调用该函数。
	void RegisterInputContextMappingsForGameInstance(UGameInstance* GameInstance);

	/** Registers owned Input Mapping Contexts to the Input Registry Subsystem for a specified Local Player. This also gets called when a Local Player is added. */
	// 为指定 LocalPlayer 注册当前 Action 的输入映射。
	// LocalPlayer 被添加时也会调用该函数。
	void RegisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer);

	/** Unregisters owned Input Mapping Contexts from the Input Registry Subsystem. Also unbinds from the start of GameInstances and the adding/removal of Local Players. */
	// 从 Input Registry Subsystem 注销当前 Action 拥有的输入映射。
	// 同时解除 GameInstance 和 LocalPlayer 相关事件绑定。
	void UnregisterInputMappingContexts();

	/** Unregisters owned Input Mapping Contexts from the Input Registry Subsystem for a specified GameInstance. */
	// 从指定 GameInstance 对应的输入注册系统中注销输入映射。
	void UnregisterInputContextMappingsForGameInstance(UGameInstance* GameInstance);

	/** Unregisters owned Input Mapping Contexts from the Input Registry Subsystem for a specified Local Player. This also gets called when a Local Player is removed. */
	// 从指定 LocalPlayer 对应的输入注册系统中注销输入映射。
	// LocalPlayer 被移除时也会调用该函数。
	void UnregisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer);

	//~UGameFeatureAction_WorldActionBase interface
	// GameFeature 被添加到 World 时执行具体的输入映射逻辑。
	virtual void AddToWorld(const FWorldContext& WorldContext,
	                        const FGameFeatureStateChangeContext& ChangeContext) override;
	//~End of UGameFeatureAction_WorldActionBase interface

	// 清理指定 GameFeature 激活上下文产生的所有运行时数据。
	void Reset(FPerContextData& ActiveData);

	// 响应 Controller 对应 Actor 的扩展事件。
	void HandleControllerExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);

	// 为指定 Player 添加当前 GameFeature 的输入映射。
	void AddInputMappingForPlayer(UPlayer* Player, FPerContextData& ActiveData);

	// 从指定 PlayerController 移除当前 GameFeature 添加的输入映射。
	void RemoveInputMapping(APlayerController* PlayerController, FPerContextData& ActiveData);
};
