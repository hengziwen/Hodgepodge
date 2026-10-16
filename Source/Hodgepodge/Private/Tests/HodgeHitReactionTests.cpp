/** @file HodgeHitReactionTests.cpp
 * @brief 实现命中受击自动化测试相关的类型、接口与现有处理流程。
 */
#include "HodgeHitReactionTestAbility.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeHitReactionTestAbility)
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Combat/HodgeHitReactionTypes.h"
#include "Component/HodgeHitReactionComponent.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Component/HodgeCombatComponentBase.h"
#include "Component/HodgeCharacterMovementComponent.h"
#include "Character/HodgeCombatCharacter.h"
#include "Character/HodgeEnemyCharacter.h"
#include "HodgeEnemyAITestSupport.h"
#include "AIController.h"
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
#include <limits>

/** 保存命中受击测试Access的数据和接口契约。 */
struct FHodgeHitReactionTestAccess
{
	/** 使当前测试的缓存或会话达到过期条件。
	 * @param Component 组件，由调用方传入。
	 */
	static void Expire(UHodgeHitReactionComponent* Component) { Component->State.EndsAt = Component->GetServerTime() - 1.f; }
	/** 重放同一已确认表现，检查网络重复交付不会重复副作用。 */
	static void Present(UHodgeHitReactionComponent* Component, const FHodgeHitPresentation& Value) { Component->MulticastContactFeedback_Implementation(Value); }
};

