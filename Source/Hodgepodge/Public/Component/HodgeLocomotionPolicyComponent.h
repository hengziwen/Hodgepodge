/** @file HodgeLocomotionPolicyComponent.h
 * @brief 声明移动表现策略组件相关的类型、接口与现有处理流程。
 */
#pragma once

#include "Components/PawnComponent.h"
#include "Combat/HodgeMovementActionTypes.h"
#include "GameplayEffectTypes.h"
#include "HodgeLocomotionPolicyComponent.generated.h"

class UHodgeAbilitySystemComponent;
class UHodgeSprintAbilityProfile;
class UGameplayAbility;
DECLARE_MULTICAST_DELEGATE(FHodgeSprintAuthorityEnded);

/** 只拥有速度和步态，朝向仍由 RotationComponent 仲裁。 */
UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class HODGEPODGE_API UHodgeLocomotionPolicyComponent : public UPawnComponent
{
	GENERATED_BODY()
public:
	/** 构造移动表现策略组件并初始化其默认状态。
	 * @param Initializer UE构造对象时传入的初始化器。
	 */
	UHodgeLocomotionPolicyComponent(const FObjectInitializer& Initializer = FObjectInitializer::Get());
	friend struct FHodgeMovementTestAccess;
	/** 初始化关联能力System。
	 * @param InASC 后续GAS操作使用的能力系统组件。
	 */
	void InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC);
	/** 解除初始化来源能力System。 */
	void UninitializeFromAbilitySystem();
	/** 申请当前冲刺移动策略的会话占用。
	 * @param Source 本次操作的来源。
	 * @param Profile 当前操作使用的配置对象。
	 * @param ActivationKey 本次激活的关联键。
	 * @return 本次计算、查询或创建得到的唯一标识。
	 */
	FGuid AcquireSprint(UGameplayAbility* Source, const UHodgeSprintAbilityProfile* Profile, int32 ActivationKey);
	/** 释放策略。
	 * @param Handle 对应操作或能力的句柄。
	 */
	void ReleasePolicy(FGuid Handle);
	/** 按来源登记移动速度修正，避免覆盖其他来源。
	 * @param Source 本次操作的来源。
	 * @param Multiplier Multiplier，由调用方传入。
	 * @return 本次计算、查询或创建得到的唯一标识。
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) FGuid AcquireSpeedModifier(UObject* Source, float Multiplier);
	/** 释放速度Modifier。
	 * @param Handle 对应操作或能力的句柄。
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void ReleaseSpeedModifier(FGuid Handle);
	/** 设置移动回放状态。
	 * @param State 当前状态数据。
	 */
	void SetMoveReplayState(const FHodgeLocomotionState& State);
	/** 清空移动回放状态。 */
	void ClearMoveReplayState();
	/** 开始服务器移动。
	 * @param ActivationKey 本次激活的关联键。
	 */
	void BeginServerMove(int32 ActivationKey);
	/** 结束服务器移动。 */
	void EndServerMove();
	/** 获取Resolved策略。
	 * @return Resolved策略的当前值或引用。
	 */
	UFUNCTION(BlueprintPure) FHodgeLocomotionState GetResolvedPolicy() const;
	/** 获取配置。
	 * @return 配置的当前值或引用。
	 */
	UFUNCTION(BlueprintPure) const UHodgeSprintAbilityProfile* GetProfile() const;
	/** 设置Sprint奔跑输入。
	 * @param SessionId 本次会话的唯一身份。
	 * @param bHeld 是否按住的控制标记。
	 */
	UFUNCTION(BlueprintCallable) void SetSprintInput(int32 SessionId, bool bHeld);
	/** 判断会话按住。
	 * @param SessionId 本次会话的唯一身份。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsSessionHeld(int32 SessionId) const;
	/** 计算当前输入会话的有效按住时间。
	 * @param SessionId 本次会话的唯一身份。
	 * @return 本次计算、查询或创建得到的浮点值。
	 */
	float SessionHeldTime(int32 SessionId) const;
	/** 记录满足条件的动作交接授权。
	 * @param DashKey Dash冲刺键，由调用方传入。
	 * @param SessionId 本次会话的唯一身份。
	 * @param ValidFor 有效关联，由调用方传入。
	 */
	void AuthorizeHandoff(int32 DashKey, int32 SessionId, float ValidFor);
	/** 检查是否允许Handoff。
	 * @param SessionId 本次会话的唯一身份。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool CanHandoff(int32 SessionId) const;
	/** 消费Handoff。 */
	void ConsumeHandoff();
	/** 记录当前Dash会话结束时的速度恢复策略。
	 * @param Source 本次操作的来源。
	 * @param MaximumSpeed 最大速度，由调用方传入。
	 */
	void RecordDashExitVelocity(FName Source, float MaximumSpeed);
	/** 登记活动 Dash 的收尾策略；地面保持原停止语义，空中保留下落速度。 */
	void RecordDashMotionVelocity(FName Source, float MaximumSpeed, float MotionDuration);
	/** 读取本次来源在当前物理模式下的收尾策略，返回 false 表示记录无效。 */
	bool GetDashFinishVelocity(FName Source, bool bFalling, float& OutMaximumSpeed, bool& bOutClamp) const;
	/** 获取Dash冲刺退出速度。
	 * @param Source 本次操作的来源。
	 * @param OutMaximumSpeed 输出最大速度。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool GetDashExitVelocity(FName Source, float& OutMaximumSpeed) const;
	// 事件Sprint奔跑权威结束对应的事件通知。
	FHodgeSprintAuthorityEnded OnSprintAuthorityEnded;
	/** 更新组件。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 * @param Type 类型，由调用方传入。
	 * @param Function Function，由调用方传入。
	 */
	virtual void TickComponent(float DeltaTime, ELevelTick Type, FActorComponentTickFunction* Function) override;
	/** 登记本类型需要网络复制的属性及复制条件。
	 * @param Out 输出输出。
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& Out) const override;
	/** 离开世界运行阶段并解除本对象持有的监听或资源。
	 * @param Reason Reason，由调用方传入。
	 */
	virtual void EndPlay(EEndPlayReason::Type Reason) override;
	// Override配置的数据或对象引用。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UHodgeSprintAbilityProfile> OverrideProfile;
