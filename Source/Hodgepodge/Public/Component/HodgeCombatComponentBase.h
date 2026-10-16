/** @file HodgeCombatComponentBase.h
 * @brief 声明统一战斗组件相关的类型、接口与现有处理流程。
 */
#pragma once

#include "CoreMinimal.h"
#include "Component/HodgeActorComponentBase.h"
#include "Combat/HodgeHitDetection.h"
#include "GameplayAbilitySpecHandle.h"
#include "Combat/HodgeCombatActionRequest.h"
#include "HodgeCombatComponentBase.generated.h"

class UHodgeHitDetectionComponent;
class USceneComponent;
class UHodgeAbilitySystemComponent;
class UHodgeComboDefinition;
class UHodgeGameplayAbility_Definition;
class UHodgePawnExtensionComponent;
struct FGameplayEventData;
struct FAbilityEndedData;
struct FHodgeComboTransition;

/** 保存连段记忆状态的数据和接口契约。 */
USTRUCT()
struct FHodgeComboMemoryState
{
	GENERATED_BODY()
	// 节点的数据或对象引用。
	UPROPERTY() FGameplayTag Node;
	// Expires时刻的数值或阈值。
	UPROPERTY() double ExpiresAt = 0;
	// 执行键的数据或对象引用。
	UPROPERTY() int16 ExecutionKey = 0;
};

