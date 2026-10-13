/** @file HodgeBTTask_ExecuteAbility.h
 * @brief 声明行为树战斗请求任务相关的类型、接口与现有处理流程。
 */
#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "Combat/HodgeCharacterFacingTypes.h"
#include "Combat/HodgeCombatActionRequest.h"
#include "HodgeBTTask_ExecuteAbility.generated.h"

class UHodgeAbilitySystemComponent;
class AHodgeEnemyAIController;
class UHodgeGameplayAbility_Definition;
class UHodgeCombatComponentBase;

/** 提交独立技能或连段意图，实例化节点隔离每个敌人的请求与回调。 */
UCLASS()
class HODGEPODGE_API UHodgeBTTask_ExecuteAbility : public UBTTaskNode
{
	GENERATED_BODY()
public:
	/** 构造行为树战斗请求任务并初始化其默认状态。 */
	UHodgeBTTask_ExecuteAbility();
	/** 从行为树资产解析节点依赖和黑板键配置。
	 * @param Asset 当前行为树资产或执行组件。
	 */
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	/** 查询目标黑板键是否已通过Actor类型校验。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool HasValidTargetKey() const { return bTargetKeyValid; }
	// 选择独立技能请求或连段语义意图请求。
	UPROPERTY(EditAnywhere, Category="Hodge|Ability") EHodgeCombatActionRequestType RequestType = EHodgeCombatActionRequestType::StandaloneAbility;
	// 查找已授予技能定义的身份Tag。
	UPROPERTY(EditAnywhere, Category="Hodge|Ability", meta=(EditCondition="RequestType == EHodgeCombatActionRequestType::StandaloneAbility")) FGameplayTag AbilityTag;
	// 连段图消费的语义输入Tag。
	UPROPERTY(EditAnywhere, Category="Hodge|Ability", meta=(EditCondition="RequestType == EHodgeCombatActionRequestType::ComboIntent")) FGameplayTag IntentTag;
	// 目标Actor或用于取得目标的选择入口。
	UPROPERTY(EditAnywhere, Category="Hodge|Ability") FBlackboardKeySelector TargetActor;
	// 等待操作完成的最长时间。
	UPROPERTY(EditAnywhere, Category="Hodge|Ability", meta=(ClampMin="0.1")) float Timeout = 8.f;
protected:
	/** 准备当前行为树任务所需的角色、目标和执行上下文。
	 * @param Owner 当前行为树资产或执行组件。
	 * @param Memory 调用方提供的状态内存。
	 * @return 行为树任务的成功、失败或等待状态。
	 */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& Owner, uint8* Memory) override;
	/** 更新行为树潜在任务，并推进准备或等待阶段。
	 * @param Owner 当前行为树资产或执行组件。
	 * @param Memory 调用方提供的状态内存。
	 * @param DeltaSeconds 本次更新的时间步长，单位秒。
	 */
	virtual void TickTask(UBehaviorTreeComponent& Owner, uint8* Memory, float DeltaSeconds) override;
	/** 取消当前任务拥有的操作并解除监听。
	 * @param Owner 当前行为树资产或执行组件。
	 * @param Memory 调用方提供的状态内存。
	 * @return 行为树任务的成功、失败或等待状态。
	 */
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& Owner, uint8* Memory) override;
	/** 在行为树任务结束时完成本节点的收尾。
	 * @param Owner 当前行为树资产或执行组件。
	 * @param Memory 调用方提供的状态内存。
	 * @param Result 本次操作的结果。
	 */
	virtual void OnTaskFinished(UBehaviorTreeComponent& Owner, uint8* Memory, EBTNodeResult::Type Result) override;
private:
	friend struct FHodgeEnemyAITestAccess;
	/** 只消费匹配本次尝试与请求身份的终结状态，结束对应行为树任务。
	 * @param Attempt 结果所属的尝试序号，用于排除旧回调。
	 * @param State 当前状态数据。
	 */
	void HandleRequestChanged(uint32 Attempt, const FHodgeCombatActionRequestState& State);
	/** 按当前对象的来源归属解除监听并清理资源。
	 * @param bCancel 是否取消的控制标记。
	 */
	void Cleanup(bool bCancel);
	/** 检查绑定对象与运行身份是否仍属于当前生命周期。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsCurrent() const;
	// 后续GAS操作使用的能力系统组件。
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeAbilitySystemComponent> ASC;
	// 当前角色使用的控制器。
	UPROPERTY(Transient) TWeakObjectPtr<AHodgeEnemyAIController> Controller;
	// 当前操作关联的Pawn角色。
	UPROPERTY(Transient) TWeakObjectPtr<APawn> Pawn;
	// 当前操作使用的树对象。
	UPROPERTY(Transient) TWeakObjectPtr<UBehaviorTreeComponent> Tree;
	// 统一战斗组件。
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeCombatComponentBase> Combat;
	// 请求状态监听的解绑句柄。
	FDelegateHandle RequestChangedHandle;
	// 本次动作持有的朝向请求句柄。
	FHodgeFacingRequestHandle Facing;
	// 本次请求的唯一身份。
	FGuid RequestId;
	// 用于拒绝旧尝试回调的序号。
	uint32 AttemptId = 0;
	// 用于隔离Possess周期的代号。
	uint32 PossessGeneration = 0;
	// 本次等待或执行已经经过的时间。
	float Elapsed = 0.f;
	// 本次尝试是否已经启动技能。
	bool bActivated = false;
	// 本次执行是否已经结束。
	bool bEnded = false;
	// 正在提交，暂不处理缺少请求身份的同步通知。
	bool bSubmitting = false;
	// 目标黑板键是否满足Actor类型契约。
	bool bTargetKeyValid = false;
};
