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

struct FHodgeWeaponPresentationTestAccess
{
	static void AttachActor(UHodgeWeaponInstance* Weapon, AHodgeWeaponPresentationActor* Actor) { Actor->BindWeapon(Weapon); }
	static void SetPhase(UHodgeWeaponInstance* Weapon, EHodgeWeaponPresentationPhase Phase) { Weapon->SetPresentationPhase(Phase, 0); }
	static void Die(UHodgeWeaponInstance* Weapon, AActor* Owner) { Weapon->OnDeathStarted(Owner); }
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeWeaponRequestsTest, "Hodge.WeaponPresentation.RequestsAndLifecycle",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeWeaponRequestsTest::RunTest(const FString& Parameters)
{
	const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(false)
		.CreateNavigation(false).CreateAISystem(false);
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
	auto* Pawn = World->SpawnActor<ACharacter>();
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
bool FHodgeWeaponGeometryTest::RunTest(const FString& Parameters)
{
	const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(false)
		.CreateNavigation(false).CreateAISystem(false);
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
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
#endif
