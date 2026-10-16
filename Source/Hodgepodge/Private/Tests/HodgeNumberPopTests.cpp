// Copyright Epic Games, Inc. All Rights Reserved.
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Feedback/NumberPops/HodgeNumberPopComponent_NiagaraText.h"
#include "Core/PlayerController/HodgePlayerController.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Component/HodgeHealthComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "GameplayEffect.h"
#include "TimerManager.h"

namespace HodgeNumberPopTests
{
	/** 隔离世界验证真实 GE 结算、来源归属和绑定周期，不加载表现资产。 */
	struct FFixture
	{
		UWorld* World;
		AHodgePlayerController* PC;
		APawn* Source;
		APawn* Target;
		UHodgeAbilitySystemComponent* SourceASC;
		UHodgeAbilitySystemComponent* TargetASC;
		UHodgeHealthComponent* Health;
		double CreatedAt = FPlatformTime::Seconds();
		FFixture()
		{
			const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(false).CreateNavigation(false).CreateAISystem(false);
			World = UWorld::CreateWorld(EWorldType::EditorPreview, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
			PC = World->SpawnActor<AHodgePlayerController>();
			Source = World->SpawnActor<APawn>();
			PC->Possess(Source);
			SourceASC = NewObject<UHodgeAbilitySystemComponent>(Source);
			Source->AddInstanceComponent(SourceASC); SourceASC->RegisterComponent();
			SourceASC->InitAbilityActorInfo(Source, Source);
			Target = World->SpawnActor<APawn>();
			TargetASC = NewObject<UHodgeAbilitySystemComponent>(Target);
			Target->AddInstanceComponent(TargetASC); TargetASC->RegisterComponent();
			TargetASC->AddAttributeSetSubobject(NewObject<UHodgeHealthSet>(Target));
			TargetASC->InitAbilityActorInfo(Target, Target);
			Health = NewObject<UHodgeHealthComponent>(Target);
			Target->AddInstanceComponent(Health); Health->RegisterComponent();
			Health->InitializeWithAbilitySystem(TargetASC);
		}
		~FFixture() { Health->UninitializeFromAbilitySystem(); World->DestroyWorld(false); }
		/** 在测试中替换属性值，正式 GE 伤害仍走 HealthSet 的完整回调。 */
		void SetHealth(float Value) { TargetASC->SetNumericAttributeBase(UHodgeHealthSet::GetHealthAttribute(), Value); }
		void Apply(float Amount, FGameplayAttribute Attribute = UHodgeHealthSet::GetDamageAttribute(), UAbilitySystemComponent* Instigator = nullptr)
		{
			auto* Effect = NewObject<UGameplayEffect>();
			Effect->DurationPolicy = EGameplayEffectDurationType::Instant;
			FGameplayModifierInfo Modifier;
			Modifier.Attribute = Attribute; Modifier.ModifierOp = EGameplayModOp::Additive;
			Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(Amount));
			Effect->Modifiers.Add(Modifier);
			FGameplayEffectSpec Spec(Effect, (Instigator ? Instigator : SourceASC)->MakeEffectContext(), 1.f);
			TargetASC->ApplyGameplayEffectSpecToSelf(Spec);
		}
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeNumberFormatTest, "Hodge.NumberPop.FormattingAndValidation", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 浮点数保留原值；字形边界负责合法取整与溢出保护。 */
bool FHodgeNumberFormatTest::RunTest(const FString& Parameters)
{
	FHodgeNumberPopRequest R;
	TestFalse(TEXT("Zero damage rejected"), R.IsValid());
	R.DamageAmount = .1f;
	TestEqual(TEXT("Positive fractional damage never renders zero"), R.GetNumberToDisplay(), 1);
	TestEqual(TEXT("Raw damage remains fractional"), R.DamageAmount, .1f);
	R.DamageAmount = 10.6f;
	TestEqual(TEXT("Display uses nearest integer"), R.GetNumberToDisplay(), 11);
	R.DamageAmount = TNumericLimits<float>::Max();
	TestEqual(TEXT("Extreme input clamps before integer conversion"), R.GetNumberToDisplay(), MAX_int32);
	R.DamageAmount = -1.f;
	TestFalse(TEXT("Negative damage is not healing presentation"), R.IsValid());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeNumberAcceptedTest, "Hodge.NumberPop.AcceptedDamageAndLifecycle", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 最后一击、零伤、免疫、治疗、直接扣血和解绑不会混淆来源。 */
bool FHodgeNumberAcceptedTest::RunTest(const FString& Parameters)
{
	HodgeNumberPopTests::FFixture F;
	F.SetHealth(10.f); F.Apply(100.f);
	TestEqual(TEXT("Overkill displays only ten accepted HP"), F.PC->GetLastDamageNumberSent().DamageAmount, 10.f);
	TestEqual(TEXT("Killing blow sends once"), F.PC->GetDamageNumberSentCount(), 1);
	F.Apply(100.f); F.Apply(0.f);
	TestEqual(TEXT("Dead or zero damage produces no number"), F.PC->GetDamageNumberSentCount(), 1);
	F.SetHealth(100.f);
	F.TargetASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Gameplay.DamageImmunity")));
	F.Apply(20.f);
	TestEqual(TEXT("Immunity produces no number"), F.PC->GetDamageNumberSentCount(), 1);
	F.TargetASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Gameplay.DamageImmunity")));
	F.Apply(-10.f, UHodgeHealthSet::GetHealthAttribute());
	F.Apply(5.f, UHodgeHealthSet::GetHealingAttribute());
	TestEqual(TEXT("Direct HP change and healing are excluded"), F.PC->GetDamageNumberSentCount(), 1);
	F.Health->UninitializeFromAbilitySystem(); F.Apply(2.f);
	TestEqual(TEXT("Unbound component stops forwarding"), F.PC->GetDamageNumberSentCount(), 1);
	F.Health->InitializeWithAbilitySystem(F.TargetASC); F.Apply(.5f);
	TestEqual(TEXT("Rebinding sends exactly once"), F.PC->GetDamageNumberSentCount(), 2);
	TestEqual(TEXT("Fractional accepted value remains exact"), F.PC->GetLastDamageNumberSent().DamageAmount, .5f);
	F.TargetASC->SetAvatarActor(F.Source); F.Apply(2.f);
	TestEqual(TEXT("Old pawn cannot forward shared ASC damage"), F.PC->GetDamageNumberSentCount(), 2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeNumberSourceTest, "Hodge.NumberPop.SourceOwnershipAndDedup", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 同额多刀不合并，重复身份被忽略；非玩家来源不会归到玩家零。 */
bool FHodgeNumberSourceTest::RunTest(const FString& Parameters)
{
	HodgeNumberPopTests::FFixture F;
	auto* PS = F.World->SpawnActor<APlayerState>();
	PS->SetOwner(F.PC);
	F.SourceASC->InitAbilityActorInfo(PS, F.Source);
	F.Apply(2.f); F.Apply(2.f);
	TestEqual(TEXT("PlayerState ASC routes separate equal hits"), F.PC->GetDamageNumberSentCount(), 2);
	F.Apply(2.f, UHodgeHealthSet::GetDamageAttribute(), F.TargetASC);
	TestEqual(TEXT("AI/self source is not player zero"), F.PC->GetDamageNumberSentCount(), 2);
	auto* Pop = F.PC->GetNumberPopComponent();
	TestEqual(TEXT("Events wait for one transport flush"), Pop->GetReceivedCount(), 0);
	const int32 Before = Pop->GetReceivedCount();
	FHodgeNumberPopRequest R; R.DamageAmount = 1.f; R.EventId = 200;
	Pop->AddNumberPop(R); Pop->AddNumberPop(R); R.EventId = 201; Pop->AddNumberPop(R);
	TestEqual(TEXT("Only duplicate identity suppressed"), Pop->GetReceivedCount() - Before, 2);
	TestNull(TEXT("Preview test does not create Niagara or load style"), Pop->GetNumberPopNiagara());
	return true;
}

DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FHodgeNumberPopBatchCheck, TSharedPtr<HodgeNumberPopTests::FFixture>, Fixture, FAutomationTestBase*, Test);
/** 隔离世界不会自动 Tick；跨引擎帧驱动，兼容 Pending Timer 在首帧仅激活的情况。 */
bool FHodgeNumberPopBatchCheck::Update()
{
	// Preview 世界没有初始化 Actor；只在本地批次测试中允许 ProcessEvent，真实网络另由 PIE 验收。
	TGuardValue<bool> AllowPreviewEvents(GAllowActorScriptExecutionInEditor, true);
	Fixture->World->GetTimerManager().Tick(.1f);
	if (Fixture->PC->GetNumberPopComponent()->GetReceivedCount() != 72 && FPlatformTime::Seconds() - Fixture->CreatedAt < 1.) { return false; }
	Test->TestEqual(TEXT("Deferred batches preserve seventy-two independent contacts"), Fixture->PC->GetNumberPopComponent()->GetReceivedCount(), 72);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeNumberBatchTest, "Hodge.NumberPop.TransportBatch", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 异步验证实际 Timer 批量传输，不改写全局帧号来伪造下一帧。 */
bool FHodgeNumberBatchTest::RunTest(const FString& Parameters)
{
	auto Fixture = MakeShared<HodgeNumberPopTests::FFixture>();
	for (int32 Index = 0; Index < 72; ++Index) { Fixture->Apply(.1f); }
	TestEqual(TEXT("Fixture queued seventy-two accepted server events"), Fixture->PC->GetDamageNumberSentCount(), 72);
	TestEqual(TEXT("Accepted contacts wait in server queue"), Fixture->PC->GetNumberPopComponent()->GetReceivedCount(), 0);
	ADD_LATENT_AUTOMATION_COMMAND(FHodgeNumberPopBatchCheck(Fixture, this));
	return true;
}
#endif
