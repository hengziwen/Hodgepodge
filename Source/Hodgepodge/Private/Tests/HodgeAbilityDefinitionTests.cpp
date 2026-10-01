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
		TEXT("/Game/CodexText/Montage/AM_Attack01_Montage.AM_Attack01_Montage"));
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
	static void SetNode(UHodgeCombatComponentBase* Combo, FGameplayTag Tag) { Combo->SetNode(Tag); }
	static const FHodgeComboTransition* Select(UHodgeCombatComponentBase* Combo, FGameplayTag Tag) { return Combo->SelectTransition(Tag, false); }
	static FGameplayTag Input(UHodgeCombatComponentBase* Combo) { return Combo->BufferedInput; }
};

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
		TEXT("/Game/CodexText/Montage/AM_Attack01_Montage.AM_Attack01_Montage"));
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
