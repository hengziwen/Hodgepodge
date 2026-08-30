// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFeatures/GameFeatureAction_SplitscreenConfig.h"

#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "GameFeatures/GameFeatureAction_WorldActionBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_SplitscreenConfig)

#define LOCTEXT_NAMESPACE "LyraGameFeatures"

//////////////////////////////////////////////////////////////////////
// UGameFeatureAction_SplitscreenConfig

// 全局记录每个 GameViewportClient 当前有多少个 Game Feature 要求禁用分屏。
// 采用“投票”机制：只要还有一个 Feature 投票禁用，就保持禁用状态。
TMap<FObjectKey, int32> UGameFeatureAction_SplitscreenConfig::GlobalDisableVotes;

void UGameFeatureAction_SplitscreenConfig::OnGameFeatureDeactivating(
    FGameFeatureDeactivatingContext& Context)
{
    // 先执行父类逻辑，移除 WorldActionBase 注册的相关监听。
    Super::OnGameFeatureDeactivating(Context);

    // 从后往前遍历当前 Action 自己提交过的所有“禁用分屏”投票。
    for (int32 i = LocalDisableVotes.Num() - 1; i >= 0; i--)
    {
       // 获取当前投票对应的 GameViewportClient。
       FObjectKey ViewportKey = LocalDisableVotes[i];

       // 根据 ObjectKey 找回实际的 GameViewportClient 对象。
       UGameViewportClient* GVP =
          Cast<UGameViewportClient>(ViewportKey.ResolveObjectPtr());

       // 根据 ViewportClient 找到它所属的 WorldContext。
       const FWorldContext* WorldContext =
          GEngine->GetWorldContextFromGameViewport(GVP);

       // 如果对象和 WorldContext 都有效，则检查当前 Game Feature 是否应该作用于这个 World。
       if (GVP && WorldContext)
       {
          // 如果这个 World 不属于当前 Game Feature 的作用范围，则暂时跳过。
          if (!Context.ShouldApplyToWorldContext(*WorldContext))
          {
             // 错误的 WorldContext 不处理；已经失效的对象则仍然认为属于当前 Context。
             continue;
          }
       }

       // 获取这个 ViewportClient 当前的全局禁用分屏投票数量。
       int32& VoteCount = GlobalDisableVotes[ViewportKey];

       // 如果当前已经是最后一票，则移除全局记录。
       if (VoteCount <= 1)
       {
          GlobalDisableVotes.Remove(ViewportKey);

          // 没有任何 Feature 要求禁用分屏，因此恢复正常分屏状态。
          if (GVP && WorldContext)
          {
             GVP->SetForceDisableSplitscreen(false);
          }
       }
       else
       {
          // 还有其他 Feature 在投票禁用分屏，因此只减少当前 Feature 的一票。
          --VoteCount;
       }

       // 从当前 Action 的本地投票列表中移除已经撤销的投票。
       LocalDisableVotes.RemoveAt(i);
    }
}

void UGameFeatureAction_SplitscreenConfig::AddToWorld(
    const FWorldContext& WorldContext,
    const FGameFeatureStateChangeContext& ChangeContext)
{
    // 只有配置要求禁用分屏时才执行后面的逻辑。
    if (bDisableSplitscreen)
    {
       // 获取当前 World 对应的 GameInstance。
       if (UGameInstance* GameInstance = WorldContext.OwningGameInstance)
       {
          // 获取 GameInstance 对应的 GameViewportClient。
          if (UGameViewportClient* VC = GameInstance->GetGameViewportClient())
          {
             // 使用 ViewportClient 作为当前分屏配置的唯一标识。
             FObjectKey ViewportKey(VC);

             // 记录当前 Action 提交了一次禁用分屏投票。
             LocalDisableVotes.Add(ViewportKey);

             // 获取这个 ViewportClient 的全局投票数量。
             int32& VoteCount = GlobalDisableVotes.FindOrAdd(ViewportKey);

             // 当前 Feature 投一票。
             VoteCount++;

             // 只有从 0 变成 1 时才真正执行禁用分屏。
             if (VoteCount == 1)
             {
                VC->SetForceDisableSplitscreen(true);
             }
          }
       }
    }
}

#undef LOCTEXT_NAMESPACE