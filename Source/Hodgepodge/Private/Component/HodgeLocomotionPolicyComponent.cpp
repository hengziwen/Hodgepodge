/** @file HodgeLocomotionPolicyComponent.cpp
 * @brief 实现移动表现策略组件相关的类型、接口与现有处理流程。
 */
#include "Component/HodgeLocomotionPolicyComponent.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Data/HodgePawnData.h"
#include "Data/HodgeSprintAbilityProfile.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeLocomotionPolicyComponent)

/** 构造移动表现策略组件并初始化其默认状态。 */
UHodgeLocomotionPolicyComponent::UHodgeLocomotionPolicyComponent(const FObjectInitializer& Initializer) : Super(Initializer)
{
	PrimaryComponentTick.bCanEverTick = true; SetIsReplicatedByDefault(true);
}
/** 初始化关联能力System。 */
void UHodgeLocomotionPolicyComponent::InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC)
{
	if (ASC == InASC && InASC && InASC->GetAvatarActor() == GetOwner()) { return; }
	UninitializeFromAbilitySystem();
	if (!InASC || InASC->GetAvatarActor() != GetOwner()) { return; }
	ASC = InASC;
}
/** 解除初始化来源能力System。 */
void UHodgeLocomotionPolicyComponent::UninitializeFromAbilitySystem()
{
	ASC.Reset(); SprintSource.Reset(); SprintProfile = nullptr; SprintHandle.Invalidate();
	State = {}; ReplayState = {}; bReplay = bServerOverride = false;
	PredictedState = {}; SpeedModifiers.Reset();
	DashExitVelocities.Reset();
	InputSessionId = HandoffSessionId = 0; bInputHeld = false; OnSprintAuthorityEnded.Clear();
}
/** 获取配置。 */
const UHodgeSprintAbilityProfile* UHodgeLocomotionPolicyComponent::GetProfile() const
{
	if (OverrideProfile) { return OverrideProfile; }
	const auto* Extension = UHodgePawnExtensionComponent::FindPawnExtensionComponent(GetOwner());
	const auto* Data = Extension ? Extension->GetPawnData<UHodgePawnData>() : nullptr;
	return Data ? Data->SprintAbilityProfile.Get() : nullptr;
}
/** 申请当前冲刺移动策略的会话占用。 */
FGuid UHodgeLocomotionPolicyComponent::AcquireSprint(UGameplayAbility* Source, const UHodgeSprintAbilityProfile* Profile, int32 ActivationKey)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!ASC.IsValid() || ASC->GetAvatarActor() != GetOwner() || !Source || !Source->IsActive() ||
		Source->GetAvatarActorFromActorInfo() != GetOwner() || !Profile || SprintHandle.IsValid()) { return {}; }
	SprintSource = Source; SprintProfile = Profile; SprintHandle = FGuid::NewGuid();
	State.ActivationKey = ActivationKey; ++State.Version; State.bSprinting = true;
	State.MaxSpeed = Profile->SprintMaxSpeed; State.Acceleration = Profile->SprintAcceleration;
	State.Braking = Profile->SprintBraking; State.TurnRate = Profile->SprintTurnRate; State.bPivotAllowed = Profile->bAllowSprintPivot;
	PredictedState = State;
	GetOwner()->ForceNetUpdate(); return SprintHandle;
}
/** 释放策略。 */
void UHodgeLocomotionPolicyComponent::ReleasePolicy(FGuid Handle)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!Handle.IsValid() || Handle != SprintHandle) { return; }
	SprintHandle.Invalidate(); SprintSource.Reset(); SprintProfile = nullptr;
	State.bSprinting = false; State.bPivotAllowed = false; ++State.Version; GetOwner()->ForceNetUpdate();
	PredictedState.bSprinting = false; PredictedState.bPivotAllowed = false;
	if (bServerOverride) { ReplayState.bSprinting = false; ReplayState.bPivotAllowed = false; }
}
/** 获取Resolved策略。 */
FHodgeLocomotionState UHodgeLocomotionPolicyComponent::GetResolvedPolicy() const
{
	if (bReplay || bServerOverride) { return ReplayState; }
	// 当前操作关联的Pawn角色。
	const auto* Pawn = GetPawn<APawn>();
	const bool bOwningClient = Pawn && Pawn->IsLocallyControlled() && !Pawn->HasAuthority();
	FHodgeLocomotionState Result = bOwningClient && SprintHandle.IsValid() ? PredictedState : State;
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (bOwningClient && !SprintHandle.IsValid()) { Result.bSprinting = false; Result.bPivotAllowed = false; }
	Result.SpeedScale = State.SpeedScale; Result.MaxSpeed *= State.SpeedScale;
	return Result;
}
/** 按来源登记移动速度修正，避免覆盖其他来源。 */
FGuid UHodgeLocomotionPolicyComponent::AcquireSpeedModifier(UObject* Source, float Multiplier)
{
	// 按服务器权限区分本次处理分支。
	if (!GetOwner()->HasAuthority() || !ASC.IsValid() || !IsValid(Source) || !FMath::IsFinite(Multiplier) || Multiplier < 0.f || Multiplier > 4.f || SpeedModifiers.Num() >= 32) { return {}; }
	const auto* Ability = Cast<UGameplayAbility>(Source);
	// 根据当前生命周期状态决定是否继续执行。
	if (Source != GetOwner() && !Source->IsIn(GetOwner()) && !(Ability && Ability->IsActive() && Ability->GetAvatarActorFromActorInfo() == GetOwner())) { return {}; }
	const FGuid Handle = FGuid::NewGuid(); SpeedModifiers.Add(Handle, {Source, Multiplier}); RefreshSpeedScale(); return Handle;
}
/** 释放速度Modifier。 */
void UHodgeLocomotionPolicyComponent::ReleaseSpeedModifier(FGuid Handle)
{ if (GetOwner()->HasAuthority() && SpeedModifiers.Remove(Handle)) { RefreshSpeedScale(); } }
/** 刷新速度缩放。 */
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
/** 设置移动回放状态。 */
void UHodgeLocomotionPolicyComponent::SetMoveReplayState(const FHodgeLocomotionState& InState) { ReplayState = InState; bReplay = true; }
/** 清空移动回放状态。 */
void UHodgeLocomotionPolicyComponent::ClearMoveReplayState() { bReplay = false; }
/** 开始服务器移动。 */
void UHodgeLocomotionPolicyComponent::BeginServerMove(int32 ActivationKey)
{
	ReplayState = GetResolvedPolicy(); bServerOverride = true;
	if (!State.bSprinting || ActivationKey != State.ActivationKey) { ReplayState.bSprinting = false; ReplayState.bPivotAllowed = false; }
}
/** 结束服务器移动。 */
void UHodgeLocomotionPolicyComponent::EndServerMove() { bServerOverride = false; }
/** 更新组件。 */
void UHodgeLocomotionPolicyComponent::TickComponent(float DeltaTime, ELevelTick Type, FActorComponentTickFunction* Function)
{
	Super::TickComponent(DeltaTime, Type, Function);
	// 按服务器权限区分本次处理分支。
	if (GetOwner()->HasAuthority()) { RefreshSpeedScale(); }
	for (auto It = DashExitVelocities.CreateIterator(); It; ++It)
	{ if (It.Value().ExpiresAt < GetWorld()->GetTimeSeconds()) { It.RemoveCurrent(); } }
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!ASC.IsValid() || ASC->GetAvatarActor() != GetOwner()) { return; }
	if (SprintHandle.IsValid() && (!SprintSource.IsValid() || !SprintSource->IsActive())) { ReleasePolicy(SprintHandle); }
}

