# AI 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeBTTask_ExecuteAbility.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AI/HodgeBTTask_ExecuteAbility.cpp](../../../Source/Hodgepodge/Private/AI/HodgeBTTask_ExecuteAbility.cpp)

项目内直接 include（不是运行调用关系）：[AI/HodgeBTTask_ExecuteAbility.h](../../../Source/Hodgepodge/Public/AI/HodgeBTTask_ExecuteAbility.h)、[AI/HodgeEnemyAIController.h](../../../Source/Hodgepodge/Public/AI/HodgeEnemyAIController.h)、[Component/HodgeEnemyInitComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeEnemyInitComponent.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)、[Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)

定义候选（多行签名仅展示首行）：

- L21: `UHodgeBTTask_ExecuteAbility::UHodgeBTTask_ExecuteAbility()`
- L30: `void UHodgeBTTask_ExecuteAbility::InitializeFromAsset(UBehaviorTree& Asset)`
- L50: `bool UHodgeBTTask_ExecuteAbility::IsCurrent() const`
- L57: `EBTNodeResult::Type UHodgeBTTask_ExecuteAbility::ExecuteTask(UBehaviorTreeComponent& Owner, uint8*)`
- L89: `void UHodgeBTTask_ExecuteAbility::TickTask(UBehaviorTreeComponent& Owner, uint8*, float DeltaSeconds)`
- L112: `void UHodgeBTTask_ExecuteAbility::HandleRequestChanged(uint32 Attempt, const FHodgeCombatActionRequestState& State)`
- L124: `void UHodgeBTTask_ExecuteAbility::Cleanup(bool bCancel)`
- L137: `EBTNodeResult::Type UHodgeBTTask_ExecuteAbility::AbortTask(UBehaviorTreeComponent&, uint8*)`
- L141: `void UHodgeBTTask_ExecuteAbility::OnTaskFinished(UBehaviorTreeComponent& Owner, uint8* Memory, EBTNodeResult::Type Result)`

## HodgeEnemyAIController.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/AI/HodgeEnemyAIController.cpp](../../../Source/Hodgepodge/Private/AI/HodgeEnemyAIController.cpp)