private:
	/** 保存Dash冲刺退出速度的数据和接口契约。 */
	struct FDashExitVelocity
	{
		float MaximumSpeed = 0.f; // 水平退出速度上限，单位 cm/s。
		double ExpiresAt = 0.; // 保存帧回放允许使用记录的截止时间。
		bool bAirborneOnly = false; // 活动 Dash 只在 Falling 限速，正常移动交接始终限速。
	};
	// Dash冲刺退出Velocities的记录集合。
	TMap<FName, FDashExitVelocity> DashExitVelocities;
	/** 处理服务器侧的设置Sprint奔跑输入消息，沿用当前身份与权限校验。
	 * @param Avatar 表现角色，由调用方传入。
	 * @param SessionId 本次会话的唯一身份。
	 * @param bHeld 是否按住的控制标记。
	 */
	UFUNCTION(Server, Reliable) void ServerSetSprintInput(AActor* Avatar, int32 SessionId, bool bHeld);
	/** 收到状态复制后更新本地状态。 */
	UFUNCTION() void OnRep_State();
	/** 设置输入内部。
	 * @param SessionId 本次会话的唯一身份。
	 * @param bHeld 是否按住的控制标记。
	 */
	void SetInputInternal(int32 SessionId, bool bHeld);
	// 后续GAS操作使用的能力系统组件。
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeAbilitySystemComponent> ASC;
	// Sprint奔跑来源的弱引用，不延长目标对象生命周期。
	UPROPERTY(Transient) TWeakObjectPtr<UGameplayAbility> SprintSource;
	// Sprint奔跑配置的数据或对象引用。
	UPROPERTY(Transient) TObjectPtr<const UHodgeSprintAbilityProfile> SprintProfile;
	// 当前状态数据。
	UPROPERTY(ReplicatedUsing=OnRep_State) FHodgeLocomotionState State;
	// 回放状态的数据或对象引用。
	FHodgeLocomotionState ReplayState;
	// Predicted状态的数据或对象引用。
	FHodgeLocomotionState PredictedState;
	// 控制是否回放的状态标记。
	bool bReplay = false;
	// 控制是否服务器Override的状态标记。
	bool bServerOverride = false;
	// Sprint奔跑句柄的数据或对象引用。
	FGuid SprintHandle;
	/** 保存速度Modifier的数据和接口契约。 */
	struct FSpeedModifier { /* 本次操作的来源。 */ TWeakObjectPtr<UObject> Source; /* Multiplier的数值或阈值。 */ float Multiplier = 1.f; };
	// 速度Modifiers的记录集合。
	TMap<FGuid, FSpeedModifier> SpeedModifiers;
	/** 刷新速度缩放。 */
	void RefreshSpeedScale();
	// 输入会话标识的数值或阈值。
	int32 InputSessionId = 0;
	// 控制是否输入按住的状态标记。
	bool bInputHeld = false;
	// 输入按下时刻的数值或阈值。
	double InputPressedAt = 0.;
	// Handoff会话标识的数值或阈值。
	int32 HandoffSessionId = 0;
	// HandoffExpires时刻的数值或阈值。
	double HandoffExpiresAt = 0.;
};
