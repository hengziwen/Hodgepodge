// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFeatures/HodgeGameFeaturePolicy.h"

// Hodge 自定义 GameplayCueManager，当前暂时没有使用。
// #include "AbilitySystem/HodgeGameplayCueManager.h"

#include "GameFeatureData.h"
#include "GameplayCueManager.h"
#include "GameplayCueSet.h"


// 构造函数，暂时没有额外初始化逻辑。
UHodgeGameFeaturePolicy::UHodgeGameFeaturePolicy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


// 获取当前 GameFeaturesSubsystem 使用的 HodgeGameFeaturePolicy。
// GameFeaturesSubsystem 内部会保存项目配置的 GameFeature Policy，
// 这里通过 GetPolicy<T>() 将其转换为 UHodgeGameFeaturePolicy。
UHodgeGameFeaturePolicy& UHodgeGameFeaturePolicy::Get()
{
	return UGameFeaturesSubsystem::Get().GetPolicy<UHodgeGameFeaturePolicy>();
}


// 初始化 GameFeatureManager。
// 这里主要做两件事情：
// 1. 创建 Hodge 自己的 GameFeature 状态监听器 Observer。
// 2. 将 Observer 注册到 GameFeaturesSubsystem，使其能够接收 GameFeature 生命周期事件。
void UHodgeGameFeaturePolicy::InitGameFeatureManager()
{
	// 创建并保存 GameFeature 状态监听器。
	// 使用 Observers 持有引用，避免这些 UObject 被 GC。
	Observers.Add(NewObject<UHodgeGameFeature_HotfixManager>());
	//Observers.Add(NewObject<UHodgeGameFeature_AddGameplayCuePaths>());

	UGameFeaturesSubsystem& Subsystem = UGameFeaturesSubsystem::Get();

	// 将所有 Observer 注册到 GameFeaturesSubsystem。
	// 之后 GameFeature 发生 Register、Load、Activate、Unload、Unregister 等状态变化时，
	// 对应的 Observer 就可以收到通知。
	for (UObject* Observer : Observers)
	{
		Subsystem.AddObserver(Observer);
	}

	// 最后调用父类的初始化逻辑。
	Super::InitGameFeatureManager();
}


// 关闭 GameFeatureManager。
//
// 与 InitGameFeatureManager 对应，负责注销之前注册的 Observer。
void UHodgeGameFeaturePolicy::ShutdownGameFeatureManager()
{
	// 先执行父类的关闭逻辑。
	Super::ShutdownGameFeatureManager();

	UGameFeaturesSubsystem& Subsystem = UGameFeaturesSubsystem::Get();

	// 从 GameFeaturesSubsystem 中移除所有 Observer。
	for (UObject* Observer : Observers)
	{
		Subsystem.RemoveObserver(Observer);
	}

	// 清空 Observer 数组，释放对这些 UObject 的引用。
	Observers.Empty();
}


// 获取某个 GameFeature 需要预加载的 PrimaryAsset。
// 当前没有额外逻辑，直接使用 UE 默认 Policy 的实现。
TArray<FPrimaryAssetId> UHodgeGameFeaturePolicy::GetPreloadAssetListForGameFeature(
	const UGameFeatureData* GameFeatureToLoad,
	bool bIncludeLoadedAssets) const
{
	return Super::GetPreloadAssetListForGameFeature(
		GameFeatureToLoad,
		bIncludeLoadedAssets
	);
}


// 获取 GameFeature 预加载时需要使用的 Asset Bundle 状态。
// 当前同样直接使用父类默认实现。
const TArray<FName> UHodgeGameFeaturePolicy::GetPreloadBundleStateForGameFeature() const
{
	return Super::GetPreloadBundleStateForGameFeature();
}


// 设置 GameFeature 的加载模式。
//
// 客户端不加载 Server-only 数据；
// Dedicated Server 不加载 Client-only 数据。
void UHodgeGameFeaturePolicy::GetGameFeatureLoadingMode(
	bool& bLoadClientData,
	bool& bLoadServerData) const
{
	// Dedicated Server 不需要加载客户端数据。
	bLoadClientData = !IsRunningDedicatedServer();

	// Client-only 程序不需要加载服务器数据。
	bLoadServerData = !IsRunningClientOnly();
}


// 判断指定 GameFeature 插件是否允许加载。
// 当前没有额外限制，直接使用父类默认逻辑。
bool UHodgeGameFeaturePolicy::IsPluginAllowed(const FString& PluginURL) const
{
	return Super::IsPluginAllowed(PluginURL);
}


//////////////////////////////////////////////////////////////////////
//
// Hotfix 相关逻辑目前暂时没有启用。
//
// #include "Hotfix/HodgeHotfixManager.h"
//
void UHodgeGameFeature_HotfixManager::OnGameFeatureLoading(
	const UGameFeatureData* GameFeatureData,
	const FString& PluginURL)
{
	// if (UHodgeHotfixManager* HotfixManager =
	//     Cast<UHodgeHotfixManager>(UOnlineHotfixManager::Get(nullptr)))
	// {
	//     HotfixManager->RequestPatchAssetsFromIniFiles();
	// }
}


//////////////////////////////////////////////////////////////////////

#include "AbilitySystemGlobals.h"
#include "GameFeatures/GameFeatureAction_AddGameplayCuePath.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameFeaturePolicy)

class FName;
struct FPrimaryAssetId;