项目内直接 include（不是运行调用关系）：[AI/HodgeEnemyAIController.h](../../../Source/Hodgepodge/Public/AI/HodgeEnemyAIController.h)、[Character/HodgeEnemyCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeEnemyCharacter.h)、[Character/HodgeHeroCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeHeroCharacter.h)、[Component/HodgeEnemyInitComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeEnemyInitComponent.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[Combat/HodgeDamageRules.h](../../../Source/Hodgepodge/Public/Combat/HodgeDamageRules.h)、[Combat/HodgeHitReactionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitReactionTypes.h)

定义候选（多行签名仅展示首行）：

- L25: `AHodgeEnemyAIController::AHodgeEnemyAIController(const FObjectInitializer& Initializer) : Super(Initializer)`
- L41: `void AHodgeEnemyAIController::PreInitializeComponents()`
- L48: `void AHodgeEnemyAIController::BeginPlay()`
- L55: `void AHodgeEnemyAIController::OnPossess(APawn* ControlledPawn)`
- L72: `bool AHodgeEnemyAIController::IsPossessCurrent(const APawn* ControlledPawn, uint32 Generation) const`
- L76: `void AHodgeEnemyAIController::HandleReadiness()`
- L85: `bool AHodgeEnemyAIController::TryStartBrain()`
- L121: `bool AHodgeEnemyAIController::IsValidEnemyTarget(const AActor* Actor) const`
- L129: `void AHodgeEnemyAIController::HandlePerception(AActor* Actor, FAIStimulus Stimulus)`
- L140: `void AHodgeEnemyAIController::UpdateDecision()`
- L180: `void AHodgeEnemyAIController::HandleConstraint(FGameplayTag Tag, int32 Count)`
- L187: `void AHodgeEnemyAIController::StopCycle(const FString& Reason)`
- L204: `void AHodgeEnemyAIController::OnUnPossess() { StopCycle(TEXT("UnPossess")); Super::OnUnPossess(); }`
- L206: `void AHodgeEnemyAIController::EndPlay(EEndPlayReason::Type Reason)`

## HodgeBTTask_ExecuteAbility.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AI/HodgeBTTask_ExecuteAbility.h](../../../Source/Hodgepodge/Public/AI/HodgeBTTask_ExecuteAbility.h)

项目内直接 include（不是运行调用关系）：[Combat/HodgeCharacterFacingTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeCharacterFacingTypes.h)、[Combat/HodgeCombatActionRequest.h](../../../Source/Hodgepodge/Public/Combat/HodgeCombatActionRequest.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "BehaviorTree/BTTaskNode.h"
   7: #include "GameplayAbilitySpecHandle.h"
   8: #include "GameplayTagContainer.h"
   9: #include "Combat/HodgeCharacterFacingTypes.h"
  10: #include "Combat/HodgeCombatActionRequest.h"
  11: #include "HodgeBTTask_ExecuteAbility.generated.h"
  13: class UHodgeAbilitySystemComponent;
  14: class AHodgeEnemyAIController;
  15: class UHodgeGameplayAbility_Definition;
  16: class UHodgeCombatComponentBase;
  19: UCLASS()
  20: class HODGEPODGE_API UHodgeBTTask_ExecuteAbility : public UBTTaskNode
  21: {
  22: 	GENERATED_BODY()
  23: public:
  25: 	UHodgeBTTask_ExecuteAbility();
  29: 	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
  33: 	bool HasValidTargetKey() const { return bTargetKeyValid; }
  35: 	UPROPERTY(EditAnywhere, Category="Hodge|Ability") EHodgeCombatActionRequestType RequestType = EHodgeCombatActionRequestType::StandaloneAbility;
  37: 	UPROPERTY(EditAnywhere, Category="Hodge|Ability", meta=(EditCondition="RequestType == EHodgeCombatActionRequestType::StandaloneAbility")) FGameplayTag AbilityTag;
  39: 	UPROPERTY(EditAnywhere, Category="Hodge|Ability", meta=(EditCondition="RequestType == EHodgeCombatActionRequestType::ComboIntent")) FGameplayTag IntentTag;
  41: 	UPROPERTY(EditAnywhere, Category="Hodge|Ability") FBlackboardKeySelector TargetActor;
  43: 	UPROPERTY(EditAnywhere, Category="Hodge|Ability", meta=(ClampMin="0.1")) float Timeout = 8.f;
  44: protected:
  50: 	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& Owner, uint8* Memory) override;
  56: 	virtual void TickTask(UBehaviorTreeComponent& Owner, uint8* Memory, float DeltaSeconds) override;
  62: 	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& Owner, uint8* Memory) override;
  68: 	virtual void OnTaskFinished(UBehaviorTreeComponent& Owner, uint8* Memory, EBTNodeResult::Type Result) override;
  69: private:
  70: 	friend struct FHodgeEnemyAITestAccess;
  75: 	void HandleRequestChanged(uint32 Attempt, const FHodgeCombatActionRequestState& State);
  79: 	void Cleanup(bool bCancel);
  83: 	bool IsCurrent() const;
  85: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgeAbilitySystemComponent> ASC;
  87: 	UPROPERTY(Transient) TWeakObjectPtr<AHodgeEnemyAIController> Controller;
  89: 	UPROPERTY(Transient) TWeakObjectPtr<APawn> Pawn;
  91: 	UPROPERTY(Transient) TWeakObjectPtr<UBehaviorTreeComponent> Tree;
  93: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgeCombatComponentBase> Combat;
  95: 	FDelegateHandle RequestChangedHandle;
  97: 	FHodgeFacingRequestHandle Facing;
  99: 	FGuid RequestId;
 101: 	uint32 AttemptId = 0;
 103: 	uint32 PossessGeneration = 0;
 105: 	float Elapsed = 0.f;
 107: 	bool bActivated = false;
 109: 	bool bEnded = false;
 111: 	bool bSubmitting = false;
 113: 	bool bTargetKeyValid = false;
 114: };
