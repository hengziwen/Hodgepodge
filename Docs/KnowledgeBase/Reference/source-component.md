# Component 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeActorComponentBase.cpp

通用 ActorComponent 基础访问与扩展。

源码：[Source/Hodgepodge/Private/Component/HodgeActorComponentBase.cpp](../../../Source/Hodgepodge/Private/Component/HodgeActorComponentBase.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeActorComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeActorComponentBase.h)

定义候选（多行签名仅展示首行）：

- L24: `UHodgeActorComponentBase::UHodgeActorComponentBase()`
- L40: `void UHodgeActorComponentBase::BeginPlay()`
- L58: `void UHodgeActorComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)`
- L70: `AActor* UHodgeActorComponentBase::GetOwnerCharacter()`
- L81: `void UHodgeActorComponentBase::InitializeComponent()`
- L92: `void UHodgeActorComponentBase::OnReady()`

## HodgeAttackMotionWarpingComponent.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Component/HodgeAttackMotionWarpingComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeAttackMotionWarpingComponent.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeAttackMotionWarpingComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeAttackMotionWarpingComponent.h)、[Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)、[Animation/HodgeRootMotionModifier_AttackFacing.h](../../../Source/Hodgepodge/Public/Animation/HodgeRootMotionModifier_AttackFacing.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)

定义候选（多行签名仅展示首行）：

- L15: `void UHodgeAttackMotionWarpingComponent::InitializeComponent()`
- L19: `void UHodgeAttackMotionWarpingComponent::ClearAttackWarp()`
- L26: `void UHodgeAttackMotionWarpingComponent::PrepareAttackEvaluation(UMotionWarpingComponent*)`
- L47: `void UHodgeAttackMotionWarpingComponent::PrepareAttackTarget(const FHodgeCharacterRotationState& State, const UAnimMontage* Montage, float Position)`

## HodgeCharacterMovementComponent.cpp

地面距离/加速度、最终旋转过滤、SavedMove 旋转策略重放与权威校正。

