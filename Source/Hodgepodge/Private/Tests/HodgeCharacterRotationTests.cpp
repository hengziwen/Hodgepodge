#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Character/HodgeCombatCharacter.h"
#include "Component/HodgeCharacterMovementComponent.h"
#include "Component/HodgeCharacterRotationComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeCharacterRotationConstraintsTest, "Hodge.Rotation.ConstraintsAndReplay",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FHodgeCharacterRotationConstraintsTest::RunTest(const FString& Parameters)
{
	const UWorld::InitializationValues Initialization = UWorld::InitializationValues().AllowAudioPlayback(false)
		.RequiresHitProxies(false).CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false)
		.ShouldSimulatePhysics(false);
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true,
		ERHIFeatureLevel::Num, &Initialization);
	if (!TestNotNull(TEXT("Test world"), World)) { return false; }
	AHodgeCombatCharacter* Character = World->SpawnActor<AHodgeCombatCharacter>();
	if (!TestNotNull(TEXT("Combat character"), Character))
	{
		World->DestroyWorld(false);
		return false;
	}
	Character->bUseControllerRotationYaw = true;
	Character->SetActorRotation(FRotator(0.f, 30.f, 0.f));
	UHodgeAbilitySystemComponent* ASC = NewObject<UHodgeAbilitySystemComponent>(Character);
	ASC->RegisterComponent();
	ASC->InitAbilityActorInfo(Character, Character);
	UHodgeCharacterRotationComponent* Rotation = Character->GetCharacterRotationComponent();
	Rotation->InitializeWithAbilitySystem(ASC);
	Rotation->InitializeWithAbilitySystem(ASC);
	APlayerController* Controller = World->SpawnActor<APlayerController>();
	Controller->Possess(Character);
	Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Controller, Character);
	ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked);
	ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked);
	TestTrue(TEXT("Two window owners lock yaw"), Rotation->IsYawLocked());
	Character->FaceRotation(FRotator(0.f, 140.f, 0.f), 1.f / 60.f);
	TestTrue(TEXT("Controller rotation is constrained"), FMath::Abs(Character->GetActorRotation().Yaw - 30.f) < 0.01f);
	UHodgeCharacterMovementComponent* Movement = CastChecked<UHodgeCharacterMovementComponent>(Character->GetCharacterMovement());
	Movement->MoveUpdatedComponent(FVector(10.f, 0.f, 0.f), FRotator(0.f, 90.f, 0.f), false);
	TestTrue(TEXT("Final movement/root motion rotation is constrained"), FMath::Abs(Character->GetActorRotation().Yaw - 30.f) < 0.01f);
	TestTrue(TEXT("Translation remains allowed"), Character->GetActorLocation().X > 9.f);
	ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked);
	TestTrue(TEXT("Removing one owner retains lock"), Rotation->IsYawLocked());
	ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked);
	TestFalse(TEXT("Last owner releases lock"), Rotation->IsYawLocked());
	Character->FaceRotation(FRotator(0.f, 140.f, 0.f), 1.f / 60.f);
	TestTrue(TEXT("Release advances with bounded recovery"), Character->GetActorRotation().Yaw > 30.f && Character->GetActorRotation().Yaw < 37.f);

	FNetworkPredictionData_Client_Character* Prediction = Movement->GetPredictionData_Client_Character();
	FSavedMovePtr UnlockedMove = Prediction->AllocateNewMove();
	UnlockedMove->SetMoveFor(Character, 1.f / 60.f, FVector::ZeroVector, *Prediction);
	ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked);
	FSavedMovePtr LockedMove = Prediction->AllocateNewMove();
	LockedMove->SetMoveFor(Character, 1.f / 60.f, FVector::ZeroVector, *Prediction);
	TestFalse(TEXT("Moves do not combine across lock boundary"), UnlockedMove->CanCombineWith(LockedMove, Character, 0.125f));
	Character->bClientUpdating = true;
	UnlockedMove->PrepMoveFor(Character);
	TestFalse(TEXT("Replay reads historical state, not live lock"), Rotation->IsYawLocked());
	Character->bClientUpdating = false;
	Rotation->ClearMoveReplayState();
	TestTrue(TEXT("Live lock survives replay"), Rotation->IsYawLocked());

	Rotation->UninitializeFromAbilitySystem();
	TestEqual(TEXT("Unbind does not remove shared ASC tag"), ASC->GetTagCount(HodgeGameplayTags::Status_Rotation_Locked), 1);
	TestFalse(TEXT("Unbound Pawn is not left locked"), Rotation->IsYawLocked());
	ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked);
	ASC->AddLooseGameplayTag(TAG_Gameplay_MovementStopped);
	Rotation->InitializeWithAbilitySystem(ASC);
	TestTrue(TEXT("Late binding reads existing MovementStopped constraint"), Rotation->IsYawLocked());
	Rotation->UninitializeFromAbilitySystem();
	ASC->RemoveLooseGameplayTag(TAG_Gameplay_MovementStopped);
	World->DestroyWorld(false);
	return true;
}

#endif
