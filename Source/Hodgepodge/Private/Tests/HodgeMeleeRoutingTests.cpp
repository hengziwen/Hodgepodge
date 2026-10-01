#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystem/HodgeGameplayEffectContext.h"
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"
#include "AbilitySystemComponent.h"
#include "Component/HodgeCombatComponentBase.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "GameplayEffect.h"
#include "GameFramework/Actor.h"
#include "Misc/AutomationTest.h"

// 不增加测试反射类，直接验证真实 Task 的幂等释放接口。
struct FHodgeMeleeTestAccess
{
	static FGameplayEffectSpecHandle Build(UHodgeGameplayAbility_Melee* Ability, UAbilitySystemComponent* ASC,
		FGameplayAbilitySpecHandle Handle, const FHodgeHitDetectionBatch& Batch, const FHitResult& Hit,
		const FHodgeHitWindowBinding& Binding)
	{
		Ability->SetCurrentActorInfo(Handle, ASC->AbilityActorInfo.Get());
		return Ability->BuildMeleeHitSpec(Batch, Hit, Binding);
	}

	static void Release(UHodgeAbilityTask_WaitHitResults* Task, UHodgeCombatComponentBase* Combat, const FGuid& Execution)
	{
		Task->CombatComponent = Combat;
		Task->ExecutionId = Execution;
		Task->ReleaseSessions();
	}
};

namespace
{
	struct FMeleeDetectionWorld
	{
		UWorld* World = nullptr;
		AActor* Owner = nullptr;
		AActor* Target = nullptr;
		UHodgeCombatComponentBase* Combat = nullptr;
		FHodgeHitDetectionRequest Request;

		FMeleeDetectionWorld()
		{
			const UWorld::InitializationValues Values = UWorld::InitializationValues()
				.AllowAudioPlayback(false).CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false);
			World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
			Owner = World->SpawnActor<AActor>();
			auto* Root = NewObject<USceneComponent>(Owner, TEXT("DetectionOrigin"));
			Owner->SetRootComponent(Root);
			Root->RegisterComponent();
			Combat = NewObject<UHodgeCombatComponentBase>(Owner);
			Combat->RegisterComponent();
			FHodgeHitSource Source;
			Source.SourceTag = HodgeGameplayTags::Combat_Source_Body_Origin;
			Source.LocalOffset = FVector(100.f, 0.f, 0.f);
			Source.Radius = 40.f;
			Combat->HitSources.Add(Source);
			Request.SourceTag = Source.SourceTag;
			Request.Profile = NewObject<UHodgeHitDetectionProfile>(Combat);
			Request.Profile->bRequireLineOfSight = false;
			Target = World->SpawnActor<AActor>();
			auto* Shape = NewObject<USphereComponent>(Target);
			Target->SetRootComponent(Shape);
			Shape->SetSphereRadius(20.f);
			Shape->SetCollisionObjectType(ECC_Pawn);
			Shape->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			Shape->SetCollisionResponseToAllChannels(ECR_Block);
			Shape->RegisterComponent();
			Target->SetActorLocation(FVector(100.f, 0.f, 0.f));
		}