// GameFeature 注册时调用。
//
// 这里的目的主要是处理 GameFeature 中配置的
// UGameFeatureAction_AddGameplayCuePath，
// 将该 Feature 提供的 GameplayCue 路径注册到 GameplayCueManager。
void UHodgeGameFeature_AddGameplayCuePaths::OnGameFeatureRegistering(
	const UGameFeatureData* GameFeatureData,
	const FString& PluginName,
	const FString& PluginURL)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(
		UHodgeGameFeature_AddGameplayCuePaths::OnGameFeatureRegistering
	);

	// GameFeature 插件的根目录，例如：
	// /GameFeatures/MyCombatFeature
	//
	// PluginName 会被转换成插件对应的虚幻包路径。
	const FString PluginRootPath = TEXT("/") + PluginName;

	// 获取这个 GameFeatureData 配置的所有 GameFeatureAction。
	for (const UGameFeatureAction* Action : GameFeatureData->GetActions())
	{
		// 找出其中专门用于添加 GameplayCue 路径的 Action。
		//if (const UGameFeatureAction_AddGameplayCuePath* AddGameplayCueGFA = =
		//        Cast<UGameFeatureAction_AddGameplayCuePath>(Action))
		//{
		// 获取该 Action 配置的 GameplayCue 目录。
		//const TArray<FDirectoryPath>& DirsToAdd =
		//    AddGameplayCueGFA->GetDirectoryPathsToAdd();

		// 这里原本 Lyra 会获取自己的 GameplayCueManager，
		// 然后把这些目录添加进去并重新扫描 GameplayCue。
		//
		// 当前 Hodge 项目暂时注释掉了这部分逻辑。
		//
		// if (UHodgeGameplayCueManager* GCM = UHodgeGameplayCueManager::Get())
		// {
		//     UGameplayCueSet* RuntimeGameplayCueSet = GCM->GetRuntimeCueSet();
		//
		//     const int32 PreInitializeNumCues =
		//         RuntimeGameplayCueSet
		//             ? RuntimeGameplayCueSet->GameplayCueData.Num()
		//             : 0;
		//
		//     for (const FDirectoryPath& Directory : DirsToAdd)
		//     {
		//         FString MutablePath = Directory.Path;
		//
		//         // 将 Feature 内部配置的路径转换成实际插件包路径。
		//         UGameFeaturesSubsystem::FixPluginPackagePath(
		//             MutablePath,
		//             PluginRootPath,
		//             false
		//         );
		//
		//         // 将 GameplayCue 路径添加到 GameplayCueManager。
		//         GCM->AddGameplayCueNotifyPath(
		//             MutablePath,
		//             false
		//         );
		//     }
		//
		//     // 如果新增了 GameplayCue 路径，则重新建立 Runtime Cue Library。
		//     if (!DirsToAdd.IsEmpty())
		//     {
		//         GCM->InitializeRuntimeObjectLibrary();
		//     }
		//
		//     const int32 PostInitializeNumCues =
		//         RuntimeGameplayCueSet
		//             ? RuntimeGameplayCueSet->GameplayCueData.Num()
		//             : 0;
		//
		//     // 如果 GameplayCue 数量发生变化，则刷新 GameplayCue PrimaryAsset。
		//     if (PreInitializeNumCues != PostInitializeNumCues)
		//     {
		//         GCM->RefreshGameplayCuePrimaryAsset();
		//     }
		// }
		//}
	}
}


// GameFeature 注销时调用。
//
// 与 OnGameFeatureRegistering 相反，这里负责将该 Feature
// 注册的 GameplayCue 路径从 GameplayCueManager 中移除。
void UHodgeGameFeature_AddGameplayCuePaths::OnGameFeatureUnregistering(
	const UGameFeatureData* GameFeatureData,
	const FString& PluginName,
	const FString& PluginURL)
{
	// 获取 GameFeature 插件根路径。
	const FString PluginRootPath = TEXT("/") + PluginName;

	// 遍历这个 GameFeature 配置的所有 Action。
	for (const UGameFeatureAction* Action : GameFeatureData->GetActions())
	{
		// // 找出 GameplayCue 路径 Action。
		// if (const UGameFeatureAction_AddGameplayCuePath* AddGameplayCueGFA =
		//         Cast<UGameFeatureAction_AddGameplayCuePath>(Action))
		// {
		//     // 获取这个 Action 配置的 GameplayCue 目录。
		//     const TArray<FDirectoryPath>& DirsToAdd =
		//         AddGameplayCueGFA->GetDirectoryPathsToAdd();
		//
		//     // 获取 GAS 全局 GameplayCueManager。
		//     if (UGameplayCueManager* GCM =
		//             UAbilitySystemGlobals::Get().GetGameplayCueManager())
		//     {
		//         int32 NumRemoved = 0;
		//
		//         // 将这个 Feature 添加过的 GameplayCue 路径逐个移除。
		//         for (const FDirectoryPath& Directory : DirsToAdd)
		//         {
		//             FString MutablePath = Directory.Path;
		//
		//             // 将 Feature 内部路径转换成实际插件包路径。
		//             UGameFeaturesSubsystem::FixPluginPackagePath(
		//                 MutablePath,
		//                 PluginRootPath,
		//                 false
		//             );
		//
		//             // 从 GameplayCueManager 中移除该路径。
		//             // false 表示暂时不立即重新扫描，最后统一处理。
		//             NumRemoved += GCM->RemoveGameplayCueNotifyPath(
		//                 MutablePath,
		//                 false
		//             );
		//         }
		//
		//         // 理论上删除数量应该和配置的目录数量一致。
		//         ensure(NumRemoved == DirsToAdd.Num());
		//
		//         // 如果确实移除了 GameplayCue 路径，
		//         // 则重新建立 Runtime GameplayCue Object Library。
		//         if (NumRemoved > 0)
		//         {
		//             GCM->InitializeRuntimeObjectLibrary();
		//         }
		//     }
		// }
	}
}
