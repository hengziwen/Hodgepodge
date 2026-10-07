#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "Combat/HodgeHitDetection.h"
#include "Engine/EngineBaseTypes.h"
#include "HodgeAbilityTask_WaitHitResults.generated.h"

class UHodgeAbilityTask_PlayTimeline;
class UHodgeGameplayAbility_Definition;
class UHodgeCombatComponentBase;
class UHodgeAbilityTask_WaitHitResults;

// 独立 Tick 保证骨骼更新后采样，不改变 ASC 上其他任务的阶段。
struct FHodgeHitResultsTickFunction final : FTickFunction
{
	TWeakObjectPtr<UHodgeAbilityTask_WaitHitResults> Target;
	virtual void ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread,
		const FGraphEventRef& CompletionEvent) override;
	virtual FString DiagnosticMessage() override;
};

/** 一个 GA 执行拥有一个采样 Task，每个 Timeline 窗口拥有独立检测会话。 */
UCLASS()
class HODGEPODGE_API UHodgeAbilityTask_WaitHitResults : public UAbilityTask
{
	GENERATED_BODY()
public:
	static UHodgeAbilityTask_WaitHitResults* WaitHitResults(UHodgeGameplayAbility_Definition* OwningAbility,
		UHodgeCombatComponentBase* Combat, UHodgeAbilityTask_PlayTimeline* Timeline, const FGuid& ExecutionId);

	DECLARE_MULTICAST_DELEGATE_OneParam(FHitResults, const FHodgeHitDetectionBatch&);
	FHitResults OnHitResults;

	// 创建不回调；GA 登记规则和句柄后显式调用首次采样。
	uint64 CreateWindow(int32 EventIndex, const FHodgeHitDetectionRequest& Request);
	void SampleWindow(int32 EventIndex);
	void CloseWindow(int32 EventIndex, bool bSampleFinal);
	bool IsRunningForExecution(const FGuid& ExecutionId) const;
	bool IsBatchCurrent(const FHodgeHitDetectionBatch& Batch) const;
	virtual void Pause() override;
	virtual void Resume() override;
	virtual void BeginDestroy() override;

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	friend struct FHodgeHitResultsTickFunction;
	friend struct FHodgeMeleeTestAccess;
	struct FWindow
	{
		uint64 Handle = 0;
		bool bClosing = false;
		bool bSampled = false;
		bool bOnce = false;
	};
	void TickDetection();
	void ReleaseSessions();
	void UpdateTickState();

	FGuid ExecutionId;
	TMap<int32, FWindow> Windows;
	FHodgeHitResultsTickFunction DetectionTick;
	UPROPERTY() TWeakObjectPtr<UHodgeGameplayAbility_Definition> ExecutionAbility;
	UPROPERTY() TWeakObjectPtr<UHodgeCombatComponentBase> CombatComponent;
	UPROPERTY() TWeakObjectPtr<UHodgeAbilityTask_PlayTimeline> TimelineTask;
	UPROPERTY() TWeakObjectPtr<AActor> Avatar;
	bool bStopped = false;
	bool bSampling = false;
	bool bTickingDetection = false;
};
