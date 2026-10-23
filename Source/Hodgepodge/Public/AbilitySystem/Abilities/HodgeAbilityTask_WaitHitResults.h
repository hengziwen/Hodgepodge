/** @file HodgeAbilityTask_WaitHitResults.h
 * @brief 声明命中结果等待任务相关的类型、接口与现有处理流程。
 */
#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "Combat/HodgeHitDetection.h"
#include "Engine/EngineBaseTypes.h"
#include "HodgeAbilityTask_WaitHitResults.generated.h"

class UHodgeGameplayAbility_Definition;
class UHodgeCombatComponentBase;
class UHodgeHitDetectionComponent;
class UHodgeAbilityTask_WaitHitResults;

// 独立 Tick 保证骨骼更新后采样，不改变 ASC 上其他任务的阶段。
struct FHodgeHitResultsTickFunction final : FTickFunction
{
	// 本次操作的目标。
	TWeakObjectPtr<UHodgeAbilityTask_WaitHitResults> Target;
	/** 执行本类型登记的帧更新入口。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 * @param TickType 帧更新类型，由调用方传入。
	 * @param CurrentThread 当前线程，由调用方传入。
	 * @param CompletionEvent Completion事件，由调用方传入。
	 */
	virtual void ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread,
		const FGraphEventRef& CompletionEvent) override;
	/** 生成此更新入口的调试说明。
	 * @return 本次生成的文字或诊断信息。
	 */
	virtual FString DiagnosticMessage() override;
};

/** 一个 GA 执行拥有一个采样 Task，每个通知区间拥有独立检测会话。 */
UCLASS()
class HODGEPODGE_API UHodgeAbilityTask_WaitHitResults : public UAbilityTask
{
	GENERATED_BODY()
public:
	/** 创建按会话采样命中结果的AbilityTask。
	 * @param OwningAbility Owning能力，由调用方传入。
	 * @param Combat 统一战斗组件。
	 * @param ExecutionId 本次动作执行的唯一身份。
	 * @return 本次计算、查询或创建得到的能力任务等待命中Results。
	 */
	static UHodgeAbilityTask_WaitHitResults* WaitHitResults(UHodgeGameplayAbility_Definition* OwningAbility,
		UHodgeCombatComponentBase* Combat, const FGuid& ExecutionId);

	DECLARE_MULTICAST_DELEGATE_OneParam(FHitResults, const FHodgeHitDetectionBatch&);
	// 事件命中Results对应的事件通知。
	FHitResults OnHitResults;

	// 创建不回调；GA 登记规则和句柄后显式调用首次采样。
	/** 创建窗口。
	 * @param OccurrenceId 通知次数标识，由调用方传入。
	 * @param Request 请求，由调用方传入。
	 * @return 本次计算、查询或创建得到的uint64。
	 */
	uint64 CreateWindow(int32 OccurrenceId, const FHodgeHitDetectionRequest& Request);
	/** 按当前动画窗口采样检测几何与候选命中。
	 * @param OccurrenceId 通知次数标识，由调用方传入。
	 */
	void SampleWindow(int32 OccurrenceId);
	/** 关闭窗口。
	 * @param OccurrenceId 通知次数标识，由调用方传入。
	 * @param bSampleFinal 是否采样最终的控制标记。
	 */
	void CloseWindow(int32 OccurrenceId, bool bSampleFinal);
	/** 判断Running关联执行。
	 * @param ExecutionId 本次动作执行的唯一身份。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsRunningForExecution(const FGuid& ExecutionId) const;
	/** 判断Batch当前。
	 * @param Batch Batch，由调用方传入。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsBatchCurrent(const FHodgeHitDetectionBatch& Batch) const;
	/** 暂停本对象当前的采样或更新。 */
	virtual void Pause() override;
	/** 恢复本对象当前的采样或更新。 */
	virtual void Resume() override;
	/** 在UObject销毁前清理仍由本对象持有的资源。 */
	virtual void BeginDestroy() override;

protected:
	/** 开始本对象的执行流程。 */
	virtual void Activate() override;
	/** 处理销毁。
	 * @param bInOwnerFinished 是否输入所属对象已结束的控制标记。
	 */
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	// 几何与来源状态只保存在独立检测组件，Task 不再经 Combat 采样。
	UPROPERTY() TWeakObjectPtr<UHodgeHitDetectionComponent> DetectionComponent;
	friend struct FHodgeHitResultsTickFunction;
	friend struct FHodgeMeleeTestAccess;
	/** 保存窗口的数据和接口契约。 */
	struct FWindow
	{
		// 对应操作或能力的句柄。
		uint64 Handle = 0;
		// 控制是否Closing的状态标记。
		bool bClosing = false;
		// 控制是否已采样的状态标记。
		bool bSampled = false;
		// 控制是否Once的状态标记。
		bool bOnce = false;
	};
	/** 更新检测。 */
	void TickDetection();
	/** 准备当前会话所需的翼姿势；普通来源沿用原更新链路。 */
	bool PrepareSourcePose(uint64 Handle);
	/** 释放Sessions。 */
	void ReleaseSessions();
	/** 更新帧更新状态。 */
	void UpdateTickState();

	// 本次动作执行的唯一身份。
	FGuid ExecutionId;
	// Windows的记录集合。
	TMap<int32, FWindow> Windows;
	// 检测帧更新的数据或对象引用。
	FHodgeHitResultsTickFunction DetectionTick;
	// 执行能力的弱引用，不延长目标对象生命周期。
	UPROPERTY() TWeakObjectPtr<UHodgeGameplayAbility_Definition> ExecutionAbility;
	// 战斗组件的弱引用，不延长目标对象生命周期。
	UPROPERTY() TWeakObjectPtr<UHodgeCombatComponentBase> CombatComponent;
	// 表现角色的弱引用，不延长目标对象生命周期。
	UPROPERTY() TWeakObjectPtr<AActor> Avatar;
	// 控制是否Stopped的状态标记。
	bool bStopped = false;
	// 控制是否采样的状态标记。
	bool bSampling = false;
	// 控制是否Ticking检测的状态标记。
	bool bTickingDetection = false;
};
