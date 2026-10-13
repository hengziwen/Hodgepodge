/** @file HodgeWeaponPresentationTests.cpp
 * @brief 实现武器表现自动化测试相关的类型、接口与现有处理流程。
 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Equipment/HodgeWeaponInstance.h"
#include "Equipment/HodgeWeaponPresentationActor.h"
#include "Equipment/HodgeWeaponPresentationProfile.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "Engine/SkeletalMesh.h"
#include "Misc/AutomationTest.h"
#include "TimerManager.h"
#include "Component/HodgeCombatComponentBase.h"
#include "Animation/HodgeCombatAnimNotifies.h"

/** 保存武器表现测试Access的数据和接口契约。 */
struct FHodgeWeaponPresentationTestAccess
{
	/** 把生成的表现Actor附着到指定组件或插槽。
	 * @param Weapon 武器，由调用方传入。
	 * @param Actor 本次操作关联的Actor。
	 */
	static void AttachActor(UHodgeWeaponInstance* Weapon, AHodgeWeaponPresentationActor* Actor) { Actor->BindWeapon(Weapon); }
	/** 设置Phase。
	 * @param Weapon 武器，由调用方传入。
	 * @param Phase Phase，由调用方传入。
	 */
	static void SetPhase(UHodgeWeaponInstance* Weapon, EHodgeWeaponPresentationPhase Phase) { Weapon->SetPresentationPhase(Phase, 0); }
	/** 执行当前实验对象的死亡流程。
	 * @param Weapon 武器，由调用方传入。
	 * @param Owner 当前操作关联的所属对象。
	 */
	static void Die(UHodgeWeaponInstance* Weapon, AActor* Owner) { Weapon->OnDeathStarted(Owner); }
	/** 调整测试快照的起始秒数以验证实际材质公式，不修改生产计时器。 */
	static void Elapse(UHodgeWeaponInstance* Weapon, double Seconds) { Weapon->LocalPresentation.StartTime -= Seconds; }
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeWeaponRequestsTest, "Hodge.WeaponPresentation.RequestsAndLifecycle",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeWeaponRequestsTest::RunTest(const FString& Parameters)
{
	const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(false)
		.CreateNavigation(false).CreateAISystem(false);
	// 本次操作所在的世界。
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
	// 当前操作关联的Pawn角色。
	auto* Pawn = World->SpawnActor<ACharacter>();
	auto* Combat = NewObject<UHodgeCombatComponentBase>(Pawn);
	Combat->RegisterComponent();
	const auto OriginalPosePolicy = Pawn->GetMesh()->VisibilityBasedAnimTickOption;
	auto* Weapon = NewObject<UHodgeWeaponInstance>(Pawn);
	Weapon->PresentationProfile = NewObject<UHodgeWeaponPresentationProfile>(Weapon);
	Weapon->PresentationProfile->ReturnGraceSeconds = .06f;
	Weapon->OnEquipped();
	Weapon->OnEquipped();
	TestTrue(TEXT("Equip starts hidden"), Weapon->GetPresentationState().Phase == EHodgeWeaponPresentationPhase::Hidden);
	const FGuid FirstExecution = FGuid::NewGuid();
	const FGuid SecondExecution = FGuid::NewGuid();
	const FGuid First = Weapon->AcquireHandUse(FirstExecution, 0, 12);
	TestTrue(TEXT("Hand request gets valid handle"), First.IsValid());
	TestTrue(TEXT("Server evaluates hand sockets even outside the view"), Pawn->GetMesh()->VisibilityBasedAnimTickOption == EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones);
	TestTrue(TEXT("Repeated window acquire is idempotent"), Weapon->AcquireHandUse(FirstExecution, 0, 12) == First);
	const FGuid Second = Weapon->AcquireHandUse(SecondExecution, 0, 13);
	const FGuid AbilityPoseLease = Combat->AcquirePoseLease();
	TestEqual(TEXT("Weapon and ability share one pose-policy owner"), Combat->GetPoseLeaseCount(), 2);
	TestEqual(TEXT("Two independent executions own two requests"), Weapon->GetHandUseCount(), 2);
	Weapon->ReleaseHandUse(First);
	Weapon->ReleaseHandUse(First);
	TestEqual(TEXT("Repeated release cannot steal another request"), Weapon->GetHandUseCount(), 1);
	TestTrue(TEXT("Other execution keeps weapon in hand"), Weapon->GetPresentationState().Phase == EHodgeWeaponPresentationPhase::Hand);
	const int32 DrawRevision = Weapon->GetPresentationState().Revision;
	Weapon->ReleaseHandUse(Second);
	TestTrue(TEXT("Last release leaves a short return grace"), Weapon->GetPresentationState().Phase == EHodgeWeaponPresentationPhase::Hand);
	const FGuid Third = Weapon->AcquireHandUse(SecondExecution, 1, 14);
	TestTrue(TEXT("New window gets a distinct handle"), Third.IsValid() && Third != Second);
	TestEqual(TEXT("Continuous combo does not restart draw"), Weapon->GetPresentationState().Revision, DrawRevision);
	++GFrameCounter;
	World->GetTimerManager().Tick(.1f);
	TestTrue(TEXT("Canceled idle timer cannot return a new attack"), Weapon->GetPresentationState().Phase == EHodgeWeaponPresentationPhase::Hand);
	Weapon->ReleaseHandUsesForExecution(FirstExecution);
	TestEqual(TEXT("Old execution cleanup leaves new request"), Weapon->GetHandUseCount(), 1);
	Weapon->ReleaseHandUsesForExecution(SecondExecution);
	TestEqual(TEXT("Execution cleanup closes all its windows"), Weapon->GetHandUseCount(), 0);
	TestTrue(TEXT("Body ability lease survives the final weapon release"), Pawn->GetMesh()->VisibilityBasedAnimTickOption == EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones);
	Combat->ReleasePoseLease(AbilityPoseLease);
	Combat->ReleasePoseLease(AbilityPoseLease);
	TestTrue(TEXT("Last request restores original owner pose policy"), Pawn->GetMesh()->VisibilityBasedAnimTickOption == OriginalPosePolicy);
	++GFrameCounter;
	World->GetTimerManager().Tick(.1f);
	TestTrue(TEXT("Final cleanup begins return"), Weapon->GetPresentationState().Phase == EHodgeWeaponPresentationPhase::Returning);
	const auto ReturningRevision = Weapon->GetPresentationState().Revision;
	Weapon->AcquireHandUse(FGuid::NewGuid(), 0, 15);
	TestTrue(TEXT("Attack interrupts return"), Weapon->GetPresentationState().Phase == EHodgeWeaponPresentationPhase::Hand);
	TestTrue(TEXT("New transition advances stale callback version"), Weapon->GetPresentationState().Revision > ReturningRevision);
	++GFrameCounter;
	World->GetTimerManager().Tick(3.f);
	TestTrue(TEXT("Old phase deadlines cannot hide an active attack"), Weapon->GetPresentationState().Phase == EHodgeWeaponPresentationPhase::Hand);
	FHodgeWeaponPresentationTestAccess::Die(Weapon, Pawn);
	TestEqual(TEXT("Death clears requests"), Weapon->GetHandUseCount(), 0);
	TestTrue(TEXT("Death hides weapon"), Weapon->GetPresentationState().Phase == EHodgeWeaponPresentationPhase::Hidden);
	TestFalse(TEXT("Dead weapon rejects new use"), Weapon->AcquireHandUse(FGuid::NewGuid(), 0, 16).IsValid());
	Weapon->OnUnequipped();
	Weapon->OnUnequipped();
	TestFalse(TEXT("Unequipped weapon rejects late callbacks"), Weapon->AcquireHandUse(FGuid::NewGuid(), 0, 17).IsValid());
	World->DestroyWorld(false);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeWeaponGeometryTest, "Hodge.WeaponPresentation.GeometrySeparation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeWeaponGeometryTest::RunTest(const FString& Parameters)
{
	const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(false)
		.CreateNavigation(false).CreateAISystem(false);
	// 本次操作所在的世界。
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
	// 当前操作关联的Pawn角色。
	auto* Pawn = World->SpawnActor<ACharacter>();
	auto* Weapon = NewObject<UHodgeWeaponInstance>(Pawn);
	auto* Profile = NewObject<UHodgeWeaponPresentationProfile>(Weapon);
	Profile->WeaponMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/Wuwa/Weapon/Sword_Qiuyuan.Sword_Qiuyuan"));
	auto* CharacterMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/Main/Character/Hero/Anim/Model/SKM_Pover_LyraLab.SKM_Pover_LyraLab"));
	if (!TestNotNull(TEXT("Character socket fixture exists"), CharacterMesh)) { World->DestroyWorld(false); return false; }
	Pawn->GetMesh()->SetSkeletalMeshAsset(CharacterMesh);
	if (!TestTrue(TEXT("Character has the default back socket"), Pawn->GetMesh()->DoesSocketExist(Profile->BackSocket))) { World->DestroyWorld(false); return false; }
	Profile->HandSocket = NAME_None;
	Profile->BackTransform = FTransform(FRotator(10.f, 25.f, 5.f), FVector(4.f, 8.f, 12.f));
	Profile->HoverAmplitude = 0.f;
	Weapon->PresentationProfile = Profile;
	Weapon->OnEquipped();
	auto* Visual = World->SpawnActor<AHodgeWeaponPresentationActor>();
	Visual->SetOwner(Pawn);
	FHodgeWeaponPresentationTestAccess::AttachActor(Weapon, Visual);
	const FVector HandLocation = Visual->GetDetectionMesh()->GetComponentLocation();
	FHodgeWeaponPresentationTestAccess::SetPhase(Weapon, EHodgeWeaponPresentationPhase::Hovering);
	Visual->RefreshPresentation();
	TestTrue(TEXT("Back pose does not move logical hit mesh"), Visual->GetDetectionMesh()->GetComponentLocation().Equals(HandLocation));
	TestFalse(TEXT("Back pose moves only visible mesh"), Visual->GetVisualMesh()->GetComponentLocation().Equals(HandLocation));
	TestTrue(TEXT("Visible mesh attaches to the character Mesh"), Visual->GetVisualMesh()->GetAttachParent() == Pawn->GetMesh());
	TestEqual(TEXT("Visible mesh attaches to the configured back socket"), Visual->GetVisualMesh()->GetAttachSocketName(), Profile->BackSocket);
	TestTrue(TEXT("Back offset uses socket space"), Visual->GetVisualTransform().Equals(Profile->BackTransform * Pawn->GetMesh()->GetSocketTransform(Profile->BackSocket), .01f));
	Pawn->SetActorTransform(FTransform(FRotator(0.f, 70.f, 0.f), FVector(120.f, 80.f, 30.f)));
	Pawn->GetMesh()->SetRelativeTransform(FTransform(FRotator(5.f, -35.f, 0.f), FVector(0.f, 0.f, -80.f)));
	Visual->RefreshPresentation();
	TestTrue(TEXT("Back pose follows the moving character Mesh"), Visual->GetVisualTransform().Equals(Profile->BackTransform * Pawn->GetMesh()->GetSocketTransform(Profile->BackSocket), .01f));
	FHodgeWeaponPresentationTestAccess::SetPhase(Weapon, EHodgeWeaponPresentationPhase::Fading);
	Visual->RefreshPresentation();
	TestEqual(TEXT("Fade keeps the back socket attachment"), Visual->GetVisualMesh()->GetAttachSocketName(), Profile->BackSocket);
	FHodgeWeaponPresentationTestAccess::SetPhase(Weapon, EHodgeWeaponPresentationPhase::Hand);
	Visual->RefreshPresentation();
	TestTrue(TEXT("Attack returns the visible mesh to the detection mesh"), Visual->GetVisualMesh()->GetAttachParent() == Visual->GetDetectionMesh());
	TestTrue(TEXT("Hand pose matches the logical source after a return"), Visual->GetVisualTransform().Equals(Visual->GetDetectionMesh()->GetComponentTransform(), .01f));
	Profile->BackSocket = TEXT("MissingBackSocketForTest");
	FHodgeWeaponPresentationTestAccess::SetPhase(Weapon, EHodgeWeaponPresentationPhase::Hovering);
	Visual->RefreshPresentation();
	TestTrue(TEXT("Missing socket falls back to the hand, not the Pawn root"), Visual->GetVisualTransform().Equals(Visual->GetDetectionMesh()->GetComponentTransform(), .01f));
	FHodgeWeaponPresentationTestAccess::SetPhase(Weapon, EHodgeWeaponPresentationPhase::Hidden);
	Visual->RefreshPresentation();
	TestFalse(TEXT("Hidden actor does not tick cosmetic motion"), Visual->IsActorTickEnabled());
	TestFalse(TEXT("Hidden appearance does not destroy weapon actor"), Visual->IsActorBeingDestroyed());
	Weapon->OnUnequipped();
	World->DestroyWorld(false);
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeWeaponEndBehaviorTest, "Hodge.WeaponPresentation.NotifyEndBehavior",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 验证通知配置、原地淡出、重新攻击、重叠请求及确定的取消顺序。 */
bool FHodgeWeaponEndBehaviorTest::RunTest(const FString& Parameters)
{
	const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(false)
		.CreateNavigation(false).CreateAISystem(false);
	auto* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
	if (!TestNotNull(TEXT("End behavior test world"), World)) { return false; }
	auto* Pawn = World->SpawnActor<ACharacter>();
	auto* Weapon = NewObject<UHodgeWeaponInstance>(Pawn);
	auto* Profile = NewObject<UHodgeWeaponPresentationProfile>(Weapon);
	Profile->WeaponMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/Wuwa/Weapon/Sword_Qiuyuan.Sword_Qiuyuan"));
	Pawn->GetMesh()->SetSkeletalMeshAsset(LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/Main/Character/Hero/Anim/Model/SKM_Pover_LyraLab.SKM_Pover_LyraLab")));
	Profile->HandSocket = NAME_None;
	Profile->FadeSeconds = .4f;
	Weapon->PresentationProfile = Profile;
	Weapon->OnEquipped();
	auto* Visual = World->SpawnActor<AHodgeWeaponPresentationActor>();
	Visual->SetOwner(Pawn);
	Visual->BindWeapon(Weapon);
	auto* Notify = NewObject<UHodgeAnimNotifyState_WeaponHand>();
	TestTrue(TEXT("Legacy notifications default to back return"), Notify->EndBehavior == EHodgeWeaponHandEndBehavior::ReturnToBack);
	const FGuid Execution = FGuid::NewGuid();
	const FGuid Direct = Weapon->AcquireHandUse(Execution, 0, 50, EHodgeWeaponHandEndBehavior::FadeAtHand);
	TestTrue(TEXT("Duplicate acquire preserves the original request"), Weapon->AcquireHandUse(Execution, 0, 50) == Direct);
	FHodgeWeaponPresentationTestAccess::Elapse(Weapon, .1);
	Weapon->ReleaseHandUse(Direct);
	TestTrue(TEXT("Direct release enters fade without return grace"), Weapon->GetPresentationState().Phase == EHodgeWeaponPresentationPhase::Fading);
	TestTrue(TEXT("Direct release records replicated hand anchor"), Weapon->GetPresentationState().EndBehavior == EHodgeWeaponHandEndBehavior::FadeAtHand);
	Visual->RefreshPresentation();
	TestTrue(TEXT("Fade remains attached to the logical hand mesh"), Visual->GetVisualMesh()->GetAttachParent() == Visual->GetDetectionMesh());
	TestTrue(TEXT("Direct fade does not move toward the back"), Visual->GetVisualTransform().Equals(Visual->GetDetectionMesh()->GetComponentTransform(), .01f));
	FHodgeWeaponPresentationTestAccess::Elapse(Weapon, .2);
	Visual->RefreshPresentation();
	TestEqual(TEXT("Direct fade midpoint opacity"), Visual->GetVisibilityAmount(), .5f);
	const FGuid Next = Weapon->AcquireHandUse(FGuid::NewGuid(), 1, 51, EHodgeWeaponHandEndBehavior::FadeAtHand);
	Visual->RefreshPresentation();
	TestEqual(TEXT("New attack starts from current opacity"), Visual->GetVisibilityAmount(), .5f);
	++GFrameCounter; World->GetTimerManager().Tick(1.f);
	TestTrue(TEXT("Old fade deadline cannot hide new hand request"), Weapon->GetPresentationState().Phase == EHodgeWeaponPresentationPhase::Hand);
	Weapon->ReleaseHandUse(Direct);
	TestEqual(TEXT("Old notify release cannot steal next request"), Weapon->GetHandUseCount(), 1);
	Weapon->ReleaseHandUse(Next);
	++GFrameCounter; World->GetTimerManager().Tick(.5f);
	TestTrue(TEXT("Direct fade completes on its own deadline"), Weapon->GetPresentationState().Phase == EHodgeWeaponPresentationPhase::Hidden);
	Visual->RefreshPresentation();
	TestFalse(TEXT("Completed fade is invisible"), Visual->GetVisualMesh()->IsVisible());
	TestTrue(TEXT("Completed direct fade retains hand attachment"), Visual->GetVisualMesh()->GetAttachParent() == Visual->GetDetectionMesh());
	const FGuid MixedExecution = FGuid::NewGuid();
	Weapon->AcquireHandUse(MixedExecution, 2, 52, EHodgeWeaponHandEndBehavior::ReturnToBack);
	Weapon->AcquireHandUse(MixedExecution, 3, 52, EHodgeWeaponHandEndBehavior::FadeAtHand);
	Weapon->ReleaseHandUsesForExecution(MixedExecution);
	TestTrue(TEXT("Bulk cancel uses newest request deterministically"), Weapon->GetPresentationState().Phase == EHodgeWeaponPresentationPhase::Fading && Weapon->GetPresentationState().EndBehavior == EHodgeWeaponHandEndBehavior::FadeAtHand);
	const FGuid OlderBack = Weapon->AcquireHandUse(FGuid::NewGuid(), 4, 53);
	const FGuid LaterDirect = Weapon->AcquireHandUse(FGuid::NewGuid(), 5, 54, EHodgeWeaponHandEndBehavior::FadeAtHand);
	Weapon->ReleaseHandUse(LaterDirect);
	TestTrue(TEXT("Another active request prevents hiding"), Weapon->GetPresentationState().Phase == EHodgeWeaponPresentationPhase::Hand);
	Weapon->ReleaseHandUse(OlderBack);
	++GFrameCounter; World->GetTimerManager().Tick(.1f);
	TestTrue(TEXT("Last naturally released request controls the policy"), Weapon->GetPresentationState().Phase == EHodgeWeaponPresentationPhase::Returning && Weapon->GetPresentationState().EndBehavior == EHodgeWeaponHandEndBehavior::ReturnToBack);
	TestFalse(TEXT("Invalid policy refuses new request"), Weapon->AcquireHandUse(FGuid::NewGuid(), 6, 55, EHodgeWeaponHandEndBehavior(255)).IsValid());
	Weapon->OnUnequipped();
	World->DestroyWorld(false);
	return true;
}
#endif
