/** @file HodgeEncounterSpawner.cpp
 * @brief 沿用 Lyra 的服务器受理、复制表现与配对清理，普通敌人继续走自身 EnemyInit。
 */
// Copyright Epic Games, Inc. All Rights Reserved.
#include "Encounter/HodgeEncounterSpawner.h"

#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "AI/HodgeEnemyAIController.h"
#include "Character/HodgeEnemyCharacter.h"
#include "Character/HodgeHeroCharacter.h"
#include "Component/HodgeEnemyInitComponent.h"
#include "Component/HodgeExperienceManagerComponent.h"
#include "Component/HodgeHealthComponent.h"
#include "Component/HodgeInteractionComponentBase.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Core/PlayerController/HodgePlayerController.h"
#include "Core/PlayState/HodgePlayerState.h"
#include "Data/HodgeEncounterDefinition.h"
#include "Data/HodgePawnData.h"
#include "Engine/StaticMesh.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "Inventory/HodgeInventoryManagerComponent.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "UI/Encounter/HodgeEncounterPromptWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeEncounterSpawner)

namespace HodgeEncounter
{
	/** 请求失败不伪装已提交；具体失败原因保留给拥有者与验证工具。 */
	FHodgeInteractionResult Failure(const FString& Reason)
	{
		FHodgeInteractionResult Result;
		Result.Reason = Reason;
		return Result;
	}
	double ServerTime(const UWorld* World)
	{
		const AGameStateBase* State = World ? World->GetGameState() : nullptr;
		return State ? State->GetServerWorldTimeSeconds() : World ? World->GetTimeSeconds() : 0.0;
	}
}

AHodgeEncounterSpawner::AHodgeEncounterSpawner()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("EncounterRoot")));
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetupAttachment(RootComponent);
	TriggerVolume->InitBoxExtent(FVector(300.0, 300.0, 150.0));
	TriggerVolume->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TriggerVolume->SetGenerateOverlapEvents(true);
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandleAreaBegin);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &ThisClass::HandleAreaEnd);

	// 开关碰撞只供交互候选与视线查询；隐藏它不影响区域重叠登记。
	SwitchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwitchMesh"));
	SwitchMesh->SetupAttachment(RootComponent);
	SwitchMesh->SetCollisionObjectType(ECC_WorldDynamic);
	SwitchMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	SwitchMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	SwitchMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (Cube.Succeeded()) { SwitchMesh->SetStaticMesh(Cube.Object); }
	SwitchMesh->SetRelativeScale3D(FVector(.5, .5, 1.0));
	SwitchPrompt = CreateDefaultSubobject<UWidgetComponent>(TEXT("SwitchPrompt"));
	SwitchPrompt->SetupAttachment(RootComponent);
	SwitchPrompt->SetWidgetClass(UHodgeEncounterPromptWidget::StaticClass());
	SwitchPrompt->SetWidgetSpace(EWidgetSpace::Screen);
	SwitchPrompt->SetDrawSize(FVector2D(260.0, 50.0));
	SwitchPrompt->SetRelativeLocation(FVector(0.0, 0.0, 90.0));
	SwitchPrompt->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AHodgeEncounterSpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ConfigureComponents();
	ApplyPresentation();
}

void AHodgeEncounterSpawner::BeginPlay()
{
	Super::BeginPlay();
	ConfigureComponents();
	if (HasAuthority())
	{
		SetInactive();
		FString Error;
		if (!Definition || !Definition->Validate(Error))
		{
			Snapshot.LastFailure = Definition ? Error : TEXT("刷怪点缺少 EncounterDefinition");
			UE_LOG(LogTemp, Error, TEXT("Encounter %s configuration: %s"), *GetName(), *Snapshot.LastFailure);
			Publish();
		}
	}
	bRuntimeReady = true;
	ApplyPresentation();
	InitializeFirstAreaCatchup();
}

void AHodgeEncounterSpawner::EndPlay(EEndPlayReason::Type Reason)
{
	// 先使轮次失效再释放成员，主动清理不能触发下一波或奖励。
	bClosing = true;
	bRuntimeReady = false;
	RetireFirstAreaCatchup();
	if (HasAuthority()) { RetireRun(true); }
	AreaOccupants.Reset();
	ArmedExitEntrants.Reset();
	Super::EndPlay(Reason);
}

void AHodgeEncounterSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AHodgeEncounterSpawner, Definition);
	DOREPLIFETIME(AHodgeEncounterSpawner, Snapshot);
}

FHodgeEncounterSnapshot AHodgeEncounterSpawner::GetSnapshot() const { return Snapshot; }

TArray<AHodgeEnemyCharacter*> AHodgeEncounterSpawner::GetLivingEnemies() const
{
	TArray<AHodgeEnemyCharacter*> Result;
	for (const auto& Pair : Members)
	{
		if (!Pair.Value.bDeathConfirmed && Pair.Value.Enemy.IsValid()) { Result.Add(Pair.Value.Enemy.Get()); }
	}
	return Result;
}

bool AHodgeEncounterSpawner::IsExperienceReady() const
{
	if (!bRequireExperience) { return true; }
	const AGameStateBase* State = GetWorld() ? GetWorld()->GetGameState() : nullptr;
	const UHodgeExperienceManagerComponent* Experience = State ? State->FindComponentByClass<UHodgeExperienceManagerComponent>() : nullptr;
	return Experience && Experience->IsExperienceLoaded();
}

bool AHodgeEncounterSpawner::ValidatePlayer(const APawn* Pawn, AHodgePlayerController*& OutPC,
	AHodgePlayerState*& OutState, FString& Error) const
{
	OutPC = IsValid(Pawn) ? Cast<AHodgePlayerController>(Pawn->GetController()) : nullptr;
	OutState = OutPC ? OutPC->GetHodgePlayerState() : nullptr;
	const auto* ASC = OutPC ? OutPC->GetHodgeAbilitySystemComponent() : nullptr;
	const auto* Health = UHodgeHealthComponent::FindHealthComponent(Pawn);
	if (!IsValid(Pawn) || !IsValid(OutPC) || !IsValid(OutState) || Pawn->GetWorld() != GetWorld() ||
		OutPC->GetWorld() != GetWorld() || OutState->GetWorld() != GetWorld() || OutPC->GetPawn() != Pawn || Pawn->GetPlayerState() != OutState ||
		!ASC || ASC->GetOwnerActor() != OutState || ASC->GetAvatarActor() != Pawn)
	{
		Error = TEXT("请求者不是当前有效的玩家 Pawn／PlayerState／ASC Avatar");
		return false;
	}
	if (!OutState->AreAttributesReadyFor(Pawn))
	{
		Error = TEXT("玩家当前 Avatar 的属性协调尚未完成");
		return false;
	}
	if (!Health || Health->IsDeadOrDying() || !FMath::IsFinite(Health->GetHealth()) || Health->GetHealth() <= 0.0f)
	{
		Error = TEXT("请求玩家未存活或生命初始化尚未完成");
		return false;
	}
	return true;
}

