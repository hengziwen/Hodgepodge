/** @file HodgeEnemyAITests.cpp
 * @brief 实现敌人AI自动化测试相关的类型、接口与现有处理流程。
 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "HodgeEnemyAITestSupport.h"
#include "HodgeEnemyAITestTypes.h"
#include "HodgeRelationshipTestAbility.h"
#include "AI/HodgeEnemyAIController.h"
#include "AI/HodgeBTTask_ExecuteAbility.h"
#include "Character/HodgeEnemyCharacter.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Component/HodgeCombatComponentBase.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"
#include "Data/HodgePawnData.h"
#include "Data/HodgeCharacterStatProfile.h"
#include "Equipment/HodgeEquipmentManagerComponent.h"
#include "Equipment/HodgeEquipmentInstance.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/Composites/BTComposite_Sequence.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Combat/HodgeHitReactionTypes.h"

/** 保存敌人AI测试Access的数据和接口契约。 */
struct FHodgeEnemyAITestAccess
{
	/** 准备pare。
	 * @param Task 任务，由调用方传入。
	 * @param Controller 当前角色使用的控制器。
	 * @param ASC 后续GAS操作使用的能力系统组件。
	 */
	static void Prepare(UHodgeBTTask_ExecuteAbility* Task, AHodgeEnemyAIController* Controller, UHodgeAbilitySystemComponent* ASC)
	{
		Task->Controller = Controller; Task->Pawn = Controller->GetPawn(); Task->ASC = ASC;
		Task->Combat = Controller->GetPawn()->FindComponentByClass<UHodgeCombatComponentBase>();
		Task->PossessGeneration = Controller->GetPossessGeneration(); Task->AttemptId = 2;
		Task->RequestId = FGuid::NewGuid();
	}
	/** 把指定结果发送到当前测试观察入口。
	 * @param Task 任务，由调用方传入。
	 * @param Attempt 结果所属的尝试序号，用于排除旧回调。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	static bool Deliver(UHodgeBTTask_ExecuteAbility* Task, uint32 Attempt)
	{
		FHodgeCombatActionRequestState State; State.RequestId = Task->RequestId;
		State.Status = EHodgeCombatActionRequestStatus::Completed;
		Task->HandleRequestChanged(Attempt, State); return Task->bEnded;
	}
	/** 模拟旧尝试身份对应的迟到结果。
	 * @param Task 任务，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	static bool DeliverOld(UHodgeBTTask_ExecuteAbility* Task) { return Deliver(Task, 1); }
	/** 模拟角色解绑后到达的执行结果。
	 * @param Task 任务，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	static bool DeliverDetached(UHodgeBTTask_ExecuteAbility* Task) { return Deliver(Task, 2); }
	/** 模拟技能清空执行身份后的正常结束结果。
	 * @param Task 任务，由调用方传入。
	 * UHodgeAbilitySystemComponent参数；用于保持该接口的调用签名。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	static bool DeliverClearedExecution(UHodgeBTTask_ExecuteAbility* Task, UHodgeAbilitySystemComponent*)
	{ return Deliver(Task, 2); }
};

namespace HodgeEnemyTests
{
	/** 创建用于初始化测试的等待行为树。
	 * @return 本次计算、查询或创建得到的行为树。
	 */
	UBehaviorTree* WaitTree()
	{
		auto* Tree = NewObject<UBehaviorTree>(); Tree->BlackboardAsset = NewObject<UBlackboardData>(Tree);
		auto* Root = NewObject<UBTComposite_Sequence>(Tree); auto* Wait = NewObject<UBTTask_Wait>(Tree);
		Wait->WaitTime = 90.f; FBTCompositeChild Child; Child.ChildTask = Wait; Root->Children.Add(Child);
		Root->InitializeNode(nullptr, 0, 0, 0); Wait->InitializeNode(Root, 1, 0, 1); Root->InitializeComposite(1);
		Tree->RootNode = Root; return Tree;
	}
	/** 保存测试场景的数据和接口契约。 */
	struct FFixture
	{
		// Instance的数据或对象引用。
		UGameInstance* Instance = nullptr;
		// 本次操作所在的世界。
		UWorld* World;
		// 当前操作关联的Pawn角色。
		AHodgeEnemyCharacter* Pawn;
		// 当前角色使用的控制器。
		AHodgeEnemyAIController* Controller;
		// 当前角色的初始化协调组件。
		UHodgeEnemyInitComponent* Init;
		// 后续GAS操作使用的能力系统组件。
		UHodgeAbilitySystemComponent* ASC;
		// 当前操作使用的数据。
		UHodgePawnData* Data;
		/** 构造测试场景并初始化其默认状态。 */
		FFixture()
		{
			World = FHodgeEnemyInitTestAccess::CreateWorld(Instance);
			Pawn = World->SpawnActor<AHodgeEnemyCharacter>();
			Controller = Cast<AHodgeEnemyAIController>(Pawn->GetController());
			// 先检查对象与上下文，避免在依赖失效后继续处理。
			if (!Controller) { Controller = World->SpawnActor<AHodgeEnemyAIController>(); Controller->Possess(Pawn); }
			Controller->BehaviorTree = WaitTree();
			Init = Pawn->GetEnemyInitComponent(); ASC = Pawn->GetNativeEnemyAbilitySystem();
			Pawn->FindComponentByClass<UHodgePawnExtensionComponent>()->TryToChangeInitState(HodgeGameplayTags::InitState_Spawned);
			Data = NewObject<UHodgePawnData>(); Data->StatProfile = NewObject<UHodgeCharacterStatProfile>(Data);
			Data->StatProfile->MaxHealth = FScalableFloat(350.f); Data->StatProfile->BaseDamage = FScalableFloat(25.f);
			Pawn->EnemyPawnData = Data; FHodgeEnemyInitTestAccess::WithoutExperience(Init);
		}
		/** 添加战斗。 */
		void AddCombat()
		{
			// 统一战斗组件。
			auto* Combat = NewObject<UHodgeCombatComponentBase>(Pawn); Pawn->AddInstanceComponent(Combat); Combat->RegisterComponent();
		}
		/** 添加装备。 */
		void AddEquipment()
		{
			auto* Equipment = NewObject<UHodgeEquipmentManagerComponent>(Pawn); Pawn->AddInstanceComponent(Equipment); Equipment->RegisterComponent();
			if (!Equipment->HasBeenInitialized()) { Equipment->InitializeComponent(); }
		}
		/** 检查测试场景是否已经初始化。
		 * @return true表示本次检查或操作成功，false表示失败。
		 */
		bool Ready()
		{
			AddCombat(); const auto Result = Init->TryInitialize();
			UE_LOG(LogTemp, Display, TEXT("Enemy fixture result=%d diagnostic=%s init=%s extension=%s controller=%s"), int32(Result),
				*Init->GetInitializationDiagnostic(), *Init->GetInitState().ToString(),
				*Pawn->FindComponentByClass<UHodgePawnExtensionComponent>()->GetInitState().ToString(), *GetNameSafe(Pawn->GetController()));
			return Result == EHodgeEnemyInitializationResult::Ready;
		}
		/** 销毁该对象，执行当前类型的析构收尾。 */
		~FFixture()
		{
			// 先检查对象与上下文，避免在依赖失效后继续处理。
			if (IsValid(Controller)) { Controller->UnPossess(); }
			FHodgeEnemyInitTestAccess::DestroyWorld(Instance, World);
		}
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEnemyIdempotencyTest, "Hodge.Enemy.Initialization.Idempotent", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeEnemyIdempotencyTest::RunTest(const FString&)
{
	HodgeEnemyTests::FFixture F;
	auto* Set = NewObject<UHodgeEnemyTestAbilitySet>(); Set->AddAbility(UHodgeRelationshipTestAbility::StaticClass()); F.Data->AbilitySets.Add(Set);
	TestTrue(TEXT("Enemy reaches its own GameplayReady"), F.Ready());
	TestEqual(TEXT("Owner is Pawn"), F.ASC->GetOwnerActor(), static_cast<AActor*>(F.Pawn));
	TestEqual(TEXT("Avatar is Pawn"), F.ASC->GetAvatarActor(), static_cast<AActor*>(F.Pawn));
	const int32 Count = F.ASC->GetActivatableAbilities().Num();
	F.ASC->SetNumericAttributeBase(UHodgeHealthSet::GetHealthAttribute(), 150.f);
	for (int32 I = 0; I < 5; ++I) { F.Init->TryInitialize(); }
	TestEqual(TEXT("Repeated callbacks do not heal"), F.ASC->GetNumericAttribute(UHodgeHealthSet::GetHealthAttribute()), 150.f);
	TestEqual(TEXT("Repeated callbacks do not grant"), F.ASC->GetActivatableAbilities().Num(), Count);
	TestEqual(TEXT("Brain started once"), F.Controller->GetBrainStartCount(), 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEnemyASCConflictTest, "Hodge.Enemy.Initialization.ASCConflict", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeEnemyASCConflictTest::RunTest(const FString&)
{
	HodgeEnemyTests::FFixture F;
	FHodgeEnemyInitTestAccess::AwaitExperience(F.Init);
	auto* Other = NewObject<UHodgeAbilitySystemComponent>(F.Pawn); F.Pawn->AddInstanceComponent(Other); Other->RegisterComponent();
	Other->AddAttributeSetSubobject(NewObject<UHodgeHealthSet>(F.Pawn)); Other->AddAttributeSetSubobject(NewObject<UHodgeCombatSet>(F.Pawn));
	auto* Extension = F.Pawn->FindComponentByClass<UHodgePawnExtensionComponent>();
	Extension->SetPawnData(F.Data); Extension->InitializeAbilitySystem(Other, F.Pawn);
	TestEqual(TEXT("Foreign binding is a conflict failure"), F.Init->TryInitialize(), EHodgeEnemyInitializationResult::Failed);
	TestFalse(TEXT("Foreign binding never reaches ready"), F.Init->IsGameplayReady());
	TestTrue(TEXT("Failure diagnostic identifies ASC"), F.Init->GetInitializationDiagnostic().Contains(TEXT("ASC identity conflict")));
	TestEqual(TEXT("Foreign binding is not silently replaced"), Extension->GetHodgeAbilitySystemComponent(), Other);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEnemyMissingStatsTest, "Hodge.Enemy.Initialization.MissingStatProfile", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeEnemyMissingStatsTest::RunTest(const FString&)
{
	HodgeEnemyTests::FFixture F; F.Data->StatProfile = nullptr;
	TestEqual(TEXT("Missing required stats fails"), F.Init->TryInitialize(), EHodgeEnemyInitializationResult::Failed);
	F.Data->StatProfile = NewObject<UHodgeCharacterStatProfile>();
	TestEqual(TEXT("Permanent failure does not silently retry"), F.Init->TryInitialize(), EHodgeEnemyInitializationResult::Failed);
	TestEqual(TEXT("No grant on failure"), F.ASC->GetActivatableAbilities().Num(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEnemyInvalidLevelTest, "Hodge.Enemy.Initialization.InvalidLevel", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeEnemyInvalidLevelTest::RunTest(const FString&)
{
	HodgeEnemyTests::FFixture F; F.Pawn->InitialLevel = 0;
	TestEqual(TEXT("Invalid stat level is a configuration failure"), F.Init->TryInitialize(), EHodgeEnemyInitializationResult::Failed);
	TestFalse(TEXT("Invalid level cannot start brain"), F.Init->IsGameplayReady()); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEnemyLateDependenciesTest, "Hodge.Enemy.Initialization.LateComponentsAndEquipment", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeEnemyLateDependenciesTest::RunTest(const FString&)
{
	HodgeEnemyTests::FFixture F; F.Data->DefaultWeaponDefinition = UHodgeEnemyTestEquipment::StaticClass();
	TestEqual(TEXT("Required late equipment waits"), F.Init->TryInitialize(), EHodgeEnemyInitializationResult::Waiting);
	F.ASC->SetNumericAttributeBase(UHodgeHealthSet::GetHealthAttribute(), 100.f);
	F.AddEquipment(); F.AddCombat();
	TestEqual(TEXT("Late dependencies complete"), F.Init->TryInitialize(), EHodgeEnemyInitializationResult::Ready);
	TestEqual(TEXT("Late component callback does not heal"), F.ASC->GetNumericAttribute(UHodgeHealthSet::GetHealthAttribute()), 100.f);
	const auto* Equipment = F.Pawn->FindComponentByClass<UHodgeEquipmentManagerComponent>();
	TestTrue(TEXT("Equipment instance and grants ready"), Equipment->IsEquipmentReady(Equipment->FindInstanceOfDefinition(F.Data->DefaultWeaponDefinition)));
	TestTrue(TEXT("Repeated equipment preparation succeeds"), F.Pawn->PrepareDefaultEquipment());
	TestEqual(TEXT("Exactly one equipped instance"), Equipment->GetEquipmentInstancesOfType(UHodgeEquipmentInstance::StaticClass()).Num(), 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEnemyBindingInvalidationTest, "Hodge.Enemy.Initialization.BindingInvalidation", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeEnemyBindingInvalidationTest::RunTest(const FString&)
{
	HodgeEnemyTests::FFixture F;
	auto* Set = NewObject<UHodgeEnemyTestAbilitySet>(); Set->AddAbility(UHodgeRelationshipTestAbility::StaticClass()); F.Data->AbilitySets.Add(Set);
	TestTrue(TEXT("Ready before invalidation"), F.Ready());
	F.Pawn->FindComponentByClass<UHodgePawnExtensionComponent>()->UninitializeAbilitySystem();
	TestFalse(TEXT("Invalidation cannot remain ready"), F.Init->IsGameplayReady());
	TestEqual(TEXT("Invalidated cycle is terminal"), F.Init->TryInitialize(), EHodgeEnemyInitializationResult::Invalidated);
	TestEqual(TEXT("Owned abilities cleaned"), F.ASC->GetActivatableAbilities().Num(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEnemyGrantPreflightTest, "Hodge.Enemy.Grants.Preflight", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeEnemyGrantPreflightTest::RunTest(const FString&)
{
	HodgeEnemyTests::FFixture F; TestTrue(TEXT("Fixture ready"), F.Ready());
	auto* Set = NewObject<UHodgeEnemyTestAbilitySet>(); Set->AddAbility(UHodgeRelationshipTestAbility::StaticClass()); Set->AddAbility(nullptr);
	FHodgeAbilitySet_GrantedHandles Handles;
	TestFalse(TEXT("Invalid batch fails before grant"), Set->TryGiveToAbilitySystem(F.ASC, Handles).bSucceeded);
	TestEqual(TEXT("No partial ability from preflight"), F.ASC->GetActivatableAbilities().Num(), 0);
	auto* SpawnSet = NewObject<UHodgeEnemyTestAbilitySet>(); SpawnSet->AddAbility(UHodgeEnemyOnSpawnTestAbility::StaticClass());
	TestFalse(TEXT("Bootstrap rejects automatic activation"), SpawnSet->TryGiveToAbilitySystem(F.ASC, Handles, nullptr, true).bSucceeded);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEnemyGrantRebindTest, "Hodge.Enemy.Grants.BindingChangedDuringGrant", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeEnemyGrantRebindTest::RunTest(const FString&)
{
	HodgeEnemyTests::FFixture F; TestTrue(TEXT("Fixture ready"), F.Ready());
	auto* Set = NewObject<UHodgeEnemyTestAbilitySet>(); Set->AddAbility(UHodgeEnemyRebindOnGrantAbility::StaticClass());
	FHodgeAbilitySet_GrantedHandles Handles;
	TestFalse(TEXT("Synchronous unbind causes checked grant failure"), Set->TryGiveToAbilitySystem(F.ASC, Handles).bSucceeded);
	TestEqual(TEXT("Pending grant removed after unbind"), F.ASC->GetActivatableAbilities().Num(), 0);
	TestFalse(TEXT("Ready invalidated"), F.Init->IsGameplayReady()); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEnemyEquipmentFailureTest, "Hodge.Enemy.Equipment.NullSetAndActorFailure", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeEnemyEquipmentFailureTest::RunTest(const FString&)
{
	HodgeEnemyTests::FFixture F; TestTrue(TEXT("Fixture ready"), F.Ready()); F.AddEquipment();
	auto* Equipment = F.Pawn->FindComponentByClass<UHodgeEquipmentManagerComponent>();
	auto* CDO = GetMutableDefault<UHodgeEnemyTestEquipment>();
	TGuardValue<TArray<TObjectPtr<const UHodgeAbilitySet>>> SetGuard(CDO->AbilitySetsToGrant, {});
	TGuardValue<TArray<FHodgeEquipmentActorToSpawn>> ActorGuard(CDO->ActorsToSpawn, {});
	CDO->AbilitySetsToGrant.Add(nullptr);
	TestNull(TEXT("Null AbilitySet reports failed equip"), Equipment->EquipItem(UHodgeEnemyTestEquipment::StaticClass()));
	TestFalse(TEXT("Failure diagnostic is available"), Equipment->GetLastEquipError().IsEmpty());
	CDO->AbilitySetsToGrant.Reset();
	FHodgeEquipmentActorToSpawn Good; Good.ActorToSpawn = AHodgeEnemyEquipmentTestActor::StaticClass();
	FHodgeEquipmentActorToSpawn Bad; Bad.ActorToSpawn = AHodgeEnemyEquipmentFailActor::StaticClass();
	CDO->ActorsToSpawn = {Good, Bad};
	TestNull(TEXT("Actor spawn failure does not commit Entry"), Equipment->EquipItem(UHodgeEnemyTestEquipment::StaticClass()));
	TestEqual(TEXT("No failed equipment Entry remains"), Equipment->GetEquipmentInstancesOfType(UHodgeEquipmentInstance::StaticClass()).Num(), 0);
	TestEqual(TEXT("No failed grants remain"), F.ASC->GetActivatableAbilities().Num(), 0); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEnemyBrainLifecycleTest, "Hodge.Enemy.AI.PossessLifecycle", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeEnemyBrainLifecycleTest::RunTest(const FString&)
{
	HodgeEnemyTests::FFixture F; TestTrue(TEXT("Ready"), F.Ready());
	const uint32 OldCycle = F.Controller->GetPossessGeneration();
	F.Controller->TryStartBrain(); F.Controller->TryStartBrain();
	TestEqual(TEXT("One start in cycle"), F.Controller->GetBrainStartCount(), 1);
	F.Controller->UnPossess();
	TestFalse(TEXT("Old cycle invalidated"), F.Controller->IsPossessCurrent(F.Pawn, OldCycle));
	F.Controller->Possess(F.Pawn);
	TestEqual(TEXT("New cycle may start once"), F.Controller->GetBrainStartCount(), 2); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEnemyControlTest, "Hodge.Enemy.AI.ControlAndDeath", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeEnemyControlTest::RunTest(const FString&)
{
	HodgeEnemyTests::FFixture F; TestTrue(TEXT("Ready"), F.Ready());
	F.ASC->AddLooseGameplayTag(HodgeHitReactionTags::Controlled);
	TestFalse(TEXT("Strong control rejects actions"), F.Init->CanExecuteAIAction());
	TestEqual(TEXT("Perception/brain remains running during control"), F.Controller->GetEnemyBrainState(), EHodgeEnemyBrainState::Running);
	F.ASC->RemoveLooseGameplayTag(HodgeHitReactionTags::Controlled);
	TestTrue(TEXT("Actions recover"), F.Init->CanExecuteAIAction());
	F.ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Death);
	TestEqual(TEXT("Death stops normal brain"), F.Controller->GetEnemyBrainState(), EHodgeEnemyBrainState::Stopped);
	TestFalse(TEXT("Death rejects actions"), F.Init->CanExecuteAIAction()); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEnemyLateCallbackTest, "Hodge.Enemy.AI.StaleTaskCallbacks", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeEnemyLateCallbackTest::RunTest(const FString&)
{
	HodgeEnemyTests::FFixture F; TestTrue(TEXT("Ready"), F.Ready());
	auto* Task = NewObject<UHodgeBTTask_ExecuteAbility>(); FHodgeEnemyAITestAccess::Prepare(Task, F.Controller, F.ASC);
	TestFalse(TEXT("Old attempt callback ignored"), FHodgeEnemyAITestAccess::DeliverOld(Task));
	F.Controller->UnPossess();
	TestFalse(TEXT("Detached Pawn callback ignored"), FHodgeEnemyAITestAccess::DeliverDetached(Task)); return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEnemyComponentRemovalTest, "Hodge.Enemy.Initialization.ComponentRemoval", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeEnemyComponentRemovalTest::RunTest(const FString&)
{
	HodgeEnemyTests::FFixture F;
	auto* Set = NewObject<UHodgeEnemyTestAbilitySet>(); Set->AddAbility(UHodgeRelationshipTestAbility::StaticClass()); F.Data->AbilitySets.Add(Set);
	TestTrue(TEXT("Ready before removal"), F.Ready());
	F.Init->UnregisterComponent();
	TestFalse(TEXT("Removed init component cannot report ready"), F.Init->IsGameplayReady());
	TestEqual(TEXT("Removed component cycle is invalid"), F.Init->TryInitialize(), EHodgeEnemyInitializationResult::Invalidated);
	TestEqual(TEXT("Owned grants cleaned on unregister"), F.ASC->GetActivatableAbilities().Num(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEnemyInstantFailureTest, "Hodge.Enemy.Grants.InstantFailureIsNotRollback", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeEnemyInstantFailureTest::RunTest(const FString&)
{
	HodgeEnemyTests::FFixture F; TestTrue(TEXT("Ready"), F.Ready());
	auto* Set = NewObject<UHodgeEnemyTestAbilitySet>(); Set->AddEffect(UHodgeEnemyInstantTestEffect::StaticClass());
	const auto Delegate = F.ASC->OnGameplayEffectAppliedDelegateToSelf.AddLambda([&F](UAbilitySystemComponent*, const FGameplayEffectSpec&, FActiveGameplayEffectHandle)
	{ F.Pawn->FindComponentByClass<UHodgePawnExtensionComponent>()->UninitializeAbilitySystem(); });
	FHodgeAbilitySet_GrantedHandles Handles;
	const auto Result = Set->TryGiveToAbilitySystem(F.ASC, Handles);
	F.ASC->OnGameplayEffectAppliedDelegateToSelf.Remove(Delegate);
	TestFalse(TEXT("Binding loss stops the batch"), Result.bSucceeded);
	TestTrue(TEXT("Instant effect is reported irreversible"), Result.bHasIrreversibleEffects);
	TestEqual(TEXT("No pretend reversal of Instant modifier"), F.ASC->GetNumericAttribute(UHodgeCombatSet::GetBaseDamageAttribute()), 26.f);
	TestFalse(TEXT("Invalidated actor cannot run AI"), F.Init->CanExecuteAIAction());
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEnemyTargetKeyTest, "Hodge.Enemy.AI.TargetKeyContract", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeEnemyTargetKeyTest::RunTest(const FString&)
{
	auto* Tree = NewObject<UBehaviorTree>();
	auto* Blackboard = NewObject<UBlackboardData>(Tree); Tree->BlackboardAsset = Blackboard;
	auto AddKey = [Blackboard](FName Name, UBlackboardKeyType* Type)
	{ FBlackboardEntry Entry; Entry.EntryName = Name; Entry.KeyType = Type; Blackboard->Keys.Add(Entry); };
	auto* ActorKey = NewObject<UBlackboardKeyType_Object>(Blackboard); ActorKey->BaseClass = AActor::StaticClass();
	AddKey(TEXT("TargetActor"), ActorKey);
	auto* AlternateKey = NewObject<UBlackboardKeyType_Object>(Blackboard); AlternateKey->BaseClass = APawn::StaticClass();
	AddKey(TEXT("AlternateTarget"), AlternateKey);
	AddKey(TEXT("GenericObject"), NewObject<UBlackboardKeyType_Object>(Blackboard));
	AddKey(TEXT("Location"), NewObject<UBlackboardKeyType_Vector>(Blackboard));
	auto* Task = NewObject<UHodgeBTTask_ExecuteAbility>(Tree);
	TestTrue(TEXT("TargetActor remains selectable in the editor Actor filter"), ActorKey->IsAllowedByFilter(Task->TargetActor.AllowedTypes[0]));
	TestFalse(TEXT("Original UObject key would be filtered out"), Blackboard->Keys.Last(1).KeyType->IsAllowedByFilter(Task->TargetActor.AllowedTypes[0]));
	TestEqual(TEXT("New task selects combat target, not SelfActor"), Task->TargetActor.SelectedKeyName, FName(TEXT("TargetActor")));
	Task->InitializeFromAsset(*Tree);
	TestTrue(TEXT("Actor target resolves"), Task->HasValidTargetKey());
	TestEqual(TEXT("Selected ID matches named target"), Task->TargetActor.GetSelectedKeyID(), Blackboard->GetKeyID(TEXT("TargetActor")));
	Task->TargetActor.SelectedKeyName = TEXT("AlternateTarget"); Task->InitializeFromAsset(*Tree);
	TestTrue(TEXT("Explicit Pawn target is preserved"), Task->HasValidTargetKey());
	TestEqual(TEXT("Custom target name preserved"), Task->TargetActor.SelectedKeyName, FName(TEXT("AlternateTarget")));
	AddExpectedError(TEXT("requires an Actor target key"), EAutomationExpectedErrorFlags::Contains, 4);
	for (FName Name : {FName(TEXT("GenericObject")), FName(TEXT("Location")), FBlackboard::KeySelf})
	{ Task->TargetActor.SelectedKeyName = Name; Task->InitializeFromAsset(*Tree); TestFalse(TEXT("Invalid target configuration rejected"), Task->HasValidTargetKey()); }
	auto* MissingBlackboard = NewObject<UBehaviorTree>(); Task->InitializeFromAsset(*MissingBlackboard);
	TestFalse(TEXT("Missing blackboard rejected"), Task->HasValidTargetKey());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEnemyClearedIdentityTest, "Hodge.Enemy.AI.EndedExecutionIdentity", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeEnemyClearedIdentityTest::RunTest(const FString&)
{
	HodgeEnemyTests::FFixture F; TestTrue(TEXT("Ready"), F.Ready());
	auto* Task = NewObject<UHodgeBTTask_ExecuteAbility>(); FHodgeEnemyAITestAccess::Prepare(Task, F.Controller, F.ASC);
	TestTrue(TEXT("A cleared Definition execution identity still completes the current task"), FHodgeEnemyAITestAccess::DeliverClearedExecution(Task, F.ASC));
	return true;
}
#endif
