/** @file HodgeEnemyAIController.h
 * @brief 声明敌人AI控制器相关的类型、接口与现有处理流程。
 */
#pragma once

#include "AIController.h"
#include "GameplayTagContainer.h"
#include "Perception/AIPerceptionTypes.h"
#include "HodgeEnemyAIController.generated.h"

class UBehaviorTree;
class UAIPerceptionComponent;
class UHodgeEnemyInitComponent;
class UHodgeAbilitySystemComponent;

/** 定义敌人决策状态的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeEnemyBrainState : uint8 { /* 等待必要依赖。 */ Waiting, /* 正在启动当前流程。 */ Starting, /* 当前流程正在运行。 */ Running, /* 配置或执行失败。 */ Failed, /* 当前流程已经停止。 */ Stopped };

/** 只在服务器运行决策，每次 Possess 独立绑定 EnemyInit。 */
UCLASS(Blueprintable)
class HODGEPODGE_API AHodgeEnemyAIController : public AAIController
{
	GENERATED_BODY()
public:
	/** 构造敌人AI控制器并初始化其默认状态。
	 * @param Initializer UE构造对象时传入的初始化器。
	 */
	AHodgeEnemyAIController(const FObjectInitializer& Initializer = FObjectInitializer::Get());
	/** 在当前Possess周期满足依赖时启动行为树。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Enemy") bool TryStartBrain();
	/** 获取敌人决策状态。
	 * @return 敌人决策状态的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Enemy") EHodgeEnemyBrainState GetEnemyBrainState() const { return State; }
	/** 获取决策起始数量。
	 * @return 决策起始数量的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Enemy") int32 GetBrainStartCount() const { return StartCount; }
	/** 获取Possess初始化代。
	 * @return Possess初始化代的当前值或引用。
	 */
	uint32 GetPossessGeneration() const { return PossessGeneration; }
	/** 判断Possess当前。
	 * @param ControlledPawn ControlledPawn角色，由调用方传入。
	 * @param Generation 初始化代，由调用方传入。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsPossessCurrent(const APawn* ControlledPawn, uint32 Generation) const;
	/** 判断有效敌人目标。
	 * @param Target 本次操作的目标。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsValidEnemyTarget(const AActor* Target) const;
	// 该AI控制器启动时运行的行为树资产。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Enemy") TObjectPtr<UBehaviorTree> BehaviorTree;
	// 黑板中保存当前敌对目标的键名。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Enemy") FName TargetKey = TEXT("TargetActor");
	// 进入攻击分支的距离阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Enemy") float AttackRange = 180.f;
	// 保持当前攻击判定的退出距离阈值，用于范围迟滞。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Enemy") float AttackExitRange = 220.f;
	// 丢失视线后保留最后已知目标的时间，单位秒。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Enemy") float TargetMemorySeconds = 3.f;
	// AI视线感知使用的范围半径。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Enemy") float SightRadius = 1400.f;
protected:
	/** 在组件初始化前接入本对象所需的框架入口。 */
	virtual void PreInitializeComponents() override;
	/** 进入世界运行阶段并接入本对象的生命周期依赖。 */
	virtual void BeginPlay() override;
	/** 开始新的控制器Possess周期并接入被控制Pawn。
	 * @param ControlledPawn ControlledPawn角色，由调用方传入。
	 */
	virtual void OnPossess(APawn* ControlledPawn) override;
	/** 结束当前Possess周期并解除旧Pawn依赖。 */
	virtual void OnUnPossess() override;
	/** 离开世界运行阶段并解除本对象持有的监听或资源。
	 * @param Reason Reason，由调用方传入。
	 */
	virtual void EndPlay(EEndPlayReason::Type Reason) override;
private:
	friend struct FHodgeEnemyAITestAccess;
	/** 处理就绪。 */
	void HandleReadiness();
	/** 更新感知目标、行动限制和行为树黑板决策数据。 */
	void UpdateDecision();
	/** 处理约束。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 * @param Count 本次处理涉及的数量。
	 */
	void HandleConstraint(FGameplayTag Tag, int32 Count);
	/** 停止本次AI控制周期并解绑其决策依赖。
	 * @param Reason Reason，由调用方传入。
	 */
	void StopCycle(const FString& Reason);
	/** 更新可见候选并触发必要的AI决策刷新。
	 * @param Actor 本次操作关联的Actor。
	 * @param Stimulus Stimulus，由调用方传入。
	 */
	UFUNCTION() void HandlePerception(AActor* Actor, FAIStimulus Stimulus);
	// 当前怪物的视线感知组件。
	UPROPERTY(VisibleAnywhere) TObjectPtr<UAIPerceptionComponent> EnemyPerception;
	// 当前角色的初始化协调组件。
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeEnemyInitComponent> Init;
	// 后续GAS操作使用的能力系统组件。
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeAbilitySystemComponent> ASC;
	// 本次操作的目标。
	UPROPERTY(Transient) TWeakObjectPtr<AActor> Target;
	// 当前状态数据。
	UPROPERTY(Transient) EHodgeEnemyBrainState State = EHodgeEnemyBrainState::Stopped;
	// 感知当前仍可见的有效目标集合。
	TSet<TWeakObjectPtr<AActor>> VisibleTargets;
	// 死亡Tag事件的解绑句柄。
	FDelegateHandle DeathHandle;
	// 强控制Tag事件的解绑句柄。
	FDelegateHandle ControlHandle;
	// 定时更新AI决策的计时器句柄。
	FTimerHandle DecisionTimer;
	// 用于隔离Possess周期的代号。
	uint32 PossessGeneration = 0;
	// 当前控制器成功启动行为树的累计次数。
	int32 StartCount = 0;
	// 最近一次看见有效目标的世界时间。
	double LastSeenTime = 0.;
	// 控制是否输入攻击范围的状态标记。
	bool bInAttackRange = false;
};