bool AHodgeEncounterSpawner::ValidateInteractionGeometry(const APawn* Pawn, const AHodgePlayerController* PC) const
{
	const auto* Interaction = PC ? PC->FindComponentByClass<UHodgeInteractionComponentBase>() : nullptr;
	const float Range = Interaction ? Interaction->GetInteractionDistance() : 0.0f;
	if (!Pawn || !GetWorld() || !FMath::IsFinite(Range) || Range <= 0.0f || Pawn->GetWorld() != GetWorld() ||
		Pawn->GetActorLocation().ContainsNaN() || GetActorLocation().ContainsNaN() || Pawn->GetPawnViewLocation().ContainsNaN() ||
		FVector::DistSquared(Pawn->GetActorLocation(), GetActorLocation()) > FMath::Square(Range) || IsHidden()) { return false; }
	FCollisionQueryParams Params(SCENE_QUERY_STAT(HodgeEncounterInteraction), false, Pawn);
	FHitResult Hit;
	const bool bBlocked = GetWorld()->LineTraceSingleByChannel(Hit, Pawn->GetPawnViewLocation(), GetActorLocation(), ECC_Visibility, Params);
	return !bBlocked || Hit.GetActor() == this;
}

bool AHodgeEncounterSpawner::GetInteractionOption(const APawn* Pawn, FHodgeInteractionOption& Option) const
{
	if (!bRuntimeReady || !Definition || Definition->Mode != EHodgeEncounterMode::Interaction || !Snapshot.RunId.IsValid() || bClosing || bSettling ||
		(Snapshot.State != EHodgeEncounterState::Inactive && Snapshot.State != EHodgeEncounterState::AwaitingReward)) { return false; }
	AHodgePlayerController* PC = nullptr;
	AHodgePlayerState* PS = nullptr;
	FString Error;
	if (!Definition->Validate(Error) || !ValidatePlayer(Pawn, PC, PS, Error) || !IsExperienceReady() || !ValidateInteractionGeometry(Pawn, PC) ||
		(Snapshot.State == EHodgeEncounterState::AwaitingReward && Snapshot.OwnerPlayerState != PS)) { return false; }
	Option.RunId = Snapshot.RunId;
	Option.Revision = Snapshot.Revision;
	Option.Text = FText::FromString(Snapshot.State == EHodgeEncounterState::Inactive ? TEXT("启动挑战") : TEXT("领取奖励"));
	return true;
}

FHodgeInteractionResult AHodgeEncounterSpawner::ExecuteInteraction(APawn* Pawn, const FGuid& RunId, int32 Revision)
{
	if (!HasAuthority() || bClosing || bSettling || !RunId.IsValid() || Snapshot.RunId != RunId || Snapshot.Revision != Revision)
	{ return HodgeEncounter::Failure(TEXT("交互轮次或修订已经失效")); }
	FHodgeInteractionOption Option;
	if (!GetInteractionOption(Pawn, Option)) { return HodgeEncounter::Failure(TEXT("当前玩家、距离、视线或刷怪点状态不允许交互")); }
	if (Snapshot.State == EHodgeEncounterState::Inactive) { return TryStartEncounter(Pawn); }
	AHodgePlayerState* PS = Snapshot.OwnerPlayerState;
	auto* Inventory = PS ? PS->FindComponentByClass<UHodgeInventoryManagerComponent>() : nullptr;
	if (!Inventory) { return HodgeEncounter::Failure(TEXT("奖励库存尚未就绪")); }
	FString Error;
	if (!Definition || RunDefinition != Definition || !Definition->Validate(Error))
	{ return HodgeEncounter::Failure(Error.IsEmpty() ? TEXT("本轮奖励配置身份已经改变") : Error); }

	// 库存委托可能同步重入；本轮状态只在真实提交后改变，旧轮次不得恢复。
	TGuardValue<bool> SettlingGuard(bSettling, true);
	const auto Result = Inventory->AddItemDefinition(Definition->RewardDefinition, Definition->RewardCount, Definition->RewardLevel);
	if (!Result.Succeeded()) { return HodgeEncounter::Failure(Result.Reason); }
	if (!bClosing && Snapshot.RunId == RunId && Snapshot.State == EHodgeEncounterState::AwaitingReward)
	{
		Snapshot.State = EHodgeEncounterState::Completed;
		Publish();
		RetireRun(false);
		SetInactive();
	}
	FHodgeInteractionResult Success;
	Success.bSucceeded = true;
	return Success;
}

FHodgeInteractionResult AHodgeEncounterSpawner::TryStartEncounter(APawn* Pawn)
{
	if (!HasAuthority() || !bRuntimeReady || bClosing || bSettling || Snapshot.State != EHodgeEncounterState::Inactive)
	{ return HodgeEncounter::Failure(TEXT("刷怪点当前不可启动")); }
	FString Error;
	if (!Definition || !Definition->Validate(Error))
	{ return HodgeEncounter::Failure(Definition ? Error : TEXT("刷怪点缺少 EncounterDefinition")); }
	AHodgePlayerController* PC = nullptr;
	AHodgePlayerState* PS = nullptr;
	if (!ValidatePlayer(Pawn, PC, PS, Error)) { return HodgeEncounter::Failure(Error); }
	if (!IsExperienceReady()) { return HodgeEncounter::Failure(TEXT("Experience 依赖尚未就绪")); }
	if (Definition->Mode == EHodgeEncounterMode::Interaction && !ValidateInteractionGeometry(Pawn, PC))
	{ return HodgeEncounter::Failure(TEXT("启动开关超距或被遮挡")); }
	// 首次真正接受启动后，后续任何恢复或失败都必须重新形成合法边沿。
	RetireFirstAreaCatchup();
	RunDefinition = Definition;
	Snapshot.RunId = FGuid::NewGuid();
	Snapshot.State = EHodgeEncounterState::Active;
	Snapshot.OwnerPlayerState = PS;
	Snapshot.LastFailure.Reset();
	Snapshot.CooldownEndServerTime = 0.0;
	Snapshot.bNeedsExit = false;
	BindOwner(PC, Pawn);
	ScheduleWave(0);
	FHodgeInteractionResult Success;
	Success.bSucceeded = true;
	return Success;
}