namespace HodgeReactionTests
{
	/** 保存测试场景的数据和接口契约。 */
	struct FFixture
	{
		// 本次操作所在的世界。
		UWorld* World;
		// 当前操作关联的Pawn角色。
		AHodgeCombatCharacter* Pawn;
		// 后续GAS操作使用的能力系统组件。
		UHodgeAbilitySystemComponent* ASC;
		// 受击的数据或对象引用。
		UHodgeHitReactionComponent* Reaction;
		// 当前操作使用的配置对象。
		UHodgeHitReactionProfile* Profile;
		/** 构造测试场景并初始化其默认状态。 */
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
		/** 销毁该对象，执行当前类型的析构收尾。 */
		~FFixture() { Reaction->UninitializeFromAbilitySystem(); World->DestroyWorld(false); }
		/** 提交当前操作请求。
		 * @param Type 类型，由调用方传入。
		 * @return 本次计算、查询或创建得到的命中受击配置。
		 */
		FHodgeHitReactionConfig Request(EHodgeImpactType Type = EHodgeImpactType::HitStun) const
		{
			FHodgeHitReactionConfig Config;
			Config.AttackJudgementTag = HodgeHitReactionTags::Judgement_Skill;
			FHodgeImpactSpec Impact;
			Impact.Type = Type;
			Config.Impacts.Add(Impact);
			return Config;
		}
		/** 提交当前测试使用的伤害。
		 * @param Id 当前记录的唯一标识。
		 * @param Amount 数量，由调用方传入。
		 */
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
			// 将构造的效果规格提交给GAS执行属性更新。
			ASC->ApplyGameplayEffectSpecToSelf(Spec);
		}
		/** 执行当前检测或测试使用的命中操作。
		 * @param Config 配置，由调用方传入。
		 * @param Amount 数量，由调用方传入。
		 * @return 本次计算、查询或创建得到的命中受击结果。
		 */
		FHodgeHitReactionResult Hit(const FHodgeHitReactionConfig& Config, float Amount = 10.f)
		{
			const FGuid Id = Reaction->BeginHit(Config, Pawn, Pawn->GetActorLocation() - FVector(100.f, 0.f, 0.f));
			Damage(Id, Amount);
			return Reaction->FinishHit(Id);
		}
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionRanksTest, "Hodge.HitReaction.RanksAndValidation", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionRelevantFieldsTest, "Hodge.HitReaction.RelevantFieldsAndPresentation", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 隐藏字段失效不能阻断无关类型，实际使用的运动参数仍严格校验。 */
bool FHodgeReactionRelevantFieldsTest::RunTest(const FString& Parameters)
{
	HodgeReactionTests::FFixture F;
	auto Config = F.Request();
	auto& Impact = Config.Impacts[0];
	Impact.MoveDuration = 0.f;
	Impact.HorizontalSpeed = -10.f;
	Impact.WorldDirection.X = std::numeric_limits<float>::quiet_NaN();
	TArray<FText> Errors;
	TestTrue(TEXT("HitStun ignores unrelated hidden motion fields"), Config.Validate(Errors));
	TestEqual(TEXT("HitStun still applies with hidden invalid motion fields"), F.Hit(Config).Outcome, EHodgeHitReactionOutcome::Applied);
	F.Reaction->CancelReaction();
	Config = F.Request(EHodgeImpactType::Launch);
	Config.Impacts[0].ControlDuration = std::numeric_limits<float>::quiet_NaN();
	Config.Impacts[0].MoveDuration = 0.f;
	F.Profile->Animations.FindByPredicate([](const auto& Entry) { return Entry.Type == EHodgeImpactType::Launch; })->DefaultDuration = 0.f;
	Errors.Reset();
	TestTrue(TEXT("Airborne control uses phase bounds instead of hidden durations"), Config.Validate(Errors));
	TestEqual(TEXT("Launch does not consume hidden durations"), F.Hit(Config).Outcome, EHodgeHitReactionOutcome::Applied);
	F.Reaction->CancelReaction();
	Config = F.Request(EHodgeImpactType::Knockback);
	Config.Impacts[0].MoveDuration = 0.f;
	Errors.Reset();
	TestFalse(TEXT("Knockback still rejects invalid relevant move duration"), Config.Validate(Errors));
	FHodgeHitPresentation Presentation;
	Presentation.HitId = FGuid::NewGuid();
	const int32 Before = F.Reaction->GetPresentationCount();
	FHodgeHitReactionTestAccess::Present(F.Reaction, Presentation);
	FHodgeHitReactionTestAccess::Present(F.Reaction, Presentation);
	TestEqual(TEXT("Duplicate contact identity is presented once"), F.Reaction->GetPresentationCount(), Before + 1);
	Presentation.HitId = FGuid::NewGuid();
	FHodgeHitReactionTestAccess::Present(F.Reaction, Presentation);
	TestEqual(TEXT("Independent multi-hit contact is retained"), F.Reaction->GetPresentationCount(), Before + 2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionDownedFeedbackTest, "Hodge.HitReaction.DownedContactAndDirections", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 倒地普通命中保留伤害与接触，不将目标切回站立硬直。 */
bool FHodgeReactionDownedFeedbackTest::RunTest(const FString& Parameters)
{
	HodgeReactionTests::FFixture F;
	TestEqual(TEXT("Knockdown enters the actual control path"), F.Hit(F.Request(EHodgeImpactType::Knockdown)).Outcome, EHodgeHitReactionOutcome::Applied);
	const auto Before = F.Reaction->GetReactionState();
	const auto Result = F.Hit(F.Request());
	TestEqual(TEXT("Standing HitStun becomes contact while downed"), Result.Outcome, EHodgeHitReactionOutcome::NoImpact);
	TestEqual(TEXT("Contact still has actual damage"), Result.ActualDamage, 10.f);
	TestEqual(TEXT("Downed phase remains"), F.Reaction->GetReactionState().Phase, EHodgeHitReactionPhase::Downed);
	TestEqual(TEXT("Contact does not refresh downed timer"), F.Reaction->GetReactionState().EndsAt, Before.EndsAt);
	TestFalse(TEXT("Synthetic contact does not pretend a precise surface"), F.Reaction->GetLastPresentation().bPreciseContact);
	const FVector Sources[] = {FVector::ForwardVector, -FVector::ForwardVector, -FVector::RightVector, FVector::RightVector};
	for (int32 Index = 0; Index < 4; ++Index)
	{
		FHodgeDirectionalHitFeedback Entry;
		Entry.Direction = EHodgeHitPresentationDirection(Index);
		Entry.Montage = NewObject<UAnimMontage>(F.Profile);
		F.Profile->DirectionalLightFeedback.Add(Entry);
		TestEqual(TEXT("Local source chooses the corresponding directional asset"), F.Profile->FindLightFeedback(Sources[Index]), Entry.Montage.Get());
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionSettlementTest, "Hodge.HitReaction.DamageAndBodySnapshot", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
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
/** 执行本自动化用例的准备、操作与断言。 */
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
/** 执行本自动化用例的准备、操作与断言。 */
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
/** 执行本自动化用例的准备、操作与断言。 */
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
/** 执行本自动化用例的准备、操作与断言。 */
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
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeReactionCancellationTest::RunTest(const FString& Parameters)
{
	HodgeReactionTests::FFixture F;
	F.Pawn->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	const auto JumpHandle = F.ASC->GiveAbility(FGameplayAbilitySpec(UHodgeHitReactionTestAbility::StaticClass(), 1));
	// 通过ASC发起激活，保留GAS自身的条件与消耗校验。
	if (!TestTrue(TEXT("Native action starts before hit"), F.ASC->TryActivateAbility(JumpHandle, false))) { return false; }
	UGameplayAbility* Jump = F.ASC->FindAbilitySpecFromHandle(JumpHandle)->GetPrimaryInstance();
	// 低判定命中高抗状态仍扣血，但不取消正在执行的真实 GAS 动作。
	F.ASC->AddLooseGameplayTag(HodgeHitReactionTags::Body_SuperArmor);
	TestEqual(TEXT("High armor only receives contact from lower judgement"), F.Hit(F.Request()).Outcome, EHodgeHitReactionOutcome::LowJudgement);
	TestTrue(TEXT("High armor keeps the active action running"), Jump->IsActive());
	F.ASC->RemoveLooseGameplayTag(HodgeHitReactionTags::Body_SuperArmor);
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
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeReactionEnemyTest::RunTest(const FString& Parameters)
{
	UGameInstance* Instance = nullptr;
	// 本次操作所在的世界。
	UWorld* World = FHodgeEnemyInitTestAccess::CreateWorld(Instance);
	auto* Enemy = World->SpawnActor<AHodgeEnemyCharacter>();
	if (!Enemy->GetController()) { World->SpawnActor<AAIController>()->Possess(Enemy); }
	Enemy->FindComponentByClass<UHodgePawnExtensionComponent>()->TryToChangeInitState(HodgeGameplayTags::InitState_Spawned);
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
	FHodgeEnemyInitTestAccess::WithoutExperience(Enemy->GetEnemyInitComponent());
	// 统一战斗组件。
	auto* Combat = NewObject<UHodgeCombatComponentBase>(Enemy); Enemy->AddInstanceComponent(Combat); Combat->RegisterComponent();
	TestTrue(TEXT("Native enemy binds configured PawnData without PlayerState"), Enemy->InitializeEnemyAbilitySystem());
	auto* ASC = Enemy->GetHodgeAbilitySystemComponent();
	if (TestNotNull(TEXT("Enemy ASC exists"), ASC))
	{
		TestEqual(TEXT("Enemy ASC owner is the Pawn"), ASC->GetOwnerActor(), static_cast<AActor*>(Enemy));
		TestEqual(TEXT("Enemy ASC avatar is the Pawn"), ASC->GetAvatarActor(), static_cast<AActor*>(Enemy));
		if (TestNotNull(TEXT("Enemy HealthSet is initialized"), ASC->GetSet<UHodgeHealthSet>()))
		{ TestEqual(TEXT("Configured stats initialize full enemy health"), ASC->GetSet<UHodgeHealthSet>()->GetHealth(), 350.f); }
		const int32 Sets = ASC->GetSpawnedAttributes().Num();
		TestTrue(TEXT("Repeat bind is idempotent"), Enemy->InitializeEnemyAbilitySystem());
		TestEqual(TEXT("No duplicate attribute sets"), ASC->GetSpawnedAttributes().Num(), Sets);
	}
	FHodgeEnemyInitTestAccess::DestroyWorld(Instance, World);
	return true;
}
#endif
