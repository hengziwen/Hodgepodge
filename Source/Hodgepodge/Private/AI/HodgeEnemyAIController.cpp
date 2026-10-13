/** @file HodgeEnemyAIController.cpp
 * @brief 实现敌人AI控制器相关的类型、接口与现有处理流程。
 */
#include "AI/HodgeEnemyAIController.h"
#include "Character/HodgeEnemyCharacter.h"
#include "Character/HodgeHeroCharacter.h"
#include "Component/HodgeEnemyInitComponent.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "Combat/HodgeDamageRules.h"
#include "Combat/HodgeHitReactionTypes.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeEnemyAIController)

/** 构造敌人AI控制器并初始化其默认状态。 */
AHodgeEnemyAIController::AHodgeEnemyAIController(const FObjectInitializer& Initializer) : Super(Initializer)
{
	bWantsPlayerState = false;
	EnemyPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("EnemyPerception"));
	SetPerceptionComponent(*EnemyPerception);
	auto* Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("EnemySight"));
	Sight->SightRadius = 1400.f; Sight->LoseSightRadius = 1650.f; Sight->PeripheralVisionAngleDegrees = 80.f;
	Sight->DetectionByAffiliation.bDetectEnemies = true;
	Sight->DetectionByAffiliation.bDetectNeutrals = true;
	Sight->DetectionByAffiliation.bDetectFriendlies = false;
	EnemyPerception->ConfigureSense(*Sight);
	EnemyPerception->SetDominantSense(UAISense_Sight::StaticClass());
	EnemyPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::HandlePerception);
}

/** 在组件初始化前接入本对象所需的框架入口。 */
void AHodgeEnemyAIController::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

/** 进入世界运行阶段并接入本对象的生命周期依赖。 */
void AHodgeEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);
}

/** 开始新的控制器Possess周期并接入被控制Pawn。 */
void AHodgeEnemyAIController::OnPossess(APawn* ControlledPawn)
{
	StopCycle(TEXT("New Possess"));
	Super::OnPossess(ControlledPawn);
	// 按服务器权限区分本次处理分支。
	if (!HasAuthority()) { return; }
	State = EHodgeEnemyBrainState::Waiting;
	Init = ControlledPawn ? ControlledPawn->FindComponentByClass<UHodgeEnemyInitComponent>() : nullptr;
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!Init.IsValid()) { State = EHodgeEnemyBrainState::Failed; return; }
	// 按当前对象生命周期建立状态监听。
	Init->OnReadinessChanged.AddUObject(this, &ThisClass::HandleReadiness);
	Init->TryInitialize();
	TryStartBrain();
}

/** 判断Possess当前。 */
bool AHodgeEnemyAIController::IsPossessCurrent(const APawn* ControlledPawn, uint32 Generation) const
{ return HasAuthority() && Generation == PossessGeneration && ControlledPawn && ControlledPawn == GetPawn() && !ControlledPawn->IsActorBeingDestroyed(); }

/** 处理就绪。 */
void AHodgeEnemyAIController::HandleReadiness()
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (State == EHodgeEnemyBrainState::Running && (!Init.IsValid() || !Init->HasValidCombatDependencies()))
	{ StopCycle(TEXT("Combat dependencies invalidated")); return; }
	TryStartBrain();
}

