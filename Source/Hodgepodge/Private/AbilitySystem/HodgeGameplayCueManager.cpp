// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/HodgeGameplayCueManager.h"
#include "Engine/AssetManager.h"
#include "GameplayCueSet.h"
#include "AbilitySystemGlobals.h"
#include "GameplayTagsManager.h"
#include "UObject/UObjectThreadContext.h"
#include "Async/Async.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayCueManager)

//////////////////////////////////////////////////////////////////////

// GameplayCue 的加载策略。
enum class EHodgeEditorLoadMode
{
	// 启动时一次性加载所有 Cue，编辑器加载时间更长，但 PIE 启动更快且效果基本不会因为未加载而播放失败。
	LoadUpfront,

	// 非编辑器环境：GameplayCue Tag 注册时异步加载。
	// 编辑器环境：只有真正调用 Cue 时才异步加载。
	// 优点是 PIE 迭代速度快，但可能出现某些效果第一次调用时没有及时显示的问题。
	PreloadAsCuesAreReferenced_GameOnly,

	// GameplayCue Tag 注册时直接异步加载对应的 Cue。
	PreloadAsCuesAreReferenced
};

namespace HodgeGameplayCueManagerCvars
{
	// 控制台命令，用于输出当前由 HodgeGameplayCueManager 加载并且仍在内存中的 GameplayCue。
	static FAutoConsoleCommand CVarDumpGameplayCues(
		TEXT("Hodge.DumpGameplayCues"),
		TEXT("Shows all assets that were loaded via HodgeGameplayCueManager and are currently in memory."),
		FConsoleCommandWithArgsDelegate::CreateStatic(UHodgeGameplayCueManager::DumpGameplayCues));

	// 当前 GameplayCue 的加载模式，默认启动时全部加载。
	static EHodgeEditorLoadMode LoadMode = EHodgeEditorLoadMode::LoadUpfront;
}

// 是否即使在编辑器中也执行 GameplayCue 预加载。
const bool bPreloadEvenInEditor = true;

//////////////////////////////////////////////////////////////////////

// 用于将 GameplayTag 加载后的处理任务切换到 GameThread。
struct FGameplayCueTagThreadSynchronizeGraphTask : public FAsyncGraphTaskBase
{
	// 实际需要在 GameThread 上执行的任务。
	TFunction<void()> TheTask;

	// 保存需要执行的任务。
	FGameplayCueTagThreadSynchronizeGraphTask(TFunction<void()>&& Task) : TheTask(MoveTemp(Task))
	{
	}

	// 执行保存下来的任务。
	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent) { TheTask(); }

	// 指定该任务必须在 GameThread 执行。
	ENamedThreads::Type GetDesiredThread() { return ENamedThreads::GameThread; }
};

//////////////////////////////////////////////////////////////////////

// 构造 GameplayCue Manager。
UHodgeGameplayCueManager::UHodgeGameplayCueManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// 获取当前由 AbilitySystemGlobals 管理的 GameplayCue Manager。
UHodgeGameplayCueManager* UHodgeGameplayCueManager::Get()
{
	return Cast<UHodgeGameplayCueManager>(UAbilitySystemGlobals::Get().GetGameplayCueManager());
}

// GameplayCue Manager 创建完成后的初始化入口。
void UHodgeGameplayCueManager::OnCreated()
{
	// 先执行父类初始化逻辑。
	Super::OnCreated();

	// 根据当前加载模式注册或移除 GameplayCue 相关 Delegate。
	UpdateDelayLoadDelegateListeners();
}

