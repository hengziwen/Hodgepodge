/** @file HodgeEnemyAITestSupport.h
 * @brief 声明怪物初始化测试世界辅助相关的类型、接口与现有处理流程。
 */
#pragma once
#include "Component/HodgeEnemyInitComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

/** 保存敌人Init测试Access的数据和接口契约。 */
struct FHodgeEnemyInitTestAccess
{
	/** 让测试初始化场景跳过Experience外部依赖。
	 * @param Init 当前角色的初始化协调组件。
	 */
	static void WithoutExperience(UHodgeEnemyInitComponent* Init) { Init->bRequireExperience = false; }
	/** 使初始化测试等待真实Experience依赖。
	 * @param Init 当前角色的初始化协调组件。
	 */
	static void AwaitExperience(UHodgeEnemyInitComponent* Init) { Init->bRequireExperience = true; Init->bExperienceReady = false; }
	/** 创建世界。
	 * @param Instance Instance，由调用方传入。
	 * @return 本次计算、查询或创建得到的世界。
	 */
	static UWorld* CreateWorld(UGameInstance*& Instance)
	{
		Instance = NewObject<UGameInstance>(GEngine); Instance->InitializeStandalone();
		auto* Manager = Instance->GetSubsystem<UGameFrameworkComponentManager>();
		Manager->RegisterInitState(HodgeGameplayTags::InitState_Spawned, false, FGameplayTag());
		Manager->RegisterInitState(HodgeGameplayTags::InitState_DataAvailable, false, HodgeGameplayTags::InitState_Spawned);
		Manager->RegisterInitState(HodgeGameplayTags::InitState_DataInitialized, false, HodgeGameplayTags::InitState_DataAvailable);
		Manager->RegisterInitState(HodgeGameplayTags::InitState_GameplayReady, false, HodgeGameplayTags::InitState_DataInitialized);
		return Instance->GetWorld();
	}
	/** 销毁世界。
	 * @param Instance Instance，由调用方传入。
	 * @param World 本次操作所在的世界。
	 */
	static void DestroyWorld(UGameInstance* Instance, UWorld* World)
	{
		Instance->Shutdown(); World->DestroyWorld(false); GEngine->DestroyWorldContext(World);
	}
};
