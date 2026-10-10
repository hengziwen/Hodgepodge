#include "Component/HodgeLocomotionPolicyComponent.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Data/HodgePawnData.h"
#include "Data/HodgeSprintAbilityProfile.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeLocomotionPolicyComponent)

UHodgeLocomotionPolicyComponent::UHodgeLocomotionPolicyComponent(const FObjectInitializer& Initializer) : Super(Initializer)
{
	PrimaryComponentTick.bCanEverTick = true; SetIsReplicatedByDefault(true);
}
void UHodgeLocomotionPolicyComponent::InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC)
{
	if (ASC == InASC && InASC && InASC->GetAvatarActor() == GetOwner()) { return; }
	UninitializeFromAbilitySystem();
	if (!InASC || InASC->GetAvatarActor() != GetOwner()) { return; }
	ASC = InASC;
}
void UHodgeLocomotionPolicyComponent::UninitializeFromAbilitySystem()
{
	ASC.Reset(); SprintSource.Reset(); SprintProfile = nullptr; SprintHandle.Invalidate();
	State = {}; ReplayState = {}; bReplay = bServerOverride = false;
	PredictedState = {}; SpeedModifiers.Reset();
	DashExitVelocities.Reset();
	InputSessionId = HandoffSessionId = 0; bInputHeld = false; OnSprintAuthorityEnded.Clear();
}
const UHodgeSprintAbilityProfile* UHodgeLocomotionPolicyComponent::GetProfile() const
{
	if (OverrideProfile) { return OverrideProfile; }
	const auto* Extension = UHodgePawnExtensionComponent::FindPawnExtensionComponent(GetOwner());
	const auto* Data = Extension ? Extension->GetPawnData<UHodgePawnData>() : nullptr;
	return Data ? Data->SprintAbilityProfile.Get() : nullptr;
}
FGuid UHodgeLocomotionPolicyComponent::AcquireSprint(UGameplayAbility* Source, const UHodgeSprintAbilityProfile* Profile, int32 ActivationKey)
{
	if (!ASC.IsValid() || ASC->GetAvatarActor() != GetOwner() || !Source || !Source->IsActive() ||
		Source->GetAvatarActorFromActorInfo() != GetOwner() || !Profile || SprintHandle.IsValid()) { return {}; }
	SprintSource = Source; SprintProfile = Profile; SprintHandle = FGuid::NewGuid();
	State.ActivationKey = ActivationKey; ++State.Version; State.bSprinting = true;
	State.MaxSpeed = Profile->SprintMaxSpeed; State.Acceleration = Profile->SprintAcceleration;
	State.Braking = Profile->SprintBraking; State.TurnRate = Profile->SprintTurnRate; State.bPivotAllowed = Profile->bAllowSprintPivot;
	PredictedState = State;
	GetOwner()->ForceNetUpdate(); return SprintHandle;
}
void UHodgeLocomotionPolicyComponent::ReleasePolicy(FGuid Handle)
{
	if (!Handle.IsValid() || Handle != SprintHandle) { return; }
	SprintHandle.Invalidate(); SprintSource.Reset(); SprintProfile = nullptr;
	State.bSprinting = false; State.bPivotAllowed = false; ++State.Version; GetOwner()->ForceNetUpdate();
	PredictedState.bSprinting = false; PredictedState.bPivotAllowed = false;
	if (bServerOverride) { ReplayState.bSprinting = false; ReplayState.bPivotAllowed = false; }
}
FHodgeLocomotionState UHodgeLocomotionPolicyComponent::GetResolvedPolicy() const
{
	if (bReplay || bServerOverride) { return ReplayState; }
	const auto* Pawn = GetPawn<APawn>();
	const bool bOwningClient = Pawn && Pawn->IsLocallyControlled() && !Pawn->HasAuthority();
	FHodgeLocomotionState Result = bOwningClient && SprintHandle.IsValid() ? PredictedState : State;
	if (bOwningClient && !SprintHandle.IsValid()) { Result.bSprinting = false; Result.bPivotAllowed = false; }
	Result.SpeedScale = State.SpeedScale; Result.MaxSpeed *= State.SpeedScale;
	return Result;
}
FGuid UHodgeLocomotionPolicyComponent::AcquireSpeedModifier(UObject* Source, float Multiplier)
{
	if (!GetOwner()->HasAuthority() || !ASC.IsValid() || !IsValid(Source) || !FMath::IsFinite(Multiplier) || Multiplier < 0.f || Multiplier > 4.f || SpeedModifiers.Num() >= 32) { return {}; }
	const auto* Ability = Cast<UGameplayAbility>(Source);
	if (Source != GetOwner() && !Source->IsIn(GetOwner()) && !(Ability && Ability->IsActive() && Ability->GetAvatarActorFromActorInfo() == GetOwner())) { return {}; }
	const FGuid Handle = FGuid::NewGuid(); SpeedModifiers.Add(Handle, {Source, Multiplier}); RefreshSpeedScale(); return Handle;
}
void UHodgeLocomotionPolicyComponent::ReleaseSpeedModifier(FGuid Handle)
{ if (GetOwner()->HasAuthority() && SpeedModifiers.Remove(Handle)) { RefreshSpeedScale(); } }
void UHodgeLocomotionPolicyComponent::RefreshSpeedScale()
{
	float Scale = 1.f;
	for (auto It = SpeedModifiers.CreateIterator(); It; ++It)
	{
		const auto* Ability = Cast<UGameplayAbility>(It.Value().Source.Get());
		if (!It.Value().Source.IsValid() || (Ability && (!Ability->IsActive() || Ability->GetAvatarActorFromActorInfo() != GetOwner()))) { It.RemoveCurrent(); }
		else { Scale *= It.Value().Multiplier; }
	}
	Scale = FMath::Clamp(Scale, 0.f, 4.f);
	if (!FMath::IsNearlyEqual(Scale, State.SpeedScale)) { State.SpeedScale = Scale; ++State.Version; GetOwner()->ForceNetUpdate(); }
}
void UHodgeLocomotionPolicyComponent::SetMoveReplayState(const FHodgeLocomotionState& InState) { ReplayState = InState; bReplay = true; }
void UHodgeLocomotionPolicyComponent::ClearMoveReplayState() { bReplay = false; }
void UHodgeLocomotionPolicyComponent::BeginServerMove(int32 ActivationKey)
{
	ReplayState = GetResolvedPolicy(); bServerOverride = true;
	if (!State.bSprinting || ActivationKey != State.ActivationKey) { ReplayState.bSprinting = false; ReplayState.bPivotAllowed = false; }
}
void UHodgeLocomotionPolicyComponent::EndServerMove() { bServerOverride = false; }
void UHodgeLocomotionPolicyComponent::TickComponent(float DeltaTime, ELevelTick Type, FActorComponentTickFunction* Function)
{
	Super::TickComponent(DeltaTime, Type, Function);
	if (GetOwner()->HasAuthority()) { RefreshSpeedScale(); }
	for (auto It = DashExitVelocities.CreateIterator(); It; ++It)
	{ if (It.Value().ExpiresAt < GetWorld()->GetTimeSeconds()) { It.RemoveCurrent(); } }
	if (!ASC.IsValid() || ASC->GetAvatarActor() != GetOwner()) { return; }
	if (SprintHandle.IsValid() && (!SprintSource.IsValid() || !SprintSource->IsActive())) { ReleasePolicy(SprintHandle); }
}