// 加载那些无论如何都应该提前加载的 GameplayCue。
void UHodgeGameplayCueManager::LoadAlwaysLoadedCues()
{
	// 只有启用了 GameplayCue 延迟加载时，才需要额外处理 Always Loaded Cue。
	if (ShouldDelayLoadGameplayCues())
	{
		// 获取全局 GameplayTag Manager。
		UGameplayTagsManager& TagManager = UGameplayTagsManager::Get();

		//@TODO: 尝试从 Native GameplayTag 中过滤出 GameplayCue Tag，自动收集这些 Tag。
		TArray<FName> AdditionalAlwaysLoadedCueTags;

		// 遍历额外指定的 Always Loaded GameplayCue Tag。
		for (const FName& CueTagName : AdditionalAlwaysLoadedCueTags)
		{
			// 根据 Tag 名称查找 GameplayTag，并允许 Tag 不存在。
			FGameplayTag CueTag = TagManager.RequestGameplayTag(CueTagName, /*ErrorIfNotFound=*/ false);

			// Tag 有效时，将对应 Cue 加入预加载流程。
			if (CueTag.IsValid())
			{
				ProcessTagToPreload(CueTag, nullptr);
			}
			else
			{
				// 配置了不存在的 GameplayTag 时输出警告。
				UE_LOG(LogTemp, Warning,
				       TEXT("UHodgeGameplayCueManager::AdditionalAlwaysLoadedCueTags contains invalid tag %s"),
				       *CueTagName.ToString());
			}
		}
	}
}

// 控制运行时 GameplayCue Object Library 是否应该异步加载。
bool UHodgeGameplayCueManager::ShouldAsyncLoadRuntimeObjectLibraries() const
{
	// 根据当前 GameplayCue 加载模式决定行为。
	switch (HodgeGameplayCueManagerCvars::LoadMode)
	{
	case EHodgeEditorLoadMode::LoadUpfront:
		// 一次性加载全部 Runtime GameplayCue Object Library。
		return true;

	case EHodgeEditorLoadMode::PreloadAsCuesAreReferenced_GameOnly:
#if WITH_EDITOR
		// GameOnly 模式下编辑器不提前异步加载。
		if (GIsEditor)
		{
			return false;
		}
#endif
		break;

	case EHodgeEditorLoadMode::PreloadAsCuesAreReferenced:
		// 由延迟加载逻辑决定是否加载。
		break;
	}

	// 只有不需要延迟加载 GameplayCue 时，才异步加载整个运行时 Cue Library。
	return !ShouldDelayLoadGameplayCues();
}

// 是否同步加载缺失的 GameplayCue。
bool UHodgeGameplayCueManager::ShouldSyncLoadMissingGameplayCues() const
{
	// 禁止同步加载，避免 GameplayCue 缺失时直接阻塞 GameThread。
	return false;
}

// 是否异步加载缺失的 GameplayCue。
bool UHodgeGameplayCueManager::ShouldAsyncLoadMissingGameplayCues() const
{
	// 允许通过异步加载方式补充缺失的 GameplayCue。
	return true;
}

