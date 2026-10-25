/** @file HodgeEncounterSpawner.h
 * @brief 交互与区域入口共用的服务器波次、成员身份、奖励与重入管理。
 */
#pragma once

#include "GameFramework/Actor.h"
#include "Encounter/HodgeEncounterTypes.h"
#include "Interaction/HodgeInteractableTarget.h"
#include "TimerManager.h"
#include "HodgeEncounterSpawner.generated.h"

class AHodgeEnemyCharacter;
class AHodgePlayerController;
class APawn;
class UBoxComponent;
class UGameFrameworkComponentManager;
class UHodgeEncounterDefinition;
class UHodgeEnemyInitComponent;
class UHodgeExperienceManagerComponent;
class UHodgeHealthComponent;
class UStaticMeshComponent;
class UWidgetComponent;

/** 不生成玩家 Bot，不承担敌人属性、AI 或库存的内部初始化。 */
UCLASS(BlueprintType, Blueprintable)
class HODGEPODGE_API AHodgeEncounterSpawner : public AActor, public IHodgeInteractableTarget
{
	GENERATED_BODY()
public:
	AHodgeEncounterSpawner();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_Definition, Category="Hodge|Encounter")
	TObjectPtr<UHodgeEncounterDefinition> Definition;

	/** 服务器启动入口；返回接受启动不表示敌人已经初始化成功。 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|Encounter")
	FHodgeInteractionResult TryStartEncounter(APawn* RequestingPawn);
	/** 显式取消当前轮次，不发奖；区域仍遵循失败后的冷却与离开门禁。 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|Encounter")
	void ResetEncounter();
	UFUNCTION(BlueprintPure, Category="Hodge|Encounter") FHodgeEncounterSnapshot GetSnapshot() const;
	/** 仅服务器登记的存活敌人；客户端使用快照计数，不用该列表本地结算。 */
	UFUNCTION(BlueprintPure, Category="Hodge|Encounter") TArray<AHodgeEnemyCharacter*> GetLivingEnemies() const;
	virtual bool GetInteractionOption(const APawn* RequestingPawn, FHodgeInteractionOption& OutOption) const override;
	virtual FHodgeInteractionResult ExecuteInteraction(APawn* RequestingPawn, const FGuid& ExpectedRunId, int32 ExpectedRevision) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