/** 在当前Possess周期满足依赖时启动行为树。 */
bool AHodgeEnemyAIController::TryStartBrain()
{
	// 按服务器权限区分本次处理分支。
	if (!HasAuthority() || !GetPawn() || !Init.IsValid()) { return false; }
	if (State == EHodgeEnemyBrainState::Running) { return true; }
	if (State != EHodgeEnemyBrainState::Waiting || !Init->IsGameplayReady() || !Init->HasValidCombatDependencies()) { return false; }
	auto* Enemy = Cast<AHodgeEnemyCharacter>(GetPawn());
	ASC = Enemy ? Enemy->GetNativeEnemyAbilitySystem() : nullptr;
	const auto* Health = ASC.IsValid() ? ASC->GetSet<UHodgeHealthSet>() : nullptr;
	if (!Health || Health->GetHealth() <= 0.f || ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death)) { State = EHodgeEnemyBrainState::Stopped; return false; }
	if (!BehaviorTree || !BehaviorTree->BlackboardAsset || !FMath::IsFinite(AttackRange) || AttackRange <= 0.f || AttackExitRange < AttackRange)
	{
		State = EHodgeEnemyBrainState::Failed;
		UE_LOG(LogTemp, Warning, TEXT("Enemy AI [%s] has invalid BehaviorTree/Blackboard/range configuration"), *GetName());
		return false;
	}
	const uint32 Cycle = PossessGeneration;
	APawn* ControlledPawn = GetPawn();
	State = EHodgeEnemyBrainState::Starting;
	if (!RunBehaviorTree(BehaviorTree) || !IsPossessCurrent(ControlledPawn, Cycle))
	{ if (Cycle == PossessGeneration) { State = EHodgeEnemyBrainState::Failed; } return false; }
	State = EHodgeEnemyBrainState::Running; ++StartCount;
	// 按当前对象生命周期建立状态监听。
	DeathHandle = ASC->RegisterGameplayTagEvent(HodgeGameplayTags::Status_Death).AddUObject(this, &ThisClass::HandleConstraint);
	ControlHandle = ASC->RegisterGameplayTagEvent(HodgeHitReactionTags::Controlled).AddUObject(this, &ThisClass::HandleConstraint);
	if (auto* Sight = Cast<UAISenseConfig_Sight>(EnemyPerception->GetSenseConfig(UAISense::GetSenseID<UAISense_Sight>())))
	{
		Sight->SightRadius = SightRadius; Sight->LoseSightRadius = SightRadius + 250.f;
		EnemyPerception->ConfigureSense(*Sight); EnemyPerception->RequestStimuliListenerUpdate();
	}
	GetWorldTimerManager().SetTimer(DecisionTimer, this, &ThisClass::UpdateDecision, .2f, true);
	UpdateDecision();
	return true;
}

/** 判断有效敌人目标。 */
bool AHodgeEnemyAIController::IsValidEnemyTarget(const AActor* Actor) const
{
	// 首版敌人只以有效玩家 Hero 为候选，阵营扩展仍通过 DamageRules 过滤。
	return Actor && Actor->IsA<AHodgeHeroCharacter>() && Actor->GetWorld() == GetWorld() &&
		!Actor->IsActorBeingDestroyed() && GetPawn() && FHodgeDamageRules::CanDamage(GetPawn(), Actor, false);
}

/** 更新可见候选并触发必要的AI决策刷新。 */
void AHodgeEnemyAIController::HandlePerception(AActor* Actor, FAIStimulus Stimulus)
{
	// 按服务器权限区分本次处理分支。
	if (!HasAuthority()) { return; }
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (Stimulus.WasSuccessfullySensed() && IsValidEnemyTarget(Actor)) { VisibleTargets.Add(Actor); }
	else { VisibleTargets.Remove(Actor); }
	if (State == EHodgeEnemyBrainState::Running) { UpdateDecision(); }
}