// 输出当前 GameplayCue 的加载情况。
void UHodgeGameplayCueManager::DumpGameplayCues(const TArray<FString>& Args)
{
	// 获取全局 GameplayCue Manager，并转换为 Hodge 自定义类型。
	UHodgeGameplayCueManager* GCM = Cast<
		UHodgeGameplayCueManager>(UAbilitySystemGlobals::Get().GetGameplayCueManager());

	// 如果当前 Manager 不是 Hodge 类型，则无法继续处理。
	if (!GCM)
	{
		UE_LOG(LogTemp, Error, TEXT("DumpGameplayCues failed. No UHodgeGameplayCueManager found."));
		return;
	}

	// 是否同时输出每个预加载 Cue 的引用者。
	const bool bIncludeRefs = Args.Contains(TEXT("Refs"));

	// 输出所有 Always Loaded GameplayCue。
	UE_LOG(LogTemp, Log, TEXT("=========== Dumping Always Loaded Gameplay Cue Notifies ==========="));
	for (UClass* CueClass : GCM->AlwaysLoadedCues)
	{
		// 输出 Cue Class 的完整对象路径。
		UE_LOG(LogTemp, Log, TEXT("  %s"), *GetPathNameSafe(CueClass));
	}

	// 输出所有因为内容引用而预加载的 GameplayCue。
	UE_LOG(LogTemp, Log, TEXT("=========== Dumping Preloaded Gameplay Cue Notifies ==========="));
	for (UClass* CueClass : GCM->PreloadedCues)
	{
		// 查找当前 Cue 被哪些对象引用。
		TSet<FObjectKey>* ReferencerSet = GCM->PreloadedCueReferencers.Find(CueClass);

		// 获取当前 Cue 的引用数量。
		int32 NumRefs = ReferencerSet ? ReferencerSet->Num() : 0;

		// 输出 Cue 路径以及引用数量。
		UE_LOG(LogTemp, Log, TEXT("  %s (%d refs)"), *GetPathNameSafe(CueClass), NumRefs);

		// 如果指定 Refs 参数，则进一步输出所有引用者。
		if (bIncludeRefs && ReferencerSet)
		{
			// 遍历当前 Cue 的所有引用者。
			for (const FObjectKey& Ref : *ReferencerSet)
			{
				// 将对象 Key 解析回 UObject。
				UObject* RefObject = Ref.ResolveObjectPtr();

				// 输出引用者对象的路径。
				UE_LOG(LogTemp, Log, TEXT("    ^- %s"), *GetPathNameSafe(RefObject));
			}
		}
	}

	// 输出那些没有被 AlwaysLoadedCues 或 PreloadedCues 管理、但运行时按需加载的 Cue。
	UE_LOG(LogTemp, Log, TEXT("=========== Dumping Gameplay Cue Notifies loaded on demand ==========="));

	// 统计运行时按需加载的 Cue 数量。
	int32 NumMissingCuesLoaded = 0;

	// Runtime GameplayCue Object Library 中存在 CueSet 时才进行统计。
	if (GCM->RuntimeGameplayCueObjectLibrary.CueSet)
	{
		// 遍历 CueSet 中记录的所有 GameplayCue。
		for (const FGameplayCueNotifyData& CueData : GCM->RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueData)
		{
			// 已加载的 Cue 如果不属于 AlwaysLoaded 或 Preloaded，则说明是运行时按需加载的。
			if (CueData.LoadedGameplayCueClass && !GCM->AlwaysLoadedCues.Contains(CueData.LoadedGameplayCueClass) && !
				GCM->PreloadedCues.Contains(CueData.LoadedGameplayCueClass))
			{
				// 增加按需加载 Cue 的统计数量。
				NumMissingCuesLoaded++;

				// 输出按需加载的 Cue 路径。
				UE_LOG(LogTemp, Log, TEXT("  %s"), *CueData.LoadedGameplayCueClass->GetPathName());
			}
		}
	}

	// 输出 GameplayCue 加载情况汇总。
	UE_LOG(LogTemp, Log, TEXT("=========== Gameplay Cue Notify summary ==========="));

	// 输出 Always Loaded Cue 数量。
	UE_LOG(LogTemp, Log, TEXT("  ... %d cues in always loaded list"), GCM->AlwaysLoadedCues.Num());

	// 输出预加载 Cue 数量。
	UE_LOG(LogTemp, Log, TEXT("  ... %d cues in preloaded list"), GCM->PreloadedCues.Num());

	// 输出运行时按需加载 Cue 数量。
	UE_LOG(LogTemp, Log, TEXT("  ... %d cues loaded on demand"), NumMissingCuesLoaded);

	// 输出当前所有已加载 GameplayCue 的总数量。
	UE_LOG(LogTemp, Log, TEXT("  ... %d cues in total"),
	       GCM->AlwaysLoadedCues.Num() + GCM->PreloadedCues.Num() + NumMissingCuesLoaded);
}