源码：[Source/Hodgepodge/Private/Component/HodgeCharacterMovementComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeCharacterMovementComponent.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)、[Component/HodgeLockOnComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLockOnComponent.h)、[Combat/HodgeHitReactionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitReactionTypes.h)、[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)、[Component/HodgeLocomotionPolicyComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLocomotionPolicyComponent.h)、[Data/HodgeSprintAbilityProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeSprintAbilityProfile.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

定义候选（多行签名仅展示首行）：

- L200: `UHodgeCharacterMovementComponent::UHodgeCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)`
- L208: `void UHodgeCharacterMovementComponent::PerformMovement(float DeltaTime)`
- L238: `void UHodgeCharacterMovementComponent::ClientHandleMoveResponse(const FCharacterMoveResponseDataContainer& MoveResponse)`
- L265: `void UHodgeCharacterMovementComponent::ClientAdjustPosition_Implementation(float TimeStamp, FVector NewLoc, FVector NewVel,`
- L275: `void UHodgeCharacterMovementComponent::SimulateMovement(float DeltaTime)`
- L296: `bool UHodgeCharacterMovementComponent::CanAttemptJump() const`
- L306: `void UHodgeCharacterMovementComponent::InitializeComponent()`
- L313: `const FHodgeCharacterGroundInfo& UHodgeCharacterMovementComponent::GetGroundInfo()`
- L394: `void UHodgeCharacterMovementComponent::SetReplicatedAcceleration(const FVector& InAcceleration)`
- L404: `FRotator UHodgeCharacterMovementComponent::GetDeltaRotation(float DeltaTime) const`
- L433: `void UHodgeCharacterMovementComponent::PhysicsRotation(float DeltaTime)`
- L489: `void UHodgeCharacterMovementComponent::ServerMove_PerformMovement(const FCharacterNetworkMoveData& MoveData)`
- L505: `bool UHodgeCharacterMovementComponent::MoveUpdatedComponentImpl(const FVector& Delta, const FQuat& NewRotation,`
- L525: `FNetworkPredictionData_Client* UHodgeCharacterMovementComponent::GetPredictionData_Client() const`
- L537: `bool UHodgeCharacterMovementComponent::ClientUpdatePositionAfterServerUpdate()`
- L552: `void UHodgeCharacterMovementComponent::SmoothCorrection(const FVector& OldLocation, const FQuat& OldRotation,`
- L560: `float UHodgeCharacterMovementComponent::GetMaxSpeed() const`
- L580: `float UHodgeCharacterMovementComponent::GetMaxAcceleration() const`
- L587: `float UHodgeCharacterMovementComponent::GetMaxBrakingDeceleration() const`
- L594: `FVector UHodgeCharacterMovementComponent::ConstrainInputAcceleration(const FVector& InputAcceleration) const`
- L608: `bool UHodgeCharacterMovementComponent::IsHitReactionControlled() const`
- L617: `bool UHodgeCharacterMovementComponent::ApplyRequestedMove(float DeltaTime, float MaxAccel, float MaxSpeed, float Friction,`

## HodgeCharacterRotationComponent.cpp

ASC 旋转标签约束、锁定 Yaw、恢复、权威复制与移动重放状态。

源码：[Source/Hodgepodge/Private/Component/HodgeCharacterRotationComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeCharacterRotationComponent.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)、[Component/HodgeLockOnComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLockOnComponent.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.h)、[Combat/HodgeHitReactionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitReactionTypes.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)

定义候选（多行签名仅展示首行）：

- L22: `UHodgeCharacterRotationComponent::UHodgeCharacterRotationComponent(const FObjectInitializer& ObjectInitializer)`
- L32: `void UHodgeCharacterRotationComponent::InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC)`
- L64: `void UHodgeCharacterRotationComponent::UninitializeFromAbilitySystem()`
- L112: `void UHodgeCharacterRotationComponent::HandleConstraintTagChanged(FGameplayTag Tag, int32 NewCount)`
- L118: `void UHodgeCharacterRotationComponent::RefreshLocalState()`
- L143: `void UHodgeCharacterRotationComponent::PublishAuthorityState()`
- L163: `bool UHodgeCharacterRotationComponent::IsReplayingMove() const`
- L170: `FHodgeCharacterRotationState UHodgeCharacterRotationComponent::GetResolvedState() const`
- L182: `bool UHodgeCharacterRotationComponent::IsYawLocked() const { return GetResolvedState().bYawLocked; }`
- L184: `bool UHodgeCharacterRotationComponent::IsRecoveringFacing() const { return GetResolvedState().bRecoveringFacing; }`
- L186: `float UHodgeCharacterRotationComponent::GetLockedYaw() const { return GetResolvedState().LockedYaw; }`
- L189: `FRotator UHodgeCharacterRotationComponent::FilterControlRotation(const FRotator& DesiredRotation, float DeltaSeconds) const`
- L206: `void UHodgeCharacterRotationComponent::NotifyFacingApplied(float DesiredYaw)`
- L225: `void UHodgeCharacterRotationComponent::SetMoveReplayState(const FHodgeCharacterRotationState& State)`
- L233: `void UHodgeCharacterRotationComponent::ClearMoveReplayState()`
- L241: `void UHodgeCharacterRotationComponent::OnRep_RotationState()`
- L254: `void UHodgeCharacterRotationComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L261: `void UHodgeCharacterRotationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L268: `bool UHodgeCharacterRotationComponent::IsFacingSystemReady() const`
- L275: `bool UHodgeCharacterRotationComponent::IsValidSource(const UObject* Source, bool bAction) const`
- L288: `bool UHodgeCharacterRotationComponent::HasPendingPrediction() const`
- L296: `bool UHodgeCharacterRotationComponent::IsAuthorityActionCurrent() const`
- L314: `FHodgeFacingRequestHandle UHodgeCharacterRotationComponent::AcquireBaseFacingMode(EHodgeCharacterFacingDriver Driver, UObject* Source)`
- L328: `FHodgeFacingRequestHandle UHodgeCharacterRotationComponent::AcquireLockOnFacing(UHodgeLockOnComponent* Source)`
- L337: `FHodgeFacingRequestHandle UHodgeCharacterRotationComponent::RequestActionFacing(FVector WorldDirection, UObject* Source,`
- L352: `FHodgeFacingRequestHandle UHodgeCharacterRotationComponent::RequestMontageWarpFacing(float TargetYaw, UObject* Source, FGuid ExecutionId)`
- L365: `bool UHodgeCharacterRotationComponent::SetFacingRequestSuspended(FHodgeFacingRequestHandle Handle, bool bSuspended)`
- L371: `bool UHodgeCharacterRotationComponent::IsFacingRequestSelected(FHodgeFacingRequestHandle Handle) const`
- L378: `FHodgeFacingRequestHandle UHodgeCharacterRotationComponent::AddRequest(EHodgeCharacterFacingDriver Driver, UObject* Source,`
- L404: `bool UHodgeCharacterRotationComponent::ReleaseFacingRequest(FHodgeFacingRequestHandle Handle)`
- L421: `void UHodgeCharacterRotationComponent::ReleaseRequestsForSource(const UObject* Source)`
- L430: `EHodgeFacingRequestStatus UHodgeCharacterRotationComponent::GetFacingRequestStatus(FHodgeFacingRequestHandle Handle) const`
- L440: `bool UHodgeCharacterRotationComponent::IsActionFacingApplied(FHodgeFacingRequestHandle Handle) const`
- L453: `void UHodgeCharacterRotationComponent::SetRequestStatus(const FGuid& Id, EHodgeFacingRequestStatus Status)`
- L464: `void UHodgeCharacterRotationComponent::ResolveRequests()`
- L530: `void UHodgeCharacterRotationComponent::ApplyResolvedMode()`
- L543: `FHodgeFacingPresentationSnapshot UHodgeCharacterRotationComponent::GetFacingPresentationSnapshot() const`
- L552: `void UHodgeCharacterRotationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* Tick)`
- L585: `void UHodgeCharacterRotationComponent::SendRequest(const FGuid& Id, bool bRelease)`
- L598: `void UHodgeCharacterRotationComponent::ServerFacingRequest_Implementation(AActor* Avatar, uint32 Sequence, FGuid Id,`
- L652: `void UHodgeCharacterRotationComponent::ClientFacingResult_Implementation(uint32 Sequence, FGuid Id, bool bAccepted,`
- L664: `bool UHodgeCharacterRotationComponent::BeginServerMove(uint32 Sequence, int32 MontageWarpKey)`
- L688: `void UHodgeCharacterRotationComponent::EndServerMove()`
- L696: `EDataValidationResult UHodgeCharacterRotationComponent::IsDataValid(FDataValidationContext& Context) const`

## HodgeCombatComponentBase.cpp

Experience 注入 Pawn 的统一战斗协调者；连段输入/记忆/预测校正、来源/配置检测体、独立会话与过滤；GA 负责目标去重及效果施加。

源码：[Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp](../../../Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)、[AbilitySystem/AttributeSet/HodgeHavocSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHavocSet.h)、[Component/HodgeEnemyInitComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeEnemyInitComponent.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[Character/HodgeHeroCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeHeroCharacter.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)、[Combat/HodgeHitReactionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitReactionTypes.h)、[Equipment/HodgeEquipmentManagerComponent.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h)、[Equipment/HodgeWeaponInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)、[Data/HodgeComboDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeComboDefinition.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)

定义候选（多行签名仅展示首行）：

- L61: `UHodgeCombatComponentBase::UHodgeCombatComponentBase()`
- L70: `FGuid UHodgeCombatComponentBase::AcquirePoseLease()`
- L88: `void UHodgeCombatComponentBase::ReleasePoseLease(FGuid Handle)`
- L101: `void UHodgeCombatComponentBase::EndPlay(const EEndPlayReason::Type Reason)`
- L110: `bool UHodgeCombatComponentBase::ResolveSource(FGameplayTag Tag, FHodgeHitDetectionSession& Session) const`
- L146: `bool UHodgeCombatComponentBase::IsSourceValid(const FHodgeHitDetectionSession& Session) const`
- L176: `bool UHodgeCombatComponentBase::CaptureDetectionGeometry(const FHodgeHitDetectionSession& Session,`
- L222: `uint64 UHodgeCombatComponentBase::CreateDetectionSession(const FGuid& ExecutionId, int32 OccurrenceId,`
- L283: `bool UHodgeCombatComponentBase::IsDetectionSessionValid(uint64 Handle, const FGuid& ExecutionId) const`
- L290: `bool UHodgeCombatComponentBase::ResetDetectionHistory(uint64 Handle, const FGuid& ExecutionId)`
- L303: `USceneComponent* UHodgeCombatComponentBase::GetDetectionSourceComponent(uint64 Handle, const FGuid& ExecutionId) const`
- L312: `void UHodgeCombatComponentBase::EndDetectionSession(uint64 Handle, const FGuid& ExecutionId)`
- L319: `void UHodgeCombatComponentBase::EndDetectionSessionsForExecution(const FGuid& ExecutionId)`
- L328: `void UHodgeCombatComponentBase::EndAllDetectionSessions()`
- L334: `void UHodgeCombatComponentBase::UpdateDetectionAnchor(const FGuid& ExecutionId, FName Key, const FTransform& Transform)`
- L352: `bool UHodgeCombatComponentBase::SampleDetection(uint64 Handle, const FGuid& ExecutionId,`
- L476: `void UHodgeCombatComponentBase::Configure(UHodgeAbilitySystemComponent* InASC,`
- L499: `void UHodgeCombatComponentBase::Shutdown()`
- L541: `void UHodgeCombatComponentBase::ConsumeBufferedInput()`
- L547: `void UHodgeCombatComponentBase::ClearInput()`
- L555: `bool UHodgeCombatComponentBase::InputPressed(FGameplayTag InputTag)`
- L568: `const FHodgeComboTransition* UHodgeCombatComponentBase::SelectTransition(FGameplayTag Trigger, bool bEvent) const`
- L591: `bool UHodgeCombatComponentBase::IsAuthorized(FGameplayAbilitySpecHandle Handle) const`
- L597: `bool UHodgeCombatComponentBase::IsMoveCancelPredicted() const`
- L607: `UHodgeGameplayAbility_Definition* UHodgeCombatComponentBase::GetMoveCancelAbility() const`
- L618: `bool UHodgeCombatComponentBase::GetMoveCancelPolicy(const UHodgeGameplayAbility_Definition* Ability,`
- L638: `void UHodgeCombatComponentBase::ClearMoveCancelRequest()`
- L645: `void UHodgeCombatComponentBase::UpdateMoveCancel()`
- L673: `int16 UHodgeCombatComponentBase::ExecutionKey() const`
- L682: `bool UHodgeCombatComponentBase::PrepareTransition(const FHodgeComboTransition& Edge, FGameplayAbilitySpecHandle Handle)`
- L721: `bool UHodgeCombatComponentBase::TryTransition(FGameplayTag Trigger, bool bEvent)`
- L773: `float UHodgeCombatComponentBase::GetServerInputBufferSeconds() const`
- L779: `bool UHodgeCombatComponentBase::ValidateServerRequestIdentity(const FGameplayEventData* Payload) const`
- L790: `bool UHodgeCombatComponentBase::CanBufferServerActivation(FGameplayAbilitySpecHandle Handle,`
- L811: `bool UHodgeCombatComponentBase::PrepareServerActivation(FGameplayAbilitySpecHandle Handle,`
- L825: `bool UHodgeCombatComponentBase::PrepareConfirmedActivation(FGameplayAbilitySpecHandle Handle,`
- L850: `void UHodgeCombatComponentBase::RejectServerActivation(const FGameplayEventData* Payload)`
- L857: `void UHodgeCombatComponentBase::CompleteServerActivation()`
- L884: `void UHodgeCombatComponentBase::ExecutionStarted(UHodgeGameplayAbility_Definition* Ability)`
- L897: `void UHodgeCombatComponentBase::ExecutionEnded(UHodgeGameplayAbility_Definition* Ability)`
- L914: `void UHodgeCombatComponentBase::WindowsChanged(UHodgeGameplayAbility_Definition* Ability)`
- L926: `void UHodgeCombatComponentBase::ExecutionEvent(UHodgeGameplayAbility_Definition* Ability, FGameplayTag Event)`
- L935: `void UHodgeCombatComponentBase::DrainEvents()`
- L954: `void UHodgeCombatComponentBase::SetNode(FGameplayTag Node)`
- L971: `void UHodgeCombatComponentBase::ResetSession(bool bEndAbility)`
- L987: `double UHodgeCombatComponentBase::ComboTime() const`
- L996: `FGameplayTag UHodgeCombatComponentBase::GetRememberedComboTag() const`
- L1004: `float UHodgeCombatComponentBase::GetComboMemoryRemainingTime() const`
- L1012: `FGameplayTag UHodgeCombatComponentBase::TransitionSourceNode() const`
- L1020: `bool UHodgeCombatComponentBase::HasResumeTransition(FGameplayTag Node) const`
- L1030: `void UHodgeCombatComponentBase::RetainComboMemory()`
- L1042: `void UHodgeCombatComponentBase::ExpireComboMemory()`
- L1054: `void UHodgeCombatComponentBase::PublishComboMemory()`
- L1065: `void UHodgeCombatComponentBase::EndCurrentExecution()`
- L1072: `void UHodgeCombatComponentBase::OnRep_ComboMemory()`
- L1082: `void UHodgeCombatComponentBase::HandlePredictionRejected()`
- L1092: `void UHodgeCombatComponentBase::ServerSynchronizeComboMemory_Implementation()`
- L1099: `void UHodgeCombatComponentBase::ClientCorrectComboMemory_Implementation(FHodgeComboMemoryState State)`
- L1106: `void UHodgeCombatComponentBase::TickComponent(float Delta, ELevelTick Type, FActorComponentTickFunction* Tick)`
- L1129: `void UHodgeCombatComponentBase::ServerMoveCancel_Implementation(AActor* Avatar, FGameplayAbilitySpecHandle Handle,`
- L1142: `void UHodgeCombatComponentBase::ProcessServerMoveCancel(TWeakObjectPtr<AActor> Avatar,`
- L1165: `void UHodgeCombatComponentBase::ClientMoveCancelResult_Implementation(AActor* Avatar, FGameplayAbilitySpecHandle Handle,`
- L1188: `void UHodgeCombatComponentBase::ServerReturnToEntry_Implementation(AActor* Avatar, FGameplayTag SourceNode, int32 Key,`
- L1201: `void UHodgeCombatComponentBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L1209: `void UHodgeCombatComponentBase::OnRep_ObserverTags(const FGameplayTagContainer& Previous)`
- L1222: `UHodgeCombatComponentBase* UHodgeCombatComponentBase::FindCombatComponent(const AActor* Avatar)`
- L1228: `bool UHodgeCombatComponentBase::IsComboReady() const`
- L1237: `void UHodgeCombatComponentBase::BindPawnExtension()`
- L1249: `void UHodgeCombatComponentBase::HandleAbilitySystemInitialized()`
- L1259: `void UHodgeCombatComponentBase::HandleAbilitySystemUninitialized()`
- L1265: `void UHodgeCombatComponentBase::OnRegister()`
- L1273: `void UHodgeCombatComponentBase::BeginPlay()`
- L1281: `void UHodgeCombatComponentBase::OnUnregister()`
- L1292: `bool UHodgeCombatComponentBase::IsCombatReadyForActions() const`
- L1310: `FHodgeCombatActionRequestState UHodgeCombatComponentBase::AddActionRequest(EHodgeCombatActionRequestType Type, FGameplayTag Tag)`
- L1331: `bool UHodgeCombatComponentBase::GetCombatActionRequest(FGuid Id, FHodgeCombatActionRequestState& OutState) const`
- L1338: `void UHodgeCombatComponentBase::SetActionRequestStatus(FGuid Id, EHodgeCombatActionRequestStatus Status, const FString& Reason)`
- L1364: `FHodgeCombatActionRequestState UHodgeCombatComponentBase::RequestStandaloneAbility(FGameplayTag Tag)`
- L1393: `FHodgeCombatActionRequestState UHodgeCombatComponentBase::SubmitCombatIntent(FGameplayTag Tag)`
- L1429: `void UHodgeCombatComponentBase::BindActionRequest(FGuid Id, FGameplayAbilitySpecHandle Handle)`
- L1440: `void UHodgeCombatComponentBase::ConfirmActionRequest(FGuid Id, bool bActivated)`
- L1455: `void UHodgeCombatComponentBase::HandleActionAbilityEnded(const FAbilityEndedData& Data)`
- L1472: `bool UHodgeCombatComponentBase::CancelCombatActionRequest(FGuid Id)`
- L1492: `void UHodgeCombatComponentBase::CloseActionRequests()`
- L1505: `void UHodgeCombatComponentBase::InvalidateModeContinuation(FGuid ModeSessionId)`
- L1511: `void UHodgeCombatComponentBase::HandleCombatModeChanged(FGameplayTag Tag,int32 Count)`
- L1519: `void UHodgeCombatComponentBase::ClearModeContinuation()`

## HodgeDefenseComponent.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Component/HodgeDefenseComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeDefenseComponent.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeDefenseComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeDefenseComponent.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Combat/HodgeMovementActionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeMovementActionTypes.h)、[Data/HodgeSprintAbilityProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeSprintAbilityProfile.h)

定义候选（多行签名仅展示首行）：

- L15: `void UHodgeDefenseComponent::InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC)`
- L22: `void UHodgeDefenseComponent::UninitializeFromAbilitySystem() { Windows.Reset(); ASC.Reset(); }`
- L24: `FGuid UHodgeDefenseComponent::RegisterDodgeWindow(UGameplayAbility* Source, FGuid Execution, const UHodgeSprintAbilityProfile* Profile, double StartTime)`
- L35: `void UHodgeDefenseComponent::UnregisterDodgeWindow(FGuid Handle) { Windows.Remove(Handle); }`
- L37: `EHodgeIncomingHitOutcome UHodgeDefenseComponent::EvaluateWindow(const UHodgeSprintAbilityProfile& Profile, double Age, bool bRewarded, bool bCanBeDodged, bool bPerfectEligible)`
- L43: `EHodgeIncomingHitOutcome UHodgeDefenseComponent::ResolveIncomingHit(AActor* Source, FGuid AttackExecution, int32 HitIndex, bool bCanBeDodged, bool bCanTriggerPerfect)`
- L83: `void UHodgeDefenseComponent::EndPlay(EEndPlayReason::Type Reason) { UninitializeFromAbilitySystem(); Super::EndPlay(Reason); }`

## HodgeEnemyInitComponent.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Component/HodgeEnemyInitComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeEnemyInitComponent.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeEnemyInitComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeEnemyInitComponent.h)、[Character/HodgeEnemyCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeEnemyCharacter.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Component/HodgeExperienceManagerComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)、[Combat/HodgeHitReactionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitReactionTypes.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)、[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)、[Data/HodgeCharacterStatProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeCharacterStatProfile.h)、[Equipment/HodgeEquipmentDefinition.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentDefinition.h)、[Equipment/HodgeEquipmentManagerComponent.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h)

定义候选（多行签名仅展示首行）：

- L23: `const FName UHodgeEnemyInitComponent::NAME_ActorFeatureName(TEXT("EnemyInit"));`
- L26: `UHodgeEnemyInitComponent::UHodgeEnemyInitComponent(const FObjectInitializer& Initializer) : Super(Initializer)`
- L33: `void UHodgeEnemyInitComponent::OnRegister()`
- L40: `void UHodgeEnemyInitComponent::OnUnregister()`
- L53: `void UHodgeEnemyInitComponent::BeginPlay()`
- L64: `void UHodgeEnemyInitComponent::BindDependencies()`
- L87: `void UHodgeEnemyInitComponent::HandleGameStateSet(AGameStateBase*) { BindDependencies(); TryInitialize(); }`
- L89: `void UHodgeEnemyInitComponent::HandleExperienceLoaded(const UHodgeExperienceDefinition*)`
- L95: `void UHodgeEnemyInitComponent::CheckDefaultInitialization() { TryInitialize(); }`
- L97: `void UHodgeEnemyInitComponent::OnActorInitStateChanged(const FActorInitStateChangedParams&) { TryInitialize(); }`
- L100: `bool UHodgeEnemyInitComponent::CanChangeInitState(UGameFrameworkComponentManager*, FGameplayTag Current, FGameplayTag Desired) const`
- L115: `EHodgeEnemyInitializationResult UHodgeEnemyInitComponent::TryInitialize()`
- L132: `EHodgeEnemyInitializationResult UHodgeEnemyInitComponent::InitializeStep()`
- L239: `bool UHodgeEnemyInitComponent::IsBindingCurrent() const`
- L247: `bool UHodgeEnemyInitComponent::IsGameplayReady() const`
- L255: `bool UHodgeEnemyInitComponent::HasValidCombatDependencies() const`
- L265: `bool UHodgeEnemyInitComponent::CanExecuteAIAction() const`
- L275: `EHodgeEnemyInitializationResult UHodgeEnemyInitComponent::Wait(const TCHAR* Reason)`
- L279: `EHodgeEnemyInitializationResult UHodgeEnemyInitComponent::Fail(const FString& Reason)`
- L288: `void UHodgeEnemyInitComponent::HandleASCUninitialized()`
- L296: `void UHodgeEnemyInitComponent::Cleanup()`
- L309: `void UHodgeEnemyInitComponent::EndPlay(EEndPlayReason::Type Reason)`

## HodgeExperienceManagerComponent.cpp

GameState 上的 Experience 复制、资源加载、插件激活、Action 执行与 Loaded 委托。

源码：[Source/Hodgepodge/Private/Component/HodgeExperienceManagerComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeExperienceManagerComponent.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeExperienceManagerComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h)、[Data/HodgeExperienceDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceDefinition.h)、[Data/HodgeExperienceActionSet.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceActionSet.h)、[Data/HodgeExperienceManager.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceManager.h)、[Data/HodgeAssetManager.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManager.h)

定义候选（多行签名仅展示首行）：

- L66: `UHodgeExperienceManagerComponent::UHodgeExperienceManagerComponent(const FObjectInitializer& ObjectInitializer)`
- L73: `void UHodgeExperienceManagerComponent::SetCurrentExperience(FPrimaryAssetId ExperienceId)`
- L104: `void UHodgeExperienceManagerComponent::CallOrRegister_OnExperienceLoaded_HighPriority(`
- L120: `void UHodgeExperienceManagerComponent::CallOrRegister_OnExperienceLoaded(FOnHodgeExperienceLoaded::FDelegate&& Delegate)`
- L135: `void UHodgeExperienceManagerComponent::CallOrRegister_OnExperienceLoaded_LowPriority(`
- L151: `const UHodgeExperienceDefinition* UHodgeExperienceManagerComponent::GetCurrentExperienceChecked() const`
- L164: `bool UHodgeExperienceManagerComponent::IsExperienceLoaded() const`
- L171: `void UHodgeExperienceManagerComponent::OnRep_CurrentExperience()`
- L178: `void UHodgeExperienceManagerComponent::StartExperienceLoad()`
- L317: `void UHodgeExperienceManagerComponent::OnExperienceLoadComplete()`
- L402: `void UHodgeExperienceManagerComponent::OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result)`
- L415: `void UHodgeExperienceManagerComponent::OnExperienceFullLoadCompleted()`
- L514: `void UHodgeExperienceManagerComponent::OnActionDeactivationCompleted()`
- L530: `void UHodgeExperienceManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L539: `void UHodgeExperienceManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L630: `bool UHodgeExperienceManagerComponent::ShouldShowLoadingScreen(FString& OutReason) const`
- L648: `void UHodgeExperienceManagerComponent::OnAllActionsDeactivated()`

## HodgeHealthComponent.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Component/HodgeHealthComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeHealthComponent.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeHealthComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHealthComponent.h)、[AbilitySystem/AttributeSet/HodgeResonanceSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeResonanceSet.h)、[AbilitySystem/Effects/HodgeResonanceEnergyEffect.h](../../../Source/Hodgepodge/Public/AbilitySystem/Effects/HodgeResonanceEnergyEffect.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[Data/HodgeAssetManager.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManager.h)、[Data/HodgeGameData.h](../../../Source/Hodgepodge/Public/Data/HodgeGameData.h)

定义候选（多行签名仅展示首行）：

- L35: `UHodgeHealthComponent::UHodgeHealthComponent(const FObjectInitializer& ObjectInitializer)`
- L58: `void UHodgeHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L68: `void UHodgeHealthComponent::OnUnregister()`
- L78: `void UHodgeHealthComponent::InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC)`
- L141: `void UHodgeHealthComponent::ResetForSpawn()`
- L151: `void UHodgeHealthComponent::UninitializeFromAbilitySystem()`
- L176: `void UHodgeHealthComponent::ClearGameplayTags()`
- L190: `float UHodgeHealthComponent::GetHealth() const`
- L197: `float UHodgeHealthComponent::GetMaxHealth() const`
- L204: `float UHodgeHealthComponent::GetHealthNormalized() const`
- L224: `void UHodgeHealthComponent::HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser,`
- L233: `void UHodgeHealthComponent::HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser,`
- L242: `void UHodgeHealthComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser,`
- L316: `void UHodgeHealthComponent::OnRep_DeathState(EHodgeDeathState OldDeathState)`
- L388: `void UHodgeHealthComponent::StartDeath()`
- L429: `void UHodgeHealthComponent::FinishDeath()`
- L461: `void UHodgeHealthComponent::DamageSelfDestruct(bool bFellOutOfWorld)`

## HodgeHeroComponent.cpp

玩家 Init State 协调、ASC 接入、输入与相机；额外输入句柄持久化并在移除/EndPlay 解绑。新增“移动意图”信号（HasMoveIntent / GetMoveIntent / OnMoveIntentChanged），记 Input_Move 原始输入量、Completed/Canceled 清零，供移动取消后摇消费。

源码：[Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.h)、[AbilitySystem/AttributeSet/HodgeHavocSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHavocSet.h)、[Component/HodgeLockOnComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLockOnComponent.h)、[Data/HodgeLockOnProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeLockOnProfile.h)、[Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)、[Component/HodgeLocomotionPolicyComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLocomotionPolicyComponent.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Dash.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Dash.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Sprint.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Sprint.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[Animation/HodgeAnimNotifyState_CinematicCamera.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimNotifyState_CinematicCamera.h)、[Data/HodgeCinematicCameraProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeCinematicCameraProfile.h)、[Camera/HodgeCameraMode_Cinematic.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode_Cinematic.h)、[Core/PlayerController/HodgePlayerController.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)、[Core/LocalPlayer/HodgeLocalPlayerBase.h](../../../Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)、[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[Input/HodgeInputConfig.h](../../../Source/Hodgepodge/Public/Input/HodgeInputConfig.h)、[Input/HodgeInputComponent.h](../../../Source/Hodgepodge/Public/Input/HodgeInputComponent.h)、[Camera/HodgeCameraComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Combat/HodgeHitReactionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitReactionTypes.h)、[Camera/HodgeCameraMode.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode.h)、[UI/Subsystem/HodgeUIManagerSubsystem.h](../../../Source/Hodgepodge/Public/UI/Subsystem/HodgeUIManagerSubsystem.h)

定义候选（多行签名仅展示首行）：

- L104: `const FName UHodgeHeroComponent::NAME_BindInputsNow("BindInputsNow");`
- L107: `const FName UHodgeHeroComponent::NAME_ActorFeatureName("Hero");`
- L110: `UHodgeHeroComponent::UHodgeHeroComponent(const FObjectInitializer& ObjectInitializer)`
- L121: `void UHodgeHeroComponent::OnRegister()`
- L165: `bool UHodgeHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,`
- L284: `void UHodgeHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,`
- L351: `void UHodgeHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)`
- L367: `void UHodgeHeroComponent::CheckDefaultInitialization()`
- L387: `void UHodgeHeroComponent::BeginPlay()`
- L411: `void UHodgeHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L438: `void UHodgeHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)`
- L664: `void UHodgeHeroComponent::AddAdditionalInputConfig(const UHodgeInputConfig* InputConfig)`
- L755: `void UHodgeHeroComponent::RemoveAdditionalInputConfig(const UHodgeInputConfig* InputConfig)`
- L784: `bool UHodgeHeroComponent::IsReadyToBindInputs() const`
- L791: `void UHodgeHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)`
- L820: `void UHodgeHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)`
- L852: `void UHodgeHeroComponent::Input_Move(const FInputActionValue& InputActionValue)`
- L934: `void UHodgeHeroComponent::Input_MoveStopped(const FInputActionValue&                     )`
- L942: `bool UHodgeHeroComponent::HasMoveIntent(float Threshold) const`
- L949: `void UHodgeHeroComponent::SetMoveIntent(const FVector2D& NewValue)`
- L965: `void UHodgeHeroComponent::RefreshMoveIntent()`
- L980: `void UHodgeHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)`
- L1034: `void UHodgeHeroComponent::Input_LookStick(const FInputActionValue& InputActionValue)`
- L1083: `void UHodgeHeroComponent::Input_Crouch(const FInputActionValue& InputActionValue)`
- L1097: `void UHodgeHeroComponent::Input_AutoRun(const FInputActionValue& InputActionValue)`
- L1118: `TSubclassOf<UHodgeCameraMode> UHodgeHeroComponent::DetermineCameraMode() const`
- L1155: `FHodgeAbilityCameraHandle UHodgeHeroComponent::SetAbilityCameraMode(TSubclassOf<UHodgeCameraMode> CameraMode,`
- L1180: `void UHodgeHeroComponent::ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle)`
- L1186: `bool UHodgeHeroComponent::ClearAbilityCameraMode(const FHodgeAbilityCameraHandle& Handle)`
- L1204: `void UHodgeHeroComponent::EndCinematicCameraForExecution(FGuid ExecutionId)`
- L1209: `void UHodgeHeroComponent::AbortCinematicCamera(const FHodgeAbilityCameraHandle& Handle, const FString& Reason)`
- L1217: `void UHodgeHeroComponent::RefreshCinematicCamera()`
- L1267: `void UHodgeHeroComponent::ResetGameplayInput()`
- L1276: `void UHodgeHeroComponent::BeginSprintInput()`
- L1301: `void UHodgeHeroComponent::EndSprintInput()`
- L1322: `void UHodgeHeroComponent::NotifyDashCommitted(int32 Id)`
- L1328: `void UHodgeHeroComponent::NotifySprintCommitted(int32 Id)`
- L1334: `void UHodgeHeroComponent::NotifyMovementActionEnded(int32 Id, bool bHandedOff)`
- L1339: `void UHodgeHeroComponent::RequestSprintHandoff(int32 Id, float HoldThreshold)`
- L1357: `void UHodgeHeroComponent::BeginCircuitHold()`
- L1363: `void UHodgeHeroComponent::EndCircuitHold()`
- L1370: `void UHodgeHeroComponent::UpdateCircuitHold()`

## HodgeHitReactionComponent.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Component/HodgeHitReactionComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeHitReactionComponent.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeHitReactionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHitReactionComponent.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_HitReaction.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_HitReaction.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Jump.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Jump.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Death.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Death.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Data/HodgeHitReactionProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeHitReactionProfile.h)、[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)

定义候选（多行签名仅展示首行）：

- L27: `UHodgeHitReactionComponent::UHodgeHitReactionComponent(const FObjectInitializer& ObjectInitializer)`
- L36: `bool UHodgeHitReactionComponent::IsBound() const`
- L42: `void UHodgeHitReactionComponent::InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC)`
- L52: `void UHodgeHitReactionComponent::UninitializeFromAbilitySystem()`
- L71: `void UHodgeHitReactionComponent::RefreshConfiguration()`
- L87: `FGuid UHodgeHitReactionComponent::BeginHit(const FHodgeHitReactionConfig& Request, AActor* Source, FVector SourceOrigin)`
- L103: `void UHodgeHitReactionComponent::RecordDamage(const FGuid& HitId, bool bAccepted, float ActualDamage)`
- L116: `FHodgeHitReactionResult UHodgeHitReactionComponent::FinishHit(const FGuid& HitId)`
- L170: `bool UHodgeHitReactionComponent::BuildPlan(const FPendingHit& Hit, FHodgeHitReactionState& Plan, TArray<EHodgeImpactType>& Applied) const`
- L259: `bool UHodgeHitReactionComponent::SubmitPlan(FHodgeHitReactionState Plan, EHodgeHitReactionOutcome& Failure)`
- L314: `void UHodgeHitReactionComponent::SetControl(bool bControlled)`
- L325: `void UHodgeHitReactionComponent::PublishState()`
- L332: `void UHodgeHitReactionComponent::OnRep_State()`
- L350: `void UHodgeHitReactionComponent::OnRep_Profile() { OnRep_State(); }`
- L353: `void UHodgeHitReactionComponent::AttachAbility(UHodgeGameplayAbility_HitReaction* Ability)`
- L360: `void UHodgeHitReactionComponent::DetachAbility(UHodgeGameplayAbility_HitReaction* Ability)`
- L369: `void UHodgeHitReactionComponent::CompletePlan(int32 Sequence)`
- L383: `void UHodgeHitReactionComponent::CancelReaction()`
- L390: `void UHodgeHitReactionComponent::ChangePhase(EHodgeHitReactionPhase Phase, UAnimMontage* Montage, float Duration)`
- L400: `bool UHodgeHitReactionComponent::RequestGetUp()`
- L410: `void UHodgeHitReactionComponent::TickPlan()`
- L453: `float UHodgeHitReactionComponent::GetServerTime() const`
- L461: `void UHodgeHitReactionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)`
- L485: `void UHodgeHitReactionComponent::MulticastLightFeedback_Implementation(FVector Direction)`
- L502: `void UHodgeHitReactionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L510: `void UHodgeHitReactionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)`