void UHodgeLocomotionPolicyComponent::SetSprintInput(int32 SessionId, bool bHeld)
{
	const auto* Pawn = GetPawn<APawn>();
	if (!Pawn || (!Pawn->HasAuthority() && !Pawn->IsLocallyControlled()) || SessionId <= 0) { return; }
	SetInputInternal(SessionId, bHeld);
	if (!Pawn->HasAuthority()) { ServerSetSprintInput(GetOwner(), SessionId, bHeld); }
}
void UHodgeLocomotionPolicyComponent::SetInputInternal(int32 SessionId, bool bHeld)
{
	if (SessionId < InputSessionId || (SessionId == InputSessionId && bInputHeld == bHeld)) { return; }
	if (SessionId == InputSessionId && bHeld) { return; }
	InputSessionId = SessionId; bInputHeld = bHeld; InputPressedAt = GetWorld()->GetTimeSeconds();
	if (!bHeld) { HandoffSessionId = 0; }
}
void UHodgeLocomotionPolicyComponent::ServerSetSprintInput_Implementation(AActor* Avatar, int32 SessionId, bool bHeld)
{ if (ASC.IsValid() && ASC->GetAvatarActor() == Avatar && Avatar == GetOwner() && SessionId > 0) { SetInputInternal(SessionId, bHeld); } }
bool UHodgeLocomotionPolicyComponent::IsSessionHeld(int32 SessionId) const { return bInputHeld && SessionId == InputSessionId; }
float UHodgeLocomotionPolicyComponent::SessionHeldTime(int32 SessionId) const
{ return IsSessionHeld(SessionId) ? FMath::Max(0., GetWorld()->GetTimeSeconds() - InputPressedAt) : 0.f; }
void UHodgeLocomotionPolicyComponent::AuthorizeHandoff(int32 DashKey, int32 SessionId, float ValidFor)
{
	if (!IsSessionHeld(SessionId) || !FMath::IsFinite(ValidFor) || ValidFor <= 0.f) { return; }
	HandoffSessionId = SessionId; HandoffExpiresAt = GetWorld()->GetTimeSeconds() + ValidFor;
}
bool UHodgeLocomotionPolicyComponent::CanHandoff(int32 SessionId) const
{ return SessionId > 0 && SessionId == HandoffSessionId && IsSessionHeld(SessionId) && GetWorld()->GetTimeSeconds() <= HandoffExpiresAt; }
void UHodgeLocomotionPolicyComponent::ConsumeHandoff() { HandoffSessionId = 0; }
void UHodgeLocomotionPolicyComponent::RecordDashExitVelocity(FName Source, float MaximumSpeed)
{
	if (!ASC.IsValid() || ASC->GetAvatarActor() != GetOwner() || Source.IsNone() ||
		!FMath::IsFinite(MaximumSpeed) || MaximumSpeed <= 0.f) { return; }
	// 保存本次来源的结束策略，供最近保存帧重放；与技能开放窗口无关。
	DashExitVelocities.Add(Source, {MaximumSpeed, GetWorld()->GetTimeSeconds() + 1.});
}
bool UHodgeLocomotionPolicyComponent::GetDashExitVelocity(FName Source, float& OutMaximumSpeed) const
{
	const auto* Exit = DashExitVelocities.Find(Source);
	if (!Exit || !ASC.IsValid() || ASC->GetAvatarActor() != GetOwner() || Exit->ExpiresAt < GetWorld()->GetTimeSeconds()) { return false; }
	OutMaximumSpeed = Exit->MaximumSpeed; return true;
}
void UHodgeLocomotionPolicyComponent::OnRep_State()
{
	if (!State.bSprinting && SprintHandle.IsValid() && State.ActivationKey == PredictedState.ActivationKey)
	{ OnSprintAuthorityEnded.Broadcast(); }
}
void UHodgeLocomotionPolicyComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{ Super::GetLifetimeReplicatedProps(OutLifetimeProps); DOREPLIFETIME(UHodgeLocomotionPolicyComponent, State); }
void UHodgeLocomotionPolicyComponent::EndPlay(EEndPlayReason::Type Reason) { UninitializeFromAbilitySystem(); Super::EndPlay(Reason); }