// 当 GameplayTag 加载完成后，暂存该 Tag 并安排后续处理。
void UHodgeGameplayCueManager::OnGameplayTagLoaded(const FGameplayTag& Tag)
{
	// 加锁保护待处理 GameplayTag 数组，避免多线程同时访问。
	FScopeLock ScopeLock(&LoadedGameplayTagsToProcessCS);

	// 只有队列原本为空时才需要创建新的异步处理任务。
	bool bStartTask = LoadedGameplayTagsToProcess.Num() == 0;

	// 获取当前 UObject 加载上下文。
	FUObjectSerializeContext* LoadContext = FUObjectThreadContext::Get().GetSerializeContext();

	// 获取触发当前加载的对象，后续用于记录 GameplayCue 的引用者。
	UObject* OwningObject = LoadContext ? LoadContext->SerializedObject : nullptr;

	// 将加载完成的 GameplayTag 和其拥有者加入待处理队列。
	LoadedGameplayTagsToProcess.Emplace(Tag, OwningObject);

	// 队列从空变为非空时，只创建一个处理任务。
	if (bStartTask)
	{
		// 创建一个异步图任务，并将实际处理逻辑切换到 GameThread。
		TGraphTask<FGameplayCueTagThreadSynchronizeGraphTask>::CreateTask().ConstructAndDispatchWhenReady([]()
		{
			// 游戏正在运行时才继续处理。
			if (GIsRunning)
			{
				// 获取当前 Hodge GameplayCue Manager。
				if (UHodgeGameplayCueManager* StrongThis = Get())
				{
					// GC 期间不能安全调用部分 StaticFindObject / UObject 静态函数，因此延迟到 GC 完成后再处理。
					if (IsGarbageCollecting())
					{
						// 标记 GC 完成后重新处理这些 GameplayTag。
						StrongThis->bProcessLoadedTagsAfterGC = true;
					}
					else
					{
						// 当前不在 GC 中，直接处理已经加载的 GameplayTag。
						StrongThis->ProcessLoadedTags();
					}
				}
			}
		});
	}
}

// Garbage Collection 完成后的回调。
void UHodgeGameplayCueManager::HandlePostGarbageCollect()
{
	// 如果之前因为 GC 而推迟了 Tag 处理，则现在继续处理。
	if (bProcessLoadedTagsAfterGC)
	{
		ProcessLoadedTags();
	}

	// 清除 GC 后处理标记。
	bProcessLoadedTagsAfterGC = false;
}

// 处理之前暂存的 GameplayTag。
void UHodgeGameplayCueManager::ProcessLoadedTags()
{
	// 将待处理 Tag 临时移动到局部数组，减少锁的持有时间。
	TArray<FLoadedGameplayTagToProcessData> TaskLoadedGameplayTagsToProcess;

	{
		// 只在复制和清空共享数组期间持锁。
		// Lock LoadedGameplayTagsToProcess just long enough to make a copy and clear
		FScopeLock TaskScopeLock(&LoadedGameplayTagsToProcessCS);

		// 复制当前所有待处理 GameplayTag。
		TaskLoadedGameplayTagsToProcess = LoadedGameplayTagsToProcess;

		// 清空共享队列，让后续加载事件可以继续进入队列。
		LoadedGameplayTagsToProcess.Empty();
	}

	// Shutdown 期间可能会执行到这里，因此运行结束时不能继续处理 UObject。
	// This might return during shutdown, and we don't want to proceed if that is the case
	if (GIsRunning)
	{
		// Runtime GameplayCue Object Library 必须已经存在有效的 CueSet。
		if (RuntimeGameplayCueObjectLibrary.CueSet)
		{
			// 遍历本次需要处理的 GameplayTag。
			for (const FLoadedGameplayTagToProcessData& LoadedTagData : TaskLoadedGameplayTagsToProcess)
			{
				// 只有 CueSet 中实际存在对应 GameplayCue Tag 时才继续。
				if (RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueDataMap.Contains(LoadedTagData.Tag))
				{
					// Owner 没有失效时，使用该 Owner 作为 GameplayCue 的引用者。
					if (!LoadedTagData.WeakOwner.IsStale())
					{
						// 根据 Tag 查找并异步预加载对应 GameplayCue。
						ProcessTagToPreload(LoadedTagData.Tag, LoadedTagData.WeakOwner.Get());
					}
				}
			}
		}
		else
		{
			// CueSet 尚未准备好时跳过本次 Tag 处理，并输出警告。
			UE_LOG(LogTemp, Warning,
			       TEXT(
				       "UHodgeGameplayCueManager::OnGameplayTagLoaded processed loaded tag(s) but RuntimeGameplayCueObjectLibrary.CueSet was null. Skipping processing."
			       ));
		}
	}
}