## HodgeInteractionComponentBase.cpp

交互组件基础占位；完整扫描、交互规则与 UI 需另行实现。

源码：[Source/Hodgepodge/Private/Component/HodgeInteractionComponentBase.cpp](../../../Source/Hodgepodge/Private/Component/HodgeInteractionComponentBase.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeInteractionComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeInteractionComponentBase.h)

定义候选（多行签名仅展示首行）：


## HodgeLockOnComponent.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Component/HodgeLockOnComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeLockOnComponent.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeLockOnComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLockOnComponent.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_LockOn.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_LockOn.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/HodgeLockOnStateEffect.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeLockOnStateEffect.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)、[Component/HodgeHealthComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHealthComponent.h)、[Combat/HodgeDamageRules.h](../../../Source/Hodgepodge/Public/Combat/HodgeDamageRules.h)、[Combat/HodgeMovementActionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeMovementActionTypes.h)、[Combat/HodgeHitReactionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitReactionTypes.h)、[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)、[Data/HodgeLockOnProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeLockOnProfile.h)、[Interface/HodgeLockOnTargetInterface.h](../../../Source/Hodgepodge/Public/Interface/HodgeLockOnTargetInterface.h)、[UI/IndicatorSystem/HodgeIndicatorManagerComponent.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/HodgeIndicatorManagerComponent.h)、[UI/IndicatorSystem/IndicatorDescriptor.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorDescriptor.h)、[UI/Subsystem/HodgeUIManagerSubsystem.h](../../../Source/Hodgepodge/Public/UI/Subsystem/HodgeUIManagerSubsystem.h)

定义候选（多行签名仅展示首行）：

- L27: `const FName UHodgeLockOnComponent::NAME_ActorFeatureName(TEXT("LockOn"));`
- L28: `UHodgeLockOnComponent::UHodgeLockOnComponent(const FObjectInitializer& Initializer) : Super(Initializer)`
- L33: `void UHodgeLockOnComponent::OnRegister() { Super::OnRegister(); bClosed = false; RegisterInitStateFeature(); }`
- L34: `void UHodgeLockOnComponent::BeginPlay()`
- L46: `bool UHodgeLockOnComponent::CanChangeInitState(UGameFrameworkComponentManager*, FGameplayTag Current, FGameplayTag Desired) const`
- L59: `void UHodgeLockOnComponent::OnActorInitStateChanged(const FActorInitStateChangedParams&) { CheckDefaultInitialization(); }`
- L60: `void UHodgeLockOnComponent::CheckDefaultInitialization()`
- L75: `void UHodgeLockOnComponent::InitializeFromExtension()`
- L95: `bool UHodgeLockOnComponent::IsReady() const`
- L97: `void UHodgeLockOnComponent::Uninitialize()`
- L116: `void UHodgeLockOnComponent::EndPlay(EEndPlayReason::Type Reason)`
- L118: `void UHodgeLockOnComponent::OnUnregister()`
- L124: `void UHodgeLockOnComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L126: `AActor* UHodgeLockOnComponent::GetPresentationTarget() const`
- L136: `bool UHodgeLockOnComponent::GetPresentationLockPoint(FVector& OutLocation) const`
- L142: `bool UHodgeLockOnComponent::IsConfirmedTargetValid() const`
- L147: `bool UHodgeLockOnComponent::HasSight(AActor* Target, FVector Point) const`
- L156: `bool UHodgeLockOnComponent::IsTargetValid(AActor* Target, FName Point, bool bAcquiring, EHodgeLockOnReason& Reason) const`
- L168: `AActor* UHodgeLockOnComponent::SelectCandidate(int32 Direction) const`
- L205: `bool UHodgeLockOnComponent::ToggleLock()`
- L210: `bool UHodgeLockOnComponent::TryLockTarget(AActor* Target, FName Point)`
- L215: `bool UHodgeLockOnComponent::SwitchTarget(int32 Direction)`
- L220: `void UHodgeLockOnComponent::ClearLock(EHodgeLockOnReason Reason)`
- L227: `bool UHodgeLockOnComponent::SubmitCommand(EHodgeLockOnOperation Operation, AActor* Target, FName Point)`
- L236: `void UHodgeLockOnComponent::DispatchQueuedCommand()`
- L252: `bool UHodgeLockOnComponent::TakeCommand(FHodgeLockOnTargetData& Out)`
- L258: `void UHodgeLockOnComponent::ProcessAuthorityCommand(const FHodgeLockOnTargetData& Data)`
- L278: `void UHodgeLockOnComponent::PublishTarget(AActor* Target, FName Point, int32 Sequence, EHodgeLockOnReason Reason, bool bAccepted)`
- L292: `void UHodgeLockOnComponent::ClientCommandResult_Implementation(FHodgeLockOnState State) { ApplyResult(State); }`
- L293: `void UHodgeLockOnComponent::OnRep_ConfirmedState() { ApplyResult(ConfirmedState); }`
- L294: `void UHodgeLockOnComponent::ApplyResult(const FHodgeLockOnState& State)`
- L305: `void UHodgeLockOnComponent::NotifyCommandEnded(int32 Sequence, bool bCancelled)`
- L317: `void UHodgeLockOnComponent::RejectPrediction(int32 Sequence, int32 InGeneration)`
- L322: `bool UHodgeLockOnComponent::CanOwnControllerFacing() const`
- L331: `void UHodgeLockOnComponent::RefreshFacing()`
- L338: `bool UHodgeLockOnComponent::ResolveControlRotation(FRotator& InOut, float Delta) const`
- L346: `void UHodgeLockOnComponent::SeedCameraView(FRotator CurrentView)`
- L350: `FRotator UHodgeLockOnComponent::UpdateCameraView(const FHodgeLockCameraFrame& Frame, float DeltaSeconds, bool bAllowAssist)`
- L356: `bool UHodgeLockOnComponent::SubmitLookIntent(FVector2D Degrees, bool bAllowTargetSwitch)`
- L367: `void UHodgeLockOnComponent::SynchronizePresentation()`
- L403: `void UHodgeLockOnComponent::HandleConstraint(FGameplayTag Tag, int32 Count)`
- L405: `void UHodgeLockOnComponent::HandleTargetDeath(AActor* Actor)`
- L407: `void UHodgeLockOnComponent::HandleTargetDestroyed(AActor* Actor)`
- L409: `void UHodgeLockOnComponent::CheckTargetValidity(double Now)`
- L430: `void UHodgeLockOnComponent::TickComponent(float Delta, ELevelTick Type, FActorComponentTickFunction* Function)`

## HodgeLocomotionPolicyComponent.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Component/HodgeLocomotionPolicyComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeLocomotionPolicyComponent.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeLocomotionPolicyComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLocomotionPolicyComponent.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)、[Data/HodgeSprintAbilityProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeSprintAbilityProfile.h)

定义候选（多行签名仅展示首行）：

- L17: `UHodgeLocomotionPolicyComponent::UHodgeLocomotionPolicyComponent(const FObjectInitializer& Initializer) : Super(Initializer)`
- L22: `void UHodgeLocomotionPolicyComponent::InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC)`
- L30: `void UHodgeLocomotionPolicyComponent::UninitializeFromAbilitySystem()`
- L39: `const UHodgeSprintAbilityProfile* UHodgeLocomotionPolicyComponent::GetProfile() const`
- L47: `FGuid UHodgeLocomotionPolicyComponent::AcquireSprint(UGameplayAbility* Source, const UHodgeSprintAbilityProfile* Profile, int32 ActivationKey)`
- L60: `void UHodgeLocomotionPolicyComponent::ReleasePolicy(FGuid Handle)`
- L70: `FHodgeLocomotionState UHodgeLocomotionPolicyComponent::GetResolvedPolicy() const`
- L83: `FGuid UHodgeLocomotionPolicyComponent::AcquireSpeedModifier(UObject* Source, float Multiplier)`
- L93: `void UHodgeLocomotionPolicyComponent::ReleaseSpeedModifier(FGuid Handle)`
- L96: `void UHodgeLocomotionPolicyComponent::RefreshSpeedScale()`
- L109: `void UHodgeLocomotionPolicyComponent::SetMoveReplayState(const FHodgeLocomotionState& InState) { ReplayState = InState; bReplay = true; }`
- L111: `void UHodgeLocomotionPolicyComponent::ClearMoveReplayState() { bReplay = false; }`
- L113: `void UHodgeLocomotionPolicyComponent::BeginServerMove(int32 ActivationKey)`
- L119: `void UHodgeLocomotionPolicyComponent::EndServerMove() { bServerOverride = false; }`
- L121: `void UHodgeLocomotionPolicyComponent::TickComponent(float DeltaTime, ELevelTick Type, FActorComponentTickFunction* Function)`
- L134: `void UHodgeLocomotionPolicyComponent::SetSprintInput(int32 SessionId, bool bHeld)`
- L144: `void UHodgeLocomotionPolicyComponent::SetInputInternal(int32 SessionId, bool bHeld)`
- L152: `void UHodgeLocomotionPolicyComponent::ServerSetSprintInput_Implementation(AActor* Avatar, int32 SessionId, bool bHeld)`
- L155: `bool UHodgeLocomotionPolicyComponent::IsSessionHeld(int32 SessionId) const { return bInputHeld && SessionId == InputSessionId; }`
- L157: `float UHodgeLocomotionPolicyComponent::SessionHeldTime(int32 SessionId) const`
- L160: `void UHodgeLocomotionPolicyComponent::AuthorizeHandoff(int32 DashKey, int32 SessionId, float ValidFor)`
- L166: `bool UHodgeLocomotionPolicyComponent::CanHandoff(int32 SessionId) const`
- L169: `void UHodgeLocomotionPolicyComponent::ConsumeHandoff() { HandoffSessionId = 0; }`
- L171: `void UHodgeLocomotionPolicyComponent::RecordDashExitVelocity(FName Source, float MaximumSpeed)`
- L180: `bool UHodgeLocomotionPolicyComponent::GetDashExitVelocity(FName Source, float& OutMaximumSpeed) const`
- L187: `void UHodgeLocomotionPolicyComponent::RecordDashMotionVelocity(FName Source, float MaximumSpeed, float MotionDuration)`
- L194: `bool UHodgeLocomotionPolicyComponent::GetDashFinishVelocity(FName Source, bool bFalling, float& OutMaximumSpeed, bool& bOutClamp) const`
- L204: `void UHodgeLocomotionPolicyComponent::OnRep_State()`
- L211: `void UHodgeLocomotionPolicyComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L214: `void UHodgeLocomotionPolicyComponent::EndPlay(EEndPlayReason::Type Reason) { UninitializeFromAbilitySystem(); Super::EndPlay(Reason); }`

## HodgeMovementComponentBase.cpp

通用移动组件基础占位，与 CharacterMovement 派生类需区分。

源码：[Source/Hodgepodge/Private/Component/HodgeMovementComponentBase.cpp](../../../Source/Hodgepodge/Private/Component/HodgeMovementComponentBase.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeMovementComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeMovementComponentBase.h)

定义候选（多行签名仅展示首行）：


## HodgePawnExtensionComponent.cpp

PawnData 复制、Init State、ASC 关联/解除、TagRelationshipMapping 与 ClearAbilityInput；需验证退出顺序。

源码：[Source/Hodgepodge/Private/Component/HodgePawnExtensionComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgePawnExtensionComponent.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)

定义候选（多行签名仅展示首行）：

- L21: `const FName UHodgePawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");`
- L24: `UHodgePawnExtensionComponent::UHodgePawnExtensionComponent(const FObjectInitializer& ObjectInitializer)`
- L39: `void UHodgePawnExtensionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L48: `void UHodgePawnExtensionComponent::OnRegister()`
- L68: `void UHodgePawnExtensionComponent::BeginPlay()`
- L83: `void UHodgePawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L95: `void UHodgePawnExtensionComponent::SetPawnData(const UHodgePawnData* InPawnData)`
- L129: `void UHodgePawnExtensionComponent::OnRep_PawnData()`
- L138: `void UHodgePawnExtensionComponent::InitializeAbilitySystem(UHodgeAbilitySystemComponent* InASC,`
- L200: `void UHodgePawnExtensionComponent::UninitializeAbilitySystem()`
- L246: `void UHodgePawnExtensionComponent::HandleControllerChanged()`
- L271: `void UHodgePawnExtensionComponent::HandlePlayerStateReplicated()`
- L278: `void UHodgePawnExtensionComponent::SetupPlayerInputComponent()`
- L285: `void UHodgePawnExtensionComponent::CheckDefaultInitialization()`
- L301: `bool UHodgePawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager,`
- L363: `void UHodgePawnExtensionComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager,`
- L374: `void UHodgePawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)`
- L387: `void UHodgePawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(`
- L404: `void UHodgePawnExtensionComponent::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)`
- L414: `void UHodgePawnExtensionComponent::UnregisterAbilitySystemDelegates(const UObject* Subscriber)`

## HodgeWingComponent.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Component/HodgeWingComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeWingComponent.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeWingComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeWingComponent.h)、[Animation/HodgeAnimNotifyState_WingAction.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimNotifyState_WingAction.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

定义候选（多行签名仅展示首行）：