		~FMeleeDetectionWorld() { World->DestroyWorld(false); }
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeDetectionRoutingTest, "Hodge.Combat.DetectionRouting",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FHodgeDetectionRoutingTest::RunTest(const FString& Parameters)
{
	FMeleeDetectionWorld Context;
	const FGuid FirstExecution = FGuid::NewGuid();
	const FGuid SecondExecution = FGuid::NewGuid();
	const uint64 First = Context.Combat->CreateDetectionSession(FirstExecution, 0, Context.Request);
	const uint64 Second = Context.Combat->CreateDetectionSession(SecondExecution, 0, Context.Request);
	TestTrue(TEXT("Concurrent executions get different nonzero handles"), First != 0 && Second != 0 && First != Second);
	FHodgeHitDetectionBatch Batch;
	TestFalse(TEXT("Another execution cannot sample this handle"), Context.Combat->SampleDetection(First, SecondExecution, Batch));
	TestTrue(TEXT("Detection works without a source ASC or CombatSet"), Context.Combat->SampleDetection(First, FirstExecution, Batch));
	TestTrue(TEXT("A target without ASC remains in geometric results"), Batch.Hits.ContainsByPredicate(
		[&Context](const FHitResult& Hit) { return Hit.GetActor() == Context.Target; }));
	TestEqual(TEXT("First sample identifies its execution"), Batch.ExecutionId, FirstExecution);
	TestEqual(TEXT("Create does not consume the first sample sequence"), Batch.SampleSequence, 1);
	Context.Combat->EndDetectionSession(First, SecondExecution);
	TestTrue(TEXT("Wrong execution cannot close the session"), Context.Combat->IsDetectionSessionValid(First, FirstExecution));
	auto* Task = NewObject<UHodgeAbilityTask_WaitHitResults>();
	FHodgeMeleeTestAccess::Release(Task, Context.Combat, FirstExecution);
	FHodgeMeleeTestAccess::Release(Task, Context.Combat, FirstExecution);
	TestFalse(TEXT("Repeated Task cleanup releases its execution"), Context.Combat->IsDetectionSessionValid(First, FirstExecution));
	TestTrue(TEXT("Task cleanup preserves another execution"), Context.Combat->IsDetectionSessionValid(Second, SecondExecution));
	Context.Owner->GetRootComponent()->DestroyComponent();
	TestFalse(TEXT("Destroyed source cannot return an old batch"), Context.Combat->SampleDetection(Second, SecondExecution, Batch));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeMeleeHitHistoryTest, "Hodge.Combat.MeleeHitHistory",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FHodgeMeleeHitHistoryTest::RunTest(const FString& Parameters)
{
	auto* Target = NewObject<UAbilitySystemComponent>();
	auto* Other = NewObject<UAbilitySystemComponent>();
	TSharedPtr<FHodgeMeleeHitHistory> First = MakeShared<FHodgeMeleeHitHistory>();
	TSharedPtr<FHodgeMeleeHitHistory> SameGroup = First;
	FHodgeMeleeHitHistory IndependentWindow;
	TestTrue(TEXT("First contact is eligible"), First->CanHit(Target, 10.0, 0.f));
	First->RecordHit(Target, 10.0);
	TestFalse(TEXT("Window default grants one opportunity"), First->CanHit(Target, 20.0, 0.f));
	TestFalse(TEXT("Shared group observes the same consumed opportunity"), SameGroup->CanHit(Target, 20.0, 0.f));
	TestTrue(TEXT("Independent window has its own opportunity"), IndependentWindow.CanHit(Target, 10.0, 0.f));
	TestTrue(TEXT("Another ASC is independently eligible"), First->CanHit(Other, 10.0, 0.f));
	TestFalse(TEXT("Repeat interval blocks early contact"), First->CanHit(Target, 10.25, 0.5f));
	TestTrue(TEXT("Repeat interval permits its boundary"), First->CanHit(Target, 10.5, 0.5f));
	TestFalse(TEXT("A reversed sample clock cannot bypass the interval"), First->CanHit(Target, 9.0, 0.5f));
	TestFalse(TEXT("Invalid interval is rejected"), First->CanHit(Target, 11.0, -1.f));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeMeleeSpecContextTest, "Hodge.Combat.MeleeSpecContext",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FHodgeMeleeSpecContextTest::RunTest(const FString& Parameters)
{
	FMeleeDetectionWorld Context;
	auto* ASC = NewObject<UAbilitySystemComponent>(Context.Owner);
	ASC->RegisterComponent();
	ASC->InitAbilityActorInfo(Context.Owner, Context.Owner);
	ASC->AddAttributeSetSubobject(NewObject<UHodgeCombatSet>(ASC));
	if (!FHodgeGameplayEffectContext::ExtractEffectContext(ASC->MakeEffectContext()))
	{
		AddError(TEXT("Melee spec tests require the configured HodgeAbilitySystemGlobals."));
		return false;
	}
	// 使用实际存在的来源组件，UE 5.5 不允许实例化抽象的 UObject 基类。
	UObject* SourceObject = Context.Owner->GetRootComponent();
	FGameplayAbilitySpec Granted(UHodgeGameplayAbility_Melee::StaticClass(), 2, INDEX_NONE, SourceObject);
	Granted.GetDynamicSpecSourceTags().AddTag(HodgeGameplayTags::Status_Attack_Active);
	Granted.SetByCallerTagMagnitudes.Add(HodgeGameplayTags::SetByCaller_Damage, 23.f);
	const FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Granted);
	FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromHandle(Handle);
	auto* Ability = AbilitySpec ? Cast<UHodgeGameplayAbility_Melee>(AbilitySpec->GetPrimaryInstance()) : nullptr;
	if (!TestNotNull(TEXT("Granted melee has an actor instance"), Ability)) { return false; }
	FHodgeHitDetectionBatch Batch;
	Batch.SourceOrigin = FVector(50.f, 20.f, 10.f);
	FHodgeHitWindowBinding Binding;
	Binding.DamageMultiplier = 1.5f;
	FHitResult FirstHit;
	FirstHit.ImpactPoint = FVector(100.f, 0.f, 0.f);
	FHitResult SecondHit;
	SecondHit.ImpactPoint = FVector(200.f, 0.f, 0.f);
	TestFalse(TEXT("Missing explicit effect does not build a fallback spec"),
		FHodgeMeleeTestAccess::Build(Ability, ASC, Handle, Batch, FirstHit, Binding).IsValid());
	// 此用例只验证 Spec 上下文隔离，显式指定基础 GE，不依赖伤害资产或隐式默认值。
	Binding.DamageEffect = UGameplayEffect::StaticClass();
	const FGameplayEffectSpecHandle First = FHodgeMeleeTestAccess::Build(Ability, ASC, Handle, Batch, FirstHit, Binding);
	const FGameplayEffectSpecHandle Second = FHodgeMeleeTestAccess::Build(Ability, ASC, Handle, Batch, SecondHit, Binding);
	if (!TestTrue(TEXT("Both hits build an independent spec"), First.IsValid() && Second.IsValid())) { return false; }
	TestEqual(TEXT("Granted SetByCaller survives GA construction"), First.Data->GetSetByCallerMagnitude(HodgeGameplayTags::SetByCaller_Damage), 23.f);
	TestEqual(TEXT("Window multiplier overrides its own parameter"), First.Data->GetSetByCallerMagnitude(HodgeGameplayTags::SetByCaller_DamageMultiplier), 1.5f);
	TestTrue(TEXT("Dynamic ability source tag is captured"), First.Data->CapturedSourceTags.GetSpecTags().HasTagExact(HodgeGameplayTags::Status_Attack_Active));
	TestTrue(TEXT("Granted SourceObject survives context construction"), First.Data->GetContext().GetSourceObject() == SourceObject);
	TestTrue(TEXT("Origin records the detection source"), First.Data->GetContext().GetOrigin().Equals(Batch.SourceOrigin));
	const FHitResult* FirstContextHit = First.Data->GetContext().GetHitResult();
	const FHitResult* SecondContextHit = Second.Data->GetContext().GetHitResult();
	TestTrue(TEXT("Each target has its own HitResult storage"), FirstContextHit && SecondContextHit && FirstContextHit != SecondContextHit);
	TestTrue(TEXT("Building the second target preserves the first hit"), FirstContextHit && FirstContextHit->ImpactPoint.Equals(FirstHit.ImpactPoint));
	TestTrue(TEXT("Second context retains its own hit"), SecondContextHit && SecondContextHit->ImpactPoint.Equals(SecondHit.ImpactPoint));
	return true;
}

#endif
