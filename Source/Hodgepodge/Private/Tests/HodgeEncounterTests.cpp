/** @file HodgeEncounterTests.cpp @brief 两种入口共用波次、异常退休、独占领奖与区域重新武装的原生回归。 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "HodgeEncounterTestTypes.h"
#include "HodgeEnemyAITestTypes.h"
#include "HodgeEnemyAITestSupport.h"
#include "Encounter/HodgeEncounterSpawner.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"
#include "AbilitySystem/Stats/HodgeAttributeCoordinator.h"
#include "Component/HodgeHealthComponent.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Core/PlayerController/HodgePlayerController.h"
#include "Core/PlayState/HodgePlayerState.h"
#include "Core/GameState/HodgeGameState.h"
#include "Data/HodgeEncounterDefinition.h"
#include "Data/HodgePawnData.h"
#include "Data/HodgeCharacterStatProfile.h"
#include "Character/HodgeHeroCharacter.h"
#include "Equipment/HodgeEquipmentManagerComponent.h"
#include "Inventory/HodgeInventoryManagerComponent.h"
#include "Inventory/HodgeEquipmentSelectionComponent.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/WorldSettings.h"
#include "TimerManager.h"

/** 只隔离外部 Experience 等待并投递真实回调，不代替成员登记和波次推进。 */
struct FHodgeEncounterTestAccess
{
	static void WithoutExperience(AHodgeEncounterSpawner* Spawner) { Spawner->bRequireExperience = false; }
	static void OldEvaluation(AHodgeEncounterSpawner* Spawner, FGuid RunId, int32 WaveIndex)
	{ Spawner->EvaluateWave(RunId, WaveIndex); }
	static void Death(AHodgeEncounterSpawner* Spawner, AActor* Enemy) { Spawner->HandleEnemyDeath(Enemy); }
	static void EnemyRemoved(AHodgeEncounterSpawner* Spawner, AActor* Enemy)
	{ Spawner->HandleEnemyEndPlay(Enemy, EEndPlayReason::Destroyed); }
	static void InitializationTimeout(AHodgeEncounterSpawner* Spawner, FGuid RunId, int32 WaveIndex)
	{ Spawner->HandleInitializationTimeout(RunId, WaveIndex); }
	static void OwnerLeft(AHodgeEncounterSpawner* Spawner, AActor* Controller)
	{ Spawner->HandleOwnerEndPlay(Controller, EEndPlayReason::LevelTransition); }
	/** 仅使服务器时间门槛到期，离开门禁与新边沿仍走实际运行逻辑。 */
	static void CooldownElapsed(AHodgeEncounterSpawner* Spawner)
	{
		Spawner->Snapshot.CooldownEndServerTime = Spawner->GetWorld()->GetTimeSeconds() - 1.0;
		Spawner->TryRearmArea();
	}
	static void BeginOverlap(AHodgeEncounterSpawner* Spawner, APawn* Pawn, UPrimitiveComponent* OtherComponent, int32 BodyIndex)
	{ Spawner->HandleAreaBegin(Spawner->TriggerVolume, Pawn, OtherComponent, BodyIndex, false, FHitResult()); }
	static void EndOverlap(AHodgeEncounterSpawner* Spawner, APawn* Pawn, UPrimitiveComponent* OtherComponent, int32 BodyIndex)
	{ Spawner->HandleAreaEnd(Spawner->TriggerVolume, Pawn, OtherComponent, BodyIndex); }
	static void RefreshOccupants(AHodgeEncounterSpawner* Spawner) { Spawner->RefreshAreaOccupants(); }
	static UBoxComponent* Area(AHodgeEncounterSpawner* Spawner) { return Spawner->TriggerVolume; }
};

namespace HodgeEncounterTests
{
	/** 仅本组启用真实世界初始化，保留外部 Experience 未加载的隔离边界。 */
	UWorld* CreateInitializedWorld(UGameInstance*& Instance)
	{
		auto* World = FHodgeEnemyInitTestAccess::CreateWorld(Instance);
		if (!World->GetPhysicsScene()) { World->CreatePhysicsScene(); }
		// 纯引擎 GameMode 不执行正式登录或 Experience 加载，Hodge GameState 满足 PlayerState 原生依赖。
		World->GetWorldSettings()->DefaultGameMode = AGameModeBase::StaticClass();
		check(World->SetGameMode(FURL()));
		auto* GameMode = World->GetAuthGameMode();
		GameMode->GameStateClass = AHodgeGameState::StaticClass();
		GameMode->PlayerStateClass = AHodgePlayerState::StaticClass();
		World->InitializeActorsForPlay(FURL());
		return World;
	}

	/** 配置使用原生普通敌人与物品类，测试不改正式资产或 CDO。 */
	UHodgeEncounterDefinition* MakeDefinition(UObject* Outer, EHodgeEncounterMode Mode = EHodgeEncounterMode::Interaction,
		int32 WaveCount = 1, int32 EnemiesPerWave = 1)
	{
		auto* Definition = NewObject<UHodgeEncounterDefinition>(Outer);
		Definition->Mode = Mode;
		Definition->RewardDefinition = UHodgeEncounterTestReward::StaticClass();
		auto* Data = NewObject<UHodgePawnData>(Definition);
		Data->PawnClass = AHodgeEncounterTestEnemy::StaticClass();
		Data->StatProfile = NewObject<UHodgeCharacterStatProfile>(Data);
		Data->StatProfile->MaxHealth = FScalableFloat(100.f);
		Data->StatProfile->BaseDamage = FScalableFloat(10.f);
		for (int32 WaveIndex = 0; WaveIndex < WaveCount; ++WaveIndex)
		{
			FHodgeEncounterWave Wave;
			for (int32 EnemyIndex = 0; EnemyIndex < EnemiesPerWave; ++EnemyIndex)
			{
				FHodgeEncounterSpawn Spawn;
				Spawn.PawnData = Data;
				Spawn.RelativeTransform.SetLocation(FVector(500.f + 200.f * EnemyIndex, 300.f * WaveIndex, 100.f));
				Wave.Enemies.Add(Spawn);
			}
			Definition->Waves.Add(Wave);
		}
		return Definition;
	}

	/** 真实 PlayerState 库存和属性协调器绑定，换身体时仍保留同一玩家身份。 */
	struct FPlayer
	{
		UWorld* World;
		AHodgePlayerState* PS;
		AHodgePlayerController* PC;
		AHodgeHeroCharacter* Pawn = nullptr;
		UHodgeAbilitySystemComponent* ASC;
		UHodgePawnData* Data;
		UHodgeInventoryManagerComponent* Inventory;
		FVector SpawnLocation;