- L20: `UHodgeWingComponent::UHodgeWingComponent()`
- L29: `void UHodgeWingComponent::BeginPlay()`
- L36: `bool UHodgeWingComponent::InitializeWing()`
- L135: `void UHodgeWingComponent::SetWingVisible(bool bVisible)`
- L147: `void UHodgeWingComponent::ApplyFadeVisibility()`
- L156: `void UHodgeWingComponent::BeginFadeOut()`
- L162: `bool UHodgeWingComponent::IsOwnerPresentationBlocked() const`
- L168: `bool UHodgeWingComponent::ResolveSource(int32& BindingIndex, int32& InstanceId, float& Position, UAnimInstance*& SourceInstance)`
- L234: `FHodgeWingActionHandle UHodgeWingComponent::BeginWingAction(int32 BindingIndex, int32 InstanceId, FGuid ExecutionId)`
- L240: `bool UHodgeWingComponent::EndWingAction(FHodgeWingActionHandle Handle)`
- L252: `void UHodgeWingComponent::EndWingForExecution(FGuid ExecutionId)`
- L257: `void UHodgeWingComponent::ResetWingPresentation()`
- L272: `bool UHodgeWingComponent::EvaluateWindowPose(float Position)`
- L302: `bool UHodgeWingComponent::PrepareAuthorityHitPose(FGuid ExecutionId)`
- L336: `void UHodgeWingComponent::RequestPresentationRefresh()`
- L344: `void UHodgeWingComponent::RefreshPresentation()`
- L398: `void UHodgeWingComponent::UpdatePose(float Position, int32 InstanceId, bool bActive, float DeltaSeconds)`
- L422: `void UHodgeWingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* Tick)`
- L444: `void UHodgeWingComponent::EndPlay(const EEndPlayReason::Type Reason)`

## HodgeActorComponentBase.h

通用 ActorComponent 基础访问与扩展。

源码：[Source/Hodgepodge/Public/Component/HodgeActorComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeActorComponentBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  12: #pragma once
  14: #include "CoreMinimal.h"
  15: #include "Components/ActorComponent.h"
  16: #include "HodgeActorComponentBase.generated.h"
  18: class UAbilitySystemComponent;
  40: UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
  41: class HODGEPODGE_API UHodgeActorComponentBase : public UActorComponent
  42: {
  43: 	GENERATED_BODY()
  45: public:
  52: 	UHodgeActorComponentBase();
  54: protected:
  62: 	virtual void BeginPlay() override;
  64: public:
  77: 	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
  78: 	                           FActorComponentTickFunction* ThisTickFunction) override;
  87: 	AActor* GetOwnerCharacter();
  95: 	virtual void InitializeComponent() override;
 104: 	virtual void OnReady();
 113: 	bool IsServer() const
 114: 	{
 115: 		return GetOwner()->HasAuthority();
 116: 	}
 117: };
```

## HodgeAttackMotionWarpingComponent.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Component/HodgeAttackMotionWarpingComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeAttackMotionWarpingComponent.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "MotionWarpingComponent.h"
   4: #include "HodgeAttackMotionWarpingComponent.generated.h"
   5: struct FHodgeCharacterRotationState;
   6: class UAnimMontage;
   9: UCLASS(ClassGroup=Movement, meta=(BlueprintSpawnableComponent))
  10: class HODGEPODGE_API UHodgeAttackMotionWarpingComponent : public UMotionWarpingComponent
  11: {
  12: 	GENERATED_BODY()
  13: public:
  14: 	virtual void InitializeComponent() override;
  16: 	void ClearAttackWarp();
  18: 	void PrepareAttackTarget(const FHodgeCharacterRotationState& State, const UAnimMontage* Montage, float Position);
  19: 	void ClearAttackWarpForRequest(FGuid Request) { if (EvaluationRequest==Request) { ClearAttackWarp(); } }
  20: private:
  21: 	UFUNCTION() void PrepareAttackEvaluation(UMotionWarpingComponent* Component);
  22: 	FGuid EvaluationRequest;
  23: 	FName OwnedTargetName;
  24: 	float PreviousPosition = -1.f;
  25: };
```

## HodgeCharacterMovementComponent.h

地面距离/加速度、最终旋转过滤、SavedMove 旋转策略重放与权威校正。

源码：[Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)

项目内直接 include（不是运行调用关系）：[Combat/HodgeMovementActionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeMovementActionTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "GameFramework/CharacterMovementComponent.h"
   9: #include "NativeGameplayTags.h"
  10: #include "GameFramework/CharacterMovementReplication.h"
  11: #include "Combat/HodgeMovementActionTypes.h"
  13: #include "HodgeCharacterMovementComponent.generated.h"
  15: class UObject;
  16: struct FFrame;
  19: HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_MovementStopped);
  25: USTRUCT(BlueprintType)
  26: struct FHodgeCharacterGroundInfo
  27: {
  28: 	GENERATED_BODY()
  32: 	FHodgeCharacterGroundInfo()
  33: 		: LastUpdateFrame(0)
  34: 		  , GroundDistance(0.0f)
  35: 	{
  36: 	}
  39: 	uint64 LastUpdateFrame;
  42: 	UPROPERTY(BlueprintReadOnly)
  43: 	FHitResult GroundHitResult;
  46: 	UPROPERTY(BlueprintReadOnly)
  47: 	float GroundDistance;
  48: };
  56: UCLASS(Config = Game)
  57: class HODGEPODGE_API UHodgeCharacterMovementComponent : public UCharacterMovementComponent
  58: {
  59: 	GENERATED_BODY()
  61: public:
  66: 	UHodgeCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);
  72: 	virtual void SimulateMovement(float DeltaTime) override;
  76: 	virtual void PerformMovement(float DeltaTime) override;
  82: 	virtual bool CanAttemptJump() const override;
  88: 	UFUNCTION(BlueprintCallable, Category = "Hodge|CharacterMovement")
  89: 	const FHodgeCharacterGroundInfo& GetGroundInfo();
  95: 	void SetReplicatedAcceleration(const FVector& InAcceleration);
 104: 	virtual FRotator GetDeltaRotation(float DeltaTime) const override;
 110: 	virtual float GetMaxSpeed() const override;
 114: 	virtual float GetMaxAcceleration() const override;
 118: 	virtual float GetMaxBrakingDeceleration() const override;
 123: 	virtual FVector ConstrainInputAcceleration(const FVector& InputAcceleration) const override;
 134: 	virtual bool ApplyRequestedMove(float DeltaTime, float MaxAccel, float MaxSpeed, float Friction,
 135: 		float BrakingDeceleration, FVector& OutAcceleration, float& OutRequestedSpeed) override;
 139: 	void SetHitReactionMoveReplay(bool bControlled) { bHasReactionReplay = true; bReactionReplayControlled = bControlled; }
 144: 	virtual void PhysicsRotation(float DeltaTime) override;
 148: 	virtual void ServerMove_PerformMovement(const FCharacterNetworkMoveData& MoveData) override;
 152: 	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
 156: 	virtual bool ClientUpdatePositionAfterServerUpdate() override;
 160: 	virtual void ClientHandleMoveResponse(const FCharacterMoveResponseDataContainer& MoveResponse) override;
 172: 	virtual void ClientAdjustPosition_Implementation(float TimeStamp, FVector NewLoc, FVector NewVel,
 173: 		UPrimitiveComponent* NewBase, FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition,
 174: 		uint8 ServerMovementMode, TOptional<FRotator> OptionalRotation = TOptional<FRotator>()) override;
 175: #if WITH_DEV_AUTOMATION_TESTS
 179: 	FCharacterNetworkMoveDataContainer& GetFacingMoveDataForTests() { return *FacingMoveData; }
 180: #endif
 187: 	virtual void SmoothCorrection(const FVector& OldLocation, const FQuat& OldRotation,
 188: 		const FVector& NewLocation, const FQuat& NewRotation) override;
 192: protected:
 195: 	virtual void InitializeComponent() override;
 205: 	virtual bool MoveUpdatedComponentImpl(const FVector& Delta, const FQuat& NewRotation, bool bSweep,
 206: 		FHitResult* OutHit = nullptr, ETeleportType Teleport = ETeleportType::None) override;
 208: protected:
 210: 	FHodgeCharacterGroundInfo CachedGroundInfo;
 213: 	UPROPERTY(Transient)
 214: 	bool bHasReplicatedAcceleration = false;
 217: 	bool bApplyingRotationCorrection = false;
 219: 	bool bHasReactionReplay = false;
 221: 	bool bReactionReplayControlled = false;
 223: 	TOptional<FVector> DashCorrectionVelocity;
 227: 	bool IsHitReactionControlled() const;
 229: 	TUniquePtr<FCharacterNetworkMoveDataContainer> FacingMoveData;
 230: };
```

## HodgeCharacterRotationComponent.h

ASC 旋转标签约束、锁定 Yaw、恢复、权威复制与移动重放状态。

源码：[Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)

项目内直接 include（不是运行调用关系）：[Combat/HodgeCharacterFacingTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeCharacterFacingTypes.h)、[Combat/HodgeAttackFacingTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeAttackFacingTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "Components/PawnComponent.h"
   8: #include "GameplayTagContainer.h"
   9: #include "Combat/HodgeCharacterFacingTypes.h"
  10: #include "GameplayAbilitySpecHandle.h"
  11: #include "Combat/HodgeAttackFacingTypes.h"
  12: #include "HodgeCharacterRotationComponent.generated.h"
  14: class UHodgeAbilityDefinition;
  16: class UHodgeAbilitySystemComponent;
  17: class UHodgeLockOnComponent;
  20: USTRUCT(BlueprintType)
  21: struct FHodgeCharacterRotationState
  22: {
  23: 	GENERATED_BODY()
  26: 	UPROPERTY(BlueprintReadOnly)
  27: 	bool bYawLocked = false;
  30: 	UPROPERTY(BlueprintReadOnly)
  31: 	bool bRecoveringFacing = false;
  34: 	UPROPERTY(BlueprintReadOnly)
  35: 	float LockedYaw = 0.f;
  39: 	UPROPERTY() bool bLockOnFacing = false;
  40: 	UPROPERTY(BlueprintReadOnly) EHodgeCharacterFacingDriver BaseDriver = EHodgeCharacterFacingDriver::Movement;
  42: 	UPROPERTY(BlueprintReadOnly) EHodgeCharacterFacingDriver Driver = EHodgeCharacterFacingDriver::Movement;
  44: 	UPROPERTY(BlueprintReadOnly) EHodgeLocomotionStyle Style = EHodgeLocomotionStyle::FreeDirectional;
  46: 	UPROPERTY(BlueprintReadOnly) EHodgeFacingExecutionMode ExecutionMode = EHodgeFacingExecutionMode::PhysicalTurn;
  47: 	UPROPERTY() TObjectPtr<const UHodgeAbilityDefinition> WarpDefinition = nullptr;
  49: 	UPROPERTY(BlueprintReadOnly) float ActionYaw = 0.f;
  51: 	UPROPERTY(BlueprintReadOnly) FGuid ActionRequestId;
  53: 	UPROPERTY() FGameplayAbilitySpecHandle ActionAbilityHandle;
  55: 	UPROPERTY() int32 ActionPredictionKey = 0;
  57: 	UPROPERTY(BlueprintReadOnly) int32 StateVersion = 0;
  59: 	UPROPERTY() uint32 RequestSequence = 0;
  60: };
  62: DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHodgeFacingStateChanged, FHodgeCharacterRotationState, Previous, FHodgeCharacterRotationState, Current);
  63: DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHodgeActionFacingApplied, FHodgeFacingRequestHandle, Handle, EHodgeFacingRequestStatus, Status);
  69: UCLASS(BlueprintType, Meta = (BlueprintSpawnableComponent))
  70: class HODGEPODGE_API UHodgeCharacterRotationComponent : public UPawnComponent
  71: {
  72: 	GENERATED_BODY()
  74: public:
  78: 	UHodgeCharacterRotationComponent(const FObjectInitializer& ObjectInitializer);
  83: 	void InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC);
  85: 	void UninitializeFromAbilitySystem();
  93: 	UFUNCTION(BlueprintCallable, Category="Hodge|Rotation")
  94: 	FHodgeFacingRequestHandle AcquireBaseFacingMode(EHodgeCharacterFacingDriver Driver, UObject* Source);
  96: 	FHodgeFacingRequestHandle AcquireLockOnFacing(UHodgeLockOnComponent* Source);
 104: 	UFUNCTION(BlueprintCallable, Category="Hodge|Rotation")
 105: 	FHodgeFacingRequestHandle RequestActionFacing(FVector WorldDirection, UObject* Source, FGuid ExecutionId, bool bInstantAtStart = false);
 107: 	FHodgeFacingRequestHandle RequestMontageWarpFacing(float TargetYaw, UObject* Source, FGuid ExecutionId);
 109: 	bool SetFacingRequestSuspended(FHodgeFacingRequestHandle Handle, bool bSuspended);
 111: 	bool IsFacingRequestSelected(FHodgeFacingRequestHandle Handle) const;
 116: 	UFUNCTION(BlueprintCallable, Category="Hodge|Rotation")
 117: 	bool ReleaseFacingRequest(FHodgeFacingRequestHandle Handle);
 121: 	void ReleaseRequestsForSource(const UObject* Source);
 126: 	UFUNCTION(BlueprintPure, Category="Hodge|Rotation")
 127: 	EHodgeFacingRequestStatus GetFacingRequestStatus(FHodgeFacingRequestHandle Handle) const;
 133: 	UFUNCTION(BlueprintPure, Category="Hodge|Rotation")
 134: 	bool IsActionFacingApplied(FHodgeFacingRequestHandle Handle) const;
 138: 	UFUNCTION(BlueprintPure, Category="Hodge|Rotation")
 139: 	FHodgeFacingPresentationSnapshot GetFacingPresentationSnapshot() const;
 143: 	UFUNCTION(BlueprintPure, Category="Hodge|Rotation")
 144: 	int32 GetAvatarGeneration() const { return AvatarGeneration; }
 148: 	UFUNCTION(BlueprintPure, Category="Hodge|Rotation")
 149: 	bool IsFacingSystemReady() const;
 151: 	UPROPERTY(BlueprintAssignable) FHodgeFacingStateChanged OnFacingStateChanged;
 153: 	UPROPERTY(BlueprintAssignable) FHodgeActionFacingApplied OnActionFacingApplied;
 156: 	void ApplyResolvedMode();
 161: 	bool BeginServerMove(uint32 Sequence, int32 MontageWarpKey = 0);
 163: 	void EndServerMove();
 168: 	UFUNCTION(BlueprintPure, Category = "Hodge|Rotation")
 169: 	bool IsYawLocked() const;
 174: 	UFUNCTION(BlueprintPure, Category = "Hodge|Rotation")
 175: 	bool IsRecoveringFacing() const;
 180: 	UFUNCTION(BlueprintPure, Category = "Hodge|Rotation")
 181: 	float GetLockedYaw() const;
 186: 	UFUNCTION(BlueprintPure, Category = "Hodge|Rotation")
 187: 	FHodgeCharacterRotationState GetResolvedState() const;
 194: 	FRotator FilterControlRotation(const FRotator& DesiredRotation, float DeltaSeconds) const;
 198: 	void NotifyFacingApplied(float DesiredYaw);
 202: 	float GetRecoveryTurnRate() const { return FMath::IsFinite(RecoveryTurnRate) && RecoveryTurnRate > 0.f ? RecoveryTurnRate : 360.f; }
 203: #if WITH_EDITOR
 208: 	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
 209: #endif
 214: 	void SetMoveReplayState(const FHodgeCharacterRotationState& State);
 216: 	void ClearMoveReplayState();
 221: 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
 223: protected:
 227: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
 233: 	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
 235: private:
 236: #if WITH_DEV_AUTOMATION_TESTS
 237: 	friend struct FHodgeFacingLifecycleTestAccess;
 238: 	friend struct FHodgeAttackWarpTestAccess;
 239: #endif
 244: 	void HandleConstraintTagChanged(FGameplayTag Tag, int32 NewCount);
 246: 	void RefreshLocalState();
 248: 	void PublishAuthorityState();
 252: 	bool IsReplayingMove() const;
 254: 	void ResolveRequests();
 260: 	bool IsValidSource(const UObject* Source, bool bAction) const;
 269: 	FHodgeFacingRequestHandle AddRequest(EHodgeCharacterFacingDriver Driver, UObject* Source, FGuid ExecutionId, float Yaw, bool bInstant, bool bLocalOnly = false);
 274: 	void SendRequest(const FGuid& Id, bool bRelease);
 279: 	void SetRequestStatus(const FGuid& Id, EHodgeFacingRequestStatus Status);
 283: 	bool HasPendingPrediction() const;
 287: 	bool IsAuthorityActionCurrent() const;
 301: 	UFUNCTION(Server, Reliable)
 302: 	void ServerFacingRequest(AActor* Avatar, uint32 Sequence, FGuid Id, bool bRelease, EHodgeCharacterFacingDriver Driver,
 303: 		float Yaw, bool bInstant, FGameplayAbilitySpecHandle AbilityHandle, int32 PredictionKey, FGuid ExecutionId);
 310: 	UFUNCTION(Client, Reliable)
 311: 	void ClientFacingResult(uint32 Sequence, FGuid Id, bool bAccepted, FHodgeCharacterRotationState State);
 314: 	UFUNCTION()
 315: 	void OnRep_RotationState();
 318: 	UPROPERTY(EditDefaultsOnly, Category = "Hodge|Rotation", Meta = (ClampMin = "1", Units = "deg/s"))
 319: 	float RecoveryTurnRate = 360.f;
 321: 	UPROPERTY(EditDefaultsOnly, Category="Hodge|Rotation", meta=(ClampMin="1", Units="deg/s"))
 322: 	float ActionTurnRate = 720.f;
 324: 	UPROPERTY(EditDefaultsOnly, Category="Hodge|Rotation")
 325: 	bool bAllowControllerFacingRequests = true;
 328: 	UPROPERTY(Transient)
 329: 	TWeakObjectPtr<UHodgeAbilitySystemComponent> AbilitySystemComponent;
 332: 	UPROPERTY(ReplicatedUsing = OnRep_RotationState)
 333: 	FHodgeCharacterRotationState ReplicatedState;
 336: 	FHodgeCharacterRotationState LocalState;
 338: 	FHodgeCharacterRotationState ReplayState;
 340: 	FDelegateHandle RotationTagHandle;
 342: 	FDelegateHandle MovementStoppedTagHandle;
 344: 	FDelegateHandle HitReactionTagHandle;
 346: 	FDelegateHandle DeathTagHandle;
 348: 	bool bHasReplayState = false;
 350: 	bool bServerMoveOverride = false;
 352: 	bool bResolvingRequests = false;
 354: 	bool bOriginalControllerYaw = false;
 356: 	bool bOriginalOrientToMovement = true;
 358: 	bool bOriginalControllerDesired = false;
 360: 	int32 AvatarGeneration = 0;
 362: 	uint64 NextRequestOrder = 0;
 364: 	uint32 LocalRequestSequence = 0;
 366: 	uint32 AcknowledgedSequence = 0;
 368: 	uint32 LastServerRequestSequence = 0;
 370: 	FHodgeCharacterRotationState ServerMoveState;
 372: 	struct FRequest
 373: 	{
 375: 		TWeakObjectPtr<UObject> Source;
 377: 		FGuid ExecutionId;
 379: 		EHodgeCharacterFacingDriver Driver = EHodgeCharacterFacingDriver::Movement;
 381: 		EHodgeFacingRequestStatus Status = EHodgeFacingRequestStatus::Pending;
 383: 		float Yaw = 0.f;
 385: 		uint64 Order = 0;
 387: 		bool bInstant = false;
 389: 		bool bRemoteClient = false;
 391: 		bool bLocalOnly = false;
 392: 		bool bAbilityManaged = false;
 393: 		bool bSuspended = false;
 394: 		EHodgeFacingExecutionMode ExecutionMode = EHodgeFacingExecutionMode::PhysicalTurn;
 395: 	};
 397: 	TMap<FGuid, FRequest> Requests;
 399: 	TMap<FGuid, EHodgeFacingRequestStatus> CompletedRequests;
 401: 	TMap<uint32, FHodgeCharacterRotationState> ServerRequestHistory;
 402: };
```

