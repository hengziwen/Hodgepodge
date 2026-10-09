#include "HodgeHitReactionTestAbility.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeHitReactionTestAbility)
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Combat/HodgeHitReactionTypes.h"
#include "Component/HodgeHitReactionComponent.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Component/HodgeCharacterMovementComponent.h"
#include "Character/HodgeCombatCharacter.h"
#include "Character/HodgeEnemyCharacter.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystem/HodgeGameplayEffectContext.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_HitReaction.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Jump.h"
#include "Data/HodgePawnData.h"
#include "Data/HodgeHitReactionProfile.h"
#include "Data/HodgeCharacterStatProfile.h"
#include "GameplayEffect.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimMontage.h"
#include "Engine/SkeletalMesh.h"

struct FHodgeHitReactionTestAccess
{
	static void Expire(UHodgeHitReactionComponent* Component) { Component->State.EndsAt = Component->GetServerTime() - 1.f; }
};

namespace HodgeReactionTests
{
	struct FFixture
	{
		UWorld* World;
		AHodgeCombatCharacter* Pawn;
		UHodgeAbilitySystemComponent* ASC;
		UHodgeHitReactionComponent* Reaction;
		UHodgeHitReactionProfile* Profile;
		FFixture()
		{
			const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false);
			World = UWorld::CreateWorld(EWorldType::EditorPreview, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
			Pawn = World->SpawnActor<AHodgeCombatCharacter>();
			Pawn->GetCharacterMovement()->SetUpdatedComponent(Pawn->GetCapsuleComponent());
			Pawn->GetCharacterMovement()->Activate();
			Pawn->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			ASC = NewObject<UHodgeAbilitySystemComponent>(Pawn);
			Pawn->AddInstanceComponent(ASC);
			ASC->RegisterComponent();
			ASC->AddAttributeSetSubobject(NewObject<UHodgeHealthSet>(Pawn));
			ASC->InitAbilityActorInfo(Pawn, Pawn);
			Profile = NewObject<UHodgeHitReactionProfile>(Pawn);
			for (auto Type : {EHodgeImpactType::HitStun, EHodgeImpactType::Knockback, EHodgeImpactType::Launch, EHodgeImpactType::Knockdown, EHodgeImpactType::AirHit, EHodgeImpactType::Slam})
			{
				FHodgeHitReactionAnimation Entry;
				Entry.Type = Type;
				Entry.bAllowWithoutMontage = true;
				Profile->Animations.Add(Entry);
			}
			Reaction = Pawn->GetHitReactionComponent();
			Reaction->OverrideProfile = Profile;
			auto* Data = NewObject<UHodgePawnData>(Pawn);
			Data->HitReactionProfile = Profile;
			auto* Extension = UHodgePawnExtensionComponent::FindPawnExtensionComponent(Pawn);
			Extension->SetPawnData(Data);
			Extension->InitializeAbilitySystem(ASC, Pawn);
			ASC->GiveAbility(FGameplayAbilitySpec(UHodgeGameplayAbility_HitReaction::StaticClass(), 1));
		}
		~FFixture() { Reaction->UninitializeFromAbilitySystem(); World->DestroyWorld(false); }
		FHodgeHitReactionConfig Request(EHodgeImpactType Type = EHodgeImpactType::HitStun) const
		{
			FHodgeHitReactionConfig Config;
			Config.AttackJudgementTag = HodgeHitReactionTags::Judgement_Skill;
			FHodgeImpactSpec Impact;
			Impact.Type = Type;
			Config.Impacts.Add(Impact);
			return Config;
		}
		void Damage(const FGuid& Id, float Amount)
		{
			auto* Effect = NewObject<UGameplayEffect>();
			Effect->DurationPolicy = EGameplayEffectDurationType::Instant;
			FGameplayModifierInfo Modifier;
			Modifier.Attribute = UHodgeHealthSet::GetDamageAttribute();
			Modifier.ModifierOp = EGameplayModOp::Additive;
			Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(Amount));
			Effect->Modifiers.Add(Modifier);
			auto* Context = new FHodgeGameplayEffectContext();
			Context->HitSettlementId = Id;
			FGameplayEffectSpec Spec(Effect, FGameplayEffectContextHandle(Context), 1.f);
			ASC->ApplyGameplayEffectSpecToSelf(Spec);
		}
		FHodgeHitReactionResult Hit(const FHodgeHitReactionConfig& Config, float Amount = 10.f)
		{
			const FGuid Id = Reaction->BeginHit(Config, Pawn, Pawn->GetActorLocation() - FVector(100.f, 0.f, 0.f));
			Damage(Id, Amount);
			return Reaction->FinishHit(Id);
		}
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionRanksTest, "Hodge.HitReaction.RanksAndValidation", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeReactionRanksTest::RunTest(const FString& Parameters)
{
	using namespace HodgeHitReactionTags;
	const FGameplayTag Bodies[] = {Body_Normal, Body_Skill, Body_SuperArmor, Body_Vajra};
	const FGameplayTag Attacks[] = {Judgement_Normal, Judgement_Skill, Judgement_SuperArmor, Judgement_Vajra};
	for (int32 A = 0; A < 4; ++A)
	{
		for (int32 B = 0; B < 4; ++B)
		{ TestEqual(FString::Printf(TEXT("Judgement %d against body %d"), A + 1, B + 1), HodgeHitReaction::CanImpact(HodgeHitReaction::JudgementRank(Attacks[A]), HodgeHitReaction::BodyRank(Bodies[B])), A >= B); }
	}
	TestFalse(TEXT("Unknown judgement never breaks untagged targets"), HodgeHitReaction::CanImpact(0, 0));
	TestTrue(TEXT("Normal works against no-body"), HodgeHitReaction::CanImpact(1, 0));
	const auto* HeroMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/Main/Character/Hero/Anim/Model/SKM_Pover_LyraLab.SKM_Pover_LyraLab"));
	const auto* HeroMontage = LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Main/Character/Hero/Anim/Montages/AM_Attack01_Montage.AM_Attack01_Montage"));
	TestTrue(TEXT("Current hero rig accepts compatible skeleton animation assets"), HodgeHitReaction::CanUseMontage(HeroMontage, HeroMesh));
	FHodgeHitReactionConfig Config;
	Config.AttackJudgementTag = Judgement_Skill;
	FHodgeImpactSpec Launch; Launch.Type = EHodgeImpactType::Launch;
	FHodgeImpactSpec Push; Push.Type = EHodgeImpactType::Knockback;
	Config.Impacts = {Launch, Push};
	TArray<FText> Errors;
	TestFalse(TEXT("Conflicting movement effects are rejected"), Config.Validate(Errors));
	Config.Impacts = {Launch}; Config.ReservedPoiseDamage = -1.f; Errors.Reset();
	TestFalse(TEXT("Negative reserved poise is rejected"), Config.Validate(Errors));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionSettlementTest, "Hodge.HitReaction.DamageAndBodySnapshot", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeReactionSettlementTest::RunTest(const FString& Parameters)
{
	HodgeReactionTests::FFixture F;
	F.ASC->AddLooseGameplayTag(HodgeHitReactionTags::Body_SuperArmor);
	const FGuid Id = F.Reaction->BeginHit(F.Request(), F.Pawn, FVector::ZeroVector);
	F.ASC->RemoveLooseGameplayTag(HodgeHitReactionTags::Body_SuperArmor);
	F.Damage(Id, 20.f);
	const auto Result = F.Reaction->FinishHit(Id);
	TestEqual(TEXT("Body is captured before callbacks can remove it"), Result.TargetBodyRank, 3);
	TestEqual(TEXT("Lower judgement only gives feedback"), Result.Outcome, EHodgeHitReactionOutcome::LowJudgement);
	TestEqual(TEXT("Low judgement still damages health"), F.ASC->GetSet<UHodgeHealthSet>()->GetHealth(), 80.f);
	TestFalse(TEXT("Feedback does not take control"), F.Reaction->IsControlled());
	TestEqual(TEXT("Same settlement cannot be consumed twice"), F.Reaction->FinishHit(Id).Outcome, EHodgeHitReactionOutcome::NoRequest);
	F.ASC->AddLooseGameplayTag(HodgeHitReactionTags::Body_Skill);
	const auto Equal = F.Hit(F.Request());
	TestEqual(TEXT("Equal judgement actually starts target reaction"), Equal.Outcome, EHodgeHitReactionOutcome::Applied);
	TestTrue(TEXT("Target owns control state"), F.ASC->HasMatchingGameplayTag(HodgeHitReactionTags::Controlled));
	F.Reaction->CancelReaction();
	TestFalse(TEXT("Cancel releases control"), F.ASC->HasMatchingGameplayTag(HodgeHitReactionTags::Controlled));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionImmuneTest, "Hodge.HitReaction.ImmunityNoOutputAndDeath", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeReactionImmuneTest::RunTest(const FString& Parameters)
{
	HodgeReactionTests::FFixture F;
	F.ASC->AddLooseGameplayTag(TAG_Gameplay_DamageImmunity);
	TestEqual(TEXT("Attribute immunity rejects the pending reaction"), F.Hit(F.Request()).Outcome, EHodgeHitReactionOutcome::Rejected);
	TestEqual(TEXT("Immunity keeps health"), F.ASC->GetSet<UHodgeHealthSet>()->GetHealth(), 100.f);
	F.ASC->RemoveLooseGameplayTag(TAG_Gameplay_DamageImmunity);
	const FGuid NoOutput = F.Reaction->BeginHit(F.Request(), F.Pawn, FVector::ZeroVector);
	TestEqual(TEXT("No damage callback does not imply accepted control"), F.Reaction->FinishHit(NoOutput).Outcome, EHodgeHitReactionOutcome::NoDamageOutput);
	TestEqual(TEXT("Lethal damage cannot start ordinary Impact"), F.Hit(F.Request(), 200.f).Outcome, EHodgeHitReactionOutcome::Dead);
	TestFalse(TEXT("Death has no ordinary reaction"), F.Reaction->IsControlled());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionLifecycleTest, "Hodge.HitReaction.UpdateExpiryAndDetach", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeReactionLifecycleTest::RunTest(const FString& Parameters)
{
	HodgeReactionTests::FFixture F;
	TestEqual(TEXT("First hit accepted"), F.Hit(F.Request()).Outcome, EHodgeHitReactionOutcome::Applied);
	const int32 FirstSequence = F.Reaction->GetReactionState().Sequence;
	TestEqual(TEXT("Active InstancedPerActor reaction can be updated"), F.Hit(F.Request()).Outcome, EHodgeHitReactionOutcome::Applied);
	TestTrue(TEXT("Update has a new identity"), F.Reaction->GetReactionState().Sequence > FirstSequence);
	F.Reaction->CompletePlan(FirstSequence);
	TestTrue(TEXT("Old completion cannot clear new plan"), F.Reaction->IsControlled());
	FHodgeHitReactionTestAccess::Expire(F.Reaction);
	F.Reaction->TickPlan();
	TestFalse(TEXT("Time expiry releases native reaction"), F.Reaction->IsControlled());
	F.ASC->AddLooseGameplayTag(HodgeHitReactionTags::Body_Normal, 2);
	F.ASC->AddLooseGameplayTag(HodgeHitReactionTags::Body_Vajra);
	FGameplayTag Body;
	TestEqual(TEXT("Highest active body wins"), HodgeHitReaction::ResolveBody(F.ASC, Body), 4);
	F.ASC->RemoveLooseGameplayTag(HodgeHitReactionTags::Body_Vajra);
	TestEqual(TEXT("Removing high body exposes lower holders"), HodgeHitReaction::ResolveBody(F.ASC, Body), 1);
	TestEqual(TEXT("Reaction starts again"), F.Hit(F.Request()).Outcome, EHodgeHitReactionOutcome::Applied);
	F.Reaction->UninitializeFromAbilitySystem();
	TestFalse(TEXT("Avatar detach releases only owned control"), F.ASC->HasMatchingGameplayTag(HodgeHitReactionTags::Controlled));
	TestTrue(TEXT("Detach preserves unrelated body holders"), F.ASC->HasMatchingGameplayTag(HodgeHitReactionTags::Body_Normal));
	F.Reaction->InitializeWithAbilitySystem(F.ASC);
	TestEqual(TEXT("Old avatar can begin Launch before replacement"), F.Hit(F.Request(EHodgeImpactType::Launch)).Outcome, EHodgeHitReactionOutcome::Applied);
	auto* Replacement = F.World->SpawnActor<AHodgeCombatCharacter>();
	Replacement->GetCharacterMovement()->SetUpdatedComponent(Replacement->GetCapsuleComponent());
	Replacement->GetCharacterMovement()->Activate();
	Replacement->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	const FVector ReplacementLaunch(180.f, 0.f, 650.f);
	Replacement->GetCharacterMovement()->Launch(ReplacementLaunch);
	TestTrue(TEXT("Replacement has its own pending movement before rebinding"), Replacement->GetCharacterMovement()->PendingLaunchVelocity.Equals(ReplacementLaunch));
	F.ASC->InitAbilityActorInfo(F.Pawn, Replacement);
	F.Reaction->UninitializeFromAbilitySystem();
	TestTrue(TEXT("Old reaction cleanup cannot clear new avatar pending movement"), Replacement->GetCharacterMovement()->PendingLaunchVelocity.Equals(ReplacementLaunch));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionFallbackTest, "Hodge.HitReaction.FallbackAndEmptyImpact", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeReactionFallbackTest::RunTest(const FString& Parameters)
{
	HodgeReactionTests::FFixture F;
	F.Profile->Animations.RemoveAll([](const auto& Entry) { return Entry.Type != EHodgeImpactType::HitStun; });
	TestEqual(TEXT("Unsupported Launch does not pretend success"), F.Hit(F.Request(EHodgeImpactType::Launch)).Outcome, EHodgeHitReactionOutcome::Unsupported);
	F.Profile->FallbackToHitStun.Add(EHodgeImpactType::Launch);
	const auto Fallback = F.Hit(F.Request(EHodgeImpactType::Launch));
	TestEqual(TEXT("Explicit fallback applies"), Fallback.Outcome, EHodgeHitReactionOutcome::Applied);
	TestFalse(TEXT("Fallback does not report Launch"), Fallback.AppliedImpacts.Contains(EHodgeImpactType::Launch));
	TestTrue(TEXT("Fallback reports real HitStun"), Fallback.AppliedImpacts.Contains(EHodgeImpactType::HitStun));
	F.Reaction->CancelReaction();
	auto Empty = F.Request(); Empty.Impacts.Reset(); Empty.ReservedPoiseDamage = 12.f;
	const auto Result = F.Hit(Empty);
	TestEqual(TEXT("Empty list has no hidden cancellation"), Result.Outcome, EHodgeHitReactionOutcome::NoImpact);
	TestEqual(TEXT("Reserved poise is carried without a poise system"), Result.ReservedPoiseDamage, 12.f);
	TestFalse(TEXT("Empty list has no control"), F.Reaction->IsControlled());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionMotionTest, "Hodge.HitReaction.MotionAndDownedRecovery", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeReactionMotionTest::RunTest(const FString& Parameters)
{
	HodgeReactionTests::FFixture F;
	TestEqual(TEXT("Ground push accepted"), F.Hit(F.Request(EHodgeImpactType::Knockback)).Outcome, EHodgeHitReactionOutcome::Applied);
	TestTrue(TEXT("Push uses CharacterMovement root-motion source"), F.Pawn->GetCharacterMovement()->HasRootMotionSources());
	F.Reaction->CancelReaction();
	TestEqual(TEXT("Launch accepted"), F.Hit(F.Request(EHodgeImpactType::Launch)).Outcome, EHodgeHitReactionOutcome::Applied);
	TestEqual(TEXT("Launch enters airborne reaction phase"), F.Reaction->GetReactionState().Phase, EHodgeHitReactionPhase::Airborne);
	F.Reaction->CancelReaction();
	F.Pawn->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	TestTrue(TEXT("Cancelling before the movement tick clears owned pending Launch"), F.Pawn->GetCharacterMovement()->PendingLaunchVelocity.IsNearlyZero());
	TestEqual(TEXT("Knockdown accepted"), F.Hit(F.Request(EHodgeImpactType::Knockdown)).Outcome, EHodgeHitReactionOutcome::Applied);
	TestEqual(TEXT("Target owns downed stage"), F.Reaction->GetReactionState().Phase, EHodgeHitReactionPhase::Downed);
	FHodgeHitReactionTestAccess::Expire(F.Reaction);
	F.Reaction->TickPlan();
	TestFalse(TEXT("Automatic recovery ends explicit animation-free fixture"), F.Reaction->IsControlled());
	F.Pawn->GetCharacterMovement()->Deactivate();
	TestEqual(TEXT("Disabled movement cannot report a successful Launch"), F.Hit(F.Request(EHodgeImpactType::Launch)).Outcome, EHodgeHitReactionOutcome::Unsupported);
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionCancellationTest, "Hodge.HitReaction.CancellationAndInput", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeReactionCancellationTest::RunTest(const FString& Parameters)
{
	HodgeReactionTests::FFixture F;
	F.Pawn->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	const auto JumpHandle = F.ASC->GiveAbility(FGameplayAbilitySpec(UHodgeHitReactionTestAbility::StaticClass(), 1));
	if (!TestTrue(TEXT("Native action starts before hit"), F.ASC->TryActivateAbility(JumpHandle, false))) { return false; }
	UGameplayAbility* Jump = F.ASC->FindAbilitySpecFromHandle(JumpHandle)->GetPrimaryInstance();
	Jump->SetCanBeCanceled(false);
	TestEqual(TEXT("Noncancellable action does not falsely report interrupt"), F.Hit(F.Request()).Outcome, EHodgeHitReactionOutcome::CannotInterrupt);
	TestFalse(TEXT("Failed interrupt leaves no control"), F.Reaction->IsControlled());
	Jump->SetCanBeCanceled(true);
	TestEqual(TEXT("Valid action is interrupted"), F.Hit(F.Request()).Outcome, EHodgeHitReactionOutcome::Applied);
	TestFalse(TEXT("Interrupted action actually ended"), Jump->IsActive());
	TestFalse(TEXT("Cannot restart jump under control"), F.ASC->TryActivateAbility(JumpHandle, false));
	auto* Movement = CastChecked<UHodgeCharacterMovementComponent>(F.Pawn->GetCharacterMovement());
	TestTrue(TEXT("Control blocks autonomous acceleration"), Movement->ConstrainInputAcceleration(FVector(50.f, 0.f, 0.f)).IsNearlyZero());
	F.Reaction->CancelReaction();
	TestFalse(TEXT("Restore permits acceleration"), Movement->ConstrainInputAcceleration(FVector(50.f, 0.f, 0.f)).IsNearlyZero());
	F.ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Attack);
	TestTrue(TEXT("Authority rejects attack-time player acceleration without a valid predicted cancel"), Movement->ConstrainInputAcceleration(FVector(50.f, 0.f, 0.f)).IsNearlyZero());
	F.ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Attack);
	TestFalse(TEXT("Attack input gate releases after the action ends"), Movement->ConstrainInputAcceleration(FVector(50.f, 0.f, 0.f)).IsNearlyZero());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionEnemyTest, "Hodge.HitReaction.EnemyPawnDataInitialization", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeReactionEnemyTest::RunTest(const FString& Parameters)
{
	const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false);
	UWorld* World = UWorld::CreateWorld(EWorldType::EditorPreview, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
	auto* Enemy = World->SpawnActor<AHodgeEnemyCharacter>();
	auto* EarlyASC = Enemy->FindComponentByClass<UHodgeAbilitySystemComponent>();
	TestEqual(TEXT("Native enemy exposes attribute ownership before PawnData arrives"), Enemy->GetHodgeAbilitySystemComponent(), EarlyASC);
	auto* EarlyHealth = NewObject<UHodgeHealthSet>(Enemy);
	TestEqual(TEXT("Early replicated attribute can resolve its owning ASC"), EarlyHealth->GetOwningAbilitySystemComponent(), static_cast<UAbilitySystemComponent*>(EarlyASC));
	auto* Data = NewObject<UHodgePawnData>(Enemy);
	Data->StatProfile = NewObject<UHodgeCharacterStatProfile>(Enemy);
	Data->StatProfile->MaxHealth = FScalableFloat(350.f);
	Data->StatProfile->BaseDamage = FScalableFloat(25.f);
	Data->HitReactionProfile = NewObject<UHodgeHitReactionProfile>(Enemy);
	Enemy->EnemyPawnData = Data;
	TestTrue(TEXT("Native enemy binds configured PawnData without PlayerState"), Enemy->InitializeEnemyAbilitySystem());
	auto* ASC = Enemy->GetHodgeAbilitySystemComponent();
	if (TestNotNull(TEXT("Enemy ASC exists"), ASC))
	{
		TestEqual(TEXT("Enemy ASC owner is the Pawn"), ASC->GetOwnerActor(), static_cast<AActor*>(Enemy));
		TestEqual(TEXT("Enemy ASC avatar is the Pawn"), ASC->GetAvatarActor(), static_cast<AActor*>(Enemy));
		TestEqual(TEXT("Configured stats initialize full enemy health"), ASC->GetSet<UHodgeHealthSet>()->GetHealth(), 350.f);
		const int32 Sets = ASC->GetSpawnedAttributes().Num();
		TestTrue(TEXT("Repeat bind is idempotent"), Enemy->InitializeEnemyAbilitySystem());
		TestEqual(TEXT("No duplicate attribute sets"), ASC->GetSpawnedAttributes().Num(), Sets);
	}
	World->DestroyWorld(false);
	return true;
}
#endif
