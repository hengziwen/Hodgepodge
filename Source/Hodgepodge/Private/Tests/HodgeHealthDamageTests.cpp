/** @file HodgeHealthDamageTests.cpp @brief 验证真实伤害GE的同步重入、独立生命快照与死亡去重。 */
#if WITH_DEV_AUTOMATION_TESTS
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameplayEffect.h"
#include "Misc/AutomationTest.h"

namespace
{
	/** 测试只使用独立世界和瞬时GE，不修改正式资产或共享默认对象。 */
	struct FHealthDamageFixture
	{
		UWorld* World;
		UHodgeAbilitySystemComponent* ASC;
		UHodgeHealthSet* Health;
		FHealthDamageFixture()
		{
			const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(false)
				.CreateNavigation(false).CreateAISystem(false);
			World = UWorld::CreateWorld(EWorldType::EditorPreview, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
			auto* Pawn = World->SpawnActor<APawn>();
			ASC = NewObject<UHodgeAbilitySystemComponent>(Pawn);
			ASC->RegisterComponent();
			Health = NewObject<UHodgeHealthSet>(Pawn);
			ASC->AddAttributeSetSubobject(Health);
			ASC->InitAbilityActorInfo(Pawn, Pawn);
		}
		~FHealthDamageFixture()
		{
			Health->OnDamageAccepted.Clear(); Health->OnHealthChanged.Clear(); Health->OnOutOfHealth.Clear();
			World->DestroyWorld(false);
		}
		/** 经过生产PostGameplayEffectExecute，不直接调用回调或伪造生命减少。 */
		void Damage(float Amount) const
		{
			auto* Effect = NewObject<UGameplayEffect>();
			Effect->DurationPolicy = EGameplayEffectDurationType::Instant;
			FGameplayModifierInfo Modifier;
			Modifier.Attribute = UHodgeHealthSet::GetDamageAttribute();
			Modifier.ModifierOp = EGameplayModOp::Additive;
			Modifier.ModifierMagnitude = FScalableFloat(Amount);
			Effect->Modifiers.Add(Modifier);
			FGameplayEffectSpec Spec(Effect, ASC->MakeEffectContext(), 1.f);
			ASC->ApplyGameplayEffectSpecToSelf(Spec);
		}
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeDamageAcceptedReentryTest, "Hodge.Combat.DamageAcceptedReentry",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 外层通知同步追加内层伤害；两笔只各扣一次，历史通知不能伪装成实时生命。 */
bool FHodgeDamageAcceptedReentryTest::RunTest(const FString& Parameters)
{
	FHealthDamageFixture F;
	TArray<FVector> Accepted;
	TArray<FVector2D> Changed;
	TArray<float> CurrentHealth;
	int32 DeathCount = 0;
	bool bNested = false;
	F.Health->OnDamageAccepted.AddLambda([&](AActor*, AActor*, const FGameplayEffectSpec*, float Amount, float Old, float New)
	{
		Accepted.Emplace(Amount, Old, New);
		if (!bNested) { bNested = true; F.Damage(5.f); }
	});
	F.Health->OnHealthChanged.AddLambda([&](AActor*, AActor*, const FGameplayEffectSpec*, float, float Old, float New)
	{
		Changed.Emplace(Old, New);
		// 与正式UI数据源一样读取当前属性，而非用迟到的外层快照覆盖血条。
		CurrentHealth.Add(F.Health->GetHealth());
	});
	F.Health->OnOutOfHealth.AddLambda([&](AActor*, AActor*, const FGameplayEffectSpec*, float, float, float) { ++DeathCount; });
	F.Damage(10.f);
	TestEqual(TEXT("Nested ten plus five consumes fifteen health"), F.Health->GetHealth(), 85.f);
	TestEqual(TEXT("Damage meta is empty after both executions"), F.Health->GetDamage(), 0.f);
	if (TestEqual(TEXT("Both accepted events exist"), Accepted.Num(), 2))
	{
		TestTrue(TEXT("Outer accepted event preserves its own result"), Accepted[0].Equals(FVector(10.f, 100.f, 90.f)));
		TestTrue(TEXT("Inner accepted event preserves its own result"), Accepted[1].Equals(FVector(5.f, 90.f, 85.f)));
	}
	if (TestEqual(TEXT("Both health events exist"), Changed.Num(), 2))
	{
		TestTrue(TEXT("Synchronous inner health event finishes first"), Changed[0].Equals(FVector2D(90.f, 85.f)));
		TestTrue(TEXT("Outer health event retains its pre-callback snapshot"), Changed[1].Equals(FVector2D(100.f, 90.f)));
		TestTrue(TEXT("UI observers always read the final live health"), CurrentHealth[0] == 85.f && CurrentHealth[1] == 85.f);
	}
	TestEqual(TEXT("Nonlethal nested damage does not signal death"), DeathCount, 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeNestedDamageDeathTest, "Hodge.Combat.DamageAcceptedDeathReentry",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 内外任一笔致死都只通知一次；死亡监听器重入不能再次发布同次死亡。 */
bool FHodgeNestedDamageDeathTest::RunTest(const FString& Parameters)
{
	for (const bool bOuterLethal : {false, true})
	{
		FHealthDamageFixture F;
		bool bNested = false, bDeathReentry = false;
		int32 DeathCount = 0;
		float KillingMagnitude = 0.f;
		TArray<float> AcceptedAmounts;
		F.Health->OnDamageAccepted.AddLambda([&](AActor*, AActor*, const FGameplayEffectSpec*, float Amount, float, float)
		{
			AcceptedAmounts.Add(Amount);
			if (!bNested) { bNested = true; F.Damage(20.f); }
		});
		F.Health->OnOutOfHealth.AddLambda([&](AActor*, AActor*, const FGameplayEffectSpec*, float Amount, float, float)
		{
			++DeathCount; KillingMagnitude = Amount;
			if (!bDeathReentry) { bDeathReentry = true; F.Damage(1.f); }
		});
		F.Damage(bOuterLethal ? 100.f : 90.f);
		TestEqual(TEXT("Nested lethal damage clamps at zero"), F.Health->GetHealth(), 0.f);
		TestEqual(TEXT("One depletion emits one death despite reentry"), DeathCount, 1);
		TestEqual(TEXT("Zero-loss nested effects do not steal death attribution"), KillingMagnitude, bOuterLethal ? 100.f : 20.f);
		TestEqual(TEXT("Only actual loss produces accepted events"), AcceptedAmounts.Num(), bOuterLethal ? 1 : 2);
		if (!bOuterLethal && AcceptedAmounts.Num() == 2)
		{ TestEqual(TEXT("Nested overkill reports only remaining health"), AcceptedAmounts[1], 10.f); }
	}
	return true;
}
#endif