		FPlayer(UWorld* InWorld, FVector Location = FVector(0.f, 0.f, 100.f)) : World(InWorld), SpawnLocation(Location)
		{
			PC = World->SpawnActor<AHodgePlayerController>();
			PS = PC->GetHodgePlayerState();
			check(PS);
			PS->SetOwner(PC);
			ASC = PS->GetHodgeAbilitySystemComponent();
			if (!ASC->GetSet<UHodgeHealthSet>()) { ASC->AddAttributeSetSubobject(CastChecked<UHodgeHealthSet>(PS->GetDefaultSubobjectByName(TEXT("HealthSet")))); }
			if (!ASC->GetSet<UHodgeCombatSet>()) { ASC->AddAttributeSetSubobject(CastChecked<UHodgeCombatSet>(PS->GetDefaultSubobjectByName(TEXT("CombatSet")))); }
			ASC->InitAbilityActorInfo(PS, nullptr);
			Inventory = NewObject<UHodgeInventoryManagerComponent>(PS);
			PS->AddInstanceComponent(Inventory);
			Inventory->RegisterComponent();
			auto* Selection = NewObject<UHodgeEquipmentSelectionComponent>(PS);
			PS->AddInstanceComponent(Selection);
			Selection->RegisterComponent();
			Data = NewObject<UHodgePawnData>(PS);
			Data->StatProfile = NewObject<UHodgeCharacterStatProfile>(Data);
			Data->bUseInventory = true;
			Data->InventoryMaxEntries = 1;
			PS->SetPawnData(Data);
			NewPawn(false);
		}

		/** 重生只换 Avatar，库存、PlayerState 和 ASC 仍使用原对象。 */
		void NewPawn(bool bRespawn = true)
		{
			if (IsValid(Pawn)) { Pawn->FindComponentByClass<UHodgePawnExtensionComponent>()->UninitializeAbilitySystem(); }
			if (bRespawn) { PS->GetAttributeCoordinator()->ExpectRespawn(); }
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			Pawn = World->SpawnActor<AHodgeHeroCharacter>(SpawnLocation, FRotator::ZeroRotator, Params);
			auto* Equipment = NewObject<UHodgeEquipmentManagerComponent>(Pawn);
			Pawn->AddInstanceComponent(Equipment);
			Equipment->RegisterComponent();
			auto* Extension = Pawn->FindComponentByClass<UHodgePawnExtensionComponent>();
			Extension->SetPawnData(Data);
			PC->Possess(Pawn);
			Extension->InitializeAbilitySystem(ASC, PS);
		}

		bool IsReady() const { return PS->AreAttributesReadyFor(Pawn) && Inventory->IsInitialized(); }
	};

	/** 波次和回收使用真实 Timer；不改 Alive/Pending、成员状态或运行身份。 */
	struct FFixture
	{
		UGameInstance* Instance = nullptr;
		UWorld* World = CreateInitializedWorld(Instance);
		FPlayer Owner{World};
		AHodgeEncounterSpawner* Spawner;
		UHodgeEncounterDefinition* Definition;

		FFixture(EHodgeEncounterMode Mode = EHodgeEncounterMode::Interaction, int32 WaveCount = 1, int32 EnemiesPerWave = 1,
			bool bBeginPlay = true)
		{
			FHodgeEncounterEnemyTestControl::Reset();
			Spawner = World->SpawnActor<AHodgeEncounterSpawner>();
			if (Mode == EHodgeEncounterMode::Area) { Spawner->SetActorLocation(FVector(4000.f, 0.f, 0.f)); }
			Definition = MakeDefinition(Spawner, Mode, WaveCount, EnemiesPerWave);
			Spawner->Definition = Definition;
			FHodgeEncounterTestAccess::WithoutExperience(Spawner);
			// 世界与 Actor 都走真实 BeginPlay，动态 UFUNCTION 和物理重叠不依赖编辑器脚本例外。
			if (bBeginPlay) { World->BeginPlay(); }
		}

		~FFixture()
		{
			// 结束夹具走真实世界清理路径，避免把正常拆卸伪造为外部取消错误。
			if (IsValid(Spawner)) { Spawner->Destroy(); }
			FHodgeEncounterEnemyTestControl::Reset();
			FHodgeEnemyInitTestAccess::DestroyWorld(Instance, World);
		}

		void Pump(int32 Frames = 4, float Step = .02f) const
		{
			for (int32 Frame = 0; Frame < Frames; ++Frame)
			{
				++GFrameCounter;
				World->GetTimerManager().Tick(Step);
			}
		}

		FHodgeInteractionResult Start() const
		{
			const auto Result = Spawner->TryStartEncounter(Owner.Pawn);
			Pump();
			return Result;
		}

		/** 同一批使用开始时的真实成员列表，避免顺手清掉刚推进生成的下一批。 */
		void KillCurrentWave() const
		{
			const auto Enemies = Spawner->GetLivingEnemies();
			for (auto* Enemy : Enemies)
			{
				Enemy->FindComponentByClass<UHodgeHealthComponent>()->StartDeath();
			}
			Pump();
		}

		/** 先更新真实几何重叠，再补投同一标准边沿；自动通知已到达时会自然去重。 */
		void EnterArea(FPlayer& Player, FVector Offset = FVector(0.f, 0.f, 100.f)) const
		{
			Player.Pawn->SetActorLocation(Spawner->GetActorLocation() + Offset, false, nullptr, ETeleportType::TeleportPhysics);
			Player.Pawn->GetCapsuleComponent()->UpdateOverlaps();
			FHodgeEncounterTestAccess::Area(Spawner)->UpdateOverlaps();
			FHodgeEncounterTestAccess::BeginOverlap(Spawner, Player.Pawn, Player.Pawn->GetCapsuleComponent(), 0);
			Pump();
		}

