/** @file HodgeCombatActionRequestTests.cpp
 * @brief 实现共享战斗请求数据自动化测试相关的类型、接口与现有处理流程。
 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "HodgeEnemyAITestSupport.h"
#include "Component/HodgeCombatComponentBase.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Character/HodgeEnemyCharacter.h"
#include "AI/HodgeEnemyAIController.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "Data/HodgePawnData.h"
#include "Data/HodgeCharacterStatProfile.h"
#include "Data/HodgeAbilityDefinition.h"
#include "Data/HodgeComboDefinition.h"
#include "Animation/AnimInstance.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_EnemyDeath.h"
#include "Component/HodgeHealthComponent.h"
#include "TimerManager.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"

/** 保存战斗动作测试Access的数据和接口契约。 */
struct FHodgeCombatActionTestAccess
{
	/** 使当前测试的缓存或会话达到过期条件。
	 * @param Combat 统一战斗组件。
	 */
	static void Expire(UHodgeCombatComponentBase* Combat)
	{ Combat->InputExpiresAt = Combat->GetWorld()->GetTimeSeconds() - 1.; Combat->TickComponent(0.f, LEVELTICK_All, nullptr); }
	/** 计算当前集合中满足条件的数量。
	 * @param Combat 统一战斗组件。
	 * @return 本次计算、查询或创建得到的整数32。
	 */
	static int32 Count(UHodgeCombatComponentBase* Combat) { return Combat->ActionRequests.Num(); }
	/** 解析移动。
	 * @param Combat 统一战斗组件。
	 * @param Ability 能力，由调用方传入。
	 */
	static void ResolveMove(UHodgeCombatComponentBase* Combat, UHodgeGameplayAbility_Definition* Ability)
	{
		Ability->bPredictedMoveCancel = true; Combat->bMoveRequestPending = true;
		Combat->PredictedMoveCancelAbility = Ability; Combat->PendingMoveHandle = Ability->GetCurrentAbilitySpecHandle(); Combat->PendingMoveKey = Combat->ExecutionKey();
		Combat->ClientMoveCancelResult_Implementation(Combat->GetOwner(), Combat->PendingMoveHandle, Combat->PendingMoveKey, true, 0.f, 0.f, 1.f);
	}
	/** 推进当前对象的随帧更新。
	 * @param Combat 统一战斗组件。
	 */
	static void Tick(UHodgeCombatComponentBase* Combat) { Combat->TickComponent(0.f, LEVELTICK_All, nullptr); }
};

