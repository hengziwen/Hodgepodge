#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "HodgeRelationshipTestAbility.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Dash.h"
#include "Animation/HodgeAnimInstance.h"
#include "Character/HodgeCombatCharacter.h"
#include "Combat/HodgeCharacterFacingTypes.h"
#include "Combat/HodgeHitReactionTypes.h"
#include "Component/HodgeCharacterMovementComponent.h"
#include "Component/HodgeCharacterRotationComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "UObject/UnrealType.h"
#include <limits>

struct FHodgeFacingLifecycleTestAccess
{
	static bool Check(UHodgeCharacterRotationComponent* Rotation, FGameplayAbilitySpecHandle Handle, int32 Key)
	{
		Rotation->ReplicatedState.ActionAbilityHandle = Handle; Rotation->ReplicatedState.ActionPredictionKey = Key;
		return Rotation->IsAuthorityActionCurrent();
	}
};

namespace HodgeFacingTests
{
	struct FFixture
	{
		UWorld* World = nullptr;
		AHodgeCombatCharacter* Character = nullptr;
		APlayerController* Controller = nullptr;
		UHodgeAbilitySystemComponent* ASC = nullptr;
		UHodgeCharacterRotationComponent* Rotation = nullptr;
		UHodgeCharacterMovementComponent* Movement = nullptr;
		FFixture()
		{
			const UWorld::InitializationValues Values = UWorld::InitializationValues().AllowAudioPlayback(false).RequiresHitProxies(false)
				.CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
			World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
			Character = World->SpawnActor<AHodgeCombatCharacter>();
			Controller = World->SpawnActor<APlayerController>(); Controller->Possess(Character);
			ASC = NewObject<UHodgeAbilitySystemComponent>(Character); ASC->RegisterComponent();
			ASC->InitAbilityActorInfo(Character, Character);
			Rotation = Character->GetCharacterRotationComponent();
			Movement = CastChecked<UHodgeCharacterMovementComponent>(Character->GetCharacterMovement());
			Rotation->InitializeWithAbilitySystem(ASC);
		}
		~FFixture() { Rotation->UninitializeFromAbilitySystem(); World->DestroyWorld(false); }
		void Step(float Seconds = 1.f / 60.f) { Movement->PhysicsRotation(Seconds); static_cast<UActorComponent*>(Rotation)->TickComponent(Seconds, LEVELTICK_All, nullptr); }
	};
	bool BoolProperty(UObject* Object, FName Name)
	{ return FindFProperty<FBoolProperty>(Object->GetClass(), Name)->GetPropertyValue_InContainer(Object); }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeFacingAuthorityActionTest, "Hodge.Facing.AuthorityActionLifecycle", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeFacingAuthorityActionTest::RunTest(const FString& Parameters)
{
	HodgeFacingTests::FFixture F;
	const auto Dash = F.ASC->GiveAbility(FGameplayAbilitySpec(UHodgeGameplayAbility_Dash::StaticClass(), 1));
	TestFalse(TEXT("Ended or inactive predicted Dash cannot restore authority action facing"), FHodgeFacingLifecycleTestAccess::Check(F.Rotation, Dash, 1));
	const auto Handle = F.ASC->GiveAbility(FGameplayAbilitySpec(UHodgeFacingPredictionTestAbility::StaticClass(), 1));
	TestTrue(TEXT("Prediction lifecycle fixture activates"), F.ASC->TryActivateAbility(Handle));
	const auto* Spec = F.ASC->FindAbilitySpecFromHandle(Handle);
	const auto* Instance = Spec ? Spec->GetPrimaryInstance() : nullptr;
	if (!Instance) { AddError(TEXT("Prediction fixture instance missing")); return false; }
	const int32 Key = Instance->GetCurrentActivationInfo().GetActivationPredictionKey().Current;
	TestTrue(TEXT("Live matching predicted action remains authoritative"), FHodgeFacingLifecycleTestAccess::Check(F.Rotation, Handle, Key));
	TestFalse(TEXT("Previous activation cannot overwrite a newer one"), FHodgeFacingLifecycleTestAccess::Check(F.Rotation, Handle, Key + 1));
	F.ASC->CancelAbilityHandle(Handle);
	TestFalse(TEXT("Late matching packet after local end is rejected"), FHodgeFacingLifecycleTestAccess::Check(F.Rotation, Handle, Key));
	const auto ServerOnly = F.ASC->GiveAbility(FGameplayAbilitySpec(UHodgeRelationshipTestAbility::StaticClass(), 1));
	TestTrue(TEXT("Server-only actions retain their authority path"), FHodgeFacingLifecycleTestAccess::Check(F.Rotation, ServerOnly, 0));
	TestTrue(TEXT("Component action sources retain their authority path"), FHodgeFacingLifecycleTestAccess::Check(F.Rotation, {}, 0));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeFacingInputTest, "Hodge.Facing.InputSnapshots", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeFacingInputTest::RunTest(const FString& Parameters)
{
	const auto North = HodgeFacing::MakeMoveIntent(FVector2D(0.f, 1.f), 90.f, 1., 3);
	TestTrue(TEXT("Input remains raw control-relative"), North.RawInput2D.Equals(FVector2D(0.f, 1.f)));
	TestTrue(TEXT("Forward input rotates into world Y"), North.DesiredDirectionWorld.Equals(FVector::RightVector, .001));
	TestEqual(TEXT("Avatar identity retained"), North.AvatarGeneration, 3);
	const auto Analog = HodgeFacing::MakeMoveIntent(FVector2D(.3f, .4f), 0.f, 2., 3);
	TestEqual(TEXT("Analog magnitude retained"), Analog.InputMagnitude, .5f);
	TestTrue(TEXT("World direction is horizontal unit"), Analog.DesiredDirectionWorld.Equals(FVector(.8, .6, 0), .001));
	TestTrue(TEXT("No input does not invent forward intent"), HodgeFacing::MakeMoveIntent({}, 0.f, 0., 0).DesiredDirectionWorld.IsZero());
	FVector Output;
	TestFalse(TEXT("Vertical-only action rejected"), HodgeFacing::NormalizeDirection(FVector(0, 0, 20), Output));
	TestFalse(TEXT("NaN direction rejected"), HodgeFacing::NormalizeDirection(FVector(std::numeric_limits<float>::quiet_NaN(), 0, 0), Output));
	TestTrue(TEXT("Invalid control angle sanitized"), HodgeFacing::MakeMoveIntent(FVector2D(1, 1), std::numeric_limits<float>::infinity(), 0., 1).DesiredDirectionWorld.IsZero());
	TestFalse(TEXT("Action driver cannot be used as base"), HodgeFacing::IsBaseDriver(EHodgeCharacterFacingDriver::ActionDirection));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeFacingModesTest, "Hodge.Facing.ModesAndPriority", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeFacingModesTest::RunTest(const FString& Parameters)
{
	HodgeFacingTests::FFixture F;
	TestTrue(TEXT("Free is default"), F.Movement->bOrientRotationToMovement && !F.Character->bUseControllerRotationYaw);
	F.Controller->SetControlRotation(FRotator(0, 90, 0));
	F.Character->FaceRotation(F.Controller->GetControlRotation(), .1f);
	TestTrue(TEXT("Free idle does not turn with camera"), FMath::Abs(F.Character->GetActorRotation().Yaw) < .01f);
	auto Strafe = F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Controller, F.Character);
	TestTrue(TEXT("Strafe request applied"), Strafe.IsValid() && F.Character->bUseControllerRotationYaw && !F.Movement->bOrientRotationToMovement);
	TestEqual(TEXT("Animation style follows base"), F.Rotation->GetResolvedState().Style, EHodgeLocomotionStyle::ReservedStrafe);
	F.Character->FaceRotation(F.Controller->GetControlRotation(), .01f);
	TestTrue(TEXT("Controller transition is bounded"), F.Character->GetActorRotation().Yaw > 0.f && F.Character->GetActorRotation().Yaw < 5.f);
	auto Free = F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Movement, F.Character);
	TestTrue(TEXT("Latest base request wins"), F.Movement->bOrientRotationToMovement);
	TestTrue(TEXT("Release restores remaining source"), F.Rotation->ReleaseFacingRequest(Free) && F.Character->bUseControllerRotationYaw);
	TestTrue(TEXT("Release last base restores Free"), F.Rotation->ReleaseFacingRequest(Strafe) && F.Movement->bOrientRotationToMovement);
	TestFalse(TEXT("Double release is harmless"), F.Rotation->ReleaseFacingRequest(Strafe));
	TestEqual(TEXT("Released status retained"), F.Rotation->GetFacingRequestStatus(Strafe), EHodgeFacingRequestStatus::Released);
	TestFalse(TEXT("Invalid source rejected"), F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Controller, GetTransientPackage()).IsValid());
	TestFalse(TEXT("Invalid base enum rejected"), F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::ActionDirection, F.Character).IsValid());
	F.Controller->UnPossess();
	TestFalse(TEXT("Controller mode requires a controller"), F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Controller, F.Character).IsValid());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeFacingActionTest, "Hodge.Facing.ActionsAndConstraints", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeFacingActionTest::RunTest(const FString& Parameters)
{
	HodgeFacingTests::FFixture F;
	auto Base = F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Controller, F.Character);
	auto Action = F.Rotation->RequestActionFacing(FVector::RightVector, F.Character, FGuid::NewGuid());
	TestTrue(TEXT("Action starts pending"), Action.IsValid() && F.Rotation->GetFacingRequestStatus(Action) == EHodgeFacingRequestStatus::Pending);
	TestTrue(TEXT("Action disables competing drivers"), !F.Character->bUseControllerRotationYaw && !F.Movement->bOrientRotationToMovement);
	TestEqual(TEXT("Underlying Strafe is retained"), F.Rotation->GetResolvedState().Style, EHodgeLocomotionStyle::ReservedStrafe);
	F.Step(.01f);
	TestTrue(TEXT("Action preparation turns gradually"), F.Character->GetActorRotation().Yaw > 0 && F.Character->GetActorRotation().Yaw < 10);
	for (int32 I = 0; I < 30; ++I) { F.Step(); }
	TestEqual(TEXT("Applied only after alignment"), F.Rotation->GetFacingRequestStatus(Action), EHodgeFacingRequestStatus::Applied);
	F.ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked);
	F.ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked);
	const float Locked = F.Character->GetActorRotation().Yaw;
	TestFalse(TEXT("Strong constraint denies a new action"), F.Rotation->RequestActionFacing(FVector::BackwardVector, F.Character, FGuid::NewGuid(), true).IsValid());
	F.Movement->MoveUpdatedComponent(FVector::ZeroVector, FRotator(0, 180, 0), false);
	TestTrue(TEXT("Final movement respects frozen yaw"), FMath::Abs(FMath::FindDeltaAngleDegrees(Locked, F.Character->GetActorRotation().Yaw)) < .01);
	F.Rotation->ReleaseFacingRequest(Base);
	F.Rotation->ReleaseFacingRequest(Action);
	F.ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked);
	TestTrue(TEXT("Other source continues locking"), F.Rotation->IsYawLocked());
	F.ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked);
	TestEqual(TEXT("Release restores latest base, not old strafe"), F.Rotation->GetResolvedState().Driver, EHodgeCharacterFacingDriver::Movement);
	auto Instant = F.Rotation->RequestActionFacing(FVector::BackwardVector, F.Character, FGuid::NewGuid(), true);
	TestTrue(TEXT("Explicit instant action applied"), Instant.IsValid() && F.Rotation->GetFacingRequestStatus(Instant) == EHodgeFacingRequestStatus::Applied);
	F.Rotation->ReleaseRequestsForSource(F.Character);
	TestFalse(TEXT("Zero direction rejected"), F.Rotation->RequestActionFacing({}, F.Character, FGuid::NewGuid()).IsValid());
	TestFalse(TEXT("Missing execution identity rejected"), F.Rotation->RequestActionFacing(FVector::ForwardVector, F.Character, {}).IsValid());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeFacingLifecycleTest, "Hodge.Facing.LifecycleAndExceptions", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeFacingLifecycleTest::RunTest(const FString& Parameters)
{
	HodgeFacingTests::FFixture F;
	const int32 Generation = F.Rotation->GetAvatarGeneration();
	F.Rotation->InitializeWithAbilitySystem(F.ASC);
	TestEqual(TEXT("Duplicate bind does not create an epoch"), F.Rotation->GetAvatarGeneration(), Generation);
	UObject* Source = NewObject<USkeletalMeshComponent>(F.Character);
	auto Handle = F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Controller, Source);
	Source->MarkAsGarbage(); F.Step();
	TestEqual(TEXT("Destroyed source restored Free"), F.Rotation->GetResolvedState().Driver, EHodgeCharacterFacingDriver::Movement);
	F.ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Death);
	TestTrue(TEXT("Death freezes yaw"), F.Rotation->IsYawLocked());
	TestFalse(TEXT("Death denies new base requests"), F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Movement, F.Character).IsValid());
	F.ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Death);
	F.ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked);
	F.Rotation->UninitializeFromAbilitySystem();
	TestEqual(TEXT("Unbind retains external tag"), F.ASC->GetTagCount(HodgeGameplayTags::Status_Rotation_Locked), 1);
	TestFalse(TEXT("Unbound API refuses requests"), F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Movement, F.Character).IsValid());
	F.ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked);
	F.Rotation->InitializeWithAbilitySystem(F.ASC);
	TestFalse(TEXT("Old handle cannot affect new epoch"), F.Rotation->ReleaseFacingRequest(Handle));
	TArray<FHodgeFacingRequestHandle> Handles;
	for (int32 I = 0; I < 32; ++I) { Handles.Add(F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Movement, F.Character)); }
	TestFalse(TEXT("Request capacity bounds abuse"), F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Movement, F.Character).IsValid());
	F.Rotation->ReleaseRequestsForSource(F.Character);
	F.ASC->InitAbilityActorInfo(F.Character, nullptr); F.Step();
	TestFalse(TEXT("Lost avatar unbinds"), F.Rotation->IsFacingSystemReady());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeFacingActivationPairTest, "Hodge.Facing.ActivationPair", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeFacingActivationPairTest::RunTest(const FString& Parameters)
{
	HodgeFacingTests::FFixture F;
	const FGuid Execution = FGuid::NewGuid();
	const auto Authority = F.Rotation->RequestActionFacing(FVector::ForwardVector, F.Character, Execution);
	const auto Prediction = F.Rotation->RequestActionFacing(FVector::ForwardVector, F.Character, Execution);
	F.Step();
	TestTrue(TEXT("Paired activation confirms both handles"), F.Rotation->IsActionFacingApplied(Authority) && F.Rotation->IsActionFacingApplied(Prediction));
	const auto NewExecution = F.Rotation->RequestActionFacing(FVector::ForwardVector, F.Character, FGuid::NewGuid()); F.Step();
	TestFalse(TEXT("Different execution cannot confirm the previous action"), F.Rotation->IsActionFacingApplied(Authority));
	F.Rotation->ReleaseFacingRequest(NewExecution); F.Step();
	auto* OtherSource = NewObject<USkeletalMeshComponent>(F.Character); OtherSource->RegisterComponent();
	const auto Other = F.Rotation->RequestActionFacing(FVector::ForwardVector, OtherSource, Execution); F.Step();
	TestFalse(TEXT("Another source cannot authorize the previous action"), F.Rotation->IsActionFacingApplied(Authority));
	F.Rotation->ReleaseFacingRequest(Other); F.Rotation->ReleaseFacingRequest(Prediction); F.Step();
	TestTrue(TEXT("Releasing prediction preserves own authority request"), F.Rotation->IsActionFacingApplied(Authority));
	F.Rotation->ReleaseFacingRequest(Authority);
	TestFalse(TEXT("Released action cannot remain applied"), F.Rotation->IsActionFacingApplied(Authority));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeFacingReplayTest, "Hodge.Facing.ReplayAndNetworkData", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeFacingReplayTest::RunTest(const FString& Parameters)
{
	HodgeFacingTests::FFixture F;
	auto* Prediction = F.Movement->GetPredictionData_Client_Character();
	FSavedMovePtr Free = Prediction->AllocateNewMove(); Free->SetMoveFor(F.Character, .016f, {}, *Prediction);
	auto Handle = F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Controller, F.Character);
	FSavedMovePtr Strafe = Prediction->AllocateNewMove(); Strafe->SetMoveFor(F.Character, .016f, {}, *Prediction);
	TestFalse(TEXT("Different modes do not combine"), Free->CanCombineWith(Strafe, F.Character, .125f));
	F.Character->bClientUpdating = true; Free->PrepMoveFor(F.Character);
	TestTrue(TEXT("Replay applies historical flags"), F.Movement->bOrientRotationToMovement && !F.Character->bUseControllerRotationYaw);
	F.Character->bClientUpdating = false; F.Rotation->ClearMoveReplayState();
	TestTrue(TEXT("Replay exit restores current flags"), F.Character->bUseControllerRotationYaw && !F.Movement->bOrientRotationToMovement);
	FHodgeCharacterRotationState State = F.Rotation->GetResolvedState(); State.RequestSequence = 17;
	F.Character->bClientUpdating = true; F.Rotation->SetMoveReplayState(State);
	FSavedMovePtr Saved = Prediction->AllocateNewMove(); Saved->SetMoveFor(F.Character, .016f, {}, *Prediction);
	F.Character->bClientUpdating = false; F.Rotation->ClearMoveReplayState();
	auto& Container = F.Movement->GetFacingMoveDataForTests();
	Container.ClientFillNetworkMoveData(Saved.Get(), nullptr, nullptr);
	TArray<uint8> Bytes; FMemoryWriter Writer(Bytes);
	TestTrue(TEXT("Custom packed move serializes"), Container.Serialize(*F.Movement, Writer, nullptr));
	FMemoryReader Reader(Bytes);
	TestTrue(TEXT("Custom packed move deserializes"), Container.Serialize(*F.Movement, Reader, nullptr));
	TestEqual(TEXT("Packet consumes exactly its payload"), Reader.Tell(), Writer.Tell());
	TestFalse(TEXT("Unknown sequence is not authorization"), F.Rotation->BeginServerMove(999));
	F.Rotation->EndServerMove();
	TestTrue(TEXT("Current mode survives rejected move"), F.Character->bUseControllerRotationYaw);
	F.Rotation->ReleaseFacingRequest(Handle);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeFacingAnimationTest, "Hodge.Facing.AnimationSnapshot", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeFacingAnimationTest::RunTest(const FString& Parameters)
{
	HodgeFacingTests::FFixture F;
	auto* Anim = NewObject<UHodgeAnimInstance>(F.Character->GetMesh());
	static_cast<UAnimInstance*>(Anim)->NativeUpdateAnimation(.12f);
	TestFalse(TEXT("Free style is default animation branch"), HodgeFacingTests::BoolProperty(Anim, TEXT("bUseStrafeLocomotion")));
	TestTrue(TEXT("Free suppresses controller turn compensation"), HodgeFacingTests::BoolProperty(Anim, TEXT("bSuppressLocomotionYaw")));
	auto Strafe = F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Controller, F.Character);
	F.Step(); static_cast<UAnimInstance*>(Anim)->NativeUpdateAnimation(.12f);
	TestTrue(TEXT("Strafe snapshot selects preserved branch"), HodgeFacingTests::BoolProperty(Anim, TEXT("bUseStrafeLocomotion")));
	F.Rotation->ReleaseFacingRequest(Strafe); F.Step(); static_cast<UAnimInstance*>(Anim)->NativeUpdateAnimation(.12f);
	TestFalse(TEXT("Release clears strafe snapshot"), HodgeFacingTests::BoolProperty(Anim, TEXT("bUseStrafeLocomotion")));
	TestTrue(TEXT("Offset only resets after blend weight zero"), HodgeFacingTests::BoolProperty(Anim, TEXT("bResetLocomotionYaw")));
	return true;
}

#endif
