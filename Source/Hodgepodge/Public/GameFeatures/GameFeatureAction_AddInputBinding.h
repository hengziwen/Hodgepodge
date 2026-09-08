// Copyright Epic Games, Inc. All Rights Reserved.

// 防止头文件被重复包含。
#pragma once

// 引入 GameFeature 世界行为基类。
#include "GameFeatureAction_WorldActionBase.h"

// 引入软对象引用。
#include "UObject/SoftObjectPtr.h"

// 引入当前类的反射生成代码。
#include "GameFeatureAction_AddInputBinding.generated.h"

// 前向声明 Actor。
class AActor;

// 前向声明 Input Mapping Context。
class UInputMappingContext;

// 前向声明 Player。
class UPlayer;

// 前向声明 PlayerController。
class APlayerController;

// 前向声明 GameFrameworkComponentManager 的组件请求句柄。
struct FComponentRequestHandle;

// 前向声明项目自定义的输入配置。
class UHodgeInputConfig;

/**
 * Adds InputMappingContext to local players' EnhancedInput system. 
 * Expects that local players are set up to use the EnhancedInput system.
 */
// 将输入配置中的 InputAction 与 InputTag 绑定关系添加到 Pawn 的输入系统。
// 这里依赖 LocalPlayer 已经配置并使用 Enhanced Input。
UCLASS(MinimalAPI, meta = (DisplayName = "Add Input Binds"))
class UGameFeatureAction_AddInputBinding final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	//~ Begin UGameFeatureAction interface
	// GameFeature 激活时执行输入绑定逻辑。
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;

	// GameFeature 停用时撤销当前 Feature 添加的输入绑定。
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	//~ End UGameFeatureAction interface

	//~ Begin UObject interface
#if WITH_EDITOR
	// 编辑器下验证输入配置是否有效。
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~ End UObject interface

	// 当前 GameFeature 使用的输入配置资源列表。
	// 一个 InputConfig 中通常包含 Native Input 和 Ability Input 的 InputAction-InputTag 映射。
	UPROPERTY(EditAnywhere, Category="Input", meta=(AssetBundles="Client,Server"))
	TArray<TSoftObjectPtr<const UHodgeInputConfig>> InputConfigs;

private:
	// 一次 GameFeature 激活上下文对应的运行时数据。
	struct FPerContextData
	{
		// 记录向 GameFrameworkComponentManager 注册的 Pawn 扩展监听句柄。
		TArray<TSharedPtr<FComponentRequestHandle>> ExtensionRequestHandles;

		// 记录已经添加过输入绑定的 Pawn，方便停用 GameFeature 时逐个撤销。
		TArray<TWeakObjectPtr<APawn>> PawnsAddedTo;
	};

	// 按 GameFeature 激活上下文保存对应的运行时数据。
	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

	//~ Begin UGameFeatureAction_WorldActionBase interface
	// 将当前 GameFeature Action 添加到指定 World。
	virtual void AddToWorld(const FWorldContext& WorldContext,
	                        const FGameFeatureStateChangeContext& ChangeContext) override;
	//~ End UGameFeatureAction_WorldActionBase interface

	// 清理一次 GameFeature 激活过程中创建的所有输入绑定和事件监听。
	void Reset(FPerContextData& ActiveData);

	// 处理 Pawn 的 GameFrameworkComponentManager 扩展事件。
	void HandlePawnExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);

	// 将输入配置中的输入绑定添加到指定 Pawn。
	void AddInputMappingForPlayer(APawn* Pawn, FPerContextData& ActiveData);

	// 移除指定 Pawn 上由当前 GameFeature 添加的输入绑定。
	void RemoveInputMapping(APawn* Pawn, FPerContextData& ActiveData);
};