## HodgeCombatComponentBase.h

Experience 注入 Pawn 的统一战斗协调者；连段输入/记忆/预测校正、来源/配置检测体、独立会话与过滤；GA 负责目标去重及效果施加。

源码：[Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)

项目内直接 include（不是运行调用关系）：[Component/HodgeActorComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeActorComponentBase.h)、[Combat/HodgeHitDetection.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitDetection.h)、[Combat/HodgeCombatActionRequest.h](../../../Source/Hodgepodge/Public/Combat/HodgeCombatActionRequest.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "Component/HodgeActorComponentBase.h"
   8: #include "Combat/HodgeHitDetection.h"
   9: #include "GameplayAbilitySpecHandle.h"
  10: #include "Combat/HodgeCombatActionRequest.h"
  11: #include "HodgeCombatComponentBase.generated.h"
  13: class UHodgeWeaponInstance;
  14: class USceneComponent;
  15: class USkeletalMeshComponent;
  16: class UHodgeAbilitySystemComponent;
  17: class UHodgeComboDefinition;
  18: class UHodgeGameplayAbility_Definition;
  19: class UHodgePawnExtensionComponent;
  20: struct FGameplayEventData;
  21: struct FAbilityEndedData;
  22: struct FHodgeComboTransition;
  25: USTRUCT()
  26: struct FHodgeComboMemoryState
  27: {
  28: 	GENERATED_BODY()
  30: 	UPROPERTY() FGameplayTag Node;
  32: 	UPROPERTY() double ExpiresAt = 0;
  34: 	UPROPERTY() int16 ExecutionKey = 0;
  35: };
  38: USTRUCT()
  39: struct FHodgeHitDetectionSession
  40: {
  41: 	GENERATED_BODY()
  43: 	UPROPERTY() FGuid ExecutionId;
  45: 	UPROPERTY() int32 OccurrenceId = INDEX_NONE;
  47: 	UPROPERTY() FHodgeHitDetectionRequest Request;
  49: 	UPROPERTY() FHodgeHitSource Source;
  51: 	UPROPERTY() TWeakObjectPtr<USceneComponent> SourceComponent;
  53: 	UPROPERTY() TWeakObjectPtr<UHodgeWeaponInstance> Weapon;
  55: 	TSubclassOf<UHodgeHitDetectionStrategy> Strategy;
  57: 	bool bWeaponSource = false;
  59: 	int32 SampleSequence = 0;
  61: 	FHodgeHitGeometry Previous;
  63: 	FTransform FixedAnchor = FTransform::Identity;
  65: 	bool bHasFixedAnchor = false;
  66: };
  69: UCLASS(Blueprintable, ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
  70: class HODGEPODGE_API UHodgeCombatComponentBase : public UHodgeActorComponentBase
  71: {
  72: 	GENERATED_BODY()
  73: public:
  75: 	UHodgeCombatComponentBase();
  82: 	void Configure(UHodgeAbilitySystemComponent* InASC, const UHodgeComboDefinition* InDefinition);
  84: 	void Shutdown();
  88: 	FGuid AcquirePoseLease();
  92: 	void ReleasePoseLease(FGuid Handle);
  96: 	int32 GetPoseLeaseCount() const { return PoseLeases.Num(); }
 100: 	int32 GetDetectionSessionCount() const { return Sessions.Num(); }
 104: 	bool CanExecuteAbilities() const { return IsRegistered() && !bShuttingDown; }
 108: 	const UHodgeComboDefinition* GetComboDefinition() const { return Definition; }
 113: 	bool InputPressed(FGameplayTag InputTag);
 118: 	UFUNCTION(BlueprintCallable, Category="Hodge|Combat|Requests")
 119: 	FHodgeCombatActionRequestState RequestStandaloneAbility(FGameplayTag AbilityTag);
 124: 	UFUNCTION(BlueprintCallable, Category="Hodge|Combat|Requests")
 125: 	FHodgeCombatActionRequestState SubmitCombatIntent(FGameplayTag IntentTag);
 131: 	UFUNCTION(BlueprintPure, Category="Hodge|Combat|Requests")
 132: 	bool GetCombatActionRequest(FGuid RequestId, FHodgeCombatActionRequestState& OutState) const;
 137: 	UFUNCTION(BlueprintCallable, Category="Hodge|Combat|Requests")
 138: 	bool CancelCombatActionRequest(FGuid RequestId);
 142: 	bool IsCombatReadyForActions() const;
 144: 	FHodgeCombatActionRequestChanged OnActionRequestChanged;
 146: 	UPROPERTY(BlueprintAssignable, Category="Hodge|Combat|Requests") FHodgeCombatActionRequestChangedBP OnCombatActionRequestChanged;
 150: 	bool IsMoveCancelPredicted() const;
 152: 	void ClearInput();
 154: 	void InvalidateModeContinuation(FGuid ModeSessionId);
 158: 	void ExecutionStarted(UHodgeGameplayAbility_Definition* Ability);
 162: 	void ExecutionEnded(UHodgeGameplayAbility_Definition* Ability);
 166: 	void WindowsChanged(UHodgeGameplayAbility_Definition* Ability);
 171: 	void ExecutionEvent(UHodgeGameplayAbility_Definition* Ability, FGameplayTag Event);
 176: 	bool IsAuthorized(FGameplayAbilitySpecHandle Handle) const;
 180: 	float GetServerInputBufferSeconds() const;
 186: 	bool CanBufferServerActivation(FGameplayAbilitySpecHandle Handle, const FGameplayEventData* Payload) const;
 192: 	bool PrepareServerActivation(FGameplayAbilitySpecHandle Handle, const FGameplayEventData* Payload);
 196: 	void RejectServerActivation(const FGameplayEventData* Payload);
 202: 	bool PrepareConfirmedActivation(FGameplayAbilitySpecHandle Handle, const FGameplayEventData& Payload);
 204: 	void CompleteServerActivation();
 206: 	void HandlePredictionRejected();
 210: 	UFUNCTION(BlueprintPure, Category="Hodge|Combat")
 211: 	FGameplayTag GetCurrentComboTag() const { return CurrentComboTag; }
 215: 	UFUNCTION(BlueprintPure, Category="Hodge|Combat")
 216: 	FGameplayTag GetRememberedComboTag() const;
 220: 	UFUNCTION(BlueprintPure, Category="Hodge|Combat")
 221: 	float GetComboMemoryRemainingTime() const;
 227: 	static UHodgeCombatComponentBase* FindCombatComponent(const AActor* Avatar);
 231: 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
 234: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Combat", meta=(TitleProperty="SourceTag"))
 235: 	TArray<FHodgeHitSource> HitSources;
 243: 	uint64 CreateDetectionSession(const FGuid& ExecutionId, int32 OccurrenceId, const FHodgeHitDetectionRequest& Request);
 250: 	bool SampleDetection(uint64 Handle, const FGuid& ExecutionId, FHodgeHitDetectionBatch& OutBatch);
 256: 	bool ResetDetectionHistory(uint64 Handle, const FGuid& ExecutionId);
 262: 	bool IsDetectionSessionValid(uint64 Handle, const FGuid& ExecutionId) const;
 268: 	USceneComponent* GetDetectionSourceComponent(uint64 Handle, const FGuid& ExecutionId) const;
 273: 	void EndDetectionSession(uint64 Handle, const FGuid& ExecutionId);
 277: 	void EndDetectionSessionsForExecution(const FGuid& ExecutionId);
 279: 	void EndAllDetectionSessions();
 285: 	void UpdateDetectionAnchor(const FGuid& ExecutionId, FName Key, const FTransform& Transform);
 286: protected:
 288: 	virtual void OnRegister() override;
 290: 	virtual void OnUnregister() override;
 292: 	virtual void BeginPlay() override;
 298: 	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* TickFunction) override;
 302: 	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
 303: private:
 304: 	friend struct FHodgeCombatActionTestAccess;
 306: 	struct FActionRequestRecord
 307: 	{
 309: 		FHodgeCombatActionRequestState State;
 311: 		TWeakObjectPtr<UHodgeGameplayAbility_Definition> Ability;
 313: 		bool bActivating = false;
 314: 	};
 320: 	FHodgeCombatActionRequestState AddActionRequest(EHodgeCombatActionRequestType Type, FGameplayTag Tag);
 326: 	void SetActionRequestStatus(FGuid Id, EHodgeCombatActionRequestStatus Status, const FString& Reason = FString());
 330: 	void HandleActionAbilityEnded(const FAbilityEndedData& Data);
 335: 	void BindActionRequest(FGuid Id, FGameplayAbilitySpecHandle Handle);
 340: 	void ConfirmActionRequest(FGuid Id, bool bActivated);
 342: 	void ConsumeBufferedInput();
 344: 	void CloseActionRequests();
 346: 	TMap<FGuid, FActionRequestRecord> ActionRequests;
 348: 	TArray<FGuid> ActionRequestOrder;
 350: 	FGuid BufferedActionRequest;
 352: 	FDelegateHandle ActionEndedHandle;
 354: 	bool bSubmittingAction = false;
 356: 	bool bNotifyingActions = false;
 358: 	TArray<FHodgeCombatActionRequestState> PendingActionNotifications;
 360: 	TSet<FGuid> PoseLeases;
 362: 	TWeakObjectPtr<USkeletalMeshComponent> PoseMesh;
 364: 	uint8 SavedPosePolicy = 0;
 370: 	bool CaptureDetectionGeometry(const FHodgeHitDetectionSession& Session, FHodgeHitGeometry& Out) const;
 372: 	void BindPawnExtension();
 376: 	bool IsComboReady() const;
 378: 	void HandleAbilitySystemInitialized();
 380: 	void HandleAbilitySystemUninitialized();
 382: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgePawnExtensionComponent> PawnExtension;
 384: 	FGameplayTagContainer AppliedObserverTags;
 387: 	friend struct FHodgeComboTestAccess;
 390: 	struct FQueuedEvent
 391: 	{
 393: 		TWeakObjectPtr<UHodgeGameplayAbility_Definition> Ability;
 395: 		int16 Key = 0;
 397: 		FGameplayTag Tag;
 398: 	};
 401: 	void DrainEvents();
 403: 	TArray<FQueuedEvent> QueuedEvents;
 405: 	bool bDrainingEvents = false;
 411: 	const FHodgeComboTransition* SelectTransition(FGameplayTag Trigger, bool bEvent) const;
 417: 	bool TryTransition(FGameplayTag Trigger, bool bEvent);
 423: 	bool PrepareTransition(const FHodgeComboTransition& Edge, FGameplayAbilitySpecHandle Handle);
 427: 	void ResetSession(bool bEndAbility);
 429: 	void EndCurrentExecution();
 431: 	void RetainComboMemory();
 433: 	void ExpireComboMemory();
 435: 	void PublishComboMemory();
 440: 	bool HasResumeTransition(FGameplayTag Node) const;
 444: 	FGameplayTag TransitionSourceNode() const;
 448: 	double ComboTime() const;
 452: 	void SetNode(FGameplayTag Node);
 456: 	int16 ExecutionKey() const;
 458: 	UHodgeGameplayAbility_Definition* GetMoveCancelAbility() const;
 460: 	bool GetMoveCancelPolicy(const UHodgeGameplayAbility_Definition* Ability, FGameplayTag& Window, float& Threshold) const;
 462: 	void UpdateMoveCancel();
 464: 	void ClearMoveCancelRequest();
 471: 	UFUNCTION(Server, Reliable)
 472: 	void ServerMoveCancel(AActor* Avatar, FGameplayAbilitySpecHandle Handle, int32 Key, FGuid RequestId, FVector2D Intent);
 479: 	UFUNCTION(Server, Reliable)
 480: 	void ServerReturnToEntry(AActor* Avatar, FGameplayTag SourceNode, int32 Key, FGameplayTag Intent);
 490: 	UFUNCTION(Client, Reliable)
 491: 	void ClientMoveCancelResult(AActor* Avatar, FGameplayAbilitySpecHandle Handle, int32 Key, FGuid RequestId, bool bEnd,
 492: 		float ServerPosition, float ServerTime, float ServerPlayRate);
 497: 	bool ValidateServerRequestIdentity(const FGameplayEventData* Payload) const;
 504: 	void ProcessServerMoveCancel(TWeakObjectPtr<AActor> Avatar, FGameplayAbilitySpecHandle Handle, int32 Key,
 505: 		FGuid RequestId, FGuid ServerExecution, FVector2D Intent);
 507: 	UFUNCTION(Server, Reliable)
 508: 	void ServerSynchronizeComboMemory();
 512: 	UFUNCTION(Client, Reliable)
 513: 	void ClientCorrectComboMemory(FHodgeComboMemoryState State);
 515: 	UFUNCTION()
 516: 	void OnRep_ComboMemory();
 520: 	UFUNCTION()
 521: 	void OnRep_ObserverTags(const FGameplayTagContainer& Previous);
 523: 	UPROPERTY(Transient)
 524: 	TObjectPtr<UHodgeAbilitySystemComponent> ASC;
 526: 	UPROPERTY(Transient)
 527: 	TObjectPtr<const UHodgeComboDefinition> Definition;
 529: 	UPROPERTY(Transient)
 530: 	TObjectPtr<UHodgeGameplayAbility_Definition> CurrentAbility;
 532: 	UPROPERTY(Transient)
 533: 	FGameplayTag CurrentComboTag;
 535: 	UPROPERTY(ReplicatedUsing=OnRep_ComboMemory)
 536: 	FHodgeComboMemoryState ReplicatedComboMemory;
 538: 	FHodgeComboMemoryState ComboMemory;
 540: 	FHodgeComboMemoryState PreviousComboMemory;
 542: 	bool bModeContinuationInvalidated=false;
 544: 	FDelegateHandle CombatModeChangedHandle;
 545: 	void HandleCombatModeChanged(FGameplayTag Tag,int32 Count);
 546: 	void ClearModeContinuation();
 548: 	UPROPERTY(ReplicatedUsing=OnRep_ObserverTags)
 549: 	FGameplayTagContainer ObserverTags;
 551: 	FGameplayTagContainer OwnedNodeTags;
 553: 	FGameplayTag BufferedInput;
 555: 	double InputExpiresAt = 0;
 557: 	FGameplayAbilitySpecHandle AuthorizedHandle;
 559: 	FGameplayTag PendingNode;
 561: 	bool bSwitching = false;
 563: 	bool bTransitionStarted = false;
 565: 	bool bMemoryCorrectionPending = false;
 567: 	bool bMoveRequestPending = false;
 569: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgeGameplayAbility_Definition> PredictedMoveCancelAbility;
 571: 	FGameplayAbilitySpecHandle PendingMoveHandle;
 573: 	int32 PendingMoveKey = 0;
 575: 	FGuid PendingMoveRequest;
 577: 	FGuid PendingMoveExecution;
 579: 	FGameplayAbilitySpecHandle RejectedMoveHandle;
 581: 	int32 RejectedMoveKey = 0;
 583: 	FGuid RejectedMoveExecution;
 585: 	bool bEvaluating = false;
 587: 	bool bShuttingDown = false;
 594: 	bool ResolveSource(FGameplayTag Tag, FHodgeHitDetectionSession& Session) const;
 599: 	bool IsSourceValid(const FHodgeHitDetectionSession& Session) const;
 601: 	UPROPERTY(Transient)
 602: 	TMap<uint64, FHodgeHitDetectionSession> Sessions;
 604: 	uint64 NextHandle = 0;
 605: };
```

## HodgeDefenseComponent.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Component/HodgeDefenseComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeDefenseComponent.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "Components/PawnComponent.h"
   7: #include "GameplayEffectTypes.h"
   8: #include "HodgeDefenseComponent.generated.h"
  10: class UHodgeAbilitySystemComponent;
  11: class UHodgeSprintAbilityProfile;
  12: class UGameplayAbility;
  14: UENUM(BlueprintType)
  15: enum class EHodgeIncomingHitOutcome : uint8 {                 Allowed,                    Dodged,                    PerfectDodge };
  18: UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
  19: class HODGEPODGE_API UHodgeDefenseComponent : public UPawnComponent
  20: {
  21: 	GENERATED_BODY()
  22: public:
  26: 	void InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC);
  28: 	void UninitializeFromAbilitySystem();
  36: 	FGuid RegisterDodgeWindow(UGameplayAbility* Source, FGuid Execution, const UHodgeSprintAbilityProfile* Profile, double StartTime);
  40: 	void UnregisterDodgeWindow(FGuid Handle);
  49: 	EHodgeIncomingHitOutcome ResolveIncomingHit(AActor* Source, FGuid AttackExecution, int32 HitIndex, bool bCanBeDodged, bool bCanTriggerPerfect);
  58: 	static EHodgeIncomingHitOutcome EvaluateWindow(const UHodgeSprintAbilityProfile& Profile, double Age, bool bRewarded, bool bCanBeDodged, bool bPerfectEligible);
  62: 	virtual void EndPlay(EEndPlayReason::Type Reason) override;
  63: private:
  65: 	struct FWindow
  66: 	{
  68: 		TWeakObjectPtr<UGameplayAbility> Source;
  70: 		TWeakObjectPtr<const UHodgeSprintAbilityProfile> Profile;
  72: 		FGuid Execution;
  74: 		double StartTime = 0.;
  76: 		bool bRewarded = false;
  77: 	};
  79: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgeAbilitySystemComponent> ASC;
  81: 	TMap<FGuid, FWindow> Windows;
  82: };
```

## HodgeEnemyInitComponent.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Component/HodgeEnemyInitComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeEnemyInitComponent.h)

项目内直接 include（不是运行调用关系）：[Data/HodgeAbilitySet.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilitySet.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "Components/PawnComponent.h"
   7: #include "Components/GameFrameworkInitStateInterface.h"
   8: #include "Data/HodgeAbilitySet.h"
   9: #include "HodgeEnemyInitComponent.generated.h"
  11: class UHodgePawnExtensionComponent;
  12: class UHodgePawnData;
  13: class UHodgeExperienceDefinition;
  14: class AGameStateBase;
  17: UENUM(BlueprintType)
  18: enum class EHodgeEnemyInitializationResult : uint8 {               Waiting,                  Ready,                Failed,                     Invalidated };
  20: DECLARE_MULTICAST_DELEGATE(FHodgeEnemyReadinessChanged);
  23: UCLASS(BlueprintType)
  24: class HODGEPODGE_API UHodgeEnemyInitComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
  25: {
  26: 	GENERATED_BODY()
  27: public:
  31: 	UHodgeEnemyInitComponent(const FObjectInitializer& Initializer = FObjectInitializer::Get());
  33: 	static const FName NAME_ActorFeatureName;
  37: 	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
  44: 	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag Current, FGameplayTag Desired) const override;
  48: 	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
  50: 	virtual void CheckDefaultInitialization() override;
  54: 	UFUNCTION(BlueprintCallable, Category="Hodge|Enemy") EHodgeEnemyInitializationResult TryInitialize();
  58: 	UFUNCTION(BlueprintPure, Category="Hodge|Enemy") bool IsGameplayReady() const;
  62: 	UFUNCTION(BlueprintPure, Category="Hodge|Enemy") bool HasValidCombatDependencies() const;
  66: 	UFUNCTION(BlueprintPure, Category="Hodge|Enemy") bool CanExecuteAIAction() const;
  70: 	UFUNCTION(BlueprintPure, Category="Hodge|Enemy") FString GetInitializationDiagnostic() const { return Diagnostic; }
  74: 	UFUNCTION(BlueprintPure, Category="Hodge|Enemy") EHodgeEnemyInitializationResult GetInitializationResult() const { return Result; }
  78: 	int32 GetInitializationGeneration() const { return Generation; }
  80: 	FHodgeEnemyReadinessChanged OnReadinessChanged;
  81: protected:
  83: 	virtual void OnRegister() override;
  85: 	virtual void OnUnregister() override;
  87: 	virtual void BeginPlay() override;
  91: 	virtual void EndPlay(EEndPlayReason::Type Reason) override;
  92: private:
  93: 	friend struct FHodgeEnemyInitTestAccess;
  97: 	EHodgeEnemyInitializationResult InitializeStep();
 102: 	EHodgeEnemyInitializationResult Fail(const FString& Reason);
 107: 	EHodgeEnemyInitializationResult Wait(const TCHAR* Reason);
 109: 	void BindDependencies();
 111: 	void HandleASCUninitialized();
 115: 	void HandleExperienceLoaded(const UHodgeExperienceDefinition* Experience);
 119: 	void HandleGameStateSet(AGameStateBase* State);
 121: 	void Cleanup();
 125: 	bool IsBindingCurrent() const;
 127: 	UPROPERTY(EditDefaultsOnly, Category="Hodge|Enemy") bool bRequireExperience = true;
 129: 	UPROPERTY(Transient) TObjectPtr<const UHodgePawnData> BoundData;
 131: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgeAbilitySystemComponent> BoundASC;
 133: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgePawnExtensionComponent> Extension;
 135: 	UPROPERTY(Transient) FHodgeAbilitySet_GrantedHandles BaseHandles;
 137: 	UPROPERTY(Transient) EHodgeEnemyInitializationResult Result = EHodgeEnemyInitializationResult::Waiting;
 139: 	UPROPERTY(Transient) FString Diagnostic;
 141: 	FDelegateHandle GameStateHandle;
 143: 	bool bExperienceBound = false;
 145: 	bool bExperienceReady = false;
 147: 	bool bTrying = false;
 149: 	bool bRecheck = false;
 151: 	bool bClosed = false;
 153: 	bool bCleaning = false;
 155: 	bool bStatsApplied = false;
 157: 	bool bBaseGranted = false;
 159: 	bool bEquipmentPrepared = false;
 161: 	int32 Generation = 1;
 162: };
```

## HodgeExperienceManagerComponent.h

GameState 上的 Experience 复制、资源加载、插件激活、Action 执行与 Loaded 委托。

源码：[Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h)

项目内直接 include（不是运行调用关系）：[Interface/LoadingProcessInterface.h](../../../Source/Hodgepodge/Public/Interface/LoadingProcessInterface.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "CoreMinimal.h"
   9: #include "Components/GameStateComponent.h"
  10: #include "Interface/LoadingProcessInterface.h"
  11: #include "HodgeExperienceManagerComponent.generated.h"
  14: namespace UE::GameFeatures
  15: {
  16:     struct FResult;
  17: }
  20: class UHodgeExperienceDefinition;
  23: DECLARE_MULTICAST_DELEGATE_OneParam(FOnHodgeExperienceLoaded, const UHodgeExperienceDefinition*               );
  26: enum class EHodgeExperienceLoadState
  27: {
  29:     Unloaded,
  31:     Loading,
  33:     LoadingGameFeatures,
  35:     LoadingChaosTestingDelay,
  37:     ExecutingActions,
  39:     Loaded,
  41:     Deactivating
  42: };
  45: UCLASS()
  46: class HODGEPODGE_API UHodgeExperienceManagerComponent final : public UGameStateComponent,
  47:                                                               public ILoadingProcessInterface
  48: {
  49:     GENERATED_BODY()
  51: public:
  56:     UHodgeExperienceManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  63:     virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  72:     virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
  79:     void SetCurrentExperience(FPrimaryAssetId ExperienceId);
  85:     void CallOrRegister_OnExperienceLoaded_HighPriority(FOnHodgeExperienceLoaded::FDelegate&& Delegate);
  91:     void CallOrRegister_OnExperienceLoaded(FOnHodgeExperienceLoaded::FDelegate&& Delegate);
  97:     void CallOrRegister_OnExperienceLoaded_LowPriority(FOnHodgeExperienceLoaded::FDelegate&& Delegate);
 103:     const UHodgeExperienceDefinition* GetCurrentExperienceChecked() const;
 109:     bool IsExperienceLoaded() const;
 111: private:
 114:     UFUNCTION()
 115:     void OnRep_CurrentExperience();
 119:     void StartExperienceLoad();
 123:     void OnExperienceLoadComplete();
 129:     void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
 133:     void OnExperienceFullLoadCompleted();
 137:     void OnActionDeactivationCompleted();
 141:     void OnAllActionsDeactivated();
 143: private:
 145:     UPROPERTY(ReplicatedUsing=OnRep_CurrentExperience)
 146:     TObjectPtr<const UHodgeExperienceDefinition> CurrentExperience;
 149:     EHodgeExperienceLoadState LoadState = EHodgeExperienceLoadState::Unloaded;
 152:     int32 NumGameFeaturePluginsLoading = 0;
 155:     TArray<FString> GameFeaturePluginURLs;
 158:     int32 NumObservedPausers = 0;
 161:     int32 NumExpectedPausers = 0;
 164:     FOnHodgeExperienceLoaded OnExperienceLoaded_HighPriority;
 167:     FOnHodgeExperienceLoaded OnExperienceLoaded;
 170:     FOnHodgeExperienceLoaded OnExperienceLoaded_LowPriority;
 171: };
```

## HodgeHealthComponent.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Component/HodgeHealthComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHealthComponent.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   9: #include "CoreMinimal.h"
  12: #include "Components/GameFrameworkComponent.h"
  14: #include "HodgeHealthComponent.generated.h"
  18: class UHodgeAbilitySystemComponent;
  21: class UHodgeHealthSet;
  24: class UObject;
  27: struct FFrame;
  30: struct FGameplayEffectSpec;
  34: DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHodgeHealth_DeathEvent, AActor*, OwningActor);
  40: DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FHodgeHealth_AttributeChanged, UHodgeHealthComponent*, HealthComponent,
  41:                                               float, OldValue, float, NewValue, AActor*, Instigator);
  52: UENUM(BlueprintType)
  53: enum class EHodgeDeathState : uint8
  54: {
  56: 	NotDead = 0,
  59: 	DeathStarted,
  62: 	DeathFinished
  63: };
  77: UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
  78: class HODGEPODGE_API UHodgeHealthComponent : public UGameFrameworkComponent
  79: {
  80: 	GENERATED_BODY()
  82: public:
  87: 	UHodgeHealthComponent(const FObjectInitializer& ObjectInitializer);
  95: 	UFUNCTION(BlueprintPure, Category = "Hodge|Health")
  96: 	static UHodgeHealthComponent* FindHealthComponent(const AActor* Actor)
  97: 	{
  99: 		return (Actor ? Actor->FindComponentByClass<UHodgeHealthComponent>() : nullptr);
 100: 	}
 107: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Health")
 108: 	void InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC);
 113: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Health")
 114: 	void UninitializeFromAbilitySystem();
 116: 	void ResetForSpawn();
 123: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Health")
 124: 	float GetHealth() const;
 131: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Health")
 132: 	float GetMaxHealth() const;
 139: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Health")
 140: 	float GetHealthNormalized() const;
 146: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Health")
 147: 	EHodgeDeathState GetDeathState() const { return DeathState; }
 151: 	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Hodge|Health",
 152: 		Meta = (ExpandBoolAsExecs = "ReturnValue"))
 156: 	bool IsDeadOrDying() const { return (DeathState > EHodgeDeathState::NotDead); }
 161: 	virtual void StartDeath();
 166: 	virtual void FinishDeath();
 173: 	virtual void DamageSelfDestruct(bool bFellOutOfWorld = false);
 175: public:
 178: 	UPROPERTY(BlueprintAssignable)
 179: 	FHodgeHealth_AttributeChanged OnHealthChanged;
 183: 	UPROPERTY(BlueprintAssignable)
 184: 	FHodgeHealth_AttributeChanged OnMaxHealthChanged;
 188: 	UPROPERTY(BlueprintAssignable)
 189: 	FHodgeHealth_DeathEvent OnDeathStarted;
 193: 	UPROPERTY(BlueprintAssignable)
 194: 	FHodgeHealth_DeathEvent OnDeathFinished;
 196: protected:
 199: 	virtual void OnUnregister() override;
 203: 	void ClearGameplayTags();
 215: 	virtual void HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser,
 216: 	                                 const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue,
 217: 	                                 float NewValue);
 228: 	virtual void HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser,
 229: 	                                    const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude,
 230: 	                                    float OldValue, float NewValue);
 241: 	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser,
 242: 	                               const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue,
 243: 	                               float NewValue);
 250: 	UFUNCTION()
 251: 	virtual void OnRep_DeathState(EHodgeDeathState OldDeathState);
 253: protected:
 256: 	UPROPERTY()
 257: 	TObjectPtr<UHodgeAbilitySystemComponent> AbilitySystemComponent;
 262: 	UPROPERTY()
 263: 	TObjectPtr<const UHodgeHealthSet> HealthSet;
 267: 	UPROPERTY(ReplicatedUsing = OnRep_DeathState)
 268: 	EHodgeDeathState DeathState;
 269: };
