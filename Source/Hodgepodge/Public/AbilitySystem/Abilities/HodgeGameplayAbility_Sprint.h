/** @file HodgeGameplayAbility_Sprint.h
 * @brief 声明玩法能力Sprint奔跑相关的类型、接口与现有处理流程。
 */
#pragma once

#include "AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.h"
#include "HodgeGameplayAbility_Sprint.generated.h"

class UAbilityTask_PlayMontageAndWait;
/** 保存Sprint奔跑掉头目标数据的数据和接口契约。 */
USTRUCT()
struct FHodgeSprintPivotTargetData : public FGameplayAbilityTargetData
{
	GENERATED_BODY()
	// Incoming方向的几何或变换数据。
	FVector IncomingDirection = FVector::ZeroVector;
	// 期望方向的几何或变换数据。
	FVector DesiredDirection = FVector::ZeroVector;
	// StartingYaw角的数值或阈值。
	float StartingYaw = 0.f;
	// 本次会话的唯一身份。
	int32 SessionId = 0;
	// 序号标识的数值或阈值。
	int32 SequenceId = 0;
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
/** 保存TStructOps类型TraitsFHodgeSprint奔跑掉头目标数据的数据和接口契约。 */
template<> struct TStructOpsTypeTraits<FHodgeSprintPivotTargetData> : TStructOpsTypeTraitsBase2<FHodgeSprintPivotTargetData>
{ enum { WithNetSerializer = true, WithCopy = true }; };

/** 封装玩法能力Sprint奔跑的状态与操作接口。 */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeGameplayAbility_Sprint : public UHodgeGameplayAbility_MovementAction
{
	GENERATED_BODY()
public:
	/** 构造玩法能力Sprint奔跑并初始化其默认状态。 */
	UHodgeGameplayAbility_Sprint();
	/** 判断Pivoting。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	UFUNCTION(BlueprintPure) bool IsPivoting() const { return bPivoting; }
	/** 检查当前速度、方向与状态是否满足掉头起手条件。
	 * @param Velocity 速度，由调用方传入。
	 * @param Desired 期望，由调用方传入。
	 * @param MinimumSpeed 最小速度，由调用方传入。
	 * @param Angle 角度，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	static bool QualifiesPivot(FVector Velocity, FVector Desired, float MinimumSpeed, float Angle);
	/** 判断掉头恢复Complete。
	 * @param Position 位置，由调用方传入。
	 * @param EndTime 结束时间，由调用方传入。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	static bool IsPivotRecoveryComplete(float Position, float EndTime);
	/** 在输入交给 CMC 前捕获掉头起手，避免制动后丢失来向速度。 */
	/** 按当前输入与会话处理移动或掉头衔接。
	 * @param Desired 期望，由调用方传入。
	 */
	void ProcessMovementIntent(FVector Desired);
protected:
	/** 开始移动。
	 * @param Intent 意图，由调用方传入。
	 */
	virtual void BeginMovement(const FHodgeMoveIntentSnapshot& Intent) override;
	/** 更新移动。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 */
	virtual void UpdateMovement(float DeltaTime) override;
	/** 处理移动FollowUp。
	 * @param Data 当前操作使用的数据。
	 */
	virtual void HandleMovementFollowUp(const FGameplayAbilityTargetDataHandle& Data) override;
	/** 结束本次技能并释放其拥有的资源，再交还GAS生命周期。
	 * @param Handle 对应操作或能力的句柄。
	 * @param Info 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param ActivationInfo 本次技能激活与预测信息。
	 * @param bReplicate 是否Replicate的控制标记。
	 * @param bCancelled 是否已取消的控制标记。
	 */
	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info, FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate, bool bCancelled) override;
private:
	/** 请求掉头。
	 * @param Velocity 速度，由调用方传入。
	 * @param Desired 期望，由调用方传入。
	 */
	void RequestPivot(FVector Velocity, FVector Desired);
	/** 开始本会话的根运动掉头并接入恢复时机。
	 * @param StartingYaw StartingYaw角，由调用方传入。
	 */
	void StartPivot(float StartingYaw);
	/** 更新掉头恢复。 */
	void UpdatePivotRecovery();
	/** 释放掉头任务。 */
	void ReleasePivotTask();
	/** 处理掉头已结束。 */
	UFUNCTION() void OnPivotFinished();
	// 掉头任务的数据或对象引用。
	UPROPERTY(Transient) TObjectPtr<UAbilityTask_PlayMontageAndWait> PivotTask;
	// 控制是否Pivoting的状态标记。
	bool bPivoting = false;
	// 控制是否Ending的状态标记。
	bool bEnding = false;
	// 掉头序号的数值或阈值。
	int32 PivotSequence = 0;
	// 掉头蒙太奇Instance标识的数值或阈值。
	int32 PivotMontageInstanceId = INDEX_NONE;
	// Deferred掉头数据对应操作的配对句柄。
	FGameplayAbilityTargetDataHandle DeferredPivotData;
	// Deferred掉头Expires时刻的数值或阈值。
	double DeferredPivotExpiresAt = 0.;
	// 下次掉头时刻的数值或阈值。
	double NextPivotAt = 0.;
	/** 处理权威结束。 */
	void OnAuthorityEnded();
	/** 处理攻击开始。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 * @param Count 本次处理涉及的数量。
	 */
	void OnAttackStarted(FGameplayTag Tag, int32 Count);
	// 攻击句柄对应的事件通知。
	FDelegateHandle AttackHandle;
	/** 处理伤害。
	 * @param Instigator 发起本次行为的对象。
	 * @param Causer Causer，由调用方传入。
	 * @param Spec 规格，由调用方传入。
	 * @param Magnitude 数值，由调用方传入。
	 * @param Old 旧，由调用方传入。
	 * @param New 新，由调用方传入。
	 */
	void OnDamage(AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* Spec, float Magnitude, float Old, float New);
	// 策略句柄的数据或对象引用。
	FGuid PolicyHandle;
	// 权威句柄对应的事件通知。
	FDelegateHandle AuthorityHandle;
	// 伤害句柄对应的事件通知。
	FDelegateHandle DamageHandle;
	// No输入时刻的数值或阈值。
	double NoInputAt = 0.;
	// 阻塞时刻的数值或阈值。
	double BlockedAt = 0.;
	// 待处理意图的数据或对象引用。
	FHodgeMoveIntentSnapshot PendingIntent;
};