// 根据 GameplayTag 查找并预加载对应 GameplayCue。
void UHodgeGameplayCueManager::ProcessTagToPreload(const FGameplayTag& Tag, UObject* OwningObject)
{
	// 根据当前加载模式决定是否执行预加载。
	switch (HodgeGameplayCueManagerCvars::LoadMode)
	{
	case EHodgeEditorLoadMode::LoadUpfront:
		// 已经全部提前加载，不需要额外预加载。
		return;

	case EHodgeEditorLoadMode::PreloadAsCuesAreReferenced_GameOnly:
#if WITH_EDITOR
		// GameOnly 模式下编辑器环境不执行引用预加载。
		if (GIsEditor)
		{
			return;
		}
#endif
		break;

	case EHodgeEditorLoadMode::PreloadAsCuesAreReferenced:
		// 正常执行引用预加载。
		break;
	}

	// 当前流程要求 Runtime GameplayCue Object Library 必须存在 CueSet。
	check(RuntimeGameplayCueObjectLibrary.CueSet);

	// 根据 GameplayTag 查找对应 GameplayCueData 在数组中的索引。
	int32* DataIdx = RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueDataMap.Find(Tag);

	// 确保索引存在，并且索引没有越界。
	if (DataIdx && RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueData.IsValidIndex(*DataIdx))
	{
		// 获取当前 GameplayCue 的资源描述信息。
		const FGameplayCueNotifyData& CueData = RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueData[*DataIdx];

		// 先尝试直接从内存中查找 GameplayCue Class，避免重复异步加载。
		UClass* LoadedGameplayCueClass = FindObject<UClass>(nullptr, *CueData.GameplayCueNotifyObj.ToString());

		// 如果 Cue Class 已经存在于内存，则直接注册。
		if (LoadedGameplayCueClass)
		{
			// 已经加载完成，不需要再次发起异步加载。
			RegisterPreloadedCue(LoadedGameplayCueClass, OwningObject);
		}
		else
		{
			// Owner 为空表示这是明确要求 Always Loaded 的 Cue。
			bool bAlwaysLoadedCue = OwningObject == nullptr;

			// 保存 Owner 的弱引用，避免异步加载期间强行延长 Owner 生命周期。
			TWeakObjectPtr<UObject> WeakOwner = OwningObject;

			// 异步加载 GameplayCue Class，加载完成后回调 OnPreloadCueComplete。
			StreamableManager.RequestAsyncLoad(CueData.GameplayCueNotifyObj,
			                                   FStreamableDelegate::CreateUObject(
				                                   this, &ThisClass::OnPreloadCueComplete, CueData.GameplayCueNotifyObj,
				                                   WeakOwner, bAlwaysLoadedCue),
			                                   FStreamableManager::DefaultAsyncLoadPriority, false, false,
			                                   TEXT("GameplayCueManager"));
		}
	}
}

// GameplayCue 异步加载完成后的回调。
void UHodgeGameplayCueManager::OnPreloadCueComplete(FSoftObjectPath Path, TWeakObjectPtr<UObject> OwningObject,
                                                    bool bAlwaysLoadedCue)
{
	// Always Loaded Cue 不依赖 Owner 是否还存在；普通预加载 Cue 则要求 Owner 仍然有效。
	if (bAlwaysLoadedCue || OwningObject.IsValid())
	{
		// 将软路径解析为已经加载完成的 GameplayCue Class。
		if (UClass* LoadedGameplayCueClass = Cast<UClass>(Path.ResolveObject()))
		{
			// 将加载完成的 Cue 注册到对应的预加载集合。
			RegisterPreloadedCue(LoadedGameplayCueClass, OwningObject.Get());
		}
	}
}