namespace HodgeActionTests
{
	using EStatus = EHodgeCombatActionRequestStatus;
	/** 保存测试场景的数据和接口契约。 */
	struct FFixture
	{
		// Instance的数据或对象引用。
		UGameInstance* Instance = nullptr;
		// 本次操作所在的世界。
		UWorld* World = FHodgeEnemyInitTestAccess::CreateWorld(Instance);
		// 当前操作关联的Pawn角色。
		AHodgeEnemyCharacter* Pawn = World->SpawnActor<AHodgeEnemyCharacter>();
		// 后续GAS操作使用的能力系统组件。
		UHodgeAbilitySystemComponent* ASC = Pawn->GetNativeEnemyAbilitySystem();
		// 统一战斗组件。
		UHodgeCombatComponentBase* Combat = nullptr;
		// 当前操作使用的定义配置。
		UHodgeAbilityDefinition* Definition = nullptr;
		// 对应操作或能力的句柄。
		FGameplayAbilitySpecHandle Handle;
		// 依赖是否已准备完成。
		bool bReady = false;
		/** 构造测试场景并初始化其默认状态。 */
		FFixture()
		{
			// 当前角色使用的控制器。
			auto* Controller = Pawn->GetController();
			// 先检查对象与上下文，避免在依赖失效后继续处理。
			if (!Controller) { Controller = World->SpawnActor<AHodgeEnemyAIController>(); Controller->Possess(Pawn); }
			Pawn->FindComponentByClass<UHodgePawnExtensionComponent>()->TryToChangeInitState(HodgeGameplayTags::InitState_Spawned);
			Pawn->EnemyPawnData = NewObject<UHodgePawnData>();
			Pawn->EnemyPawnData->StatProfile = NewObject<UHodgeCharacterStatProfile>();
			FHodgeEnemyInitTestAccess::WithoutExperience(Pawn->GetEnemyInitComponent());
			Combat = NewObject<UHodgeCombatComponentBase>(Pawn); Pawn->AddInstanceComponent(Combat); Combat->RegisterComponent();
			bReady = Pawn->GetEnemyInitComponent()->TryInitialize() == EHodgeEnemyInitializationResult::Ready;
			Pawn->GetMesh()->SetSkeletalMesh(LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/Main/Character/Enemies/Guardian/Model/SKM_Guardian.SKM_Guardian")));
			Pawn->GetMesh()->SetAnimInstanceClass(LoadClass<UAnimInstance>(nullptr, TEXT("/Game/Main/Character/Enemies/Guardian/Anim/ABP_Guardian.ABP_Guardian_C")));
			ASC->RefreshAbilityActorInfo();
			// 本次操作的来源。
			const auto* Source = LoadObject<UHodgeAbilityDefinition>(nullptr, TEXT("/Game/Main/Character/Enemies/Guardian/DA_Guardian_Attack.DA_Guardian_Attack"));
			Definition = DuplicateObject<UHodgeAbilityDefinition>(Source, Pawn);
			Definition->AbilityClass = UHodgeGameplayAbility_Definition::StaticClass();
			Definition->ExecutionConfig.Montage = DuplicateObject<UAnimMontage>(Definition->ExecutionConfig.Montage, Definition);
			Definition->ExecutionConfig.Montage->Notifies.Reset();
			Handle = ASC->GiveAbilityDefinition(Definition, 1, Pawn);
			bReady &= Handle.IsValid() && Pawn->GetMesh()->GetAnimInstance() != nullptr;
		}
		/** 读取当前测试场景或记录关联的能力实例。
		 * @return 本次计算、查询或创建得到的玩法能力定义。
		 */
		UHodgeGameplayAbility_Definition* Ability() const
		{ const auto* Spec = ASC->FindAbilitySpecFromHandle(Handle); return Spec ? Cast<UHodgeGameplayAbility_Definition>(Spec->GetPrimaryInstance()) : nullptr; }
		/** 取得当前请求或测试场景的状态快照。
		 * @param Id 当前记录的唯一标识。
		 * @return 本次计算、查询或创建得到的战斗动作请求状态。
		 */
		FHodgeCombatActionRequestState State(FGuid Id) const
		{ FHodgeCombatActionRequestState Result; Combat->GetCombatActionRequest(Id, Result); return Result; }
		/** 配置连段。
		 * @param bBinding 是否绑定的控制标记。
		 */
		void ConfigureCombo(bool bBinding = false)
		{
			Definition->ExecutionRoute = EHodgeAbilityExecutionRoute::ComboCoordinated;
			auto* Combo = NewObject<UHodgeComboDefinition>(Pawn); Combo->ComboTable = NewObject<UDataTable>(Combo);
			Combo->ComboTable->RowStruct = FHodgeComboRow::StaticStruct(); Combo->EntryComboTag = HodgeGameplayTags::Status_Attack;
			FHodgeComboRow Entry; Entry.ComboTag = Combo->EntryComboTag;
			FHodgeComboRow Attack; Attack.ComboTag = HodgeGameplayTags::Status_Attack_Active; Attack.AbilityTag = Definition->AbilityTag;
			FHodgeComboTransition Edge; Edge.TriggerInputIntentTag = HodgeGameplayTags::Status_Attack_Cancel_NextAttack; Edge.TargetComboTag = Attack.ComboTag;
			Entry.Transitions.Add(Edge);
			Edge.RequiredWindowTags.AddTag(HodgeGameplayTags::Status_Attack_Cancel_Move); Attack.Transitions.Add(Edge);
			Combo->ComboTable->AddRow(Entry.ComboTag.GetTagName(), Entry); Combo->ComboTable->AddRow(Attack.ComboTag.GetTagName(), Attack);
			if (bBinding) { Combo->InputBindings.Add({HodgeGameplayTags::InputTag_Ability_Melee, Edge.TriggerInputIntentTag}); }
			Pawn->EnemyPawnData->ComboDefinition = Combo; Combat->Configure(ASC, Combo);
		}
		/** 销毁该对象，执行当前类型的析构收尾。 */
		~FFixture() { Combat->Shutdown(); FHodgeEnemyInitTestAccess::DestroyWorld(Instance, World); }
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeActionLifecycleTest, "Hodge.ActionRequests.StandaloneLifecycle", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeActionLifecycleTest::RunTest(const FString&)
{
	HodgeActionTests::FFixture F;
	if (!TestTrue(TEXT("Fixture initialized with real animation"), F.bReady)) { return false; }
	const auto First = F.Combat->RequestStandaloneAbility(F.Definition->AbilityTag);
	TestEqual(TEXT("Standalone starts"), First.Status, HodgeActionTests::EStatus::Started);
	TestTrue(TEXT("Execution identity captured"), First.ExecutionId.IsValid());
	// 根据当前生命周期状态决定是否继续执行。
	if (!F.Ability() || !F.Ability()->IsActive()) { return false; }
	TestEqual(TEXT("Duplicate activation rejected"), F.Combat->RequestStandaloneAbility(F.Definition->AbilityTag).Status, HodgeActionTests::EStatus::Rejected);
	F.Ability()->FinishExecution(false, false);
	TestEqual(TEXT("Cleared execution identity still completes request"), F.State(First.RequestId).Status, HodgeActionTests::EStatus::Completed);
	const auto Second = F.Combat->RequestStandaloneAbility(F.Definition->AbilityTag);
	TestFalse(TEXT("Old cancellation cannot cancel new execution"), F.Combat->CancelCombatActionRequest(First.RequestId));
	TestTrue(TEXT("New execution remains active"), F.Ability()->IsActive());
	TestTrue(TEXT("Current request can cancel its own execution"), F.Combat->CancelCombatActionRequest(Second.RequestId));
	TestEqual(TEXT("Cancellation delivered"), F.State(Second.RequestId).Status, HodgeActionTests::EStatus::Cancelled);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeActionIntentTest, "Hodge.ActionRequests.IntentMappingAndBuffer", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeActionIntentTest::RunTest(const FString&)
{
	HodgeActionTests::FFixture F; if (!TestTrue(TEXT("Ready"), F.bReady)) { return false; }
	F.ConfigureCombo(true);
	const FGameplayTag Intent = HodgeGameplayTags::Status_Attack_Cancel_NextAttack;
	const auto First = F.Combat->SubmitCombatIntent(Intent);
	TestEqual(TEXT("Intent authorized through combo graph"), First.Status, HodgeActionTests::EStatus::Started);
	TestTrue(TEXT("Coordinator selected attack node"), F.Combat->GetCurrentComboTag() == HodgeGameplayTags::Status_Attack_Active);
	const auto Queued = F.Combat->SubmitCombatIntent(Intent);
	TestEqual(TEXT("Missing window queues intent"), Queued.Status, HodgeActionTests::EStatus::Queued);
	FHodgeCombatActionRequestState InputRequest;
	F.Combat->OnActionRequestChanged.AddLambda([&InputRequest](const auto& State) { InputRequest = State; });
	TestTrue(TEXT("Player input maps into same intent submission"), F.Combat->InputPressed(HodgeGameplayTags::InputTag_Ability_Melee));
	TestEqual(TEXT("Replaced request terminates"), F.State(Queued.RequestId).Status, HodgeActionTests::EStatus::Cancelled);
	TestEqual(TEXT("Mapped player request is queued"), InputRequest.Status, HodgeActionTests::EStatus::Queued);
	TestEqual(TEXT("Mapped intent preserves semantic tag"), InputRequest.ActionTag, Intent);
	const FGuid InputId = InputRequest.RequestId;
	FHodgeCombatActionTestAccess::Expire(F.Combat);
	TestEqual(TEXT("Buffered request expires explicitly"), F.State(InputId).Status, HodgeActionTests::EStatus::Expired);
	TestTrue(TEXT("Expiry does not cancel running source attack"), F.Ability()->IsActive());
	F.Combat->OnActionRequestChanged.Clear(); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeActionAIRouteTest, "Hodge.ActionRequests.AIIntentWithoutInputBinding", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeActionAIRouteTest::RunTest(const FString&)
{
	HodgeActionTests::FFixture F; if (!TestTrue(TEXT("Ready"), F.bReady)) { return false; }
	F.ConfigureCombo();
	TestEqual(TEXT("Combo cannot bypass authorization through standalone entry"), F.Combat->RequestStandaloneAbility(F.Definition->AbilityTag).Status, HodgeActionTests::EStatus::Rejected);
	const auto Request = F.Combat->SubmitCombatIntent(HodgeGameplayTags::Status_Attack_Cancel_NextAttack);
	TestEqual(TEXT("AI uses graph intent without player key mapping"), Request.Status, HodgeActionTests::EStatus::Started);
	TestTrue(TEXT("AI cancels exactly the owned combo request"), F.Combat->CancelCombatActionRequest(Request.RequestId)); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeActionSynchronousTest, "Hodge.ActionRequests.SynchronousEndAndReentrancy", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeActionSynchronousTest::RunTest(const FString&)
{
	HodgeActionTests::FFixture F; if (!TestTrue(TEXT("Ready"), F.bReady)) { return false; }
	FHodgeCombatActionRequestState Nested;
	F.Combat->OnActionRequestChanged.AddLambda([&](const auto& State)
	{
		if (State.Status == HodgeActionTests::EStatus::Started)
		{ F.Combat->CancelCombatActionRequest(State.RequestId); Nested = F.Combat->RequestStandaloneAbility(F.Definition->AbilityTag); }
	});
	TArray<HodgeActionTests::EStatus> Seen;
	F.Combat->OnActionRequestChanged.AddLambda([&Seen](const auto& State) { Seen.Add(State.Status); });
	const auto Cancelled = F.Combat->RequestStandaloneAbility(F.Definition->AbilityTag);
	TestTrue(TEXT("All listeners observe start before reentrant cancellation"), Seen.Num() == 2 && Seen[0] == HodgeActionTests::EStatus::Started && Seen[1] == HodgeActionTests::EStatus::Cancelled);
	TestEqual(TEXT("Reentrant cancellation reflected in return snapshot"), Cancelled.Status, HodgeActionTests::EStatus::Cancelled);
	TestEqual(TEXT("Nested submission rejects safely"), Nested.Status, HodgeActionTests::EStatus::Rejected);
	F.Combat->OnActionRequestChanged.Clear();
	F.Pawn->GetMesh()->SetAnimInstanceClass(nullptr); F.ASC->RefreshAbilityActorInfo();
	const auto Synchronous = F.Combat->RequestStandaloneAbility(F.Definition->AbilityTag);
	TestTrue(TEXT("Synchronous animation failure is terminal, not a hanging started request"), Synchronous.IsTerminal());
	TestEqual(TEXT("Synchronous end is cancelled"), Synchronous.Status, HodgeActionTests::EStatus::Cancelled); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeActionFailuresTest, "Hodge.ActionRequests.ValidationAndRevocation", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeActionFailuresTest::RunTest(const FString&)
{
	HodgeActionTests::FFixture F; if (!TestTrue(TEXT("Ready"), F.bReady)) { return false; }
	TestEqual(TEXT("Invalid tag rejected"), F.Combat->RequestStandaloneAbility({}).Status, HodgeActionTests::EStatus::Rejected);
	TestEqual(TEXT("Unconfigured combo rejected"), F.Combat->SubmitCombatIntent(HodgeGameplayTags::Status_Attack).Status, HodgeActionTests::EStatus::Rejected);
	F.ASC->SetNumericAttributeBase(UHodgeHealthSet::GetHealthAttribute(), 0.f);
	TestFalse(TEXT("Dead pawn fails shared readiness gate"), F.Combat->IsCombatReadyForActions());
	TestEqual(TEXT("Dead pawn request rejected"), F.Combat->RequestStandaloneAbility(F.Definition->AbilityTag).Status, HodgeActionTests::EStatus::Rejected);
	F.ASC->SetNumericAttributeBase(UHodgeHealthSet::GetHealthAttribute(), 100.f);
	const auto Request = F.Combat->RequestStandaloneAbility(F.Definition->AbilityTag);
	F.ASC->ClearAbility(F.Handle);
	TestTrue(TEXT("Grant removal terminates active request"), F.State(Request.RequestId).IsTerminal());
	TestEqual(TEXT("Revoked ability cannot be requested"), F.Combat->RequestStandaloneAbility(F.Definition->AbilityTag).Status, HodgeActionTests::EStatus::Rejected); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeActionHistoryTest, "Hodge.ActionRequests.BoundedHistoryAndShutdown", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeActionHistoryTest::RunTest(const FString&)
{
	HodgeActionTests::FFixture F; if (!TestTrue(TEXT("Ready"), F.bReady)) { return false; }
	const auto Active = F.Combat->RequestStandaloneAbility(F.Definition->AbilityTag);
	for (int32 I = 0; I < 100; ++I) { F.Combat->RequestStandaloneAbility({}); }
	TestTrue(TEXT("History bounded"), FHodgeCombatActionTestAccess::Count(F.Combat) <= 64);
	TestEqual(TEXT("Active request never evicted"), F.State(Active.RequestId).Status, HodgeActionTests::EStatus::Started);
	F.Combat->Shutdown();
	TestEqual(TEXT("Shutdown closes owned request"), F.State(Active.RequestId).Status, HodgeActionTests::EStatus::Cancelled);
	TestFalse(TEXT("Shutdown cancelled owned ability"), F.Ability()->IsActive());
	TestEqual(TEXT("Unbound component refuses new actions"), F.Combat->RequestStandaloneAbility(F.Definition->AbilityTag).Status, HodgeActionTests::EStatus::Rejected); return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeActionMoveResolutionTest, "Hodge.ActionRequests.BufferedIdentityAcrossMoveResolution", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeActionMoveResolutionTest::RunTest(const FString&)
{
	HodgeActionTests::FFixture F; if (!TestTrue(TEXT("Ready"), F.bReady)) { return false; }
	F.ConfigureCombo();
	F.Combat->SubmitCombatIntent(HodgeGameplayTags::Status_Attack_Cancel_NextAttack);
	const auto Buffered = F.Combat->SubmitCombatIntent(HodgeGameplayTags::Status_Attack_Cancel_NextAttack);
	TestEqual(TEXT("Input queued during source attack"), Buffered.Status, HodgeActionTests::EStatus::Queued);
	FHodgeCombatActionTestAccess::ResolveMove(F.Combat, F.Ability());
	TestEqual(TEXT("Ending source for move confirmation preserves queued request identity"), F.State(Buffered.RequestId).Status, HodgeActionTests::EStatus::Queued);
	FHodgeCombatActionTestAccess::Tick(F.Combat);
	TestEqual(TEXT("Restored request can start after source action ended"), F.State(Buffered.RequestId).Status, HodgeActionTests::EStatus::Started); return true;
}

/** 保存按住死亡测试场景的数据和接口契约。 */
struct FHodgeHeldDeathFixture
{
	// 测试场景的数据或对象引用。
	TSharedPtr<HodgeActionTests::FFixture> Fixture;
	// 死亡的弱引用，不延长目标对象生命周期。
	TWeakObjectPtr<UHodgeGameplayAbility_EnemyDeath> Death;
	// 测试的数据或对象引用。
	FAutomationTestBase* Test = nullptr;
	// Steps的数值或阈值。
	int32 Steps = 0;
};
DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FHodgeVerifyHeldDeath, TSharedPtr<FHodgeHeldDeathFixture>, Data);
/** 更新当前对象保存的计算或表现状态。 */
bool FHodgeVerifyHeldDeath::Update()
{
	auto& F = *Data->Fixture;
	F.Pawn->GetMesh()->TickAnimation(.1f, false);
	F.World->GetTimerManager().Tick(.1f);
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (Data->Death.IsValid() && Data->Death->IsActive() && ++Data->Steps < 40) { return false; }
	Data->Test->TestTrue(TEXT("Held pose ends through the actual scheduled monitor"), !Data->Death.IsValid() || !Data->Death->IsActive());
	Data->Test->TestEqual(TEXT("Health lifecycle finished"), F.Pawn->FindComponentByClass<UHodgeHealthComponent>()->GetDeathState(), EHodgeDeathState::DeathFinished);
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeActionHeldDeathTest, "Hodge.ActionRequests.HeldDeathPoseCompatibility", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeActionHeldDeathTest::RunTest(const FString&)
{
	auto Data = MakeShared<FHodgeHeldDeathFixture>(); Data->Fixture = MakeShared<HodgeActionTests::FFixture>(); Data->Test = this;
	auto& F = *Data->Fixture; if (!TestTrue(TEXT("Ready"), F.bReady)) { return false; }
	const auto Handle = F.ASC->GiveAbility(FGameplayAbilitySpec(UHodgeGameplayAbility_EnemyDeath::StaticClass(), 1));
	auto* Death = Cast<UHodgeGameplayAbility_EnemyDeath>(F.ASC->FindAbilitySpecFromHandle(Handle)->GetPrimaryInstance());
	if (!TestNotNull(TEXT("Death instance exists"), Death)) { return false; }
	Death->DeathMontage = DuplicateObject<UAnimMontage>(LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Main/Character/Enemies/Guardian/Anim/AM_Death.AM_Death")), F.Pawn);
	Death->DeathMontage->bEnableAutoBlendOut = false; Data->Death = Death;
	// 通过ASC发起激活，保留GAS自身的条件与消耗校验。
	TestTrue(TEXT("Existing death GA can activate"), F.ASC->TryActivateAbility(Handle));
	ADD_LATENT_AUTOMATION_COMMAND(FHodgeVerifyHeldDeath(Data)); return true;
}
#endif