bool AHodgeEncounterSpawner::IsRunCurrent(const FGuid& RunId, int32 Index) const
{
	return HasAuthority() && !bClosing && RunId.IsValid() && Snapshot.RunId == RunId && RunDefinition &&
		(Index == INDEX_NONE || Snapshot.WaveIndex == Index) &&
		(Snapshot.State == EHodgeEncounterState::Active || Snapshot.State == EHodgeEncounterState::Spawning);
}

void AHodgeEncounterSpawner::ScheduleWave(int32 Index)
{
	if (!RunDefinition || !RunDefinition->Waves.IsValidIndex(Index)) { FailEncounter(TEXT("波次索引或配置失效")); return; }
	GetWorldTimerManager().ClearTimer(EvaluationTimer);
	bEvaluationQueued = false;
	Snapshot.State = EHodgeEncounterState::Active;
	Snapshot.WaveIndex = Index;
	Snapshot.AliveCount = 0;
	Snapshot.PendingCount = RunDefinition->Waves[Index].Enemies.Num();
	Publish();
	const FGuid RunId = Snapshot.RunId;
	const FTimerDelegate Callback = FTimerDelegate::CreateWeakLambda(this, [this, RunId, Index]() { SpawnWave(RunId, Index); });
	const float Delay = RunDefinition->Waves[Index].DelayBeforeSpawn;
	if (Delay > 0.0f) { GetWorldTimerManager().SetTimer(WaveTimer, Callback, Delay, false); }
	else { WaveTimer = GetWorldTimerManager().SetTimerForNextTick(Callback); }
}

void AHodgeEncounterSpawner::SpawnWave(FGuid RunId, int32 Index)
{
	if (!IsRunCurrent(RunId, Index)) { return; }
	if (!RunDefinition->Waves.IsValidIndex(Index)) { FailEncounter(TEXT("等待期间波次配置索引已失效")); return; }
	bSpawning = true;
	Snapshot.State = EHodgeEncounterState::Spawning;
	Publish();
	GetWorldTimerManager().SetTimer(InitializationTimer,
		FTimerDelegate::CreateWeakLambda(this, [this, RunId, Index]() { HandleInitializationTimeout(RunId, Index); }),
		RunDefinition->EnemyInitializationTimeout, false);
	const TArray<FHodgeEncounterSpawn> Entries = RunDefinition->Waves[Index].Enemies;
	for (const auto& Entry : Entries)
	{
		if (!IsRunCurrent(RunId, Index)) { return; }
		if (!IsValid(Entry.PawnData.Get())) { FailEncounter(TEXT("等待期间敌人 PawnData 配置已失效")); return; }
		const FTransform Transform = Entry.RelativeTransform * GetActorTransform();
		if (Transform.ContainsNaN() || !Transform.GetRotation().IsNormalized())
		{ FailEncounter(TEXT("等待期间敌人出生变换已失效")); return; }
		auto* Enemy = GetWorld()->SpawnActorDeferred<AHodgeEnemyCharacter>(Entry.PawnData->PawnClass, Transform,
			this, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding);
		if (!Enemy) { FailEncounter(FString::Printf(TEXT("第 %d 波敌人生成失败"), Index + 1)); return; }
		FMember Member;
		Member.Enemy = Enemy;
		Member.Health = UHodgeHealthComponent::FindHealthComponent(Enemy);
		Member.Init = Enemy->GetEnemyInitComponent();
		Member.RunId = RunId;
		Member.WaveIndex = Index;
		Members.Add(Enemy, Member);
		++Snapshot.AliveCount;
		if (!Member.Health.IsValid() || !Member.Init.IsValid()) { FailEncounter(TEXT("生成敌人缺少 Health 或 EnemyInit")); return; }
		Member.Health->OnDeathStarted.AddUniqueDynamic(this, &ThisClass::HandleEnemyDeath);
		Enemy->OnEndPlay.AddUniqueDynamic(this, &ThisClass::HandleEnemyEndPlay);
		const TWeakObjectPtr<AHodgeEnemyCharacter> WeakEnemy = Enemy;
		Members.FindChecked(WeakEnemy).ReadinessHandle = Member.Init->OnReadinessChanged.AddWeakLambda(this,
			[this, RunId, WeakEnemy]() { HandleInitializationChanged(RunId, WeakEnemy); });
		Enemy->EnemyPawnData = Entry.PawnData;
		Enemy->InitialLevel = Entry.Level;
		Enemy->FinishSpawning(Transform);
		if (!IsRunCurrent(RunId, Index)) { return; }
		if (!IsValid(Enemy)) { FailEncounter(TEXT("敌人在完成生成时意外移除")); return; }
		if (Enemy->EnemyPawnData != Entry.PawnData || Enemy->InitialLevel != Entry.Level)
		{ FailEncounter(TEXT("敌人构造改写了本波指定的 PawnData 或等级")); return; }
		const auto* Health = UHodgeHealthComponent::FindHealthComponent(Enemy);
		if (Health && Health->IsDeadOrDying()) { HandleEnemyDeath(Enemy); }
	}
	if (!IsRunCurrent(RunId, Index)) { return; }
	bSpawning = false;
	Publish();
	EvaluateWave(RunId, Index);
}

void AHodgeEncounterSpawner::HandleInitializationChanged(FGuid RunId, TWeakObjectPtr<AHodgeEnemyCharacter> Enemy)
{
	const auto* Member = Members.Find(Enemy);
	if (!IsRunCurrent(RunId) || !Member || Member->RunId != RunId || Member->bDeathConfirmed) { return; }
	// 活着时已宣布的终态失败立即退休，不能被同帧后续 DeathStarted 洗成清波。
	if (Member->Init.IsValid() && Member->Health.IsValid() && !Member->Health->IsDeadOrDying())
	{
		const auto Result = Member->Init->GetInitializationResult();
		if (Result == EHodgeEnemyInitializationResult::Failed || Result == EHodgeEnemyInitializationResult::Invalidated)
		{ FailEncounter(TEXT("敌人初始化失效：") + Member->Init->GetInitializationDiagnostic()); return; }
	}
	QueueEvaluation();
}

