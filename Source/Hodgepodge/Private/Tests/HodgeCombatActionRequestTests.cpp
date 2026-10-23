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
#include "Animation/HodgeCombatAnimNotifies.h"
#include "Animation/HodgeRootMotionModifier_AttackFacing.h"
#include "AnimNotifyState_MotionWarping.h"
#include "Component/HodgeCharacterRotationComponent.h"
#include "Component/HodgeAttackMotionWarpingComponent.h"
#include "HodgeAttackWarpTestTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/PlayerState.h"
#include <limits>

/** 保存战斗动作测试Access的数据和接口契约。 */
struct FHodgeCombatActionTestAccess
{
	/** 注入服务器收到的请求；传输层另由 PIE 验证，不把此入口当成真实 RPC。 */
	static void ServerComboRequest(UHodgeAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle Handle,
		const FPredictionKey& Key, const FGameplayEventData& Payload)
	{ ASC->InternalServerTryActivateAbility(Handle, true, Key, &Payload); }
	/** 等待记录由生产 Timer 创建和移除，测试不直接改写队列。 */
	static int32 PendingComboRequests(const UHodgeAbilitySystemComponent* ASC) { return ASC->DeferredComboPayloads.Num(); }
	/** 只读确认数据确已进入原 GAS 缓存，避免缺失上行输入也误判交接成功。 */
	static bool HasConfirmedTargetData(UHodgeAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle Handle, const FPredictionKey& Key)
	{
		const auto Cached = ASC->AbilityTargetDataMap.Find(FGameplayAbilitySpecHandleAndPredictionKey(Handle, Key));
		return Cached.IsValid() && Cached->bTargetConfirmed && Cached->TargetData.Num() > 0;
	}
	/** 读取当前执行的网络来源身份，不能用新请求身份替代旧来源。 */
	static int16 ComboSourceKey(const UHodgeCombatComponentBase* Combat) { return Combat->ExecutionKey(); }
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
		Combat->PendingMoveRequest = FGuid::NewGuid(); Combat->PendingMoveExecution = Ability->GetExecutionId();
		Combat->ClientMoveCancelResult_Implementation(Combat->GetOwner(), Combat->PendingMoveHandle, Combat->PendingMoveKey, Combat->PendingMoveRequest, true, 0.f, 0.f, 1.f);
	}
	/** 推进当前对象的随帧更新。
	 * @param Combat 统一战斗组件。
	 */
	static void Tick(UHodgeCombatComponentBase* Combat) { Combat->TickComponent(0.f, LEVELTICK_All, nullptr); }
	/** 查询生产取消候选，不将独立技能塞进连段状态。 */
	static UHodgeGameplayAbility_Definition* Candidate(UHodgeCombatComponentBase* Combat) { return Combat->GetMoveCancelAbility(); }
	/** 读取路由策略，验证独立技能与连段配置互不串用。 */
	static bool Policy(UHodgeCombatComponentBase* Combat, UHodgeGameplayAbility_Definition* Ability, FGameplayTag& Tag, float& Threshold)
	{ return Combat->GetMoveCancelPolicy(Ability, Tag, Threshold); }
	/** 只修改测试连段数据，验证兼容原有零阈值配置。 */
	static void ComboThreshold(UHodgeCombatComponentBase* Combat, float Threshold)
	{
		auto* Combo = const_cast<UHodgeComboDefinition*>(Combat->Definition.Get());
		Combo->MoveIntentThreshold = Threshold; Combo->MoveCancelWindowTag = HodgeGameplayTags::Status_Attack_Cancel_Move;
	}

	/** 注入测试窗口许可，服务器处理仍调用完整生产验证与 EndAbility。 */
	static void Window(UHodgeGameplayAbility_Definition* Ability, FGameplayTag Tag)
	{ Ability->StateCounts.Add(Tag, 1); Ability->GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(Tag); }
	/** 模拟已经到达服务器的请求，允许单独验证非法身份和输入。 */
	static void Request(UHodgeCombatComponentBase* Combat, UHodgeGameplayAbility_Definition* Ability, FVector2D Intent,
		AActor* Avatar = nullptr, FGuid Execution = FGuid())
	{
		const auto Prediction = Ability->GetCurrentActivationInfo().GetActivationPredictionKey();
		Combat->ProcessServerMoveCancel(Avatar ? Avatar : Combat->GetOwner(), Ability->GetCurrentAbilitySpecHandle(),
			Prediction.IsServerInitiatedKey() ? -Prediction.Current : Prediction.Current, FGuid::NewGuid(),
			Execution.IsValid() ? Execution : Ability->GetExecutionId(), Intent);
	}
	/** 过期回执不得结束或清理新的待确认请求。 */
	static bool IgnoreOldResult(UHodgeCombatComponentBase* Combat, UHodgeGameplayAbility_Definition* Ability)
	{
		Combat->bMoveRequestPending = true; Combat->PredictedMoveCancelAbility = Ability;
		Combat->PendingMoveHandle = Ability->GetCurrentAbilitySpecHandle(); Combat->PendingMoveKey = 0;
		Combat->PendingMoveRequest = FGuid::NewGuid(); Combat->PendingMoveExecution = Ability->GetExecutionId();
		Combat->ClientMoveCancelResult_Implementation(Combat->GetOwner(), Combat->PendingMoveHandle, 0, FGuid::NewGuid(), true, 0.f, 0.f, 1.f);
		return Combat->bMoveRequestPending && Ability->IsActive();
	}

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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeStandaloneMovePolicyTest, "Hodge.ActionRequests.MoveCancel.RouteAndPolicy", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 验证没有连段配置的独立技能及可调窗口、阈值。 */
bool FHodgeStandaloneMovePolicyTest::RunTest(const FString&)
{
	HodgeActionTests::FFixture F; if (!TestTrue(TEXT("场景就绪"), F.bReady)) { return false; }
	F.Combat->Configure(F.ASC, nullptr);
	F.Combat->RequestStandaloneAbility(F.Definition->AbilityTag);
	TestTrue(TEXT("无 Combo 配置仍开启取消更新"), F.Combat->IsComponentTickEnabled());
	TestEqual(TEXT("独立执行从 ASC 解析"), FHodgeCombatActionTestAccess::Candidate(F.Combat), F.Ability());
	FGameplayTag Tag; float Threshold = 0.f;
	TestTrue(TEXT("独立策略可用"), FHodgeCombatActionTestAccess::Policy(F.Combat, F.Ability(), Tag, Threshold));
	TestTrue(TEXT("标准通知窗口"), Tag == HodgeGameplayTags::Status_Attack_Cancel_Move);
	TestEqual(TEXT("默认输入阈值"), Threshold, .1f);
	F.Definition->StandaloneMoveIntentThreshold = .4f;
	FHodgeCombatActionTestAccess::Policy(F.Combat, F.Ability(), Tag, Threshold);
	TestEqual(TEXT("独立配置不依赖 Combo"), Threshold, .4f);
	F.Definition->StandaloneMoveCancelWindowTag = {};
	TestFalse(TEXT("空标签关闭取消"), FHodgeCombatActionTestAccess::Policy(F.Combat, F.Ability(), Tag, Threshold));
	F.ASC->InitAbilityActorInfo(F.Pawn, nullptr);
	TestNull(TEXT("解绑后旧执行不能取得预测权限"), FHodgeCombatActionTestAccess::Candidate(F.Combat));
	HodgeActionTests::FFixture ComboFixture; ComboFixture.ConfigureCombo();
	ComboFixture.Definition->StandaloneMoveCancelWindowTag = {};
	ComboFixture.Combat->SubmitCombatIntent(HodgeGameplayTags::Status_Attack_Cancel_NextAttack);
	FHodgeCombatActionTestAccess::ComboThreshold(ComboFixture.Combat, 0.f);
	TestTrue(TEXT("连段仍兼容零阈值"), FHodgeCombatActionTestAccess::Policy(ComboFixture.Combat, ComboFixture.Ability(), Tag, Threshold));
	TestEqual(TEXT("连段不读取独立窗口配置"), Threshold, 0.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeStandaloneMoveValidationTest, "Hodge.ActionRequests.MoveCancel.AuthorityValidation", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 正常请求走完整结束链路，窗口、输入和执行身份均由服务器验证。 */
bool FHodgeStandaloneMoveValidationTest::RunTest(const FString&)
{
	HodgeActionTests::FFixture F; if (!TestTrue(TEXT("场景就绪"), F.bReady)) { return false; }
	const auto Request = F.Combat->RequestStandaloneAbility(F.Definition->AbilityTag);
	FHodgeCombatActionTestAccess::Request(F.Combat, F.Ability(), FVector2D(0.f, 1.f));
	TestTrue(TEXT("前摇没有窗口时拒绝"), F.Ability()->IsActive());
	FHodgeCombatActionTestAccess::Window(F.Ability(), HodgeGameplayTags::Status_Attack_Cancel_Move);
	for (const FVector2D Intent : {FVector2D::ZeroVector, FVector2D(.099f, 0.f), FVector2D(2.f, 2.f), FVector2D(std::numeric_limits<float>::quiet_NaN(), 0.f)})
	{
		FHodgeCombatActionTestAccess::Request(F.Combat, F.Ability(), Intent);
		TestTrue(TEXT("非法或未达阈值输入不结束能力"), F.Ability()->IsActive());
	}
	FHodgeCombatActionTestAccess::Request(F.Combat, F.Ability(), FVector2D(0.f, 1.f), F.World->SpawnActor<AActor>());
	TestTrue(TEXT("错误 Avatar 拒绝"), F.Ability()->IsActive());
	FHodgeCombatActionTestAccess::Request(F.Combat, F.Ability(), FVector2D(0.f, 1.f), nullptr, FGuid::NewGuid());
	TestTrue(TEXT("旧执行的延迟处理拒绝"), F.Ability()->IsActive());
	F.ASC->AddLooseGameplayTag(TAG_Gameplay_AbilityInputBlocked);
	FHodgeCombatActionTestAccess::Request(F.Combat, F.Ability(), FVector2D(0.f, 1.f));
	TestTrue(TEXT("禁止输入时不能取消"), F.Ability()->IsActive());
	F.ASC->RemoveLooseGameplayTag(TAG_Gameplay_AbilityInputBlocked);
	FHodgeCombatActionTestAccess::Request(F.Combat, F.Ability(), FVector2D(.1f, 0.f));
	TestFalse(TEXT("窗口内阈值边界允许取消"), F.Ability()->IsActive());
	TestEqual(TEXT("请求记录正确取消"), F.State(Request.RequestId).Status, HodgeActionTests::EStatus::Cancelled);
	TestFalse(TEXT("结束清理通知标签"), F.ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Attack_Cancel_Move));
	TestEqual(TEXT("结束清理姿势占用"), F.Combat->GetPoseLeaseCount(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeStandaloneMoveIdentityTest, "Hodge.ActionRequests.MoveCancel.LateResultAndDeath", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 检查旧确认与死亡优先级，避免结束新动作或放开死亡移动。 */
bool FHodgeStandaloneMoveIdentityTest::RunTest(const FString&)
{
	HodgeActionTests::FFixture F; if (!TestTrue(TEXT("场景就绪"), F.bReady)) { return false; }
	F.Combat->RequestStandaloneAbility(F.Definition->AbilityTag);
	TestTrue(TEXT("旧请求回执不影响新请求"), FHodgeCombatActionTestAccess::IgnoreOldResult(F.Combat, F.Ability()));
	F.ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Death);
	TestNull(TEXT("死亡时无取消候选"), FHodgeCombatActionTestAccess::Candidate(F.Combat));
	TestFalse(TEXT("死亡不拥有预测移动权限"), F.Combat->IsMoveCancelPredicted());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeStandaloneMoveConfigTest, "Hodge.ActionRequests.MoveCancel.InvalidConfiguration", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 配置验证拒绝非法阈值，显式关闭窗口不会被视为错误。 */
bool FHodgeStandaloneMoveConfigTest::RunTest(const FString&)
{
	HodgeActionTests::FFixture F; if (!TestTrue(TEXT("场景就绪"), F.bReady)) { return false; }
	for (float Value : {0.f, -1.f, 1.1f, std::numeric_limits<float>::infinity(), std::numeric_limits<float>::quiet_NaN()})
	{
		F.Definition->StandaloneMoveIntentThreshold = Value; TArray<FText> Errors;
		TestFalse(TEXT("非法阈值拒绝配置"), F.Definition->ValidateDefinition(Errors));
	}
	F.Definition->StandaloneMoveIntentThreshold = 1.f; F.Definition->StandaloneMoveCancelWindowTag = {};
	TArray<FText> Errors;
	TestTrue(TEXT("合法阈值且空窗口可关闭取消"), F.Definition->ValidateDefinition(Errors));
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

/** 分帧运行真实服务器等待 Timer；每个场景独立创建和销毁测试世界。 */
struct FHodgeDeferredComboFixture
{
	TSharedPtr<HodgeActionTests::FFixture> Fixture;
	FAutomationTestBase* Test = nullptr;
	FName Scenario;
	FGameplayAbilitySpecHandle TargetHandle;
	FGameplayEventData Payload;
	FPredictionKey Key;
	int32 Step = 0;
	// 使用生产入口的同一世界时钟验证截止边界，不将其他提前拒绝误判成超时。
	double ExpiresAt = 0.0;
};

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FHodgeVerifyDeferredCombo, TSharedPtr<FHodgeDeferredComboFixture>, Data);
/** 生产队列只能在开窗时激活，超时或取消必须清理且不能被重复消息复活。 */
bool FHodgeVerifyDeferredCombo::Update()
{
	if (!Data->Fixture)
	{
		Data->Fixture = MakeShared<HodgeActionTests::FFixture>();
		auto& F = *Data->Fixture;
		if (!Data->Test->TestTrue(TEXT("Deferred combo fixture is ready"), F.bReady)) { return true; }
		F.ConfigureCombo();
		auto* Combo = const_cast<UHodgeComboDefinition*>(F.Pawn->EnemyPawnData->ComboDefinition.Get());
		auto* Target = DuplicateObject<UHodgeAbilityDefinition>(F.Definition, F.Pawn);
		Target->AbilityTag = HodgeGameplayTags::Status_Attack_Recovery;
		Data->TargetHandle = F.ASC->GiveAbilityDefinition(Target, 1, F.Pawn);
		if (!Data->Test->TestTrue(TEXT("Separate target action is granted"), Data->TargetHandle.IsValid())) { return true; }
		FHodgeComboRow TargetRow;
		TargetRow.ComboTag = HodgeGameplayTags::Status_Attack_Recovery;
		TargetRow.AbilityTag = Target->AbilityTag;
		Combo->ComboTable->AddRow(TargetRow.ComboTag.GetTagName(), TargetRow);
		const FGameplayTag SourceTag = HodgeGameplayTags::Status_Attack_Active;
		auto* SourceRow = Combo->ComboTable->FindRow<FHodgeComboRow>(SourceTag.GetTagName(), TEXT("DeferredComboTest"));
		if (!Data->Test->TestNotNull(TEXT("Source combo node exists"), SourceRow) ||
			!Data->Test->TestTrue(TEXT("Source node has its native closed edge"), !SourceRow->Transitions.IsEmpty())) { return true; }
		SourceRow->Transitions[0].TargetComboTag = TargetRow.ComboTag;
		const auto Started = F.Combat->SubmitCombatIntent(HodgeGameplayTags::Status_Attack_Cancel_NextAttack);
		if (!Data->Test->TestEqual(TEXT("Source action starts through Combat"), Started.Status, HodgeActionTests::EStatus::Started)) { return true; }
		Data->Payload.EventTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayEvent.Combo.InputRequest"));
		Data->Payload.Instigator = F.Pawn;
		Data->Payload.OptionalObject = Combo;
		Data->Payload.TargetTags.AddTag(HodgeGameplayTags::Status_Attack_Active);
		Data->Payload.InstigatorTags.AddTag(HodgeGameplayTags::Status_Attack_Cancel_NextAttack);
		Data->Payload.EventMagnitude = FHodgeCombatActionTestAccess::ComboSourceKey(F.Combat);
		Data->Key.Current = 101;
		Data->ExpiresAt = F.World->GetTimeSeconds() + F.Combat->GetServerInputBufferSeconds();
		FHodgeCombatActionTestAccess::ServerComboRequest(F.ASC, Data->TargetHandle, Data->Key, Data->Payload);
		Data->Test->TestEqual(TEXT("Closed edge really enters server wait queue"), FHodgeCombatActionTestAccess::PendingComboRequests(F.ASC), 1);
		return false;
	}
	auto& F = *Data->Fixture;
	F.World->Tick(LEVELTICK_All, .01f);
	const auto* TargetSpec = F.ASC->FindAbilitySpecFromHandle(Data->TargetHandle);
	if (!TargetSpec) { Data->Test->AddError(TEXT("Target grant disappeared during deferred test")); return true; }
	if (Data->Scenario == TEXT("Timeout"))
	{
		// 来源在整个等待期间仍有效，队列只能由真正到期而不是动作结束清理。
		Data->Test->TestTrue(TEXT("Source remains active through the timeout boundary"), F.Ability() && F.Ability()->IsActive());
		const bool bBeforeDeadline = F.World->GetTimeSeconds() < Data->ExpiresAt;
		Data->Test->TestEqual(TEXT("Queue is retained before its deadline and removed at expiry"),
			FHodgeCombatActionTestAccess::PendingComboRequests(F.ASC), bBeforeDeadline ? 1 : 0);
		Data->Test->TestFalse(TEXT("Closed target never activates while timing out"), TargetSpec->IsActive());
	}
	if (++Data->Step == 1)
	{
		Data->Test->TestFalse(TEXT("A scheduled closed edge cannot activate early"), TargetSpec->IsActive());
		Data->Test->TestEqual(TEXT("Closed edge remains queued after real Timer tick"), FHodgeCombatActionTestAccess::PendingComboRequests(F.ASC), 1);
		if (Data->Scenario == TEXT("Open"))
		{
			// 同一预测请求重复到达仍由原去重逻辑处理，不在测试中删掉重复队列。
			FHodgeCombatActionTestAccess::ServerComboRequest(F.ASC, Data->TargetHandle, Data->Key, Data->Payload);
			FHodgeCombatActionTestAccess::Window(F.Ability(), HodgeGameplayTags::Status_Attack_Cancel_Move);
		}
		else if (Data->Scenario == TEXT("Cancel")) { F.Ability()->FinishExecution(true, false); }
		return false;
	}
	if (FHodgeCombatActionTestAccess::PendingComboRequests(F.ASC) > 0 && Data->Step < 100) { return false; }
	Data->Test->TestEqual(TEXT("Wait records are eventually removed"), FHodgeCombatActionTestAccess::PendingComboRequests(F.ASC), 0);
	const bool bOpen = Data->Scenario == TEXT("Open");
	Data->Test->TestEqual(TEXT("Only the opened edge activates its requested action"), TargetSpec->IsActive(), bOpen);
	if (bOpen)
	{
		auto* Ability = Cast<UHodgeGameplayAbility_Definition>(TargetSpec->GetPrimaryInstance());
		if (!Data->Test->TestNotNull(TEXT("Opened target owns a real ability instance"), Ability)) { return true; }
		const FGuid Execution = Ability->GetExecutionId();
		FHodgeCombatActionTestAccess::ServerComboRequest(F.ASC, Data->TargetHandle, Data->Key, Data->Payload);
		Data->Test->TestTrue(TEXT("Duplicate request does not restart target execution"), Ability->IsActive() && Ability->GetExecutionId() == Execution);
	}
	else
	{
		FHodgeCombatActionTestAccess::ServerComboRequest(F.ASC, Data->TargetHandle, Data->Key, Data->Payload);
		Data->Test->TestFalse(TEXT("Expired or cancelled source cannot be revived by a duplicate"), TargetSpec->IsActive());
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeDeferredComboPipelineTest, "Hodge.Combo.DeferredServerPipeline",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 开窗、超时和来源取消分别穿过现有服务器等待流程，不只测试 Ready／Wait 的即时查询。 */
bool FHodgeDeferredComboPipelineTest::RunTest(const FString&)
{
	for (FName Scenario : {FName(TEXT("Open")), FName(TEXT("Timeout")), FName(TEXT("Cancel"))})
	{
		auto Data = MakeShared<FHodgeDeferredComboFixture>();
		Data->Test = this;
		Data->Scenario = Scenario;
		ADD_LATENT_AUTOMATION_COMMAND(FHodgeVerifyDeferredCombo(Data));
	}
	return true;
}

/** 真实远端权威身份与临时动画配置只属于新用例，不改变旧 Deferred 矩阵的 AI 前提。 */
struct FHodgeDeferredFacingFixture
{
	TSharedPtr<HodgeActionTests::FFixture> Fixture;
	FAutomationTestBase* Test = nullptr;
	FName Scenario;
	FGameplayAbilitySpecHandle TargetHandle;
	UHodgeAbilityDefinition* Target = nullptr;
	FGameplayEventData Payload;
	FPredictionKey Key;
	FHodgeAttackFacingTargetData Facing;
	int32 Step = 0;
	double ExpiresAt = 0.;
	// 只通过原 RPC 实现投递值；网络传输本身仍由 PIE 验证。
	void SendFacing(const FPredictionKey& ForKey, const FHodgeAttackFacingTargetData& Value) const
	{
		Fixture->ASC->ServerSetReplicatedTargetData_Implementation(TargetHandle, ForKey,
			FGameplayAbilityTargetDataHandle(new FHodgeAttackFacingTargetData(Value)), {}, ForKey);
	}
};

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FHodgeVerifyDeferredFacingHandoff, TSharedPtr<FHodgeDeferredFacingFixture>, Data);
/** 跨真实延后 Timer 和引擎 Super 的数据交接；旧版本会在开窗后仍停在 FacingData 等待。 */
bool FHodgeVerifyDeferredFacingHandoff::Update()
{
	if (!Data->Fixture)
	{
		Data->Fixture = MakeShared<HodgeActionTests::FFixture>();
		auto& F = *Data->Fixture;
		if (!Data->Test->TestTrue(TEXT("方向交接基础场景就绪"), F.bReady)) { return true; }
		auto* Controller = F.World->SpawnActor<AHodgeDeferredFacingTestController>();
		if (!Data->Test->TestNotNull(TEXT("远端玩家控制器存在"), Controller)) { return true; }
		// 无GameMode的测试世界不会自动创建PlayerState；原生IsPlayerControlled要求真实非Bot状态。
		FActorSpawnParameters StateSpawn; StateSpawn.Owner = Controller;
		auto* PlayerState = F.World->SpawnActor<APlayerState>(StateSpawn);
		if (!Data->Test->TestNotNull(TEXT("远端玩家状态存在"), PlayerState)) { return true; }
		PlayerState->SetIsABot(false);
		Controller->SetPlayerState(PlayerState);
		Controller->Possess(F.Pawn);
		// 两刀使用同一套真实骨架；仅复制动画通知到内存，不保存正式资源。
		F.Pawn->GetMesh()->SetSkeletalMesh(LoadObject<USkeletalMesh>(nullptr,
			TEXT("/Game/Main/Character/Hero/Anim/Model/SKM_Pover_LyraLab.SKM_Pover_LyraLab")));
		F.Pawn->GetMesh()->SetAnimInstanceClass(UAnimInstance::StaticClass());
		F.ASC->RefreshAbilityActorInfo();
		// 分别验证远端前提，失败报告能直接区分控制权、Pawn状态和GAS缓存。
		bool bRemoteReady = Data->Test->TestTrue(TEXT("方向交接Pawn具有权威"), F.Pawn->HasAuthority());
		bRemoteReady &= Data->Test->TestTrue(TEXT("非Bot PlayerState使Pawn确认为玩家控制"), F.Pawn->IsPlayerControlled());
		bRemoteReady &= Data->Test->TestFalse(TEXT("远端Pawn不属于本地控制"), F.Pawn->IsLocallyControlled());
		bRemoteReady &= Data->Test->TestFalse(TEXT("GAS ActorInfo不属于本地控制"), F.ASC->AbilityActorInfo->IsLocallyControlled());
		bRemoteReady &= Data->Test->TestEqual(TEXT("Pawn持有本次真实PlayerState"), F.Pawn->GetPlayerState(), PlayerState);
		bRemoteReady &= Data->Test->TestEqual(TEXT("GAS缓存本次远端PlayerController"), F.ASC->AbilityActorInfo->PlayerController.Get(), static_cast<APlayerController*>(Controller));
		if (!bRemoteReady) { return true; }
		auto* Original = LoadObject<UAnimMontage>(nullptr,
			TEXT("/Game/Main/Character/Hero/Anim/Montages/AM_Attack01_Montage.AM_Attack01_Montage"));
		if (!Data->Test->TestNotNull(TEXT("真实根运动源动画存在"), Original)) { return true; }
		F.Definition->ExecutionConfig.Montage = DuplicateObject<UAnimMontage>(Original, F.Definition);
		F.Definition->ExecutionConfig.Montage->Notifies.Reset();
		F.Definition->FacingWarp.bEnabled = false;
		F.ConfigureCombo();
		Data->Target = DuplicateObject<UHodgeAbilityDefinition>(F.Definition, F.Pawn);
		Data->Target->AbilityTag = HodgeGameplayTags::Status_Attack_Recovery;
		Data->Target->AbilityClass = UHodgeGameplayAbility_Melee::StaticClass();
		Data->Target->FacingWarp.bEnabled = true;
		Data->Target->FacingWarp.bTrackExecutionLockTargetDuringWarp = false;
		Data->Target->ExecutionConfig.Montage = DuplicateObject<UAnimMontage>(Original, Data->Target);
		auto* Montage = Data->Target->ExecutionConfig.Montage.Get();
		Montage->Notifies.Reset();
		auto* WarpNotify = NewObject<UAnimNotifyState_MotionWarping>(Montage);
		auto* Modifier = NewObject<UHodgeRootMotionModifier_AttackFacing>(WarpNotify);
		Modifier->WarpTargetName = Data->Target->FacingWarp.WarpTargetName;
		WarpNotify->RootMotionModifier = Modifier;
		auto& WarpEvent = Montage->Notifies.AddDefaulted_GetRef();
		WarpEvent.NotifyStateClass = WarpNotify; WarpEvent.Link(Montage, 0.f);
		WarpEvent.SetDuration(.1f); WarpEvent.EndLink.Link(Montage, .1f);
		auto* HitNotify = NewObject<UHodgeAnimNotify_Hit>(Montage);
		HitNotify->Hit.BoneOrSocket = TEXT("root");
		auto& HitEvent = Montage->Notifies.AddDefaulted_GetRef();
		HitEvent.Notify = HitNotify; HitEvent.Link(Montage, .3f);
		TArray<FText> Errors;
		if (!Data->Test->TestTrue(TEXT("临时目标保持完整生产配置校验"), Data->Target->ValidateDefinition(Errors)))
		{
			for (const auto& Error : Errors) { Data->Test->AddError(Error.ToString()); }
			return true;
		}
		Data->TargetHandle = F.ASC->GiveAbilityDefinition(Data->Target, 1, F.Pawn);
		if (!Data->Test->TestTrue(TEXT("开启方向等待的目标获得真实Spec"), Data->TargetHandle.IsValid())) { return true; }
		F.Pawn->GetCharacterRotationComponent()->InitializeWithAbilitySystem(F.ASC);
		auto* Warp = F.Pawn->FindComponentByClass<UHodgeAttackMotionWarpingComponent>();
		if (!Data->Test->TestNotNull(TEXT("原运动修正组件存在"), Warp)) { return true; }
		if (!Warp->HasBeenInitialized()) { Warp->InitializeComponent(); }
		auto* Combo = const_cast<UHodgeComboDefinition*>(F.Pawn->EnemyPawnData->ComboDefinition.Get());
		FHodgeComboRow TargetRow; TargetRow.ComboTag = HodgeGameplayTags::Status_Attack_Recovery;
		TargetRow.AbilityTag = Data->Target->AbilityTag;
		Combo->ComboTable->AddRow(TargetRow.ComboTag.GetTagName(), TargetRow);
		const FGameplayTag SourceTag = HodgeGameplayTags::Status_Attack_Active;
		auto* Source = Combo->ComboTable->FindRow<FHodgeComboRow>(SourceTag.GetTagName(), TEXT("DeferredFacingHandoff"));
		if (!Data->Test->TestNotNull(TEXT("来源保留原关闭窗口边"), Source) ||
			!Data->Test->TestTrue(TEXT("来源转换边存在"), !Source->Transitions.IsEmpty())) { return true; }
		Source->Transitions[0].TargetComboTag = TargetRow.ComboTag;
		// 远端权威不能用无预测键的本地输入冒充客户端；首刀也走真实服务器请求入口。
		FGameplayEventData SourcePayload;
		SourcePayload.EventTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayEvent.Combo.InputRequest"));
		SourcePayload.Instigator = F.Pawn; SourcePayload.OptionalObject = Combo;
		SourcePayload.TargetTags.AddTag(Combo->EntryComboTag);
		SourcePayload.InstigatorTags.AddTag(HodgeGameplayTags::Status_Attack_Cancel_NextAttack);
		SourcePayload.EventMagnitude = FHodgeCombatActionTestAccess::ComboSourceKey(F.Combat);
		FPredictionKey SourceKey; SourceKey.Current = 169;
		FHodgeCombatActionTestAccess::ServerComboRequest(F.ASC, F.Handle, SourceKey, SourcePayload);
		if (!Data->Test->TestTrue(TEXT("第一刀通过正式服务器Combat授权与Super激活"), F.Ability() && F.Ability()->IsActive() &&
			F.ASC->GetAnimatingAbility() == F.Ability())) { return true; }
		Data->Payload.EventTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayEvent.Combo.InputRequest"));
		Data->Payload.Instigator = F.Pawn; Data->Payload.OptionalObject = Combo;
		Data->Payload.TargetTags.AddTag(HodgeGameplayTags::Status_Attack_Active);
		Data->Payload.InstigatorTags.AddTag(HodgeGameplayTags::Status_Attack_Cancel_NextAttack);
		Data->Payload.EventMagnitude = FHodgeCombatActionTestAccess::ComboSourceKey(F.Combat);
		Data->Key.Current = 171;
		Data->Facing.SourceAvatar = F.Pawn;
		Data->Facing.Source = EHodgeAttackFacingSource::Actor;
		Data->Facing.StartYaw = FRotator::NormalizeAxis(F.Pawn->GetActorRotation().Yaw);
		Data->Facing.DesiredYaw = FRotator::NormalizeAxis(Data->Facing.StartYaw + 35.f);
		Data->ExpiresAt = F.World->GetTimeSeconds() + F.Combat->GetServerInputBufferSeconds();
		FHodgeCombatActionTestAccess::ServerComboRequest(F.ASC, Data->TargetHandle, Data->Key, Data->Payload);
		Data->Test->TestEqual(TEXT("激活实际进入Deferred队列"), FHodgeCombatActionTestAccess::PendingComboRequests(F.ASC), 1);
		// 旧Key及旧Avatar都不能冒充本笔方向，再提供唯一有效的当前载荷。
		auto OldKey = Data->Key; --OldKey.Current;
		auto WrongYaw = Data->Facing; WrongYaw.DesiredYaw = FRotator::NormalizeAxis(Data->Facing.StartYaw - 25.f);
		Data->SendFacing(OldKey, WrongYaw);
		auto WrongAvatar = Data->Facing; WrongAvatar.SourceAvatar = F.World->SpawnActor<AActor>();
		Data->SendFacing(Data->Key, WrongAvatar);
		Data->Test->TestFalse(TEXT("旧Avatar没有建立本笔方向缓存"),
			FHodgeCombatActionTestAccess::HasConfirmedTargetData(F.ASC, Data->TargetHandle, Data->Key));
		Data->SendFacing(Data->Key, Data->Facing);
		Data->Test->TestTrue(TEXT("方向RPC实现已在激活前缓存本笔数据"),
			FHodgeCombatActionTestAccess::HasConfirmedTargetData(F.ASC, Data->TargetHandle, Data->Key));
		return false;
	}
	auto& F = *Data->Fixture;
	F.World->Tick(LEVELTICK_All, .01f);
	const auto* Spec = F.ASC->FindAbilitySpecFromHandle(Data->TargetHandle);
	if (!Data->Test->TestNotNull(TEXT("目标Spec在交接期间存在"), Spec)) { return true; }
	if (++Data->Step == 1)
	{
		Data->Test->TestFalse(TEXT("关闭窗口期间缓存方向不会提前激活目标"), Spec->IsActive());
		Data->Test->TestEqual(TEXT("真实Timer仍等待原窗口"), FHodgeCombatActionTestAccess::PendingComboRequests(F.ASC), 1);
		if (Data->Scenario == TEXT("Open"))
		{
			FHodgeCombatActionTestAccess::ServerComboRequest(F.ASC, Data->TargetHandle, Data->Key, Data->Payload);
			FHodgeCombatActionTestAccess::Window(F.Ability(), HodgeGameplayTags::Status_Attack_Cancel_Move);
		}
		else if (Data->Scenario == TEXT("Cancel")) { F.Ability()->FinishExecution(true, false); }
		else if (Data->Scenario == TEXT("Avatar")) { F.ASC->InitAbilityActorInfo(F.Pawn, nullptr); }
		return false;
	}
	if (FHodgeCombatActionTestAccess::PendingComboRequests(F.ASC) > 0 && Data->Step < 100) { return false; }
	Data->Test->TestEqual(TEXT("交接结束后生产等待记录被移除"), FHodgeCombatActionTestAccess::PendingComboRequests(F.ASC), 0);
	const bool bOpen = Data->Scenario == TEXT("Open");
	Data->Test->TestEqual(TEXT("只有真实开窗场景激活目标"), Spec->IsActive(), bOpen);
	if (bOpen)
	{
		auto* Ability = Cast<UHodgeGameplayAbility_Definition>(Spec->GetPrimaryInstance());
		if (!Data->Test->TestNotNull(TEXT("目标通过Super创建活动实例"), Ability)) { return true; }
		Data->Test->TestFalse(TEXT("Super清缓存后仍完整交接方向，不等待第二次RPC"), Ability->IsAwaitingFacingData());
		Data->Test->TestEqual(TEXT("交接真正到达目标Montage播放"), F.ASC->GetAnimatingAbility(), static_cast<UGameplayAbility*>(Ability));
		Data->Test->TestEqual(TEXT("当前片段就是目标临时Montage"), F.ASC->GetCurrentMontage(), Data->Target->ExecutionConfig.Montage.Get());
		Data->Test->TestTrue(TEXT("采用当前Key的35度Yaw，不混入旧Key"), FMath::IsNearlyEqual(Ability->GetAttackFacingYaw(), Data->Facing.DesiredYaw, .01f));
		Data->Test->TestFalse(TEXT("数据按原路径消费一次"), FHodgeCombatActionTestAccess::HasConfirmedTargetData(F.ASC, Data->TargetHandle, Data->Key));
		const FGuid Execution = Ability->GetExecutionId();
		auto Duplicate = Data->Facing; Duplicate.DesiredYaw = FRotator::NormalizeAxis(Data->Facing.StartYaw - 20.f);
		Data->SendFacing(Data->Key, Duplicate);
		FHodgeCombatActionTestAccess::ServerComboRequest(F.ASC, Data->TargetHandle, Data->Key, Data->Payload);
		Data->Test->TestTrue(TEXT("重复和已消费数据不重启或改写本次执行"), Ability->IsActive() && Ability->GetExecutionId() == Execution &&
			FMath::IsNearlyEqual(Ability->GetAttackFacingYaw(), Data->Facing.DesiredYaw, .01f));
		Ability->FinishExecution(true, false);
		Data->SendFacing(Data->Key, Duplicate);
		FHodgeCombatActionTestAccess::ServerComboRequest(F.ASC, Data->TargetHandle, Data->Key, Data->Payload);
		Data->Test->TestFalse(TEXT("结束后旧数据和旧激活不能复活目标"), Spec->IsActive());
	}
	else
	{
		if (Data->Scenario == TEXT("Timeout"))
		{ Data->Test->TestTrue(TEXT("来源有效直到生产截止时刻"), F.Ability() && F.Ability()->IsActive() && F.World->GetTimeSeconds() >= Data->ExpiresAt); }
		Data->SendFacing(Data->Key, Data->Facing);
		FHodgeCombatActionTestAccess::ServerComboRequest(F.ASC, Data->TargetHandle, Data->Key, Data->Payload);
		Data->Test->TestFalse(TEXT("取消、到期或旧Avatar不能因缓存数据复活"), Spec->IsActive());
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeDeferredFacingHandoffTest, "Hodge.Combo.DeferredFacingTargetDataHandoff",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 真实Timer覆盖开窗交接与三项失效边界；不替代独立进程网络传输验收。 */
bool FHodgeDeferredFacingHandoffTest::RunTest(const FString&)
{
	for (FName Scenario : {FName(TEXT("Open")), FName(TEXT("Cancel")), FName(TEXT("Timeout")), FName(TEXT("Avatar"))})
	{
		auto Data = MakeShared<FHodgeDeferredFacingFixture>();
		Data->Test = this; Data->Scenario = Scenario;
		ADD_LATENT_AUTOMATION_COMMAND(FHodgeVerifyDeferredFacingHandoff(Data));
	}
	return true;
}
#endif
