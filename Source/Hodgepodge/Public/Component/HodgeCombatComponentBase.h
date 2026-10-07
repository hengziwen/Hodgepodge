#pragma once

#include "CoreMinimal.h"
#include "Component/HodgeActorComponentBase.h"
#include "Combat/HodgeHitDetection.h"
#include "GameplayAbilitySpecHandle.h"
#include "HodgeCombatComponentBase.generated.h"

class UHodgeWeaponInstance;
class USceneComponent;
class UHodgeAbilitySystemComponent;
class UHodgeComboDefinition;
class UHodgeGameplayAbility_Definition;
class UHodgePawnExtensionComponent;
struct FGameplayEventData;
struct FHodgeComboTransition;

USTRUCT()
struct FHodgeComboMemoryState
{
	GENERATED_BODY()
	UPROPERTY() FGameplayTag Node;
	UPROPERTY() double ExpiresAt = 0;
	UPROPERTY() int16 ExecutionKey = 0;
};

USTRUCT()
struct FHodgeHitDetectionSession
{
	GENERATED_BODY()
	UPROPERTY() FGuid ExecutionId;
	UPROPERTY() int32 EventIndex = INDEX_NONE;
	UPROPERTY() FHodgeHitDetectionRequest Request;
	UPROPERTY() FHodgeHitSource Source;
	UPROPERTY() TWeakObjectPtr<USceneComponent> SourceComponent;
	UPROPERTY() TWeakObjectPtr<UHodgeWeaponInstance> Weapon;
	bool bWeaponSource = false;
	int32 SampleSequence = 0;
	FHodgeHitGeometry Previous;
	FTransform FixedAnchor = FTransform::Identity;
	bool bHasFixedAnchor = false;
};