void AHodgeEncounterSpawner::QueueEvaluation()
{
	if (bEvaluationQueued || !IsRunCurrent(Snapshot.RunId)) { return; }
	bEvaluationQueued = true;
	const FGuid RunId = Snapshot.RunId;
	const int32 Index = Snapshot.WaveIndex;
	EvaluationTimer = GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this, RunId, Index]()
	{
		if (!IsRunCurrent(RunId, Index)) { return; }
		bEvaluationQueued = false;
		EvaluateWave(RunId, Index);
	}));
}

void AHodgeEncounterSpawner::EvaluateWave(FGuid RunId, int32 Index)
{
	if (!IsRunCurrent(RunId, Index) || bSpawning) { return; }
	const int32 PreviousPending = Snapshot.PendingCount;
	TArray<TWeakObjectPtr<AHodgeEnemyCharacter>> Keys;
	Members.GetKeys(Keys);
	for (const auto& Key : Keys)
	{
		auto* Member = Members.Find(Key);
		if (!Member || Member->RunId != RunId || Member->WaveIndex != Index || Member->bDeathConfirmed) { continue; }
		if (!Member->Enemy.IsValid() || !Member->Health.IsValid() || !Member->Init.IsValid())
		{ FailEncounter(TEXT("本波存活敌人的依赖或对象已失效")); return; }
		if (Member->Health->IsDeadOrDying()) { HandleEnemyDeath(Member->Enemy.Get()); continue; }
		const auto Result = Member->Init->GetInitializationResult();
		if (Result == EHodgeEnemyInitializationResult::Failed || Result == EHodgeEnemyInitializationResult::Invalidated)
		{ FailEncounter(TEXT("敌人初始化失败：") + Member->Init->GetInitializationDiagnostic()); return; }
		const auto* Brain = Cast<AHodgeEnemyAIController>(Member->Enemy->GetController());
		if (Brain && Brain->GetEnemyBrainState() == EHodgeEnemyBrainState::Failed)
		{ FailEncounter(TEXT("敌人行为树启动失败")); return; }
		if (!Member->bInitialized && Result == EHodgeEnemyInitializationResult::Ready && Brain &&
			Brain->GetEnemyBrainState() == EHodgeEnemyBrainState::Running)
		{
			Member->bInitialized = true;
			--Snapshot.PendingCount;
		}
	}
	if (PreviousPending != Snapshot.PendingCount) { Publish(); }
	if (Snapshot.PendingCount == 0) { GetWorldTimerManager().ClearTimer(InitializationTimer); }
	if (Snapshot.PendingCount == 0 && Snapshot.AliveCount == 0) { FinishWave(); }
}

void AHodgeEncounterSpawner::HandleInitializationTimeout(FGuid RunId, int32 Index)
{
	if (!IsRunCurrent(RunId, Index)) { return; }
	EvaluateWave(RunId, Index);
	if (IsRunCurrent(RunId, Index) && Snapshot.PendingCount > 0) { FailEncounter(TEXT("敌人初始化或行为树就绪等待超时")); }
}

void AHodgeEncounterSpawner::HandleEnemyDeath(AActor* Actor)
{
	auto* Enemy = Cast<AHodgeEnemyCharacter>(Actor);
	auto* Member = Members.Find(Enemy);
	if (!Member || !IsRunCurrent(Member->RunId, Member->WaveIndex) || Member->bDeathConfirmed ||
		!Member->Health.IsValid() || !Member->Health->IsDeadOrDying()) { return; }
	Member->bDeathConfirmed = true;
	--Snapshot.AliveCount;
	if (!Member->bInitialized) { --Snapshot.PendingCount; }
	Publish();
	QueueEvaluation();
}

void AHodgeEncounterSpawner::HandleEnemyEndPlay(AActor* Actor, EEndPlayReason::Type Reason)
{
	const auto* Member = Members.Find(Cast<AHodgeEnemyCharacter>(Actor));
	if (!Member || !IsRunCurrent(Member->RunId, Member->WaveIndex) || Member->bDeathConfirmed) { return; }
	FailEncounter(TEXT("本波敌人未经死亡确认即离开世界"));
}

void AHodgeEncounterSpawner::FinishWave()
{
	const int32 Next = Snapshot.WaveIndex + 1;
	ClearMembers(false);
	if (RunDefinition && RunDefinition->Waves.IsValidIndex(Next)) { ScheduleWave(Next); return; }
	if (RunDefinition && RunDefinition->Mode == EHodgeEncounterMode::Interaction)
	{
		Snapshot.State = EHodgeEncounterState::AwaitingReward;
		Publish();
		return;
	}
	RetireRun(false);
	CompleteArea();
}

void AHodgeEncounterSpawner::ClearMembers(bool bDestroyLiving)
{
	const auto Previous = MoveTemp(Members);
	Members.Reset();
	for (const auto& Pair : Previous)
	{
		const auto& Member = Pair.Value;
		if (Member.Init.IsValid()) { Member.Init->OnReadinessChanged.Remove(Member.ReadinessHandle); }
		if (Member.Health.IsValid()) { Member.Health->OnDeathStarted.RemoveDynamic(this, &ThisClass::HandleEnemyDeath); }
		if (Member.Enemy.IsValid())
		{
			Member.Enemy->OnEndPlay.RemoveDynamic(this, &ThisClass::HandleEnemyEndPlay);
			if (bDestroyLiving && !Member.bDeathConfirmed) { Member.Enemy->Destroy(); }
		}
	}
}

void AHodgeEncounterSpawner::RetireRun(bool bDestroyLiving)
{
	Snapshot.RunId = FGuid::NewGuid();
	GetWorldTimerManager().ClearTimer(WaveTimer);
	GetWorldTimerManager().ClearTimer(InitializationTimer);
	GetWorldTimerManager().ClearTimer(EvaluationTimer);
	GetWorldTimerManager().ClearTimer(CooldownTimer);
	bSpawning = false;
	bEvaluationQueued = false;
	UnbindOwner();
	ClearMembers(bDestroyLiving);
	RunDefinition = nullptr;
}

