// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/StreamableManager.h"

// 一个“子步骤进度”委托。
// 参数是当前这个 Job 的局部进度，范围通常是 0.0 ~ 1.0。
//
// 例如：
// 当前资源加载了 30%
// => SubstepProgressDelegate.Execute(0.3f)
DECLARE_DELEGATE_OneParam(
	FHodgeAssetManagerStartupJobSubstepProgress,
	float /*NewProgress*/
);


/**
 * AssetManager 启动阶段的一个“加载任务（Job）”。
 * 一个 Job 可以代表一项启动工作，例如：
 * 1. 加载某一批 Primary Asset
 * 2. 加载某个 Gameplay Feature
 * 3. 加载某一批配置资源
 * 4. 执行某个初始化逻辑
 * 每个 Job 都有自己的：
 * - JobName：任务名称
 * - JobWeight：任务权重
 * - JobFunc：实际执行任务的函数
 * - SubstepProgressDelegate：向外报告当前任务内部的加载进度
 */
struct FHodgeAssetManagerStartupJob
{
	/**
	 * 子步骤进度回调。
	 * Job 内部加载资源时，可以通过这个 Delegate
	 * 把当前 Job 的进度通知给外部。
	 * 例如：
	 * 0.0f -> 还没开始
	 * 0.5f -> 当前 Job 完成 50%
	 * 1.0f -> 当前 Job 完成
	 */
	FHodgeAssetManagerStartupJobSubstepProgress SubstepProgressDelegate;


	/**
	 * 实际执行 Job 的函数。
	 * 参数：
	 * const FHodgeAssetManagerStartupJob&
	 *     当前 Job 自身。
	 * TSharedPtr<FStreamableHandle>&
	 *     如果这个 Job 创建了异步资源加载 Handle，
	 *     可以把 Handle 写到这里。
	 * 例如：
	 * JobFunc = [](const FHodgeAssetManagerStartupJob& Job,
	 *              TSharedPtr<FStreamableHandle>& Handle)
	 * {
	 *     Handle = StreamableManager.RequestAsyncLoad(...);
	 * };
	 * 这样 DoJob() 就可以拿到这个异步加载 Handle。
	 */
	TFunction<
		void(
			const FHodgeAssetManagerStartupJob&,
			TSharedPtr<FStreamableHandle>&
		)
	> JobFunc;


	/**
	 * Job 的名字。
	 * 主要用于：
	 * - Debug
	 * - 日志
	 * - 启动加载进度显示
	 */
	FString JobName;


	/**
	 * 当前 Job 在整个启动流程中的权重。
	 * 例如有三个 Job：
	 * 加载核心资源     Weight = 5
	 * 加载角色资源     Weight = 3
	 * 加载 UI 资源      Weight = 2
	 * 那么总权重 = 10。
	 * 核心资源占整个启动进度的 50%。
	 */
	float JobWeight;


	/**
	 * 上一次向外报告进度的时间。
	 * 用于限制 GetProgress() 的调用频率。
	 * 因为 FStreamableHandle::GetProgress()
	 * 会遍历资源依赖关系图，可能比较昂贵。
	 */
	mutable double LastUpdate = 0;


	/**
	 * 构造一个启动 Job。
	 * @param InJobName
	 *     Job 名称
	 * @param InJobFunc
	 *     实际执行 Job 的函数
	 * @param InJobWeight
	 *     Job 在整个启动流程中的权重
	 */
	FHodgeAssetManagerStartupJob(
		const FString& InJobName,
		const TFunction<
			void(
				const FHodgeAssetManagerStartupJob&,
				TSharedPtr<FStreamableHandle>&
			)
		>& InJobFunc,
		float InJobWeight
	)
		: JobFunc(InJobFunc)
		  , JobName(InJobName)
		  , JobWeight(InJobWeight)
	{
	}


	/**
	 * 执行这个 Job。
	 * 实际实现通常会调用 JobFunc。
	 * 如果 JobFunc 创建了异步资源加载，
	 * 那么返回对应的 FStreamableHandle。
	 * 如果只是同步执行的初始化逻辑，
	 * 则可能返回 nullptr。
	 */
	TSharedPtr<FStreamableHandle> DoJob() const;

	/**
	 * 手动更新当前 Job 的子步骤进度。
	 * @param NewProgress
	 *     当前 Job 的进度，通常为 0.0 ~ 1.0。
	 * 例如：
	 * UpdateSubstepProgress(0.25f);
	 * 表示：
	 * 当前 Job 完成了 25%。
	 */
	void UpdateSubstepProgress(float NewProgress) const
	{
		SubstepProgressDelegate.ExecuteIfBound(NewProgress);
	}

	/**
	 * 根据 FStreamableHandle 自动获取当前资源加载进度。
	 * StreamableHandle 可以代表一批异步加载资源。
	 * 例如：
	 * RequestAsyncLoad(...)
	 * 返回：
	 * TSharedPtr<FStreamableHandle>
	 * 然后通过：
	 * StreamableHandle->GetProgress()
	 * 获取当前加载进度。
	 */
	void UpdateSubstepProgressFromStreamable(
		TSharedRef<FStreamableHandle> StreamableHandle
	) const
	{
		// 只有外部真正绑定了进度回调，
		// 才有必要计算加载进度。
		if (SubstepProgressDelegate.IsBound())
		{
			/**
			 * 注意：
			 * FStreamableHandle::GetProgress()
			 * 并不是一个非常便宜的函数。
			 * 它需要遍历资源加载关系图。
			 * 如果每帧调用一次：
			 * Tick
			 *   -> GetProgress()
			 *   -> 遍历大量资源
			 * 可能产生额外性能开销。
			 */
			double Now = FPlatformTime::Seconds();


			/**
			 * 限制进度更新频率。
			 * 理论上：
			 * 1 / 60 = 0.016666...
			 * 也就是最多大约每 16.67ms 更新一次，
			 * 相当于最多 60 次/秒。
			 * 这样可以避免疯狂调用 GetProgress()。
			 */
			if (Now - LastUpdate > 1.0 / 60)
			{
				/**
				 * 获取 StreamableHandle 当前的整体加载进度，
				 * 然后通知外部。
				 * 例如：
				 * GetProgress() = 0.35
				 * 就会通知：
				 * SubstepProgressDelegate(0.35)
				 */
				SubstepProgressDelegate.Execute(
					StreamableHandle->GetProgress()
				);
				/**
				 * 记录这次更新时间，
				 * 下一次只有超过 1/60 秒才允许再次更新。
				 */
				LastUpdate = Now;
			}
		}
	}
};