private:
	friend struct FHodgeEncounterTestAccess;
	/** 成员在 FinishSpawning 前登记；死亡与初始化结果各自只提交一次。 */
	struct FMember
	{
		TWeakObjectPtr<AHodgeEnemyCharacter> Enemy;
		TWeakObjectPtr<UHodgeHealthComponent> Health;
		TWeakObjectPtr<UHodgeEnemyInitComponent> Init;
		FDelegateHandle ReadinessHandle;
		FGuid RunId;
		int32 WaveIndex = INDEX_NONE;
		bool bInitialized = false;
		bool bDeathConfirmed = false;
	};
	/** 首次候选只保留弱身份与可撤销监听，不用轮询等待玩家初始化。 */
	struct FInitialAreaCandidate
	{
		TWeakObjectPtr<APawn> Pawn;
		TWeakObjectPtr<UGameFrameworkComponentManager> InitManager;
		TWeakObjectPtr<AHodgePlayerState> PlayerState;
		TWeakObjectPtr<UHodgeHealthComponent> Health;
		FDelegateHandle InitStateHandle;
		FDelegateHandle AttributeReadyHandle;
		FGuid Identity;
		bool bObservedAvatarBinding = false;
	};

	UPROPERTY(VisibleAnywhere, Category="Hodge|Encounter") TObjectPtr<UBoxComponent> TriggerVolume;
	UPROPERTY(VisibleAnywhere, Category="Hodge|Encounter") TObjectPtr<UStaticMeshComponent> SwitchMesh;
	UPROPERTY(VisibleAnywhere, Category="Hodge|Encounter") TObjectPtr<UWidgetComponent> SwitchPrompt;
	UPROPERTY(ReplicatedUsing=OnRep_Snapshot) FHodgeEncounterSnapshot Snapshot;
	UPROPERTY(Transient) TObjectPtr<UHodgeEncounterDefinition> RunDefinition;
	TMap<TWeakObjectPtr<AHodgeEnemyCharacter>, FMember> Members;
	TSet<TWeakObjectPtr<APawn>> AreaOccupants;
	TSet<TWeakObjectPtr<APawn>> ArmedExitEntrants;
	TMap<TWeakObjectPtr<APawn>, FInitialAreaCandidate> InitialAreaCandidates;
	TWeakObjectPtr<UHodgeExperienceManagerComponent> InitialAreaExperience;
	FDelegateHandle InitialAreaGameStateHandle;
	FGuid InitialAreaEpoch;
	TWeakObjectPtr<AHodgePlayerController> OwnerController;
	TWeakObjectPtr<APawn> OwnerPawn;
	TWeakObjectPtr<UHodgeHealthComponent> OwnerHealth;
	FTimerHandle WaveTimer;
	FTimerHandle InitializationTimer;
	FTimerHandle EvaluationTimer;
	FTimerHandle CooldownTimer;
	bool bSpawning = false;
	bool bEvaluationQueued = false;
	bool bClosing = false;
	bool bSettling = false;
	bool bAreaRetryAfterFailure = false;
	bool bRuntimeReady = false;
	bool bInitialAreaCatchupAvailable = false;
	bool bCheckingInitialAreaCandidates = false;
	// 仅原生夹具可跳过外部 Experience 等待，正式对象始终要求现有体验就绪。
	bool bRequireExperience = true;

	bool ValidatePlayer(const APawn* Pawn, AHodgePlayerController*& OutController, AHodgePlayerState*& OutState, FString& OutError) const;
	bool ValidateInteractionGeometry(const APawn* Pawn, const AHodgePlayerController* Controller) const;
	bool IsExperienceReady() const;
	bool IsRunCurrent(const FGuid& RunId, int32 WaveIndex = INDEX_NONE) const;
	void Publish();
	void ConfigureComponents();
	void ApplyPresentation();
	void ScheduleWave(int32 WaveIndex);
	void SpawnWave(FGuid RunId, int32 WaveIndex);
	void QueueEvaluation();
	void EvaluateWave(FGuid RunId, int32 WaveIndex);
	void HandleInitializationChanged(FGuid RunId, TWeakObjectPtr<AHodgeEnemyCharacter> Enemy);
	void HandleInitializationTimeout(FGuid RunId, int32 WaveIndex);
	void FinishWave();
	void FailEncounter(const FString& Reason);
	void RetireRun(bool bDestroyLivingEnemies);
	void ClearMembers(bool bDestroyLivingEnemies);
	void SetInactive();
	void CompleteArea(bool bFailed = false);
	void RefreshAreaOccupants();
	void TryRearmArea();
	bool CanFirstAreaCatchup() const;
	void InitializeFirstAreaCatchup();
	void RegisterFirstAreaExperience();
	void RegisterFirstAreaCandidate(APawn* Pawn);
	void BindFirstAreaCandidateAttributes(FInitialAreaCandidate& Candidate);
	void HandleFirstAreaCandidateReady(FGuid Epoch, TWeakObjectPtr<APawn> Pawn, FGuid CandidateIdentity);
	void RetryFirstAreaCandidates(FGuid Epoch);
	void RemoveFirstAreaCandidate(TWeakObjectPtr<APawn> Pawn);
	void RetireFirstAreaCatchup();
	void BindOwner(AHodgePlayerController* Controller, APawn* Pawn);
	void BindOwnerPawn(APawn* Pawn);
	void UnbindOwner();

	UFUNCTION() void OnRep_Snapshot();
	UFUNCTION() void OnRep_Definition();
	UFUNCTION() void HandleEnemyDeath(AActor* Actor);
	UFUNCTION() void HandleEnemyEndPlay(AActor* Actor, EEndPlayReason::Type Reason);
	UFUNCTION() void HandleOwnerDeath(AActor* Actor);
	UFUNCTION() void HandleOwnerEndPlay(AActor* Actor, EEndPlayReason::Type Reason);
	UFUNCTION() void HandleOwnerPawnEndPlay(AActor* Actor, EEndPlayReason::Type Reason);
	UFUNCTION() void HandleOwnerPawnChanged(APawn* OldPawn, APawn* NewPawn);
	UFUNCTION() void HandleInitialAreaCandidateDeath(AActor* Actor);
	UFUNCTION() void HandleInitialAreaCandidateEndPlay(AActor* Actor, EEndPlayReason::Type Reason);
	UFUNCTION() void HandleAreaBegin(UPrimitiveComponent* Component, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION() void HandleAreaEnd(UPrimitiveComponent* Component, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);
};