void AHodgeEncounterSpawner::SetInactive()
{
	Snapshot.RunId = FGuid::NewGuid();
	Snapshot.State = EHodgeEncounterState::Inactive;
	Snapshot.WaveIndex = INDEX_NONE;
	Snapshot.AliveCount = 0;
	Snapshot.PendingCount = 0;
	Snapshot.OwnerPlayerState = nullptr;
	Snapshot.CooldownEndServerTime = 0.0;
	Snapshot.bNeedsExit = false;
	bAreaRetryAfterFailure = false;
	ArmedExitEntrants.Reset();
	Publish();
}

void AHodgeEncounterSpawner::FailEncounter(const FString& Reason)
{
	if (!HasAuthority() || bClosing) { return; }
	RetireFirstAreaCatchup();
	Snapshot.LastFailure = Reason;
	UE_LOG(LogTemp, Error, TEXT("Encounter %s failed: %s"), *GetName(), *Reason);
	RetireRun(true);
	if (Definition && Definition->Mode == EHodgeEncounterMode::Area) { CompleteArea(true); }
	else { SetInactive(); }
}

void AHodgeEncounterSpawner::ResetEncounter()
{
	if (!HasAuthority() || bClosing || bSettling) { return; }
	if (Snapshot.State != EHodgeEncounterState::Active && Snapshot.State != EHodgeEncounterState::Spawning &&
		Snapshot.State != EHodgeEncounterState::AwaitingReward) { return; }
	FailEncounter(TEXT("外部请求取消当前刷怪轮次"));
}

void AHodgeEncounterSpawner::CompleteArea(bool bFailed)
{
	// 一次性只限制成功完成次数；失败仍按批准的冷却与离开规则重试。
	bAreaRetryAfterFailure = bFailed;
	ArmedExitEntrants.Reset();
	const bool bCanRearm = Definition && (Definition->bRepeatable || bAreaRetryAfterFailure);
	Snapshot.State = EHodgeEncounterState::Completed;
	Snapshot.AliveCount = 0;
	Snapshot.PendingCount = 0;
	Snapshot.OwnerPlayerState = nullptr;
	Snapshot.bNeedsExit = bCanRearm;
	Snapshot.CooldownEndServerTime = bCanRearm ? HodgeEncounter::ServerTime(GetWorld()) + Definition->CooldownSeconds : 0.0;
	RefreshAreaOccupants();
	if (AreaOccupants.IsEmpty()) { Snapshot.bNeedsExit = false; }
	Publish();
	if (bCanRearm)
	{
		const FGuid Epoch = Snapshot.RunId;
		const FTimerDelegate Callback = FTimerDelegate::CreateWeakLambda(this, [this, Epoch]()
		{ if (Snapshot.RunId == Epoch) { TryRearmArea(); } });
		if (Definition->CooldownSeconds > 0.0f) { GetWorldTimerManager().SetTimer(CooldownTimer, Callback, Definition->CooldownSeconds, false); }
		else { CooldownTimer = GetWorldTimerManager().SetTimerForNextTick(Callback); }
	}
}

void AHodgeEncounterSpawner::RefreshAreaOccupants()
{
	AreaOccupants.Reset();
	TArray<AActor*> Overlaps;
	TriggerVolume->GetOverlappingActors(Overlaps, AHodgeHeroCharacter::StaticClass());
	for (AActor* Actor : Overlaps)
	{
		if (APawn* Pawn = Cast<APawn>(Actor)) { AreaOccupants.Add(Pawn); }
	}
}

void AHodgeEncounterSpawner::TryRearmArea()
{
	if (!HasAuthority() || bClosing || !Definition || Definition->Mode != EHodgeEncounterMode::Area ||
		(!Definition->bRepeatable && !bAreaRetryAfterFailure) || Snapshot.State != EHodgeEncounterState::Completed) { return; }
	RefreshAreaOccupants();
	if (Snapshot.bNeedsExit && AreaOccupants.IsEmpty()) { Snapshot.bNeedsExit = false; Publish(); }
	if (!Snapshot.bNeedsExit && HodgeEncounter::ServerTime(GetWorld()) + UE_KINDA_SMALL_NUMBER >= Snapshot.CooldownEndServerTime)
	{ SetInactive(); }
}

bool AHodgeEncounterSpawner::CanFirstAreaCatchup() const
{
	return HasAuthority() && bRuntimeReady && !bClosing && !bSettling && bInitialAreaCatchupAvailable &&
		InitialAreaEpoch.IsValid() && Definition && Definition->Mode == EHodgeEncounterMode::Area &&
		Snapshot.State == EHodgeEncounterState::Inactive && !Snapshot.bNeedsExit;
}

void AHodgeEncounterSpawner::InitializeFirstAreaCatchup()
{
	FString Error;
	if (!HasAuthority() || !bRuntimeReady || !Definition || Definition->Mode != EHodgeEncounterMode::Area || !Definition->Validate(Error)) { return; }
	bInitialAreaCatchupAvailable = true;
	InitialAreaEpoch = FGuid::NewGuid();
	const FGuid Epoch = InitialAreaEpoch;
	// 先监听稍后出现的 GameState，再查询当前 Experience，避免遗漏同步加载完成。
	InitialAreaGameStateHandle = GetWorld()->GameStateSetEvent.AddWeakLambda(this, [this, Epoch](AGameStateBase*)
	{
		if (InitialAreaEpoch != Epoch || !CanFirstAreaCatchup()) { return; }
		RegisterFirstAreaExperience();
		RetryFirstAreaCandidates(Epoch);
	});
	RegisterFirstAreaExperience();
	RefreshAreaOccupants();
	const TArray<TWeakObjectPtr<APawn>> Existing = AreaOccupants.Array();
	for (const auto& Pawn : Existing)
	{
		if (!CanFirstAreaCatchup()) { break; }
		RegisterFirstAreaCandidate(Pawn.Get());
	}
}

void AHodgeEncounterSpawner::RegisterFirstAreaExperience()
{
	if (!CanFirstAreaCatchup() || !bRequireExperience) { return; }
	AGameStateBase* State = GetWorld()->GetGameState();
	auto* Experience = State ? State->FindComponentByClass<UHodgeExperienceManagerComponent>() : nullptr;
	if (!Experience || InitialAreaExperience.Get() == Experience) { return; }
	InitialAreaExperience = Experience;
	const TWeakObjectPtr<UHodgeExperienceManagerComponent> WeakExperience = Experience;
	const FGuid Epoch = InitialAreaEpoch;
	// 该既有入口不返回解绑句柄；弱对象、来源 Manager 与退休 epoch 一同阻止旧回调执行。
	Experience->CallOrRegister_OnExperienceLoaded_LowPriority(FOnHodgeExperienceLoaded::FDelegate::CreateWeakLambda(this,
		[this, Epoch, WeakExperience](const UHodgeExperienceDefinition*)
		{
			if (InitialAreaEpoch == Epoch && InitialAreaExperience.HasSameIndexAndSerialNumber(WeakExperience) && CanFirstAreaCatchup())
			{ RetryFirstAreaCandidates(Epoch); }
		}));
}

