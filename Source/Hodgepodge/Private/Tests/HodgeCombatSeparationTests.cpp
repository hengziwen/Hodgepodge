/** @file HodgeCombatSeparationTests.cpp @brief 独立检测资源清理与角色规则身份、阶段和重入契约。 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "HodgeEnemyAITestSupport.h"
#include "HodgeAttackRuleTestAbility.h"
#include "Combat/HodgeAttackRule.h"
#include "Data/HodgeAbilityDefinition.h"
#include "Data/HodgeHavocAttackRule.h"
#include "Data/HodgeResonanceAttackRule.h"
#include "Component/HodgeCombatComponentBase.h"
#include "Component/HodgeHitDetectionComponent.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"
#include "AbilitySystem/AttributeSet/HodgeHavocSet.h"
#include "AbilitySystem/AttributeSet/HodgeResonanceSet.h"
#include "AbilitySystem/Effects/HodgeResonanceEnergyEffect.h"
#include "AbilitySystem/Effects/HodgeHavocEffects.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/StrongObjectPtr.h"

/** 仅建立隔离执行身份；创建、分发、结束均调用生产实现。 */
struct FHodgeAttackRuleTestAccess
{
	static bool Start(UHodgeGameplayAbility_Definition* Source, const UHodgeAbilityDefinition* Data)
	{ Source->ExecutionId = FGuid::NewGuid(); Source->bEnding = false; return Data->CreateAttackRules(Source, Source->AttackRules); }
	static void Publish(UHodgeGameplayAbility_Definition* Source, const FHodgeAttackSettlement& Settlement)
	{ Source->PublishAttackSettlement(Settlement); }
	static UHodgeAttackRule* First(UHodgeGameplayAbility_Definition* Source) { return Source->AttackRules[0]; }
	static int32 Count(UHodgeGameplayAbility_Definition* Source) { return Source->AttackRules.Num(); }
};