// 将加载完成的 GameplayCue 注册到 AlwaysLoaded 或 Preloaded 集合。
void UHodgeGameplayCueManager::RegisterPreloadedCue(UClass* LoadedGameplayCueClass, UObject* OwningObject)
{
	// 调用该函数时必须保证 GameplayCue Class 有效。
	check(LoadedGameplayCueClass);

	// Owner 为空代表当前 Cue 是 Always Loaded Cue。
	const bool bAlwaysLoadedCue = OwningObject == nullptr;

	// Always Loaded Cue 优先级最高。
	if (bAlwaysLoadedCue)
	{
		// 加入始终加载集合。
		AlwaysLoadedCues.Add(LoadedGameplayCueClass);

		// 如果之前属于普通预加载 Cue，则将其移除。
		PreloadedCues.Remove(LoadedGameplayCueClass);

		// Always Loaded Cue 不再需要记录普通引用者。
		PreloadedCueReferencers.Remove(LoadedGameplayCueClass);
	}
	// 普通预加载 Cue 必须有有效 Owner，并且不能把 Cue 自身或 CDO 当作引用者。
	else if ((OwningObject != LoadedGameplayCueClass) && (OwningObject != LoadedGameplayCueClass->GetDefaultObject()) &&
		!AlwaysLoadedCues.Contains(LoadedGameplayCueClass))
	{
		// 将 Cue 加入普通预加载集合。
		PreloadedCues.Add(LoadedGameplayCueClass);

		// 获取或创建该 Cue 对应的引用者集合。
		TSet<FObjectKey>& ReferencerSet = PreloadedCueReferencers.FindOrAdd(LoadedGameplayCueClass);

		// 记录当前对象对该 GameplayCue 的引用。
		ReferencerSet.Add(OwningObject);
	}
}

// 地图加载完成后的 GameplayCue 清理和引用检查。
void UHodgeGameplayCueManager::HandlePostLoadMap(UWorld* NewWorld)
{
	// Runtime CueSet 存在时，先从 CueSet 中移除这些已经由 Hodge 自己追踪的 LoadedClass。
	if (RuntimeGameplayCueObjectLibrary.CueSet)
	{
		// 清理 Always Loaded Cue 的原生 LoadedClass 状态。
		for (UClass* CueClass : AlwaysLoadedCues)
		{
			RuntimeGameplayCueObjectLibrary.CueSet->RemoveLoadedClass(CueClass);
		}

		// 清理普通预加载 Cue 的原生 LoadedClass 状态。
		for (UClass* CueClass : PreloadedCues)
		{
			RuntimeGameplayCueObjectLibrary.CueSet->RemoveLoadedClass(CueClass);
		}
	}

	// 遍历所有普通预加载 Cue，检查其引用者是否已经失效。
	for (auto CueIt = PreloadedCues.CreateIterator(); CueIt; ++CueIt)
	{
		// 获取当前 Cue 的引用者集合。
		TSet<FObjectKey>& ReferencerSet = PreloadedCueReferencers.FindChecked(*CueIt);

		// 清理已经无法解析回 UObject 的失效引用。
		for (auto RefIt = ReferencerSet.CreateIterator(); RefIt; ++RefIt)
		{
			// 引用对象已经不存在时，从集合中删除。
			if (!RefIt->ResolveObjectPtr())
			{
				RefIt.RemoveCurrent();
			}
		}

		// 如果当前 Cue 已经没有任何引用者，则整个预加载记录也可以移除。
		if (ReferencerSet.Num() == 0)
		{
			// 删除该 Cue 的引用者记录。
			PreloadedCueReferencers.Remove(*CueIt);

			// 从普通预加载集合中删除该 Cue。
			CueIt.RemoveCurrent();
		}
	}
}