void AHodgeEncounterSpawner::RegisterFirstAreaCandidate(APawn* Pawn)
{
	if (!CanFirstAreaCatchup() || !IsValid(Pawn) || Pawn->IsActorBeingDestroyed() || Pawn->GetWorld() != GetWorld() || !TriggerVolume->IsOverlappingActor(Pawn)) { return; }
	const TWeakObjectPtr<APawn> WeakPawn = Pawn;
	if (!InitialAreaCandidates.Contains(WeakPawn))
	{
		FInitialAreaCandidate Candidate;
		Candidate.Pawn = Pawn;
		Candidate.Identity = FGuid::NewGuid();
		Candidate.Health = UHodgeHealthComponent::FindHealthComponent(Pawn);
		InitialAreaCandidates.Add(WeakPawn, Candidate);
		Pawn->OnEndPlay.AddUniqueDynamic(this, &ThisClass::HandleInitialAreaCandidateEndPlay);
		if (Candidate.Health.IsValid()) { Candidate.Health->OnDeathStarted.AddUniqueDynamic(this, &ThisClass::HandleInitialAreaCandidateDeath); }
		if (UGameInstance* Instance = Pawn->GetGameInstance())
		{
			if (auto* Manager = Instance->GetSubsystem<UGameFrameworkComponentManager>())
			{
				const FGuid Epoch = InitialAreaEpoch;
				const FGuid Identity = Candidate.Identity;
				auto& Stored = InitialAreaCandidates.FindChecked(WeakPawn);
				Stored.InitManager = Manager;
				// 先保存返回句柄再显式查询，已就绪对象不能在赋值前同步退休记录。
				Stored.InitStateHandle = Manager->RegisterAndCallForActorInitState(Pawn, UHodgePawnExtensionComponent::NAME_ActorFeatureName,
					HodgeGameplayTags::InitState_GameplayReady, FActorInitStateChangedDelegate::CreateWeakLambda(this,
						[this, Epoch, WeakPawn, Identity](const FActorInitStateChangedParams&)
						{ HandleFirstAreaCandidateReady(Epoch, WeakPawn, Identity); }), false);
			}
		}
	}
	if (auto* Candidate = InitialAreaCandidates.Find(WeakPawn))
	{
		BindFirstAreaCandidateAttributes(*Candidate);
		HandleFirstAreaCandidateReady(InitialAreaEpoch, WeakPawn, Candidate->Identity);
	}
}

void AHodgeEncounterSpawner::BindFirstAreaCandidateAttributes(FInitialAreaCandidate& Candidate)
{
	APawn* Pawn = Candidate.Pawn.Get();
	AHodgePlayerState* State = Pawn ? Pawn->GetPlayerState<AHodgePlayerState>() : nullptr;
	if (Candidate.PlayerState.Get() == State && Candidate.AttributeReadyHandle.IsValid()) { return; }
	if (Candidate.PlayerState.IsValid()) { Candidate.PlayerState->OnAttributeReadinessChanged.Remove(Candidate.AttributeReadyHandle); }
	Candidate.AttributeReadyHandle.Reset();
	Candidate.PlayerState = State;
	if (!State) { return; }
	const FGuid Epoch = InitialAreaEpoch;
	const FGuid Identity = Candidate.Identity;
	const TWeakObjectPtr<APawn> WeakPawn = Candidate.Pawn;
	Candidate.AttributeReadyHandle = State->OnAttributeReadinessChanged.AddWeakLambda(this, [this, Epoch, WeakPawn, Identity]()
	{ HandleFirstAreaCandidateReady(Epoch, WeakPawn, Identity); });
}

void AHodgeEncounterSpawner::HandleFirstAreaCandidateReady(FGuid Epoch, TWeakObjectPtr<APawn> Pawn, FGuid Identity)
{
	const auto* Candidate = InitialAreaCandidates.Find(Pawn);
	if (!CanFirstAreaCatchup() || InitialAreaEpoch != Epoch || !Candidate || Candidate->Identity != Identity) { return; }
	RetryFirstAreaCandidates(Epoch);
}

void AHodgeEncounterSpawner::RetryFirstAreaCandidates(FGuid Epoch)
{
	if (!CanFirstAreaCatchup() || InitialAreaEpoch != Epoch || bCheckingInitialAreaCandidates) { return; }
	TGuardValue<bool> CheckingGuard(bCheckingInitialAreaCandidates, true);
	RegisterFirstAreaExperience();
	TArray<TWeakObjectPtr<APawn>> Keys;
	InitialAreaCandidates.GetKeys(Keys);
	for (const auto& Key : Keys)
	{
		if (!CanFirstAreaCatchup() || InitialAreaEpoch != Epoch) { return; }
		auto* Candidate = InitialAreaCandidates.Find(Key);
		if (!Candidate) { continue; }
		APawn* Pawn = Candidate->Pawn.Get();
		const auto* Health = Candidate->Health.Get();
		if (!IsValid(Pawn) || Pawn->IsActorBeingDestroyed() || Pawn->GetWorld() != GetWorld() || !TriggerVolume->IsOverlappingActor(Pawn) ||
			(Health && Health->IsDeadOrDying()))
		{
			RemoveFirstAreaCandidate(Key);
			AreaOccupants.Remove(Key);
			ArmedExitEntrants.Remove(Key);
			continue;
		}
		BindFirstAreaCandidateAttributes(*Candidate);
		const auto* BoundState = Candidate->PlayerState.Get();
		const auto* BoundPC = Pawn ? Cast<AHodgePlayerController>(Pawn->GetController()) : nullptr;
		const auto* BoundASC = BoundState ? BoundState->GetHodgeAbilitySystemComponent() : nullptr;
		const bool bCurrentAvatar = BoundPC && BoundPC->GetPawn() == Pawn && BoundPC->GetHodgePlayerState() == BoundState &&
			BoundASC && BoundASC->GetOwnerActor() == BoundState && BoundASC->GetAvatarActor() == Pawn;
		if (Candidate->bObservedAvatarBinding && !bCurrentAvatar)
		{
			RemoveFirstAreaCandidate(Key);
			ArmedExitEntrants.Remove(Key);
			continue;
		}
		Candidate->bObservedAvatarBinding |= bCurrentAvatar;
		AHodgePlayerController* PC = nullptr;
		AHodgePlayerState* State = nullptr;
		FString Error;
		if (!ValidatePlayer(Pawn, PC, State, Error) || !IsExperienceReady()) { continue; }
		if (!Definition->Validate(Error))
		{
			Snapshot.LastFailure = Error;
			RetireFirstAreaCatchup();
			UE_LOG(LogTemp, Error, TEXT("Encounter %s first area configuration: %s"), *GetName(), *Error);
			Publish();
			return;
		}
		if (Definition->TriggerEdge == EHodgeEncounterTriggerEdge::Exit)
		{
			// 就绪只能补齐合法进入见证，离开必须由之后真实的 EndOverlap 提供。
			ArmedExitEntrants.Add(Pawn);
			RemoveFirstAreaCandidate(Key);
			continue;
		}
		const auto Result = TryStartEncounter(Pawn);
		if (Result.bSucceeded) { return; }
	}
}