/** 更新感知目标、行动限制和行为树黑板决策数据。 */
void AHodgeEnemyAIController::UpdateDecision()
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (State != EHodgeEnemyBrainState::Running || !GetPawn() || !Init.IsValid()) { return; }
	if (!Init->HasValidCombatDependencies()) { StopCycle(TEXT("Required combat resource removed")); return; }
	const auto* Health = ASC->GetSet<UHodgeHealthSet>();
	if (!Health || Health->GetHealth() <= 0.f || ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death)) { StopCycle(TEXT("Death")); return; }
	auto* BB = GetBlackboardComponent();
	if (!BB) { StopCycle(TEXT("Blackboard removed")); return; }
	const double Now = GetWorld()->GetTimeSeconds();
	AActor* Best = Target.IsValid() && VisibleTargets.Contains(Target) && IsValidEnemyTarget(Target.Get()) ? Target.Get() : nullptr;
	const bool bKeepTarget = Best != nullptr;
	float BestDistance = Best ? FVector::DistSquared2D(GetPawn()->GetActorLocation(), Best->GetActorLocation()) : TNumericLimits<float>::Max();
	for (auto It = VisibleTargets.CreateIterator(); It; ++It)
	{
		AActor* Candidate = It->Get();
		if (!IsValidEnemyTarget(Candidate)) { It.RemoveCurrent(); continue; }
		const float Distance = FVector::DistSquared2D(GetPawn()->GetActorLocation(), Candidate->GetActorLocation());
		if (!bKeepTarget && Distance < BestDistance) { Best = Candidate; BestDistance = Distance; }
	}
	const bool bSeen = Best != nullptr;
	if (bSeen)
	{
		Target = Best; LastSeenTime = Now;
		BB->SetValueAsVector(TEXT("LastKnownLocation"), Best->GetActorLocation());
	}
	else if (!IsValidEnemyTarget(Target.Get()) || Now - LastSeenTime > TargetMemorySeconds) { Target.Reset(); }
	// 不可见时只移动到最后已知位置，不继续追踪隐藏目标的实时位置。
	BB->SetValueAsObject(TargetKey, bSeen ? Target.Get() : nullptr);
	BB->SetValueAsBool(TEXT("HasLastKnownLocation"), Target.IsValid());
	BB->SetValueAsBool(TEXT("HasLineOfSight"), bSeen);
	const bool bCanAct = Init->CanExecuteAIAction();
	BB->SetValueAsBool(TEXT("CanAct"), bCanAct);
	const float Range = bInAttackRange ? AttackExitRange : AttackRange;
	bInAttackRange = bSeen && FVector::DistSquared2D(GetPawn()->GetActorLocation(), Best->GetActorLocation()) <= FMath::Square(Range);
	BB->SetValueAsBool(TEXT("InAttackRange"), bInAttackRange);
	if (!bCanAct) { StopMovement(); }
}

/** 处理约束。 */
void AHodgeEnemyAIController::HandleConstraint(FGameplayTag Tag, int32 Count)
{
	if (Tag == HodgeGameplayTags::Status_Death && Count > 0) { StopCycle(TEXT("Death")); }
	else { UpdateDecision(); }
}

/** 停止本次AI控制周期并解绑其决策依赖。 */
void AHodgeEnemyAIController::StopCycle(const FString& Reason)
{
	++PossessGeneration; State = EHodgeEnemyBrainState::Stopped;
	GetWorldTimerManager().ClearTimer(DecisionTimer);
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (Init.IsValid()) { Init->OnReadinessChanged.RemoveAll(this); }
	if (ASC.IsValid())
	{
		ASC->RegisterGameplayTagEvent(HodgeGameplayTags::Status_Death).Remove(DeathHandle);
		ASC->RegisterGameplayTagEvent(HodgeHitReactionTags::Controlled).Remove(ControlHandle);
	}
	if (BrainComponent) { BrainComponent->StopLogic(Reason); }
	StopMovement(); ClearFocus(EAIFocusPriority::Gameplay);
	Init.Reset(); ASC.Reset(); Target.Reset(); VisibleTargets.Reset(); bInAttackRange = false;
}

/** 结束当前Possess周期并解除旧Pawn依赖。 */
void AHodgeEnemyAIController::OnUnPossess() { StopCycle(TEXT("UnPossess")); Super::OnUnPossess(); }
/** 离开世界运行阶段并解除本对象持有的监听或资源。 */
void AHodgeEnemyAIController::EndPlay(EEndPlayReason::Type Reason)
{
	StopCycle(TEXT("EndPlay"));
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);
	Super::EndPlay(Reason);
}
