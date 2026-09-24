#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "Data/HodgeAbilityTimeline.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "Misc/AutomationTest.h"
#include <limits>

// 不增加反射测试类；直接驱动真实 Task 的启动、时钟和清理路径。
struct FHodgeTimelineTestAccess
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
	struct FTimelineWorld
	{
		UWorld* World = nullptr;
		UAbilitySystemComponent* ASC = nullptr;

		FTimelineWorld()
		{
			const UWorld::InitializationValues Values = UWorld::InitializationValues()
				.AllowAudioPlayback(false).CreatePhysicsScene(false).CreateNavigation(false).CreateAISystem(false);
			World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
			AActor* Owner = World->SpawnActor<AActor>();
			ASC = NewObject<UAbilitySystemComponent>(Owner);
			ASC->RegisterComponent();
			ASC->InitAbilityActorInfo(Owner, Owner);
		}

		~FTimelineWorld()
		{
			World->DestroyWorld(false);
		}
	};

	FHodgeTimelineEvent Window(FName ID, FGameplayTag Tag, float Start, float End)
	{
		FHodgeTimelineEvent Event;
		Event.EventID = ID;
		Event.WindowTag = Tag;
		Event.StartTime = Start;
		Event.EndTime = End;
		Event.WindowEffectClass = UGameplayEffect::StaticClass();
		return Event;
	}

	bool Valid(const UHodgeAbilityTimeline* Timeline)
	{
		TArray<FText> Errors;
		return Timeline->ValidateForPlayback(Errors);
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeTimelineValidationTest, "Hodge.Timeline.Validation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FHodgeTimelineValidationTest::RunTest(const FString& Parameters)
{
	UGameplayEffect* Effect = GetMutableDefault<UGameplayEffect>();
	TGuardValue<EGameplayEffectDurationType> DurationGuard(Effect->DurationPolicy, EGameplayEffectDurationType::Infinite);
	TGuardValue<EGameplayEffectStackingType> StackGuard(Effect->StackingType, EGameplayEffectStackingType::None);
	auto* Timeline = NewObject<UHodgeAbilityTimeline>();
	Timeline->Events.Add(Window(TEXT("Window"), HodgeGameplayTags::Status_Attack_Active, 0.f, 1.f));
	TestTrue(TEXT("Non-stacking infinite window is valid"), Valid(Timeline));
	Effect->StackingType = EGameplayEffectStackingType::AggregateByTarget;
	TestFalse(TEXT("Target stacking is rejected"), Valid(Timeline));
	Effect->StackingType = EGameplayEffectStackingType::AggregateBySource;
	TestFalse(TEXT("Source stacking is rejected"), Valid(Timeline));
	Effect->StackingType = EGameplayEffectStackingType::None;
	Effect->DurationPolicy = EGameplayEffectDurationType::Instant;
	TestFalse(TEXT("Instant window is rejected"), Valid(Timeline));
	Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	TestFalse(TEXT("Duration window is rejected"), Valid(Timeline));
	FHodgeTimelineEvent& Point = Timeline->Events[0];
	Point.Kind = EHodgeTimelineEventKind::Point;
	Point.PointEventTag = HodgeGameplayTags::GameplayEvent_Attack_Test;
	Point.PointEffectClass = UGameplayEffect::StaticClass();
	TestTrue(TEXT("Duration point is valid"), Valid(Timeline));
	Effect->DurationPolicy = EGameplayEffectDurationType::Instant;
	TestTrue(TEXT("Instant point is valid"), Valid(Timeline));
	Effect->DurationPolicy = EGameplayEffectDurationType::Infinite;
	TestFalse(TEXT("Infinite point is rejected"), Valid(Timeline));
	Point.PointEffectClass = nullptr;
	Point.StartTime = std::numeric_limits<float>::quiet_NaN();
	TestFalse(TEXT("NaN event time is rejected"), Valid(Timeline));
	Point.StartTime = 0.f;
	Timeline->Duration = std::numeric_limits<float>::infinity();
	TestFalse(TEXT("Infinite timeline duration is rejected"), Valid(Timeline));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeTimelineRejectTest, "Hodge.Timeline.RejectBeforeSideEffects",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FHodgeTimelineRejectTest::RunTest(const FString& Parameters)
{
	UGameplayEffect* Effect = GetMutableDefault<UGameplayEffect>();
	TGuardValue<EGameplayEffectDurationType> DurationGuard(Effect->DurationPolicy, EGameplayEffectDurationType::Infinite);
	TGuardValue<EGameplayEffectStackingType> StackGuard(Effect->StackingType, EGameplayEffectStackingType::AggregateByTarget);
	FTimelineWorld Fixture;
	auto* Timeline = NewObject<UHodgeAbilityTimeline>();
	Timeline->Events.Add(Window(TEXT("Window"), HodgeGameplayTags::Status_Attack_Active, 0.f, 1.f));
	const FActiveGameplayEffectHandle External = Fixture.ASC->ApplyGameplayEffectToSelf(Effect, 1.f, Fixture.ASC->MakeEffectContext());
	AddExpectedError(TEXT("PlayTimeline invalid asset"), EAutomationExpectedErrorFlags::Contains, 2);
	auto* Task = FHodgeTimelineTestAccess::Create(Fixture.ASC, Timeline);
	Task->ReadyForActivation();
	TestTrue(TEXT("Invalid window stops at activation"), Task->IsTimelineStopped());
	TestEqual(TEXT("Rejected window grants no tag"), Fixture.ASC->GetTagCount(HodgeGameplayTags::Status_Attack_Active), 0);
	TestEqual(TEXT("Existing effect stack is untouched"), Fixture.ASC->GetCurrentStackCount(External), 1);
	Fixture.ASC->RemoveActiveGameplayEffect(External);

	Effect->StackingType = EGameplayEffectStackingType::None;
	FHodgeTimelineEvent& Point = Timeline->Events[0];
	Point.Kind = EHodgeTimelineEventKind::Point;
	Point.PointEventTag = HodgeGameplayTags::GameplayEvent_Attack_Test;
	Point.PointEffectClass = UGameplayEffect::StaticClass();
	int32 EventCount = 0;
	const FDelegateHandle EventHandle = Fixture.ASC->GenericGameplayEventCallbacks.FindOrAdd(Point.PointEventTag)
		.AddLambda([&EventCount](const FGameplayEventData*) { ++EventCount; });
	Task = FHodgeTimelineTestAccess::Create(Fixture.ASC, Timeline);
	Task->ReadyForActivation();
	TestTrue(TEXT("Infinite point stops at activation"), Task->IsTimelineStopped());
	TestEqual(TEXT("Invalid point dispatches no event"), EventCount, 0);
	TestEqual(TEXT("Invalid point leaves no permanent GE"), Fixture.ASC->GetActiveEffects(FGameplayEffectQuery()).Num(), 0);
	Fixture.ASC->GenericGameplayEventCallbacks.FindChecked(Point.PointEventTag).Remove(EventHandle);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeTimelineCleanupTest, "Hodge.Timeline.CleanupAndOrdering",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FHodgeTimelineCleanupTest::RunTest(const FString& Parameters)
{
	UGameplayEffect* Effect = GetMutableDefault<UGameplayEffect>();
	TGuardValue<EGameplayEffectDurationType> DurationGuard(Effect->DurationPolicy, EGameplayEffectDurationType::Infinite);
	TGuardValue<EGameplayEffectStackingType> StackGuard(Effect->StackingType, EGameplayEffectStackingType::None);
	FTimelineWorld Fixture;
	auto* Timeline = NewObject<UHodgeAbilityTimeline>();
	Timeline->Events.Add(Window(TEXT("First"), HodgeGameplayTags::Status_Attack_Active, 0.f, 0.5f));
	Timeline->Events.Add(Window(TEXT("Second"), HodgeGameplayTags::Status_Attack_Recovery, 0.25f, 1.f));
	FHodgeTimelineEvent Point;
	Point.Kind = EHodgeTimelineEventKind::Point;
	Point.EventID = TEXT("Boundary");
	Point.StartTime = 0.5f;
	Point.PointEventTag = HodgeGameplayTags::GameplayEvent_Attack_Test;
	Timeline->Events.Add(Point);
	const FActiveGameplayEffectHandle External = Fixture.ASC->ApplyGameplayEffectToSelf(Effect, 1.f, Fixture.ASC->MakeEffectContext());
	int32 PointCount = 0;
	const FDelegateHandle EventHandle = Fixture.ASC->GenericGameplayEventCallbacks.FindOrAdd(Point.PointEventTag)
		.AddLambda([this, &Fixture, &PointCount](const FGameplayEventData*)
		{
			++PointCount;
			TestEqual(TEXT("Point observes first window closed"), Fixture.ASC->GetTagCount(HodgeGameplayTags::Status_Attack_Active), 0);
			TestEqual(TEXT("Point observes second window open"), Fixture.ASC->GetTagCount(HodgeGameplayTags::Status_Attack_Recovery), 1);
		});
	auto* Task = FHodgeTimelineTestAccess::Create(Fixture.ASC, Timeline);
	Task->ReadyForActivation();
	FHodgeTimelineTestAccess::Advance(Task, 0.3f);
	TestEqual(TEXT("External GE plus two independent windows"), Fixture.ASC->GetActiveEffects(FGameplayEffectQuery()).Num(), 3);
	FHodgeTimelineTestAccess::Advance(Task, 0.3f);
	TestEqual(TEXT("First exit preserves second and external GE"), Fixture.ASC->GetActiveEffects(FGameplayEffectQuery()).Num(), 2);
	FHodgeTimelineTestAccess::Advance(Task, 0.5f);
	TestTrue(TEXT("Timeline naturally completes"), Task->IsTimelineStopped());
	TestEqual(TEXT("Point fires exactly once across frames"), PointCount, 1);
	TestNotNull(TEXT("External GE survives natural end"), Fixture.ASC->GetActiveGameplayEffect(External));
	TestEqual(TEXT("All timeline effects removed"), Fixture.ASC->GetActiveEffects(FGameplayEffectQuery()).Num(), 1);
	Fixture.ASC->GenericGameplayEventCallbacks.FindChecked(Point.PointEventTag).Remove(EventHandle);

	Task = FHodgeTimelineTestAccess::Create(Fixture.ASC, Timeline);
	Task->ReadyForActivation();
	Task->StopTimeline(EHodgeTimelineStopReason::Interrupted);
	Task->StopTimeline(EHodgeTimelineStopReason::Interrupted);
	TestEqual(TEXT("Repeated stop is harmless"), Fixture.ASC->GetActiveEffects(FGameplayEffectQuery()).Num(), 1);
	TestEqual(TEXT("Interrupted window tag removed"), Fixture.ASC->GetTagCount(HodgeGameplayTags::Status_Attack_Active), 0);

	Task = FHodgeTimelineTestAccess::Create(Fixture.ASC, Timeline);
	Task->ReadyForActivation();
	Task->TaskOwnerEnded();
	TestEqual(TEXT("Owner shutdown cleans window GE"), Fixture.ASC->GetActiveEffects(FGameplayEffectQuery()).Num(), 1);
	TestEqual(TEXT("Owner shutdown cleans window tag"), Fixture.ASC->GetTagCount(HodgeGameplayTags::Status_Attack_Active), 0);
	Fixture.ASC->RemoveActiveGameplayEffect(External);
	return true;
}

#endif