void AHodgeEncounterSpawner::RemoveFirstAreaCandidate(TWeakObjectPtr<APawn> Pawn)
{
	FInitialAreaCandidate Removed;
	if (!InitialAreaCandidates.RemoveAndCopyValue(Pawn, Removed)) { return; }
	// 只在解绑时读取正在销毁的原对象；不允许它参与新的启动判断。
	APawn* Original = Removed.Pawn.GetEvenIfUnreachable();
	if (Removed.InitManager.IsValid() && Original && Removed.InitStateHandle.IsValid())
	{ Removed.InitManager->UnregisterActorInitStateDelegate(Original, Removed.InitStateHandle); }
	if (Removed.PlayerState.IsValid()) { Removed.PlayerState->OnAttributeReadinessChanged.Remove(Removed.AttributeReadyHandle); }
	if (Removed.Health.IsValid()) { Removed.Health->OnDeathStarted.RemoveDynamic(this, &ThisClass::HandleInitialAreaCandidateDeath); }
	if (Original) { Original->OnEndPlay.RemoveDynamic(this, &ThisClass::HandleInitialAreaCandidateEndPlay); }
}

void AHodgeEncounterSpawner::RetireFirstAreaCatchup()
{
	bInitialAreaCatchupAvailable = false;
	InitialAreaEpoch = FGuid::NewGuid();
	if (GetWorld()) { GetWorld()->GameStateSetEvent.Remove(InitialAreaGameStateHandle); }
	InitialAreaGameStateHandle.Reset();
	InitialAreaExperience.Reset();
	TArray<TWeakObjectPtr<APawn>> Keys;
	InitialAreaCandidates.GetKeys(Keys);
	for (const auto& Key : Keys) { RemoveFirstAreaCandidate(Key); }
}

void AHodgeEncounterSpawner::HandleInitialAreaCandidateDeath(AActor* Actor)
{
	if (APawn* Pawn = Cast<APawn>(Actor))
	{
		RemoveFirstAreaCandidate(Pawn);
		ArmedExitEntrants.Remove(Pawn);
	}
}

void AHodgeEncounterSpawner::HandleInitialAreaCandidateEndPlay(AActor* Actor, EEndPlayReason::Type Reason)
{
	if (APawn* Pawn = Cast<APawn>(Actor))
	{
		RemoveFirstAreaCandidate(Pawn);
		AreaOccupants.Remove(Pawn);
		ArmedExitEntrants.Remove(Pawn);
	}
}

void AHodgeEncounterSpawner::HandleAreaBegin(UPrimitiveComponent* Component, AActor* Actor, UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority() || !bRuntimeReady || bClosing || !Definition || Definition->Mode != EHodgeEncounterMode::Area) { return; }
	APawn* Pawn = Cast<AHodgeHeroCharacter>(Actor);
	if (!Pawn || AreaOccupants.Contains(Pawn)) { return; }
	AreaOccupants.Add(Pawn);
	if (CanFirstAreaCatchup()) { RegisterFirstAreaCandidate(Pawn); return; }
	if (Snapshot.State == EHodgeEncounterState::Inactive && !Snapshot.bNeedsExit)
	{
		ArmedExitEntrants.Add(Pawn);
		if (Definition->TriggerEdge == EHodgeEncounterTriggerEdge::Enter) { TryStartEncounter(Pawn); }
	}
}

void AHodgeEncounterSpawner::HandleAreaEnd(UPrimitiveComponent* Component, AActor* Actor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	if (!HasAuthority() || !bRuntimeReady || bClosing || !Definition || Definition->Mode != EHodgeEncounterMode::Area) { return; }
	APawn* Pawn = Cast<AHodgeHeroCharacter>(Actor);
	if (!Pawn || TriggerVolume->IsOverlappingActor(Pawn) || !AreaOccupants.Contains(Pawn)) { return; }
	const bool bWasArmed = Snapshot.State == EHodgeEncounterState::Inactive && !Snapshot.bNeedsExit;
	const bool bEnteredAfterArming = ArmedExitEntrants.Remove(Pawn) > 0;
	AreaOccupants.Remove(Pawn);
	RemoveFirstAreaCandidate(Pawn);
	TryRearmArea();
	// 完成本轮离开的事件只负责重新武装，不能同时当作下一轮 Exit 触发。
	if (bWasArmed && bEnteredAfterArming && Snapshot.State == EHodgeEncounterState::Inactive && Definition->TriggerEdge == EHodgeEncounterTriggerEdge::Exit)
	{ TryStartEncounter(Pawn); }
}

void AHodgeEncounterSpawner::BindOwner(AHodgePlayerController* PC, APawn* Pawn)
{
	OwnerController = PC;
	PC->OnEndPlay.AddUniqueDynamic(this, &ThisClass::HandleOwnerEndPlay);
	PC->OnPossessedPawnChanged.AddUniqueDynamic(this, &ThisClass::HandleOwnerPawnChanged);
	if (Snapshot.OwnerPlayerState) { Snapshot.OwnerPlayerState->OnEndPlay.AddUniqueDynamic(this, &ThisClass::HandleOwnerEndPlay); }
	BindOwnerPawn(Pawn);
}