// 根据当前 GameplayCue 加载模式注册或移除相关 Delegate。
void UHodgeGameplayCueManager::UpdateDelayLoadDelegateListeners()
{
	// 先移除旧的 GameplayTag 加载监听，避免重复注册。
	UGameplayTagsManager::Get().OnGameplayTagLoadedDelegate.RemoveAll(this);

	// 移除旧的 GC 完成监听。
	FCoreUObjectDelegates::GetPostGarbageCollect().RemoveAll(this);

	// 移除旧的地图加载监听。
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

	// 根据当前加载模式决定是否需要注册延迟加载相关 Delegate。
	switch (HodgeGameplayCueManagerCvars::LoadMode)
	{
	case EHodgeEditorLoadMode::LoadUpfront:
		// 全部提前加载，不需要监听后续 Tag 加载事件。
		return;

	case EHodgeEditorLoadMode::PreloadAsCuesAreReferenced_GameOnly:
#if WITH_EDITOR
		// GameOnly 模式在编辑器中不执行引用预加载。
		if (GIsEditor)
		{
			return;
		}
#endif
		break;

	case EHodgeEditorLoadMode::PreloadAsCuesAreReferenced:
		// 正常启用 GameplayCue 引用预加载。
		break;
	}

	// 监听 GameplayTag 加载事件，以便 Tag 加载后预加载对应 Cue。
	UGameplayTagsManager::Get().OnGameplayTagLoadedDelegate.AddUObject(this, &ThisClass::OnGameplayTagLoaded);

	// 监听 GC 完成事件，用于处理因为 GC 而暂缓的 GameplayTag。
	FCoreUObjectDelegates::GetPostGarbageCollect().AddUObject(this, &ThisClass::HandlePostGarbageCollect);

	// 监听地图加载完成事件，用于清理旧地图相关的 Cue 引用。
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::HandlePostLoadMap);
}

// 判断当前运行环境是否应该启用 GameplayCue 延迟加载。
bool UHodgeGameplayCueManager::ShouldDelayLoadGameplayCues() const
{
	// 当前配置允许客户端延迟加载 GameplayCue。
	const bool bClientDelayLoadGameplayCues = true;

	// Dedicated Server 不需要加载客户端 GameplayCue，因此服务器始终不执行这套延迟加载逻辑。
	return !IsRunningDedicatedServer() && bClientDelayLoadGameplayCues;
}

// GameplayCue 引用 Primary Asset 的类型。
const FPrimaryAssetType UFortAssetManager_GameplayCueRefsType = TEXT("GameplayCueRefs");

// GameplayCue 引用 Primary Asset 的名称。
const FName UFortAssetManager_GameplayCueRefsName = TEXT("GameplayCueReferences");

// GameplayCue 引用资源使用的 Client Load State。
const FName UFortAssetManager_LoadStateClient = FName(TEXT("Client"));

// 根据当前 Runtime GameplayCueSet 刷新 GameplayCue Primary Asset。
void UHodgeGameplayCueManager::RefreshGameplayCuePrimaryAsset()
{
	// 保存 Runtime GameplayCueSet 中所有 Cue 的软对象路径。
	TArray<FSoftObjectPath> CuePaths;

	// 获取当前运行时使用的 GameplayCueSet。
	UGameplayCueSet* RuntimeGameplayCueSet = GetRuntimeCueSet();

	// CueSet 存在时收集其中所有 GameplayCue 的软对象路径。
	if (RuntimeGameplayCueSet)
	{
		RuntimeGameplayCueSet->GetSoftObjectPaths(CuePaths);
	}

	// 创建 GameplayCue 的 Asset Bundle 数据。
	FAssetBundleData BundleData;

	// 将所有 Cue 路径加入 Client Bundle，并截断重复或无效的路径。
	BundleData.AddBundleAssetsTruncated(UFortAssetManager_LoadStateClient, CuePaths);

	// 构造 GameplayCue 引用 Primary Asset 的唯一 ID。
	FPrimaryAssetId PrimaryAssetId = FPrimaryAssetId(UFortAssetManager_GameplayCueRefsType,
	                                                 UFortAssetManager_GameplayCueRefsName);

	// 动态向 AssetManager 注册这个 Primary Asset，使 AssetManager 能够按照 Client Bundle 管理这些 Cue。
	UAssetManager::Get().AddDynamicAsset(PrimaryAssetId, FSoftObjectPath(), BundleData);
}
