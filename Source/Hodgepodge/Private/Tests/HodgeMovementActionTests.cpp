#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Combat/HodgeMovementActionTypes.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Dash.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Sprint.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "Component/HodgeLocomotionPolicyComponent.h"
#include "Component/HodgeDefenseComponent.h"
#include "Component/HodgeCharacterMovementComponent.h"
#include "Character/HodgeCombatCharacter.h"
#include "Data/HodgeSprintAbilityProfile.h"
#include "Animation/AnimMontage.h"
#include "Animation/HodgeAnimInstance.h"
#include "Engine/World.h"
#include "GameFramework/RootMotionSource.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "UObject/UnrealType.h"
#include <limits>

struct FHodgeMovementTestAccess
{
	static void ConfigurePolicy(UHodgeLocomotionPolicyComponent* Policy, const UHodgeSprintAbilityProfile* Profile)
	{
		Policy->SprintProfile = Profile; Policy->State.bSprinting = true; Policy->State.ActivationKey = 17;
		Policy->State.MaxSpeed = Profile->SprintMaxSpeed;
	}
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeDashDirectionsTest, "Hodge.Movement.DashDirections", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeDashDirectionsTest::RunTest(const FString& Parameters)
{
	const auto* Config = GetDefault<UHodgeSprintAbilityProfile>();
	TestTrue(TEXT("Backward defaults enabled"), Config->bEnableBackwardVariant);
	FHodgeDashDirections D;
	TestTrue(TEXT("Backward valid"), FHodgeDashDirections::Select(-FVector::ForwardVector, FVector::ForwardVector, true, 45.f, D));
	TestEqual(TEXT("Backward variant"), D.Variant, EHodgeDashVariant::Backward);
	TestTrue(TEXT("Back moves backwards and preserves facing"), D.MoveDirection.Equals(-FVector::ForwardVector) && D.FacingDirection.Equals(FVector::ForwardVector));
	FHodgeDashDirections::Select(FVector(-1, 1, 0), FVector::ForwardVector, true, 45.f, D);
	TestEqual(TEXT("Cone includes diagonal boundary"), D.Variant, EHodgeDashVariant::Backward);
	FHodgeDashDirections::Select(FRotator(0, 134, 0).Vector(), FVector::ForwardVector, true, 45.f, D);
	TestEqual(TEXT("Just outside cone uses forward"), D.Variant, EHodgeDashVariant::Forward);
	FHodgeDashDirections::Select(-FVector::RightVector, FVector::RightVector, true, 45.f, D);
	TestTrue(TEXT("Cone is relative to actor, not controller input Y"), D.Variant == EHodgeDashVariant::Backward && D.FacingDirection.Equals(FVector::RightVector));
	FHodgeDashDirections::Select(-FVector::ForwardVector, FVector::ForwardVector, false, 45.f, D);
	TestTrue(TEXT("Disabled B turns F towards backwards input"), D.Variant == EHodgeDashVariant::Forward && D.FacingDirection.Equals(-FVector::ForwardVector));
	FHodgeDashDirections::Select({}, FVector::ForwardVector, true, 45.f, D);
	TestTrue(TEXT("No input falls forward"), D.MoveDirection.Equals(FVector::ForwardVector));
	TestFalse(TEXT("NaN input rejected"), FHodgeDashDirections::Select(FVector(std::numeric_limits<float>::quiet_NaN(),0,0), FVector::ForwardVector, true, 45, D));
	TestFalse(TEXT("Invalid cone rejected"), FHodgeDashDirections::Select({}, FVector::ForwardVector, true, 90, D));
	TestFalse(TEXT("Vertical actor facing rejected"), FHodgeDashDirections::Select({}, FVector::UpVector, true, 45, D));
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeSprintSessionTest, "Hodge.Movement.InputSessions", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeSprintSessionTest::RunTest(const FString& Parameters)
{
	FHodgeSprintInputSession S;
	TestFalse(TEXT("Unbound avatar rejected"), S.Begin(0, 0));
	TestTrue(TEXT("Started accepted"), S.Begin(1, 2));
	TestFalse(TEXT("Repeated Triggered cannot restart"), S.Begin(1.1, 2));
	TestFalse(TEXT("Hold cannot bypass unsuccessful Dash"), S.Qualifies(2, .22f));
	S.State = EHodgeSprintInputState::DashActive;
	TestFalse(TEXT("Before hold threshold"), S.Qualifies(1.21, .22f));
	TestTrue(TEXT("Hold threshold qualifies"), S.Qualifies(1.23, .22f));
	S.Consume(); TestFalse(TEXT("Consumed session cannot retry"), S.Qualifies(5, .22f));
	S.Release(); TestTrue(TEXT("Release permits new session"), S.Begin(6, 2));
	TestEqual(TEXT("New session increments"), S.SessionId, 2);
	S.Release(); TestFalse(TEXT("Release clears handoff"), S.Qualifies(10, .22f));
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeMovementTargetDataTest, "Hodge.Movement.TargetDataSerialization", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeMovementTargetDataTest::RunTest(const FString& Parameters)
{
	FHodgeMovementActionTargetData Source, Restored; Source.Input = FVector2D(.2f,-.8f); Source.ControlYaw = 123.f; Source.ActorYawAtStart = -57.f; Source.SessionId = 14;
	TArray<uint8> Bytes; FMemoryWriter Writer(Bytes); bool Success = false; Source.NetSerialize(Writer, nullptr, Success);
	TestTrue(TEXT("Write successful"), Success); FMemoryReader Reader(Bytes); Restored.NetSerialize(Reader, nullptr, Success);
	TestTrue(TEXT("Read successful and exact intent retained"), Success && Source.Input.Equals(Restored.Input));
	TestEqual(TEXT("Control sampling yaw retained"), Restored.ControlYaw, 123.f);
	TestEqual(TEXT("Actor start yaw retained across network"), Restored.ActorYawAtStart, -57.f);
	TestEqual(TEXT("Session identity retained"), Restored.SessionId, 14);
	TestFalse(TEXT("No cooldown configuration exists"), FindFProperty<FProperty>(UHodgeSprintAbilityProfile::StaticClass(), TEXT("CooldownDuration")) != nullptr);
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeMovementPolicyTest, "Hodge.Movement.PolicyAndAuthorization", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeMovementPolicyTest::RunTest(const FString& Parameters)
{
	const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).RequiresHitProxies(false).CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
	auto* Character = World->SpawnActor<AHodgeCombatCharacter>();
	auto* Owner = World->SpawnActor<AActor>();
	auto* ASC = NewObject<UHodgeAbilitySystemComponent>(Owner); ASC->RegisterComponent(); ASC->InitAbilityActorInfo(Owner, Character);
	auto* Policy = Character->FindComponentByClass<UHodgeLocomotionPolicyComponent>(); Policy->InitializeWithAbilitySystem(ASC);
	auto* Profile = NewObject<UHodgeSprintAbilityProfile>(); FHodgeMovementTestAccess::ConfigurePolicy(Policy, Profile);
	TestNull(TEXT("Stamina cost is removed"), FindFProperty<FProperty>(UHodgeSprintAbilityProfile::StaticClass(), TEXT("DashCost")));
	TestNull(TEXT("Resource budget is removed from saved moves"), FindFProperty<FProperty>(FHodgeLocomotionState::StaticStruct(), TEXT("RemainingBudget")));
	FGameplayAbilityActorInfo ActorInfo; ActorInfo.InitFromActor(Owner, Character, ASC);
	auto* Dash = NewObject<UHodgeGameplayAbility_Dash>(Owner);
	ASC->AddLooseGameplayTag(HodgeMovementTags::DashPreparing);
	TestTrue(TEXT("Own preparation must not reject commit cost check"), Dash->CheckCost({}, &ActorInfo));
	ASC->RemoveLooseGameplayTag(HodgeMovementTags::DashPreparing);
	ASC->AddLooseGameplayTag(HodgeMovementTags::Dashing);
	TestFalse(TEXT("Already dashing cannot commit another dash"), Dash->CheckCost({}, &ActorInfo));
	ASC->RemoveLooseGameplayTag(HodgeMovementTags::Dashing);
	Policy->BeginServerMove(999);
	TestFalse(TEXT("Unknown move key grants no sprint speed"), Policy->GetResolvedPolicy().bSprinting);
	Policy->EndServerMove(); TestTrue(TEXT("End server move restores live policy"), Policy->GetResolvedPolicy().bSprinting);
	Policy->BeginServerMove(17); TestTrue(TEXT("Authorized key consumes server profile"), Policy->GetResolvedPolicy().bSprinting);
	Policy->EndServerMove();
	const FGuid Slow = Policy->AcquireSpeedModifier(Character, .5f);
	const FGuid Slow2 = Policy->AcquireSpeedModifier(Policy, .5f);
	TestTrue(TEXT("Speed modifiers have independent handles"), Slow.IsValid() && Slow2.IsValid());
	TestTrue(TEXT("Sprint applies both persistent modifiers"), FMath::IsNearlyEqual(Policy->GetResolvedPolicy().MaxSpeed, 180.f));
	Policy->ReleaseSpeedModifier(Slow);
	TestTrue(TEXT("Release preserves other slowdown"), FMath::IsNearlyEqual(Policy->GetResolvedPolicy().MaxSpeed, 360.f));
	Policy->ReleaseSpeedModifier(Slow2);
	TestFalse(TEXT("Nonfinite speed modifier rejected"), Policy->AcquireSpeedModifier(Character, std::numeric_limits<float>::infinity()).IsValid());
	FHodgeLocomotionState Historical = Policy->GetResolvedPolicy(); Historical.MaxSpeed = 600.f;
	Policy->SetMoveReplayState(Historical);
	TestEqual(TEXT("Replay preserves recorded speed without charging a resource"), Policy->GetResolvedPolicy().MaxSpeed, 600.f);
	Policy->ClearMoveReplayState();
	Policy->SetSprintInput(1, true); Policy->AuthorizeHandoff(17, 1, .5f);
	TestTrue(TEXT("Successful Dash grants held handoff"), Policy->CanHandoff(1));
	Policy->ConsumeHandoff(); TestFalse(TEXT("Handoff is one use"), Policy->CanHandoff(1));
	Policy->SetSprintInput(1,false); Policy->AuthorizeHandoff(17,1,.5f);
	TestFalse(TEXT("Release cannot resume while held"), Policy->CanHandoff(1));
	float ExitSpeed = 0.f;
	Policy->RecordDashExitVelocity(TEXT("DashSession1"), 720.f);
	TestTrue(TEXT("Successful exit strategy survives saved-move source restoration"), Policy->GetDashExitVelocity(TEXT("DashSession1"), ExitSpeed));
	TestEqual(TEXT("Replay preserves the approved target speed limit"), ExitSpeed, 720.f);
	TestFalse(TEXT("Unrelated RMS is never changed"), Policy->GetDashExitVelocity(TEXT("OtherAbility"), ExitSpeed));
	Policy->RecordDashExitVelocity(TEXT("InvalidExit"), -1.f);
	TestFalse(TEXT("Invalid exit speed cannot enter replay cache"), Policy->GetDashExitVelocity(TEXT("InvalidExit"), ExitSpeed));
	auto* Movement = CastChecked<UHodgeCharacterMovementComponent>(Character->GetCharacterMovement());
	Movement->Activate(true);
	TestTrue(TEXT("Correction fixture has an active movement component"), Movement->IsActive() && Movement->UpdatedComponent != nullptr);
	auto* Prediction = Movement->GetPredictionData_Client_Character();
	auto Source = MakeShared<FRootMotionSource_ConstantForce>(); Source->InstanceName = TEXT("DashSession1"); Source->LocalID = 1;
	Movement->CurrentRootMotion.RootMotionSources.Add(Source);
	FCharacterMoveResponseDataContainer Response;
	Response.ClientAdjustment.bAckGoodMove = false; Response.bRootMotionSourceCorrection = true;
	Response.ClientAdjustment.NewLoc = Character->GetActorLocation(); Response.ClientAdjustment.NewVel = FVector(800, 0, 0);
	Response.ClientAdjustment.MovementMode = MOVE_Walking; Response.ClientAdjustment.TimeStamp = 1.f;
	FSavedMovePtr Saved = Prediction->AllocateNewMove(); Saved->TimeStamp = 1.f; Prediction->SavedMoves.Add(Saved);
	Movement->ClientHandleMoveResponse(Response);
	TestEqual(TEXT("Exited Dash correction retains the packet's authoritative planar velocity"), Movement->Velocity.X, 800.);
	Response.ClientAdjustment.NewVel = FVector::ZeroVector;
	Movement->ClientHandleMoveResponse(Response);
	TestEqual(TEXT("Expired correction still cannot overwrite acknowledged velocity"), Movement->Velocity.X, 800.);
	Source->InstanceName = TEXT("OtherAbility"); Response.ClientAdjustment.TimeStamp = 2.f;
	Response.ClientAdjustment.NewVel = FVector(800, 0, 0);
	Saved = Prediction->AllocateNewMove(); Saved->TimeStamp = 2.f; Prediction->SavedMoves.Add(Saved);
	Movement->ClientHandleMoveResponse(Response);
	TestTrue(TEXT("Unrelated RMS retains the engine correction path"), Movement->Velocity.IsZero());
	Movement->CurrentRootMotion.Clear();
	Policy->UninitializeFromAbilitySystem(); Policy->InitializeWithAbilitySystem(ASC);
	TestFalse(TEXT("Pawn rebind clears old source exit decisions"), Policy->GetDashExitVelocity(TEXT("DashSession1"), ExitSpeed));
	TestFalse(TEXT("Pawn rebind clears old movement policy"), Policy->GetResolvedPolicy().bSprinting);
	Policy->UninitializeFromAbilitySystem(); World->DestroyWorld(false); return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeMovementTimingTest, "Hodge.Movement.TimingAndPivot", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeMovementTimingTest::RunTest(const FString& Parameters)
{
	auto* P = NewObject<UHodgeSprintAbilityProfile>();
	TestTrue(TEXT("Default handoff opens at travel completion without a dead interval"), P->IsHandoffWindow(P->Duration));
	TestTrue(TEXT("Handoff includes configured opening"), P->IsHandoffWindow(P->HandoffOpenTime));
	TestTrue(TEXT("Handoff includes configured closing"), P->IsHandoffWindow(P->HandoffCloseTime));
	TestFalse(TEXT("Expired handoff rejected"), P->IsHandoffWindow(P->HandoffCloseTime + .01));
	TestTrue(TEXT("Remote authority may authorize an in-flight handoff during grace"), P->IsAuthorityHandoffWindow(P->HandoffCloseTime + .1));
	TestFalse(TEXT("Authority never opens the window early"), P->IsAuthorityHandoffWindow(P->HandoffOpenTime - .01));
	TestFalse(TEXT("Authority grace is bounded"), P->IsAuthorityHandoffWindow(P->HandoffCloseTime + P->HandoffNetworkGrace + .01));
	TestFalse(TEXT("No movement cancel before travel completes"), P->CanMoveCancel(P->Duration - .01));
	TestTrue(TEXT("Default movement recovery opens at travel completion"), P->CanMoveCancel(P->Duration));
	TestTrue(TEXT("Configured recovery opens cancellation"), P->CanMoveCancel(P->MoveCancelOpenTime));
	P->bAllowMoveCancel = false; TestFalse(TEXT("Cancellation can be disabled"), P->CanMoveCancel(10.));
	P->HandoffOpenTime = .8f; P->HandoffCloseTime = 1.1f;
	TestFalse(TEXT("Handoff time is configurable"), P->IsHandoffWindow(.6));
	TestTrue(TEXT("Later configured window accepted"), P->IsHandoffWindow(.9));
	TestFalse(TEXT("NaN handoff rejected"), P->IsHandoffWindow(std::numeric_limits<double>::quiet_NaN()));
	TestTrue(TEXT("Pivot advances at authored normal speed"), FMath::IsNearlyEqual(UHodgeAnimInstance::AdvanceTurnTime(.1f,.02f,1.6f,1.f),.12f));
	TestTrue(TEXT("Pivot rate is configurable"), FMath::IsNearlyEqual(UHodgeAnimInstance::AdvanceTurnTime(.1f,.02f,1.6f,2.f),.14f));
	TestEqual(TEXT("Turn clamps at end without wrap or freeze at zero"), UHodgeAnimInstance::AdvanceTurnTime(1.59f,.02f,1.6f,1.f), 1.6f);
	TestFalse(TEXT("Turn cannot exit before its recovery tail"), UHodgeAnimInstance::IsTurnComplete(1.4f,1.6f,1.f,.12f));
	TestTrue(TEXT("Turn must exit into cycle before holding final frame"), UHodgeAnimInstance::IsTurnComplete(1.5f,1.6f,1.f,.12f));
	TestTrue(TEXT("Turn exit uses configured playback rate"), UHodgeAnimInstance::IsTurnComplete(.7f,1.6f,2.f,.12f));
	TestFalse(TEXT("Invalid playback rate cannot authorize transition"), UHodgeAnimInstance::IsTurnComplete(1.f,1.6f,0.f,.12f));
	TestEqual(TEXT("Negative update cannot rewind"), UHodgeAnimInstance::AdvanceTurnTime(.4f,-.1f,1.6f,1.f), .4f);
	TestEqual(TEXT("NaN input does not poison evaluator"), UHodgeAnimInstance::AdvanceTurnTime(std::numeric_limits<float>::quiet_NaN(), .01f,1.6f,1.f), 0.f);
	TArray<FText> Errors; P->MontagePlayRate = 0.f; TestFalse(TEXT("Invalid playback profile rejected"), P->Validate(Errors));
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeDashFootIKTest, "Hodge.Movement.DashFootIKContinuity", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeDashFootIKTest::RunTest(const FString& Parameters)
{
	using Anim = UHodgeAnimInstance;
	TestEqual(TEXT("Idle keeps original floor correction"), Anim::ResolveFootIKAlpha(0.f, 0.f), 1.f);
	TestEqual(TEXT("Full Dash keeps the same floor correction"), Anim::ResolveFootIKAlpha(1.f, 1.f), 1.f);
	TestEqual(TEXT("Dash blend-out never restarts pelvis correction"), Anim::ResolveFootIKAlpha(.4f, .4f), 1.f);
	TestEqual(TEXT("Other full-body attacks still suppress IK"), Anim::ResolveFootIKAlpha(1.f, 0.f), 0.f);
	TestEqual(TEXT("Dash replaced by another action blends out IK"), Anim::ResolveFootIKAlpha(1.f, .25f), .25f);
	TestEqual(TEXT("Malformed excess Dash weight is bounded"), Anim::ResolveFootIKAlpha(.5f, 2.f), 1.f);
	TestEqual(TEXT("NaN cannot leak to skeletal control"), Anim::ResolveFootIKAlpha(std::numeric_limits<float>::quiet_NaN(), 1.f), 0.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeSprintPivotTest, "Hodge.Movement.RootMotionPivot", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeSprintPivotTest::RunTest(const FString& Parameters)
{
	using GA = UHodgeGameplayAbility_Sprint;
	TestFalse(TEXT("Pivot retains control during required recovery"), GA::IsPivotRecoveryComplete(.89f, .9f));
	TestTrue(TEXT("Pivot releases at the configured recovery boundary"), GA::IsPivotRecoveryComplete(.9f, .9f));
	TestTrue(TEXT("Pivot no longer waits for the running tail"), GA::IsPivotRecoveryComplete(1.1f, .9f));
	TestFalse(TEXT("Malformed montage position cannot authorize recovery"), GA::IsPivotRecoveryComplete(std::numeric_limits<float>::quiet_NaN(), .9f));
	TestFalse(TEXT("Malformed recovery point cannot authorize release"), GA::IsPivotRecoveryComplete(1.f, -1.f));
	TestEqual(TEXT("Sprint has an isolated stride group"), UHodgeAnimInstance::ResolveCycleSyncGroup(true), FName(TEXT("HodgeSprint")));
	TestEqual(TEXT("Ordinary movement restores its existing group"), UHodgeAnimInstance::ResolveCycleSyncGroup(false), FName(TEXT("Locomotion")));
	TestTrue(TEXT("Reverse at sprint speed starts a pivot"), GA::QualifiesPivot(FVector(720,0,0), FVector(-1,0,0), 250.f,150.f));
	TestFalse(TEXT("Side input does not select a half-turn clip"), GA::QualifiesPivot(FVector(720,0,0), FVector(0,1,0), 250.f,150.f));
	TestFalse(TEXT("Low speed stays ordinary locomotion"), GA::QualifiesPivot(FVector(100,0,0), FVector(-1,0,0), 250.f,150.f));
	TestFalse(TEXT("No intent cannot start a pivot"), GA::QualifiesPivot(FVector(720,0,0), {}, 250.f,150.f));
	TestFalse(TEXT("Nonfinite velocity rejected"), GA::QualifiesPivot(FVector(std::numeric_limits<float>::quiet_NaN(),0,0), FVector(-1,0,0), 250.f,150.f));
	TestFalse(TEXT("Invalid trigger angle rejected"), GA::QualifiesPivot(FVector(720,0,0), FVector(-1,0,0),250.f,181.f));
	TestEqual(TEXT("Sprint cycle matches nominal root speed"), UHodgeAnimInstance::MatchCycleRate(720,720,1,.5f,1.5f), 1.f);
	TestEqual(TEXT("Slowed movement also slows stride playback"), UHodgeAnimInstance::MatchCycleRate(360,720,1,.5f,1.5f), .5f);
	TestEqual(TEXT("No cycle freeze at stop"), UHodgeAnimInstance::MatchCycleRate(0,720,1,.5f,1.5f), .5f);
	TestEqual(TEXT("Bad cycle reference falls back safely"), UHodgeAnimInstance::MatchCycleRate(720,0,1,.5f,1.5f), 1.f);
	FHodgeSprintPivotTargetData Payload, Restored;
	Payload.IncomingDirection = FVector(1,0,0); Payload.DesiredDirection = FVector(-1,0,0); Payload.StartingYaw=12.f; Payload.SessionId=8; Payload.SequenceId=3;
	TArray<uint8> Bytes; FMemoryWriter Writer(Bytes); bool Success=false; Payload.NetSerialize(Writer,nullptr,Success);
	FMemoryReader Reader(Bytes); Restored.NetSerialize(Reader,nullptr,Success);
	TestTrue(TEXT("Pivot target data retains directions and identity"), Success && Restored.IncomingDirection==Payload.IncomingDirection && Restored.DesiredDirection==Payload.DesiredDirection && Restored.StartingYaw==12.f && Restored.SessionId==8 && Restored.SequenceId==3);
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeMovementDefenseTest, "Hodge.Movement.DefenseWindows", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeMovementDefenseTest::RunTest(const FString& Parameters)
{
	const auto* P = GetDefault<UHodgeSprintAbilityProfile>(); using Outcome = EHodgeIncomingHitOutcome;
	TestEqual(TEXT("Before window is allowed"), UHodgeDefenseComponent::EvaluateWindow(*P, .039, false, true, true), Outcome::Allowed);
	TestEqual(TEXT("Window start is perfect"), UHodgeDefenseComponent::EvaluateWindow(*P, P->PerfectStart, false, true, true), Outcome::PerfectDodge);
	TestEqual(TEXT("Perfect end is excluded but immunity continues"), UHodgeDefenseComponent::EvaluateWindow(*P, P->PerfectEnd, false, true, true), Outcome::Dodged);
	TestEqual(TEXT("Immunity end is excluded"), UHodgeDefenseComponent::EvaluateWindow(*P, P->InvulnerabilityEnd, false, true, true), Outcome::Allowed);
	TestEqual(TEXT("Consumed reward never repeats"), UHodgeDefenseComponent::EvaluateWindow(*P, .05, true, true, true), Outcome::Dodged);
	TestEqual(TEXT("Undodgeable attack bypasses window"), UHodgeDefenseComponent::EvaluateWindow(*P, .05, false, false, true), Outcome::Allowed);
	TestEqual(TEXT("Unqualified hit cannot reward perfect"), UHodgeDefenseComponent::EvaluateWindow(*P, .05, false, true, false), Outcome::Dodged);
	TestEqual(TEXT("Nonfinite time rejected"), UHodgeDefenseComponent::EvaluateWindow(*P, std::numeric_limits<double>::quiet_NaN(), false, true, true), Outcome::Allowed);
	return true;
}
#endif