/** Pawn 级战斗入口，负责连招与请求；旧检测接口仅代理独立检测组件。 */
UCLASS(Blueprintable, ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class HODGEPODGE_API UHodgeCombatComponentBase : public UHodgeActorComponentBase
{
	GENERATED_BODY()
public:
	/** 构造统一战斗组件并初始化其默认状态。 */
	UHodgeCombatComponentBase();

	// 连招输入、执行授权和网络确认统一通过当前 Pawn 的战斗组件。
	/** 绑定当前执行所用的ASC与配置数据。
	 * @param InASC 后续GAS操作使用的能力系统组件。
	 * @param InDefinition 当前操作使用的定义配置。
	 */
	void Configure(UHodgeAbilitySystemComponent* InASC, const UHodgeComboDefinition* InDefinition);
	/** 停止当前对象的运行绑定并清理其资源。 */
	void Shutdown();
	/** 申请一份动作姿势占用，供结束时按来源释放。
	 * @return 本次计算、查询或创建得到的唯一标识。
	 */
	FGuid AcquirePoseLease();
	/** 释放该动作拥有的姿势占用。
	 * @param Handle 对应操作或能力的句柄。
	 */
	void ReleasePoseLease(FGuid Handle);
	/** 获取姿势占用许可数量。
	 * @return 姿势占用许可数量的当前值或引用。
	 */
	int32 GetPoseLeaseCount() const;
	/** 获取检测会话数量。
	 * @return 检测会话数量的当前值或引用。
	 */
	int32 GetDetectionSessionCount() const;
	/** 检查是否允许执行Abilities。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool CanExecuteAbilities() const { return IsRegistered() && !bShuttingDown; }
	/** 获取连段图配置。
	 * @return 连段图配置的当前值或引用。
	 */
	const UHodgeComboDefinition* GetComboDefinition() const { return Definition; }
	/** 将按下的输入Tag交给现有连段意图映射。
	 * @param InputTag 输入动作对应的GameplayTag。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool InputPressed(FGameplayTag InputTag);
	/** 校验独立技能请求并通过现有ASC执行激活。
	 * @param AbilityTag 查找已授予技能定义的身份Tag。
	 * @return 本次计算、查询或创建得到的战斗动作请求状态。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Combat|Requests")
	FHodgeCombatActionRequestState RequestStandaloneAbility(FGameplayTag AbilityTag);
	/** 提交语义意图，由连段图选择并授权实际动作。
	 * @param IntentTag 连段图消费的语义输入Tag。
	 * @return 本次计算、查询或创建得到的战斗动作请求状态。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Combat|Requests")
	FHodgeCombatActionRequestState SubmitCombatIntent(FGameplayTag IntentTag);
	/** 获取共享战斗请求数据。
	 * @param RequestId 本次请求的唯一身份。
	 * @param OutState 输出查询得到的状态快照。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Combat|Requests")
	bool GetCombatActionRequest(FGuid RequestId, FHodgeCombatActionRequestState& OutState) const;
	/** 按请求身份取消其对应的缓存或技能执行。
	 * @param RequestId 本次请求的唯一身份。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Combat|Requests")
	bool CancelCombatActionRequest(FGuid RequestId);
	/** 检查当前Avatar绑定、生命和战斗初始化依赖。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsCombatReadyForActions() const;
	// 事件动作请求变化对应的事件通知。
	FHodgeCombatActionRequestChanged OnActionRequestChanged;
	// 事件战斗动作请求变化对应的事件通知。
	UPROPERTY(BlueprintAssignable, Category="Hodge|Combat|Requests") FHodgeCombatActionRequestChangedBP OnCombatActionRequestChanged;
	/** 判断移动取消Predicted。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsMoveCancelPredicted() const;
	/** 清空输入。 */
	void ClearInput();
	/** 形态结束只废弃续接与缓冲，不停止当前攻击、R或重新配置Combat。 */
	void InvalidateModeContinuation(FGuid ModeSessionId);
	/** 记录当前动作开始并更新连段执行状态。
	 * @param Ability 能力，由调用方传入。
	 */
	void ExecutionStarted(UHodgeGameplayAbility_Definition* Ability);
	/** 接收当前动作结束结果并收敛连段状态。
	 * @param Ability 能力，由调用方传入。
	 */
	void ExecutionEnded(UHodgeGameplayAbility_Definition* Ability);
	/** 在动画窗口变化时尝试推进连段或取消状态。
	 * @param Ability 能力，由调用方传入。
	 */
	void WindowsChanged(UHodgeGameplayAbility_Definition* Ability);
	/** 接收当前动作发出的语义事件。
	 * @param Ability 能力，由调用方传入。
	 * @param Event 事件，用于对应标签查询或匹配。
	 */
	void ExecutionEvent(UHodgeGameplayAbility_Definition* Ability, FGameplayTag Event);
	/** 判断Authorized。
	 * @param Handle 对应操作或能力的句柄。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsAuthorized(FGameplayAbilitySpecHandle Handle) const;
	/** 获取服务器输入缓存秒。
	 * @return 服务器输入缓存秒的当前值或引用。
	 */
	float GetServerInputBufferSeconds() const;
	/** 检查是否允许缓存服务器Activation。
	 * @param Handle 对应操作或能力的句柄。
	 * @param Payload 动画通知或玩法事件携带的数据。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool CanBufferServerActivation(FGameplayAbilitySpecHandle Handle, const FGameplayEventData* Payload) const;
	/** 准备服务器Activation。
	 * @param Handle 对应操作或能力的句柄。
	 * @param Payload 动画通知或玩法事件携带的数据。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool PrepareServerActivation(FGameplayAbilitySpecHandle Handle, const FGameplayEventData* Payload);
	/** 拒绝当前服务器激活请求并清理对应记录。
	 * @param Payload 动画通知或玩法事件携带的数据。
	 */
	void RejectServerActivation(const FGameplayEventData* Payload);
	/** 准备ConfirmedActivation。
	 * @param Handle 对应操作或能力的句柄。
	 * @param Payload 动画通知或玩法事件携带的数据。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool PrepareConfirmedActivation(FGameplayAbilitySpecHandle Handle, const FGameplayEventData& Payload);
	/** 完成服务器Activation。 */
	void CompleteServerActivation();
	/** 处理预测Rejected。 */
	void HandlePredictionRejected();
	/** 获取当前连段标签。
	 * @return 当前连段标签的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Combat")
	FGameplayTag GetCurrentComboTag() const { return CurrentComboTag; }
	/** 获取Remembered连段标签。
	 * @return Remembered连段标签的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Combat")
	FGameplayTag GetRememberedComboTag() const;
	/** 获取连段记忆Remaining时间。
	 * @return 连段记忆Remaining时间的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Combat")
	float GetComboMemoryRemainingTime() const;

	/** 查找战斗组件。
	 * @param Avatar 表现角色，由调用方传入。
	 * @return 战斗组件的当前值或引用。
	 */
	static UHodgeCombatComponentBase* FindCombatComponent(const AActor* Avatar);
	/** 登记本类型需要网络复制的属性及复制条件。
	 * @param OutLifetimeProps 输出LifetimeProps。
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 角色自身的来源配置；武器来源从已装备的 WeaponInstance 读取。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Combat", meta=(TitleProperty="SourceTag"))
	TArray<FHodgeHitSource> HitSources;

	/** 返回当前唯一检测服务；仅在注册且未关闭时创建。 */
	UHodgeHitDetectionComponent* GetHitDetectionComponent();
	/** 创建检测会话。
	 * @param ExecutionId 本次动作执行的唯一身份。
	 * @param OccurrenceId 通知次数标识，由调用方传入。
	 * @param Request 请求，由调用方传入。
	 * @return 本次计算、查询或创建得到的uint64。
	 */
	uint64 CreateDetectionSession(const FGuid& ExecutionId, int32 OccurrenceId, const FHodgeHitDetectionRequest& Request);
	/** 按当前检测配置采样几何与目标。
	 * @param Handle 对应操作或能力的句柄。
	 * @param ExecutionId 本次动作执行的唯一身份。
	 * @param OutBatch 输出Batch。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool SampleDetection(uint64 Handle, const FGuid& ExecutionId, FHodgeHitDetectionBatch& OutBatch);
	/** 重置检测历史。
	 * @param Handle 对应操作或能力的句柄。
	 * @param ExecutionId 本次动作执行的唯一身份。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool ResetDetectionHistory(uint64 Handle, const FGuid& ExecutionId);
	/** 判断检测会话有效。
	 * @param Handle 对应操作或能力的句柄。
	 * @param ExecutionId 本次动作执行的唯一身份。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsDetectionSessionValid(uint64 Handle, const FGuid& ExecutionId) const;
	/** 获取检测来源组件。
	 * @param Handle 对应操作或能力的句柄。
	 * @param ExecutionId 本次动作执行的唯一身份。
	 * @return 检测来源组件的当前值或引用。
	 */
	USceneComponent* GetDetectionSourceComponent(uint64 Handle, const FGuid& ExecutionId) const;
	/** 结束检测会话。
	 * @param Handle 对应操作或能力的句柄。
	 * @param ExecutionId 本次动作执行的唯一身份。
	 */
	void EndDetectionSession(uint64 Handle, const FGuid& ExecutionId);
	/** 结束检测Sessions关联执行。
	 * @param ExecutionId 本次动作执行的唯一身份。
	 */
	void EndDetectionSessionsForExecution(const FGuid& ExecutionId);
	/** 结束All检测Sessions。 */
	void EndAllDetectionSessions();
	/** 更新检测锚点。
	 * @param ExecutionId 本次动作执行的唯一身份。
	 * @param Key 键，由调用方传入。
	 * @param Transform 变换，由调用方传入。
	 */
	void UpdateDetectionAnchor(const FGuid& ExecutionId, FName Key, const FTransform& Transform);
protected:
	/** 组件注册后接入当前Owner的依赖。 */
	virtual void OnRegister() override;
	/** 组件注销时解除依赖并清理本组件资源。 */
	virtual void OnUnregister() override;
	/** 进入世界运行阶段并接入本对象的生命周期依赖。 */
	virtual void BeginPlay() override;
	/** 更新组件。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 * @param TickType 帧更新类型，由调用方传入。
	 * @param TickFunction 帧更新Function，由调用方传入。
	 */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* TickFunction) override;
	/** 离开世界运行阶段并解除本对象持有的监听或资源。
	 * @param Reason Reason，由调用方传入。
	 */
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
private:
	friend struct FHodgeCombatActionTestAccess;
	/** 保存动作请求记录的数据和接口契约。 */
	struct FActionRequestRecord
	{
		// 当前状态数据。
		FHodgeCombatActionRequestState State;
		// 能力的弱引用，不延长目标对象生命周期。
		TWeakObjectPtr<UHodgeGameplayAbility_Definition> Ability;
		// 控制是否Activating的状态标记。
		bool bActivating = false;
	};
	/** 添加动作请求。
	 * @param Type 类型，由调用方传入。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 * @return 本次计算、查询或创建得到的战斗动作请求状态。
	 */
	FHodgeCombatActionRequestState AddActionRequest(EHodgeCombatActionRequestType Type, FGameplayTag Tag);
	/** 更新请求状态，并按队列顺序分发原生和蓝图通知。
	 * @param Id 当前记录的唯一标识。
	 * @param Status 状态，由调用方传入。
	 * @param Reason Reason，由调用方传入。
	 */
	void SetActionRequestStatus(FGuid Id, EHodgeCombatActionRequestStatus Status, const FString& Reason = FString());
	/** 将匹配的技能结束结果写入对应战斗请求。
	 * @param Data 当前操作使用的数据。
	 */
	void HandleActionAbilityEnded(const FAbilityEndedData& Data);
	/** 绑定动作请求。
	 * @param Id 当前记录的唯一标识。
	 * @param Handle 对应操作或能力的句柄。
	 */
	void BindActionRequest(FGuid Id, FGameplayAbilitySpecHandle Handle);
	/** 根据实际GAS激活结果提交请求的执行状态。
	 * @param Id 当前记录的唯一标识。
	 * @param bActivated 本次尝试是否已经启动技能。
	 */
	void ConfirmActionRequest(FGuid Id, bool bActivated);
	/** 消费已缓存输入。 */
	void ConsumeBufferedInput();
	/** 关闭动作Requests。 */
	void CloseActionRequests();
	// 当前保留的战斗请求记录。
	TMap<FGuid, FActionRequestRecord> ActionRequests;
	// 请求历史的保存与裁剪顺序。
	TArray<FGuid> ActionRequestOrder;
	// 当前连段缓存对应的请求身份。
	FGuid BufferedActionRequest;
	// ASC技能结束监听的解绑句柄。
	FDelegateHandle ActionEndedHandle;
	// 正在提交请求，防止嵌套提交。
	bool bSubmittingAction = false;
	// 正在分发状态通知，保持重入事件顺序。
	bool bNotifyingActions = false;
	// 等待按顺序分发的请求状态快照。
	TArray<FHodgeCombatActionRequestState> PendingActionNotifications;
	/** 兼容入口的本地检测服务，状态只在服务中保存。 */
	UPROPERTY(Transient) TObjectPtr<UHodgeHitDetectionComponent> HitDetection;
	/** 绑定Pawn角色扩展。 */
	void BindPawnExtension();
	/** 判断连段就绪。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsComboReady() const;
	/** 处理能力SystemInitialized。 */
	void HandleAbilitySystemInitialized();
	/** 处理能力SystemUninitialized。 */
	void HandleAbilitySystemUninitialized();
	// Pawn角色扩展的弱引用，不延长目标对象生命周期。
	UPROPERTY(Transient) TWeakObjectPtr<UHodgePawnExtensionComponent> PawnExtension;
	// 已应用观察者标签的数据或对象引用。
	FGameplayTagContainer AppliedObserverTags;

	// 连招状态与几何历史各自保存，外部只使用一个组件入口。
	friend struct FHodgeComboTestAccess;

	/** 保存Queued事件的数据和接口契约。 */
	struct FQueuedEvent
	{
		// 能力的弱引用，不延长目标对象生命周期。
		TWeakObjectPtr<UHodgeGameplayAbility_Definition> Ability;
		// 键的数据或对象引用。
		int16 Key = 0;
		// 标签的数据或对象引用。
		FGameplayTag Tag;
	};

	/** 按队列顺序消费当前待处理事件。 */
	void DrainEvents();
	// QueuedEvents的记录集合。
	TArray<FQueuedEvent> QueuedEvents;
	// 控制是否DrainingEvents的状态标记。
	bool bDrainingEvents = false;
	/** 按当前意图与窗口条件选择连段转换边。
	 * @param Trigger Trigger，用于对应标签查询或匹配。
	 * @param bEvent 是否事件的控制标记。
	 * @return 本次计算、查询或创建得到的连段转换。
	 */
	const FHodgeComboTransition* SelectTransition(FGameplayTag Trigger, bool bEvent) const;
	/** 尝试转换。
	 * @param Trigger Trigger，用于对应标签查询或匹配。
	 * @param bEvent 是否事件的控制标记。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool TryTransition(FGameplayTag Trigger, bool bEvent);
	/** 准备转换。
	 * @param Edge 边缘，由调用方传入。
	 * @param Handle 对应操作或能力的句柄。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool PrepareTransition(const FHodgeComboTransition& Edge, FGameplayAbilitySpecHandle Handle);
	/** 重置会话。
	 * @param bEndAbility 是否结束能力的控制标记。
	 */
	void ResetSession(bool bEndAbility);
	/** 结束当前执行。 */
	void EndCurrentExecution();
	/** 记录当前允许保留的连段节点与有效时间。 */
	void RetainComboMemory();
	/** 按现有规则清除已经过期的连段记忆。 */
	void ExpireComboMemory();
	/** 发布当前连段记忆及其修订信息。 */
	void PublishComboMemory();
	/** 判断是否存在恢复转换。
	 * @param Node 节点，用于对应标签查询或匹配。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool HasResumeTransition(FGameplayTag Node) const;
	/** 取得本次连段转换使用的源节点。
	 * @return 本次计算、查询或创建得到的玩法标签。
	 */
	FGameplayTag TransitionSourceNode() const;
	/** 读取当前连段计时基准。
	 * @return 本次计算、查询或创建得到的double。
	 */
	double ComboTime() const;
	/** 设置节点。
	 * @param Node 节点，用于对应标签查询或匹配。
	 */
	void SetNode(FGameplayTag Node);
	/** 读取当前动作与网络确认使用的激活键。
	 * @return 本次计算、查询或创建得到的整数16。
	 */
	int16 ExecutionKey() const;
	/** 查询 ASC 实际播放的活动 Definition 实例，不占用连段 CurrentAbility。 */
	UHodgeGameplayAbility_Definition* GetMoveCancelAbility() const;
	/** 根据执行路由取取消窗口及输入阈值，返回 false 表示未配置或不合法。 */
	bool GetMoveCancelPolicy(const UHodgeGameplayAbility_Definition* Ability, FGameplayTag& Window, float& Threshold) const;
	/** 独立检查持续移动输入，支持窗口开启前已按住移动。 */
	void UpdateMoveCancel();
	/** 清除当前请求身份，不结束能力或修改技能冷却。 */
	void ClearMoveCancelRequest();
	/** 处理服务器侧的移动取消消息，沿用当前身份与权限校验。
	 * @param Avatar 表现角色，由调用方传入。
	 * @param Handle 对应操作或能力的句柄。
	 * @param Key 键，由调用方传入。
	 * @param Intent 意图，由调用方传入。
	 */
	UFUNCTION(Server, Reliable)
	void ServerMoveCancel(AActor* Avatar, FGameplayAbilitySpecHandle Handle, int32 Key, FGuid RequestId, FVector2D Intent);
	/** 处理服务器侧的返回目标条目消息，沿用当前身份与权限校验。
	 * @param Avatar 表现角色，由调用方传入。
	 * @param SourceNode 来源节点，用于对应标签查询或匹配。
	 * @param Key 键，由调用方传入。
	 * @param Intent 意图，用于对应标签查询或匹配。
	 */
	UFUNCTION(Server, Reliable)
	void ServerReturnToEntry(AActor* Avatar, FGameplayTag SourceNode, int32 Key, FGameplayTag Intent);
	/** 处理客户端侧的移动取消结果消息，沿用当前身份与权限校验。
	 * @param Avatar 表现角色，由调用方传入。
	 * @param Handle 对应操作或能力的句柄。
	 * @param Key 键，由调用方传入。
	 * @param bEnd 是否结束的控制标记。
	 * @param ServerPosition 服务器位置，由调用方传入。
	 * @param ServerTime 服务器时间，由调用方传入。
	 * @param ServerPlayRate 服务器Play速率，由调用方传入。
	 */
	UFUNCTION(Client, Reliable)
	void ClientMoveCancelResult(AActor* Avatar, FGameplayAbilitySpecHandle Handle, int32 Key, FGuid RequestId, bool bEnd,
		float ServerPosition, float ServerTime, float ServerPlayRate);
	/** 校验服务器请求身份。
	 * @param Payload 动画通知或玩法事件携带的数据。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool ValidateServerRequestIdentity(const FGameplayEventData* Payload) const;
	/** 处理服务器移动取消。
	 * @param Avatar 表现角色，由调用方传入。
	 * @param Handle 对应操作或能力的句柄。
	 * @param Key 键，由调用方传入。
	 * @param Intent 意图，由调用方传入。
	 */
	void ProcessServerMoveCancel(TWeakObjectPtr<AActor> Avatar, FGameplayAbilitySpecHandle Handle, int32 Key,
		FGuid RequestId, FGuid ServerExecution, FVector2D Intent);
	/** 处理服务器侧的同步连段记忆消息，沿用当前身份与权限校验。 */
	UFUNCTION(Server, Reliable)
	void ServerSynchronizeComboMemory();
	/** 处理客户端侧的Correct连段记忆消息，沿用当前身份与权限校验。
	 * @param State 当前状态数据。
	 */
	UFUNCTION(Client, Reliable)
	void ClientCorrectComboMemory(FHodgeComboMemoryState State);
	/** 收到连段记忆复制后更新本地状态。 */
	UFUNCTION()
	void OnRep_ComboMemory();
	/** 收到观察者标签复制后更新本地状态。
	 * @param Previous 更新前的状态。
	 */
	UFUNCTION()
	void OnRep_ObserverTags(const FGameplayTagContainer& Previous);
	// 后续GAS操作使用的能力系统组件。
	UPROPERTY(Transient)
	TObjectPtr<UHodgeAbilitySystemComponent> ASC;
	// 当前操作使用的定义配置。
	UPROPERTY(Transient)
	TObjectPtr<const UHodgeComboDefinition> Definition;
	// 当前能力的数据或对象引用。
	UPROPERTY(Transient)
	TObjectPtr<UHodgeGameplayAbility_Definition> CurrentAbility;
	// 当前连段标签的数据或对象引用。
	UPROPERTY(Transient)
	FGameplayTag CurrentComboTag;
	// 复制连段记忆的数据或对象引用。
	UPROPERTY(ReplicatedUsing=OnRep_ComboMemory)
	FHodgeComboMemoryState ReplicatedComboMemory;
	// 连段记忆的数据或对象引用。
	FHodgeComboMemoryState ComboMemory;
	// 前次连段记忆的数据或对象引用。
	FHodgeComboMemoryState PreviousComboMemory;
	/** 形态变化后当前刀允许结束，但不再保留其旧形态续接。 */
	bool bModeContinuationInvalidated=false;
	/** 服务器与拥有者共同监听真实状态标签，客户端不运行服务器规则被动。 */
	FDelegateHandle CombatModeChangedHandle;
	void HandleCombatModeChanged(FGameplayTag Tag,int32 Count);
	void ClearModeContinuation();
	// 观察者标签的数据或对象引用。
	UPROPERTY(ReplicatedUsing=OnRep_ObserverTags)
	FGameplayTagContainer ObserverTags;
	// Owned节点标签的数据或对象引用。
	FGameplayTagContainer OwnedNodeTags;
	// 已缓存输入的数据或对象引用。
	FGameplayTag BufferedInput;
	// 输入Expires时刻的数值或阈值。
	double InputExpiresAt = 0;
	// Authorized句柄对应操作的配对句柄。
	FGameplayAbilitySpecHandle AuthorizedHandle;
	// 待处理节点的数据或对象引用。
	FGameplayTag PendingNode;
	// 控制是否Switching的状态标记。
	bool bSwitching = false;
	// 控制是否转换开始的状态标记。
	bool bTransitionStarted = false;
	// 控制是否记忆Correction待处理的状态标记。
	bool bMemoryCorrectionPending = false;
	// 控制是否移动请求待处理的状态标记。
	bool bMoveRequestPending = false;
	// Predicted移动取消能力的弱引用，不延长目标对象生命周期。
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeGameplayAbility_Definition> PredictedMoveCancelAbility;
	// 待处理移动句柄对应操作的配对句柄。
	FGameplayAbilitySpecHandle PendingMoveHandle;
	// 待处理移动键的数值或阈值。
	int32 PendingMoveKey = 0;
	// 客户端请求唯一身份随 RPC 回传，旧回执不能确认新请求。
	FGuid PendingMoveRequest;
	// 仅本地使用的执行身份；服务器和客户端的 ExecutionId 不是同一个 GUID。
	FGuid PendingMoveExecution;
	// Rejected移动句柄对应操作的配对句柄。
	FGameplayAbilitySpecHandle RejectedMoveHandle;
	// Rejected移动键的数值或阈值。
	int32 RejectedMoveKey = 0;
	// 拒绝仅抑制本次执行的持续输入，松键或换执行后允许重试。
	FGuid RejectedMoveExecution;
	// 控制是否Evaluating的状态标记。
	bool bEvaluating = false;
	// 正在解绑或关闭，阻止重入执行。
	bool bShuttingDown = false;

};
