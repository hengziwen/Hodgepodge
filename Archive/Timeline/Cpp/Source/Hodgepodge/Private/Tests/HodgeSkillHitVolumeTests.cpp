#if WITH_DEV_AUTOMATION_TESTS
#include "Combat/HodgeHitDetection.h"
#include "Component/HodgeCombatComponentBase.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h"
#include "AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h"
#include "Data/HodgeAbilityDefinition.h"
#include "Data/HodgeAbilityTimeline.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Animation/AnimMontage.h"
#include "Engine/World.h"
#include "Misc/AutomationTest.h"

struct FHodgeSkillTimelineTestAccess
{
	static UHodgeAbilityTask_PlayTimeline* Create(UAbilitySystemComponent* ASC, UHodgeAbilityTimeline* Timeline)
	{
		auto* Task = NewObject<UHodgeAbilityTask_PlayTimeline>(ASC);
		Task->InitTask(*ASC, FGameplayTasks::DefaultPriority);
		Task->SetAbilitySystemComponent(ASC);
		Task->TimelineAsset = Timeline;
		return Task;
	}
	static void Advance(UHodgeAbilityTask_PlayTimeline* Task, float Delta)
	{
		Task->LastUpdateWorldTime -= Delta;
		Task->TickTask(Delta);
	}
};

