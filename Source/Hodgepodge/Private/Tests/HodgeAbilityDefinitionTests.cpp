/** @file HodgeAbilityDefinitionTests.cpp
 * @brief 实现单段技能执行定义自动化测试相关的类型、接口与现有处理流程。
 */
#if WITH_DEV_AUTOMATION_TESTS
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Data/HodgeAbilityDefinition.h"
#include "Data/HodgeComboDefinition.h"
#include "Component/HodgeCombatComponentBase.h"
#include "Animation/AnimMontage.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Misc/AutomationTest.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeDefinitionGrantTest, "Hodge.Combo.DefinitionGrant",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeDefinitionGrantTest::RunTest(const FString& Parameters)
{
	const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(false)
		.CreateNavigation(false).CreateAISystem(false);
	// 本次操作所在的世界。
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
	// 当前操作关联的所属对象。
	AActor* Owner = World->SpawnActor<AActor>();
	auto* ASC = NewObject<UHodgeAbilitySystemComponent>(Owner);
	ASC->RegisterComponent();
	ASC->InitAbilityActorInfo(Owner, Owner);
	auto* Definition = NewObject<UHodgeAbilityDefinition>();
	Definition->AbilityTag = HodgeGameplayTags::Status_Attack;
	Definition->AbilityClass = UHodgeGameplayAbility_Definition::StaticClass();
	Definition->ExecutionConfig.Montage = LoadObject<UAnimMontage>(nullptr,
		TEXT("/Game/Main/Character/Hero/Anim/Montages/AM_Attack01_Montage.AM_Attack01_Montage"));
	Definition->ExecutionConfig.Montage = DuplicateObject<UAnimMontage>(Definition->ExecutionConfig.Montage, Definition);
	Definition->ExecutionConfig.Montage->Notifies.Reset();
	UObject* Source = NewObject<USceneComponent>(Owner);
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
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeComboValidationTest::RunTest(const FString& Parameters)
{
	// 当前操作使用的定义配置。
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
	Entry.Transitions[0].TriggerEventTag = HodgeGameplayTags::GameplayEvent_Attack_Completed;
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

/** 保存连段测试Access的数据和接口契约。 */
struct FHodgeComboTestAccess
{
	/** 设置节点。
	 * @param Combo 连段，由调用方传入。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 */
	static void SetNode(UHodgeCombatComponentBase* Combo, FGameplayTag Tag)
	{
		Combo->CurrentAbility = NewObject<UHodgeGameplayAbility_Definition>(Combo);
		Combo->SetNode(Tag);
	}
	/** 按当前约束选择所需候选。
	 * @param Combo 连段，由调用方传入。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 * @return 本次计算、查询或创建得到的连段转换。
	 */
	static const FHodgeComboTransition* Select(UHodgeCombatComponentBase* Combo, FGameplayTag Tag) { return Combo->SelectTransition(Tag, false); }
	/** 记录当前实验对象使用的输入状态。
	 * @param Combo 连段，由调用方传入。
	 * @return 本次计算、查询或创建得到的玩法标签。
	 */
	static FGameplayTag Input(UHodgeCombatComponentBase* Combo) { return Combo->BufferedInput; }
	/** 开始本轮处理。
	 * @param Combo 连段，由调用方传入。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 * @return 本次计算、查询或创建得到的玩法能力定义。
	 */
	static UHodgeGameplayAbility_Definition* Begin(UHodgeCombatComponentBase* Combo, FGameplayTag Tag)
	{
		SetNode(Combo, Tag);
		Combo->ComboMemory = {Tag, 0, 7};
		return Combo->CurrentAbility;
	}
	/** 使当前测试的缓存或会话达到过期条件。
	 * @param Combo 连段，由调用方传入。
	 */
	static void Expire(UHodgeCombatComponentBase* Combo)
	{
		Combo->ComboMemory.ExpiresAt = Combo->ComboTime();
		Combo->ExpireComboMemory();
	}
	/** 恢复本对象的初始状态。
	 * @param Combo 连段，由调用方传入。
	 */
	static void Reset(UHodgeCombatComponentBase* Combo) { Combo->ResetSession(false); }
	/** 结束当前被拒绝的请求或交接。
	 * @param Combo 连段，由调用方传入。
	 */
	static void Reject(UHodgeCombatComponentBase* Combo) { Combo->RejectServerActivation(nullptr); }
	/** 取得当前来源用于配对资源记录的标识。
	 * @param Combo 连段，由调用方传入。
	 * @return 本次计算、查询或创建得到的整数16。
	 */
	static int16 SourceKey(UHodgeCombatComponentBase* Combo) { return Combo->ExecutionKey(); }
	/** 结束未能成功提交的准备激活并清理其状态。
	 * @param Combo 连段，由调用方传入。
	 */
	static void FailPreparedActivation(UHodgeCombatComponentBase* Combo)
	{
		Combo->PreviousComboMemory = Combo->ComboMemory;
		Combo->ComboMemory = {};
		Combo->bTransitionStarted = false;
		Combo->bSwitching = true;
		Combo->CompleteServerActivation();
	}
	/** 结束DuringActivation。
	 * @param Combo 连段，由调用方传入。
	 * @param Ability 能力，由调用方传入。
	 */
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
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeComboRetentionTest::RunTest(const FString& Parameters)
{
	const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(false)
		.CreateNavigation(false).CreateAISystem(false);
	// 本次操作所在的世界。
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
	// 当前操作关联的所属对象。
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
	auto* TargetDefinition = NewObject<UHodgeAbilityDefinition>();
	TargetDefinition->AbilityTag = Target;
	TargetDefinition->AbilityClass = UHodgeGameplayAbility_Definition::StaticClass();
	TargetDefinition->ExecutionConfig.Montage = LoadObject<UAnimMontage>(nullptr,
		TEXT("/Game/Main/Character/Hero/Anim/Montages/AM_Attack01_Montage.AM_Attack01_Montage"));
	TargetDefinition->ExecutionConfig.Montage = DuplicateObject<UAnimMontage>(TargetDefinition->ExecutionConfig.Montage, TargetDefinition);
	TargetDefinition->ExecutionConfig.Montage->Notifies.Reset();
	const auto TargetHandle = ASC->GiveAbilityDefinition(TargetDefinition, 1, Combo);
	TestTrue(TEXT("Buffer fixture grants a valid target"), TargetHandle.IsValid());
	FGameplayEventData Request;
	Request.EventTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayEvent.Combo.InputRequest"));
	Request.Instigator = Owner;
	Request.OptionalObject = Definition;
	Request.TargetTags.AddTag(Source);
	Request.InstigatorTags.AddTag(Intent);
	Request.EventMagnitude = FHodgeComboTestAccess::SourceKey(Combo);
	TestTrue(TEXT("Valid early input may wait for its native window"), Combo->CanBufferServerActivation(TargetHandle, &Request));
	TestFalse(TEXT("Waiting never grants a closed execution window"), Combo->PrepareServerActivation(TargetHandle, &Request));
	// 同一目标有开放边时应立即准备，较高优先级闭边不能劫持请求进入等待。
	auto* SourceRow = Definition->ComboTable->FindRow<FHodgeComboRow>(Source.GetTagName(), TEXT("MixedWindowRegression"));
	FHodgeComboTransition OpenEdge = SourceRow->Transitions[0];
	OpenEdge.RequiredWindowTags.Reset();
	OpenEdge.TransitionPriority = -1;
	SourceRow->Transitions.Add(OpenEdge);
	TestFalse(TEXT("An open edge prevents waiting for another closed edge to the same target"), Combo->CanBufferServerActivation(TargetHandle, &Request));
	TestTrue(TEXT("Normal selection still chooses the ready edge"), FHodgeComboTestAccess::Select(Combo, Intent) == &SourceRow->Transitions[1]);
	SourceRow->Transitions[1].BlockedSourceTags.AddTag(Intent);
	TestTrue(TEXT("A blocked open edge does not bypass valid waiting"), Combo->CanBufferServerActivation(TargetHandle, &Request));
	// 另一个合法目标动作开放时，仍保留已请求派生动作的原有短暂等待。
	auto* OtherDefinition = NewObject<UHodgeAbilityDefinition>();
	OtherDefinition->AbilityTag = Source;
	OtherDefinition->AbilityClass = UHodgeGameplayAbility_Definition::StaticClass();
	OtherDefinition->ExecutionConfig.Montage = TargetDefinition->ExecutionConfig.Montage;
	const auto OtherHandle = ASC->GiveAbilityDefinition(OtherDefinition, 1, Combo);
	TestTrue(TEXT("Alternative target is really granted"), OtherHandle.IsValid());
	SourceRow->Transitions[1] = OpenEdge;
	SourceRow->Transitions[1].TargetComboTag = Source;
	TestTrue(TEXT("Another ready target does not reject the requested target's native wait"), Combo->CanBufferServerActivation(TargetHandle, &Request));
	SourceRow->Transitions.SetNum(1);
	Request.EventMagnitude += 1;
	TestFalse(TEXT("Another activation key cannot be buffered"), Combo->CanBufferServerActivation(TargetHandle, &Request));
	Request.EventMagnitude -= 1;
	Request.EventTag = HodgeGameplayTags::GameplayEvent_Attack_Completed;
	TestFalse(TEXT("Client-declared gameplay messages cannot be buffered"), Combo->CanBufferServerActivation(TargetHandle, &Request));
	Request.EventTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayEvent.Combo.InputRequest"));
	Request.TargetTags.Reset();
	Request.TargetTags.AddTag(Target);
	TestFalse(TEXT("Another source node cannot be buffered"), Combo->CanBufferServerActivation(TargetHandle, &Request));
	Request.TargetTags.Reset();
	Request.TargetTags.AddTag(Source);
	TestFalse(TEXT("An ungranted target cannot be buffered"), Combo->CanBufferServerActivation({}, &Request));
	ASC->AddLooseGameplayTag(TAG_Gameplay_AbilityInputBlocked);
	TestFalse(TEXT("Input blocking invalidates pending requests"), Combo->CanBufferServerActivation(TargetHandle, &Request));
	ASC->RemoveLooseGameplayTag(TAG_Gameplay_AbilityInputBlocked);
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
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeComboSessionTest::RunTest(const FString& Parameters)
{
	const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(false)
		.CreateNavigation(false).CreateAISystem(false);
	// 本次操作所在的世界。
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
	// 当前操作关联的所属对象。
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

#endif
