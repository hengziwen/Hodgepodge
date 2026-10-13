/** @file HodgeGameplayCueManager.h
 * @brief 声明GameplayCue管理器相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayCueManager.h"

#include "HodgeGameplayCueManager.generated.h"

class FString;
class UClass;
class UObject;
class UWorld;
struct FObjectKey;

/**
 * UHodgeGameplayCueManager
 *
 * Hodge 项目自己的 GameplayCue 全局管理器。
 * 负责 GameplayCue 的注册、加载、预加载以及运行时资源管理。
 */
UCLASS()
class UHodgeGameplayCueManager : public UGameplayCueManager
{
	GENERATED_BODY()

public:
	// 构造 GameplayCue Manager，并使用默认的对象初始化参数。
	/** 构造GameplayCue管理器并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	UHodgeGameplayCueManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 获取当前全局的 Hodge GameplayCue Manager。
	/** 获取当前记录。
	 * @return 的当前值或引用。
	 */
	static UHodgeGameplayCueManager* Get();

	//~UGameplayCueManager interface

	// GameplayCue Manager 创建完成后进行项目自定义初始化。
	/** 处理Created。 */
	virtual void OnCreated() override;

	// 是否应该异步加载运行时 GameplayCue Object Library。
	/** 判断是否应异步加载Runtime对象Libraries。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	virtual bool ShouldAsyncLoadRuntimeObjectLibraries() const override;

	// 是否应该同步加载缺失的 GameplayCue。
	/** 判断是否应同步加载Missing玩法Cues。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	virtual bool ShouldSyncLoadMissingGameplayCues() const override;

	// 是否应该异步加载缺失的 GameplayCue。
	/** 判断是否应异步加载Missing玩法Cues。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	virtual bool ShouldAsyncLoadMissingGameplayCues() const override;

	//~End of UGameplayCueManager interface

	// 输出当前 GameplayCue 的调试和统计信息。
	/** 输出当前Cue加载与引用记录。
	 * @param Args Args，由调用方传入。
	 */
	static void DumpGameplayCues(const TArray<FString>& Args);

	// 延迟加载 GameplayCue 时，优先加载那些无论如何都必须常驻的 Cue。
	/** 按当前策略加载需常驻的GameplayCue资源。 */
	void LoadAlwaysLoadedCues();

	// 更新 GameplayCue Primary Asset 的 Asset Bundle 信息。
	/** 刷新玩法GameplayCue主要资产。 */
	void RefreshGameplayCuePrimaryAsset();

private:
	// 当新的 GameplayTag 加载完成后处理对应的 GameplayCue。
	/** 处理玩法标签已加载。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 */
	void OnGameplayTagLoaded(const FGameplayTag& Tag);

	// Garbage Collection 完成后处理之前暂存的 GameplayTag。
	/** 处理PostGarbageCollect。 */
	void HandlePostGarbageCollect();

	// 处理已经加载完成、等待进一步处理的 GameplayTag。
	/** 处理已加载标签。 */
	void ProcessLoadedTags();

	// 将需要预加载的 GameplayCue Tag 加入处理流程。
	/** 处理标签目标Preload。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 * @param OwningObject Owning对象，由调用方传入。
	 */
	void ProcessTagToPreload(const FGameplayTag& Tag, UObject* OwningObject);

	// GameplayCue 异步预加载完成后的回调。
	/** 处理PreloadGameplayCueComplete。
	 * @param Path 路径，由调用方传入。
	 * @param OwningObject Owning对象，由调用方传入。
	 * @param bAlwaysLoadedCue 是否Always已加载GameplayCue的控制标记。
	 */
	void OnPreloadCueComplete(FSoftObjectPath Path, TWeakObjectPtr<UObject> OwningObject, bool bAlwaysLoadedCue);

	// 将已经加载完成的 GameplayCue Class 注册到预加载集合中。
	/** 注册PreloadedGameplayCue。
	 * @param LoadedGameplayCueClass 已加载玩法GameplayCue类型，由调用方传入。
	 * @param OwningObject Owning对象，由调用方传入。
	 */
	void RegisterPreloadedCue(UClass* LoadedGameplayCueClass, UObject* OwningObject);

	// 地图加载完成后的 GameplayCue 处理回调。
	/** 处理Post加载映射。
	 * @param NewWorld 新世界，由调用方传入。
	 */
	void HandlePostLoadMap(UWorld* NewWorld);

	// 根据当前延迟加载状态更新相关 Delegate 的监听关系。
	/** 更新Delay加载委托监听者。 */
	void UpdateDelayLoadDelegateListeners();

	// 判断当前是否应该启用 GameplayCue 的延迟加载机制。
	/** 判断是否应Delay加载玩法Cues。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool ShouldDelayLoadGameplayCues() const;

private:
	// 描述一个已经加载完成、等待 GameplayCue Manager 进一步处理的 GameplayTag。
	/** 构造已加载玩法标签目标处理数据并初始化其默认状态。
	 * @return 本次计算、查询或创建得到的structF已加载玩法标签目标处理数据玩法标签F玩法标签标签标签垃圾回收T弱引用对象PtrU对象弱引用所属对象F已加载玩法标签目标处理数据玩法标签F已加载玩法标签目标处理数据constF玩法标签输入标签constT弱引用对象PtrU对象输入弱引用所属对象标签输入标签弱引用所属对象输入弱引用所属对象。
	 */
	struct FLoadedGameplayTagToProcessData
	{
		// 需要处理的 GameplayTag。
		FGameplayTag Tag;

		// 产生或拥有该 Tag 的对象，使用弱引用避免阻止对象被 GC。
		TWeakObjectPtr<UObject> WeakOwner;

		// 默认构造函数。
		/** 构造已加载玩法标签目标处理数据并初始化其默认状态。 */
		FLoadedGameplayTagToProcessData()
		{
		}

		// 使用 GameplayTag 和拥有者初始化待处理数据。
		/** 构造已加载玩法标签目标处理数据并初始化其默认状态。
		 * @param InTag 标签，用于对应标签查询或匹配。
		 * @param InWeakOwner 弱引用所属对象，由调用方传入。
		 */
		FLoadedGameplayTagToProcessData(const FGameplayTag& InTag,
		                                const TWeakObjectPtr<UObject>& InWeakOwner) : Tag(InTag), WeakOwner(InWeakOwner)
		{
		}
	};

private:
	// 因内容资源引用而在客户端预加载的 GameplayCue。
	UPROPERTY(transient)
	TSet<TObjectPtr<UClass>> PreloadedCues;

	// 记录每个预加载 Cue 被哪些对象引用，用于追踪 Cue 与引用者之间的关系。
	TMap<FObjectKey, TSet<FObjectKey>> PreloadedCueReferencers;

	// 在客户端预加载并且要求始终保持加载状态的 GameplayCue。
	// 这些 Cue 可能来自代码引用，也可能被显式标记为 Always Loaded。
	UPROPERTY(transient)
	TSet<TObjectPtr<UClass>> AlwaysLoadedCues;

	// 暂存已经加载完成但尚未处理的 GameplayTag。
	TArray<FLoadedGameplayTagToProcessData> LoadedGameplayTagsToProcess;

	// 保护 LoadedGameplayTagsToProcess，避免多线程访问时发生数据竞争。
	FCriticalSection LoadedGameplayTagsToProcessCS;

	// 标记是否需要在下一次 GC 完成后继续处理已经加载的 GameplayTag。
	bool bProcessLoadedTagsAfterGC = false;
};
