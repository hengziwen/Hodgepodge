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
	Timeline->bUseMontageDuration = true;
	AddExpectedError(TEXT("PlayTimeline 参数非法"), EAutomationExpectedErrorFlags::Contains, 1);
	Task = FHodgeTimelineTestAccess::Create(Fixture.ASC, Timeline);
	Task->ReadyForActivation();
	TestTrue(TEXT("Montage mode cannot silently use standalone clock"), Task->IsTimelineStopped());
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
	int32 CompletionCount = 0;
	Task->OnFinished.AddLambda([this, &CompletionCount](EHodgeTimelineStopReason Reason)
	{
		++CompletionCount;
		TestTrue(TEXT("Native completion reports natural end"), Reason == EHodgeTimelineStopReason::NaturalEnd);
	});
	Task->ReadyForActivation();
	FHodgeTimelineTestAccess::Advance(Task, 0.3f);
	TestTrue(TEXT("Execution windows include its own active window"), Task->GetActiveWindowTags().HasTag(HodgeGameplayTags::Status_Attack_Active));
	TestEqual(TEXT("External GE plus two independent windows"), Fixture.ASC->GetActiveEffects(FGameplayEffectQuery()).Num(), 3);
	FHodgeTimelineTestAccess::Advance(Task, 0.3f);
	TestEqual(TEXT("First exit preserves second and external GE"), Fixture.ASC->GetActiveEffects(FGameplayEffectQuery()).Num(), 2);
	FHodgeTimelineTestAccess::Advance(Task, 0.5f);
	TestTrue(TEXT("Timeline naturally completes"), Task->IsTimelineStopped());
	TestEqual(TEXT("Native completion fires exactly once"), CompletionCount, 1);
	TestTrue(TEXT("Stopped execution grants no windows"), Task->GetActiveWindowTags().IsEmpty());
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeTimelineWindowIdentityTest, "Hodge.Timeline.WindowIdentityAndCrossFrame",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FHodgeTimelineWindowIdentityTest::RunTest(const FString& Parameters)
{
	FTimelineWorld Fixture;
	auto* Timeline = NewObject<UHodgeAbilityTimeline>();
	FHodgeTimelineEvent Event = Window(TEXT("Hit"), HodgeGameplayTags::Status_Attack_HitCheck_Weapon, 0.1f, 0.12f);
	Event.WindowEffectClass = nullptr;
	Timeline->Events.Add(Event);
	auto* Task = FHodgeTimelineTestAccess::Create(Fixture.ASC, Timeline);
	TArray<int32> Order;
	Task->OnWindowEntered.AddLambda([&Order](int32 Index, FGameplayTag) { Order.Add(Index + 1); });
	Task->OnWindowExited.AddLambda([this, &Order](int32 Index, bool bFinal)
	{
		TestTrue(TEXT("Normal boundary requests final sample"), bFinal);
		Order.Add(-(Index + 1));
	});
	Task->ReadyForActivation();
	FHodgeTimelineTestAccess::Advance(Task, 0.2f);
	TestTrue(TEXT("Short window emits enter and exit in one update"), Order == TArray<int32>({1, -1}));
	Task->StopTimeline(EHodgeTimelineStopReason::AbilityCancelled);
	TestEqual(TEXT("Completed window is not closed twice"), Order.Num(), 2);

	// 两个 Task 对同一个 ASC 授予相同标签，仍各自收到独立的窗口生命周期。
	Timeline->Events[0].StartTime = 0.f;
	Timeline->Events[0].EndTime = 0.9f;
	auto* First = FHodgeTimelineTestAccess::Create(Fixture.ASC, Timeline);
	auto* Second = FHodgeTimelineTestAccess::Create(Fixture.ASC, Timeline);
	int32 Enters = 0;
	int32 Exits = 0;
	First->OnWindowEntered.AddLambda([&Enters](int32, FGameplayTag) { ++Enters; });
	Second->OnWindowEntered.AddLambda([&Enters](int32, FGameplayTag) { ++Enters; });
	First->OnWindowExited.AddLambda([&Exits](int32, bool) { ++Exits; });
	Second->OnWindowExited.AddLambda([&Exits](int32, bool) { ++Exits; });
	First->ReadyForActivation();
	Second->ReadyForActivation();
	TestEqual(TEXT("Same tag has two independent starts"), Enters, 2);
	First->StopTimeline(EHodgeTimelineStopReason::AbilityCancelled);
	TestEqual(TEXT("Only first task has exited"), Exits, 1);
	TestTrue(TEXT("Second window remains active"), Second->IsWindowActive(0));
	Second->TaskOwnerEnded();
	TestEqual(TEXT("Owner destruction closes remaining window"), Exits, 2);
	TestEqual(TEXT("Both tag contributions are released"), Fixture.ASC->GetTagCount(Event.WindowTag), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeTimelineWindowReentryTest, "Hodge.Timeline.WindowCallbackReentry",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FHodgeTimelineWindowReentryTest::RunTest(const FString& Parameters)
{
	FTimelineWorld Fixture;
	auto* Timeline = NewObject<UHodgeAbilityTimeline>();
	FHodgeTimelineEvent Event = Window(TEXT("Hit"), HodgeGameplayTags::Status_Attack_HitCheck_Body, 0.f, 0.5f);
	Event.WindowEffectClass = nullptr;
	Timeline->Events.Add(Event);
	auto* Task = FHodgeTimelineTestAccess::Create(Fixture.ASC, Timeline);
	int32 Exits = 0;
	Task->OnWindowEntered.AddLambda([Task](int32, FGameplayTag) { Task->StopTimeline(EHodgeTimelineStopReason::AbilityCancelled); });
	Task->OnWindowExited.AddLambda([this, &Exits](int32, bool bFinal)
	{
		++Exits;
		TestFalse(TEXT("Cancellation must not cause final damage sample"), bFinal);
	});
	Task->ReadyForActivation();
	TestTrue(TEXT("Enter callback can cancel task"), Task->IsTimelineStopped());
	Task->StopTimeline(EHodgeTimelineStopReason::AbilityCancelled);
	TestEqual(TEXT("Reentrant cleanup exits exactly once"), Exits, 1);
	TestEqual(TEXT("Cancellation removes tag"), Fixture.ASC->GetTagCount(Event.WindowTag), 0);

	Task = FHodgeTimelineTestAccess::Create(Fixture.ASC, Timeline);
	Task->OnWindowExited.AddLambda([Task](int32, bool) { Task->StopTimeline(EHodgeTimelineStopReason::AbilityCancelled); });
	Task->ReadyForActivation();
	FHodgeTimelineTestAccess::Advance(Task, 0.6f);
	TestEqual(TEXT("Exit callback cancellation still balances tag"), Fixture.ASC->GetTagCount(Event.WindowTag), 0);
	return true;
}

#endif