```

## HodgeHeroComponent.h

玩家 Init State 协调、ASC 接入、输入与相机；额外输入句柄持久化并在移除/EndPlay 解绑。新增“移动意图”信号（HasMoveIntent / GetMoveIntent / OnMoveIntentChanged），记 Input_Move 原始输入量、Completed/Canceled 清零，供移动取消后摇消费。

源码：[Source/Hodgepodge/Public/Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)

项目内直接 include（不是运行调用关系）：[GameFeatures/GameFeatureAction_AddInputContextMapping.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddInputContextMapping.h)、[Combat/HodgeCharacterFacingTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeCharacterFacingTypes.h)、[Combat/HodgeMovementActionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeMovementActionTypes.h)、[Camera/HodgeCinematicCameraTypes.h](../../../Source/Hodgepodge/Public/Camera/HodgeCinematicCameraTypes.h)、[Combat/HodgeHavocRules.h](../../../Source/Hodgepodge/Public/Combat/HodgeHavocRules.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   9: #include "Components/GameFrameworkInitStateInterface.h"
  12: #include "Components/PawnComponent.h"
  15: #include "GameFeatures/GameFeatureAction_AddInputContextMapping.h"
  18: #include "GameplayAbilitySpecHandle.h"
  19: #include "Combat/HodgeCharacterFacingTypes.h"
  20: #include "Combat/HodgeMovementActionTypes.h"
  21: #include "Camera/HodgeCinematicCameraTypes.h"
  22: #include "Combat/HodgeHavocRules.h"
  24: #include "HodgeHeroComponent.generated.h"
  27: namespace EEndPlayReason
  28: {
  29: 	enum Type : int;
  30: }
  34: class UGameFrameworkComponentManager;
  37: class UInputComponent;
  40: class UHodgeCameraMode;
  41: class UHodgeCinematicCameraProfile;
  42: class UHodgeGameplayAbility_Definition;
  43: class UHodgeAnimNotifyState_CinematicCamera;
  46: class UHodgeInputConfig;
  48: class UObject;
  51: struct FActorInitStateChangedParams;
  53: struct FFrame;
  56: struct FGameplayTag;
  59: struct FInputActionValue;
  67: DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHodgeMoveIntentChanged, bool, bHasMoveIntent);
  76: UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
  77: class HODGEPODGE_API UHodgeHeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
  78: {
  79: 	GENERATED_BODY()
  81: public:
  86: 	UHodgeHeroComponent(const FObjectInitializer& ObjectInitializer);
  94: 	UFUNCTION(BlueprintPure, Category = "Hodge|Hero")
  95: 	static UHodgeHeroComponent* FindHeroComponent(const AActor* Actor)
  96: 	{
  98: 		return (Actor ? Actor->FindComponentByClass<UHodgeHeroComponent>() : nullptr);
  99: 	}
 107: 	FHodgeAbilityCameraHandle SetAbilityCameraMode(TSubclassOf<UHodgeCameraMode> CameraMode,
 108: 	                          const FGameplayAbilitySpecHandle& OwningSpecHandle, FGuid ExecutionId = FGuid());
 110: 	bool ClearAbilityCameraMode(const FHodgeAbilityCameraHandle& Handle);
 112: 	void RefreshCinematicCamera();
 114: 	void EndCinematicCameraForExecution(FGuid ExecutionId);
 116: 	void AbortCinematicCamera(const FHodgeAbilityCameraHandle& Handle, const FString& Reason);
 118: 	const UHodgeCinematicCameraProfile* GetCinematicProfile() const { return CinematicProfile; }
 120: 	UFUNCTION(BlueprintPure, Category="Hodge|Camera") FHodgeCinematicCameraState GetCinematicCameraState() const { return CinematicState; }
 122: 	UFUNCTION(BlueprintPure, Category="Hodge|Camera") TSubclassOf<UHodgeCameraMode> GetDesiredCameraMode() const { return DetermineCameraMode(); }
 129: 	void ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle);
 136: 	void AddAdditionalInputConfig(const UHodgeInputConfig* InputConfig);
 143: 	void RemoveAdditionalInputConfig(const UHodgeInputConfig* InputConfig);
 150: 	bool IsReadyToBindInputs() const;
 167: 	UFUNCTION(BlueprintPure, Category = "Hodge|Hero|Input")
 168: 	bool HasMoveIntent(float Threshold = 0.1f) const;
 170: 	void ResetGameplayInput();
 172: 	UFUNCTION(BlueprintCallable, Category="Hodge|Movement") void BeginSprintInput();
 174: 	UFUNCTION(BlueprintCallable, Category="Hodge|Movement") void EndSprintInput();
 178: 	UFUNCTION(BlueprintPure, Category="Hodge|Movement") FHodgeSprintInputSession GetSprintInputSession() const { return SprintInputSession; }
 182: 	void NotifyDashCommitted(int32 SessionId);
 186: 	void NotifySprintCommitted(int32 SessionId);
 191: 	void NotifyMovementActionEnded(int32 SessionId, bool bHandedOff);
 196: 	void RequestSprintHandoff(int32 SessionId, float HoldThreshold);
 202: 	UFUNCTION(BlueprintPure, Category = "Hodge|Hero|Input")
 203: 	FVector2D GetMoveIntent() const { return CurrentMoveInput; }
 207: 	UFUNCTION(BlueprintPure, Category="Hodge|Hero|Input")
 208: 	FHodgeMoveIntentSnapshot GetMoveIntentSnapshot() const { return MoveIntentSnapshot; }
 212: 	UFUNCTION(BlueprintPure, Category="Hodge|Hero|Input")
 213: 	FVector GetWorldMoveIntent() const { return MoveIntentSnapshot.DesiredDirectionWorld; }
 216: 	UPROPERTY(BlueprintAssignable, Category = "Hodge|Hero|Input")
 217: 	FHodgeMoveIntentChanged OnMoveIntentChanged;
 221: 	static const FName NAME_BindInputsNow;
 225: 	static const FName NAME_ActorFeatureName;
 233: 	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
 242: 	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
 243: 	                                FGameplayTag DesiredState) const override;
 251: 	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
 252: 	                                   FGameplayTag DesiredState) override;
 258: 	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
 262: 	virtual void CheckDefaultInitialization() override;
 266: protected:
 269: 	virtual void OnRegister() override;
 273: 	virtual void BeginPlay() override;
 279: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
 285: 	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);
 291: 	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
 297: 	void Input_AbilityInputTagReleased(FGameplayTag InputTag);
 303: 	void Input_Move(const FInputActionValue& InputActionValue);
 310: 	void Input_MoveStopped(const FInputActionValue& InputActionValue);
 316: 	void Input_LookMouse(const FInputActionValue& InputActionValue);
 322: 	void Input_LookStick(const FInputActionValue& InputActionValue);
 328: 	void Input_Crouch(const FInputActionValue& InputActionValue);
 334: 	void Input_AutoRun(const FInputActionValue& InputActionValue);
 340: 	TSubclassOf<UHodgeCameraMode> DetermineCameraMode() const;
 346: 	void SetMoveIntent(const FVector2D& NewValue);
 350: 	void RefreshMoveIntent();
 352: protected:
 355: 	FHodgeHavocHoldSession CircuitHold;
 356: 	FTimerHandle CircuitHoldTimer;
 357: 	void BeginCircuitHold();
 358: 	void UpdateCircuitHold();
 359: 	void EndCircuitHold();
 360: 	UPROPERTY(EditAnywhere)
 361: 	TArray<FInputMappingContextAndPriority> DefaultInputMappings;
 365: 	UPROPERTY()
 366: 	TSubclassOf<UHodgeCameraMode> AbilityCameraMode;
 370: 	FGameplayAbilitySpecHandle AbilityCameraModeOwningSpecHandle;
 372: 	FHodgeAbilityCameraHandle AbilityCameraHandle;
 374: 	int32 NextCameraGeneration = 0;
 376: 	UPROPERTY() TObjectPtr<UHodgeCinematicCameraProfile> CinematicProfile;
 378: 	UPROPERTY() TWeakObjectPtr<UHodgeGameplayAbility_Definition> CinematicSource;
 379: 	UPROPERTY() TObjectPtr<UHodgeAnimNotifyState_CinematicCamera> CinematicWindow;
 380: 	FHodgeCinematicCameraState CinematicState;
 382: 	FGuid FailedCinematicExecution;
 383: 	int32 FailedCinematicInstance = INDEX_NONE;
 385: 	bool bRefreshingCinematic = false;
 389: 	bool bReadyToBindInputs;
 397: 	FVector2D CurrentMoveInput = FVector2D::ZeroVector;
 399: 	FHodgeMoveIntentSnapshot MoveIntentSnapshot;
 401: 	FHodgeSprintInputSession SprintInputSession;
 404: 	bool bLastMoveIntent = false;
 414: 	TMap<const UHodgeInputConfig*, TArray<uint32>> AdditionalInputConfigHandles;
 415: };
```

