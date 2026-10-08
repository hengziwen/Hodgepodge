// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/HUD/HodgeHUD.h"

// GAS 的 AbilitySystemComponent。
// Debug Actor 收集时会遍历当前进程中的 ASC，并取得它们的 OwnerActor / AvatarActor。
#include "AbilitySystemComponent.h"

// GAS 全局工具类。
// 用于从 Actor 上查询对应的 AbilitySystemComponent。
#include "AbilitySystemGlobals.h"

// UE TaskGraph 相关接口。
#include "Async/TaskGraphInterfaces.h"

// GameFrameworkComponentManager。
// 用于将 AHodgeHUD 注册成 GameFramework Component Receiver，
// 从而支持 GameFeature / Modular Gameplay 对 HUD Actor 进行动态扩展。
#include "Components/GameFrameworkComponentManager.h"

// UObject 全局迭代器。
// GetDebugActorList 中通过 TObjectIterator 遍历当前存在的 AbilitySystemComponent。
#include "UObject/UObjectIterator.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeHUD)

// Actor 前向声明。
class AActor;

// World 前向声明。
class UWorld;

//////////////////////////////////////////////////////////////////////
// AHodgeHUD

// 构造函数。
AHodgeHUD::AHodgeHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// AHodgeHUD 本身没有需要每帧执行的 Tick 逻辑，
	// 因此默认关闭 Actor Tick，避免无意义的每帧更新开销。
	PrimaryActorTick.bStartWithTickEnabled = false;
}

// Actor 组件正式初始化之前的生命周期阶段。
void AHodgeHUD::PreInitializeComponents()
{
	// 先执行 AHUD / AActor 的默认预初始化流程。
	Super::PreInitializeComponents();

	// 将当前 AHodgeHUD 注册为 GameFrameworkComponentManager 的 Receiver。
	//
	// 注册后，GameFeature / Modular Gameplay 系统
	// 才能够识别这个 HUD Actor，并针对它执行组件注入或扩展逻辑。
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

// HUD Actor 正式进入游戏时调用。
void AHodgeHUD::BeginPlay()
{
	// 向 GameFrameworkComponentManager 广播 NAME_GameActorReady 扩展事件。
	//
	// 表示当前 AHodgeHUD 已经完成基础初始化，
	// 可以开始执行依赖“Actor 已准备完成”状态的 GameFeature 扩展逻辑。
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(
		this, UGameFrameworkComponentManager::NAME_GameActorReady);

	// 执行父类 BeginPlay。
	Super::BeginPlay();
}

// HUD Actor 结束生命周期时调用。
void AHodgeHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 从 GameFrameworkComponentManager 中注销当前 Receiver。
	//
	// HUD 销毁后不应该继续接收来自 GameFeature / Modular Gameplay 的扩展事件。
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	// 执行父类 EndPlay。
	Super::EndPlay(EndPlayReason);
}

// 收集可以参与 HUD Debug 显示的 Actor。
void AHodgeHUD::GetDebugActorList(TArray<AActor*>& InOutList)
{
	// 获取当前 HUD 所属 World。
	// 后续添加 Debug Actor 时需要确保 Actor 属于正确的 World。
	UWorld* World = GetWorld();

	// 先保留 AHUD 默认提供的 Debug Actor。
	Super::GetDebugActorList(InOutList);

	// Add all actors with an ability system component.
	// 将拥有 AbilitySystemComponent 的 Gameplay Actor
	// 额外加入 HUD 的 Debug Actor 列表。
	//
	// 这样使用 ShowDebug 等调试功能时，
	// 可以方便地查看参与 GAS 系统的 Actor。
	for (TObjectIterator<UAbilitySystemComponent> It; It; ++It)
	{
		// 获取当前遍历到的 AbilitySystemComponent。
		if (UAbilitySystemComponent* ASC = *It)
		{
			// 排除 CDO 和 Archetype。
			//
			// 这里只关心游戏运行时真正存在的 ASC 实例，
			// 而不是类默认对象或模板对象。
			if (!ASC->HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
			{
				// GAS 中 ASC 的 AvatarActor。
				//
				// AvatarActor 通常代表当前实际执行 Ability / Gameplay 行为的“肉体”，
				// 例如当前玩家控制的 Pawn / Character。
				AActor* AvatarActor = ASC->GetAvatarActor();

				// GAS 中 ASC 的 OwnerActor。
				//
				// OwnerActor 表示真正拥有 ASC 的 Actor。
				// 在 Hodge 当前 PlayerState 持有 ASC 的设计中，
				// 玩家 ASC 的 OwnerActor 通常就是 PlayerState。
				AActor* OwnerActor = ASC->GetOwnerActor();

				// 优先尝试使用 AvatarActor 作为 Debug Actor。
				//
				// 再次通过 AbilitySystemGlobals 验证该 Actor
				// 确实能够取得 AbilitySystemComponent。
				if (AvatarActor && UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(AvatarActor))
				{
					// 将 AvatarActor 添加到当前 World 的 HUD Debug Actor 列表。
					AddActorToDebugList(AvatarActor, InOutList, World);
				}

				// 如果没有有效 AvatarActor，
				// 则退回使用真正拥有 ASC 的 OwnerActor。
				else if (OwnerActor && UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerActor))
				{
					// 将 OwnerActor 添加到 HUD Debug Actor 列表。
					AddActorToDebugList(OwnerActor, InOutList, World);
				}
			}
		}
	}
}
