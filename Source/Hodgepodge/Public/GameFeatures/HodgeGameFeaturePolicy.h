// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFeatureStateChangeObserver.h"
#include "GameFeaturesProjectPolicies.h"

#include "HodgeGameFeaturePolicy.generated.h"

class FName;
class UGameFeatureData;
struct FPrimaryAssetId;

/**
 * Hodge 项目的 GameFeature 策略类。
 *
 * 继承自 UE 提供的 UDefaultGameFeaturesProjectPolicies，
 * 用于管理 GameFeature 插件从发现、加载到激活/卸载的整个生命周期。
 *
 * GameFeature 插件本身只提供通用的插件管理机制，
 * 具体项目需要加载哪些 Feature、哪些 Feature 允许启用、
 * 启动时需要预加载哪些资源等，可以通过这个 Policy 进行定制。
 */
UCLASS(MinimalAPI, Config = Game)
class UHodgeGameFeaturePolicy : public UDefaultGameFeaturesProjectPolicies
{
    GENERATED_BODY()

public:
    // 获取 Hodge 项目的 GameFeature Policy 单例。
    HODGEPODGE_API static UHodgeGameFeaturePolicy& Get();

    // 构造函数。
    UHodgeGameFeaturePolicy(const FObjectInitializer& ObjectInitializer);

    //~UGameFeaturesProjectPolicies interface

    // 初始化 GameFeatureManager，在游戏启动阶段调用。
    // 可以在这里注册 GameFeature 状态监听器等初始化逻辑。
    virtual void InitGameFeatureManager() override;

    // 关闭 GameFeatureManager，在程序退出或系统关闭时调用。
    virtual void ShutdownGameFeatureManager() override;

    // 获取某个 GameFeature 在加载时需要预加载的 Primary Asset。
    // 可以用来提前加载 Feature 运行所需要的资源。
    virtual TArray<FPrimaryAssetId> GetPreloadAssetListForGameFeature(
        const UGameFeatureData* GameFeatureToLoad,
        bool bIncludeLoadedAssets = false
    ) const override;

    // 判断指定 GameFeature 插件是否允许被当前项目加载。
    virtual bool IsPluginAllowed(const FString& PluginURL) const override;

    // 获取 GameFeature 预加载资源所使用的 Asset Bundle 状态。
    virtual const TArray<FName> GetPreloadBundleStateForGameFeature() const override;

    // 获取 GameFeature 的加载模式。
    // 可以控制客户端和服务器是否加载该 GameFeature 的数据。
    virtual void GetGameFeatureLoadingMode(
        bool& bLoadClientData,
        bool& bLoadServerData
    ) const override;

    //~End of UGameFeaturesProjectPolicies interface

private:
    // 保存 GameFeature 状态监听器。
    //
    // Observer 一般用于监听 GameFeature 的注册、加载、激活、卸载等状态变化，
    // 由 Policy 持有引用，保证这些 Observer 在 GameFeatureManager 生命周期内不会被 GC。
    UPROPERTY(Transient)
    TArray<TObjectPtr<UObject>> Observers;
};


// 监听 GameFeature 加载事件。
// 当前主要用于处理 Hotfix 相关逻辑。
UCLASS()
class UHodgeGameFeature_HotfixManager
    : public UObject
    , public IGameFeatureStateChangeObserver
{
    GENERATED_BODY()

public:

    // 当 GameFeature 开始加载时调用。
    virtual void OnGameFeatureLoading(
        const UGameFeatureData* GameFeatureData,
        const FString& PluginURL
    ) override;
};


// 监听 GameFeature 注册和注销事件。
// 主要用于在 GameFeature 加载/卸载时动态管理 GameplayCue 路径。
UCLASS()
class UHodgeGameFeature_AddGameplayCuePaths
    : public UObject
    , public IGameFeatureStateChangeObserver
{
    GENERATED_BODY()

public:

    // GameFeature 注册时调用，可以在这里添加该 Feature 提供的 GameplayCue 路径。
    virtual void OnGameFeatureRegistering(
        const UGameFeatureData* GameFeatureData,
        const FString& PluginName,
        const FString& PluginURL
    ) override;

    // GameFeature 注销时调用，可以在这里移除之前注册的 GameplayCue 路径。
    virtual void OnGameFeatureUnregistering(
        const UGameFeatureData* GameFeatureData,
        const FString& PluginName,
        const FString& PluginURL
    ) override;
};