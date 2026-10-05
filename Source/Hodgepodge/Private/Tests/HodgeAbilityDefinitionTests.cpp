#if WITH_DEV_AUTOMATION_TESTS
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Data/HodgeAbilityDefinition.h"
#include "Data/HodgeAbilityTimeline.h"
#include "Data/HodgeComboDefinition.h"
#include "Component/HodgeCombatComponentBase.h"
#include "Animation/AnimMontage.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Misc/AutomationTest.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeDefinitionGrantTest, "Hodge.Combo.DefinitionGrant",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeDefinitionGrantTest::RunTest(const FString& Parameters)
{
	const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(false)
		.CreateNavigation(false).CreateAISystem(false);
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
	AActor* Owner = World->SpawnActor<AActor>();
	auto* ASC = NewObject<UHodgeAbilitySystemComponent>(Owner);
	ASC->RegisterComponent();
	ASC->InitAbilityActorInfo(Owner, Owner);
	auto* Definition = NewObject<UHodgeAbilityDefinition>();
	Definition->AbilityTag = HodgeGameplayTags::Status_Attack;
	Definition->AbilityClass = UHodgeGameplayAbility_Definition::StaticClass();
	Definition->ExecutionConfig.Montage = LoadObject<UAnimMontage>(nullptr,
		TEXT("/Game/Main/Character/Hero/Anim/Montages/AM_Attack01_Montage.AM_Attack01_Montage"));
	Definition->ExecutionConfig.TimelineTaskConfig.Timeline = NewObject<UHodgeAbilityTimeline>();
	Definition->ExecutionConfig.TimelineTaskConfig.Timeline->bUseMontageDuration = true;
	UObject* Source = NewObject<UHodgeAbilityTimeline>(Owner);
	const auto First = ASC->GiveAbilityDefinition(Definition, 1, Source);
	TestTrue(TEXT("Valid definition grants a spec"), First.IsValid());
	if (auto* Spec = ASC->FindAbilitySpecFromHandle(First))
	{
		TestEqual(TEXT("Equipment source is preserved"), Spec->SourceObject.Get(), Source);
		TestTrue(TEXT("Definition lookup is separate"), ASC->FindAbilityDefinition(First) == Definition);
	}
	const auto Second = ASC->GiveAbilityDefinition(Definition, 1, Source);
	TestFalse(TEXT("Ambiguous ability tags do not select arbitrary grants"), ASC->FindDefinitionAbility(Definition->AbilityTag).IsValid());
	ASC->ClearAbility(First);
	TestNull(TEXT("Revoked definition association is removed"), ASC->FindAbilityDefinition(First));
	TestTrue(TEXT("Remaining grant resolves uniquely"), ASC->FindDefinitionAbility(Definition->AbilityTag) == Second);
	ASC->ClearAbility(Second);
	World->DestroyWorld(false);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeComboValidationTest, "Hodge.Combo.GraphValidation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeComboValidationTest::RunTest(const FString& Parameters)
{
	auto* Definition = NewObject<UHodgeComboDefinition>();
	Definition->ComboTable = NewObject<UDataTable>();
	Definition->ComboTable->RowStruct = FHodgeComboRow::StaticStruct();
	Definition->EntryComboTag = HodgeGameplayTags::Status_Attack;
	FHodgeComboRow Entry;
	Entry.ComboTag = Definition->EntryComboTag;
	FHodgeComboRow Attack;
	Attack.ComboTag = HodgeGameplayTags::Status_Attack_Active;
	Attack.AbilityTag = HodgeGameplayTags::Status_Attack_Active;
	FHodgeComboTransition Edge;
	Edge.TriggerInputIntentTag = HodgeGameplayTags::Status_Attack;
	Edge.TargetComboTag = Attack.ComboTag;
	Entry.Transitions.Add(Edge);
	Definition->ComboTable->AddRow(Entry.ComboTag.GetTagName(), Entry);
	Definition->ComboTable->AddRow(Attack.ComboTag.GetTagName(), Attack);
	TArray<FText> Errors;
	TestTrue(TEXT("Minimal graph valid"), Definition->ValidateDefinition(Errors));
	Entry.Transitions[0].TriggerEventTag = HodgeGameplayTags::GameplayEvent_Attack_Timeline_End;
	Definition->ComboTable->AddRow(Entry.ComboTag.GetTagName(), Entry);
	Errors.Reset();
	TestFalse(TEXT("Combined trigger rejected in V1"), Definition->ValidateDefinition(Errors));
	Entry.Transitions[0].TriggerEventTag = FGameplayTag();
	Entry.Transitions[0].TargetComboTag = HodgeGameplayTags::Status_Attack_Cancel_Move;
	Definition->ComboTable->AddRow(Entry.ComboTag.GetTagName(), Entry);
	Errors.Reset();
	TestFalse(TEXT("Missing target rejected"), Definition->ValidateDefinition(Errors));
	return true;
}

struct FHodgeComboTestAccess
{
	static void SetNode(UHodgeCombatComponentBase* Combo, FGameplayTag Tag)
	{
		Combo->CurrentAbility = NewObject<UHodgeGameplayAbility_Definition>(Combo);
		Combo->SetNode(Tag);
	}
	static const FHodgeComboTransition* Select(UHodgeCombatComponentBase* Combo, FGameplayTag Tag) { return Combo->SelectTransition(Tag, false); }
	static FGameplayTag Input(UHodgeCombatComponentBase* Combo) { return Combo->BufferedInput; }
	static UHodgeGameplayAbility_Definition* Begin(UHodgeCombatComponentBase* Combo, FGameplayTag Tag)
	{
		SetNode(Combo, Tag);
		Combo->ComboMemory = {Tag, 0, 7};
		return Combo->CurrentAbility;
	}
	static void Expire(UHodgeCombatComponentBase* Combo)
	{
		Combo->ComboMemory.ExpiresAt = Combo->ComboTime();
		Combo->ExpireComboMemory();
	}
	static void Reset(UHodgeCombatComponentBase* Combo) { Combo->ResetSession(false); }
	static void Reject(UHodgeCombatComponentBase* Combo) { Combo->RejectServerActivation(nullptr); }
	static int16 SourceKey(UHodgeCombatComponentBase* Combo) { return Combo->ExecutionKey(); }
	static void FailPreparedActivation(UHodgeCombatComponentBase* Combo)
	{
		Combo->PreviousComboMemory = Combo->ComboMemory;
		Combo->ComboMemory = {};
		Combo->bTransitionStarted = false;
		Combo->bSwitching = true;
		Combo->CompleteServerActivation();
	}
	static void FinishDuringActivation(UHodgeCombatComponentBase* Combo, UHodgeGameplayAbility_Definition* Ability)
	{
		Combo->bSwitching = true;
		Combo->bTransitionStarted = true;
		Combo->ExecutionEnded(Ability);
		Combo->CompleteServerActivation();
	}
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeComboRetentionTest, "Hodge.Combo.Retention",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeComboRetentionTest::RunTest(const FString& Parameters)
{
	const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(false)
		.CreateNavigation(false).CreateAISystem(false);
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
	AActor* Owner = World->SpawnActor<AActor>();
	auto* ASC = NewObject<UHodgeAbilitySystemComponent>(Owner);
	ASC->RegisterComponent();
	ASC->InitAbilityActorInfo(Owner, Owner);
	auto* Combo = NewObject<UHodgeCombatComponentBase>(Owner);
	Combo->RegisterComponent();
	auto* Definition = NewObject<UHodgeComboDefinition>();
	Definition->ComboTable = NewObject<UDataTable>();
	Definition->ComboTable->RowStruct = FHodgeComboRow::StaticStruct();
	Definition->EntryComboTag = HodgeGameplayTags::Status_Attack_Recovery;
	Definition->ComboRetentionSeconds = 1.f;
	const FGameplayTag Source = HodgeGameplayTags::Status_Attack_Active;
	const FGameplayTag Target = HodgeGameplayTags::Status_Attack_Cancel_Move;
	const FGameplayTag Intent = HodgeGameplayTags::InputTag_Move;
	FHodgeComboRow Entry;
	Entry.ComboTag = Definition->EntryComboTag;
	FHodgeComboRow Attack;
	Attack.ComboTag = Source;
	Attack.AbilityTag = Source;
	Attack.GrantedTags.AddTag(HodgeGameplayTags::Status_Attack);
	FHodgeComboTransition Edge;
	Edge.TriggerInputIntentTag = Intent;
	Edge.TargetComboTag = Target;
	Edge.RequiredWindowTags.AddTag(Intent);
	Edge.bAllowAfterExecutionEnded = true;
	Attack.Transitions.Add(Edge);
	FHodgeComboRow Last;
	Last.ComboTag = Target;
	Last.AbilityTag = Target;
	Last.GrantedTags = Attack.GrantedTags;
	Definition->ComboTable->AddRow(Entry.ComboTag.GetTagName(), Entry);
	Definition->ComboTable->AddRow(Source.GetTagName(), Attack);
	Definition->ComboTable->AddRow(Target.GetTagName(), Last);
	Combo->Configure(ASC, Definition);
	auto* Execution = FHodgeComboTestAccess::Begin(Combo, Source);
	ASC->AddLooseGameplayTag(Intent);
	TestNull(TEXT("Resume permission never bypasses an active execution window"), FHodgeComboTestAccess::Select(Combo, Intent));
	ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Attack);
	Combo->ExecutionEnded(Execution);
	TestEqual(TEXT("Stopped execution clears the active node"), Combo->GetCurrentComboTag(), Definition->EntryComboTag);
	TestEqual(TEXT("Movement/ability interruption preserves the last started node"), Combo->GetRememberedComboTag(), Source);
	TestEqual(TEXT("Idle continuation still identifies the last execution"), FHodgeComboTestAccess::SourceKey(Combo), int16(7));
	TestEqual(TEXT("Only this execution's attack tag is removed"), ASC->GetTagCount(HodgeGameplayTags::Status_Attack), 1);
	TestTrue(TEXT("Retention countdown begins at execution end"), FMath::IsNearlyEqual(Combo->GetComboMemoryRemainingTime(), 1.f));
	TestNotNull(TEXT("Idle resume does not need a finished timeline window"), FHodgeComboTestAccess::Select(Combo, Intent));
	FHodgeComboTestAccess::Reject(Combo);
	TestEqual(TEXT("Rejected activation does not erase confirmed memory"), Combo->GetRememberedComboTag(), Source);
	FHodgeComboTestAccess::FailPreparedActivation(Combo);
	TestEqual(TEXT("Failed prepared activation restores confirmed progress"), Combo->GetRememberedComboTag(), Source);
	Combo->ExecutionEnded(Execution);
	TestEqual(TEXT("Duplicate old execution completion preserves memory"), Combo->GetRememberedComboTag(), Source);
	FHodgeComboTestAccess::Expire(Combo);
	TestFalse(TEXT("Deadline expiry clears memory"), Combo->GetRememberedComboTag().IsValid());
	TestEqual(TEXT("Expiry discards the previous request identity"), FHodgeComboTestAccess::SourceKey(Combo), int16(0));
	TestNull(TEXT("Expired memory cannot resume"), FHodgeComboTestAccess::Select(Combo, Intent));
	Execution = FHodgeComboTestAccess::Begin(Combo, Target);
	Combo->ExecutionEnded(Execution);
	TestFalse(TEXT("Final node without resume edges resets immediately"), Combo->GetRememberedComboTag().IsValid());
	Execution = FHodgeComboTestAccess::Begin(Combo, Source);
	Definition->ComboRetentionSeconds = 0.f;
	Combo->ExecutionEnded(Execution);
	TestFalse(TEXT("Zero retention disables memory"), Combo->GetRememberedComboTag().IsValid());
	Definition->ComboRetentionSeconds = 1.f;
	Execution = FHodgeComboTestAccess::Begin(Combo, Source);
	FHodgeComboTestAccess::FinishDuringActivation(Combo, Execution);
	TestEqual(TEXT("Synchronous execution end still retains progress"), Combo->GetRememberedComboTag(), Source);
	TestEqual(TEXT("Synchronous execution end cleans active node"), Combo->GetCurrentComboTag(), Definition->EntryComboTag);
	FHodgeComboTestAccess::Reset(Combo);
	TestFalse(TEXT("Explicit reset clears memory"), Combo->GetRememberedComboTag().IsValid());
	Execution = FHodgeComboTestAccess::Begin(Combo, Source);
	Combo->ExecutionEnded(Execution);
	Combo->Shutdown();
	TestFalse(TEXT("Pawn/component teardown clears memory"), Combo->GetRememberedComboTag().IsValid());
	ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Attack);
	ASC->RemoveLooseGameplayTag(Intent);
	World->DestroyWorld(false);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeComboSessionTest, "Hodge.Combo.SessionRules",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeComboSessionTest::RunTest(const FString& Parameters)
{
	const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(false)
		.CreateNavigation(false).CreateAISystem(false);
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
	AActor* Owner = World->SpawnActor<AActor>();
	AActor* Avatar = World->SpawnActor<AActor>();
	auto* ASC = NewObject<UHodgeAbilitySystemComponent>(Owner);
	Owner->AddInstanceComponent(ASC);
	ASC->RegisterComponent();
	ASC->InitAbilityActorInfo(Owner, Avatar);
	auto* Combo = NewObject<UHodgeCombatComponentBase>(Avatar);
	Avatar->AddInstanceComponent(Combo);
	Combo->RegisterComponent();
	TestTrue(TEXT("Combat entry belongs to the current Avatar"),
		UHodgeCombatComponentBase::FindCombatComponent(ASC->GetAvatarActor()) == Combo);
	TestNull(TEXT("ASC owner does not host a second combat entry"), UHodgeCombatComponentBase::FindCombatComponent(Owner));
	auto* Definition = NewObject<UHodgeComboDefinition>();
	Definition->ComboTable = NewObject<UDataTable>();
	Definition->ComboTable->RowStruct = FHodgeComboRow::StaticStruct();
	Definition->EntryComboTag = HodgeGameplayTags::Status_Attack;
	const FGameplayTag Source = HodgeGameplayTags::Status_Attack_Active;
	const FGameplayTag First = HodgeGameplayTags::Status_Attack_Cancel_Move;
	const FGameplayTag Second = HodgeGameplayTags::Status_Attack_Cancel_NextAttack;
	FHodgeComboRow Entry;
	Entry.ComboTag = Definition->EntryComboTag;
	FHodgeComboRow Attack;
	Attack.ComboTag = Source;
	Attack.AbilityTag = Source;
	Attack.GrantedTags.AddTag(Source);
	FHodgeComboTransition Edge;
	Edge.TriggerInputIntentTag = First;
	Edge.TargetComboTag = Source;
	Edge.TransitionPriority = 1;
	Attack.Transitions.Add(Edge);
	Edge.TransitionPriority = 2;
	Attack.Transitions.Add(Edge);
	Attack.Transitions.Add(Edge);
	Definition->ComboTable->AddRow(Entry.ComboTag.GetTagName(), Entry);
	Definition->ComboTable->AddRow(Source.GetTagName(), Attack);
	FHodgeComboInputBinding Binding;
	Binding.InputTag = First;
	Binding.IntentTag = First;
	Definition->InputBindings.Add(Binding);
	Binding.InputTag = Second;
	Binding.IntentTag = Second;
	Definition->InputBindings.Add(Binding);
	Combo->Configure(ASC, Definition);
	FHodgeComboTestAccess::SetNode(Combo, Source);
	const auto* Row = Definition->FindNode(Source);
	TestTrue(TEXT("Highest priority then stable array order"), FHodgeComboTestAccess::Select(Combo, First) == &Row->Transitions[1]);
	Attack.Transitions[1].BlockedSourceTags.AddTag(Source);
	Attack.Transitions[2].RequiredSourceTags.AddTag(Second);
	Definition->ComboTable->AddRow(Source.GetTagName(), Attack);
	Row = Definition->FindNode(Source);
	TestTrue(TEXT("Blocked and missing required source tags exclude edges"), FHodgeComboTestAccess::Select(Combo, First) == &Row->Transitions[0]);
	Attack.Transitions[0].RequiredWindowTags.AddTag(Second);
	Definition->ComboTable->AddRow(Source.GetTagName(), Attack);
	ASC->AddLooseGameplayTag(Second);
	// Isolate the window requirement from the independent source-tag edge.
	Attack.Transitions.SetNum(1);
	Definition->ComboTable->AddRow(Source.GetTagName(), Attack);
	TestNull(TEXT("Aggregated foreign window cannot authorize this session"), FHodgeComboTestAccess::Select(Combo, First));
	Combo->InputPressed(First);
	TestEqual(TEXT("Unmatched input stays buffered"), FHodgeComboTestAccess::Input(Combo), First);
	Combo->InputPressed(Second);
	TestEqual(TEXT("New input replaces old slot"), FHodgeComboTestAccess::Input(Combo), Second);
	Combo->Configure(ASC, Definition);
	TestEqual(TEXT("Repeated initialization does not double node tags"), ASC->GetTagCount(Source), 1);
	ASC->InitAbilityActorInfo(Owner, World->SpawnActor<AActor>());
	TestEqual(TEXT("Avatar change cleans owned node tags"), ASC->GetTagCount(Source), 0);
	TestFalse(TEXT("Avatar change clears buffered input"), FHodgeComboTestAccess::Input(Combo).IsValid());
	Combo->Configure(ASC, Definition);
	TestFalse(TEXT("Old Pawn cannot accept combo input after Avatar replacement"), Combo->InputPressed(First));
	TestFalse(TEXT("Old Pawn remains unconfigured for the new Avatar"), Combo->GetCurrentComboTag().IsValid());
	TestEqual(TEXT("Avatar cleanup preserves foreign tags"), ASC->GetTagCount(Second), 1);
	ASC->InitAbilityActorInfo(Owner, Avatar);
	Combo->Configure(ASC, Definition);
	TestEqual(TEXT("Rebind starts at Entry"), Combo->GetCurrentComboTag(), Definition->EntryComboTag);
	Combo->Shutdown();
	Combo->Shutdown();
	ASC->RemoveLooseGameplayTag(Second);
	World->DestroyWorld(false);
	return true;
}

#if WITH_EDITOR
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeMontageDurationTest, "Hodge.Combo.MontageDuration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeMontageDurationTest::RunTest(const FString& Parameters)
{
	auto* Timeline = NewObject<UHodgeAbilityTimeline>();
	Timeline->bUseMontageDuration = true;
	Timeline->Duration = 0.f;
	FHodgeTimelineEvent Event;
	Event.Kind = EHodgeTimelineEventKind::Point;
	Event.EventID = TEXT("LatePoint");
	Event.StartTime = 2.f;
	Event.PointEventTag = HodgeGameplayTags::GameplayEvent_Attack_Test;
	Timeline->Events.Add(Event);
	FDataValidationContext AssetContext;
	TestTrue(TEXT("Montage asset validation ignores stale manual duration"),
		Timeline->IsDataValid(AssetContext) == EDataValidationResult::Valid);
	TArray<FText> Errors;
	TestFalse(TEXT("Montage playback requires an external length"), Timeline->ValidateForPlayback(Errors));
	Errors.Reset();
	TestTrue(TEXT("External length accepts reachable events"), Timeline->ValidateForPlayback(Errors, 3.f));
	Errors.Reset();
	TestFalse(TEXT("Shorter montage rejects unreachable events"), Timeline->ValidateForPlayback(Errors, 1.f));
	Timeline->Events[0].EventID = NAME_None;
	FDataValidationContext InvalidContext;
	TestTrue(TEXT("Montage asset still checks event structure"),
		Timeline->IsDataValid(InvalidContext) == EDataValidationResult::Invalid);
	Timeline->Events[0].EventID = TEXT("LatePoint");
	Timeline->bUseMontageDuration = false;
	Timeline->Duration = 1.f;
	FDataValidationContext ManualContext;
	TestTrue(TEXT("Standalone assets retain manual upper bound"),
		Timeline->IsDataValid(ManualContext) == EDataValidationResult::Invalid);
	Timeline->Duration = 3.f;
	Errors.Reset();
	TestTrue(TEXT("Standalone playback retains manual duration"), Timeline->ValidateForPlayback(Errors));

	auto* Definition = NewObject<UHodgeAbilityDefinition>();
	Definition->AbilityTag = HodgeGameplayTags::Status_Attack;
	Definition->AbilityClass = UHodgeGameplayAbility_Definition::StaticClass();
	Definition->ExecutionConfig.Montage = LoadObject<UAnimMontage>(nullptr,
		TEXT("/Game/Main/Character/Hero/Anim/Montages/AM_Attack01_Montage.AM_Attack01_Montage"));
	Definition->ExecutionConfig.TimelineTaskConfig.Timeline = Timeline;
	if (!TestNotNull(TEXT("Test montage loaded"), Definition->ExecutionConfig.Montage.Get())) { return false; }
	Timeline->Events[0].StartTime = Definition->GetDuration() * .6f;
	Errors.Reset();
	TestFalse(TEXT("Definition rejects manual timeline mode"), Definition->ValidateDefinition(Errors));
	Timeline->bUseMontageDuration = true;
	Timeline->Duration = 0.f;
	Errors.Reset();
	TestTrue(TEXT("Definition derives duration from montage"), Definition->ValidateDefinition(Errors));
	Definition->ExecutionConfig.PlayRate = 2.f;
	Errors.Reset();
	TestTrue(TEXT("Play rate does not rescale source-time event bounds"), Definition->ValidateDefinition(Errors));
	Timeline->Events[0].StartTime = Definition->GetDuration() + 1.f;
	Errors.Reset();
	TestFalse(TEXT("Definition rejects events past montage end"), Definition->ValidateDefinition(Errors));
	return true;
}
#endif
#endif
