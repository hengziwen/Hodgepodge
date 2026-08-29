// 111屎山代码来袭

#include "Data/HodgeAssetManagerStartupJob.h"

/**
 * 执行当前的 AssetManager 启动任务。
 * 一个 Job 可能：
 * 1. 只是执行一些同步初始化逻辑
 * 2. 发起一批异步资源加载，并返回 FStreamableHandle
 * 如果返回了 FStreamableHandle，
 * 这里会监听它的加载进度，并等待这批资源加载完成。
 */
TSharedPtr<FStreamableHandle> FHodgeAssetManagerStartupJob::DoJob() const
{
	// 记录 Job 开始执行的时间。
	// 后面用于统计这个启动任务一共耗时多少秒。
	const double JobStartTime = FPlatformTime::Seconds();

	// 用来接收 JobFunc 执行后产生的异步加载 Handle。
	// 如果 JobFunc 内部调用：
	// StreamableManager.RequestAsyncLoad(...)
	// 那么通常会把返回的 FStreamableHandle 保存到这里。
	TSharedPtr<FStreamableHandle> Handle;
	
	UE_LOG(LogTemp, Display, TEXT("Startup job \"%s\" starting"), *JobName);

	// 真正执行这个 Job。
	// JobFunc 是前面构造 FHodgeAssetManagerStartupJob 时传进来的函数。
	// 第一个参数：
	//     *this
	//     把当前 Job 自己传进去，
	//     这样 JobFunc 内部可以调用：
	//     Job.UpdateSubstepProgress(...)
	//     或：
	//     Job.UpdateSubstepProgressFromStreamable(...)
	// 第二个参数：
	//     Handle
	//     用于让 JobFunc 把自己创建的 FStreamableHandle 返回出来。
	JobFunc(*this, Handle);

	// 如果 JobFunc 创建了异步资源加载，
	// 那么 Handle 就应该是有效的。
	// 如果这个 Job 只是同步执行一些初始化代码，
	// Handle 可能就是 nullptr。
	if (Handle.IsValid())
	{
		/**
		 * 给这个 StreamableHandle 绑定“进度更新回调”。
		 * 当资源加载进度发生变化时，
		 * FStreamableHandle 会调用这个 Delegate。
		 * CreateRaw(this, ...)
		 * 表示：
		 * 当前 FHodgeAssetManagerStartupJob 对象
		 * 接收 FStreamableHandle 的进度通知。
		 * 最终会调用：
		 * UpdateSubstepProgressFromStreamable(...)
		 * 然后这个函数内部再：
		 * StreamableHandle->GetProgress()
		 * 获取当前资源加载进度。
		 */
		Handle->BindUpdateDelegate(
			FStreamableUpdateDelegate::CreateRaw(
				this,
				&FHodgeAssetManagerStartupJob::UpdateSubstepProgressFromStreamable
			)
		);

		/**
		 * 等待当前 Handle 对应的资源加载完成。
		 * 这里传入：
		 * 0.0f
		 *     表示不设置额外的超时时间。
		 * false
		 *     不进行额外的强制处理。
		 *
		 * 注意：
		 * 虽然前面使用的是 RequestAsyncLoad，
		 * 但是到了这里，Startup Job 会等待它完成。
		 * 所以这里的“异步加载”主要是指：
		 * FStreamableManager 使用异步资源加载机制，
		 * 而 Startup Job 本身会等待这批资源加载完，
		 * 才认为这个 Startup Job 完成。
		 */
		Handle->WaitUntilComplete(0.0f, false);
		/**
		 * 资源加载完成以后，解除进度回调。
		 * 因为这个 Job 已经结束了，
		 * 不再需要继续监听 Handle 的进度变化。
		 * 这里绑定一个空的 Delegate，
		 * 相当于清除之前的 Update Delegate。
		 */
		Handle->BindUpdateDelegate(
			FStreamableUpdateDelegate()
		);
	}

	// 计算这个 Job 从开始到结束一共耗时多少秒。
	UE_LOG(LogTemp, Display, TEXT("Startup job \"%s\" took %.2f seconds to complete"), *JobName,
	       FPlatformTime::Seconds() - JobStartTime
	);

	// 返回这个 Job 创建的 StreamableHandle。
	// 如果这个 Job 没有异步加载资源，
	// 那么这里可能返回 nullptr。
	return Handle;
}
