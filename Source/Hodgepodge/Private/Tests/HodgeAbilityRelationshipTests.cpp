#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "AbilitySystem/HodgeAbilityTagRelationshipMapping.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "HodgeRelationshipTestAbility.h"
#include "Engine/World.h"

struct FHodgeRelationshipTestAccess
{
	static void SetRules(UHodgeAbilityTagRelationshipMapping* Map, TArray<FHodgeAbilityTagRelationship> Rules)
	{ Map->AbilityTagRelationships = MoveTemp(Rules); }
	static void Execute(UHodgeAbilitySystemComponent* ASC, FGameplayTag Source)
	{ ASC->ApplyAbilityBlockAndCancelTags(FGameplayTagContainer(Source), nullptr, false, {}, true, {}); }
};
namespace HodgeRelationshipTests
{
	FGameplayTag Attack() { return FGameplayTag::RequestGameplayTag(TEXT("Ability.Attack")); }
	UHodgeAbilityTagRelationshipMapping* MakeMapping()
	{
		auto* Map = NewObject<UHodgeAbilityTagRelationshipMapping>();
		FHodgeAbilityTagRelationship Row; Row.AbilityTag = HodgeGameplayTags::Ability_Type_Action_Dash;
		Row.AbilityTagsToCancel.AddTag(Attack()); Row.bForceCancel = true;
		FHodgeRelationshipTestAccess::SetRules(Map, {Row}); return Map;
	}
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeRelationshipRuleTest, "Hodge.Relationship.PriorityAndHierarchy", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeRelationshipRuleTest::RunTest(const FString& Parameters)
{
	using namespace HodgeRelationshipTests;
	auto* Map = MakeMapping(); const FGameplayTag Dash = HodgeGameplayTags::Ability_Type_Action_Dash;
	FGameplayTagContainer Target(FGameplayTag::RequestGameplayTag(TEXT("Ability.Attack.Light.01")));
	TestTrue(TEXT("Parent cancel tag matches attack child"), Map->IsAbilityCancelledByTag(Target, Dash));
	TestTrue(TEXT("Force is authorized by mapping"), Map->IsForcedCancellation(FGameplayTagContainer(Dash), Target));
	Target.AddTag(HodgeGameplayTags::Ability_Type_StatusChange_Death);
	TestFalse(TEXT("Death wins over attack on same ability"), Map->IsAbilityCancelledByTag(Target, Dash));
	TestFalse(TEXT("Force cannot override Death"), Map->IsForcedCancellation(FGameplayTagContainer(Dash), Target));
	Target.RemoveTag(HodgeGameplayTags::Ability_Type_StatusChange_Death); Target.AddTag(HodgeGameplayTags::Status_Death_Dying);
	TestFalse(TEXT("Death status child is protected too"), Map->IsAbilityCancelledByTag(Target, Dash));
	TestFalse(TEXT("Nonmatching source cannot authorize cancellation"), Map->IsAbilityCancelledByTags(FGameplayTagContainer(HodgeGameplayTags::Ability_Type_Action_Emote), FGameplayTagContainer(Attack())));
	TestFalse(TEXT("Unrelated ability is not cancelled"), Map->IsAbilityCancelledByTag(FGameplayTagContainer(HodgeGameplayTags::Ability_Type_Action_Emote), Dash));
	FGameplayTagContainer Required, Blocked; Map->GetRequiredAndBlockedActivationTags(FGameplayTagContainer(Dash), &Required, &Blocked);
	TestTrue(TEXT("Dash always blocked by death state"), Blocked.HasTag(HodgeGameplayTags::Status_Death));
	FHodgeAbilityTagRelationship Row; Row.AbilityTag=Dash; Row.AbilityTagsToCancel.AddTag(Attack()); Row.AbilityTagsToCancelExceptions.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Attack.Light.01")));
	TestFalse(TEXT("Normal cancellation is not force authorization"), Map->IsForcedCancellation(FGameplayTagContainer(HodgeGameplayTags::Ability_Type_Action_Emote), FGameplayTagContainer(Attack())));
	FHodgeRelationshipTestAccess::SetRules(Map, {Row});
	TestFalse(TEXT("Configured exceptions override cancel tags"), Map->IsAbilityCancelledByTag(FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("Ability.Attack.Light.01"))), Dash));
	TestFalse(TEXT("Empty tags cannot cancel"), Map->IsAbilityCancelledByTags({}, {})); return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeRelationshipRuntimeTest, "Hodge.Relationship.ActualCancellation", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeRelationshipRuntimeTest::RunTest(const FString& Parameters)
{
	using namespace HodgeRelationshipTests;
	const auto Values=UWorld::InitializationValues().AllowAudioPlayback(false).RequiresHitProxies(false).CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false);
	auto* World=UWorld::CreateWorld(EWorldType::Game,false,NAME_None,nullptr,true,ERHIFeatureLevel::Num,&Values);
	auto* Owner=World->SpawnActor<AActor>(); auto* ASC=NewObject<UHodgeAbilitySystemComponent>(Owner); ASC->RegisterComponent(); ASC->InitAbilityActorInfo(Owner,Owner);
	ASC->SetTagRelationshipMapping(MakeMapping());
	auto Grant = [ASC](FGameplayTagContainer Tags)
	{
		FGameplayAbilitySpec Spec(UHodgeRelationshipTestAbility::StaticClass(),1); Spec.GetDynamicSpecSourceTags().AppendTags(Tags);
		const auto Handle=ASC->GiveAbility(Spec); ASC->TryActivateAbility(Handle); return Handle;
	};
	const auto AttackHandle=Grant(FGameplayTagContainer(Attack()));
	FGameplayTagContainer Dual(Attack()); Dual.AddTag(HodgeGameplayTags::Ability_Type_StatusChange_Death);
	const auto DeathHandle=Grant(Dual); const auto UtilityHandle=Grant(FGameplayTagContainer(HodgeGameplayTags::Ability_Type_Action_Emote));
	auto* AttackInstance=ASC->FindAbilitySpecFromHandle(AttackHandle)->GetPrimaryInstance(); AttackInstance->SetCanBeCanceled(false);
	TestTrue(TEXT("Attack is actually active and noncancelable"), AttackInstance->IsActive() && !AttackInstance->CanBeCanceled());
	TestTrue(TEXT("Active death classification is visible through dynamic spec tags"), ASC->HasActiveAbilityTag(HodgeGameplayTags::Ability_Type_StatusChange_Death));
	FHodgeAbilityTagRelationship Normal; Normal.AbilityTag = HodgeGameplayTags::Ability_Type_Action_Dash; Normal.AbilityTagsToCancel.AddTag(Attack());
	auto* NormalMap = NewObject<UHodgeAbilityTagRelationshipMapping>(); FHodgeRelationshipTestAccess::SetRules(NormalMap, {Normal});
	ASC->SetTagRelationshipMapping(NormalMap);
	FHodgeRelationshipTestAccess::Execute(ASC,HodgeGameplayTags::Ability_Type_Action_Dash);
	TestTrue(TEXT("Mapping without force preserves a noncancelable phase"), ASC->FindAbilitySpecFromHandle(AttackHandle)->IsActive());
	ASC->SetTagRelationshipMapping(MakeMapping());
	FHodgeRelationshipTestAccess::Execute(ASC,HodgeGameplayTags::Ability_Type_Action_Dash);
	TestFalse(TEXT("Mapped force cancels attack's protected phase"), ASC->FindAbilitySpecFromHandle(AttackHandle)->IsActive());
	TestTrue(TEXT("Attack plus Death survives"), ASC->FindAbilitySpecFromHandle(DeathHandle)->IsActive());
	TestTrue(TEXT("Unrelated active utility survives"), ASC->FindAbilitySpecFromHandle(UtilityHandle)->IsActive());
	const auto Second=Grant(FGameplayTagContainer(Attack())); ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Death_Dead);
	FHodgeRelationshipTestAccess::Execute(ASC,HodgeGameplayTags::Ability_Type_Action_Dash);
	TestTrue(TEXT("Owner death state vetoes all Dash cancellation"), ASC->FindAbilitySpecFromHandle(Second)->IsActive());
	ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Death_Dead);
	ASC->CancelAbilities(); World->DestroyWorld(false); return true;
}
#endif