/** 设置Sprint奔跑输入。 */
void UHodgeLocomotionPolicyComponent::SetSprintInput(int32 SessionId, bool bHeld)
{
	// 当前操作关联的Pawn角色。
	const auto* Pawn = GetPawn<APawn>();
	// 按服务器权限区分本次处理分支。
	if (!Pawn || (!Pawn->HasAuthority() && !Pawn->IsLocallyControlled()) || SessionId <= 0) { return; }
	SetInputInternal(SessionId, bHeld);
	if (!Pawn->HasAuthority()) { ServerSetSprintInput(GetOwner(), SessionId, bHeld); }
}
/** 设置输入内部。 */
void UHodgeLocomotionPolicyComponent::SetInputInternal(int32 SessionId, bool bHeld)
{
	if (SessionId < InputSessionId || (SessionId == InputSessionId && bInputHeld == bHeld)) { return; }
	if (SessionId == InputSessionId && bHeld) { return; }
	InputSessionId = SessionId; bInputHeld = bHeld; InputPressedAt = GetWorld()->GetTimeSeconds();
	if (!bHeld) { HandoffSessionId = 0; }
}
/** 处理服务器侧的设置Sprint奔跑输入消息，沿用当前身份与权限校验。 */
void UHodgeLocomotionPolicyComponent::ServerSetSprintInput_Implementation(AActor* Avatar, int32 SessionId, bool bHeld)
{ if (ASC.IsValid() && ASC->GetAvatarActor() == Avatar && Avatar == GetOwner() && SessionId > 0) { SetInputInternal(SessionId, bHeld); } }
/** 判断会话按住。 */
bool UHodgeLocomotionPolicyComponent::IsSessionHeld(int32 SessionId) const { return bInputHeld && SessionId == InputSessionId; }
/** 计算当前输入会话的有效按住时间。 */
float UHodgeLocomotionPolicyComponent::SessionHeldTime(int32 SessionId) const
{ return IsSessionHeld(SessionId) ? FMath::Max(0., GetWorld()->GetTimeSeconds() - InputPressedAt) : 0.f; }
/** 记录满足条件的动作交接授权。 */
void UHodgeLocomotionPolicyComponent::AuthorizeHandoff(int32 DashKey, int32 SessionId, float ValidFor)
{
	if (!IsSessionHeld(SessionId) || !FMath::IsFinite(ValidFor) || ValidFor <= 0.f) { return; }
	HandoffSessionId = SessionId; HandoffExpiresAt = GetWorld()->GetTimeSeconds() + ValidFor;
}
/** 检查是否允许Handoff。 */
bool UHodgeLocomotionPolicyComponent::CanHandoff(int32 SessionId) const
{ return SessionId > 0 && SessionId == HandoffSessionId && IsSessionHeld(SessionId) && GetWorld()->GetTimeSeconds() <= HandoffExpiresAt; }
/** 消费Handoff。 */
void UHodgeLocomotionPolicyComponent::ConsumeHandoff() { HandoffSessionId = 0; }
/** 记录当前Dash会话结束时的速度恢复策略。 */
void UHodgeLocomotionPolicyComponent::RecordDashExitVelocity(FName Source, float MaximumSpeed)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!ASC.IsValid() || ASC->GetAvatarActor() != GetOwner() || Source.IsNone() ||
		!FMath::IsFinite(MaximumSpeed) || MaximumSpeed <= 0.f) { return; }
	// 保存本次来源的结束策略，供最近保存帧重放；与技能开放窗口无关。
	DashExitVelocities.Add(Source, {MaximumSpeed, GetWorld()->GetTimeSeconds() + 1.});
}
/** 获取Dash冲刺退出速度。 */
bool UHodgeLocomotionPolicyComponent::GetDashExitVelocity(FName Source, float& OutMaximumSpeed) const
{
	const auto* Exit = DashExitVelocities.Find(Source);
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!Exit || !ASC.IsValid() || ASC->GetAvatarActor() != GetOwner() || Exit->ExpiresAt < GetWorld()->GetTimeSeconds()) { return false; }
	OutMaximumSpeed = Exit->MaximumSpeed; return true;
}
/** 收到状态复制后更新本地状态。 */
void UHodgeLocomotionPolicyComponent::OnRep_State()
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!State.bSprinting && SprintHandle.IsValid() && State.ActivationKey == PredictedState.ActivationKey)
	{ OnSprintAuthorityEnded.Broadcast(); }
}
/** 登记本类型需要网络复制的属性及复制条件。 */
void UHodgeLocomotionPolicyComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{ Super::GetLifetimeReplicatedProps(OutLifetimeProps); DOREPLIFETIME(UHodgeLocomotionPolicyComponent, State); }
/** 离开世界运行阶段并解除本对象持有的监听或资源。 */
void UHodgeLocomotionPolicyComponent::EndPlay(EEndPlayReason::Type Reason) { UninitializeFromAbilitySystem(); Super::EndPlay(Reason); }
