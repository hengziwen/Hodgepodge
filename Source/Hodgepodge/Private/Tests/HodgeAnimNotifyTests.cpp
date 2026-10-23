/** @file HodgeAnimNotifyTests.cpp
 * @brief 实现动画通知自动化测试相关的类型、接口与现有处理流程。
 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Animation/HodgeCombatAnimNotifies.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h"
#include "Animation/AnimNotifyQueue.h"
#include "Animation/ActiveMontageInstanceScope.h"
#include "Misc/AutomationTest.h"
#include <limits>

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeNotifyConfigurationTest, "Hodge.AnimNotify.Configuration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeNotifyRelevantFieldsTest, "Hodge.AnimNotify.RelevantFields",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 隐藏字段不阻断有效来源，实际使用的形状、目标和伤害参数仍严格校验。 */
bool FHodgeNotifyRelevantFieldsTest::RunTest(const FString& Parameters)
{
	const float NaN = std::numeric_limits<float>::quiet_NaN();
	FHodgeAnimHitConfig Hit;
	Hit.Source = EHodgeAnimHitSource::AvatarRoot;
	Hit.Shape = EHodgeHitShape::Sphere;
	Hit.BoxHalfExtent = FVector::ZeroVector;
	Hit.CapsuleHalfHeight = NaN;
	Hit.MaxTargetDistance = -1.f;
	TArray<FText> Errors;
	TestTrue(TEXT("Sphere ignores hidden box, capsule and unselected target fields"), Hit.Validate(Errors));
	Hit.Radius = NaN; Errors.Reset();
	TestFalse(TEXT("Sphere still rejects its own invalid radius"), Hit.Validate(Errors));
	Hit.Shape = EHodgeHitShape::Box; Hit.BoxHalfExtent = FVector(30.f); Errors.Reset();
	TestTrue(TEXT("Box ignores unused radius and capsule height"), Hit.Validate(Errors));
	Hit.Source = EHodgeAnimHitSource::EquippedWeapon;
	Hit.BoxHalfExtent = FVector::ZeroVector;
	Hit.LocalTransform.SetScale3D(FVector(2.f));
	Hit.MaxAnchorDistance = -1.f; Errors.Reset();
	TestTrue(TEXT("Weapon uses registered geometry rather than hidden notification values"), Hit.Validate(Errors));
	Hit.TargetPolicy = EHodgeHitTargetPolicy::LockedTargetInVolume;
	Hit.TargetKey = TEXT("LockedTarget"); Errors.Reset();
	TestFalse(TEXT("Selected target still requires a valid visible distance"), Hit.Validate(Errors));
	Hit.MaxTargetDistance = 1000.f; Hit.DamageScale = NaN; Errors.Reset();
	TestFalse(TEXT("Unrelated geometry never weakens damage multiplier validation"), Hit.Validate(Errors));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeNotifyContextTest, "Hodge.AnimNotify.MontageIdentity",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
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