```

## HodgeEnemyAIController.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/AI/HodgeEnemyAIController.h](../../../Source/Hodgepodge/Public/AI/HodgeEnemyAIController.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "AIController.h"
   7: #include "GameplayTagContainer.h"
   8: #include "Perception/AIPerceptionTypes.h"
   9: #include "HodgeEnemyAIController.generated.h"
  11: class UBehaviorTree;
  12: class UAIPerceptionComponent;
  13: class UHodgeEnemyInitComponent;
  14: class UHodgeAbilitySystemComponent;
  17: UENUM(BlueprintType)
  18: enum class EHodgeEnemyBrainState : uint8 {               Waiting,                 Starting,                 Running,                Failed,                 Stopped };
  21: UCLASS(Blueprintable)
  22: class HODGEPODGE_API AHodgeEnemyAIController : public AAIController
  23: {
  24: 	GENERATED_BODY()
  25: public:
  29: 	AHodgeEnemyAIController(const FObjectInitializer& Initializer = FObjectInitializer::Get());
  33: 	UFUNCTION(BlueprintCallable, Category="Hodge|Enemy") bool TryStartBrain();
  37: 	UFUNCTION(BlueprintPure, Category="Hodge|Enemy") EHodgeEnemyBrainState GetEnemyBrainState() const { return State; }
  41: 	UFUNCTION(BlueprintPure, Category="Hodge|Enemy") int32 GetBrainStartCount() const { return StartCount; }
  45: 	uint32 GetPossessGeneration() const { return PossessGeneration; }
  51: 	bool IsPossessCurrent(const APawn* ControlledPawn, uint32 Generation) const;
  56: 	bool IsValidEnemyTarget(const AActor* Target) const;
  58: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Enemy") TObjectPtr<UBehaviorTree> BehaviorTree;
  60: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Enemy") FName TargetKey = TEXT("TargetActor");
  62: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Enemy") float AttackRange = 180.f;
  64: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Enemy") float AttackExitRange = 220.f;
  66: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Enemy") float TargetMemorySeconds = 3.f;
  68: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Enemy") float SightRadius = 1400.f;
  69: protected:
  71: 	virtual void PreInitializeComponents() override;
  73: 	virtual void BeginPlay() override;
  77: 	virtual void OnPossess(APawn* ControlledPawn) override;
  79: 	virtual void OnUnPossess() override;
  83: 	virtual void EndPlay(EEndPlayReason::Type Reason) override;
  84: private:
  85: 	friend struct FHodgeEnemyAITestAccess;
  87: 	void HandleReadiness();
  89: 	void UpdateDecision();
  94: 	void HandleConstraint(FGameplayTag Tag, int32 Count);
  98: 	void StopCycle(const FString& Reason);
 103: 	UFUNCTION() void HandlePerception(AActor* Actor, FAIStimulus Stimulus);
 105: 	UPROPERTY(VisibleAnywhere) TObjectPtr<UAIPerceptionComponent> EnemyPerception;
 107: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgeEnemyInitComponent> Init;
 109: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgeAbilitySystemComponent> ASC;
 111: 	UPROPERTY(Transient) TWeakObjectPtr<AActor> Target;
 113: 	UPROPERTY(Transient) EHodgeEnemyBrainState State = EHodgeEnemyBrainState::Stopped;
 115: 	TSet<TWeakObjectPtr<AActor>> VisibleTargets;
 117: 	FDelegateHandle DeathHandle;
 119: 	FDelegateHandle ControlHandle;
 121: 	FTimerHandle DecisionTimer;
 123: 	uint32 PossessGeneration = 0;
 125: 	int32 StartCount = 0;
 127: 	double LastSeenTime = 0.;
 129: 	bool bInAttackRange = false;
 130: };
```
