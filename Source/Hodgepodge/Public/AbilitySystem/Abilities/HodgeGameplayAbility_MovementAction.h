/** @file HodgeGameplayAbility_MovementAction.h
 * @brief 声明玩法能力移动动作相关的类型、接口与现有处理流程。
 */
#pragma once

#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "Combat/HodgeMovementActionTypes.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "HodgeGameplayAbility_MovementAction.generated.h"

class UHodgeSprintAbilityProfile;
class UHodgeLocomotionPolicyComponent;
class UHodgeCharacterRotationComponent;

/** GAS 激活关联的方向快照，客户端不能提交速度、费用或防御时间。 */
USTRUCT()
struct FHodgeMovementActionTargetData : public FGameplayAbilityTargetData
{
	GENERATED_BODY()
	// 输入待处理的数据。
	FVector2D Input = FVector2D::ZeroVector;
	// 控制Yaw角的数值或阈值。
	float ControlYaw = 0.f;
	// Actor对象Yaw角时刻起始的数值或阈值。
	float ActorYawAtStart = 0.f;
	// 本次会话的唯一身份。
	int32 SessionId = 0;
	/** 获取ScriptStruct。
	 * @return ScriptStruct的当前值或引用。
	 */
	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct(); }
	/** 按既有协议读写网络载荷及对象引用。
	 * @param Ar 读取或写入数据的归档对象。
	 * @param Map 序列化使用的对象映射或当前映射表。
	 * @param bSuccess 是否成功的控制标记。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bSuccess);
};
/** 保存TStructOps类型TraitsFHodge移动动作目标数据的数据和接口契约。 */
template<> struct TStructOpsTypeTraits<FHodgeMovementActionTargetData> : TStructOpsTypeTraitsBase2<FHodgeMovementActionTargetData>
{ enum { WithNetSerializer = true, WithCopy = true }; };

