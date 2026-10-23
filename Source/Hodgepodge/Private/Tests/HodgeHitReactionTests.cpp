/** @file HodgeHitReactionTests.cpp
 * @brief 实现命中受击自动化测试相关的类型、接口与现有处理流程。
 */
#include "HodgeHitReactionTestAbility.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeHitReactionTestAbility)
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Combat/HodgeHitReactionTypes.h"
#include "Combat/HodgeHitReactionMotion.h"
#include "Combat/HodgeHitReactionPlanBuilder.h"
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
#include "Animation/HodgeCombatAnimNotifies.h"
#include "GameplayEffect.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "Curves/CurveFloat.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/UnrealType.h"
#include "UObject/Package.h"
#include <limits>

/** 保存命中受击测试Access的数据和接口契约。 */
struct FHodgeHitReactionTestAccess
{
	/** 使当前测试的缓存或会话达到过期条件。
	 * @param Component 组件，由调用方传入。
	 */
	static void Expire(UHodgeHitReactionComponent* Component) { Component->State.EndsAt = Component->GetServerTime() - 1.f; }
	/** 只调整受击时间，实际资格查询和服务器交接仍走正式接口。 */
	static void RecoveryAge(UHodgeHitReactionComponent* Component, float Age) { Component->State.RecoveryStartedAt = Component->GetServerTime() - Age; }
	/** 重放同一已确认表现，检查网络重复交付不会重复副作用。 */
	static void Present(UHodgeHitReactionComponent* Component, const FHodgeHitPresentation& Value) { Component->MulticastContactFeedback_Implementation(Value); }
	/** 只推进测试样本年龄，生产过期与裁决仍走真实 FinishHit。 */
	static void AgePoise(UHodgeHitReactionComponent* Component, int32 Index, float Age) { Component->PoiseSamples[Index].Time = Component->GetServerTime() - Age; }
	/** 隔离计划计算而不提交状态，验证跨家族选片与运动政策的真实生产入口。 */
	static bool Build(UHodgeHitReactionComponent* Component, const FHodgeHitReactionConfig& Request,
		FHodgeHitReactionState& Plan)
	{
		UHodgeHitReactionComponent::FPendingHit Hit;
		Hit.Request = Request;
		Hit.Presentation.Strength = 1.f;
		Hit.Presentation.LocalSourceDirection = FVector::RightVector;
		TArray<EHodgeImpactType> Applied;
		return Component->BuildPlan(Hit, Plan, Applied);
	}
	/** 只设置待续播的计划快照，不伪造蒙太奇实例或运动结果。 */
	static void SetPlan(UHodgeHitReactionComponent* Component, const FHodgeHitReactionState& Plan) { Component->State = Plan; }
	/** 准备阶段运动来源，退出与发布仍走生产ChangePhase；不冒充真实根运动动画。 */
	static void ChangeMotionPhase(UHodgeHitReactionComponent* Component, EHodgeReactionMotionSource Source, EHodgeHitReactionPhase Phase)
	{
		Component->State.MotionSource = Source;
		Component->ChangePhase(Phase, nullptr, .2f);
	}
	/** 只布置同步差分的输入，不提交计划或驱动动画、运动及能力。 */
	static void SetPlanningInputs(UHodgeHitReactionComponent* Component, UHodgeHitReactionProfile* Profile,
		const FHodgeHitReactionState& State, bool bSawAirborne, int32 NextSequence = 4)
	{
		Component->Profile = Profile;
		Component->State = State;
		Component->bSawAirborne = bSawAirborne;
		Component->NextSequence = NextSequence;
	}
	/** 从同一实际场景读取纯计算输入，原组件入口继续独立读取相同场景。 */
	static FHodgeHitReactionPlanContext PlanningContext(UHodgeHitReactionComponent* Component,
		const FHodgeHitReactionConfig& Request, const FHodgeHitPresentation& Presentation,
		TWeakObjectPtr<AActor> Source, FVector Origin, EHodgeReactionLevel Level)
	{
		FHodgeHitReactionPlanContext Context;
		Context.Request = Request;
		Context.Presentation = Presentation;
		Context.ReactionLevel = Level;
		Context.State = Component->State;
		Context.Profile = Component->Profile;
		Context.Origin = Origin;
		Context.TargetLocation = Component->GetOwner()->GetActorLocation();
		Context.TargetForward = Component->GetOwner()->GetActorForwardVector();
		Context.bSourceValid = Source.IsValid();
		if (Context.bSourceValid) { Context.SourceForward = Source->GetActorForwardVector(); }
		Context.ServerTime = Component->GetServerTime();
		Context.Variation = Component->NextSequence + 1;
		const auto* Character = CastChecked<ACharacter>(Component->GetOwner());
		const auto* Movement = Character->GetCharacterMovement();
		Context.Mesh = Character->GetMesh()->GetSkeletalMeshAsset();
		Context.bAir = Movement->IsFalling() || (Component->State.Phase == EHodgeHitReactionPhase::Airborne && !Component->bSawAirborne);
		Context.bCanMove = Movement->IsActive() && Movement->MovementMode != MOVE_None && Movement->HasValidData();
		Context.bHasAnimInstance = Component->ASC.IsValid() && Component->ASC->AbilityActorInfo.IsValid() &&
			Component->ASC->AbilityActorInfo->GetAnimInstance() != nullptr;
		return Context;
	}
	/** 差分基准直接调用当前生产函数，不复制原计划算法。 */
	static bool BuildComponent(UHodgeHitReactionComponent* Component, const FHodgeHitReactionPlanContext& Context,
		TWeakObjectPtr<AActor> Source, FHodgeHitReactionState& Plan, TArray<EHodgeImpactType>& Applied)
	{
		UHodgeHitReactionComponent::FPendingHit Hit;
		Hit.Request = Context.Request;
		Hit.Presentation = Context.Presentation;
		Hit.ReactionLevel = Context.ReactionLevel;
		Hit.Source = Source;
		Hit.Origin = Context.Origin;
		return Component->BuildPlan(Hit, Plan, Applied);
	}
	/** 补充非反射执行身份与容器数量，发现计划计算意外提交或消耗。 */
	static FString PlanningIdentity(const UHodgeHitReactionComponent* Component)
	{
		return FString::Printf(TEXT("%d/%d/%d/%d/%d/%u/%d/%d/%d/%d"), Component->NextSequence,
			int32(Component->bSawAirborne), int32(Component->bOwnsControl), int32(Component->bSubmitting),
			int32(Component->bCompleting), Component->PoiseGeneration, int32(Component->ConsumedPoiseLevel),
			Component->PendingHits.Num(), Component->PoiseSamples.Num(), Component->PresentedHits.Num());
	}
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
		// 测试启用目标规则而不修改生产类默认对象。
		UHodgePawnData* Data;
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
			Data = NewObject<UHodgePawnData>(Pawn);
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
			// 测试中的显式空中动作模拟已经由分段解析通过的击飞资格。
			Config.bCanLaunch = Type == EHodgeImpactType::Launch || Type == EHodgeImpactType::AirHit || Type == EHodgeImpactType::Slam;
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionIntegratedMotionTest, "Hodge.HitReaction.IntegratedMotionSteps", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 相同步长、抖动和跨结束帧均应保持总距离，追赶模拟不改变速度面积。 */
bool FHodgeReactionIntegratedMotionTest::RunTest(const FString& Parameters)
{
	HodgeReactionTests::FFixture F;
	auto* Curve = NewObject<UCurveFloat>(F.Profile);
	Curve->FloatCurve.AddKey(0.f, 1.f);
	Curve->FloatCurve.AddKey(1.f, 0.f);
	for (float Step : {1.f / 30.f, 1.f / 60.f, 0.047f})
	{
		FHodgeRootMotionSource_Knockback Source;
		Source.Duration = 0.3f;
		Source.Force = FVector(1000.f, 0.f, 0.f);
		Source.StrengthOverTime = Curve;
		float Distance = 0.f;
		for (int32 Index = 0; Index * Step < 0.3f; ++Index)
		{
			Source.PrepareRootMotion(Step, Step, *F.Pawn, *F.Pawn->GetCharacterMovement());
			Distance += Source.RootMotionParams.GetRootMotionTransform().GetTranslation().X * Step;
		}
		TestTrue(TEXT("Curve integrates to 150cm for every physical step"), FMath::IsNearlyEqual(Distance, 150.f, 0.02f));
		TestEqual(TEXT("Source force is not permanently scaled"), Source.Force.X, 1000.0);
		TestEqual(TEXT("Curve retains GC ownership after sampling"), Source.StrengthOverTime.Get(), Curve);
		TUniquePtr<FRootMotionSource> Copy(Source.Clone());
		TestEqual(TEXT("Saved move clone retains integrated RMS type"), Copy->GetScriptStruct(), FHodgeRootMotionSource_Knockback::StaticStruct());
		TestTrue(TEXT("Saved move clone retains matching state"), Copy->MatchesAndHasSameState(&Source));
	}
	FHodgeRootMotionSource_Knockback Catchup;
	Catchup.Duration = 0.3f;
	Catchup.Force = FVector(1000.f, 0.f, 0.f);
	Catchup.StrengthOverTime = Curve;
	Catchup.PrepareRootMotion(0.3f, 0.1f, *F.Pawn, *F.Pawn->GetCharacterMovement());
	TestTrue(TEXT("Simulated catch-up retains total displacement"), FMath::IsNearlyEqual(float(Catchup.RootMotionParams.GetRootMotionTransform().GetTranslation().X * 0.1f), 150.f, 0.02f));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionRecoveryWindowsTest, "Hodge.HitReaction.RecoveryWindows", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 验证分时开放、重复命中重置和旧身份拒绝，避免续播姿态意外免控。 */
bool FHodgeReactionRecoveryWindowsTest::RunTest(const FString& Parameters)
{
	HodgeReactionTests::FFixture F;
	F.Profile->bEnableGroundRecovery = true;
	F.Hit(F.Request(), 1.f);
	TestFalse(TEXT("Dodge cannot escape on impact"), F.Reaction->CanRecover(EHodgeHitRecoveryAction::Dodge));
	FHodgeHitReactionTestAccess::RecoveryAge(F.Reaction, 0.21f);
	TestTrue(TEXT("Dodge opens after 0.2 seconds"), F.Reaction->CanRecover(EHodgeHitRecoveryAction::Dodge));
	// 独立禁令必须保留，恢复入口不能成为通用免控开关。
	F.ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked);
	TestFalse(TEXT("Independent rotation lock blocks recovery"), F.Reaction->CanRecover(EHodgeHitRecoveryAction::Dodge));
	F.ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked);
	F.ASC->AddLooseGameplayTag(TAG_Gameplay_MovementStopped);
	TestFalse(TEXT("Independent movement stop blocks recovery"), F.Reaction->CanRecover(EHodgeHitRecoveryAction::Dodge));
	F.ASC->RemoveLooseGameplayTag(TAG_Gameplay_MovementStopped);
	TestFalse(TEXT("Attack remains closed"), F.Reaction->CanRecover(EHodgeHitRecoveryAction::BasicAttack));
	TestFalse(TEXT("Movement remains closed"), F.Reaction->CanRecover(EHodgeHitRecoveryAction::Movement));
	FHodgeHitReactionTestAccess::RecoveryAge(F.Reaction, 0.31f);
	TestTrue(TEXT("Attack opens after 0.3 seconds"), F.Reaction->CanRecover(EHodgeHitRecoveryAction::BasicAttack));
	const int32 OldSequence = F.Reaction->GetReactionState().Sequence;
	F.Hit(F.Request(), 1.f);
	TestFalse(TEXT("New accepted hit restarts recovery wait"), F.Reaction->CanRecover(EHodgeHitRecoveryAction::Dodge));
	FHodgeHitReactionTestAccess::RecoveryAge(F.Reaction, 0.31f);
	TestFalse(TEXT("Stale action cannot release new control"), F.Reaction->TryRecover(EHodgeHitRecoveryAction::Dodge, OldSequence));
	TestTrue(TEXT("Authorized current attack releases control"), F.Reaction->TryRecover(EHodgeHitRecoveryAction::BasicAttack, F.Reaction->GetReactionState().Sequence));
	TestFalse(TEXT("Released plan has no control tag"), F.ASC->HasMatchingGameplayTag(HodgeHitReactionTags::Controlled));
	F.Hit(F.Request(EHodgeImpactType::Knockback), 1.f);
	FHodgeHitReactionTestAccess::RecoveryAge(F.Reaction, 1.f);
	TestFalse(TEXT("Ground windows do not escape knockback"), F.Reaction->CanRecover(EHodgeHitRecoveryAction::Dodge));
	F.Reaction->CancelReaction();
	F.Hit(F.Request(), 1.f);
	FHodgeHitReactionTestAccess::RecoveryAge(F.Reaction, 0.41f);
	F.Reaction->TickPlan();
	TestFalse(TEXT("Movement recovers naturally after 0.4 seconds"), F.Reaction->IsControlled());
	F.Profile->DodgeRecoveryDelay = std::numeric_limits<float>::quiet_NaN();
	TArray<FText> Errors;
	TestFalse(TEXT("Nonfinite recovery delays are invalid"), F.Profile->Validate(Errors));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionTransitionPolicyTest, "Hodge.HitReaction.TransitionProtection", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 真实伤害仍结算，但轻硬直不得撤销击退或倒地运动。 */
bool FHodgeReactionTransitionPolicyTest::RunTest(const FString& Parameters)
{
	HodgeReactionTests::FFixture F;
	TestEqual(TEXT("Knockback starts"), F.Hit(F.Request(EHodgeImpactType::Knockback), 1.f).Outcome, EHodgeHitReactionOutcome::Applied);
	const auto Before = F.Reaction->GetReactionState();
	const float Health = F.ASC->GetSet<UHodgeHealthSet>()->GetHealth();
	const int32 Contacts = F.Reaction->GetPresentationCount();
	TestEqual(TEXT("Light hit retains strong movement"), F.Hit(F.Request(), 1.f).Outcome, EHodgeHitReactionOutcome::NoImpact);
	TestEqual(TEXT("Protected hit does not replace execution"), F.Reaction->GetReactionState().Sequence, Before.Sequence);
	TestEqual(TEXT("Protected hit still damages"), F.ASC->GetSet<UHodgeHealthSet>()->GetHealth(), Health - 1.f);
	TestEqual(TEXT("Protected hit still presents contact"), F.Reaction->GetPresentationCount(), Contacts + 1);
	TestEqual(TEXT("Explicit Launch may upgrade knockback"), F.Hit(F.Request(EHodgeImpactType::Launch), 1.f).Outcome, EHodgeHitReactionOutcome::Applied);
	F.Reaction->CancelReaction();
	F.Pawn->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	TestEqual(TEXT("Knockdown starts"), F.Hit(F.Request(EHodgeImpactType::Knockdown), 1.f).Outcome, EHodgeHitReactionOutcome::Applied);
	TestEqual(TEXT("Ground knockback cannot stand up downed target"), F.Hit(F.Request(EHodgeImpactType::Knockback), 1.f).Outcome, EHodgeHitReactionOutcome::NoImpact);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionLethalContactTest, "Hodge.HitReaction.LethalContact", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 致死刀保留接触音画，但不启动或覆盖死亡姿态。 */
bool FHodgeReactionLethalContactTest::RunTest(const FString& Parameters)
{
	HodgeReactionTests::FFixture F;
	const int32 Before = F.Reaction->GetPresentationCount();
	TestEqual(TEXT("Lethal hit remains death outcome"), F.Hit(F.Request(), 200.f).Outcome, EHodgeHitReactionOutcome::Dead);
	TestEqual(TEXT("Lethal accepted hit gets one contact"), F.Reaction->GetPresentationCount(), Before + 1);
	TestFalse(TEXT("Lethal contact never starts light montage"), F.Reaction->GetLastPresentation().bLightFeedback);
	TestFalse(TEXT("Lethal contact never starts control"), F.Reaction->IsControlled());
	F.Hit(F.Request(), 1.f);
	TestEqual(TEXT("Already dead does not replay a lethal contact"), F.Reaction->GetPresentationCount(), Before + 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionCurveTest, "Hodge.HitReaction.NormalizedKnockbackCurve", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 校验衰减曲线的积分、峰值限速及非法曲线拒绝。 */
bool FHodgeReactionCurveTest::RunTest(const FString& Parameters)
{
	HodgeReactionTests::FFixture F;
	float Peak = 0.f;
	TestEqual(TEXT("Null curve preserves constant force"), F.Profile->GetKnockbackCurveIntegral(Peak), 1.f);
	F.Profile->KnockbackStrengthCurve = NewObject<UCurveFloat>(F.Profile);
	F.Profile->KnockbackStrengthCurve->FloatCurve.AddKey(0.f, 1.f);
	const auto End = F.Profile->KnockbackStrengthCurve->FloatCurve.AddKey(1.f, 0.f);
	TestTrue(TEXT("Linear decay integrates to half"), FMath::IsNearlyEqual(F.Profile->GetKnockbackCurveIntegral(Peak), 0.5f));
	TestEqual(TEXT("Curve peak remains one"), Peak, 1.f);
	TestEqual(TEXT("Valid curve submits movement"), F.Hit(F.Request(EHodgeImpactType::Knockback), 1.f).Outcome, EHodgeHitReactionOutcome::Applied);
	TestTrue(TEXT("Velocity is normalized for original distance"), FMath::IsNearlyEqual(F.Reaction->GetReactionState().MotionVelocity.Size(), 1200.f, 0.01f));
	F.Profile->KnockbackStrengthCurve->FloatCurve.GetKey(End).Value = -1.f;
	TArray<FText> Errors;
	TestFalse(TEXT("Negative strength curve is invalid"), F.Profile->Validate(Errors));
	F.Profile->KnockbackStrengthCurve->FloatCurve.Reset();
	TestEqual(TEXT("Empty curve has no usable area"), F.Profile->GetKnockbackCurveIntegral(Peak), 0.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionVariantsTest, "Hodge.HitReaction.VariantsAndSustain", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 方向与力度选择独立于控制等级，延长地面控制时持续目标姿态。 */
bool FHodgeReactionVariantsTest::RunTest(const FString& Parameters)
{
	HodgeReactionTests::FFixture F;
	auto& Entry = F.Profile->Animations[0];
	Entry.Montage = NewObject<UAnimMontage>(F.Profile);
	for (float Strength : {0.5f, 1.5f})
	{
		auto& Variant = Entry.Variants.AddDefaulted_GetRef();
		Variant.MinimumStrength = Strength;
		Variant.Montage = NewObject<UAnimMontage>(F.Profile);
	}
	TestEqual(TEXT("Below threshold uses default"), F.Profile->FindReactionMontage(EHodgeImpactType::HitStun, FVector::ForwardVector, 0.1f), Entry.Montage.Get());
	TestEqual(TEXT("Light front selects lower threshold"), F.Profile->FindReactionMontage(EHodgeImpactType::HitStun, FVector::ForwardVector, 1.f), Entry.Variants[0].Montage.Get());
	TestEqual(TEXT("Strong front selects upper threshold"), F.Profile->FindReactionMontage(EHodgeImpactType::HitStun, FVector::ForwardVector, 2.f), Entry.Variants[1].Montage.Get());
	TestEqual(TEXT("Missing direction falls back"), F.Profile->FindReactionMontage(EHodgeImpactType::HitStun, -FVector::ForwardVector, 2.f), Entry.Montage.Get());
	FHodgeHitReactionState State;
	State.Phase = EHodgeHitReactionPhase::Controlled;
	TestTrue(TEXT("Ground hitstun can hold until authority releases it"), F.Profile->ShouldSustainPose(State));
	State.Type = EHodgeImpactType::Launch;
	TestFalse(TEXT("Sustain does not freeze airborne content"), F.Profile->ShouldSustainPose(State));
	State.Phase = EHodgeHitReactionPhase::None;
	TestFalse(TEXT("Completed reaction never holds pose"), F.Profile->ShouldSustainPose(State));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionMotionContractTest, "Hodge.HitReaction.PhaseMotionAndLateralSelection", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 校验根运动阶段互斥、空中禁止双重驱动，以及左右受力的确定性选择。 */
bool FHodgeReactionMotionContractTest::RunTest(const FString& Parameters)
{
	HodgeReactionTests::FFixture F;
	auto* Sequence = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Wuwa/Anim/BaseAnim/Behit_B_R.Behit_B_R"));
	if (!TestNotNull(TEXT("Existing authored reaction exists"), Sequence)) { return false; }
	auto* Montage = NewObject<UAnimMontage>(F.Profile);
	auto& Track = Montage->SlotAnimTracks.AddDefaulted_GetRef();
	auto& Segment = Track.AnimTrack.AnimSegments.AddDefaulted_GetRef();
	Segment.SetAnimReference(Sequence);
	Segment.AnimEndTime = Sequence->GetPlayLength();
	Montage->SetCompositeLength(Sequence->GetPlayLength());
	auto& Ground = F.Profile->Animations[0];
	Ground.Montage = Montage;
	TArray<FText> Errors;
	TestTrue(TEXT("Test montage carries authored root motion"), Montage->HasRootMotion());
	TestFalse(TEXT("Impact movement rejects root-motion clip"), F.Profile->Validate(Errors));
	Ground.MotionSource = EHodgeReactionMotionSource::AnimationRootMotion;
	Errors.Reset();
	TestTrue(TEXT("Explicit ground animation motion is valid"), F.Profile->Validate(Errors));
	auto& Air = F.Profile->Animations[2];
	Air.Montage = Montage;
	Air.MotionSource = EHodgeReactionMotionSource::AnimationRootMotion;
	Errors.Reset();
	TestFalse(TEXT("Airborne start cannot override gravity with animation motion"), F.Profile->Validate(Errors));
	Air.Montage = nullptr;
	Air.MotionSource = EHodgeReactionMotionSource::Impact;
	Air.LandingMontage = Montage;
	Air.LandingMotionSource = EHodgeReactionMotionSource::AnimationRootMotion;
	Errors.Reset();
	TestTrue(TEXT("Air impact may hand off to grounded root-motion landing"), F.Profile->Validate(Errors));
	Ground.bUseLateralVariants = true;
	for (auto Side : {EHodgeHitPresentationDirection::Left, EHodgeHitPresentationDirection::Right})
	{
		auto& Variant = Ground.Variants.AddDefaulted_GetRef();
		Variant.Direction = Side;
		Variant.Montage = NewObject<UAnimMontage>(F.Profile);
	}
	TestEqual(TEXT("Front automatic even identity chooses left"), F.Profile->FindReactionMontage(EHodgeImpactType::HitStun,
		FVector::ForwardVector, 1.f, EHodgeHitSide::Automatic, 2), Ground.Variants[0].Montage.Get());
	TestEqual(TEXT("Front automatic odd identity chooses right"), F.Profile->FindReactionMontage(EHodgeImpactType::HitStun,
		FVector::ForwardVector, 1.f, EHodgeHitSide::Automatic, 3), Ground.Variants[1].Montage.Get());
	TestEqual(TEXT("Explicit attack side is stable across direction"), F.Profile->FindReactionMontage(EHodgeImpactType::HitStun,
		-FVector::RightVector, 1.f, EHodgeHitSide::Right, 2), Ground.Variants[1].Montage.Get());
	FHodgeHitReactionState State;
	State.Phase = EHodgeHitReactionPhase::Controlled;
	State.MotionSource = EHodgeReactionMotionSource::AnimationRootMotion;
	TestTrue(TEXT("Long control holds authored end pose after the complete root track"), F.Profile->ShouldSustainPose(State));
	State.Montage = Montage;
	TestEqual(TEXT("Root track time never wraps to repeat displacement"), F.Profile->GetPresentationPosition(State, 5.f), 5.f);
	State.Phase = EHodgeHitReactionPhase::None;
	TestFalse(TEXT("Released root motion does not hold pose"), F.Profile->ShouldSustainPose(State));
	State.MotionSource = EHodgeReactionMotionSource::Impact;
	State.Phase = EHodgeHitReactionPhase::Airborne;
	State.AirLoopTransitionTime = .2f;
	TestTrue(TEXT("Air start can hold until actual descent"), F.Profile->ShouldSustainPose(State));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionAirContactBudgetTest, "Hodge.HitReaction.AirContactDoesNotSpendLaunchBudget", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 无击飞资格不重启空中姿态；有资格的续飞沿用 Fly 并受到次数限制。 */
bool FHodgeReactionAirContactBudgetTest::RunTest(const FString& Parameters)
{
	HodgeReactionTests::FFixture F;
	F.Pawn->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	F.Profile->MaxAirHits = 1;
	TestEqual(TEXT("Initial flight is accepted"), F.Hit(F.Request(EHodgeImpactType::Launch), 1.f).Outcome, EHodgeHitReactionOutcome::Applied);
	const auto InitialSequence = F.Reaction->GetReactionState().Sequence;
	TestEqual(TEXT("Ordinary air hit is contact only"), F.Hit(F.Request(), 1.f).Outcome, EHodgeHitReactionOutcome::NoImpact);
	TestEqual(TEXT("Ordinary hit retains flight identity"), F.Reaction->GetReactionState().Sequence, InitialSequence);
	TestEqual(TEXT("Ordinary air contact preserves movement budget"), F.Reaction->GetReactionState().AirHitCount, 0);
	TestEqual(TEXT("First explicit air motion is accepted"), F.Hit(F.Request(EHodgeImpactType::AirHit), 1.f).Outcome, EHodgeHitReactionOutcome::Applied);
	TestEqual(TEXT("Explicit air motion spends one unit"), F.Reaction->GetReactionState().AirHitCount, 1);
	TestEqual(TEXT("Qualified air hit selects Fly instead of Push"), F.Reaction->GetReactionState().Type, EHodgeImpactType::Launch);
	TestEqual(TEXT("Another ordinary hit does not restart Fly"), F.Hit(F.Request(), 1.f).Outcome, EHodgeHitReactionOutcome::NoImpact);
	TestEqual(TEXT("Contact cannot reset or spend the exhausted budget"), F.Reaction->GetReactionState().AirHitCount, 1);
	const auto Before = F.Reaction->GetReactionState().Sequence;
	TestEqual(TEXT("Exhausted explicit air motion is contact only"), F.Hit(F.Request(EHodgeImpactType::AirHit), 1.f).Outcome, EHodgeHitReactionOutcome::NoImpact);
	TestEqual(TEXT("Rejected air motion keeps current execution"), F.Reaction->GetReactionState().Sequence, Before);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionSustainSectionsTest, "Hodge.HitReaction.SustainSectionsAndTailSafety", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 持续段的晚到重建保持相位，恢复尾部不能持有不存在或无关的实例。 */
bool FHodgeReactionSustainSectionsTest::RunTest(const FString& Parameters)
{
	HodgeReactionTests::FFixture F;
	auto& Entry = F.Profile->Animations[0];
	Entry.Montage = NewObject<UAnimMontage>(F.Profile);
	Entry.Montage->SetCompositeLength(1.f);
	Entry.SustainSection = TEXT("Sustain");
	Entry.RecoverySection = TEXT("Recover");
	const FName Names[] = {TEXT("Impact"), TEXT("Sustain"), TEXT("Recover")};
	const float Times[] = {0.f, 0.2f, 0.5f};
	for (int32 Index = 0; Index < 3; ++Index)
	{
		FCompositeSection Section;
		Section.SectionName = Names[Index];
		Section.Link(Entry.Montage, Times[Index]);
		Entry.Montage->CompositeSections.Add(Section);
	}
	FHodgeHitReactionState State;
	State.Phase = EHodgeHitReactionPhase::Controlled;
	State.Montage = Entry.Montage;
	TestTrue(TEXT("Impact position remains unchanged"), FMath::IsNearlyEqual(F.Profile->GetPresentationPosition(State, 0.1f), 0.1f));
	TestTrue(TEXT("Late state maps into sustain loop"), FMath::IsNearlyEqual(F.Profile->GetPresentationPosition(State, 0.85f), 0.25f));
	TArray<FText> Errors;
	TestTrue(TEXT("Paired ordered sections are valid"), F.Profile->Validate(Errors));
	Entry.RecoverySection = NAME_None;
	Errors.Reset();
	TestFalse(TEXT("Half-configured recovery is rejected"), F.Profile->Validate(Errors));
	F.Profile->bAllowNaturalRecoveryTail = true;
	TestFalse(TEXT("Missing instance cannot be retired"), F.Reaction->RetirePresentation(Entry.Montage, 99));
	F.Reaction->StopRetiredPresentation();
	TestFalse(TEXT("Tail cleanup does not invent control"), F.Reaction->IsControlled());
	return true;
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
	TestEqual(TEXT("Missing Fly cannot silently become another reaction"), Fallback.Outcome, EHodgeHitReactionOutcome::Unsupported);
	TestFalse(TEXT("Fallback does not report Launch"), Fallback.AppliedImpacts.Contains(EHodgeImpactType::Launch));
	TestFalse(TEXT("Refused flight does not pretend HitStun"), Fallback.AppliedImpacts.Contains(EHodgeImpactType::HitStun));
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
	const FVector PendingLaunch = F.Pawn->GetCharacterMovement()->PendingLaunchVelocity;
	TestFalse(TEXT("Launch has a pending physics velocity"), PendingLaunch.IsNearlyZero());
	TestEqual(TEXT("Same-frame light impact is contact only"), F.Hit(F.Request(), 1.f).Outcome, EHodgeHitReactionOutcome::NoImpact);
	TestEqual(TEXT("Same-frame light impact preserves airborne plan"), F.Reaction->GetReactionState().Phase, EHodgeHitReactionPhase::Airborne);
	TestTrue(TEXT("Same-frame light impact preserves pending launch"), F.Pawn->GetCharacterMovement()->PendingLaunchVelocity.Equals(PendingLaunch));
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
namespace HodgeReactionTests
{
	/** 测试只开启目标规则，攻击普通模式没有直接 Impact。 */
	FHodgeHitReactionConfig PoiseRequest(FFixture& F, float Amount)
	{
		F.Data->Poise.bEnabled = true;
		F.Data->Poise.BreakImpact = EHodgeImpactType::HitStun;
		FHodgeHitReactionConfig Request;
		Request.InputMode = EHodgeReactionInputMode::PoiseDriven;
		Request.AttackJudgementTag = HodgeHitReactionTags::Judgement_Skill;
		Request.ReservedPoiseDamage = Amount;
		return Request;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgePoiseProgressionTest, "Hodge.HitReaction.Poise.ProgressionStopsAtHeavy", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 真实 GE 结算只推进轻重阈值，任意大削韧不能再产生破韧击飞。 */
bool FHodgePoiseProgressionTest::RunTest(const FString& Parameters)
{
	using namespace HodgeReactionTests;
	FFixture F;
	TestEqual(TEXT("Below light stays contact"), F.Hit(PoiseRequest(F, 15.f), 1.f).ReactionLevel, EHodgeReactionLevel::None);
	TestEqual(TEXT("35 crosses light"), F.Hit(PoiseRequest(F, 20.f), 1.f).ReactionLevel, EHodgeReactionLevel::Light);
	const int32 Sequence = F.Reaction->GetReactionState().Sequence;
	TestEqual(TEXT("Same level does not refresh"), F.Hit(PoiseRequest(F, 1.f), 1.f).Outcome, EHodgeHitReactionOutcome::NoImpact);
	TestEqual(TEXT("Same level retains sequence"), F.Reaction->GetReactionState().Sequence, Sequence);
	TestEqual(TEXT("61 crosses heavy"), F.Hit(PoiseRequest(F, 25.f), 1.f).ReactionLevel, EHodgeReactionLevel::Heavy);
	const int32 HeavySequence = F.Reaction->GetReactionState().Sequence;
	TestEqual(TEXT("111 does not cross into launch"), F.Hit(PoiseRequest(F, 50.f), 1.f).Outcome, EHodgeHitReactionOutcome::NoImpact);
	TestEqual(TEXT("Further poise stays contact only"), F.Hit(PoiseRequest(F, 120.f), 1.f).Outcome, EHodgeHitReactionOutcome::NoImpact);
	TestEqual(TEXT("Huge poise keeps the heavy identity"), F.Reaction->GetReactionState().Sequence, HeavySequence);
	TestEqual(TEXT("Huge poise remains heavy knockback"), F.Reaction->GetReactionState().Type, EHodgeImpactType::Knockback);
	F.Reaction->CancelReaction();
	TestEqual(TEXT("Consumed heavy cannot be refreshed by more poise"), F.Hit(PoiseRequest(F, 120.f), 1.f).Outcome, EHodgeHitReactionOutcome::NoImpact);
	F.Reaction->ResetPoise();
	TestEqual(TEXT("Fresh cycle still reaches only heavy"), F.Hit(PoiseRequest(F, 120.f), 1.f).ReactionLevel, EHodgeReactionLevel::Heavy);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgePoiseWindowTest, "Hodge.HitReaction.Poise.SlidingWindowAndConsumedLevels", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 最早样本逐条到期，同轮分数下降再上升不重复触发轻硬直。 */
bool FHodgePoiseWindowTest::RunTest(const FString& Parameters)
{
	using namespace HodgeReactionTests;
	FFixture F;
	F.Hit(PoiseRequest(F, 15.f), 1.f);
	F.Hit(PoiseRequest(F, 20.f), 1.f);
	FHodgeHitReactionTestAccess::AgePoise(F.Reaction, 0, 1.6f);
	TestEqual(TEXT("Expired old hit leaves 20"), F.Reaction->GetAccumulatedPoise(), 20.f);
	TestEqual(TEXT("Score rebound within cycle cannot retrigger light"), F.Hit(PoiseRequest(F, 10.f), 1.f).Outcome, EHodgeHitReactionOutcome::NoImpact);
	TestEqual(TEXT("Remaining window retains new samples"), F.Reaction->GetAccumulatedPoise(), 30.f);
	FHodgeHitReactionTestAccess::AgePoise(F.Reaction, 0, 1.6f);
	FHodgeHitReactionTestAccess::AgePoise(F.Reaction, 1, 1.6f);
	TestEqual(TEXT("Empty window opens next cycle"), F.Hit(PoiseRequest(F, 20.f), 1.f).ReactionLevel, EHodgeReactionLevel::Light);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgePoiseTransactionTest, "Hodge.HitReaction.Poise.TransactionAndAcceptance", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 多 GE 只消费一次；零生命损失、免疫和重复结算不产生削韧。 */
bool FHodgePoiseTransactionTest::RunTest(const FString& Parameters)
{
	using namespace HodgeReactionTests;
	FFixture F;
	const auto Request = PoiseRequest(F, 15.f);
	const FGuid Id = F.Reaction->BeginHit(Request, F.Pawn, FVector::ZeroVector);
	F.Damage(Id, 1.f);
	F.Damage(Id, 2.f);
	TestEqual(TEXT("Transaction sums actual health loss"), F.Reaction->FinishHit(Id).ActualDamage, 3.f);
	TestEqual(TEXT("Two GE callbacks consume one poise input"), F.Reaction->GetAccumulatedPoise(), 15.f);
	F.Reaction->FinishHit(Id);
	TestEqual(TEXT("Duplicate Finish cannot add poise"), F.Reaction->GetAccumulatedPoise(), 15.f);
	const FGuid Zero = F.Reaction->BeginHit(Request, F.Pawn, FVector::ZeroVector);
	F.Reaction->RecordDamage(Zero, true, 0.f);
	TestEqual(TEXT("Accepted zero loss is not ordinary control"), F.Reaction->FinishHit(Zero).Outcome, EHodgeHitReactionOutcome::NoDamageOutput);
	F.ASC->AddLooseGameplayTag(TAG_Gameplay_DamageImmunity);
	TestEqual(TEXT("Immunity rejects actual GE"), F.Hit(Request, 1.f).Outcome, EHodgeHitReactionOutcome::Rejected);
	TestEqual(TEXT("Immunity does not bank poise"), F.Reaction->GetAccumulatedPoise(), 15.f);
	F.ASC->RemoveLooseGameplayTag(TAG_Gameplay_DamageImmunity);
	F.Hit(PoiseRequest(F, 0.f), 1.f);
	TestEqual(TEXT("Zero poise does not add a sample"), F.Reaction->GetAccumulatedPoise(), 15.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgePoiseBodyTest, "Hodge.HitReaction.Poise.BodyAndPhaseGuards", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 霸体积累可选但不能自动打断；倒地、起身和空中普通命中不续控。 */
bool FHodgePoiseBodyTest::RunTest(const FString& Parameters)
{
	using namespace HodgeReactionTests;
	FFixture F;
	auto Request = PoiseRequest(F, 30.f);
	F.ASC->AddLooseGameplayTag(HodgeHitReactionTags::Body_Vajra);
	TestEqual(TEXT("Low rank stays feedback"), F.Hit(Request, 1.f).Outcome, EHodgeHitReactionOutcome::LowJudgement);
	TestEqual(TEXT("Default armor banks no poise"), F.Reaction->GetAccumulatedPoise(), 0.f);
	F.Data->Poise.bAccumulateUnderArmor = true;
	TestEqual(TEXT("Optional armor still cannot interrupt"), F.Hit(Request, 1.f).Outcome, EHodgeHitReactionOutcome::LowJudgement);
	TestEqual(TEXT("Optional armor banks poise"), F.Reaction->GetAccumulatedPoise(), 30.f);
	F.ASC->RemoveLooseGameplayTag(HodgeHitReactionTags::Body_Vajra);
	TestFalse(TEXT("Removing armor never auto fires control"), F.Reaction->IsControlled());
	TestEqual(TEXT("Only next eligible positive hit evaluates"), F.Hit(Request, 1.f).ReactionLevel, EHodgeReactionLevel::Heavy);
	F.Reaction->ResetPoise();
	F.Reaction->CancelReaction();
	F.Hit(F.Request(EHodgeImpactType::Knockdown), 1.f);
	TestEqual(TEXT("Downed normal hit does not accumulate"), F.Hit(Request, 1.f).Outcome, EHodgeHitReactionOutcome::NoImpact);
	TestEqual(TEXT("Downed history stays empty"), F.Reaction->GetAccumulatedPoise(), 0.f);
	F.Reaction->CancelReaction();
	F.Pawn->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	F.Hit(Request, 1.f);
	TestEqual(TEXT("Air normal hit cannot launch twice"), F.Reaction->GetAccumulatedPoise(), 0.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgePoiseLifecycleTest, "Hodge.HitReaction.Poise.LifecycleAndSubmissionFailure", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 不可取消技能导致提交失败时保留可重试阈值；解绑、死亡清理样本。 */
bool FHodgePoiseLifecycleTest::RunTest(const FString& Parameters)
{
	using namespace HodgeReactionTests;
	FFixture F;
	auto Request = PoiseRequest(F, 20.f);
	const auto Handle = F.ASC->GiveAbility(FGameplayAbilitySpec(UHodgeHitReactionTestAbility::StaticClass(), 1));
	F.ASC->TryActivateAbility(Handle);
	auto* Spec = F.ASC->FindAbilitySpecFromHandle(Handle);
	if (auto* Ability = Spec ? Spec->GetPrimaryInstance() : nullptr)
	{
		Ability->SetCanBeCanceled(false);
		TestEqual(TEXT("Failed submission does not consume light"), F.Hit(Request, 1.f).ReactionLevel, EHodgeReactionLevel::None);
		Ability->SetCanBeCanceled(true);
		Ability->CancelAbility(Handle, F.ASC->AbilityActorInfo.Get(), Ability->GetCurrentActivationInfo(), true);
		TestEqual(TEXT("Next fresh hit can retry light"), F.Hit(PoiseRequest(F, 1.f), 1.f).ReactionLevel, EHodgeReactionLevel::Light);
	}
	else { AddError(TEXT("Test ability did not create an active instance")); }
	F.Reaction->UninitializeFromAbilitySystem();
	TestEqual(TEXT("Unbind clears old Avatar history"), F.Reaction->GetAccumulatedPoise(), 0.f);
	F.Reaction->InitializeWithAbilitySystem(F.ASC);
	F.Hit(PoiseRequest(F, 10.f), 1.f);
	F.ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Death);
	TestEqual(TEXT("Death tag clears idle history immediately"), F.Reaction->GetAccumulatedPoise(), 0.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgePoiseConfigTest, "Hodge.HitReaction.Poise.ConfigurationContracts", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 互斥作者入口、阈值顺序、隐藏旧字段和目标侧选片共同构成配置契约。 */
bool FHodgePoiseConfigTest::RunTest(const FString& Parameters)
{
	using namespace HodgeReactionTests;
	FFixture F;
	auto Request = PoiseRequest(F, 20.f);
	TArray<FText> Errors;
	TestTrue(TEXT("Poise request is valid"), Request.Validate(Errors));
	F.Profile->Animations[0].MotionSource = EHodgeReactionMotionSource::AnimationRootMotion;
	TestEqual(TEXT("Ordinary poise refuses legacy animation displacement"), F.Hit(Request, 1.f).Outcome, EHodgeHitReactionOutcome::Unsupported);
	F.Profile->Animations[0].MotionSource = EHodgeReactionMotionSource::Impact;
	TestEqual(TEXT("Refused motion did not consume threshold"), F.Hit(PoiseRequest(F, 1.f), 1.f).ReactionLevel, EHodgeReactionLevel::Light);
	Request.Impacts.Add(FHodgeImpactSpec());
	Errors.Reset(); TestFalse(TEXT("Poise and direct Impact are mutually exclusive"), Request.Validate(Errors));
	F.Data->Poise.HeavyThreshold = 10.f;
	Errors.Reset(); TestFalse(TEXT("Thresholds must be ordered"), F.Data->Poise.Validate(Errors));
	F.Data->Poise.HeavyThreshold = 55.f;
	auto* S = NewObject<UAnimMontage>(F.Profile);
	auto* B = NewObject<UAnimMontage>(F.Profile);
	F.Profile->bUseCompactAnimations = true;
	F.Profile->Compact.Light.Left = S; F.Profile->Compact.Light.Right = S;
	F.Profile->Compact.Heavy.Left = B; F.Profile->Compact.Heavy.Right = B;
	TestEqual(TEXT("Low visual strength cannot downgrade target heavy"), F.Profile->FindReactionMontage(EHodgeImpactType::HitStun, FVector::ForwardVector, .1f, EHodgeHitSide::Left, 1, EHodgeReactionLevel::Heavy), B);
	TestEqual(TEXT("Large effect strength cannot upgrade target light"), F.Profile->FindReactionMontage(EHodgeImpactType::HitStun, FVector::ForwardVector, 2.f, EHodgeHitSide::Left, 1, EHodgeReactionLevel::Light), S);
	TestFalse(TEXT("Missing compact Fly does not read legacy Launch"), F.Profile->SupportsImpact(EHodgeImpactType::Launch));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgePoiseBoundedHistoryTest, "Hodge.HitReaction.Poise.BoundedHistoryAndHealthCleanup", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 明确拒绝超限样本，保留原窗口；直接致死也立即清理空闲目标的历史。 */
bool FHodgePoiseBoundedHistoryTest::RunTest(const FString& Parameters)
{
	using namespace HodgeReactionTests;
	FFixture F;
	auto Request = PoiseRequest(F, 5.f);
	F.Data->Poise.MaxSamples = 1;
	F.Hit(Request, 1.f);
	F.Hit(Request, 1.f);
	TestEqual(TEXT("Capacity rejects new poise without dropping old samples"), F.Reaction->GetAccumulatedPoise(), 5.f);
	FHodgeHitReactionTestAccess::AgePoise(F.Reaction, 0, 1.6f);
	F.Hit(Request, 1.f);
	TestEqual(TEXT("Expired capacity can be reused"), F.Reaction->GetAccumulatedPoise(), 5.f);
	F.ASC->SetNumericAttributeBase(UHodgeHealthSet::GetHealthAttribute(), 0.f);
	TestEqual(TEXT("Direct fatal health change clears history"), F.Reaction->GetAccumulatedPoise(), 0.f);
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgePoiseDirectionTest, "Hodge.HitReaction.Poise.AttackerOriginSnapshot", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 武器检测体偏置不应把普通击退打向侧面，源快照也不随伤害回调后的攻击者移动漂移。 */
bool FHodgePoiseDirectionTest::RunTest(const FString& Parameters)
{
	using namespace HodgeReactionTests;
	FFixture F;
	F.Pawn->SetActorLocation(FVector(180.f, 0.f, 0.f));
	auto* Source = F.World->SpawnActor<AHodgeCombatCharacter>();
	const FVector ContactOrigin(180.f, 100.f, 0.f);
	const FGuid Id = F.Reaction->BeginHit(PoiseRequest(F, 60.f), Source, ContactOrigin);
	Source->SetActorLocation(FVector(0.f, 200.f, 0.f));
	F.Damage(Id, 1.f);
	TestEqual(TEXT("Real damage commits ordinary heavy control"), F.Reaction->FinishHit(Id).ReactionLevel, EHodgeReactionLevel::Heavy);
	const FVector Direction = F.Reaction->GetReactionState().MotionVelocity;
	TestTrue(TEXT("Ordinary motion points away from attacker snapshot"), Direction.X > 0.f && FMath::Abs(Direction.Y) < .001f);
	F.Reaction->CancelReaction();
	const FGuid Legacy = F.Reaction->BeginHit(F.Request(EHodgeImpactType::Knockback), Source, ContactOrigin);
	F.Damage(Legacy, 1.f);
	F.Reaction->FinishHit(Legacy);
	const FVector LegacyDirection = F.Reaction->GetReactionState().MotionVelocity;
	TestTrue(TEXT("Legacy preserves its explicitly supplied origin"), FMath::Abs(LegacyDirection.X) < .001f && LegacyDirection.Y < 0.f);
	return true;
}

namespace HodgeReactionTests
{
	/** 仅替换真实命中来源，伤害、计划裁决及 GA 提交仍走生产入口。 */
	FHodgeHitReactionResult FacingHit(FFixture& Fixture, AActor* Source, EHodgeImpactType Type = EHodgeImpactType::HitStun)
	{
		const auto Id = Fixture.Reaction->BeginHit(Fixture.Request(Type), Source, FVector(-100.f, 0.f, 0.f));
		Fixture.Damage(Id, 1.f);
		return Fixture.Reaction->FinishHit(Id);
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeFacingFrozenSourceTest, "Hodge.HitReaction.FacingLifecycle.FrozenSourceAndMotion",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 来源与目标的命中位置被冻结，弱来源销毁也不改变选片或原有世界位移。 */
bool FHodgeFacingFrozenSourceTest::RunTest(const FString& Parameters)
{
	HodgeReactionTests::FFixture F;
	F.Profile->Facing.bEnabled = true;
	F.Pawn->SetActorLocation(FVector(0.f, 0.f, 200.f));
	F.Pawn->SetActorRotation(FRotator::ZeroRotator);
	auto* Source = F.World->SpawnActor<AHodgeCombatCharacter>();
	Source->SetActorLocation(FVector(0.f, 100.f, 200.f));
	const auto Id = F.Reaction->BeginHit(F.Request(EHodgeImpactType::Knockback), Source, FVector(-100.f, 0.f, 200.f));
	Source->SetActorLocation(FVector(0.f, -100.f, 200.f));
	Source->Destroy();
	F.Pawn->SetActorLocation(FVector(50.f, 0.f, 200.f));
	F.Damage(Id, 1.f);
	TestEqual(TEXT("来源销毁后合法受击仍提交"), F.Reaction->FinishHit(Id).Outcome, EHodgeHitReactionOutcome::Applied);
	const auto Facing = F.Reaction->GetFacingState();
	TestTrue(TEXT("冻结值不随来源移动或销毁丢失"), Facing.HasIdentity() && Facing.bHasTarget);
	TestEqual(TEXT("兼容None等级击退使用Heavy"), Facing.Preset, EHodgeHitReactionFacingPreset::Heavy);
	TestTrue(TEXT("来源角使用命中时双方位置并按Heavy限幅"), FMath::IsNearlyEqual(Facing.TargetYaw, 60.f, .01f));
	const auto Motion = F.Reaction->GetReactionState().MotionVelocity;
	TestTrue(TEXT("兼容世界运动仍使用原检测原点及构建时目标位置"), Motion.X > 0.f && FMath::Abs(Motion.Y) < .01f);
	TestTrue(TEXT("接触局部方向不被补转来源替换"), F.Reaction->GetLastPresentation().LocalSourceDirection.Equals(-FVector::ForwardVector, .01f));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeFacingChainTakeoverTest, "Hodge.HitReaction.FacingLifecycle.HoldUpgradeAndAir",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 同级不取第二来源，地面升级使用实际起点，空中追加不创建新段。 */
bool FHodgeFacingChainTakeoverTest::RunTest(const FString& Parameters)
{
	using namespace HodgeReactionTests;
	FFixture F;
	F.Profile->Facing.bEnabled = true;
	auto* A = F.World->SpawnActor<AHodgeCombatCharacter>();
	auto* B = F.World->SpawnActor<AHodgeCombatCharacter>();
	A->SetActorLocation(F.Pawn->GetActorLocation() + FVector(0.f, 200.f, 0.f));
	B->SetActorLocation(F.Pawn->GetActorLocation() + FVector(0.f, -200.f, 0.f));
	TestEqual(TEXT("首次轻僵直提交"), FacingHit(F, A).Outcome, EHodgeHitReactionOutcome::Applied);
	const auto First = F.Reaction->GetFacingState();
	TestTrue(TEXT("首次有效来源建立段"), First.HasIdentity() && First.bHasTarget);
	TestEqual(TEXT("另一来源同级受击仍有效"), FacingHit(F, B).Outcome, EHodgeHitReactionOutcome::Applied);
	const auto Same = F.Reaction->GetFacingState();
	TestTrue(TEXT("同级保持原链段及目标"), Same.ChainId == First.ChainId && Same.Segment == First.Segment && Same.TargetYaw == First.TargetYaw);
	const float BeforeHeavyYaw = F.Pawn->GetActorRotation().Yaw;
	TestEqual(TEXT("合法兼容Heavy提交"), FacingHit(F, B, EHodgeImpactType::Knockback).Outcome, EHodgeHitReactionOutcome::Applied);
	const auto Heavy = F.Reaction->GetFacingState();
	TestTrue(TEXT("升级仍在同链但新建一段"), Heavy.ChainId == First.ChainId && Heavy.Segment == First.Segment + 1);
	TestTrue(TEXT("升级以当下实际Yaw为预算起点"), FMath::IsNearlyEqual(Heavy.StartYaw, BeforeHeavyYaw, .01f));
	TestEqual(TEXT("首次Launch提交"), FacingHit(F, A, EHodgeImpactType::Launch).Outcome, EHodgeHitReactionOutcome::Applied);
	const auto Launch = F.Reaction->GetFacingState();
	TestTrue(TEXT("首次击飞是同链更高段"), Launch.ChainId == Heavy.ChainId && Launch.Segment == Heavy.Segment + 1 && Launch.Preset == EHodgeHitReactionFacingPreset::Launch);
	TestEqual(TEXT("尚未物理起飞的追加Launch合法"), FacingHit(F, B, EHodgeImpactType::Launch).Outcome, EHodgeHitReactionOutcome::Applied);
	TestEqual(TEXT("空中追加不能重开补转"), F.Reaction->GetFacingState().Segment, Launch.Segment);
	F.Reaction->FinishFacingSegment(First.ChainId, First.Segment, First.Revision, 120.f, true);
	TestEqual(TEXT("旧段结果不能冻结新段"), F.Reaction->GetFacingState().Revision, Launch.Revision);
	F.Reaction->CancelReaction();
	TestFalse(TEXT("取消使段身份永久失效"), F.Reaction->IsFacingStateCurrent(Launch.ChainId, Launch.Segment));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeFacingEmptyAttemptTest, "Hodge.HitReaction.FacingLifecycle.EmptySourceAttempt",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 首次无效来源也消耗本级尝试，后续同级不能借有效来源补建。 */
bool FHodgeFacingEmptyAttemptTest::RunTest(const FString& Parameters)
{
	using namespace HodgeReactionTests;
	FFixture F;
	F.Profile->Facing.bEnabled = true;
	TestEqual(TEXT("自身来源不影响原受击"), F.Hit(F.Request(), 1.f).Outcome, EHodgeHitReactionOutcome::Applied);
	const auto Empty = F.Reaction->GetFacingState();
	TestTrue(TEXT("记录本级但不授予目标"), Empty.HasIdentity() && !Empty.bHasTarget);
	auto* Source = F.World->SpawnActor<AHodgeCombatCharacter>();
	Source->SetActorLocation(F.Pawn->GetActorLocation() + FVector(0.f, 200.f, 0.f));
	FacingHit(F, Source);
	const auto Same = F.Reaction->GetFacingState();
	TestTrue(TEXT("同级不能补开"), Same.ChainId == Empty.ChainId && Same.Segment == Empty.Segment && !Same.bHasTarget);
	FacingHit(F, Source, EHodgeImpactType::Knockback);
	TestTrue(TEXT("更高反应才重新取源"), F.Reaction->GetFacingState().Segment == Empty.Segment + 1 && F.Reaction->GetFacingState().bHasTarget);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeFacingSubmissionAndDeathTest, "Hodge.HitReaction.FacingLifecycle.SubmissionFailureAndDeath",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 无能力提交不泄漏许可，死亡与直接零生命立即退休且不可由标签移除恢复。 */
bool FHodgeFacingSubmissionAndDeathTest::RunTest(const FString& Parameters)
{
	using namespace HodgeReactionTests;
	FFixture F;
	F.Profile->Facing.bEnabled = true;
	auto* Source = F.World->SpawnActor<AHodgeCombatCharacter>();
	Source->SetActorLocation(F.Pawn->GetActorLocation() + FVector(0.f, 200.f, 0.f));
	F.ASC->ClearAllAbilities();
	TestTrue(TEXT("无受击GA的原提交失败"), FacingHit(F, Source).Outcome != EHodgeHitReactionOutcome::Applied);
	TestFalse(TEXT("失败候选没有可复制段"), F.Reaction->GetFacingState().HasIdentity());
	F.ASC->GiveAbility(FGameplayAbilitySpec(UHodgeGameplayAbility_HitReaction::StaticClass(), 1));
	// 沿用引擎自动化的激活抑制入口，覆盖有合法GA但TryActivate确实失败。
	F.ASC->SetUserAbilityActivationInhibited(true);
	TestTrue(TEXT("被抑制的GA真实激活失败"), FacingHit(F, Source).Outcome != EHodgeHitReactionOutcome::Applied);
	TestFalse(TEXT("激活失败不能发布补转许可"), F.Reaction->GetFacingState().HasIdentity());
	F.ASC->SetUserAbilityActivationInhibited(false);
	TestEqual(TEXT("恢复合法GA后可提交"), FacingHit(F, Source).Outcome, EHodgeHitReactionOutcome::Applied);
	const auto Prior = F.Reaction->GetFacingState();
	const auto BeforeDeathHit = F.Reaction->BeginHit(F.Request(), Source, FVector::ZeroVector);
	F.ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Death);
	TestFalse(TEXT("死亡标签同步清段"), F.Reaction->GetFacingState().HasIdentity());
	F.ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Death);
	TestFalse(TEXT("移除死亡标签不能复活旧许可"), F.Reaction->IsFacingStateCurrent(Prior.ChainId, Prior.Segment));
	F.Damage(BeforeDeathHit, 1.f);
	TestEqual(TEXT("旧待结算命中的原受击裁决保留"), F.Reaction->FinishHit(BeforeDeathHit).Outcome, EHodgeHitReactionOutcome::Applied);
	TestFalse(TEXT("死亡前快照不能在复活后建立新补转链"), F.Reaction->GetFacingState().HasIdentity());
	F.Reaction->CancelReaction();
	FacingHit(F, Source);
	const auto BeforeZero = F.Reaction->GetFacingState();
	const auto BeforeZeroHit = F.Reaction->BeginHit(F.Request(), Source, FVector::ZeroVector);
	F.ASC->SetNumericAttributeBase(UHodgeHealthSet::GetHealthAttribute(), 0.f);
	TestFalse(TEXT("直接零生命在下一帧前清理许可"), F.Reaction->GetFacingState().HasIdentity());
	F.ASC->SetNumericAttributeBase(UHodgeHealthSet::GetHealthAttribute(), 100.f);
	TestFalse(TEXT("恢复数值不能恢复旧段"), F.Reaction->IsFacingStateCurrent(BeforeZero.ChainId, BeforeZero.Segment));
	F.Damage(BeforeZeroHit, 1.f);
	TestEqual(TEXT("恢复生命后旧命中仍沿用原受击规则"), F.Reaction->FinishHit(BeforeZeroHit).Outcome, EHodgeHitReactionOutcome::Applied);
	TestFalse(TEXT("零生命前快照不能在恢复后建立新补转链"), F.Reaction->GetFacingState().HasIdentity());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeFacingRejectedPlansTest, "Hodge.HitReaction.FacingLifecycle.RejectedPlansDoNotOccupy",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 启用补转后，合法来源也不能为未达阈值、霸体、不可中断或动画依赖失败新增占用。 */
bool FHodgeFacingRejectedPlansTest::RunTest(const FString& Parameters)
{
	using namespace HodgeReactionTests;
	FFixture F;
	F.Profile->Facing.bEnabled = true;
	auto* Source = F.World->SpawnActor<AHodgeCombatCharacter>();
	Source->SetActorLocation(F.Pawn->GetActorLocation() + FVector(0.f, 200.f, 0.f));
	// 每项均从真实命中结算入口提交，避免无效来源让错误的补转提交被动漏检。
	auto CheckRejected = [&](const TCHAR* Label, const FHodgeHitReactionConfig& Request, EHodgeHitReactionOutcome Expected)
	{
		const FGuid Id = F.Reaction->BeginHit(Request, Source, Source->GetActorLocation());
		F.Damage(Id, 1.f);
		const auto Result = F.Reaction->FinishHit(Id);
		TestEqual(FString::Printf(TEXT("%s沿用原裁决"), Label), Result.Outcome, Expected);
		TestFalse(FString::Printf(TEXT("%s不建立补转身份"), Label), F.Reaction->GetFacingState().HasIdentity());
		TestFalse(FString::Printf(TEXT("%s不建立受击控制"), Label), F.Reaction->IsControlled());
	};
	auto Empty = F.Request();
	Empty.Impacts.Reset();
	CheckRejected(TEXT("空效果接触"), Empty, EHodgeHitReactionOutcome::NoImpact);
	CheckRejected(TEXT("削韧未达阈值"), PoiseRequest(F, 1.f), EHodgeHitReactionOutcome::NoImpact);
	F.ASC->AddLooseGameplayTag(HodgeHitReactionTags::Body_SuperArmor);
	CheckRejected(TEXT("霸体低判定"), F.Request(), EHodgeHitReactionOutcome::LowJudgement);
	F.ASC->RemoveLooseGameplayTag(HodgeHitReactionTags::Body_SuperArmor);

	const auto ActionHandle = F.ASC->GiveAbility(FGameplayAbilitySpec(UHodgeHitReactionTestAbility::StaticClass(), 1));
	if (!TestTrue(TEXT("不可中断专项真实动作已激活"), F.ASC->TryActivateAbility(ActionHandle, false))) { return false; }
	auto* Action = F.ASC->FindAbilitySpecFromHandle(ActionHandle)->GetPrimaryInstance();
	Action->SetCanBeCanceled(false);
	CheckRejected(TEXT("不可中断动作"), F.Request(), EHodgeHitReactionOutcome::CannotInterrupt);
	TestTrue(TEXT("拒绝受击后原动作仍活动"), Action->IsActive());
	Action->SetCanBeCanceled(true);
	F.ASC->CancelAbilityHandle(ActionHandle);
	TestFalse(TEXT("专项拥有的动作已清理"), Action->IsActive());

	// 缺少必需动画保持Unsupported，不得先取得补转许可再伪装为配置兼容。
	for (auto& Entry : F.Profile->Animations)
	{
		if (Entry.Type == EHodgeImpactType::HitStun) { Entry.bAllowWithoutMontage = false; Entry.Montage = nullptr; }
	}
	CheckRejected(TEXT("必需动画缺失"), F.Request(), EHodgeHitReactionOutcome::Unsupported);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeFacingRootMotionRetirementTest, "Hodge.HitReaction.FacingLifecycle.ExcludedPlanFreezesPrior",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 新合法根运动计划不启用补转，也不能承接旧段尚未结束的转向。 */
bool FHodgeFacingRootMotionRetirementTest::RunTest(const FString& Parameters)
{
	using namespace HodgeReactionTests;
	FFixture F;
	F.Profile->Facing.bEnabled = true;
	auto* Source = F.World->SpawnActor<AHodgeCombatCharacter>();
	Source->SetActorLocation(F.Pawn->GetActorLocation() + FVector(0.f, 200.f, 0.f));
	FacingHit(F, Source, EHodgeImpactType::Knockback);
	const auto Prior = F.Reaction->GetFacingState();
	F.Profile->Animations[1].MotionSource = EHodgeReactionMotionSource::AnimationRootMotion;
	TestEqual(TEXT("原有无动画兼容根运动路径仍可提交"), FacingHit(F, Source, EHodgeImpactType::Knockback).Outcome, EHodgeHitReactionOutcome::Applied);
	const auto Frozen = F.Reaction->GetFacingState();
	TestTrue(TEXT("排除计划记录实际终点并永久退休旧段"), Frozen.ChainId == Prior.ChainId && Frozen.Segment == Prior.Segment && Frozen.bFinal && Frozen.bRetired);
	TestTrue(TEXT("冻结使用实际ActorYaw"), FMath::IsNearlyEqual(Frozen.FinalYaw, F.Pawn->GetActorRotation().Yaw, .01f));
	TestFalse(TEXT("根运动计划不能借旧Final取得可选Yaw资格"), F.Reaction->IsFacingStateCurrent(Frozen.ChainId, Frozen.Segment));
	F.Pawn->SetActorRotation(FRotator(0.f, 40.f, 0.f));
	// 恢复同类型同级运动，不用原规则会拒绝的轻僵直降级冒充Root→Impact交接。
	F.Profile->Animations[1].MotionSource = EHodgeReactionMotionSource::Impact;
	TestEqual(TEXT("后续同级Impact仍按原规则受击"), FacingHit(F, Source, EHodgeImpactType::Knockback).Outcome, EHodgeHitReactionOutcome::Applied);
	const auto Same = F.Reaction->GetFacingState();
	TestTrue(TEXT("同级保留退休记录且不重新取源"), Same.ChainId == Frozen.ChainId && Same.Segment == Frozen.Segment && Same.bRetired);
	TestFalse(TEXT("回到Impact也不能重新校正旧段"), F.Reaction->IsFacingStateCurrent(Same.ChainId, Same.Segment));
	TestEqual(TEXT("合法更高Launch仍可接管"), FacingHit(F, Source, EHodgeImpactType::Launch).Outcome, EHodgeHitReactionOutcome::Applied);
	const auto Upgraded = F.Reaction->GetFacingState();
	TestTrue(TEXT("升级新段重新按实际朝向取源"), Upgraded.ChainId == Frozen.ChainId && Upgraded.Segment == Frozen.Segment + 1 &&
		!Upgraded.bRetired && Upgraded.bHasTarget && FMath::IsNearlyEqual(Upgraded.StartYaw, 40.f, .01f));
	// 阶段切换与新计划入口共享退休规则，已经冻结的实际终点不能被根运动改写。
	for (const auto Phase : {EHodgeHitReactionPhase::Downed, EHodgeHitReactionPhase::GettingUp})
	{
		FFixture Stage;
		Stage.Profile->Facing.bEnabled = true;
		auto* StageSource = Stage.World->SpawnActor<AHodgeCombatCharacter>();
		StageSource->SetActorLocation(Stage.Pawn->GetActorLocation() + FVector(0.f, 200.f, 0.f));
		FacingHit(Stage, StageSource);
		const auto StageFacing = Stage.Reaction->GetFacingState();
		Stage.Pawn->SetActorRotation(FRotator(0.f, 12.f, 0.f));
		Stage.Reaction->FinishFacingSegment(StageFacing.ChainId, StageFacing.Segment, StageFacing.Revision, 12.f, false);
		const auto Ended = Stage.Reaction->GetFacingState();
		Stage.Pawn->SetActorRotation(FRotator(0.f, 50.f, 0.f));
		FHodgeHitReactionTestAccess::ChangeMotionPhase(Stage.Reaction, EHodgeReactionMotionSource::AnimationRootMotion, Phase);
		const auto Retired = Stage.Reaction->GetFacingState();
		TestTrue(TEXT("根运动落地或起身仅退休既有结束记录"), Retired.bRetired && Retired.bFinal &&
			Retired.ChainId == Ended.ChainId && Retired.Segment == Ended.Segment && Retired.Revision == Ended.Revision + 1 && Retired.FinalYaw == Ended.FinalYaw);
		FHodgeHitReactionTestAccess::ChangeMotionPhase(Stage.Reaction, EHodgeReactionMotionSource::Impact, EHodgeHitReactionPhase::Controlled);
		TestFalse(TEXT("后续原地阶段不能恢复旧段资格"), Stage.Reaction->IsFacingStateCurrent(Retired.ChainId, Retired.Segment));
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeSegmentLaunchAuthoringTest, "Hodge.HitReaction.SegmentLaunch.AuthoritativeSegmentFlag", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 默认反应与通知覆盖都不能代替分段自己的击飞开关。 */
bool FHodgeSegmentLaunchAuthoringTest::RunTest(const FString& Parameters)
{
	HodgeReactionTests::FFixture F;
	FHodgeAnimHitConfig Segment;
	auto Default = F.Request(EHodgeImpactType::Launch);
	TestFalse(TEXT("New segment defaults to no launch even with launch default"), Segment.ResolveReaction(Default).bCanLaunch);
	Segment.bCanLaunch = true;
	TestTrue(TEXT("Segment enables launch while inheriting default reaction"), Segment.ResolveReaction(Default).bCanLaunch);
	Segment.bUseDefaultReaction = false;
	Segment.ReactionOverride = F.Request();
	TestTrue(TEXT("Overridden reaction still receives the segment flag"), Segment.ResolveReaction(Default).bCanLaunch);
	Segment.bCanLaunch = false;
	Segment.ReactionOverride.bCanLaunch = true;
	TestFalse(TEXT("Runtime flag inside override cannot bypass the segment"), Segment.ResolveReaction(Default).bCanLaunch);
	auto Disabled = Segment.ResolveReaction(Default);
	Disabled.Impacts = Default.Impacts;
	TestEqual(TEXT("Old Launch spec alone cannot launch"), F.Hit(Disabled).Outcome, EHodgeHitReactionOutcome::NoImpact);
	TestFalse(TEXT("Unflagged legacy launch does not create control"), F.Reaction->IsControlled());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeSegmentLaunchReplayTest, "Hodge.HitReaction.SegmentLaunch.ReplayAndLanding", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 零削韧也可按分段击飞，空中再次命中只由开关决定是否续播 Fly。 */
bool FHodgeSegmentLaunchReplayTest::RunTest(const FString& Parameters)
{
	using namespace HodgeReactionTests;
	FFixture F;
	auto Launch = PoiseRequest(F, 0.f);
	Launch.bCanLaunch = true;
	TestEqual(TEXT("Flagged zero-poise hit launches"), F.Hit(Launch, 1.f).Outcome, EHodgeHitReactionOutcome::Applied);
	const auto First = F.Reaction->GetReactionState();
	TestEqual(TEXT("Flight is not a poise break level"), First.ReactionLevel, EHodgeReactionLevel::None);
	TestEqual(TEXT("Launch does not accumulate poise"), F.Reaction->GetAccumulatedPoise(), 0.f);
	F.Pawn->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	// 原生夹具不是已注册的 PIE World，只推进组件；真实物理轨迹由后续 PIE 验证。
	F.Reaction->TickPlan();
	TestEqual(TEXT("Flagged air hit restarts Fly"), F.Hit(Launch, 1.f).Outcome, EHodgeHitReactionOutcome::Applied);
	const auto Replayed = F.Reaction->GetReactionState();
	TestTrue(TEXT("Replayed Fly has a new identity"), Replayed.Sequence != First.Sequence);
	TestEqual(TEXT("Flight keeps original chain time"), Replayed.ChainStartedAt, First.ChainStartedAt);
	TestEqual(TEXT("Flight keeps original airborne time"), Replayed.AirStartedAt, First.AirStartedAt);
	TestEqual(TEXT("Replayed Fly spends one air-hit slot"), Replayed.AirHitCount, 1);
	const auto Pending = F.Pawn->GetCharacterMovement()->PendingLaunchVelocity;
	TestEqual(TEXT("Unflagged air hit does not restart flight"), F.Hit(PoiseRequest(F, 10000.f), 1.f).Outcome, EHodgeHitReactionOutcome::NoImpact);
	TestEqual(TEXT("Unflagged air hit keeps identity"), F.Reaction->GetReactionState().Sequence, Replayed.Sequence);
	TestTrue(TEXT("Contact preserves queued CMC motion"), F.Pawn->GetCharacterMovement()->PendingLaunchVelocity.Equals(Pending));
	TestTrue(TEXT("Unflagged contact takes normal down-and-get-up path"), F.Reaction->GetReactionState().bKnockdownOnLanding);
	F.Reaction->TickPlan();
	F.Pawn->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	F.Reaction->TickPlan();
	TestEqual(TEXT("Actual ground contact enters Downed"), F.Reaction->GetReactionState().Phase, EHodgeHitReactionPhase::Downed);
	TestTrue(TEXT("Downed can use existing get-up entry"), F.Reaction->RequestGetUp());
	TestFalse(TEXT("Animation-free fixture cleans control after recovery"), F.Reaction->IsControlled());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeSegmentLaunchGuardsTest, "Hodge.HitReaction.SegmentLaunch.AcceptanceBodyAndLimits", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 分段开关不是免疫／霸体绕过入口，重复 Finish 和次数上限不能无限续飞。 */
bool FHodgeSegmentLaunchGuardsTest::RunTest(const FString& Parameters)
{
	using namespace HodgeReactionTests;
	FFixture F;
	auto Request = PoiseRequest(F, 0.f); Request.bCanLaunch = true;
	TestEqual(TEXT("Zero actual damage cannot launch"), F.Hit(Request, 0.f).Outcome, EHodgeHitReactionOutcome::NoDamageOutput);
	F.ASC->AddLooseGameplayTag(HodgeHitReactionTags::Body_Vajra);
	TestEqual(TEXT("Flag cannot bypass target armor"), F.Hit(Request, 1.f).Outcome, EHodgeHitReactionOutcome::LowJudgement);
	F.ASC->RemoveLooseGameplayTag(HodgeHitReactionTags::Body_Vajra);
	F.Profile->MaxAirHits = 1;
	const auto Id = F.Reaction->BeginHit(Request, nullptr, FVector(-100.f, 0.f, 0.f));
	F.Damage(Id, 1.f);
	TestEqual(TEXT("Accepted hit launches once"), F.Reaction->FinishHit(Id).Outcome, EHodgeHitReactionOutcome::Applied);
	const int32 FirstSequence = F.Reaction->GetReactionState().Sequence;
	F.Reaction->FinishHit(Id);
	TestEqual(TEXT("Duplicate Finish cannot replay Fly"), F.Reaction->GetReactionState().Sequence, FirstSequence);
	TestEqual(TEXT("First rehit can fly"), F.Hit(Request, 1.f).Outcome, EHodgeHitReactionOutcome::Applied);
	const int32 LastSequence = F.Reaction->GetReactionState().Sequence;
	TestEqual(TEXT("Air-hit cap prevents further Fly"), F.Hit(Request, 1.f).Outcome, EHodgeHitReactionOutcome::NoImpact);
	TestEqual(TEXT("Capped hit retains existing flight"), F.Reaction->GetReactionState().Sequence, LastSequence);
	F.Reaction->UninitializeFromAbilitySystem();
	TestFalse(TEXT("Unbind clears control"), F.Reaction->IsControlled());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeSegmentLaunchRelevantSettingsTest, "Hodge.HitReaction.SegmentLaunch.IndependentSettings",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** Fly只依赖目标运动预设；非法轻重窗口仍由完整配置校验报告。 */
bool FHodgeSegmentLaunchRelevantSettingsTest::RunTest(const FString& Parameters)
{
	using namespace HodgeReactionTests;
	for (int32 Case = 0; Case < 7; ++Case)
	{
		FFixture F;
		auto Request = PoiseRequest(F, 0.f);
		Request.bCanLaunch = true;
		auto& Rules = F.Data->Poise;
		switch (Case)
		{
		case 0: Rules.Window = -1.f; break;
		case 1: Rules.HeavyThreshold = Rules.LightThreshold; break;
		case 2: Rules.MaxSamples = 0; break;
		case 3: Rules.MoveDuration = 0.f; break;
		case 4: Rules.HeavyDistance = -1.f; break;
		case 5: Rules.MaxDistance = -1.f; break;
		case 6: Rules.bEnabled = false; Rules.Window = -1.f; break;
		}
		TArray<FText> Errors;
		if (Case < 6) { TestFalse(TEXT("Full enabled settings still diagnose invalid poise fields"), Rules.Validate(Errors)); }
		Errors.Reset();
		TestTrue(TEXT("Unrelated poise fields do not invalidate launch motion"), Rules.ValidateLaunchMotion(Errors));
		TestEqual(TEXT("A valid segment independently launches with target preset"), F.Hit(Request, 1.f).Outcome, EHodgeHitReactionOutcome::Applied);
	}
	for (const bool bInvalidScale : {false, true})
	{
		FFixture F;
		auto Request = PoiseRequest(F, 0.f); Request.bCanLaunch = true;
		if (bInvalidScale) { F.Data->Poise.DisplacementScale = -1.f; }
		else { F.Data->Poise.LaunchVerticalSpeed = std::numeric_limits<float>::quiet_NaN(); }
		TestEqual(TEXT("Relevant invalid launch parameters are still rejected"), F.Hit(Request, 1.f).Outcome, EHodgeHitReactionOutcome::Unsupported);
	}
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeSelectedReactionValidationTest, "Hodge.HitReaction.SelectedPresentationValidation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 左侧资源错误仍完整报告，但不阻断实际选用的合法右侧动作。 */
bool FHodgeSelectedReactionValidationTest::RunTest(const FString& Parameters)
{
	HodgeReactionTests::FFixture F;
	auto* Montage = LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Main/Character/Hero/Anim/HitReactions/Behit/InPlace/AM_IP_Behit_S_R.AM_IP_Behit_S_R"));
	if (!TestNotNull(TEXT("Authored in-place reaction montage exists"), Montage)) { return false; }
	auto* Mesh = NewObject<USkeletalMesh>(F.Profile);
	Mesh->SetSkeleton(Montage->GetSkeleton());
	auto& Entry = F.Profile->Animations[0];
	Entry.Montage = Montage;
	FHodgeHitReactionVariant Left;
	Left.Direction = EHodgeHitPresentationDirection::Left;
	Entry.Variants.Add(Left);
	FHodgeHitReactionVariant Right;
	Right.Direction = EHodgeHitPresentationDirection::Right;
	Right.Montage = Montage;
	Entry.Variants.Add(Right);
	TArray<FText> Errors;
	TestFalse(TEXT("Full asset validation retains the invalid left variant"), F.Profile->Validate(Errors));
	FHodgeHitReactionAnimation Selected;
	TestTrue(TEXT("Right path resolves independently"), F.Profile->ResolvePresentation(EHodgeImpactType::HitStun,
		FVector::RightVector, 1.f, EHodgeHitSide::Right, 0, Selected));
	FHodgeHitReactionState Plan;
	Plan.Type = EHodgeImpactType::HitStun;
	Plan.Phase = EHodgeHitReactionPhase::Controlled;
	Plan.Montage = Selected.Montage;
	Plan.EndsAt = 0.4f;
	Errors.Reset();
	TestTrue(TEXT("Unrelated left error cannot reject the right plan"), F.Profile->ValidatePlanPresentation(Selected, Plan, Mesh, Errors));
	F.Profile->ResolvePresentation(EHodgeImpactType::HitStun, -FVector::RightVector, 1.f, EHodgeHitSide::Left, 0, Selected);
	Plan.Montage = Selected.Montage;
	Errors.Reset();
	TestFalse(TEXT("The actual missing left montage is rejected"), F.Profile->ValidatePlanPresentation(Selected, Plan, Mesh, Errors));
	Entry.Variants.Add(Right);
	Entry.Variants.Last().Montage = LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Main/Character/Hero/Anim/HitReactions/Behit/InPlace/AM_IP_Behit_S_L.AM_IP_Behit_S_L"));
	if (!TestNotNull(TEXT("Second authored montage exists for ambiguous right variant"), Entry.Variants.Last().Montage.Get())) { return false; }
	F.Profile->ResolvePresentation(EHodgeImpactType::HitStun, FVector::RightVector, 1.f, EHodgeHitSide::Right, 0, Selected);
	Plan.Montage = Selected.Montage;
	Errors.Reset();
	TestFalse(TEXT("Duplicate selected direction and threshold remain invalid"), F.Profile->ValidatePlanPresentation(Selected, Plan, Mesh, Errors));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionRelevantPathTest, "Hodge.HitReaction.RelevantPathValidation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 通过真实命中事务区分无关家族错误与正在执行的击退曲线错误。 */
bool FHodgeReactionRelevantPathTest::RunTest(const FString& Parameters)
{
	HodgeReactionTests::FFixture F;
	F.Profile->Animations[4].bAllowWithoutMontage = false;
	F.Profile->KnockbackStrengthCurve = NewObject<UCurveFloat>(F.Profile);
	F.Profile->DirectionalLightFeedback.AddDefaulted();
	TArray<FText> Errors;
	TestFalse(TEXT("Whole profile still reports unrelated air, feedback and curve errors"), F.Profile->Validate(Errors));
	TestEqual(TEXT("Valid animation-free ground reaction still executes"), F.Hit(F.Request(), 1.f).Outcome, EHodgeHitReactionOutcome::Applied);
	F.Reaction->CancelReaction();
	TestEqual(TEXT("Empty curve rejects actual external knockback"), F.Hit(F.Request(EHodgeImpactType::Knockback), 1.f).Outcome, EHodgeHitReactionOutcome::Unsupported);
	// 兼容根运动路线不使用外部曲线，但原有移动时长对控制期限的贡献不能丢失。
	F.Profile->Animations[1].MotionSource = EHodgeReactionMotionSource::AnimationRootMotion;
	auto RootRequest = F.Request(EHodgeImpactType::Knockback);
	RootRequest.Impacts[0].ControlDuration = 0.1f;
	RootRequest.Impacts[0].MoveDuration = 0.9f;
	TestEqual(TEXT("Explicit animation-free root-motion fixture ignores unused external curve"), F.Hit(RootRequest, 1.f).Outcome, EHodgeHitReactionOutcome::Applied);
	const auto State = F.Reaction->GetReactionState();
	TestTrue(TEXT("Legacy root-motion knockback retains original control duration"), FMath::IsNearlyEqual(State.EndsAt - State.StartedAt, 0.9f));
	F.Reaction->CancelReaction();
	const FHodgeHitReactionAnimation Duplicate = F.Profile->Animations[1];
	F.Profile->Animations.Add(Duplicate);
	F.Profile->FallbackToHitStun.Add(EHodgeImpactType::Knockback);
	TestEqual(TEXT("Duplicate actual type is invalid rather than missing and cannot trigger fallback"), F.Hit(RootRequest, 1.f).Outcome, EHodgeHitReactionOutcome::Unsupported);
	F.Profile->MaxControlDuration = 0.f;
	TestEqual(TEXT("Required control safety remains enforced"), F.Hit(F.Request(), 1.f).Outcome, EHodgeHitReactionOutcome::Unsupported);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionRequiredAirResourcesTest, "Hodge.HitReaction.RequiredAirResources",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 实际空中路径必须具备已约定的完整家族，不能因缩小校验范围而漏过落地资源。 */
bool FHodgeReactionRequiredAirResourcesTest::RunTest(const FString& Parameters)
{
	HodgeReactionTests::FFixture F;
	auto* Montage = LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Main/Character/Hero/Anim/HitReactions/Behit/InPlace/AM_IP_Behit_S_R.AM_IP_Behit_S_R"));
	if (!TestNotNull(TEXT("Existing authored montage is available"), Montage)) { return false; }
	auto* Mesh = NewObject<USkeletalMesh>(F.Profile);
	Mesh->SetSkeleton(Montage->GetSkeleton());
	F.Profile->bUseCompactAnimations = true;
	F.Profile->Compact.Fly.Start = Montage;
	F.Profile->Compact.Fly.Loop = Montage;
	F.Profile->Compact.Fly.Landing = Montage;
	FHodgeHitReactionAnimation Selected;
	TestTrue(TEXT("Fly resolves without requiring unused Push"), F.Profile->ResolvePresentation(EHodgeImpactType::Launch,
		FVector::ForwardVector, 1.f, EHodgeHitSide::Automatic, 0, Selected));
	FHodgeHitReactionState Plan;
	Plan.Type = EHodgeImpactType::Launch;
	Plan.Phase = EHodgeHitReactionPhase::Airborne;
	Plan.Montage = Selected.Montage;
	Plan.AirLoopMontage = Selected.AirLoopMontage;
	Plan.LandingMontage = Selected.LandingMontage;
	TArray<FText> Errors;
	TestTrue(TEXT("Complete actual Fly family is valid"), F.Profile->ValidatePlanPresentation(Selected, Plan, Mesh, Errors));
	Plan.LandingMontage = nullptr;
	Errors.Reset();
	TestFalse(TEXT("Missing required landing is rejected"), F.Profile->ValidatePlanPresentation(Selected, Plan, Mesh, Errors));
	Plan.LandingMontage = Montage;
	Plan.AirLoopMontage = nullptr;
	Errors.Reset();
	TestFalse(TEXT("Missing required air loop is rejected"), F.Profile->ValidatePlanPresentation(Selected, Plan, Mesh, Errors));
	F.Profile->bUseCompactAnimations = false;
	Plan.AirLoopMontage = Montage;
	Plan.bKnockdownOnLanding = true;
	Errors.Reset();
	TestTrue(TEXT("Legacy airborne path retains explicit optional get-up contract"), F.Profile->ValidatePlanPresentation(Selected, Plan, Mesh, Errors));
	const FHodgeHitReactionAnimation DuplicateDown = F.Profile->Animations[3];
	F.Profile->Animations.Add(DuplicateDown);
	Errors.Reset();
	TestFalse(TEXT("Actual later Knockdown selection must also be unambiguous"), F.Profile->ValidatePlanPresentation(Selected, Plan, Mesh, Errors));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionLandingFallbackContractTest, "Hodge.HitReaction.LandingFallbackContract",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 后续倒地只验证真正消费的回退主片，并保留旧布局显式无动画许可。 */
bool FHodgeReactionLandingFallbackContractTest::RunTest(const FString&)
{
	HodgeReactionTests::FFixture F;
	auto* Montage = LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Main/Character/Hero/Anim/HitReactions/Behit/InPlace/AM_IP_Behit_S_R.AM_IP_Behit_S_R"));
	if (!TestNotNull(TEXT("Authored montage exists"), Montage)) { return false; }
	auto* Mesh = NewObject<USkeletalMesh>(F.Profile);
	Mesh->SetSkeleton(Montage->GetSkeleton());
	F.Profile->Animations[2].Montage = Montage;
	F.Profile->Animations[3].bAllowWithoutMontage = false;
	FHodgeHitReactionAnimation Selected;
	F.Profile->ResolvePresentation(EHodgeImpactType::Launch, FVector::ForwardVector, 1.f, EHodgeHitSide::Automatic, 0, Selected);
	FHodgeHitReactionState Plan;
	Plan.Type = EHodgeImpactType::Launch;
	Plan.Phase = EHodgeHitReactionPhase::Airborne;
	Plan.Montage = Montage;
	Plan.bKnockdownOnLanding = true;
	TArray<FText> Errors;
	TestFalse(TEXT("Missing mandatory future downed fallback is rejected"), F.Profile->ValidatePlanPresentation(Selected, Plan, Mesh, Errors));
	Plan.LandingMontage = Montage;
	Errors.Reset();
	TestTrue(TEXT("Independent landing does not require unused downed main clip"), F.Profile->ValidatePlanPresentation(Selected, Plan, Mesh, Errors));
	Plan.LandingMontage = nullptr;
	F.Profile->Animations[3].bAllowWithoutMontage = true;
	Errors.Reset();
	TestTrue(TEXT("Explicit animation-free downed fallback stays supported"), F.Profile->ValidatePlanPresentation(Selected, Plan, Mesh, Errors));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionSelectedPhaseContextTest, "Hodge.HitReaction.SelectedPhaseContext",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 跨条目换片不能被误判成旧动作续播；真正续播仍忽略未播放的新变体。 */
bool FHodgeReactionSelectedPhaseContextTest::RunTest(const FString&)
{
	HodgeReactionTests::FFixture F;
	auto* Right = LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Main/Character/Hero/Anim/HitReactions/Behit/InPlace/AM_IP_Behit_S_R.AM_IP_Behit_S_R"));
	auto* Left = LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Main/Character/Hero/Anim/HitReactions/Behit/InPlace/AM_IP_Behit_S_L.AM_IP_Behit_S_L"));
	if (!TestNotNull(TEXT("Right authored montage exists"), Right) || !TestNotNull(TEXT("Left authored montage exists"), Left)) { return false; }
	F.Pawn->GetMesh()->SetSkeletalMesh(LoadObject<USkeletalMesh>(nullptr,
		TEXT("/Game/Main/Character/Hero/Anim/Model/SKM_Pover_LyraLab.SKM_Pover_LyraLab")));
	F.Pawn->GetMesh()->SetAnimInstanceClass(UAnimInstance::StaticClass());
	F.ASC->RefreshAbilityActorInfo();
	if (!TestNotNull(TEXT("Actual animation dependency is ready"), F.Pawn->GetMesh()->GetAnimInstance())) { return false; }
	F.Pawn->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	F.Profile->Animations[3].Montage = Left;
	auto& Air = F.Profile->Animations[4];
	Air.Montage = Right;
	FHodgeHitReactionVariant Variant;
	Variant.Direction = EHodgeHitPresentationDirection::Right;
	Variant.Montage = Right;
	Air.Variants = {Variant, Variant};
	FHodgeHitReactionState Plan;
	TestFalse(TEXT("Actual selected AirHit ambiguity rejects airborne Knockdown"), FHodgeHitReactionTestAccess::Build(F.Reaction, F.Request(EHodgeImpactType::Knockdown), Plan));
	Air.Variants.Pop();
	Plan = {};
	TestTrue(TEXT("Valid selected AirHit permits airborne Knockdown"), FHodgeHitReactionTestAccess::Build(F.Reaction, F.Request(EHodgeImpactType::Knockdown), Plan));
	TestEqual(TEXT("AirHit is the actual main presentation"), Plan.Montage.Get(), Right);
	TestEqual(TEXT("Knockdown is retained as actual landing"), Plan.LandingMontage.Get(), Left);
	Variant.Montage = Left;
	F.Profile->Animations[3].Variants = {Variant, Variant};
	Plan = {};
	TestFalse(TEXT("Actual selected landing ambiguity is also rejected"), FHodgeHitReactionTestAccess::Build(F.Reaction, F.Request(EHodgeImpactType::Knockdown), Plan));
	F.Profile->Animations[3].Variants.Reset();
	F.Pawn->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	auto& Ground = F.Profile->Animations[0];
	Ground.Montage = Right;
	Plan = {};
	TestTrue(TEXT("Initial ground plan is valid"), FHodgeHitReactionTestAccess::Build(F.Reaction, F.Request(), Plan));
	FHodgeHitReactionTestAccess::SetPlan(F.Reaction, Plan);
	Variant.Montage = Left;
	Ground.Variants = {Variant, Variant};
	Plan = {};
	TestTrue(TEXT("Continuing old pose ignores unused new ambiguous variants"), FHodgeHitReactionTestAccess::Build(F.Reaction, F.Request(), Plan));
	TestEqual(TEXT("Continuation keeps the original main clip"), Plan.Montage.Get(), Right);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionFinalAnimationDependencyTest, "Hodge.HitReaction.FinalAnimationDependency",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 跨阶段最终选片没有动画实例时提前拒绝，不能先取消目标仍在执行的动作。 */
bool FHodgeReactionFinalAnimationDependencyTest::RunTest(const FString&)
{
	HodgeReactionTests::FFixture F;
	auto* Montage = LoadObject<UAnimMontage>(nullptr,
		TEXT("/Game/Main/Character/Hero/Anim/HitReactions/Behit/InPlace/AM_IP_Behit_S_R.AM_IP_Behit_S_R"));
	auto* Mesh = LoadObject<USkeletalMesh>(nullptr,
		TEXT("/Game/Main/Character/Hero/Anim/Model/SKM_Pover_LyraLab.SKM_Pover_LyraLab"));
	if (!TestNotNull(TEXT("Authored AirHit clip exists"), Montage) || !TestNotNull(TEXT("Compatible target mesh exists"), Mesh)) { return false; }
	F.Pawn->GetMesh()->SetSkeletalMesh(Mesh);
	F.ASC->RefreshAbilityActorInfo();
	if (!TestNull(TEXT("Fixture deliberately has no animation instance"), F.ASC->AbilityActorInfo->GetAnimInstance())) { return false; }
	F.Profile->Animations[4].Montage = Montage;
	F.Profile->Animations[4].bAllowWithoutMontage = false;
	const auto Handle = F.ASC->GiveAbility(FGameplayAbilitySpec(UHodgeHitReactionTestAbility::StaticClass(), 1));
	if (!TestTrue(TEXT("Existing cancellable action starts before the hit"), F.ASC->TryActivateAbility(Handle))) { return false; }
	auto* Ability = F.ASC->FindAbilitySpecFromHandle(Handle)->GetPrimaryInstance();
	F.Pawn->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	const FVector Velocity(11.f, 7.f, -30.f);
	F.Pawn->GetCharacterMovement()->Velocity = Velocity;
	FHodgeHitReactionState Plan;
	const auto Request = F.Request(EHodgeImpactType::Knockdown);
	TestFalse(TEXT("Final AirHit dependency rejects the plan before submission"), FHodgeHitReactionTestAccess::Build(F.Reaction, Request, Plan));
	TestEqual(TEXT("Real hit reports unsupported presentation before cancelling anything"), F.Hit(Request).Outcome, EHodgeHitReactionOutcome::Unsupported);
	TestTrue(TEXT("Rejected reaction preserves the original active action"), Ability->IsActive());
	TestFalse(TEXT("Rejected reaction owns no control"), F.Reaction->IsControlled());
	TestFalse(TEXT("Rejected reaction adds no control tag"), F.ASC->HasMatchingGameplayTag(HodgeHitReactionTags::Controlled));
	TestTrue(TEXT("Rejected reaction does not change the target velocity"), F.Pawn->GetCharacterMovement()->Velocity.Equals(Velocity));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeReactionRelevantMotionPolicyTest, "Hodge.HitReaction.RelevantMotionPolicy",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 用户确认普通削韧只约束实际执行阶段，完整资产诊断继续保留。 */
bool FHodgeReactionRelevantMotionPolicyTest::RunTest(const FString&)
{
	HodgeReactionTests::FFixture F;
	auto Request = F.Request();
	Request.InputMode = EHodgeReactionInputMode::PoiseDriven;
	F.Profile->Animations[0].LandingMotionSource = EHodgeReactionMotionSource::AnimationRootMotion;
	F.Profile->Animations[0].GetUpMotionSource = EHodgeReactionMotionSource::AnimationRootMotion;
	FHodgeHitReactionState Plan;
	TestTrue(TEXT("Unused landing/get-up motion does not block ground poise"), FHodgeHitReactionTestAccess::Build(F.Reaction, Request, Plan));
	F.Profile->Animations[0].MotionSource = EHodgeReactionMotionSource::AnimationRootMotion;
	Plan = {};
	TestFalse(TEXT("Actual poise ground motion must still use Impact"), FHodgeHitReactionTestAccess::Build(F.Reaction, Request, Plan));
	Request = F.Request(EHodgeImpactType::Launch);
	Request.InputMode = EHodgeReactionInputMode::PoiseDriven;
	Request.Impacts[0].bKnockdownOnLanding = true;
	F.Profile->Animations[3].GetUpMotionSource = EHodgeReactionMotionSource::AnimationRootMotion;
	Plan = {};
	TestTrue(TEXT("Animation-free optional get-up does not execute its motion source"), FHodgeHitReactionTestAccess::Build(F.Reaction, Request, Plan));
	auto* GetUp = LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Main/Character/Hero/Anim/HitReactions/Behit/AM_StandUp.AM_StandUp"));
	if (!TestNotNull(TEXT("Existing root-motion get-up exists"), GetUp) || !TestTrue(TEXT("Fixture get-up carries root motion"), GetUp->HasRootMotion())) { return false; }
	auto* Mesh = NewObject<USkeletalMesh>(F.Profile);
	Mesh->SetSkeleton(GetUp->GetSkeleton());
	F.Pawn->GetMesh()->SetSkeletalMesh(Mesh);
	F.Profile->Animations[3].GetUpMontage = GetUp;
	Plan = {};
	TestFalse(TEXT("Actual later poise get-up cannot enable animation movement"), FHodgeHitReactionTestAccess::Build(F.Reaction, Request, Plan));
	return true;
}

namespace HodgeReactionTests
{
	/** 每个场景只描述输入和预期边界，不实现选片、运动或预算计算。 */
	struct FPlanScenario
	{
		FString Name;
		FHodgeHitReactionConfig Request;
		FHodgeHitPresentation Presentation;
		TWeakObjectPtr<AActor> Source;
		FVector Origin;
		EHodgeReactionLevel Level = EHodgeReactionLevel::None;
		FHodgeHitReactionState InitialPlan;
		TArray<EHodgeImpactType> InitialApplied;
		TOptional<bool> Expected;

		FPlanScenario(const FFixture& Fixture, FString InName, EHodgeImpactType Type, AActor* InSource = nullptr)
			: Name(MoveTemp(InName)), Request(Fixture.Request(Type)), Source(InSource),
			Origin(Fixture.Pawn->GetActorLocation() - FVector(120.f, -80.f, 0.f))
		{
			Presentation.LocalSourceDirection = FVector::RightVector;
			Presentation.Strength = 1.f;
		}
	};

	/** 返回实际候选，供部分失败输出和续播契约补充独立断言。 */
	struct FPlanComparison
	{
		bool bBuilt = false;
		FHodgeHitReactionState Plan;
		TArray<EHodgeImpactType> Applied;
	};

	/** 对反射字段逐项比较，后续 State 增加字段时也不能被差分漏掉。 */
	void ExpectSamePlan(FAutomationTestBase& Test, const FString& Label,
		const FHodgeHitReactionState& Expected, const FHodgeHitReactionState& Actual)
	{
		for (TFieldIterator<FProperty> It(FHodgeHitReactionState::StaticStruct()); It; ++It)
		{
			Test.TestTrue(Label + TEXT(".") + It->GetName(), It->Identical_InContainer(&Expected, &Actual));
		}
	}

	/** 属性导出只作副作用观察，不代替原生产函数的结果或分支。 */
	FString PlanningObjectSnapshot(const UObject* Object)
	{
		if (!Object) { return TEXT("<null>"); }
		FString Result = Object->GetPathName();
		for (TFieldIterator<FProperty> It(Object->GetClass()); It; ++It)
		{
			FString Value;
			It->ExportText_InContainer(0, Value, Object, nullptr, const_cast<UObject*>(Object), PPF_None);
			Result += TEXT("\n") + It->GetName() + TEXT("=") + Value;
		}
		return Result;
	}

	/** 原入口与新计算器共用同一时刻的场景；失败仍比较未被回滚的部分输出。 */
	FPlanComparison ComparePlan(FAutomationTestBase& Test, FFixture& Fixture, const FPlanScenario& Scenario)
	{
		const auto Context = FHodgeHitReactionTestAccess::PlanningContext(Fixture.Reaction, Scenario.Request,
			Scenario.Presentation, Scenario.Source, Scenario.Origin, Scenario.Level);
		const auto RequestBefore = Context.Request;
		const auto PresentationBefore = Context.Presentation;
		const FString ProfileBefore = PlanningObjectSnapshot(Context.Profile);
		const bool bDirtyBefore = Context.Profile && Context.Profile->GetOutermost()->IsDirty();
		const FString ComponentBefore = PlanningObjectSnapshot(Fixture.Reaction);
		const FString IdentityBefore = FHodgeHitReactionTestAccess::PlanningIdentity(Fixture.Reaction);
		const FTransform TargetBefore = Fixture.Pawn->GetActorTransform();
		const FTransform SourceBefore = Scenario.Source.IsValid() ? Scenario.Source->GetActorTransform() : FTransform::Identity;
		const FVector VelocityBefore = Fixture.Pawn->GetCharacterMovement()->Velocity;
		const EMovementMode ModeBefore = Fixture.Pawn->GetCharacterMovement()->MovementMode;
		const int32 ControlBefore = Fixture.ASC->GetTagCount(HodgeHitReactionTags::Controlled);
		FGameplayTagContainer TagsBefore;
		Fixture.ASC->GetOwnedGameplayTags(TagsBefore);
		const auto* MontageBefore = Fixture.ASC->GetCurrentMontage();
		const float HealthBefore = Fixture.ASC->GetSet<UHodgeHealthSet>()->GetHealth();
		FHodgeHitReactionState OriginalPlan = Scenario.InitialPlan;
		TArray<EHodgeImpactType> OriginalApplied = Scenario.InitialApplied;
		const bool bOriginal = FHodgeHitReactionTestAccess::BuildComponent(Fixture.Reaction, Context,
			Scenario.Source, OriginalPlan, OriginalApplied);
		FPlanComparison Result;
		Result.Plan = Scenario.InitialPlan;
		Result.Applied = Scenario.InitialApplied;
		TArray<FText> Errors{FText::FromString(TEXT("stale-plan-error"))};
		Result.bBuilt = FHodgeHitReactionPlanBuilder::Build(Context, Result.Plan, Result.Applied, Errors);
		Test.TestEqual(Scenario.Name + TEXT(" result matches component snapshot adapter"), Result.bBuilt, bOriginal);
		if (Scenario.Expected.IsSet()) { Test.TestEqual(Scenario.Name + TEXT(" expected boundary"), Result.bBuilt, Scenario.Expected.GetValue()); }
		ExpectSamePlan(Test, Scenario.Name + TEXT(" original/new"), OriginalPlan, Result.Plan);
		Test.TestTrue(Scenario.Name + TEXT(" Applied retains exact order and partial results"), Result.Applied == OriginalApplied);
		Test.TestFalse(Scenario.Name + TEXT(" diagnostics clear stale entries"), Errors.ContainsByPredicate([](const FText& Error)
		{ return Error.ToString() == TEXT("stale-plan-error"); }));
		if (Result.bBuilt) { Test.TestTrue(Scenario.Name + TEXT(" successful build has no diagnostics"), Errors.IsEmpty()); }
		else { Test.TestFalse(Scenario.Name + TEXT(" rejected build provides current diagnostics"), Errors.IsEmpty()); }
		FHodgeHitReactionState RepeatedPlan = Scenario.InitialPlan;
		TArray<EHodgeImpactType> RepeatedApplied = Scenario.InitialApplied;
		TArray<FText> RepeatedErrors;
		const bool bRepeated = FHodgeHitReactionPlanBuilder::Build(Context, RepeatedPlan, RepeatedApplied, RepeatedErrors);
		Test.TestEqual(Scenario.Name + TEXT(" repeated result"), bRepeated, Result.bBuilt);
		ExpectSamePlan(Test, Scenario.Name + TEXT(" repeat"), Result.Plan, RepeatedPlan);
		Test.TestTrue(Scenario.Name + TEXT(" repeated Applied"), RepeatedApplied == Result.Applied);
		Test.TestEqual(Scenario.Name + TEXT(" repeated diagnostic count"), RepeatedErrors.Num(), Errors.Num());
		for (int32 Index = 0; Index < FMath::Min(Errors.Num(), RepeatedErrors.Num()); ++Index)
		{ Test.TestEqual(Scenario.Name + TEXT(" repeated diagnostic"), RepeatedErrors[Index].ToString(), Errors[Index].ToString()); }

		// 计算不能提交状态、清理旧动作、施加受控标签或移动任何参与者。
		Test.TestEqual(Scenario.Name + TEXT(" profile unchanged"), PlanningObjectSnapshot(Context.Profile), ProfileBefore);
		Test.TestEqual(Scenario.Name + TEXT(" profile dirty flag unchanged"), Context.Profile && Context.Profile->GetOutermost()->IsDirty(), bDirtyBefore);
		Test.TestEqual(Scenario.Name + TEXT(" component properties unchanged"), PlanningObjectSnapshot(Fixture.Reaction), ComponentBefore);
		Test.TestEqual(Scenario.Name + TEXT(" component identity unchanged"), FHodgeHitReactionTestAccess::PlanningIdentity(Fixture.Reaction), IdentityBefore);
		ExpectSamePlan(Test, Scenario.Name + TEXT(" component State unchanged"), Context.State, Fixture.Reaction->GetReactionState());
		Test.TestTrue(Scenario.Name + TEXT(" target transform unchanged"), Fixture.Pawn->GetActorTransform().Equals(TargetBefore));
		if (Scenario.Source.IsValid()) { Test.TestTrue(Scenario.Name + TEXT(" source transform unchanged"), Scenario.Source->GetActorTransform().Equals(SourceBefore)); }
		Test.TestTrue(Scenario.Name + TEXT(" velocity unchanged"), Fixture.Pawn->GetCharacterMovement()->Velocity == VelocityBefore);
		Test.TestEqual(Scenario.Name + TEXT(" movement mode unchanged"), EMovementMode(Fixture.Pawn->GetCharacterMovement()->MovementMode), ModeBefore);
		Test.TestEqual(Scenario.Name + TEXT(" control tag unchanged"), Fixture.ASC->GetTagCount(HodgeHitReactionTags::Controlled), ControlBefore);
		FGameplayTagContainer TagsAfter;
		Fixture.ASC->GetOwnedGameplayTags(TagsAfter);
		Test.TestTrue(Scenario.Name + TEXT(" all GAS tags unchanged"), TagsAfter == TagsBefore);
		Test.TestTrue(Scenario.Name + TEXT(" active montage unchanged"), Fixture.ASC->GetCurrentMontage() == MontageBefore);
		Test.TestEqual(Scenario.Name + TEXT(" health unchanged"), Fixture.ASC->GetSet<UHodgeHealthSet>()->GetHealth(), HealthBefore);
		Test.TestEqual(Scenario.Name + TEXT(" same world time"), Fixture.Reaction->GetServerTime(), Context.ServerTime);
		Test.TestTrue(Scenario.Name + TEXT(" request snapshot unchanged"), FHodgeHitReactionConfig::StaticStruct()->CompareScriptStruct(&RequestBefore, &Context.Request, PPF_None));
		Test.TestTrue(Scenario.Name + TEXT(" contact snapshot unchanged"), FHodgeHitPresentation::StaticStruct()->CompareScriptStruct(&PresentationBefore, &Context.Presentation, PPF_None));
		return Result;
	}

	/** 只借用已制作的原地资源，所有自定义配置均位于瞬态测试 Profile。 */
	bool PrepareAuthoredPlanningProfiles(FAutomationTestBase& Test, FFixture& Fixture,
		UHodgeHitReactionProfile*& Legacy, UHodgeHitReactionProfile*& Compact)
	{
		auto* Mesh = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/Main/Character/Hero/Anim/Model/SKM_Pover_LyraLab.SKM_Pover_LyraLab"));
		if (!Test.TestNotNull(TEXT("Plan differential target mesh exists"), Mesh)) { return false; }
		const auto LoadClip = [&Test](const TCHAR* Name)
		{
			const FString ObjectPath = FString(TEXT("/Game/Main/Character/Hero/Anim/HitReactions/Behit/InPlace/")) + Name + TEXT(".") + Name;
			auto* Montage = LoadObject<UAnimMontage>(nullptr, *ObjectPath);
			Test.TestNotNull(ObjectPath, Montage);
			return Montage;
		};
		auto* Left = LoadClip(TEXT("AM_IP_Behit_S_L"));
		auto* Right = LoadClip(TEXT("AM_IP_Behit_S_R"));
		auto* HeavyLeft = LoadClip(TEXT("AM_IP_Behit_B_L"));
		auto* HeavyRight = LoadClip(TEXT("AM_IP_Behit_B_R"));
		auto* FlyStart = LoadClip(TEXT("AM_IP_Behit_Fly_Start"));
		auto* FlyLoop = LoadClip(TEXT("AM_IP_Behit_Fly_Loop"));
		auto* FlyFall = LoadClip(TEXT("AM_IP_Behit_Fly_Fall"));
		auto* PushStart = LoadClip(TEXT("AM_IP_Behit_Push_Start"));
		auto* PushLoop = LoadClip(TEXT("AM_IP_Behit_Push_Loop"));
		auto* PushFall = LoadClip(TEXT("AM_IP_Behit_Push_Fall"));
		auto* GetUp = LoadClip(TEXT("AM_IP_StandUp"));
		if (!Left || !Right || !HeavyLeft || !HeavyRight || !FlyStart || !FlyLoop || !FlyFall || !PushStart || !PushLoop || !PushFall || !GetUp) { return false; }
		Fixture.Pawn->GetMesh()->SetSkeletalMesh(Mesh);
		Fixture.Pawn->GetMesh()->SetAnimInstanceClass(UAnimInstance::StaticClass());
		Fixture.ASC->RefreshAbilityActorInfo();
		if (!Test.TestNotNull(TEXT("Plan differential has a real ability animation dependency"), Fixture.ASC->AbilityActorInfo->GetAnimInstance())) { return false; }
		Legacy = Fixture.Profile;
		for (auto& Entry : Legacy->Animations)
		{
			Entry.bAllowWithoutMontage = false;
			Entry.Montage = Right;
		}
		Legacy->Animations[1].Montage = HeavyRight;
		Legacy->Animations[1].DefaultDuration = .65f;
		Legacy->Animations[2].Montage = FlyStart;
		Legacy->Animations[2].AirLoopMontage = FlyLoop;
		Legacy->Animations[2].LandingMontage = FlyFall;
		Legacy->Animations[2].AirLoopTransitionTime = .18f;
		Legacy->Animations[3].Montage = FlyFall;
		Legacy->Animations[3].GetUpMontage = GetUp;
		for (int32 Index : {4, 5})
		{
			Legacy->Animations[Index].Montage = PushStart;
			Legacy->Animations[Index].AirLoopMontage = PushLoop;
			Legacy->Animations[Index].LandingMontage = PushFall;
			Legacy->Animations[Index].AirLoopTransitionTime = .18f;
		}
		FHodgeHitReactionVariant Variant;
		Variant.Direction = EHodgeHitPresentationDirection::Left;
		Variant.Montage = Left;
		Legacy->Animations[0].Variants.Add(Variant);
		Variant.Direction = EHodgeHitPresentationDirection::Right;
		Variant.Montage = Right;
		Legacy->Animations[0].Variants.Add(Variant);
		Variant.MinimumStrength = 1.5f;
		Variant.Montage = HeavyRight;
		Legacy->Animations[0].Variants.Add(Variant);
		Compact = NewObject<UHodgeHitReactionProfile>(Fixture.Pawn);
		Compact->bUseCompactAnimations = true;
		Compact->Compact.Light.Left = Left;
		Compact->Compact.Light.Right = Right;
		Compact->Compact.Heavy.Left = HeavyLeft;
		Compact->Compact.Heavy.Right = HeavyRight;
		Compact->Compact.Heavy.ControlDuration = .65f;
		Compact->Compact.Fly.Start = FlyStart;
		Compact->Compact.Fly.Loop = FlyLoop;
		Compact->Compact.Fly.Landing = FlyFall;
		Compact->Compact.Push.Start = PushStart;
		Compact->Compact.Push.Loop = PushLoop;
		Compact->Compact.Push.Landing = PushFall;
		Compact->Compact.Downed = FlyFall;
		Compact->Compact.GetUp = GetUp;
		return true;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgePlanBuilderDifferentialTest, "Hodge.HitReaction.PlanBuilder.DifferentialMatrix",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 首次切换前对照原组件，之后继续保护双布局、来源快照、真实空中与排队起飞接线。 */
bool FHodgePlanBuilderDifferentialTest::RunTest(const FString&)
{
	using namespace HodgeReactionTests;
	FFixture F;
	UHodgeHitReactionProfile* Legacy = nullptr;
	UHodgeHitReactionProfile* Compact = nullptr;
	if (!PrepareAuthoredPlanningProfiles(*this, F, Legacy, Compact)) { return false; }
	F.Pawn->SetActorLocation(FVector(340.f, -110.f, 120.f));
	F.Pawn->SetActorRotation(FRotator(0.f, 37.f, 0.f));
	auto* Source = F.World->SpawnActor<ACharacter>();
	Source->SetActorLocation(FVector(-170.f, 80.f, 220.f));
	Source->SetActorRotation(FRotator(0.f, 127.f, 0.f));
	int32 Cases = 0, Successes = 0, Failures = 0;
	for (auto* Profile : {Legacy, Compact})
	{
		for (int32 AirMode = 0; AirMode != 3; ++AirMode)
		{
			F.Pawn->GetCharacterMovement()->SetMovementMode(AirMode == 1 ? MOVE_Falling : MOVE_Walking);
			FHodgeHitReactionState State;
			if (AirMode != 0)
			{
				State.Phase = EHodgeHitReactionPhase::Airborne;
				State.Type = EHodgeImpactType::Launch;
				State.ChainStartedAt = -.4f;
				State.AirStartedAt = -.2f;
				State.AirOriginZ = 91.f;
				State.AirHitCount = 1;
				State.bKnockdownOnLanding = true;
			}
			FHodgeHitReactionTestAccess::SetPlanningInputs(F.Reaction, Profile, State, AirMode == 1, 4 + AirMode);
			for (int32 Index = 0; Index != 6; ++Index)
			{
				FPlanScenario Scenario(F, FString::Printf(TEXT("%s/Air%d/Impact%d"), Profile == Legacy ? TEXT("Legacy") : TEXT("Compact"), AirMode, Index), EHodgeImpactType(Index),
					(Index + AirMode) % 2 == 0 ? Source : nullptr);
				Scenario.Request.Impacts[0].Direction = EHodgeImpactDirection((Index + AirMode) % 3);
				Scenario.Request.Impacts[0].WorldDirection = FVector(-2.f, 3.f, 4.f);
				Scenario.Presentation.Strength = Index % 2 == 0 ? .1f : 2.f;
				Scenario.Presentation.LocalSourceDirection = Index % 2 == 0 ? FVector::ForwardVector : -FVector::RightVector;
				Scenario.Level = Index % 3 == 0 ? EHodgeReactionLevel::Heavy : EHodgeReactionLevel::None;
				const auto Result = ComparePlan(*this, F, Scenario);
				++Cases;
				Result.bBuilt ? ++Successes : ++Failures;
			}
		}
	}
	F.Pawn->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	FHodgeHitReactionTestAccess::SetPlanningInputs(F.Reaction, Legacy, {}, false);
	// 来源无效、来源朝向退化、世界方向退化与同点原点必须保留各自既有回退。
	for (int32 Index = 0; Index != 7; ++Index)
	{
		Source->SetActorRotation(Index == 2 ? FRotator(90.f, 0.f, 0.f) : FRotator(0.f, 127.f, 0.f));
		FPlanScenario Scenario(F, FString::Printf(TEXT("DirectionSnapshot%d"), Index), EHodgeImpactType::Knockback, Index == 1 ? nullptr : Source);
		Scenario.Request.Impacts[0].Direction = Index < 3 ? EHodgeImpactDirection::SourceForward :
			Index < 6 ? EHodgeImpactDirection::WorldDirection : EHodgeImpactDirection::AwayFromSource;
		Scenario.Request.Impacts[0].WorldDirection = Index == 3 ? FVector(-2.f, 3.f, 7.f) :
			Index == 4 ? FVector::ZeroVector : FVector(0.f, 0.f, 37.f);
		if (Index == 6) { Scenario.Origin = F.Pawn->GetActorLocation(); }
		Scenario.Expected = true;
		ComparePlan(*this, F, Scenario);
		++Cases;
	}

	// 借用正式三份 Profile，不改其内容；Guardian 缺失家族仍按实际配置拒绝或降级。
	const TCHAR* Profiles[] = {TEXT("/Game/Main/Character/Hero/Anim/HitReactions/DA_Hero_HitReaction.DA_Hero_HitReaction"),
		TEXT("/Game/CodexText/HitReactionValidation/Dummy/DA_DummyHitReaction.DA_DummyHitReaction"),
		TEXT("/Game/Main/Character/Enemies/Guardian/DA_Guardian_HitReaction.DA_Guardian_HitReaction")};
	for (int32 ProfileIndex = 0; ProfileIndex != 3; ++ProfileIndex)
	{
		auto* Profile = LoadObject<UHodgeHitReactionProfile>(nullptr, Profiles[ProfileIndex]);
		if (!TestNotNull(Profiles[ProfileIndex], Profile)) { return false; }
		if (ProfileIndex == 2)
		{
			auto* GuardianMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/Main/Character/Enemies/Guardian/Model/SKM_Guardian.SKM_Guardian"));
			if (!TestNotNull(TEXT("Guardian keeps its own target mesh"), GuardianMesh)) { return false; }
			F.Pawn->GetMesh()->SetSkeletalMesh(GuardianMesh);
			F.ASC->RefreshAbilityActorInfo();
		}
		FHodgeHitReactionTestAccess::SetPlanningInputs(F.Reaction, Profile, {}, false);
		for (int32 Index = 0; Index != 6; ++Index)
		{
			F.Pawn->GetCharacterMovement()->SetMovementMode(Index >= 4 ? MOVE_Falling : MOVE_Walking);
			FPlanScenario Scenario(F, FString::Printf(TEXT("AuthoredProfile%d/Impact%d"), ProfileIndex, Index), EHodgeImpactType(Index), Source);
			ComparePlan(*this, F, Scenario);
			++Cases;
		}
	}
	TestTrue(TEXT("Differential matrix includes genuinely accepted plans"), Successes > 0);
	TestTrue(TEXT("Differential matrix includes genuinely rejected plans"), Failures > 0);
	AddInfo(FString::Printf(TEXT("PlanBuilder differential scenarios: %d; every scenario also repeats pure calculation and checks side effects."), Cases));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgePlanBuilderGuardTest, "Hodge.HitReaction.PlanBuilder.GuardsAndPartialOutputs",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 验证失败的部分写入、未消费配置、预算与续播，不以成功返回代替纯计算契约。 */
bool FHodgePlanBuilderGuardTest::RunTest(const FString&)
{
	using namespace HodgeReactionTests;
	FFixture F;
	int32 Cases = 0;
	const auto Check = [this, &F, &Cases](const FPlanScenario& Scenario)
	{
		++Cases;
		return ComparePlan(*this, F, Scenario);
	};
	FPlanScenario NoAnimation(F, TEXT("AnimationFreeSupported"), EHodgeImpactType::HitStun);
	NoAnimation.Expected = true;
	Check(NoAnimation);
	FPlanScenario Partial(F, TEXT("CurveFailureRetainsPartialOutputs"), EHodgeImpactType::Knockback);
	Partial.InitialPlan.Sequence = 73;
	Partial.InitialPlan.StartedAt = -12.f;
	Partial.InitialApplied = {EHodgeImpactType::HitStun};
	Partial.Expected = false;
	F.Profile->KnockbackStrengthCurve = NewObject<UCurveFloat>(F.Profile);
	const auto PartialResult = Check(Partial);
	TestEqual(TEXT("False does not reset caller-owned Sequence"), PartialResult.Plan.Sequence, 73);
	TestEqual(TEXT("Early movement failure retains untouched timestamp"), PartialResult.Plan.StartedAt, -12.f);
	TestTrue(TEXT("False retains movement flag written before curve rejection"), PartialResult.Plan.bHasMovement);
	TestTrue(TEXT("False retains both prior and newly applied impact entries"), PartialResult.Applied == TArray<EHodgeImpactType>{EHodgeImpactType::HitStun, EHodgeImpactType::Knockback});
	Check(NoAnimation);
	F.Profile->KnockbackStrengthCurve = nullptr;
	{
		TGuardValue<float> InvalidControl(F.Profile->MaxControlDuration, 0.f);
		Partial.Name = TEXT("InvalidSafetyLeavesCallerOutputs");
		const auto Early = Check(Partial);
		ExpectSamePlan(*this, TEXT("Early failure preserves initial plan"), Partial.InitialPlan, Early.Plan);
		TestTrue(TEXT("Early failure preserves initial Applied"), Early.Applied == Partial.InitialApplied);
	}
	{
		TGuardValue<float> BadAirLimit(F.Profile->MaxAirborneDuration, 0.f);
		Check(NoAnimation);
		FPlanScenario AirLimit(F, TEXT("SelectedBadAirLimit"), EHodgeImpactType::Launch);
		AirLimit.Expected = false;
		Check(AirLimit);
	}
	{
		TGuardValue<float> BadSpeed(F.Profile->MaxLaunchSpeed, 0.f);
		Check(NoAnimation);
		FPlanScenario MotionLimit(F, TEXT("SelectedBadMotionLimit"), EHodgeImpactType::Knockback);
		MotionLimit.Expected = false;
		Check(MotionLimit);
	}
	F.Pawn->GetCharacterMovement()->Deactivate();
	FPlanScenario InactiveMovement(F, TEXT("InactiveMovementNoFallback"), EHodgeImpactType::Knockback);
	InactiveMovement.Expected = false;
	Check(InactiveMovement);
	F.Profile->FallbackToHitStun.Add(EHodgeImpactType::Knockback);
	InactiveMovement.Name = TEXT("InactiveMovementUsesExistingFallback");
	InactiveMovement.Expected = true;
	Check(InactiveMovement);
	F.Profile->FallbackToHitStun.Reset();
	F.Pawn->GetCharacterMovement()->Activate();
	FHodgeHitReactionState Budget;
	Budget.Phase = EHodgeHitReactionPhase::Controlled;
	Budget.ChainStartedAt = F.Reaction->GetServerTime() - F.Profile->MaxControlDuration + .05f;
	FHodgeHitReactionTestAccess::SetPlanningInputs(F.Reaction, F.Profile, Budget, false);
	const auto NearBudget = Check(NoAnimation);
	TestTrue(TEXT("Near-expired chain retains only its existing remaining time"), FMath::IsNearlyEqual(NearBudget.Plan.EndsAt - F.Reaction->GetServerTime(), .05f, .001f));
	Budget.ChainStartedAt -= 1.f;
	FHodgeHitReactionTestAccess::SetPlanningInputs(F.Reaction, F.Profile, Budget, false);
	FPlanScenario Expired(F, TEXT("ExpiredChainRejectsWithoutRollback"), EHodgeImpactType::HitStun);
	Expired.Expected = false;
	const auto ExpiredResult = Check(Expired);
	TestTrue(TEXT("Budget failure retains the selected Applied entry"), ExpiredResult.Applied.Contains(EHodgeImpactType::HitStun));
	Budget = {};
	Budget.Phase = EHodgeHitReactionPhase::Airborne;
	Budget.AirHitCount = F.Profile->MaxAirHits;
	F.Pawn->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	FHodgeHitReactionTestAccess::SetPlanningInputs(F.Reaction, F.Profile, Budget, true);
	FPlanScenario Exhausted(F, TEXT("ExhaustedAirCount"), EHodgeImpactType::AirHit);
	Exhausted.Expected = false;
	Check(Exhausted);
	F.Pawn->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	FHodgeHitReactionTestAccess::SetPlanningInputs(F.Reaction, F.Profile, {}, false);
	FPlanScenario NoQualification(F, TEXT("LegacyLaunchCannotBypassSegmentFlag"), EHodgeImpactType::Launch);
	NoQualification.Request.bCanLaunch = false;
	NoQualification.Expected = false;
	Check(NoQualification);

	UHodgeHitReactionProfile* Legacy = nullptr;
	UHodgeHitReactionProfile* Compact = nullptr;
	if (!PrepareAuthoredPlanningProfiles(*this, F, Legacy, Compact)) { return false; }
	FHodgeHitReactionTestAccess::SetPlanningInputs(F.Reaction, Legacy, {}, false);
	F.Pawn->GetMesh()->SetAnimInstanceClass(nullptr);
	F.ASC->RefreshAbilityActorInfo();
	FPlanScenario MissingInstance(F, TEXT("FinalMainClipRequiresAnimInstance"), EHodgeImpactType::HitStun);
	MissingInstance.Expected = false;
	const auto MissingResult = Check(MissingInstance);
	TestNotNull(TEXT("Dependency failure retains selected main clip"), MissingResult.Plan.Montage.Get());
	F.Pawn->GetMesh()->SetAnimInstanceClass(UAnimInstance::StaticClass());
	F.ASC->RefreshAbilityActorInfo();
	// 旧动画布局也必须覆盖明确的显式输入枚举，不能只依赖 Legacy 默认值。
	FPlanScenario ExplicitLegacy(F, TEXT("LegacyLayoutWithExplicitImpactMode"), EHodgeImpactType::HitStun);
	ExplicitLegacy.Request.InputMode = EHodgeReactionInputMode::ExplicitImpact;
	ExplicitLegacy.Expected = true;
	Check(ExplicitLegacy);
	FPlanScenario Threshold(F, TEXT("BelowStrengthThreshold"), EHodgeImpactType::HitStun);
	Threshold.Presentation.Strength = 1.49f;
	Threshold.Expected = true;
	const auto Below = Check(Threshold);
	Threshold.Name = TEXT("AtStrengthThreshold");
	Threshold.Presentation.Strength = 1.5f;
	const auto At = Check(Threshold);
	TestTrue(TEXT("Threshold scenarios actually select different authored resources"), Below.Plan.Montage != At.Plan.Montage);
	// 续播保持旧主片；新选中的歧义变体只有真正重播时才参与校验。
	FHodgeHitReactionState Continuing = Below.Plan;
	Continuing.StartedAt = -2.f;
	Continuing.RecoveryStartedAt = -.2f;
	Continuing.PresentationStrength = 2.f;
	FHodgeHitReactionTestAccess::SetPlanningInputs(F.Reaction, Legacy, Continuing, false);
	Threshold.Name = TEXT("ContinuationRetainsPriorMainClip");
	const auto Continued = Check(Threshold);
	TestEqual(TEXT("Continuation keeps old main montage"), Continued.Plan.Montage.Get(), Continuing.Montage.Get());
	TestEqual(TEXT("Continuation keeps old presentation start"), Continued.Plan.StartedAt, Continuing.StartedAt);
	TestEqual(TEXT("Continuation keeps stronger existing contact strength"), Continued.Plan.PresentationStrength, 2.f);
	Legacy->GroundReplayInterval = .1f;
	Threshold.Name = TEXT("ReplayWindowAllowsStrongerNewClip");
	Threshold.Presentation.Strength = 2.f;
	const auto Replayed = Check(Threshold);
	TestTrue(TEXT("Replay after interval uses a new main clip"), Replayed.Plan.Montage != Continuing.Montage);
	FHodgeHitReactionTestAccess::SetPlanningInputs(F.Reaction, Compact, {}, false);
	FPlanScenario MissingFamily(F, TEXT("CompactActualMissingFlyDependency"), EHodgeImpactType::Launch);
	MissingFamily.Expected = false;
	Compact->Compact.Fly.Landing = nullptr;
	Check(MissingFamily);
	FHodgeHitReactionTestAccess::SetPlanningInputs(F.Reaction, nullptr, {}, false);
	FPlanScenario MissingProfile(F, TEXT("MissingProfilePreservesInitialOutputs"), EHodgeImpactType::HitStun);
	MissingProfile.Expected = false;
	MissingProfile.InitialPlan.Sequence = 92;
	const auto MissingProfileResult = Check(MissingProfile);
	ExpectSamePlan(*this, TEXT("Missing profile keeps initial plan"), MissingProfile.InitialPlan, MissingProfileResult.Plan);
	AddInfo(FString::Printf(TEXT("PlanBuilder guard scenarios: %d; all false returns compare full partial outputs."), Cases));
	return true;
}
#endif
