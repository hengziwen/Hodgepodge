#if WITH_DEV_AUTOMATION_TESTS
#include "Animation/HodgeCombatAnimNotifies.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h"
#include "Animation/AnimNotifyQueue.h"
#include "Animation/ActiveMontageInstanceScope.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeNotifyConfigurationTest, "Hodge.AnimNotify.Configuration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeNotifyConfigurationTest::RunTest(const FString& Parameters)
{
	FHodgeAnimHitConfig Hit;
	TArray<FText> Errors;
	TestFalse(TEXT("Body source requires an explicit bone"), Hit.Validate(Errors));
	Hit.BoneOrSocket = TEXT("Bip001LHand"); Errors.Reset();
	TestTrue(TEXT("Direct body source needs no source registration or profile"), Hit.Validate(Errors));
	Hit.AttackPhase = TEXT("Slash01"); Errors.Reset();
	TestFalse(TEXT("Phase without a shared group is rejected"), Hit.Validate(Errors));
	Hit.HitGroup = TEXT("TwoHands"); Errors.Reset();
	TestTrue(TEXT("Explicit phase groups are valid"), Hit.Validate(Errors));
	Hit.Shape = EHodgeHitShape::Capsule; Hit.CapsuleHalfHeight = 1.f; Errors.Reset();
	TestFalse(TEXT("Capsule dimensions are validated before session creation"), Hit.Validate(Errors));
	Hit.CapsuleHalfHeight = 100.f; Hit.LocalTransform.SetScale3D(FVector(2.f)); Errors.Reset();
	TestFalse(TEXT("Scale cannot silently change query dimensions"), Hit.Validate(Errors));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeNotifyContextTest, "Hodge.AnimNotify.MontageIdentity",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeNotifyContextTest::RunTest(const FString& Parameters)
{
	FAnimNotifyEvent Event;
	Event.NotifyStateClass = NewObject<UHodgeAnimNotifyState_GameplayTag>();
	FAnimNotifyEventReference First(&Event, Event.NotifyStateClass);
	First.AddContextData<UE::Anim::FAnimNotifyMontageInstanceContext>(42);
	FAnimNotifyEventReference Copy = First;
	const auto* Context = Copy.GetContextData<UE::Anim::FAnimNotifyMontageInstanceContext>();
	TestTrue(TEXT("End-event reference preserves the originating montage ID"), Context && Context->MontageInstanceID == 42);
	TestNull(TEXT("An unrelated callback has no montage authority"), FAnimNotifyEventReference().GetContextData<UE::Anim::FAnimNotifyMontageInstanceContext>());
	FHodgeHitGroupKey FirstSlash{TEXT("Blade"), TEXT("Slash01")};
	FHodgeHitGroupKey SameSlash{TEXT("Blade"), TEXT("Slash01")};
	FHodgeHitGroupKey NextSlash{TEXT("Blade"), TEXT("Slash02")};
	TestTrue(TEXT("Two sources can share a phase without equal timestamps"), FirstSlash == SameSlash);
	TestFalse(TEXT("A later slash has an independent opportunity"), FirstSlash == NextSlash);
	return true;
}
#endif