		void LeaveArea(FPlayer& Player) const
		{
			Player.Pawn->SetActorLocation(Spawner->GetActorLocation() + FVector(1000.f, 0.f, 100.f), false, nullptr, ETeleportType::TeleportPhysics);
			Player.Pawn->GetCapsuleComponent()->UpdateOverlaps();
			FHodgeEncounterTestAccess::Area(Spawner)->UpdateOverlaps();
			FHodgeEncounterTestAccess::EndOverlap(Spawner, Player.Pawn, Player.Pawn->GetCapsuleComponent(), 0);
			Pump();
		}
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEncounterWaveTest, "Hodge.Encounter.Waves.DeathStartedAndDuplicate", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeEncounterWaveTest::RunTest(const FString&)
{
	HodgeEncounterTests::FFixture F(EHodgeEncounterMode::Interaction, 2, 2);
	F.Definition->Waves[1].DelayBeforeSpawn = .25f;
	TestTrue(TEXT("夹具世界已真实完成初始化和 BeginPlay"), F.World->AreActorsInitialized() && F.World->HasBegunPlay());
	if (!TestTrue(TEXT("真实玩家属性与库存绑定就绪"), F.Owner.IsReady())) { return false; }
	if (!TestTrue(TEXT("启动请求被接受"), F.Start().bSucceeded)) { return false; }
	auto State = F.Spawner->GetSnapshot();
	TestEqual(TEXT("首波索引为零"), State.WaveIndex, 0);
	TestEqual(TEXT("首波全部真实生成"), State.AliveCount, 2);
	TestEqual(TEXT("真实 EnemyInit 和 Brain 已就绪"), State.PendingCount, 0);
	const auto RunId = State.RunId;
	TestFalse(TEXT("已有运行时拒绝第二次启动"), F.Spawner->TryStartEncounter(F.Owner.Pawn).bSucceeded);
	TestTrue(TEXT("重复启动不更换本轮身份"), F.Spawner->GetSnapshot().RunId == RunId);
	const auto FirstWave = F.Spawner->GetLivingEnemies();
	if (!TestEqual(TEXT("服务器公开成员列表完整"), FirstWave.Num(), 2)) { return false; }
	FirstWave[0]->FindComponentByClass<UHodgeHealthComponent>()->StartDeath();
	F.Pump();
	TestEqual(TEXT("死亡开始立即减少存活，不等尸体销毁"), F.Spawner->GetSnapshot().AliveCount, 1);
	FHodgeEncounterTestAccess::Death(F.Spawner, FirstWave[0]);
	TestEqual(TEXT("同一死亡回调只能扣减一次"), F.Spawner->GetSnapshot().AliveCount, 1);
	FirstWave[1]->FindComponentByClass<UHodgeHealthComponent>()->StartDeath();
	F.Pump();
	TestEqual(TEXT("下一波等待期间保持 Active"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Active);
	TestEqual(TEXT("等待时间未到不提前生成"), FHodgeEncounterEnemyTestControl::ConstructionCount, 2);
	F.Pump(1, .3f);
	State = F.Spawner->GetSnapshot();
	TestEqual(TEXT("清空后才生成第二波"), State.WaveIndex, 1);
	TestEqual(TEXT("第二波独立完整生成"), State.AliveCount, 2);
	TestEqual(TEXT("总共只生成两波四个成员"), FHodgeEncounterEnemyTestControl::ConstructionCount, 4);
	const int32 Revision = State.Revision;
	FHodgeEncounterTestAccess::OldEvaluation(F.Spawner, RunId, 0);
	TestEqual(TEXT("上一波旧回调不修改当前修订"), F.Spawner->GetSnapshot().Revision, Revision);
	F.KillCurrentWave();
	State = F.Spawner->GetSnapshot();
	TestEqual(TEXT("所有波结束进入待领取"), State.State, EHodgeEncounterState::AwaitingReward);
	TestTrue(TEXT("领取权属于启动者 PlayerState"), State.OwnerPlayerState == F.Owner.PS);
	TestEqual(TEXT("完成后存活为零"), State.AliveCount, 0);
	TestEqual(TEXT("完成后等待为零"), State.PendingCount, 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEncounterSynchronousDeathTest, "Hodge.Encounter.Waves.DeathDuringDeferredConstruction", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeEncounterSynchronousDeathTest::RunTest(const FString&)
{
	HodgeEncounterTests::FFixture F(EHodgeEncounterMode::Interaction, 2, 2);
	FHodgeEncounterEnemyTestControl::Mode = EHodgeEncounterEnemyTestMode::DeathDuringFirstConstruction;
	if (!TestTrue(TEXT("同步死亡场景启动成功"), F.Start().bSucceeded)) { return false; }
	const auto State = F.Spawner->GetSnapshot();
	TestEqual(TEXT("首个构造中的死亡没有提前推进下一波"), State.WaveIndex, 0);
	TestEqual(TEXT("生成闸门保护本波后续成员"), FHodgeEncounterEnemyTestControl::ConstructionCount, 2);
	TestEqual(TEXT("同步死亡只扣当前已登记成员"), State.AliveCount, 1);
	TestEqual(TEXT("合法死亡与就绪各自结束等待"), State.PendingCount, 0);
	F.KillCurrentWave();
	TestEqual(TEXT("剩余成员死亡后才推进"), F.Spawner->GetSnapshot().WaveIndex, 1);
	TestEqual(TEXT("第二波正常生成两个存活成员"), F.Spawner->GetSnapshot().AliveCount, 2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEncounterSpawnFailureTest, "Hodge.Encounter.Failures.FirstMidAndDeathThenDestroyed", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeEncounterSpawnFailureTest::RunTest(const FString&)
{
	AddExpectedError(TEXT(" failed:"), EAutomationExpectedErrorFlags::Contains, 5);
	for (int32 FailureIndex = 0; FailureIndex < 2; ++FailureIndex)
	{
		HodgeEncounterTests::FFixture F(EHodgeEncounterMode::Interaction, 1, 2);
		auto* EntryData = DuplicateObject<UHodgePawnData>(F.Definition->Waves[0].Enemies[FailureIndex].PawnData, F.Definition);
		F.Definition->Waves[0].Enemies[FailureIndex].PawnData = EntryData;
		TestTrue(TEXT("首个或中途原生生成失败前配置通过受理"), F.Spawner->TryStartEncounter(F.Owner.Pawn).bSucceeded);
		// 模拟受理后、下一帧生成前类依赖失效，让真正 SpawnActorDeferred 返回 nullptr。
		EntryData->PawnClass = nullptr;
		F.Pump();
		TestEqual(TEXT("原生空生成结果中止轮次"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive);
		TestEqual(TEXT("空生成结果不会继续后续条目"), FHodgeEncounterEnemyTestControl::ConstructionCount, FailureIndex);
		TestTrue(TEXT("首个或中途空生成回收已生成成员"), F.Spawner->GetLivingEnemies().IsEmpty());
	}
	for (int32 Case = 0; Case < 3; ++Case)
	{
		HodgeEncounterTests::FFixture F(EHodgeEncounterMode::Interaction, 1, 2);
		FHodgeEncounterEnemyTestControl::Mode = Case == 2 ? EHodgeEncounterEnemyTestMode::DeathThenDestroyDuringConstruction
			: EHodgeEncounterEnemyTestMode::DestroyDuringConstruction;
		FHodgeEncounterEnemyTestControl::FailureOrdinal = Case == 1 ? 2 : 1;
		TestTrue(TEXT("失败发生前请求仍是合法启动"), F.Start().bSucceeded);
		const auto State = F.Spawner->GetSnapshot();
		TestEqual(TEXT("构造失败停止本轮而非伪装待领取"), State.State, EHodgeEncounterState::Inactive);
		TestFalse(TEXT("保留生成失败原因"), State.LastFailure.IsEmpty());
		TestEqual(TEXT("失败后的存活计数清零"), State.AliveCount, 0);
		TestEqual(TEXT("失败后的等待计数清零"), State.PendingCount, 0);
		TestTrue(TEXT("失败后没有可遗留存活成员"), F.Spawner->GetLivingEnemies().IsEmpty());
		for (const auto& Enemy : FHodgeEncounterEnemyTestControl::ConstructedEnemies)
		{
			TestFalse(TEXT("首个或中途失败回收已经生成的存活敌人"), Enemy.IsValid());
		}
		F.Pump();
		TestEqual(TEXT("迟到生成事件不能恢复失败轮次"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEncounterInitializationFailureTest, "Hodge.Encounter.Failures.InitializationBrainAndTimeout", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeEncounterInitializationFailureTest::RunTest(const FString&)
{
	AddExpectedError(TEXT(" failed:"), EAutomationExpectedErrorFlags::Contains, 3);
	for (int32 Case = 0; Case < 3; ++Case)
	{
		HodgeEncounterTests::FFixture F;
		if (Case == 0)
		{
			// 合法静态配置在真实 OnGive 中解绑，覆盖初始化运行失败而非只测静态预检。
			auto* Set = NewObject<UHodgeEnemyTestAbilitySet>(F.Definition);
			Set->AddAbility(UHodgeEnemyRebindOnGrantAbility::StaticClass());
			F.Definition->Waves[0].Enemies[0].PawnData->AbilitySets.Add(Set);
		}
		else
		{
			FHodgeEncounterEnemyTestControl::Mode = Case == 1 ? EHodgeEncounterEnemyTestMode::InvalidBrain : EHodgeEncounterEnemyTestMode::WaitForCombat;
		}
		TestTrue(TEXT("运行异常场景接受启动"), F.Start().bSucceeded);
		if (Case == 2)
		{
			const auto Waiting = F.Spawner->GetSnapshot();
			TestTrue(TEXT("未就绪成员保留 Pending"), Waiting.PendingCount > 0);
			TestEqual(TEXT("等待依赖不能直接清波"), Waiting.State, EHodgeEncounterState::Spawning);
			FHodgeEncounterTestAccess::InitializationTimeout(F.Spawner, Waiting.RunId, Waiting.WaveIndex);
		}
		const auto State = F.Spawner->GetSnapshot();
		TestEqual(TEXT("初始化／Brain／超时失败均终止轮次"), State.State, EHodgeEncounterState::Inactive);
		TestFalse(TEXT("失败必须返回实际原因"), State.LastFailure.IsEmpty());
		TestEqual(TEXT("异常不得伪装成清空后待领取"), State.PendingCount, 0);
		TestTrue(TEXT("异常回收当前敌人"), F.Spawner->GetLivingEnemies().IsEmpty());
	}
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEncounterUnexpectedRemovalTest, "Hodge.Encounter.Failures.UnexpectedRemovalAndRetiredCallbacks", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeEncounterUnexpectedRemovalTest::RunTest(const FString&)
{
	AddExpectedError(TEXT(" failed:"), EAutomationExpectedErrorFlags::Contains, 2);
	HodgeEncounterTests::FFixture F(EHodgeEncounterMode::Interaction, 1, 2);
	if (!TestTrue(TEXT("清理专项启动"), F.Start().bSucceeded)) { return false; }
	const auto Previous = F.Spawner->GetSnapshot();
	const auto Enemies = F.Spawner->GetLivingEnemies();
	if (!TestEqual(TEXT("准备两个实际成员"), Enemies.Num(), 2)) { return false; }
	FHodgeEncounterTestAccess::EnemyRemoved(F.Spawner, Enemies[0]);
	TestEqual(TEXT("非死亡移除属于失败"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive);
	TestTrue(TEXT("失败没有进入奖励结算"), F.Owner.Inventory->GetItems().IsEmpty());
	TestFalse(TEXT("剩余成员也被回收"), IsValid(Enemies[1]));
	if (!TestTrue(TEXT("失败后可以显式开始新交互轮次"), F.Start().bSucceeded)) { return false; }
	const auto Current = F.Spawner->GetSnapshot();
	TestTrue(TEXT("新轮次使用新身份"), Current.RunId != Previous.RunId);
	FHodgeEncounterTestAccess::OldEvaluation(F.Spawner, Previous.RunId, Previous.WaveIndex);
	FHodgeEncounterTestAccess::InitializationTimeout(F.Spawner, Previous.RunId, Previous.WaveIndex);
	TestEqual(TEXT("旧轮次回调不修改新轮次修订"), F.Spawner->GetSnapshot().Revision, Current.Revision);
	F.Spawner->ResetEncounter();
	F.Pump();
	TestEqual(TEXT("显式取消先退休再回收，不会生成奖励"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive);
	TestTrue(TEXT("显式取消后成员为空"), F.Spawner->GetLivingEnemies().IsEmpty());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEncounterRewardTest, "Hodge.Encounter.Reward.OwnerAndAtomicInventory", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeEncounterRewardTest::RunTest(const FString&)
{
	HodgeEncounterTests::FFixture F;
	if (!TestTrue(TEXT("奖励专项真实玩家就绪"), F.Owner.IsReady()) || !TestTrue(TEXT("奖励专项启动"), F.Start().bSucceeded)) { return false; }
	F.KillCurrentWave();
	const auto Awaiting = F.Spawner->GetSnapshot();
	if (!TestEqual(TEXT("真实清场进入待领取"), Awaiting.State, EHodgeEncounterState::AwaitingReward)) { return false; }
	HodgeEncounterTests::FPlayer Peer(F.World, FVector(100.f, 100.f, 100.f));
	TestFalse(TEXT("非拥有者不能领取"), F.Spawner->ExecuteInteraction(Peer.Pawn, Awaiting.RunId, Awaiting.Revision).bSucceeded);
	TestEqual(TEXT("其他玩家没有得到奖励"), Peer.Inventory->GetTotalItemCountByDefinition(UHodgeEncounterTestReward::StaticClass()), int64(0));
	TestTrue(TEXT("真实库存占满唯一条目"), F.Owner.Inventory->AddItemDefinition(UHodgeEncounterTestOtherItem::StaticClass(), 1).Succeeded());
	TestFalse(TEXT("满库存使领取原子失败"), F.Spawner->ExecuteInteraction(F.Owner.Pawn, Awaiting.RunId, Awaiting.Revision).bSucceeded);
	TestEqual(TEXT("库存失败保留待领取"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::AwaitingReward);
	TestTrue(TEXT("失败仍保留原领取归属"), F.Spawner->GetSnapshot().OwnerPlayerState == F.Owner.PS);
	TestEqual(TEXT("失败不产生奖励物品"), F.Owner.Inventory->GetTotalItemCountByDefinition(UHodgeEncounterTestReward::StaticClass()), int64(0));
	TestTrue(TEXT("腾出真实库存容量"), F.Owner.Inventory->ConsumeItemsByDefinition(UHodgeEncounterTestOtherItem::StaticClass(), 1).Succeeded());
	const auto Retry = F.Spawner->GetSnapshot();
	bool bReentered = false;
	bool bNestedSucceeded = true;
	// 真实库存通知同步重入领取入口，结算闸门必须阻止同轮二次副作用。
	const auto Handle = F.Owner.Inventory->OnInventoryChanged.AddLambda([&](const TArray<FHodgeInventoryChange>&, int64)
	{
		bReentered = true;
		bNestedSucceeded = F.Spawner->ExecuteInteraction(F.Owner.Pawn, Retry.RunId, Retry.Revision).bSucceeded;
	});
	TestTrue(TEXT("拥有者重新领取成功"), F.Spawner->ExecuteInteraction(F.Owner.Pawn, Retry.RunId, Retry.Revision).bSucceeded);
	F.Owner.Inventory->OnInventoryChanged.Remove(Handle);
	TestTrue(TEXT("领取实际触发库存变更通知"), bReentered);
	TestFalse(TEXT("库存通知内的重复领取被拒绝"), bNestedSucceeded);
	TestEqual(TEXT("领取成功立即恢复初始"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive);
	TestEqual(TEXT("奖励只提交一份"), F.Owner.Inventory->GetTotalItemCountByDefinition(UHodgeEncounterTestReward::StaticClass()), int64(1));
	TestFalse(TEXT("原领取请求重复到达被拒绝"), F.Spawner->ExecuteInteraction(F.Owner.Pawn, Retry.RunId, Retry.Revision).bSucceeded);
	TestEqual(TEXT("重复请求不增发奖励"), F.Owner.Inventory->GetTotalItemCountByDefinition(UHodgeEncounterTestReward::StaticClass()), int64(1));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEncounterOwnerLifecycleTest, "Hodge.Encounter.Owner.DeathRespawnAndDisconnect", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeEncounterOwnerLifecycleTest::RunTest(const FString&)
{
	AddExpectedError(TEXT(" failed:"), EAutomationExpectedErrorFlags::Contains, 3);
	{
		HodgeEncounterTests::FFixture F;
		if (!TestTrue(TEXT("拥有者死亡专项实际启动"), F.Start().bSucceeded)) { return false; }
		F.Owner.Pawn->FindComponentByClass<UHodgeHealthComponent>()->StartDeath();
		F.Pump();
		TestEqual(TEXT("战斗中拥有者死亡中止本轮"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive);
		TestTrue(TEXT("死亡失败回收战斗成员"), F.Spawner->GetLivingEnemies().IsEmpty());
	}
	{
		HodgeEncounterTests::FFixture F;
		if (!TestTrue(TEXT("拥有者离开专项实际启动"), F.Start().bSucceeded)) { return false; }
		FHodgeEncounterTestAccess::OwnerLeft(F.Spawner, F.Owner.PC);
		TestEqual(TEXT("战斗中 Controller 离开中止本轮"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive);
	}
	{
		HodgeEncounterTests::FFixture F;
		if (!TestTrue(TEXT("待领取重生专项实际启动"), F.Start().bSucceeded)) { return false; }
		F.KillCurrentWave();
		F.Owner.Pawn->FindComponentByClass<UHodgeHealthComponent>()->StartDeath();
		TestEqual(TEXT("待领取死亡保留奖励状态"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::AwaitingReward);
		TestTrue(TEXT("待领取归属仍为原 PlayerState"), F.Spawner->GetSnapshot().OwnerPlayerState == F.Owner.PS);
		F.Owner.NewPawn();
		if (!TestTrue(TEXT("重生后实际 PS／ASC／Health 绑定就绪"), F.Owner.IsReady())) { return false; }
		const auto State = F.Spawner->GetSnapshot();
		TestTrue(TEXT("同一 PlayerState 的新身体可以领取"), F.Spawner->ExecuteInteraction(F.Owner.Pawn, State.RunId, State.Revision).bSucceeded);
		TestEqual(TEXT("新身体领取仍只发一份"), F.Owner.Inventory->GetTotalItemCountByDefinition(UHodgeEncounterTestReward::StaticClass()), int64(1));
	}
	{
		HodgeEncounterTests::FFixture F;
		if (!TestTrue(TEXT("待领取断线专项实际启动"), F.Start().bSucceeded)) { return false; }
		F.KillCurrentWave();
		FHodgeEncounterTestAccess::OwnerLeft(F.Spawner, F.Owner.PC);
		TestEqual(TEXT("待领取断线放弃本轮"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive);
		TestTrue(TEXT("断线不自动发奖"), F.Owner.Inventory->GetItems().IsEmpty());
	}
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEncounterAreaRepeatTest, "Hodge.Encounter.Area.CooldownAllExitAndNewEdge", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeEncounterAreaRepeatTest::RunTest(const FString&)
{
	HodgeEncounterTests::FFixture F(EHodgeEncounterMode::Area);
	if (!TestTrue(TEXT("区域触发玩家真实就绪"), F.Owner.IsReady())) { return false; }
	F.EnterArea(F.Owner);
	if (!TestEqual(TEXT("真实 Enter 启动区域波次"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Spawning)) { return false; }
	const auto RunId = F.Spawner->GetSnapshot().RunId;
	auto* ExtraShape = NewObject<USphereComponent>(F.Owner.Pawn);
	F.Owner.Pawn->AddInstanceComponent(ExtraShape);
	ExtraShape->SetupAttachment(F.Owner.Pawn->GetRootComponent());
	ExtraShape->InitSphereRadius(20.f);
	ExtraShape->SetCollisionObjectType(ECC_Pawn);
	ExtraShape->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ExtraShape->SetCollisionResponseToAllChannels(ECR_Overlap);
	ExtraShape->SetGenerateOverlapEvents(true);
	ExtraShape->RegisterComponent();
	FHodgeEncounterTestAccess::BeginOverlap(F.Spawner, F.Owner.Pawn, ExtraShape, 1);
	TestTrue(TEXT("同一 Pawn 多组件重叠只创建一个轮次"), F.Spawner->GetSnapshot().RunId == RunId);
	TestEqual(TEXT("多组件不会重复刷怪"), FHodgeEncounterEnemyTestControl::ConstructionCount, 1);
	ExtraShape->SetRelativeLocation(FVector(1000.f, 0.f, 0.f));
	FHodgeEncounterTestAccess::EndOverlap(F.Spawner, F.Owner.Pawn, ExtraShape, 1);
	TestTrue(TEXT("单个附加组件离开不视为 Pawn 离开"), F.Spawner->GetSnapshot().RunId == RunId);
	HodgeEncounterTests::FPlayer Peer(F.World, FVector(2000.f, 2000.f, 100.f));
	F.EnterArea(Peer, FVector(100.f, 100.f, 100.f));
	F.LeaveArea(F.Owner);
	TestEqual(TEXT("普通离开区域不中止战斗"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Spawning);
	F.EnterArea(F.Owner);
	F.KillCurrentWave();
	const auto Complete = F.Spawner->GetSnapshot();
	TestEqual(TEXT("区域清场自动完成"), Complete.State, EHodgeEncounterState::Completed);
	TestTrue(TEXT("完成后需要所有重叠玩家离开"), Complete.bNeedsExit);
	TestTrue(TEXT("采用正式三十秒冷却"), Complete.CooldownEndServerTime - F.World->GetTimeSeconds() >= 29.99);
	TestTrue(TEXT("区域清场没有奖励提交"), F.Owner.Inventory->GetItems().IsEmpty() && Peer.Inventory->GetItems().IsEmpty());
	FHodgeEncounterTestAccess::CooldownElapsed(F.Spawner);
	TestEqual(TEXT("仅冷却到期不会在区域内重开"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Completed);
	F.LeaveArea(F.Owner);
	TestEqual(TEXT("还有其他玩家时继续等待离开"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Completed);
	F.LeaveArea(Peer);
	TestEqual(TEXT("冷却到期且全员离开后才重新武装"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive);
	TestEqual(TEXT("重新武装事件本身不生成新波次"), FHodgeEncounterEnemyTestControl::ConstructionCount, 1);
	F.Pump();
	TestEqual(TEXT("没有新边沿时保持未激活"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive);
	F.EnterArea(F.Owner);
	TestEqual(TEXT("新的 Enter 才开始下一轮"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Spawning);
	TestEqual(TEXT("下一轮实际新增一个成员"), FHodgeEncounterEnemyTestControl::ConstructionCount, 2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEncounterAreaExitTest, "Hodge.Encounter.Area.ExitRearmCannotStartOnSameEdge", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeEncounterAreaExitTest::RunTest(const FString&)
{
	HodgeEncounterTests::FFixture F(EHodgeEncounterMode::Area);
	F.Definition->TriggerEdge = EHodgeEncounterTriggerEdge::Exit;
	F.EnterArea(F.Owner);
	TestEqual(TEXT("Exit 模式进入时只登记"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive);
	F.LeaveArea(F.Owner);
	if (!TestEqual(TEXT("首个合法 Exit 启动"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Spawning)) { return false; }
	F.EnterArea(F.Owner);
	F.KillCurrentWave();
	TestEqual(TEXT("真实清场完成本轮"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Completed);
	FHodgeEncounterTestAccess::CooldownElapsed(F.Spawner);
	F.LeaveArea(F.Owner);
	TestEqual(TEXT("完成后的最后离开只重新武装"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive);
	TestEqual(TEXT("同一个 Exit 不同时启动下一轮"), FHodgeEncounterEnemyTestControl::ConstructionCount, 1);
	F.EnterArea(F.Owner);
	TestEqual(TEXT("重新进入不替代所需 Exit"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive);
	F.LeaveArea(F.Owner);
	TestEqual(TEXT("再次形成 Exit 后才启动"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Spawning);
	TestEqual(TEXT("第二个轮次仅新生成一次"), FHodgeEncounterEnemyTestControl::ConstructionCount, 2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEncounterAreaSingleTest, "Hodge.Encounter.Area.SingleSuccessAndFailureRetry", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeEncounterAreaSingleTest::RunTest(const FString&)
{
	AddExpectedError(TEXT(" failed:"), EAutomationExpectedErrorFlags::Contains, 1);
	{
		HodgeEncounterTests::FFixture F(EHodgeEncounterMode::Area);
		F.Definition->bRepeatable = false;
		F.EnterArea(F.Owner);
		F.KillCurrentWave();
		TestEqual(TEXT("一次性正常清场永久完成"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Completed);
		F.LeaveArea(F.Owner);
		FHodgeEncounterTestAccess::CooldownElapsed(F.Spawner);
		F.EnterArea(F.Owner);
		TestEqual(TEXT("一次性成功后新边沿也不重开"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Completed);
		TestEqual(TEXT("成功机会仅实际执行一次"), FHodgeEncounterEnemyTestControl::ConstructionCount, 1);
	}
	{
		HodgeEncounterTests::FFixture F(EHodgeEncounterMode::Area);
		F.Definition->bRepeatable = false;
		FHodgeEncounterEnemyTestControl::Mode = EHodgeEncounterEnemyTestMode::DestroyDuringConstruction;
		F.EnterArea(F.Owner);
		TestFalse(TEXT("失败保留原因而非消耗成功机会"), F.Spawner->GetSnapshot().LastFailure.IsEmpty());
		TestEqual(TEXT("失败仍进入冷却门禁"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Completed);
		FHodgeEncounterEnemyTestControl::Mode = EHodgeEncounterEnemyTestMode::Normal;
		FHodgeEncounterTestAccess::CooldownElapsed(F.Spawner);
		TestEqual(TEXT("失败后也必须离开区域"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Completed);
		F.LeaveArea(F.Owner);
		TestEqual(TEXT("失败后冷却加离开重新武装"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive);
		F.EnterArea(F.Owner);
		TestEqual(TEXT("一次性配置失败仍允许重新挑战"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Spawning);
		F.KillCurrentWave();
		TestEqual(TEXT("重新挑战成功后才永久完成"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Completed);
		F.LeaveArea(F.Owner);
		FHodgeEncounterTestAccess::CooldownElapsed(F.Spawner);
		F.EnterArea(F.Owner);
		TestEqual(TEXT("成功后不再恢复为可触发"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Completed);
	}
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEncounterInvalidationDeathOrderTest, "Hodge.Encounter.Failures.InvalidationBeforeDeathCannotClear", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeEncounterInvalidationDeathOrderTest::RunTest(const FString&)
{
	AddExpectedError(TEXT(" failed:"), EAutomationExpectedErrorFlags::Contains, 1);
	{
		HodgeEncounterTests::FFixture F;
		if (!TestTrue(TEXT("真实成员先完成初始化"), F.Start().bSucceeded)) { return false; }
		const auto Enemies = F.Spawner->GetLivingEnemies();
		if (!TestEqual(TEXT("准备一个实际成员"), Enemies.Num(), 1)) { return false; }
		auto* Enemy = Enemies[0];
		auto* Health = Enemy->FindComponentByClass<UHodgeHealthComponent>();
		// 同帧先使真实 ASC 绑定失效；不推进 Timer，也不手改刷怪成员状态。
		Enemy->FindComponentByClass<UHodgePawnExtensionComponent>()->UninitializeAbilitySystem();
		TestEqual(TEXT("初始化失效立即终止轮次"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive);
		Health->StartDeath();
		F.Pump();
		TestEqual(TEXT("同帧迟到死亡不能洗成合法清波"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive);
		TestFalse(TEXT("保留真实失效原因"), F.Spawner->GetSnapshot().LastFailure.IsEmpty());
		TestTrue(TEXT("失效后不进入奖励提交"), F.Owner.Inventory->GetItems().IsEmpty());
	}
	{
		HodgeEncounterTests::FFixture F(EHodgeEncounterMode::Interaction, 1, 2);
		if (!TestTrue(TEXT("正常死亡先发生的对照轮次启动"), F.Start().bSucceeded)) { return false; }
		const auto Enemies = F.Spawner->GetLivingEnemies();
		if (!TestEqual(TEXT("对照准备两个实际成员"), Enemies.Num(), 2)) { return false; }
		Enemies[0]->FindComponentByClass<UHodgeHealthComponent>()->StartDeath();
		Enemies[0]->FindComponentByClass<UHodgePawnExtensionComponent>()->UninitializeAbilitySystem();
		F.Pump();
		TestEqual(TEXT("已确认死亡后的解绑不误杀本轮"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Spawning);
		TestEqual(TEXT("已确认死亡只保留另一名存活成员"), F.Spawner->GetSnapshot().AliveCount, 1);
		F.KillCurrentWave();
		TestEqual(TEXT("正常死亡清理仍可完成清波"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::AwaitingReward);
	}
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEncounterAreaExitCooldownEntryTest, "Hodge.Encounter.Area.ExitRequiresEntryAfterRearm", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeEncounterAreaExitCooldownEntryTest::RunTest(const FString&)
{
	HodgeEncounterTests::FFixture F(EHodgeEncounterMode::Area);
	F.Definition->TriggerEdge = EHodgeEncounterTriggerEdge::Exit;
	F.EnterArea(F.Owner);
	F.LeaveArea(F.Owner);
	if (!TestEqual(TEXT("Exit 专项首轮实际启动"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Spawning)) { return false; }
	F.EnterArea(F.Owner);
	F.KillCurrentWave();
	F.LeaveArea(F.Owner);
	TestEqual(TEXT("全员离开但冷却未到时仍完成等待"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Completed);
	TestFalse(TEXT("完成后的全员离开门槛已经满足"), F.Spawner->GetSnapshot().bNeedsExit);
	HodgeEncounterTests::FPlayer Peer(F.World, FVector(2000.f, 2000.f, 100.f));
	F.EnterArea(Peer);
	TestEqual(TEXT("冷却期间进入不能启动"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Completed);
	FHodgeEncounterTestAccess::CooldownElapsed(F.Spawner);
	TestEqual(TEXT("既有离开门槛满足后冷却可重新武装"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive);
	F.LeaveArea(Peer);
	TestEqual(TEXT("冷却期间旧进入产生的 Exit 不算新一轮边沿"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive);
	TestEqual(TEXT("旧进入不能增刷成员"), FHodgeEncounterEnemyTestControl::ConstructionCount, 1);
	F.EnterArea(Peer);
	F.LeaveArea(Peer);
	TestEqual(TEXT("重新武装后真正 Enter 再 Exit 才启动"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Spawning);
	TestEqual(TEXT("新边沿仅创建一个下一轮成员"), FHodgeEncounterEnemyTestControl::ConstructionCount, 2);
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEncounterPlayerAuthorityTest, "Hodge.Encounter.Authority.WorldAndAttributeReadiness", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeEncounterPlayerAuthorityTest::RunTest(const FString&)
{
	HodgeEncounterTests::FFixture F;
	HodgeEncounterTests::FFixture Foreign;
	if (!TestTrue(TEXT("本地与另一 World 的玩家均真实就绪"), F.Owner.IsReady() && Foreign.Owner.IsReady())) { return false; }
	TestFalse(TEXT("跨 World 的合法玩家也不能启动本刷怪点"), F.Spawner->TryStartEncounter(Foreign.Owner.Pawn).bSucceeded);
	TestEqual(TEXT("跨 World 请求不改变状态"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive);
	// 通过真实协调器解除当前 Avatar，就绪失败不以改写测试标志模拟。
	F.Owner.PS->GetAttributeCoordinator()->DetachAvatar(F.Owner.Pawn);
	TestFalse(TEXT("夹具真实属性就绪已经退休"), F.Owner.PS->AreAttributesReadyFor(F.Owner.Pawn));
	TestTrue(TEXT("未就绪场景仍保留正生命与当前 ASC Avatar"), F.Owner.ASC->GetAvatarActor() == F.Owner.Pawn &&
		F.Owner.Pawn->FindComponentByClass<UHodgeHealthComponent>()->GetHealth() > 0.f);
	TestFalse(TEXT("正生命但属性未就绪仍拒绝启动"), F.Spawner->TryStartEncounter(F.Owner.Pawn).bSucceeded);
	FHodgeInteractionOption Option;
	TestFalse(TEXT("属性未就绪不暴露可提交交互选项"), F.Spawner->GetInteractionOption(F.Owner.Pawn, Option));
	{
		HodgeEncounterTests::FFixture Area(EHodgeEncounterMode::Area);
		if (!TestTrue(TEXT("区域玩家原本实际就绪"), Area.Owner.IsReady())) { return false; }
		Area.Owner.PS->GetAttributeCoordinator()->DetachAvatar(Area.Owner.Pawn);
		TestFalse(TEXT("区域公共启动也服从属性就绪合同"), Area.Spawner->TryStartEncounter(Area.Owner.Pawn).bSucceeded);
		TestEqual(TEXT("未就绪区域请求不生成成员"), FHodgeEncounterEnemyTestControl::ConstructionCount, 0);
	}
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEncounterInitialInsideTest, "Hodge.Encounter.Area.InitialReadyOccupantCatchUp", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeEncounterInitialInsideTest::RunTest(const FString&)
{
	HodgeEncounterTests::FFixture F(EHodgeEncounterMode::Area, 1, 1, false);
	if (!TestTrue(TEXT("初始区域内玩家真实就绪"), F.Owner.IsReady())) { return false; }
	// Box 启用前玩家已在出生区域内，不投递测试侧 Enter 来替代首次重叠发现。
	F.Owner.Pawn->SetActorLocation(F.Spawner->GetActorLocation() + FVector(0.f, 0.f, 100.f), false, nullptr, ETeleportType::TeleportPhysics);
	F.Owner.Pawn->GetCapsuleComponent()->UpdateOverlaps();
	F.World->BeginPlay();
	F.Pump();
	TestTrue(TEXT("初始扫描确有真实 Box 重叠"), FHodgeEncounterTestAccess::Area(F.Spawner)->IsOverlappingActor(F.Owner.Pawn));
	TestEqual(TEXT("初始已在区域内也补一次首轮 Enter"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Spawning);
	TestEqual(TEXT("初始重叠只生成一轮成员"), FHodgeEncounterEnemyTestControl::ConstructionCount, 1);
	TestTrue(TEXT("补触发仍记录真实玩家身份"), F.Spawner->GetSnapshot().OwnerPlayerState == F.Owner.PS);
	F.Owner.PS->NotifyAttributeReadiness();
	F.Pump();
	TestEqual(TEXT("重复就绪通知不重复生成首轮"), FHodgeEncounterEnemyTestControl::ConstructionCount, 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEncounterUnreadyInsideTest, "Hodge.Encounter.Area.ReadinessRecoveryWhileInside", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeEncounterUnreadyInsideTest::RunTest(const FString&)
{
	HodgeEncounterTests::FFixture F(EHodgeEncounterMode::Area);
	auto* Extension = F.Owner.Pawn->FindComponentByClass<UHodgePawnExtensionComponent>();
	Extension->UninitializeAbilitySystem();
	if (!TestFalse(TEXT("真实 Pawn 解绑已使属性未就绪"), F.Owner.PS->AreAttributesReadyFor(F.Owner.Pawn))) { return false; }
	F.EnterArea(F.Owner);
	TestNull(TEXT("等待期间 Pawn 的 ASC 仍未重新绑定"), Extension->GetHodgeAbilitySystemComponent());
	TestFalse(TEXT("实际 Timer 后仍保持未就绪前提"), F.Owner.PS->AreAttributesReadyFor(F.Owner.Pawn));
	TestTrue(TEXT("未就绪玩家实际仍在 Box 内"), FHodgeEncounterTestAccess::Area(F.Spawner)->IsOverlappingActor(F.Owner.Pawn));
	TestEqual(TEXT("未就绪入区只等待，不提前开始"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive);
	TestEqual(TEXT("等待期间不生成成员"), FHodgeEncounterEnemyTestControl::ConstructionCount, 0);
	// 公开重绑同一 ASC 会驱动角色与真实属性协调器，通知仍由生产 Timer 发布。
	Extension->InitializeAbilitySystem(F.Owner.ASC, F.Owner.PS);
	if (!TestTrue(TEXT("同一 Pawn 的真实属性已恢复"), F.Owner.IsReady())) { return false; }
	F.Pump();
	TestEqual(TEXT("就绪且仍在 Box 内补首轮 Enter"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Spawning);
	TestEqual(TEXT("恢复补触发只生成一次"), FHodgeEncounterEnemyTestControl::ConstructionCount, 1);
	TestTrue(TEXT("仍使用原 Pawn 和 PlayerState 受理"), F.Spawner->GetSnapshot().OwnerPlayerState == F.Owner.PS && F.Owner.ASC->GetAvatarActor() == F.Owner.Pawn);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEncounterUnreadyLeaveTest, "Hodge.Encounter.Area.LeaveBeforeReadinessDoesNotCatchUp", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeEncounterUnreadyLeaveTest::RunTest(const FString&)
{
	HodgeEncounterTests::FFixture F(EHodgeEncounterMode::Area);
	auto* Extension = F.Owner.Pawn->FindComponentByClass<UHodgePawnExtensionComponent>();
	Extension->UninitializeAbilitySystem();
	F.EnterArea(F.Owner);
	TestNull(TEXT("未就绪进入后尚未重新绑定 ASC"), Extension->GetHodgeAbilitySystemComponent());
	TestFalse(TEXT("离开前真实属性仍未就绪"), F.Owner.PS->AreAttributesReadyFor(F.Owner.Pawn));
	TestEqual(TEXT("未就绪进入时仍为初始状态"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive);
	F.LeaveArea(F.Owner);
	TestFalse(TEXT("恢复之前已经真实离开 Box"), FHodgeEncounterTestAccess::Area(F.Spawner)->IsOverlappingActor(F.Owner.Pawn));
	Extension->InitializeAbilitySystem(F.Owner.ASC, F.Owner.PS);
	if (!TestTrue(TEXT("区域外同一 Pawn 真实恢复就绪"), F.Owner.IsReady())) { return false; }
	F.Pump();
	F.Owner.PS->NotifyAttributeReadiness();
	F.Pump();
	TestEqual(TEXT("离开后的迟到就绪不补触发"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive);
	TestEqual(TEXT("区域外就绪没有生成敌人"), FHodgeEncounterEnemyTestControl::ConstructionCount, 0);
	F.EnterArea(F.Owner);
	TestEqual(TEXT("新的真实 Enter 仍可正常启动首轮"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Spawning);
	TestEqual(TEXT("正常重入只生成一个成员"), FHodgeEncounterEnemyTestControl::ConstructionCount, 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEncounterCatchUpConsumedTest, "Hodge.Encounter.Area.FirstCatchUpCannotBypassReentry", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeEncounterCatchUpConsumedTest::RunTest(const FString&)
{
	AddExpectedError(TEXT(" failed:"), EAutomationExpectedErrorFlags::Contains, 1);
	for (int32 Case = 0; Case < 2; ++Case)
	{
		HodgeEncounterTests::FFixture F(EHodgeEncounterMode::Area);
		if (Case == 1) { FHodgeEncounterEnemyTestControl::Mode = EHodgeEncounterEnemyTestMode::DestroyDuringConstruction; }
		F.EnterArea(F.Owner);
		if (Case == 0) { F.KillCurrentWave(); }
		if (!TestEqual(TEXT("成功或失败均进入首轮结束门禁"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Completed)) { return false; }
		FHodgeEncounterEnemyTestControl::Mode = EHodgeEncounterEnemyTestMode::Normal;
		F.Owner.PS->NotifyAttributeReadiness();
		F.Pump();
		TestEqual(TEXT("首轮结束的旧就绪通知不越过冷却"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Completed);
		F.LeaveArea(F.Owner);
		F.EnterArea(F.Owner);
		TestEqual(TEXT("冷却内重入仍然等待"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Completed);
		FHodgeEncounterTestAccess::CooldownElapsed(F.Spawner);
		if (!TestEqual(TEXT("冷却与结束后离开门槛满足才重武装"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive)) { return false; }
		F.Owner.PS->NotifyAttributeReadiness();
		F.Pump();
		TestTrue(TEXT("就绪通知发生时角色仍真实在 Box 内"), FHodgeEncounterTestAccess::Area(F.Spawner)->IsOverlappingActor(F.Owner.Pawn));
		TestEqual(TEXT("首轮资格已消费，旧就绪回调不能替代新边沿"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive);
		TestEqual(TEXT("首轮后没有被补触发增刷"), FHodgeEncounterEnemyTestControl::ConstructionCount, 1);
		F.LeaveArea(F.Owner);
		F.EnterArea(F.Owner);
		TestEqual(TEXT("完整新边沿仍可启动下一轮"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Spawning);
		TestEqual(TEXT("下一轮仅新增一个成员"), FHodgeEncounterEnemyTestControl::ConstructionCount, 2);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEncounterExitCatchUpTest, "Hodge.Encounter.Area.FirstCatchUpExitRequiresRealExit", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeEncounterExitCatchUpTest::RunTest(const FString&)
{
	for (int32 Case = 0; Case < 2; ++Case)
	{
		HodgeEncounterTests::FFixture F(EHodgeEncounterMode::Area, 1, 1, false);
		F.Definition->TriggerEdge = EHodgeEncounterTriggerEdge::Exit;
		if (Case == 0)
		{
			// 初次已在区域内的 Exit 配置只能建立进入见证，不能直接开始挑战。
			F.Owner.Pawn->SetActorLocation(F.Spawner->GetActorLocation() + FVector(0.f, 0.f, 100.f), false, nullptr, ETeleportType::TeleportPhysics);
			F.Owner.Pawn->GetCapsuleComponent()->UpdateOverlaps();
			F.World->BeginPlay();
			F.Pump();
		}
		else
		{
			F.World->BeginPlay();
			auto* Extension = F.Owner.Pawn->FindComponentByClass<UHodgePawnExtensionComponent>();
			Extension->UninitializeAbilitySystem();
			F.EnterArea(F.Owner);
			TestFalse(TEXT("Exit 进入后保持真实未就绪"), F.Owner.PS->AreAttributesReadyFor(F.Owner.Pawn));
			TestEqual(TEXT("Exit 未就绪入区不提前开始"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive);
			Extension->InitializeAbilitySystem(F.Owner.ASC, F.Owner.PS);
			if (!TestTrue(TEXT("Exit 恢复场景实际重新就绪"), F.Owner.IsReady())) { return false; }
			F.Pump();
		}
		TestTrue(TEXT("Exit 首轮见证来自真实 Box 内玩家"), FHodgeEncounterTestAccess::Area(F.Spawner)->IsOverlappingActor(F.Owner.Pawn));
		TestEqual(TEXT("首次补资格不会把 Exit 改成 Enter 触发"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Inactive);
		TestEqual(TEXT("真实离开之前不生成敌人"), FHodgeEncounterEnemyTestControl::ConstructionCount, 0);
		F.LeaveArea(F.Owner);
		TestEqual(TEXT("随后真正 Exit 才启动首轮"), F.Spawner->GetSnapshot().State, EHodgeEncounterState::Spawning);
		TestEqual(TEXT("真正 Exit 只启动一次"), FHodgeEncounterEnemyTestControl::ConstructionCount, 1);
	}
	return true;
}
#endif