/** Pawn 级战斗入口，统一连招协调和几何检测；命中规则与 GE 应用仍由攻击 GA 处理。 */
UCLASS(Blueprintable, ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class HODGEPODGE_API UHodgeCombatComponentBase : public UHodgeActorComponentBase
{
	GENERATED_BODY()
public:
	UHodgeCombatComponentBase();

	// 连招输入、执行授权和网络确认统一通过当前 Pawn 的战斗组件。
	void Configure(UHodgeAbilitySystemComponent* InASC, const UHodgeComboDefinition* InDefinition);
	void Shutdown();
	bool InputPressed(FGameplayTag InputTag);
	void ClearInput();
	void ExecutionStarted(UHodgeGameplayAbility_Definition* Ability);
	void ExecutionEnded(UHodgeGameplayAbility_Definition* Ability);
	void WindowsChanged(UHodgeGameplayAbility_Definition* Ability);
	void TimelineEvent(UHodgeGameplayAbility_Definition* Ability, FGameplayTag Event);
	bool IsAuthorized(FGameplayAbilitySpecHandle Handle) const;
	bool PrepareServerActivation(FGameplayAbilitySpecHandle Handle, const FGameplayEventData* Payload);
	void RejectServerActivation(const FGameplayEventData* Payload);
	bool PrepareConfirmedActivation(FGameplayAbilitySpecHandle Handle, const FGameplayEventData& Payload);
	void CompleteServerActivation();
	void HandlePredictionRejected();
	UFUNCTION(BlueprintPure, Category="Hodge|Combat")
	FGameplayTag GetCurrentComboTag() const { return CurrentComboTag; }
	UFUNCTION(BlueprintPure, Category="Hodge|Combat")
	FGameplayTag GetRememberedComboTag() const;
	UFUNCTION(BlueprintPure, Category="Hodge|Combat")
	float GetComboMemoryRemainingTime() const;

	static UHodgeCombatComponentBase* FindCombatComponent(const AActor* Avatar);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 角色自身的来源配置；武器来源从已装备的 WeaponInstance 读取。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Combat", meta=(TitleProperty="SourceTag"))
	TArray<FHodgeHitSource> HitSources;

	uint64 CreateDetectionSession(const FGuid& ExecutionId, int32 EventIndex, const FHodgeHitDetectionRequest& Request);
	bool SampleDetection(uint64 Handle, const FGuid& ExecutionId, FHodgeHitDetectionBatch& OutBatch);
	bool ResetDetectionHistory(uint64 Handle, const FGuid& ExecutionId);
	bool IsDetectionSessionValid(uint64 Handle, const FGuid& ExecutionId) const;
	USceneComponent* GetDetectionSourceComponent(uint64 Handle, const FGuid& ExecutionId) const;
	void EndDetectionSession(uint64 Handle, const FGuid& ExecutionId);
	void EndDetectionSessionsForExecution(const FGuid& ExecutionId);
	void EndAllDetectionSessions();
	void UpdateDetectionAnchor(const FGuid& ExecutionId, FName Key, const FTransform& Transform);
protected:
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* TickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
private:
	bool CaptureDetectionGeometry(const FHodgeHitDetectionSession& Session, FHodgeHitGeometry& Out) const;
	void BindPawnExtension();
	bool IsComboReady() const;
	void HandleAbilitySystemInitialized();
	void HandleAbilitySystemUninitialized();
	UPROPERTY(Transient) TWeakObjectPtr<UHodgePawnExtensionComponent> PawnExtension;
	FGameplayTagContainer AppliedObserverTags;

	// 连招状态与几何历史各自保存，外部只使用一个组件入口。
	friend struct FHodgeComboTestAccess;

	struct FQueuedEvent
	{
		TWeakObjectPtr<UHodgeGameplayAbility_Definition> Ability;
		int16 Key = 0;
		FGameplayTag Tag;
	};

	void DrainEvents();
	TArray<FQueuedEvent> QueuedEvents;
	bool bDrainingEvents = false;
	const FHodgeComboTransition* SelectTransition(FGameplayTag Trigger, bool bEvent) const;
	bool TryTransition(FGameplayTag Trigger, bool bEvent);
	bool PrepareTransition(const FHodgeComboTransition& Edge, FGameplayAbilitySpecHandle Handle);
	void ResetSession(bool bEndAbility);
	void EndCurrentExecution();
	void RetainComboMemory();
	void ExpireComboMemory();
	void PublishComboMemory();
	bool HasResumeTransition(FGameplayTag Node) const;
	FGameplayTag TransitionSourceNode() const;
	double ComboTime() const;
	void SetNode(FGameplayTag Node);
	int16 ExecutionKey() const;
	UFUNCTION(Server, Reliable)
	void ServerMoveCancel(AActor* Avatar, FGameplayAbilitySpecHandle Handle, int32 Key);
	UFUNCTION(Server, Reliable)
	void ServerReturnToEntry(AActor* Avatar, FGameplayTag SourceNode, int32 Key, FGameplayTag Intent);
	UFUNCTION(Client, Reliable)
	void ClientMoveCancelResult();
	UFUNCTION(Server, Reliable)
	void ServerSynchronizeComboMemory();
	UFUNCTION(Client, Reliable)
	void ClientCorrectComboMemory(FHodgeComboMemoryState State);
	UFUNCTION()
	void OnRep_ComboMemory();
	UFUNCTION()
	void OnRep_ObserverTags(const FGameplayTagContainer& Previous);
	UPROPERTY(Transient)
	TObjectPtr<UHodgeAbilitySystemComponent> ASC;
	UPROPERTY(Transient)
	TObjectPtr<const UHodgeComboDefinition> Definition;
	UPROPERTY(Transient)
	TObjectPtr<UHodgeGameplayAbility_Definition> CurrentAbility;
	UPROPERTY(Transient)
	FGameplayTag CurrentComboTag;
	UPROPERTY(ReplicatedUsing=OnRep_ComboMemory)
	FHodgeComboMemoryState ReplicatedComboMemory;
	FHodgeComboMemoryState ComboMemory;
	FHodgeComboMemoryState PreviousComboMemory;
	UPROPERTY(ReplicatedUsing=OnRep_ObserverTags)
	FGameplayTagContainer ObserverTags;
	FGameplayTagContainer OwnedNodeTags;
	FGameplayTag BufferedInput;
	double InputExpiresAt = 0;
	FGameplayAbilitySpecHandle AuthorizedHandle;
	FGameplayTag PendingNode;
	bool bSwitching = false;
	bool bTransitionStarted = false;
	bool bMemoryCorrectionPending = false;
	bool bMoveRequestPending = false;
	bool bEvaluating = false;
	bool bShuttingDown = false;

	bool ResolveSource(FGameplayTag Tag, FHodgeHitDetectionSession& Session) const;
	bool IsSourceValid(const FHodgeHitDetectionSession& Session) const;
	UPROPERTY(Transient)
	TMap<uint64, FHodgeHitDetectionSession> Sessions;
	uint64 NextHandle = 0;
};