void AHodgeEncounterSpawner::BindOwnerPawn(APawn* Pawn)
{
	if (OwnerHealth.IsValid()) { OwnerHealth->OnDeathStarted.RemoveDynamic(this, &ThisClass::HandleOwnerDeath); }
	if (OwnerPawn.IsValid()) { OwnerPawn->OnEndPlay.RemoveDynamic(this, &ThisClass::HandleOwnerPawnEndPlay); }
	OwnerPawn = Pawn;
	OwnerHealth = UHodgeHealthComponent::FindHealthComponent(Pawn);
	if (OwnerHealth.IsValid()) { OwnerHealth->OnDeathStarted.AddUniqueDynamic(this, &ThisClass::HandleOwnerDeath); }
	if (Pawn) { Pawn->OnEndPlay.AddUniqueDynamic(this, &ThisClass::HandleOwnerPawnEndPlay); }
}

void AHodgeEncounterSpawner::UnbindOwner()
{
	BindOwnerPawn(nullptr);
	if (OwnerController.IsValid())
	{
		OwnerController->OnEndPlay.RemoveDynamic(this, &ThisClass::HandleOwnerEndPlay);
		OwnerController->OnPossessedPawnChanged.RemoveDynamic(this, &ThisClass::HandleOwnerPawnChanged);
	}
	if (Snapshot.OwnerPlayerState) { Snapshot.OwnerPlayerState->OnEndPlay.RemoveDynamic(this, &ThisClass::HandleOwnerEndPlay); }
	OwnerController.Reset();
}

void AHodgeEncounterSpawner::HandleOwnerDeath(AActor* Actor)
{
	APawn* Pawn = Cast<APawn>(Actor);
	if (Pawn && OwnerPawn.HasSameIndexAndSerialNumber(TWeakObjectPtr<APawn>(Pawn)) &&
		(Snapshot.State == EHodgeEncounterState::Active || Snapshot.State == EHodgeEncounterState::Spawning))
	{ FailEncounter(TEXT("触发玩家在战斗中死亡，本轮已终止")); }
}

void AHodgeEncounterSpawner::HandleOwnerEndPlay(AActor* Actor, EEndPlayReason::Type Reason)
{
	auto* PC = Cast<AHodgePlayerController>(Actor);
	// EndPlay 中弱引用可能已不可解引用，使用序列身份避免把两个失效对象当成同一个。
	if ((PC && OwnerController.HasSameIndexAndSerialNumber(TWeakObjectPtr<AHodgePlayerController>(PC))) || Actor == Snapshot.OwnerPlayerState)
	{ FailEncounter(TEXT("触发玩家断线或离开世界，本轮已放弃")); }
}

void AHodgeEncounterSpawner::HandleOwnerPawnEndPlay(AActor* Actor, EEndPlayReason::Type Reason)
{
	APawn* Pawn = Cast<APawn>(Actor);
	if (!Pawn || !OwnerPawn.HasSameIndexAndSerialNumber(TWeakObjectPtr<APawn>(Pawn))) { return; }
	if (Snapshot.State == EHodgeEncounterState::Active || Snapshot.State == EHodgeEncounterState::Spawning)
	{ FailEncounter(TEXT("触发玩家的战斗 Pawn 已离开世界")); }
	else { BindOwnerPawn(nullptr); }
}

void AHodgeEncounterSpawner::HandleOwnerPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (bClosing || (Snapshot.State != EHodgeEncounterState::Active && Snapshot.State != EHodgeEncounterState::Spawning &&
		Snapshot.State != EHodgeEncounterState::AwaitingReward) || !OwnerController.IsValid()) { return; }
	if (OwnerController->GetHodgePlayerState() != Snapshot.OwnerPlayerState)
	{ FailEncounter(TEXT("触发玩家的 PlayerState 身份已改变")); return; }
	const bool bOldMatches = OldPawn && OwnerPawn.HasSameIndexAndSerialNumber(TWeakObjectPtr<APawn>(OldPawn));
	const bool bNewMatches = NewPawn && OwnerController->GetPawn() == NewPawn && NewPawn->GetController() == OwnerController.Get();
	// 委托广播期间可能重入新轮次，旧 Controller 的换身回调不能解绑新拥有者。
	if (OwnerController->GetPawn() != NewPawn || (!bOldMatches && !bNewMatches)) { return; }
	BindOwnerPawn(NewPawn);
}

void AHodgeEncounterSpawner::Publish()
{
	if (!HasAuthority() || bClosing) { return; }
	Snapshot.Revision = Snapshot.Revision == MAX_int32 ? 0 : Snapshot.Revision + 1;
	ApplyPresentation();
	ForceNetUpdate();
}

void AHodgeEncounterSpawner::ConfigureComponents()
{
	const bool bArea = Definition && Definition->Mode == EHodgeEncounterMode::Area;
	const bool bValidExtent = bArea && !Definition->AreaHalfExtent.ContainsNaN() &&
		Definition->AreaHalfExtent.X > 0.0 && Definition->AreaHalfExtent.Y > 0.0 && Definition->AreaHalfExtent.Z > 0.0;
	if (bValidExtent) { TriggerVolume->SetBoxExtent(Definition->AreaHalfExtent); }
	TriggerVolume->SetCollisionEnabled(bValidExtent ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void AHodgeEncounterSpawner::ApplyPresentation()
{
	const bool bVisible = Definition && Definition->Mode == EHodgeEncounterMode::Interaction &&
		(Snapshot.State == EHodgeEncounterState::Inactive || Snapshot.State == EHodgeEncounterState::AwaitingReward);
	SwitchMesh->SetVisibility(bVisible, true);
	SwitchMesh->SetCollisionEnabled(bVisible ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	SwitchPrompt->SetVisibility(bVisible, true);
	if (GetNetMode() != NM_DedicatedServer && HasActorBegunPlay())
	{
		SwitchPrompt->InitWidget();
		if (auto* Widget = Cast<UHodgeEncounterPromptWidget>(SwitchPrompt->GetUserWidgetObject()))
		{ Widget->SetPrompt(FText::FromString(Snapshot.State == EHodgeEncounterState::AwaitingReward ? TEXT("[F] 领取奖励") : TEXT("[F] 启动挑战"))); }
	}
}

void AHodgeEncounterSpawner::OnRep_Snapshot() { ApplyPresentation(); }
void AHodgeEncounterSpawner::OnRep_Definition() { ConfigureComponents(); ApplyPresentation(); }
