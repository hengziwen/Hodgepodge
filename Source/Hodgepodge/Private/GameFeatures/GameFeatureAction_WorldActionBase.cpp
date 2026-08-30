// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFeatures/GameFeatureAction_WorldActionBase.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_WorldActionBase)

void UGameFeatureAction_WorldActionBase::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
    // 注册 GameInstance 启动委托：之后每当新的 GameInstance 启动时，都会调用 HandleGameInstanceStart。
    GameInstanceStartHandles.FindOrAdd(Context) = FWorldDelegates::OnStartGameInstance.AddUObject(
        this,
        &UGameFeatureAction_WorldActionBase::HandleGameInstanceStart,
        FGameFeatureStateChangeContext(Context)
    );

    // 遍历当前已经存在的所有 World，避免 GameFeature 激活时漏掉已经初始化完成的 World。
    for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
    {
        // 判断当前 GameFeature 是否应该应用到这个 World。
        if (Context.ShouldApplyToWorldContext(WorldContext))
        {
            // 对已经存在且符合条件的 World 执行具体的 GameFeature 逻辑。
            AddToWorld(WorldContext, Context);
        }
    }
}

void UGameFeatureAction_WorldActionBase::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
    // 找到当前 GameFeature 激活时注册的 GameInstance 启动委托。
    FDelegateHandle* FoundHandle = GameInstanceStartHandles.Find(Context);

    // 确保之前确实注册过对应的委托。
    if (ensure(FoundHandle))
    {
        // GameFeature 停用时解除委托，避免后续新 GameInstance 启动时继续执行该 Action。
        FWorldDelegates::OnStartGameInstance.Remove(*FoundHandle);
    }
}

void UGameFeatureAction_WorldActionBase::HandleGameInstanceStart(
    UGameInstance* GameInstance,
    FGameFeatureStateChangeContext ChangeContext)
{
    // 获取这个 GameInstance 对应的 WorldContext。
    if (FWorldContext* WorldContext = GameInstance->GetWorldContext())
    {
        // 判断当前 GameFeature 是否应该应用到这个新启动的 World。
        if (ChangeContext.ShouldApplyToWorldContext(*WorldContext))
        {
            // 对新启动并符合条件的 World 执行具体的 GameFeature 逻辑。
            AddToWorld(*WorldContext, ChangeContext);
        }
    }
}