## HodgeHitReactionComponent.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Component/HodgeHitReactionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHitReactionComponent.h)

项目内直接 include（不是运行调用关系）：[Combat/HodgeHitReactionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitReactionTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "Components/PawnComponent.h"
   7: #include "Combat/HodgeHitReactionTypes.h"
   8: #include "HodgeHitReactionComponent.generated.h"
  10: class UHodgeAbilitySystemComponent;
  11: class UHodgeHitReactionProfile;
  12: class UHodgeGameplayAbility_HitReaction;
  13: class UAbilitySystemComponent;
  15: DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHodgeReactionResolved, const FHodgeHitReactionResult&, Result);
  16: DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHodgeReactionStateChanged, const FHodgeHitReactionState&, State);
  17: DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHodgeLightFeedback, FVector, Direction);
  20: UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
  21: class HODGEPODGE_API UHodgeHitReactionComponent : public UPawnComponent
  22: {
  23: 	GENERATED_BODY()
  24: public:
  28: 	UHodgeHitReactionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  32: 	void InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC);
  34: 	void UninitializeFromAbilitySystem();
  36: 	void RefreshConfiguration();
  40: 	UFUNCTION(BlueprintPure, Category="Hodge|HitReaction") FHodgeHitReactionState GetReactionState() const { return State; }
  44: 	UFUNCTION(BlueprintPure, Category="Hodge|HitReaction") FHodgeHitReactionResult GetLastResult() const { return LastResult; }
  48: 	UFUNCTION(BlueprintPure, Category="Hodge|HitReaction") bool IsControlled() const { return State.IsActive(); }
  52: 	UFUNCTION(BlueprintPure, Category="Hodge|HitReaction") const UHodgeHitReactionProfile* GetProfile() const { return Profile; }
  54: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|HitReaction") TObjectPtr<UHodgeHitReactionProfile> OverrideProfile;
  56: 	UPROPERTY(BlueprintAssignable) FHodgeReactionResolved OnReactionResolved;
  58: 	UPROPERTY(BlueprintAssignable) FHodgeReactionStateChanged OnReactionStateChanged;
  60: 	UPROPERTY(BlueprintAssignable) FHodgeLightFeedback OnLightFeedback;
  64: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|HitReaction") bool RequestGetUp();
  66: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|HitReaction") void CancelReaction();
  73: 	FGuid BeginHit(const FHodgeHitReactionConfig& Request, AActor* Source, FVector SourceOrigin);
  79: 	void RecordDamage(const FGuid& HitId, bool bAccepted, float ActualDamage);
  84: 	FHodgeHitReactionResult FinishHit(const FGuid& HitId);
  88: 	void AttachAbility(UHodgeGameplayAbility_HitReaction* Ability);
  92: 	void DetachAbility(UHodgeGameplayAbility_HitReaction* Ability);
  96: 	void CompletePlan(int32 Sequence);
  98: 	void TickPlan();
 102: 	float GetServerTime() const;
 106: 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
 112: 	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
 116: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
 118: private:
 119: 	friend struct FHodgeHitReactionTestAccess;
 121: 	struct FPendingHit
 122: 	{
 124: 		FHodgeHitReactionConfig Request;
 126: 		TWeakObjectPtr<AActor> Source;
 128: 		TWeakObjectPtr<AActor> Avatar;
 130: 		FVector Origin = FVector::ZeroVector;
 132: 		FGameplayTag BodyTag;
 134: 		int32 BodyRank = 0;
 136: 		float Damage = 0.f;
 138: 		bool bAccepted = false;
 140: 		bool bRejected = false;
 141: 	};
 143: 	TMap<FGuid, FPendingHit> PendingHits;
 145: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgeAbilitySystemComponent> ASC;
 147: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgeGameplayAbility_HitReaction> ActiveAbility;
 149: 	UPROPERTY(ReplicatedUsing=OnRep_Profile) TObjectPtr<UHodgeHitReactionProfile> Profile;
 151: 	UPROPERTY(ReplicatedUsing=OnRep_State) FHodgeHitReactionState State;
 153: 	UPROPERTY(Transient) FHodgeHitReactionResult LastResult;
 155: 	int32 NextSequence = 0;
 157: 	bool bOwnsControl = false;
 159: 	bool bSubmitting = false;
 161: 	bool bCompleting = false;
 163: 	bool bSawAirborne = false;
 167: 	bool IsBound() const;
 174: 	bool BuildPlan(const FPendingHit& Hit, FHodgeHitReactionState& Plan, TArray<EHodgeImpactType>& Applied) const;
 180: 	bool SubmitPlan(FHodgeHitReactionState Plan, EHodgeHitReactionOutcome& Failure);
 184: 	void SetControl(bool bControlled);
 186: 	void PublishState();
 192: 	void ChangePhase(EHodgeHitReactionPhase Phase, UAnimMontage* Montage, float Duration);
 194: 	UFUNCTION() void OnRep_State();
 196: 	UFUNCTION() void OnRep_Profile();
 200: 	UFUNCTION(NetMulticast, Unreliable) void MulticastLightFeedback(FVector Direction);
 201: };
```

## HodgeInteractionComponentBase.h

交互组件基础占位；完整扫描、交互规则与 UI 需另行实现。

源码：[Source/Hodgepodge/Public/Component/HodgeInteractionComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeInteractionComponentBase.h)

项目内直接 include（不是运行调用关系）：[Component/HodgeActorComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeActorComponentBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  11: #pragma once
  13: #include "CoreMinimal.h"
  14: #include "Component/HodgeActorComponentBase.h"
  15: #include "HodgeInteractionComponentBase.generated.h"
  27: UCLASS()
  28: class HODGEPODGE_API UHodgeInteractionComponentBase : public UHodgeActorComponentBase
  29: {
  30: 	GENERATED_BODY()
  31: };
```

## HodgeLockOnComponent.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Component/HodgeLockOnComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLockOnComponent.h)

项目内直接 include（不是运行调用关系）：[Targeting/HodgeLockOnTypes.h](../../../Source/Hodgepodge/Public/Targeting/HodgeLockOnTypes.h)、[Combat/HodgeCharacterFacingTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeCharacterFacingTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "Components/PawnComponent.h"
   4: #include "Components/GameFrameworkInitStateInterface.h"
   5: #include "Targeting/HodgeLockOnTypes.h"
   6: #include "Combat/HodgeCharacterFacingTypes.h"
   7: #include "GameplayEffectTypes.h"
   8: #include "HodgeLockOnComponent.generated.h"
   9: class UHodgeAbilitySystemComponent;
  10: class UHodgePawnExtensionComponent;
  11: class UHodgeLockOnProfile;
  12: class UHodgeCharacterRotationComponent;
  13: class UIndicatorDescriptor;
  14: class UHodgeHealthComponent;
  15: DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FHodgeLockOnChanged, FHodgeLockOnState, Confirmed, AActor*, PresentationTarget, int32, PresentationRevision);
  17: UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
  18: class HODGEPODGE_API UHodgeLockOnComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
  19: {
  20: 	GENERATED_BODY()
  21: public:
  22: 	UHodgeLockOnComponent(const FObjectInitializer& Initializer = FObjectInitializer::Get());
  23: 	static const FName NAME_ActorFeatureName;
  24: 	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
  25: 	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag Current, FGameplayTag Desired) const override;
  26: 	virtual void CheckDefaultInitialization() override;
  27: 	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
  29: 	UFUNCTION(BlueprintCallable, Category="Hodge|LockOn") bool ToggleLock();
  31: 	UFUNCTION(BlueprintCallable, Category="Hodge|LockOn") bool TryLockTarget(AActor* Target, FName LockPointId = NAME_None);
  33: 	UFUNCTION(BlueprintCallable, Category="Hodge|LockOn") bool SwitchTarget(int32 Direction);
  35: 	UFUNCTION(BlueprintCallable, Category="Hodge|LockOn") void ClearLock(EHodgeLockOnReason Reason = EHodgeLockOnReason::UserClear);
  36: 	UFUNCTION(BlueprintPure, Category="Hodge|LockOn") AActor* GetPresentationTarget() const;
  37: 	UFUNCTION(BlueprintPure, Category="Hodge|LockOn") AActor* GetConfirmedTarget() const { return ConfirmedState.Target; }
  38: 	UFUNCTION(BlueprintPure, Category="Hodge|LockOn") FHodgeLockOnState GetConfirmedState() const { return ConfirmedState; }
  39: 	UFUNCTION(BlueprintPure, Category="Hodge|LockOn") bool IsReady() const;
  40: 	UFUNCTION(BlueprintPure, Category="Hodge|LockOn") bool IsCommandPending() const { return bHasPending || bHasQueued; }
  41: 	UFUNCTION(BlueprintPure, Category="Hodge|LockOn") EHodgeLockOnReason GetLastLocalReason() const { return LastLocalReason; }
  42: 	UFUNCTION(BlueprintPure, Category="Hodge|LockOn") const UHodgeLockOnProfile* GetProfile() const { return Profile; }
  44: 	UFUNCTION(BlueprintPure, Category="Hodge|LockOn") int32 GetPresentationRevision() const { return PresentationRevision; }
  45: 	UPROPERTY(BlueprintAssignable) FHodgeLockOnChanged OnLockStateChanged;
  47: 	bool SubmitLookIntent(FVector2D Degrees, bool bAllowTargetSwitch = true);
  49: 	bool ResolveControlRotation(FRotator& InOutRotation, float DeltaSeconds) const;
  51: 	bool GetPresentationLockPoint(FVector& OutLocation) const;
  53: 	bool IsConfirmedTargetValid() const;
  55: 	void RefreshFacing();
  57: 	bool TakeCommand(FHodgeLockOnTargetData& OutData);
  59: 	void ProcessAuthorityCommand(const FHodgeLockOnTargetData& Data);
  61: 	void NotifyCommandEnded(int32 Sequence, bool bCancelled);
  63: 	void RejectPrediction(int32 Sequence, int32 Generation);
  64: 	int32 GetGeneration() const { return Generation; }
  66: 	bool CanOwnControllerFacing() const;
  68: 	void SeedCameraView(FRotator CurrentView);
  69: 	void BeginCameraAcquire() { SoftCamera.BeginAcquire(); }
  71: 	FRotator UpdateCameraView(const FHodgeLockCameraFrame& Frame, float DeltaSeconds, bool bAllowAssist);
  73: 	void Uninitialize();
  74: 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& Out) const override;
  75: protected:
  76: 	virtual void OnRegister() override;
  77: 	virtual void BeginPlay() override;
  78: 	virtual void EndPlay(EEndPlayReason::Type Reason) override;
  79: 	virtual void OnUnregister() override;
  80: 	virtual void TickComponent(float Delta, ELevelTick Type, FActorComponentTickFunction* Function) override;
  81: private:
  82: #if WITH_DEV_AUTOMATION_TESTS
  83: 	friend struct FHodgeLockOnTestAccess;
  84: #endif
  85: 	void InitializeFromExtension();
  86: 	void SynchronizePresentation();
  87: 	void CheckTargetValidity(double Now);
  88: 	bool SubmitCommand(EHodgeLockOnOperation Operation, AActor* Target, FName Point);
  89: 	void DispatchQueuedCommand();
  90: 	bool IsTargetValid(AActor* Target, FName Point, bool bAcquiring, EHodgeLockOnReason& Reason) const;
  91: 	bool HasSight(AActor* Target, FVector Point) const;
  92: 	AActor* SelectCandidate(int32 Direction) const;
  93: 	void PublishTarget(AActor* Target, FName Point, int32 Sequence, EHodgeLockOnReason Reason, bool bAccepted);
  94: 	void HandleConstraint(FGameplayTag Tag, int32 Count);
  95: 	UFUNCTION() void HandleTargetDeath(AActor* Actor);
  96: 	UFUNCTION() void HandleTargetDestroyed(AActor* Actor);
  97: 	UFUNCTION() void OnRep_ConfirmedState();
  99: 	UFUNCTION(Client, Reliable) void ClientCommandResult(FHodgeLockOnState State);
 100: 	void ApplyResult(const FHodgeLockOnState& State);
 101: 	UPROPERTY(ReplicatedUsing=OnRep_ConfirmedState) FHodgeLockOnState ConfirmedState;
 102: 	UPROPERTY(Transient) TObjectPtr<const UHodgeLockOnProfile> Profile;
 103: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgeAbilitySystemComponent> ASC;
 104: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgePawnExtensionComponent> Extension;
 105: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgeCharacterRotationComponent> Rotation;
 106: 	UPROPERTY(Transient) TObjectPtr<UIndicatorDescriptor> Indicator;
 107: 	UPROPERTY(Transient) TWeakObjectPtr<AActor> ObservedTarget;
 108: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgeHealthComponent> ObservedHealth;
 109: 	FHodgeLockOnTargetData Pending, Queued;
 110: 	FHodgeFacingRequestHandle FacingHandle;
 111: 	FActiveGameplayEffectHandle StateEffect;
 112: 	TArray<TPair<FGameplayTag,FDelegateHandle>> ConstraintHandles;
 113: 	FHodgeLockOnGesture Gesture;
 114: 	FHodgeSoftLockCamera SoftCamera;
 116: 	int32 NextSequence = 0, Generation = 0;
 117: 	bool bBound = false, bClosed = false, bHasPending = false, bHasQueued = false, bCommandActive = false;
 118: 	bool bCheckingInitialization = false;
 119: 	bool bRecheckInitialization = false;
 120: 	double PendingAt = 0., NextCheckAt = 0., OutOfRangeAt = -1., OccludedAt = -1., ReferenceWaitAt = -1.;
 121: 	EHodgeLockOnReason LastLocalReason = EHodgeLockOnReason::None;
 123: 	TWeakObjectPtr<AActor> LastBroadcastTarget;
 124: 	int32 PresentationRevision = 0, LastBroadcastVersion = -1;
 125: 	bool bLastBroadcastPending = false;
 126: 	EHodgeLockOnReason LastBroadcastReason = EHodgeLockOnReason::None;
 127: 	FString Diagnostic;
 128: };