/** 两份移动能力复用目标数据及生命周期，不继承攻击 Definition。 */
UCLASS(Abstract, Blueprintable)
class HODGEPODGE_API UHodgeGameplayAbility_MovementAction : public UHodgeGameplayAbility
{
	GENERATED_BODY()
public:
	/** 构造玩法能力移动动作并初始化其默认状态。 */
	UHodgeGameplayAbility_MovementAction();
	// 当前操作使用的配置对象。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UHodgeSprintAbilityProfile> Profile;
	/** 判断是否存在Committed移动。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	UFUNCTION(BlueprintPure) bool HasCommittedMovement() const { return bCommitted; }
	/** 获取输入会话标识。
	 * @return 输入会话标识的当前值或引用。
	 */
	UFUNCTION(BlueprintPure) int32 GetInputSessionId() const { return SessionId; }
	/** 获取移动执行标识。
	 * @return 移动执行标识的当前值或引用。
	 */
	FGuid GetMovementExecutionId() const { return ExecutionId; }
	/** 解析配置。
	 * @param Info 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @return 本次计算、查询或创建得到的Dash/Sprint参数配置。
	 */
	const UHodgeSprintAbilityProfile* ResolveProfile(const FGameplayAbilityActorInfo* Info) const;
	/** 检查冷却。
	 * @param Handle 对应操作或能力的句柄。
	 * @param Info 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param Tags 标签，用于对应标签查询或匹配。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	virtual bool CheckCooldown(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayTagContainer* Tags = nullptr) const override { return true; }
	/** 应用冷却。
	 * @param Handle 对应操作或能力的句柄。
	 * @param Info 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param ActivationInfo 本次技能激活与预测信息。
	 */
	virtual void ApplyCooldown(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayAbilityActivationInfo ActivationInfo) const override {}
	/** 检查消耗。
	 * @param Handle 对应操作或能力的句柄。
	 * @param Info 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param Tags 标签，用于对应标签查询或匹配。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	virtual bool CheckCost(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayTagContainer* Tags = nullptr) const override { return true; }
	/** 应用消耗。
	 * @param Handle 对应操作或能力的句柄。
	 * @param Info 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param ActivationInfo 本次技能激活与预测信息。
	 */
	virtual void ApplyCost(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayAbilityActivationInfo ActivationInfo) const override {}
	/** 检查技能激活所需的上下文、标签与现有GAS限制。
	 * @param Handle 对应操作或能力的句柄。
	 * @param Info 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param SourceTags 来源标签，用于对应标签查询或匹配。
	 * @param TargetTags 目标标签，用于对应标签查询或匹配。
	 * @param RelevantTags Relevant标签，用于对应标签查询或匹配。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* RelevantTags = nullptr) const override;
protected:
	/** 启动本次技能，按现有配置提交消耗并建立执行资源。
	 * @param Handle 对应操作或能力的句柄。
	 * @param Info 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param ActivationInfo 本次技能激活与预测信息。
	 * @param Event 事件，由调用方传入。
	 */
	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* Event) override;
	/** 结束本次技能并释放其拥有的资源，再交还GAS生命周期。
	 * @param Handle 对应操作或能力的句柄。
	 * @param Info 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param ActivationInfo 本次技能激活与预测信息。
	 * @param bReplicate 是否Replicate的控制标记。
	 * @param bCancelled 是否已取消的控制标记。
	 */
	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate, bool bCancelled) override;
	/** 开始移动。
	 * @param Intent 意图，由调用方传入。
	 */
	virtual void BeginMovement(const FHodgeMoveIntentSnapshot& Intent) PURE_VIRTUAL(UHodgeGameplayAbility_MovementAction::BeginMovement, );
	/** 更新移动。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 */
	virtual void UpdateMovement(float DeltaTime) PURE_VIRTUAL(UHodgeGameplayAbility_MovementAction::UpdateMovement, );
	/** 处理移动FollowUp。
	 * @param Data 当前操作使用的数据。
	 */
	virtual void HandleMovementFollowUp(const FGameplayAbilityTargetDataHandle& Data) {}
	/** 结束当前任务阶段并完成其收尾。
	 * @param bCancelled 是否已取消的控制标记。
	 */
	void Finish(bool bCancelled = true);
	/** 判断移动有效。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsMovementValid() const;
	/** 设置移动标签。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 * @param bAdd 是否添加的控制标记。
	 */
	void SetMovementTag(FGameplayTag Tag, bool bAdd);
	/** 按当前移动动作配置开始GameplayCue表现。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 */
	void StartCue(FGameplayTag Tag);
	/** 结束当前动作拥有的GameplayCue表现。 */
	void StopCue();
	// 执行配置的数据或对象引用。
	UPROPERTY(Transient) TObjectPtr<const UHodgeSprintAbilityProfile> ExecutionProfile;
	// 策略的弱引用，不延长目标对象生命周期。
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeLocomotionPolicyComponent> Policy;
	// 朝向组件或旋转数据。
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeCharacterRotationComponent> Rotation;
	// 本次动作执行的唯一身份。
	FGuid ExecutionId;
	// 本次会话的唯一身份。
	int32 SessionId = 0;
	// Actor对象Yaw角时刻起始的数值或阈值。
	float ActorYawAtStart = 0.f;
	// 控制是否Committed的状态标记。
	bool bCommitted = false;
	// Preparing时刻的数值或阈值。
	double PreparingAt = 0.;
	// Committed时刻的数值或阈值。
	double CommittedAt = 0.;
	// 朝向句柄对应操作的配对句柄。
	FHodgeFacingRequestHandle FacingHandle;
	// 移动标签的数据或对象引用。
	FGameplayTag MovementTag;
private:
	/** 处理目标数据。
	 * @param Data 当前操作使用的数据。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 */
	void OnTargetData(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag Tag);
	/** 更新移动。 */
	void TickMovement();
	/** 处理约束变化。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 * @param Count 本次处理涉及的数量。
	 */
	void OnConstraintChanged(FGameplayTag Tag, int32 Count);
	// 约束Handles对应的事件通知。
	FDelegateHandle ConstraintHandles[3];
	// 数据句柄对应的事件通知。
	FDelegateHandle DataHandle;
	// 帧更新句柄对应操作的配对句柄。
	FTimerHandle TickHandle;
	// 活动GameplayCue的数据或对象引用。
	FGameplayTag ActiveCue;
	// 控制是否Received数据的状态标记。
	bool bReceivedData = false;
};