namespace
{
	struct FVolumeWorld
	{
		UWorld* World;
		AActor* Owner;
		AActor* Target;
		UHodgeCombatComponentBase* Combat;
		FHodgeHitDetectionRequest Request;
		FVolumeWorld()
		{
			const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false);
			World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
			Owner = World->SpawnActor<AActor>();
			Owner->SetRootComponent(NewObject<USceneComponent>(Owner));
			Owner->GetRootComponent()->RegisterComponent();
			Combat = NewObject<UHodgeCombatComponentBase>(Owner);
			Combat->RegisterComponent();
			Target = World->SpawnActor<AActor>();
			auto* Shape = NewObject<USphereComponent>(Target);
			Target->SetRootComponent(Shape);
			Shape->SetSphereRadius(20.f);
			Shape->SetCollisionObjectType(ECC_Pawn);
			Shape->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			Shape->SetCollisionResponseToAllChannels(ECR_Block);
			Shape->RegisterComponent();
			Target->SetActorLocation(FVector(100.f, 0.f, 0.f));
			Request.Profile = NewObject<UHodgeHitDetectionProfile>(Combat);
			Request.Profile->Strategy = UHodgeShapeQueryStrategy::StaticClass();
			Request.Profile->bRequireLineOfSight = false;
			Request.Profile->QueryMode = EHodgeHitQueryMode::Overlap;
			Request.Volume.GeometryMode = EHodgeHitGeometryMode::ConfiguredShape;
			Request.Volume.LocalTransform.SetTranslation(FVector(100.f, 0.f, 0.f));
			Request.Volume.SphereRadius = 40.f;
		}
		~FVolumeWorld() { World->DestroyWorld(false); }
		bool Hits(uint64 Handle, const FGuid& Execution)
		{
			FHodgeHitDetectionBatch Batch;
			return Combat->SampleDetection(Handle, Execution, Batch) && Batch.Hits.ContainsByPredicate([this](const auto& Hit) { return Hit.GetActor() == Target; });
		}
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeConfiguredVolumesTest, "Hodge.Combat.ConfiguredVolumes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeConfiguredVolumesTest::RunTest(const FString&)
{
	FVolumeWorld F;
	const FGuid Execution = FGuid::NewGuid();
	for (int32 Index = 0; Index < 3; ++Index)
	{
		F.Request.Volume.Shape = static_cast<EHodgeHitShape>(Index);
		F.Request.Volume.BoxHalfExtent = FVector(40.f);
		F.Request.Volume.CapsuleRadius = 40.f;
		F.Request.Volume.CapsuleHalfHeight = 80.f;
		const uint64 Handle = F.Combat->CreateDetectionSession(Execution, Index, F.Request);
		TestTrue(TEXT("Each configured shape queries without a source component or SourceTag"), Handle && F.Hits(Handle, Execution));
	}
	F.Request.Volume.TransformPolicy = EHodgeHitTransformPolicy::SnapshotOnEventEnter;
	const uint64 Fixed = F.Combat->CreateDetectionSession(Execution, 3, F.Request);
	F.Request.Volume.TransformPolicy = EHodgeHitTransformPolicy::Follow;
	const uint64 Follow = F.Combat->CreateDetectionSession(Execution, 4, F.Request);
	F.Owner->SetActorLocation(FVector(500.f, 0.f, 0.f));
	TestTrue(TEXT("Snapshot stays at its original world position"), F.Hits(Fixed, Execution));
	TestFalse(TEXT("Follow moves with the caster"), F.Hits(Follow, Execution));
	F.Request.Volume.AnchorKind = EHodgeHitAnchorKind::ExecutionTransform;
	F.Request.Volume.AnchorKey = TEXT("Center");
	F.Request.Volume.LocalTransform = FTransform::Identity;
	AddExpectedError(TEXT("Cannot resolve unique detection source"), EAutomationExpectedErrorFlags::Contains, 1);
	TestEqual(TEXT("Missing runtime anchor fails rather than hitting world zero"), F.Combat->CreateDetectionSession(Execution, 5, F.Request), uint64(0));
	F.Request.bHasRuntimeAnchor = true;
	F.Request.RuntimeAnchor = FTransform(FVector(100.f, 0.f, 0.f));
	const uint64 WorldAnchor = F.Combat->CreateDetectionSession(Execution, 6, F.Request);
	TestTrue(TEXT("World anchor is independent of caster transform"), F.Hits(WorldAnchor, Execution));
	F.Combat->UpdateDetectionAnchor(Execution, TEXT("Center"), FTransform(FVector(700.f, 0.f, 0.f)));
	TestFalse(TEXT("Follow runtime anchor updates only its own live sessions"), F.Hits(WorldAnchor, Execution));
	F.Combat->EndDetectionSessionsForExecution(Execution);
	TestFalse(TEXT("End clears configured sessions"), F.Combat->IsDetectionSessionValid(Fixed, Execution));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeVolumeFilteringTest, "Hodge.Combat.VolumeFilteringAndMotion",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeVolumeFilteringTest::RunTest(const FString&)
{
	FVolumeWorld F;
	const auto Execution = FGuid::NewGuid();
	F.Target->SetActorLocation(FVector(1000.f, 0.f, 0.f));
	F.Request.Volume.LocalTransform.SetTranslation(FVector(950.f, 0.f, 0.f));
	F.Request.Volume.SphereRadius = 100.f;
	F.Request.Profile->bRequireLineOfSight = true;
	AActor* Wall = F.World->SpawnActor<AActor>();
	auto* Box = NewObject<UBoxComponent>(Wall);
	Wall->SetRootComponent(Box);
	Box->SetBoxExtent(FVector(20.f, 100.f, 100.f));
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionObjectType(ECC_WorldStatic);
	Box->SetCollisionResponseToAllChannels(ECR_Block);
	Box->RegisterComponent();
	Wall->SetActorLocation(FVector(500.f, 0.f, 0.f));
	const auto Caster = F.Combat->CreateDetectionSession(Execution, 0, F.Request);
	TestFalse(TEXT("Caster LOS sees the intervening wall"), F.Hits(Caster, Execution));
	F.Request.Profile->FilterFrame = EHodgeHitFilterFrame::DetectionAnchor;
	TestTrue(TEXT("Remote anchor LOS starts at the attack center"), F.Hits(Caster, Execution));
	F.Request.Profile->bRequireLineOfSight = false;
	F.Request.Profile->QueryMode = EHodgeHitQueryMode::Sweep;
	F.Request.Volume.LocalTransform = FTransform::Identity;
	F.Request.Volume.SphereRadius = 40.f;
	F.Target->SetActorLocation(FVector(500.f, 0.f, 0.f));
	const auto Dash = F.Combat->CreateDetectionSession(Execution, 1, F.Request);
	F.Owner->SetActorLocation(FVector(1000.f, 0.f, 0.f));
	TestFalse(TEXT("Default teleport guard does not hit the intervening target"), F.Hits(Dash, Execution));
	F.Owner->SetActorLocation(FVector::ZeroVector);
	F.Combat->ResetDetectionHistory(Dash, Execution);
	F.Request.Profile->bContinuousMotion = true;
	F.Owner->SetActorLocation(FVector(1000.f, 0.f, 0.f));
	TestTrue(TEXT("Explicit continuous dash covers its movement path"), F.Hits(Dash, Execution));
	F.Request.TargetPolicy = EHodgeHitTargetPolicy::ConfirmedTarget;
	F.Request.RuntimeTarget = F.Target;
	F.Request.MaxTargetDistance = 100.f;
	AddExpectedError(TEXT("Cannot resolve unique detection source"), EAutomationExpectedErrorFlags::Contains, 1);
	TestEqual(TEXT("Confirmed target rejects excessive range"), F.Combat->CreateDetectionSession(Execution, 2, F.Request), uint64(0));
	F.Request.MaxTargetDistance = 1000.f;
	const auto Confirmed = F.Combat->CreateDetectionSession(Execution, 3, F.Request);
	TestTrue(TEXT("Confirmed target uses only its selected target"), F.Hits(Confirmed, Execution));
	F.Target->Destroy();
	FHodgeHitDetectionBatch Batch;
	TestFalse(TEXT("Destroyed target invalidates the occurrence"), F.Combat->SampleDetection(Confirmed, Execution, Batch));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeStandaloneRouteTest, "Hodge.Combat.StandaloneActivationContract",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeStandaloneRouteTest::RunTest(const FString&)
{
	FVolumeWorld F;
	auto* ASC = NewObject<UHodgeAbilitySystemComponent>(F.Owner);
	ASC->RegisterComponent();
	ASC->InitAbilityActorInfo(F.Owner, F.Owner);
	auto* D = NewObject<UHodgeAbilityDefinition>();
	D->AbilityTag = HodgeGameplayTags::Status_Attack;
	D->AbilityClass = UHodgeGameplayAbility_Melee::StaticClass();
	D->ExecutionConfig.Montage = LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Main/Character/Hero/Anim/Montages/AM_Attack01_Montage.AM_Attack01_Montage"));
	D->ExecutionConfig.TimelineTaskConfig.Timeline = NewObject<UHodgeAbilityTimeline>();
	D->ExecutionConfig.TimelineTaskConfig.Timeline->bUseMontageDuration = true;
	const auto Combo = ASC->GiveAbilityDefinition(D, 1, nullptr);
	auto* CDO = GetDefault<UHodgeGameplayAbility_Melee>();
	TestFalse(TEXT("Existing combo still requires coordinator authorization"), CDO->CanActivateAbility(Combo, ASC->AbilityActorInfo.Get()));
	D->ExecutionRoute = EHodgeAbilityExecutionRoute::Standalone;
	D->InputTag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Jump"));
	const auto Standalone = ASC->GiveAbilityDefinition(D, 1, nullptr);
	TestTrue(TEXT("Standalone uses normal GAS activation without a combo graph"), CDO->CanActivateAbility(Standalone, ASC->AbilityActorInfo.Get()));
	const auto* Spec = ASC->FindAbilitySpecFromHandle(Standalone);
	TestTrue(TEXT("Standalone input is bound to its granted spec"), Spec && Spec->GetDynamicSpecSourceTags().HasTagExact(D->InputTag));
	ASC->SetLooseGameplayTagCount(HodgeGameplayTags::Status_Death_Dying, 1);
	TestFalse(TEXT("Standalone cannot activate during death"), CDO->CanActivateAbility(Standalone, ASC->AbilityActorInfo.Get()));
	ASC->SetLooseGameplayTagCount(HodgeGameplayTags::Status_Death_Dying, 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeIndexedPointsTest, "Hodge.Combat.IndexedPointsAndCancellation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeIndexedPointsTest::RunTest(const FString&)
{
	FVolumeWorld F;
	auto* ASC = NewObject<UHodgeAbilitySystemComponent>(F.Owner);
	ASC->RegisterComponent();
	ASC->InitAbilityActorInfo(F.Owner, F.Owner);
	auto* Timeline = NewObject<UHodgeAbilityTimeline>();
	Timeline->Duration = 1.f;
	for (int32 Index = 0; Index < 3; ++Index)
	{
		FHodgeTimelineEvent Point;
		Point.Kind = EHodgeTimelineEventKind::Point;
		Point.EventID = FName(*FString::Printf(TEXT("Pulse%d"), Index));
		Point.StartTime = .1f + .1f * Index;
		Point.PointEventTag = HodgeGameplayTags::GameplayEvent_Attack_Test;
		Timeline->Events.Add(Point);
	}
	auto* Task = FHodgeSkillTimelineTestAccess::Create(ASC, Timeline);
	TArray<int32> Indices;
	Task->OnIndexedPoint.AddLambda([&Indices](int32 Index, FGameplayTag) { Indices.Add(Index); });
	Task->ReadyForActivation();
	FHodgeSkillTimelineTestAccess::Advance(Task, .5f);
	TestTrue(TEXT("One low-rate update preserves three distinct occurrences of the same tag"), Indices == TArray<int32>({0, 1, 2}));
	FHodgeSkillTimelineTestAccess::Advance(Task, .1f);
	TestEqual(TEXT("Consumed points never repeat on the next frame"), Indices.Num(), 3);
	Task->StopTimeline(EHodgeTimelineStopReason::AbilityCancelled);
	Task = FHodgeSkillTimelineTestAccess::Create(ASC, Timeline);
	Indices.Reset();
	Task->OnIndexedPoint.AddLambda([&Indices, Task](int32 Index, FGameplayTag)
	{
		Indices.Add(Index);
		Task->StopTimeline(EHodgeTimelineStopReason::AbilityCancelled);
	});
	Task->ReadyForActivation();
	FHodgeSkillTimelineTestAccess::Advance(Task, .5f);
	TestEqual(TEXT("Cancellation in the first hit discards later hits in the same frame"), Indices.Num(), 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgePointBindingValidationTest, "Hodge.Combat.PointBindingValidation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgePointBindingValidationTest::RunTest(const FString&)
{
	FVolumeWorld F;
	auto* D = NewObject<UHodgeAbilityDefinition>();
	D->AbilityTag = HodgeGameplayTags::Status_Attack;
	D->AbilityClass = UHodgeGameplayAbility_Melee::StaticClass();
	D->ExecutionRoute = EHodgeAbilityExecutionRoute::Standalone;
	D->ExecutionConfig.Montage = LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Main/Character/Hero/Anim/Montages/AM_Attack01_Montage.AM_Attack01_Montage"));
	auto* Timeline = NewObject<UHodgeAbilityTimeline>();
	Timeline->bUseMontageDuration = true;
	D->ExecutionConfig.TimelineTaskConfig.Timeline = Timeline;
	FHodgeTimelineEvent Event;
	Event.Kind = EHodgeTimelineEventKind::Point;
	Event.EventID = TEXT("Pulse");
	Event.StartTime = .1f;
	Event.PointEventTag = HodgeGameplayTags::GameplayEvent_Attack_Test;
	Timeline->Events.Add(Event);
	FHodgeHitPointBinding Binding;
	Binding.PointEventTag = Event.PointEventTag;
	Binding.Profile = F.Request.Profile;
	Binding.Volume = F.Request.Volume;
	Binding.RequiresWeaponInHand = false;
	Binding.DamageEffect = LoadClass<UGameplayEffect>(nullptr, TEXT("/Game/GameplayEffects/Damage/GE_Damage_Basic_SetByCaller.GE_Damage_Basic_SetByCaller_C"));
	D->HitPoints.Add(Binding);
	TArray<FText> Errors;
	TestTrue(TEXT("Configured point requires no registered character Box"), D->ValidateDefinition(Errors));
	Timeline->Events[0].NetPolicy = EHodgeTimelineEventNetPolicy::LocallyControlledOnly;
	Errors.Reset();
	TestFalse(TEXT("Client-only damage points are rejected"), D->ValidateDefinition(Errors));
	Timeline->Events[0].NetPolicy = EHodgeTimelineEventNetPolicy::AuthorityOnly;
	D->HitPoints[0].Volume.Shape = EHodgeHitShape::Capsule;
	D->HitPoints[0].Volume.CapsuleHalfHeight = 1.f;
	Errors.Reset();
	TestFalse(TEXT("Invalid capsule dimensions fail content validation"), D->ValidateDefinition(Errors));
	D->HitPoints[0].Volume.CapsuleHalfHeight = 100.f;
	D->HitPoints.Add(Binding);
	Errors.Reset();
	TestFalse(TEXT("Duplicate point bindings are rejected"), D->ValidateDefinition(Errors));
	return true;
}
#endif