namespace
{
	/** 每个用例拥有独立世界及真实资源 GE，不修改共享 CDO。 */
	struct FRuleWorld
	{
		UGameInstance* Instance;
		UWorld* World;
		APawn* Pawn;
		UHodgeAbilitySystemComponent* ASC;
		UHodgeAttackRuleTestAbility* Source;
		FGameplayAbilitySpecHandle Handle;
		FRuleWorld()
		{
			World = FHodgeEnemyInitTestAccess::CreateWorld(Instance); Pawn = World->SpawnActor<APawn>();
			ASC = NewObject<UHodgeAbilitySystemComponent>(Pawn); ASC->RegisterComponent(); ASC->InitAbilityActorInfo(Pawn, Pawn);
			ASC->AddAttributeSetSubobject(NewObject<UHodgeHealthSet>(Pawn)); ASC->AddAttributeSetSubobject(NewObject<UHodgeCombatSet>(Pawn));
			ASC->AddAttributeSetSubobject(NewObject<UHodgeResonanceSet>(Pawn)); ASC->AddAttributeSetSubobject(NewObject<UHodgeHavocSet>(Pawn));
			ASC->SetNumericAttributeBase(UHodgeHealthSet::GetMaxHealthAttribute(), 100.f); ASC->SetNumericAttributeBase(UHodgeHealthSet::GetHealthAttribute(), 100.f);
			ASC->ApplyGameplayEffectToSelf(GetDefault<UHodgeResonanceEnergyInitialization>(), 1.f, ASC->MakeEffectContext());
			ASC->ApplyGameplayEffectToSelf(GetDefault<UHodgeDarkFlowInitialization>(), 1.f, ASC->MakeEffectContext());
			ASC->GiveAbility(FGameplayAbilitySpec(UHodgeGameplayAbility_HavocRules::StaticClass()));
			Handle = ASC->GiveAbility(FGameplayAbilitySpec(UHodgeAttackRuleTestAbility::StaticClass()));
			ASC->TryActivateAbility(Handle); Source = Cast<UHodgeAttackRuleTestAbility>(ASC->FindAbilitySpecFromHandle(Handle)->GetPrimaryInstance());
		}
		~FRuleWorld() { ASC->ClearAllAbilities(); FHodgeEnemyInitTestAccess::DestroyWorld(Instance, World); }
		float Energy() const { return ASC->GetSet<UHodgeResonanceSet>()->GetResonanceEnergy(); }
		float Flow() const { return ASC->GetSet<UHodgeHavocSet>()->GetDarkFlow(); }
		FHodgeAttackSettlement Settlement() const
		{
			FHodgeAttackSettlement Value; Value.ExecutionId = Source->GetExecutionId(); Value.SettlementId = FGuid::NewGuid();
			Value.AbilityHandle = Handle; Value.HitGroup = TEXT("Attack"); Value.AttackPhase = TEXT("Strike"); Value.AcceptedDamage = 10.f; return Value;
		}
		UHodgeAbilityDefinition* Data(bool bHavoc = true)
		{
			auto* Value = NewObject<UHodgeAbilityDefinition>();
			auto* Energy = NewObject<UHodgeResonanceAttackRuleConfig>(Value); Energy->GainPerPhase = FScalableFloat(5.f); Value->AttackRules.Add(Energy);
			if (bHavoc) { auto* Flow = NewObject<UHodgeHavocAttackRuleConfig>(Value); Flow->DarkFlowGainPerPhase = 5.f; Value->AttackRules.Add(Flow); }
			return Value;
		}
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeDetectionOwnerCleanupTest, "Hodge.Refactor.DetectionOwnerCleanup", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 独立服务被移除时释放会话与专服姿势；旧身份不能清理后来占用。 */
bool FHodgeDetectionOwnerCleanupTest::RunTest(const FString&)
{
	UGameInstance* Instance; UWorld* World = FHodgeEnemyInitTestAccess::CreateWorld(Instance);
	auto* Character = World->SpawnActor<ACharacter>(); auto* Combat = NewObject<UHodgeCombatComponentBase>(Character); Combat->RegisterComponent();
	auto* Detection = Combat->GetHitDetectionComponent();
	const auto Policy = Character->GetMesh()->VisibilityBasedAnimTickOption;
	const auto First = Combat->AcquirePoseLease(), Second = Combat->AcquirePoseLease();
	TestTrue(TEXT("Authority receives independent pose leases"), First.IsValid() && Second.IsValid() && First != Second);
	FHodgeHitDetectionRequest Request; Request.Profile = NewObject<UHodgeHitDetectionProfile>(Detection);
	Request.Volume.GeometryMode = EHodgeHitGeometryMode::ConfiguredShape; Request.Volume.AnchorKind = EHodgeHitAnchorKind::AvatarRoot;
	const FGuid Execution = FGuid::NewGuid(); const auto Handle = Combat->CreateDetectionSession(Execution, 0, Request);
	TestTrue(TEXT("Facade creates session in independent owner"), Handle != 0 && Detection->GetDetectionSessionCount() == 1);
	Detection->UnregisterComponent();
	TestEqual(TEXT("Unregister releases geometry sessions"), Combat->GetDetectionSessionCount(), 0);
	TestEqual(TEXT("Unregister releases all pose leases"), Combat->GetPoseLeaseCount(), 0);
	TestTrue(TEXT("Original skeletal refresh policy restored"), Character->GetMesh()->VisibilityBasedAnimTickOption == Policy);
	Detection->RegisterComponent(); Detection->EnableDetection(); const auto NewLease = Detection->AcquirePoseLease();
	Detection->ReleasePoseLease(First); TestEqual(TEXT("Old lease cannot release newer owner"), Detection->GetPoseLeaseCount(), 1);
	Detection->ReleasePoseLease(NewLease); Detection->Shutdown();
	FHodgeEnemyInitTestAccess::DestroyWorld(Instance, World); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAttackRulePhaseTest, "Hodge.Refactor.RulePhaseAndIdentity", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 两种收益各自限阶段；旧执行、错 Spec 和旧 Avatar 都不能继续回能。 */
bool FHodgeAttackRulePhaseTest::RunTest(const FString&)
{
	FRuleWorld F; auto* Data = F.Data();
	if (!TestTrue(TEXT("Real GAS source activated"), F.Source && F.Source->IsActive())) { return false; }
	TestTrue(TEXT("Rules created"), FHodgeAttackRuleTestAccess::Start(F.Source, Data));
	auto Value = F.Settlement(); Value.AcceptedDamage = 0.f; FHodgeAttackRuleTestAccess::Publish(F.Source, Value);
	TestEqual(TEXT("Zero damage does not reserve phase"), F.Energy(), 0.f);
	Value.AcceptedDamage = 10.f; FHodgeAttackRuleTestAccess::Publish(F.Source, Value);
	TestEqual(TEXT("Energy phase gain"), F.Energy(), 5.f); TestEqual(TEXT("Independent flow phase gain"), F.Flow(), 5.f);
	Value.SettlementId = FGuid::NewGuid(); FHodgeAttackRuleTestAccess::Publish(F.Source, Value);
	TestEqual(TEXT("Second target in same phase gives no duplicate"), F.Energy(), 5.f); TestEqual(TEXT("Flow is also once per phase"), F.Flow(), 5.f);
	Value.AttackPhase = TEXT("Second"); FHodgeAttackRuleTestAccess::Publish(F.Source, Value);
	TestEqual(TEXT("New phase remains eligible"), F.Energy(), 10.f);
	TStrongObjectPtr<UHodgeAttackRule> Old(FHodgeAttackRuleTestAccess::First(F.Source));
	FHodgeAttackRuleTestAccess::Start(F.Source, Data); Old->OnSettlement(Value);
	TestEqual(TEXT("Old rule cannot follow GA reuse"), F.Energy(), 10.f);
	Value = F.Settlement(); Value.AbilityHandle = {}; FHodgeAttackRuleTestAccess::Publish(F.Source, Value);
	TestEqual(TEXT("Wrong Spec rejected"), F.Energy(), 10.f);
	Value = F.Settlement(); FHodgeAttackRuleTestAccess::Publish(F.Source, Value); TestEqual(TEXT("Fresh execution resets phase"), F.Energy(), 15.f);
	F.ASC->InitAbilityActorInfo(F.Pawn, F.World->SpawnActor<APawn>()); Value.AttackPhase = TEXT("AfterRebind"); FHodgeAttackRuleTestAccess::Publish(F.Source, Value);
	TestEqual(TEXT("Old Avatar rule rejected"), F.Energy(), 15.f); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAttackRuleReentryTest, "Hodge.Refactor.RuleSynchronousReentry", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** GE 广播重入同一结算时，各规则预占自己的账本，不能重复应用收益。 */
bool FHodgeAttackRuleReentryTest::RunTest(const FString&)
{
	FRuleWorld F; FHodgeAttackRuleTestAccess::Start(F.Source, F.Data()); const auto Value = F.Settlement();
	const auto Listener = F.ASC->GetGameplayAttributeValueChangeDelegate(UHodgeResonanceSet::GetResonanceEnergyAttribute()).AddLambda(
		[&](const FOnAttributeChangeData&) { FHodgeAttackRuleTestAccess::Publish(F.Source, Value); });
	FHodgeAttackRuleTestAccess::Publish(F.Source, Value);
	F.ASC->GetGameplayAttributeValueChangeDelegate(UHodgeResonanceSet::GetResonanceEnergyAttribute()).Remove(Listener);
	TestEqual(TEXT("Nested energy still once"), F.Energy(), 5.f); TestEqual(TEXT("Nested other resource still once"), F.Flow(), 5.f); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAttackRuleCompatibilityTest, "Hodge.Refactor.RuleNewConfigPrecedence", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 新配置与旧字段共存只运行一次，明确的配置错误仍可诊断。 */
bool FHodgeAttackRuleCompatibilityTest::RunTest(const FString&)
{
	FRuleWorld F; auto* Data = F.Data(); Data->DarkFlowGainPerPhase = 99.f; Data->ResonanceEnergyGainPerPhase = FScalableFloat(99.f);
	FHodgeAttackRuleTestAccess::Start(F.Source, Data); TestEqual(TEXT("Only two explicit rules"), FHodgeAttackRuleTestAccess::Count(F.Source), 2);
	FHodgeAttackRuleTestAccess::Publish(F.Source, F.Settlement()); TestEqual(TEXT("Explicit energy wins"), F.Energy(), 5.f); TestEqual(TEXT("Explicit flow wins"), F.Flow(), 5.f);
	auto* Legacy = NewObject<UHodgeAbilityDefinition>(); Legacy->DarkFlowGainPerPhase = 3.f; Legacy->ResonanceEnergyGainPerPhase = FScalableFloat(2.f);
	FHodgeAttackRuleTestAccess::Start(F.Source, Legacy); FHodgeAttackRuleTestAccess::Publish(F.Source, F.Settlement());
	TestEqual(TEXT("Legacy delegates same rule implementation"), F.Energy(), 7.f); TestEqual(TEXT("Legacy flow remains compatible"), F.Flow(), 8.f); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAttackRuleCastTest, "Hodge.Refactor.RuleCastCommitOnce", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 明确施放事件独立于命中，无事件不回能，重复事件只给一次。 */
bool FHodgeAttackRuleCastTest::RunTest(const FString&)
{
	FRuleWorld F; auto* Data = NewObject<UHodgeAbilityDefinition>(); auto* Rule = NewObject<UHodgeHavocAttackRuleConfig>(Data); Rule->DarkFlowGainOnCast = 20.f; Data->AttackRules.Add(Rule);
	FHodgeAttackRuleTestAccess::Start(F.Source, Data); TestEqual(TEXT("Before commit no gain"), F.Flow(), 0.f);
	F.Source->SendExecutionEvent(HodgeHavocTags::CastCommitted); F.Source->SendExecutionEvent(HodgeHavocTags::CastCommitted);
	TestEqual(TEXT("Empty cast committed once"), F.Flow(), 20.f);
	F.ASC->ClearAbility(F.Handle); F.Source->SendExecutionEvent(HodgeHavocTags::CastCommitted);
	TestEqual(TEXT("Ended execution cannot commit"), F.Flow(), 20.f); return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAttackRuleCancellationTest, "Hodge.Refactor.RuleCancellationDuringSettlement", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 第一项收益回调结束能力并触发 GC 后，其余旧规则不能继续结算。 */
bool FHodgeAttackRuleCancellationTest::RunTest(const FString&)
{
	FRuleWorld F; FHodgeAttackRuleTestAccess::Start(F.Source, F.Data()); const auto Value = F.Settlement();
	const auto Listener = F.ASC->GetGameplayAttributeValueChangeDelegate(UHodgeResonanceSet::GetResonanceEnergyAttribute()).AddLambda(
		[&](const FOnAttributeChangeData&)
		{ F.ASC->ClearAbility(F.Handle); CollectGarbage(RF_NoFlags); });
	FHodgeAttackRuleTestAccess::Publish(F.Source, Value);
	F.ASC->GetGameplayAttributeValueChangeDelegate(UHodgeResonanceSet::GetResonanceEnergyAttribute()).Remove(Listener);
	TestNull(TEXT("Source spec was revoked during callback"), F.ASC->FindAbilitySpecFromHandle(F.Handle));
	TestEqual(TEXT("Already committed energy retained"), F.Energy(), 5.f);
	TestEqual(TEXT("Ended execution cannot apply the next resource rule"), F.Flow(), 0.f); return true;
}
#endif
