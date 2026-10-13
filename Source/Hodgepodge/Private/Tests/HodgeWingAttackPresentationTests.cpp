/** @file HodgeWingAttackPresentationTests.cpp
 * @brief 将攻击限定翅膀的单元与世界集成检查关联到 WING 需求。
 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Animation/HodgeWingProfile.h"
#include "Animation/HodgeAnimNotifyState_WingAction.h"
#include "Component/HodgeWingComponent.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Engine/World.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace HodgeWingAttackTests
{
	/** 在测试副本上配置窗口，不修改实际角色动画。 */
	void AddWindow(UAnimSequenceBase* Source, float Begin, float End)
	{
		FAnimNotifyEvent Event;
		Event.NotifyStateClass = NewObject<UHodgeAnimNotifyState_WingAction>(Source);
		Event.Link(Source, Begin);
		Event.SetDuration(End - Begin);
		Event.EndLink.Link(Source, End);
		Source->Notifies.Add(Event);
	}

	/** 加载已导入的角色与翼动画，返回拥有独立通知副本的映射。 */
	FHodgeWingAnimationBinding MakeBinding()
	{
		FHodgeWingAnimationBinding Binding;
		auto* Source = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Wuwa/Anim/AM_Attack04.AM_Attack04"));
		Binding.SourceAnimation = Source ? DuplicateObject<UAnimSequence>(Source, GetTransientPackage()) : nullptr;
		Binding.WingAnimation = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Wuwa/Wing/BaseAnim/B_Attack04.B_Attack04"));
		if (Binding.SourceAnimation)
		{
			Binding.SourceAnimation->Notifies.Reset();
			AddWindow(Binding.SourceAnimation, .2f, 1.f);
		}
		return Binding;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeWingFadeStateTest, "Hodge.Wing.FadeState",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 验证 WING-13 的重复关闭、时间边界和新攻击覆盖。 */
bool FHodgeWingFadeStateTest::RunTest(const FString& Parameters)
{
	FHodgeWingFadeState Fade;
	Fade.BeginFade(.35f);
	TestFalse(TEXT("WING-13 Hidden close does not start fade"), Fade.bFading);
	Fade.Show();
	Fade.BeginFade(.4f);
	Fade.Advance(.2f);
	TestEqual(TEXT("WING-13 Midpoint visibility"), Fade.Amount, .5f);
	Fade.BeginFade(.4f);
	Fade.Advance(NAN);
	Fade.Advance(-1.f);
	TestEqual(TEXT("WING-13 Duplicate close and invalid step preserve progress"), Fade.Elapsed, .2f);
	Fade.Show();
	TestTrue(TEXT("WING-13 New attack cancels old fade"), !Fade.bFading && Fade.Amount == 1.f);
	Fade.BeginFade(.4f);
	Fade.Advance(10.f);
	TestTrue(TEXT("WING-13 Large step completes fade"), Fade.Amount == 0.f && !Fade.bFading);
	Fade.Show();
	Fade.BeginFade(0.f);
	TestEqual(TEXT("WING-13 Zero duration hides immediately"), Fade.Amount, 0.f);
	Fade.Show();
	Fade.BeginFade(NAN);
	TestEqual(TEXT("WING-13 Invalid duration fails hidden"), Fade.Amount, 0.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeWingAnimationMappingTest, "Hodge.Wing.AnimationMapping",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 验证 WING-02/03/04/07/08，Parameters 为自动化框架参数，失败由断言记录。 */
bool FHodgeWingAnimationMappingTest::RunTest(const FString& Parameters)
{
	auto Binding = HodgeWingAttackTests::MakeBinding();
	if (!TestNotNull(TEXT("WING-02 Source asset"), Binding.SourceAnimation.Get()) || !TestNotNull(TEXT("WING-02 Wing asset"), Binding.WingAnimation.Get())) { return false; }
	TestFalse(TEXT("WING-03 Before begin hidden"), Binding.IsWindowOpen(.19f));
	TestTrue(TEXT("WING-03 Begin inclusive"), Binding.IsWindowOpen(.2f));
	TestFalse(TEXT("WING-03 End exclusive"), Binding.IsWindowOpen(1.f));
	HodgeWingAttackTests::AddWindow(Binding.SourceAnimation, .8f, 1.4f);
	TestTrue(TEXT("WING-03 Overlapping window keeps visibility"), Binding.IsWindowOpen(1.1f));
	TestFalse(TEXT("WING-08 Nonfinite time hidden"), Binding.IsWindowOpen(NAN));
	Binding.SourceStartTime = .2f;
	Binding.WingStartTime = .1f;
	Binding.PlayRateRatio = 2.f;
	TestEqual(TEXT("WING-04 Source time mapping"), Binding.EvaluateWingTime(.6f), .9f);
	TestEqual(TEXT("WING-04 End clamps instead of looping"), Binding.EvaluateWingTime(100.f), Binding.WingAnimation->GetPlayLength());
	TestEqual(TEXT("WING-04 Early source clamps"), Binding.EvaluateWingTime(0.f), 0.f);
	auto* Profile = NewObject<UHodgeWingProfile>();
	Profile->PlaybackMode = EHodgeWingPlaybackMode::AnimationAsset;
	Profile->Mesh = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/Wuwa/Wing/EDarkNanzhuWingMd10011/Model/EDarkNanzhuWingMd10011.EDarkNanzhuWingMd10011"));
	Profile->MeshOffset.SetScale3D(FVector(.8f));
	Profile->AnimationBindings.Add(Binding);
	TArray<FText> Errors;
	TestTrue(TEXT("WING-07 New skeleton and calibrated uniform scale accepted"), Profile->Validate(Errors));
	Profile->AnimationBindings.Add(Binding);
	Errors.Reset();
	TestFalse(TEXT("WING-08 Duplicate source rejected"), Profile->Validate(Errors));
	Profile->AnimationBindings.SetNum(1);
	Profile->AnimationBindings[0].PlayRateRatio = NAN;
	Errors.Reset();
	TestFalse(TEXT("WING-08 Nonfinite rate rejected"), Profile->Validate(Errors));
	Profile->AnimationBindings[0].PlayRateRatio = 1.f;
	Profile->MeshOffset.SetScale3D(FVector(.8f, 1.f, .8f));
	Errors.Reset();
	TestFalse(TEXT("WING-08 Nonuniform scale rejected"), Profile->Validate(Errors));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeWingPresentationSessionTest, "Hodge.Wing.PresentationSession",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 验证 WING-06，Parameters 为自动化框架参数，返回值表示用例已完成。 */
bool FHodgeWingPresentationSessionTest::RunTest(const FString& Parameters)
{
	FHodgeWingPresentationState State;
	const FGuid FirstExecution = FGuid::NewGuid();
	const auto First = State.Begin(0, 10, FirstExecution);
	TestTrue(TEXT("WING-06 Valid source begins"), First.IsValid());
	TestEqual(TEXT("WING-06 Duplicate begin is idempotent"), State.Begin(0, 10, FirstExecution).Id, First.Id);
	const auto Next = State.Begin(0, 11, FGuid::NewGuid());
	TestFalse(TEXT("WING-06 Old end cannot end next attack"), State.End(First));
	TestEqual(TEXT("WING-06 Next identity survives old end"), State.Handle.Id, Next.Id);
	TestTrue(TEXT("WING-06 Matching end accepted"), State.End(Next));
	TestFalse(TEXT("WING-06 Duplicate end ignored"), State.End(Next));
	const auto BeforeReset = State.Begin(0, 12, FGuid::NewGuid());
	State.Reset();
	TestFalse(TEXT("WING-06 Old generation invalidated"), State.End(BeforeReset));
	TestFalse(TEXT("WING-08 Missing execution rejected"), State.Begin(0, 12, {}).IsValid());
	TestFalse(TEXT("WING-08 Missing source instance rejected"), State.Begin(0, INDEX_NONE, FGuid::NewGuid()).IsValid());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeWingAttackIntegrationTest, "Hodge.Wing.AttackPresentationIntegration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 验证真实世界中的挂接、求值和清理，Parameters 为自动化框架参数。 */
bool FHodgeWingAttackIntegrationTest::RunTest(const FString& Parameters)
{
	const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(true)
		.CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
	if (!TestNotNull(TEXT("WING-01 Test world"), World)) { return false; }
	auto* Character = World->SpawnActor<ACharacter>();
	auto Binding = HodgeWingAttackTests::MakeBinding();
	auto* BodyMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/Wuwa/Model/R2T1PlayerMaleMd10011.R2T1PlayerMaleMd10011"));
	auto* WingAsset = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/Wuwa/Wing/EDarkNanzhuWingMd10011/Model/EDarkNanzhuWingMd10011.EDarkNanzhuWingMd10011"));
	if (!Character || !Binding.SourceAnimation || !BodyMesh || !WingAsset)
	{
		AddError(TEXT("WING-08 Required integration assets missing"));
		World->DestroyWorld(false);
		return false;
	}
	Character->GetMesh()->SetSkeletalMeshAsset(BodyMesh);
	Character->GetMesh()->PlayAnimation(Binding.SourceAnimation, true);
	auto* Profile = NewObject<UHodgeWingProfile>(Character);
	Profile->PlaybackMode = EHodgeWingPlaybackMode::AnimationAsset;
	Profile->Mesh = WingAsset;
	Profile->AttachSocket = TEXT("WingCase");
	Profile->MeshOffset.SetScale3D(FVector(.8f));
	Profile->AnimationBindings.Add(Binding);
	// 原有清理用例保持零时长，淡出用例单独打开配置。
	Profile->FadeOutSeconds = 0.f;
	auto* Wing = NewObject<UHodgeWingComponent>(Character);
	Wing->Profile = Profile;
	Wing->RegisterComponent();
	if (!TestTrue(TEXT("WING-07 Initialize real mesh"), Wing->InitializeWing())) { World->DestroyWorld(false); return false; }
	auto* Mesh = Wing->GetWingMesh();
	TestFalse(TEXT("WING-HIT-02 Invalid execution cannot prepare hit pose"), Wing->PrepareAuthorityHitPose({}));
	TestFalse(TEXT("WING-HIT-02 Presentation-only profile rejects authoritative hits"), Wing->PrepareAuthorityHitPose(FGuid::NewGuid()));
	Profile->bEnableAuthorityHitSampling = true;
	TestFalse(TEXT("WING-HIT-02 Missing active GAS execution rejects hit pose"), Wing->PrepareAuthorityHitPose(FGuid::NewGuid()));
	Profile->bEnableAuthorityHitSampling = false;
	TestTrue(TEXT("WING-01 Initially hidden"), Mesh && !Mesh->IsVisible() && Mesh->bHiddenInGame);
	TestFalse(TEXT("WING-10 Idle wing tick disabled"), Mesh->IsComponentTickEnabled());
	Character->GetMesh()->SetPosition(.5f, false);
	Wing->RefreshPresentation();
	TestTrue(TEXT("WING-05 First frame prepared and shown"), Wing->GetPresentationState().bVisible && Wing->GetPresentationState().PoseEvaluations > 0);
	const auto Point = Mesh->GetSocketTransform(TEXT("Bone_Wing006"), RTS_Component).GetLocation();
	Character->GetMesh()->SetPosition(.7f, false);
	Wing->RefreshPresentation();
	const auto NextPoint = Mesh->GetSocketTransform(TEXT("Bone_Wing006"), RTS_Component).GetLocation();
	TestFalse(TEXT("WING-04 Actual wing bones move"), Point.Equals(NextPoint, .01f));
	TestEqual(TEXT("WING-04 Wing clock equals source clock"), Wing->GetPresentationState().WingTime, .7f);
	const auto Handle = Wing->GetPresentationState().Handle;
	const int32 PausedEvaluations = Wing->GetPresentationState().PoseEvaluations;
	Wing->RefreshPresentation();
	TestEqual(TEXT("WING-04 Paused source retains pose without reevaluation"), Wing->GetPresentationState().PoseEvaluations, PausedEvaluations);
	TestTrue(TEXT("WING-06 Explicit cancellation accepted"), Wing->EndWingAction(Handle));
	Wing->RefreshPresentation();
	TestFalse(TEXT("WING-06 Canceled living source stays hidden"), Wing->GetPresentationState().bVisible);
	Character->GetMesh()->SetPosition(.1f, false);
	Wing->RefreshPresentation();
	Character->GetMesh()->SetPosition(.6f, false);
	Wing->RefreshPresentation();
	TestTrue(TEXT("WING-06 New preview cycle can show"), Wing->GetPresentationState().bVisible);
	TestFalse(TEXT("WING-06 Old handle cannot hide new cycle"), Wing->EndWingAction(Handle));
	Character->GetMesh()->SetPosition(1.5f, false);
	Wing->RefreshPresentation();
	const int32 Evaluations = Wing->GetPresentationState().PoseEvaluations;
	Wing->RefreshPresentation();
	TestFalse(TEXT("WING-03 Outside window hidden"), Wing->GetPresentationState().bVisible);
	TestEqual(TEXT("WING-10 Hidden frame performs no pose evaluation"), Wing->GetPresentationState().PoseEvaluations, Evaluations);
	TestTrue(TEXT("WING-07 Initialization reuses the same mesh"), Wing->InitializeWing() && Wing->GetWingMesh() == Mesh);
	Profile->FadeOutSeconds = .35f;
	Character->GetMesh()->SetPosition(.1f, false);
	Wing->RefreshPresentation();
	Character->GetMesh()->SetPosition(.5f, false);
	Wing->RefreshPresentation();
	const auto FadeHandle = Wing->GetPresentationState().Handle;
	const int32 FadeEvaluations = Wing->GetPresentationState().PoseEvaluations;
	Character->GetMesh()->SetPosition(1.5f, false);
	Wing->RefreshPresentation();
	Wing->RefreshPresentation();
	TestTrue(TEXT("WING-13 Window exit retains rendered last pose"), Wing->IsFadingOut() && Mesh->IsVisible() && Wing->GetVisibilityAmount() == 1.f);
	// 通过组件公开基类入口推进生产 Tick，不使用额外测试时钟。
	static_cast<UActorComponent*>(Wing)->TickComponent(.175f, LEVELTICK_All, nullptr);
	TestEqual(TEXT("WING-13 Rendered midpoint"), Wing->GetVisibilityAmount(), .5f);
	TestEqual(TEXT("WING-13 Fade freezes bones"), Wing->GetPresentationState().PoseEvaluations, FadeEvaluations);
	auto* Material = Cast<UMaterialInstanceDynamic>(Mesh->GetMaterial(0));
	TestNotNull(TEXT("WING-13 Per-character dynamic material"), Material);
	Character->GetMesh()->SetPosition(.1f, false);
	Wing->RefreshPresentation();
	Character->GetMesh()->SetPosition(.5f, false);
	Wing->RefreshPresentation();
	TestTrue(TEXT("WING-13 New window interrupts fade"), !Wing->IsFadingOut() && Wing->GetVisibilityAmount() == 1.f);
	TestFalse(TEXT("WING-13 Old callback cannot cancel new window"), Wing->EndWingAction(FadeHandle));
	TestTrue(TEXT("WING-13 Cancel begins fade"), Wing->EndWingAction(Wing->GetPresentationState().Handle) && Wing->IsFadingOut());
	static_cast<UActorComponent*>(Wing)->TickComponent(.5f, LEVELTICK_All, nullptr);
	TestTrue(TEXT("WING-13 Completed fade disables visibility"), !Mesh->IsVisible() && Mesh->bHiddenInGame && Wing->GetVisibilityAmount() == 0.f);
	Profile->FadeOutSeconds = 0.f;
	// 预览 Character 也能通过自有 ASC 验证死亡及 Avatar 更换。
	auto* ASC = NewObject<UAbilitySystemComponent>(Character);
	Character->AddInstanceComponent(ASC);
	ASC->RegisterComponent();
	ASC->InitAbilityActorInfo(Character, Character);
	Character->GetMesh()->SetPosition(.1f, false);
	Wing->RefreshPresentation();
	Character->GetMesh()->SetPosition(.5f, false);
	Wing->RefreshPresentation();
	ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Death);
	Wing->RefreshPresentation();
	TestFalse(TEXT("WING-06 Death clears active visibility"), Wing->GetPresentationState().bVisible);
	ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Death);
	Character->GetMesh()->SetPosition(.1f, false);
	Wing->RefreshPresentation();
	Character->GetMesh()->SetPosition(.5f, false);
	Wing->RefreshPresentation();
	ASC->InitAbilityActorInfo(Character, World->SpawnActor<ACharacter>());
	Wing->RefreshPresentation();
	TestFalse(TEXT("WING-12 Old avatar cannot retain visibility"), Wing->GetPresentationState().bVisible);
	Character->DispatchBeginPlay();
	auto* Reused = NewObject<UHodgeWingComponent>(Character);
	Reused->Profile = Profile;
	Reused->ExistingMeshComponentName = Mesh->GetFName();
	Reused->RegisterComponent();
	TestTrue(TEXT("WING-11 Reuses calibrated mesh without duplicate"), Reused->InitializeWing() && Reused->GetWingMesh() == Mesh);
	Reused->DestroyComponent();
	TestTrue(TEXT("WING-11 Removing coordinator preserves user mesh"), IsValid(Mesh));
	Mesh->SetVisibility(true);
	Mesh->SetHiddenInGame(false);
	auto* Invalid = NewObject<UHodgeWingComponent>(Character);
	Invalid->ExistingMeshComponentName = Mesh->GetFName();
	AddExpectedError(TEXT("invalid profile/owner/socket"), EAutomationExpectedErrorFlags::Contains, 1);
	TestFalse(TEXT("WING-08 Missing profile rejected"), Invalid->InitializeWing());
	TestTrue(TEXT("WING-08 Invalid reusable mesh fails hidden"), !Mesh->IsVisible() && Mesh->bHiddenInGame);
	World->DestroyWorld(false);
	return true;
}
#endif