```

## HodgeLocomotionPolicyComponent.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Component/HodgeLocomotionPolicyComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLocomotionPolicyComponent.h)

项目内直接 include（不是运行调用关系）：[Combat/HodgeMovementActionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeMovementActionTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "Components/PawnComponent.h"
   7: #include "Combat/HodgeMovementActionTypes.h"
   8: #include "GameplayEffectTypes.h"
   9: #include "HodgeLocomotionPolicyComponent.generated.h"
  11: class UHodgeAbilitySystemComponent;
  12: class UHodgeSprintAbilityProfile;
  13: class UGameplayAbility;
  14: DECLARE_MULTICAST_DELEGATE(FHodgeSprintAuthorityEnded);
  17: UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
  18: class HODGEPODGE_API UHodgeLocomotionPolicyComponent : public UPawnComponent
  19: {
  20: 	GENERATED_BODY()
  21: public:
  25: 	UHodgeLocomotionPolicyComponent(const FObjectInitializer& Initializer = FObjectInitializer::Get());
  26: 	friend struct FHodgeMovementTestAccess;
  30: 	void InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC);
  32: 	void UninitializeFromAbilitySystem();
  39: 	FGuid AcquireSprint(UGameplayAbility* Source, const UHodgeSprintAbilityProfile* Profile, int32 ActivationKey);
  43: 	void ReleasePolicy(FGuid Handle);
  49: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) FGuid AcquireSpeedModifier(UObject* Source, float Multiplier);
  53: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void ReleaseSpeedModifier(FGuid Handle);
  57: 	void SetMoveReplayState(const FHodgeLocomotionState& State);
  59: 	void ClearMoveReplayState();
  63: 	void BeginServerMove(int32 ActivationKey);
  65: 	void EndServerMove();
  69: 	UFUNCTION(BlueprintPure) FHodgeLocomotionState GetResolvedPolicy() const;
  73: 	UFUNCTION(BlueprintPure) const UHodgeSprintAbilityProfile* GetProfile() const;
  78: 	UFUNCTION(BlueprintCallable) void SetSprintInput(int32 SessionId, bool bHeld);
  83: 	bool IsSessionHeld(int32 SessionId) const;
  88: 	float SessionHeldTime(int32 SessionId) const;
  94: 	void AuthorizeHandoff(int32 DashKey, int32 SessionId, float ValidFor);
  99: 	bool CanHandoff(int32 SessionId) const;
 101: 	void ConsumeHandoff();
 106: 	void RecordDashExitVelocity(FName Source, float MaximumSpeed);
 108: 	void RecordDashMotionVelocity(FName Source, float MaximumSpeed, float MotionDuration);
 110: 	bool GetDashFinishVelocity(FName Source, bool bFalling, float& OutMaximumSpeed, bool& bOutClamp) const;
 116: 	bool GetDashExitVelocity(FName Source, float& OutMaximumSpeed) const;
 118: 	FHodgeSprintAuthorityEnded OnSprintAuthorityEnded;
 124: 	virtual void TickComponent(float DeltaTime, ELevelTick Type, FActorComponentTickFunction* Function) override;
 128: 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& Out) const override;
 132: 	virtual void EndPlay(EEndPlayReason::Type Reason) override;
 134: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UHodgeSprintAbilityProfile> OverrideProfile;
 135: private:
 137: 	struct FDashExitVelocity
 138: 	{
 139: 		float MaximumSpeed = 0.f;
 140: 		double ExpiresAt = 0.;
 141: 		bool bAirborneOnly = false;
 142: 	};
 144: 	TMap<FName, FDashExitVelocity> DashExitVelocities;
 150: 	UFUNCTION(Server, Reliable) void ServerSetSprintInput(AActor* Avatar, int32 SessionId, bool bHeld);
 152: 	UFUNCTION() void OnRep_State();
 157: 	void SetInputInternal(int32 SessionId, bool bHeld);
 159: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgeAbilitySystemComponent> ASC;
 161: 	UPROPERTY(Transient) TWeakObjectPtr<UGameplayAbility> SprintSource;
 163: 	UPROPERTY(Transient) TObjectPtr<const UHodgeSprintAbilityProfile> SprintProfile;
 165: 	UPROPERTY(ReplicatedUsing=OnRep_State) FHodgeLocomotionState State;
 167: 	FHodgeLocomotionState ReplayState;
 169: 	FHodgeLocomotionState PredictedState;
 171: 	bool bReplay = false;
 173: 	bool bServerOverride = false;
 175: 	FGuid SprintHandle;
 177: 	struct FSpeedModifier {                TWeakObjectPtr<UObject> Source;                         float Multiplier = 1.f; };
 179: 	TMap<FGuid, FSpeedModifier> SpeedModifiers;
 181: 	void RefreshSpeedScale();
 183: 	int32 InputSessionId = 0;
 185: 	bool bInputHeld = false;
 187: 	double InputPressedAt = 0.;
 189: 	int32 HandoffSessionId = 0;
 191: 	double HandoffExpiresAt = 0.;
 192: };
```

## HodgeMovementComponentBase.h

通用移动组件基础占位，与 CharacterMovement 派生类需区分。

源码：[Source/Hodgepodge/Public/Component/HodgeMovementComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeMovementComponentBase.h)

项目内直接 include（不是运行调用关系）：[Component/HodgeActorComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeActorComponentBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  11: #pragma once
  13: #include "CoreMinimal.h"
  14: #include "Component/HodgeActorComponentBase.h"
  15: #include "HodgeMovementComponentBase.generated.h"
  27: UCLASS()
  28: class HODGEPODGE_API UHodgeMovementComponentBase : public UHodgeActorComponentBase
  29: {
  30: 	GENERATED_BODY()
  31: };
```

## HodgePawnExtensionComponent.h

PawnData 复制、Init State、ASC 关联/解除、TagRelationshipMapping 与 ClearAbilityInput；需验证退出顺序。

源码：[Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "CoreMinimal.h"
   9: #include "Components/GameFrameworkInitStateInterface.h"
  10: #include "Components/PawnComponent.h"
  11: #include "HodgePawnExtensionComponent.generated.h"
  13: class UHodgePawnData;
  14: class UHodgeAbilitySystemComponent;
  17: namespace EEndPlayReason
  18: {
  19: 	enum Type : int;
  20: }
  26: UCLASS()
  27: class HODGEPODGE_API UHodgePawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
  28: {
  29: 	GENERATED_BODY()
  31: public:
  36: 	UHodgePawnExtensionComponent(const FObjectInitializer& ObjectInitializer);
  39: 	static const FName NAME_ActorFeatureName;
  41: 	FSimpleMulticastDelegate OnPawnDataChanged;
  49: 	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
  58: 	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
  59: 	                                FGameplayTag DesiredState) const override;
  67: 	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
  68: 	                                   FGameplayTag DesiredState) override;
  74: 	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
  78: 	virtual void CheckDefaultInitialization() override;
  87: 	UFUNCTION(BlueprintPure, Category = "Hodge|Pawn")
  88: 	static UHodgePawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor)
  89: 	{
  91: 		return (Actor ? Actor->FindComponentByClass<UHodgePawnExtensionComponent>() : nullptr);
  92: 	}
  95: 	template <class T>
  99: 	const T* GetPawnData() const { return Cast<T>(PawnData); }
 105: 	void SetPawnData(const UHodgePawnData* InPawnData);
 111: 	UFUNCTION(BlueprintPure, Category = "Hodge|Pawn")
 112: 	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponent() const { return AbilitySystemComponent; }
 119: 	void InitializeAbilitySystem(UHodgeAbilitySystemComponent* InASC, AActor* InOwnerActor);
 123: 	void UninitializeAbilitySystem();
 127: 	void HandleControllerChanged();
 131: 	void HandlePlayerStateReplicated();
 135: 	void SetupPlayerInputComponent();
 141: 	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);
 147: 	void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);
 153: 	void UnregisterAbilitySystemDelegates(const UObject* Subscriber);
 155: protected:
 158: 	virtual void OnRegister() override;
 162: 	virtual void BeginPlay() override;
 168: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
 172: 	UFUNCTION()
 173: 	void OnRep_PawnData();
 176: 	FSimpleMulticastDelegate OnAbilitySystemInitialized;
 179: 	FSimpleMulticastDelegate OnAbilitySystemUninitialized;
 182: 	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_PawnData, Category = "Hodge|Pawn")
 183: 	TObjectPtr<const UHodgePawnData> PawnData;
 186: 	UPROPERTY(Transient)
 187: 	TObjectPtr<UHodgeAbilitySystemComponent> AbilitySystemComponent;
 188: };
```

## HodgeWingComponent.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Component/HodgeWingComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeWingComponent.h)

项目内直接 include（不是运行调用关系）：[Animation/HodgeWingProfile.h](../../../Source/Hodgepodge/Public/Animation/HodgeWingProfile.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "Components/ActorComponent.h"
   7: #include "Animation/HodgeWingProfile.h"
   8: #include "HodgeWingComponent.generated.h"
  10: class ACharacter;
  11: class USkeletalMeshComponent;
  12: class UAnimInstance;
  13: class UAnimSequenceBase;
  14: class UMaterialInstanceDynamic;
  15: class UMaterialInterface;
  16: class UHodgeAnimNotifyState_WingAction;
  19: UCLASS(Blueprintable, ClassGroup=(Hodge), meta=(BlueprintSpawnableComponent))
  20: class HODGEPODGE_API UHodgeWingComponent : public UActorComponent
  21: {
  22: 	GENERATED_BODY()
  23: public:
  25: 	UHodgeWingComponent();
  27: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Wing") TObjectPtr<UHodgeWingProfile> Profile;
  29: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Wing") FName ExistingMeshComponentName;
  31: 	UPROPERTY(BlueprintReadOnly, Transient, Category="Hodge|Wing") FHodgeWingPose Pose;
  33: 	UPROPERTY(BlueprintReadOnly, Transient, Category="Hodge|Wing") float SourcePosition = 0.f;
  35: 	UPROPERTY(BlueprintReadOnly, Transient, Category="Hodge|Wing") int32 SourceInstanceId = INDEX_NONE;
  39: 	UFUNCTION(BlueprintPure, Category="Hodge|Wing") USkeletalMeshComponent* GetWingMesh() const { return WingMesh; }
  43: 	UFUNCTION(BlueprintCallable, Category="Hodge|Wing") bool InitializeWing();
  45: 	UFUNCTION(BlueprintCallable, Category="Hodge|Wing") void RefreshPresentation();
  47: 	void RequestPresentationRefresh();
  52: 	bool PrepareAuthorityHitPose(FGuid ExecutionId);
  59: 	FHodgeWingActionHandle BeginWingAction(int32 BindingIndex, int32 InstanceId, FGuid ExecutionId);
  64: 	UFUNCTION(BlueprintCallable, Category="Hodge|Wing") bool EndWingAction(FHodgeWingActionHandle Handle);
  68: 	void EndWingForExecution(FGuid ExecutionId);
  70: 	UFUNCTION(BlueprintCallable, Category="Hodge|Wing") void ResetWingPresentation();
  72: 	UFUNCTION(BlueprintPure, Category="Hodge|Wing") FHodgeWingPresentationState GetPresentationState() const { return Presentation; }
  74: 	UFUNCTION(BlueprintPure, Category="Hodge|Wing") float GetVisibilityAmount() const { return Fade.Amount; }
  76: 	UFUNCTION(BlueprintPure, Category="Hodge|Wing") bool IsFadingOut() const { return Fade.bFading; }
  83: 	void UpdatePose(float Position, int32 InstanceId, bool bActive, float DeltaSeconds);
  84: protected:
  86: 	virtual void BeginPlay() override;
  90: 	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
  96: 	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
  97: private:
 105: 	bool ResolveSource(int32& BindingIndex, int32& InstanceId, float& Position, UAnimInstance*& SourceInstance);
 107: 	bool EvaluateWindowPose(float Position);
 109: 	UPROPERTY(Transient) TObjectPtr<UHodgeAnimNotifyState_WingAction> ActiveWindow;
 111: 	UPROPERTY(Transient) TObjectPtr<UAnimSequenceBase> ActiveSourceAnimation;
 113: 	float WindowStartTime = 0.f;
 115: 	int32 WingMontageInstanceId = INDEX_NONE;
 117: 	float WindowFadeOutSeconds = -1.f;
 119: 	TSet<TWeakObjectPtr<UHodgeAnimNotifyState_WingAction>> InvalidWindows;
 121: 	void SetWingVisible(bool bVisible);
 123: 	void BeginFadeOut();
 125: 	void ApplyFadeVisibility();
 127: 	UPROPERTY(Transient) TArray<TObjectPtr<UMaterialInstanceDynamic>> DynamicMaterials;
 129: 	UPROPERTY(Transient) TArray<TObjectPtr<UMaterialInterface>> OriginalMaterials;
 131: 	FHodgeWingFadeState Fade;
 133: 	bool IsOwnerPresentationBlocked() const;
 135: 	FHodgeWingPresentationState Presentation;
 137: 	TWeakObjectPtr<UAnimInstance> ActiveSourceInstance;
 139: 	TWeakObjectPtr<UAnimInstance> SuppressedSourceInstance;
 141: 	int32 SuppressedInstanceId = INDEX_NONE;
 143: 	TWeakObjectPtr<UAnimSequenceBase> PreviewSource;
 145: 	int32 PreviewEpoch = 0;
 147: 	float LastPreviewPosition = -1.f;
 149: 	bool bInitialized = false;
 151: 	bool bInitializationFailed = false;
 153: 	bool bRefreshing = false;
 155: 	uint64 LastAuthorityBodyRefreshFrame = MAX_uint64;
 157: 	bool bOwnsWingMesh = false;
 159: 	UPROPERTY(Transient) TObjectPtr<USkeletalMeshComponent> WingMesh;
 161: 	TWeakObjectPtr<ACharacter> Character;
 163: 	FHodgeWingPose ReturnFrom;
 165: 	float ReturnElapsed = 0.f;
 167: 	bool bWasActive = false;
 168: };
```
