# Component 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeActorComponentBase.cpp

通用 ActorComponent 基础访问与扩展。

源码：[Source/Hodgepodge/Private/Component/HodgeActorComponentBase.cpp](../../../Source/Hodgepodge/Private/Component/HodgeActorComponentBase.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeActorComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeActorComponentBase.h)

定义候选（多行签名仅展示首行）：

- L21: `UHodgeActorComponentBase::UHodgeActorComponentBase()`
- L37: `void UHodgeActorComponentBase::BeginPlay()`
- L55: `void UHodgeActorComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)`
- L67: `AActor* UHodgeActorComponentBase::GetOwnerCharacter()`
- L78: `void UHodgeActorComponentBase::InitializeComponent()`
- L89: `void UHodgeActorComponentBase::OnReady()`

## HodgeCharacterMovementComponent.cpp

地面距离/加速度、最终旋转过滤、SavedMove 旋转策略重放与权威校正。

源码：[Source/Hodgepodge/Private/Component/HodgeCharacterMovementComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeCharacterMovementComponent.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)、[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)

定义候选（多行签名仅展示首行）：

- L94: `UHodgeCharacterMovementComponent::UHodgeCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)`
- L100: `void UHodgeCharacterMovementComponent::SimulateMovement(float DeltaTime)`
- L121: `bool UHodgeCharacterMovementComponent::CanAttemptJump() const`
- L130: `void UHodgeCharacterMovementComponent::InitializeComponent()`
- L137: `const FHodgeCharacterGroundInfo& UHodgeCharacterMovementComponent::GetGroundInfo()`
- L218: `void UHodgeCharacterMovementComponent::SetReplicatedAcceleration(const FVector& InAcceleration)`
- L228: `FRotator UHodgeCharacterMovementComponent::GetDeltaRotation(float DeltaTime) const`
- L253: `void UHodgeCharacterMovementComponent::PhysicsRotation(float DeltaTime)`
- L277: `bool UHodgeCharacterMovementComponent::MoveUpdatedComponentImpl(const FVector& Delta, const FQuat& NewRotation,`
- L294: `FNetworkPredictionData_Client* UHodgeCharacterMovementComponent::GetPredictionData_Client() const`
- L304: `bool UHodgeCharacterMovementComponent::ClientUpdatePositionAfterServerUpdate()`
- L314: `void UHodgeCharacterMovementComponent::SmoothCorrection(const FVector& OldLocation, const FQuat& OldRotation,`
- L322: `float UHodgeCharacterMovementComponent::GetMaxSpeed() const`

## HodgeCharacterRotationComponent.cpp

ASC 旋转标签约束、锁定 Yaw、恢复、权威复制与移动重放状态。

源码：[Source/Hodgepodge/Private/Component/HodgeCharacterRotationComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeCharacterRotationComponent.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)

定义候选（多行签名仅展示首行）：

- L11: `UHodgeCharacterRotationComponent::UHodgeCharacterRotationComponent(const FObjectInitializer& ObjectInitializer)`
- L18: `void UHodgeCharacterRotationComponent::InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC)`
- L35: `void UHodgeCharacterRotationComponent::UninitializeFromAbilitySystem()`
- L58: `void UHodgeCharacterRotationComponent::HandleConstraintTagChanged(FGameplayTag Tag, int32 NewCount)`
- L63: `void UHodgeCharacterRotationComponent::RefreshLocalState()`
- L77: `void UHodgeCharacterRotationComponent::PublishAuthorityState()`
- L87: `bool UHodgeCharacterRotationComponent::IsReplayingMove() const`
- L93: `FHodgeCharacterRotationState UHodgeCharacterRotationComponent::GetResolvedState() const`
- L101: `bool UHodgeCharacterRotationComponent::IsYawLocked() const { return GetResolvedState().bYawLocked; }`
- L102: `bool UHodgeCharacterRotationComponent::IsRecoveringFacing() const { return GetResolvedState().bRecoveringFacing; }`
- L103: `float UHodgeCharacterRotationComponent::GetLockedYaw() const { return GetResolvedState().LockedYaw; }`
- L105: `FRotator UHodgeCharacterRotationComponent::FilterControlRotation(const FRotator& DesiredRotation, float DeltaSeconds) const`
- L118: `void UHodgeCharacterRotationComponent::NotifyFacingApplied(float DesiredYaw)`
- L129: `void UHodgeCharacterRotationComponent::SetMoveReplayState(const FHodgeCharacterRotationState& State)`
- L135: `void UHodgeCharacterRotationComponent::ClearMoveReplayState()`
- L141: `void UHodgeCharacterRotationComponent::OnRep_RotationState()`
- L151: `void UHodgeCharacterRotationComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L157: `void UHodgeCharacterRotationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)`

## HodgeCombatComponentBase.cpp

Experience 注入 Pawn 的统一战斗协调者；连段输入/记忆/预测校正，服务器采样、去重与结果上报；GA 负责效果施加。

源码：[Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp](../../../Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)、[Equipment/HodgeEquipmentManagerComponent.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h)、[Equipment/HodgeWeaponInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)、[Data/HodgeComboDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeComboDefinition.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)

定义候选（多行签名仅展示首行）：

- L39: `UHodgeCombatComponentBase::UHodgeCombatComponentBase()`
- L47: `void UHodgeCombatComponentBase::EndPlay(const EEndPlayReason::Type Reason)`
- L53: `bool UHodgeCombatComponentBase::ResolveSource(FGameplayTag Tag, FHodgeHitDetectionSession& Session) const`
- L88: `bool UHodgeCombatComponentBase::IsSourceValid(const FHodgeHitDetectionSession& Session) const`
- L100: `uint64 UHodgeCombatComponentBase::CreateDetectionSession(const FGuid& ExecutionId, int32 EventIndex,`
- L134: `bool UHodgeCombatComponentBase::IsDetectionSessionValid(uint64 Handle, const FGuid& ExecutionId) const`
- L140: `bool UHodgeCombatComponentBase::ResetDetectionHistory(uint64 Handle, const FGuid& ExecutionId)`
- L153: `USceneComponent* UHodgeCombatComponentBase::GetDetectionSourceComponent(uint64 Handle, const FGuid& ExecutionId) const`
- L160: `void UHodgeCombatComponentBase::EndDetectionSession(uint64 Handle, const FGuid& ExecutionId)`
- L166: `void UHodgeCombatComponentBase::EndDetectionSessionsForExecution(const FGuid& ExecutionId)`
- L174: `void UHodgeCombatComponentBase::EndAllDetectionSessions()`
- L179: `bool UHodgeCombatComponentBase::SampleDetection(uint64 Handle, const FGuid& ExecutionId,`
- L255: `void UHodgeCombatComponentBase::Configure(UHodgeAbilitySystemComponent* InASC, const UHodgeComboDefinition* InDefinition)`
- L272: `void UHodgeCombatComponentBase::Shutdown()`
- L296: `void UHodgeCombatComponentBase::ClearInput()`
- L302: `bool UHodgeCombatComponentBase::InputPressed(FGameplayTag InputTag)`
- L321: `const FHodgeComboTransition* UHodgeCombatComponentBase::SelectTransition(FGameplayTag Trigger, bool bEvent) const`
- L341: `bool UHodgeCombatComponentBase::IsAuthorized(FGameplayAbilitySpecHandle Handle) const`
- L346: `int16 UHodgeCombatComponentBase::ExecutionKey() const`
- L353: `bool UHodgeCombatComponentBase::PrepareTransition(const FHodgeComboTransition& Edge, FGameplayAbilitySpecHandle Handle)`
- L385: `bool UHodgeCombatComponentBase::TryTransition(FGameplayTag Trigger, bool bEvent)`
- L423: `bool UHodgeCombatComponentBase::PrepareServerActivation(FGameplayAbilitySpecHandle Handle, const FGameplayEventData* Payload)`
- L445: `bool UHodgeCombatComponentBase::PrepareConfirmedActivation(FGameplayAbilitySpecHandle Handle,`
- L469: `void UHodgeCombatComponentBase::RejectServerActivation(const FGameplayEventData* Payload)`
- L475: `void UHodgeCombatComponentBase::CompleteServerActivation()`
- L496: `void UHodgeCombatComponentBase::ExecutionStarted(UHodgeGameplayAbility_Definition* Ability)`
- L507: `void UHodgeCombatComponentBase::ExecutionEnded(UHodgeGameplayAbility_Definition* Ability)`
- L521: `void UHodgeCombatComponentBase::WindowsChanged(UHodgeGameplayAbility_Definition* Ability)`
- L530: `void UHodgeCombatComponentBase::TimelineEvent(UHodgeGameplayAbility_Definition* Ability, FGameplayTag Event)`
- L537: `void UHodgeCombatComponentBase::DrainEvents()`
- L555: `void UHodgeCombatComponentBase::SetNode(FGameplayTag Node)`
- L569: `void UHodgeCombatComponentBase::ResetSession(bool bEndAbility)`
- L583: `double UHodgeCombatComponentBase::ComboTime() const`
- L590: `FGameplayTag UHodgeCombatComponentBase::GetRememberedComboTag() const`
- L596: `float UHodgeCombatComponentBase::GetComboMemoryRemainingTime() const`
- L602: `FGameplayTag UHodgeCombatComponentBase::TransitionSourceNode() const`
- L609: `bool UHodgeCombatComponentBase::HasResumeTransition(FGameplayTag Node) const`
- L618: `void UHodgeCombatComponentBase::RetainComboMemory()`
- L629: `void UHodgeCombatComponentBase::ExpireComboMemory()`
- L639: `void UHodgeCombatComponentBase::PublishComboMemory()`
- L648: `void UHodgeCombatComponentBase::EndCurrentExecution()`
- L653: `void UHodgeCombatComponentBase::OnRep_ComboMemory()`
- L662: `void UHodgeCombatComponentBase::HandlePredictionRejected()`
- L670: `void UHodgeCombatComponentBase::ServerSynchronizeComboMemory_Implementation()`
- L676: `void UHodgeCombatComponentBase::ClientCorrectComboMemory_Implementation(FHodgeComboMemoryState State)`
- L682: `void UHodgeCombatComponentBase::TickComponent(float Delta, ELevelTick Type, FActorComponentTickFunction* Tick)`
- L709: `void UHodgeCombatComponentBase::ServerMoveCancel_Implementation(AActor* Avatar, FGameplayAbilitySpecHandle Handle, int32 Key)`
- L721: `void UHodgeCombatComponentBase::ClientMoveCancelResult_Implementation() { bMoveRequestPending = false; }`
- L723: `void UHodgeCombatComponentBase::ServerReturnToEntry_Implementation(AActor* Avatar, FGameplayTag SourceNode, int32 Key,`
- L736: `void UHodgeCombatComponentBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L743: `void UHodgeCombatComponentBase::OnRep_ObserverTags(const FGameplayTagContainer& Previous)`
- L754: `UHodgeCombatComponentBase* UHodgeCombatComponentBase::FindCombatComponent(const AActor* Avatar)`
- L759: `bool UHodgeCombatComponentBase::IsComboReady() const`
- L767: `void UHodgeCombatComponentBase::BindPawnExtension()`
- L778: `void UHodgeCombatComponentBase::HandleAbilitySystemInitialized()`
- L785: `void UHodgeCombatComponentBase::HandleAbilitySystemUninitialized()`
- L790: `void UHodgeCombatComponentBase::OnRegister()`
- L796: `void UHodgeCombatComponentBase::BeginPlay()`
- L803: `void UHodgeCombatComponentBase::OnUnregister()`

## HodgeExperienceManagerComponent.cpp

GameState 上的 Experience 复制、资源加载、插件激活、Action 执行与 Loaded 委托。

源码：[Source/Hodgepodge/Private/Component/HodgeExperienceManagerComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeExperienceManagerComponent.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeExperienceManagerComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h)、[Data/HodgeExperienceDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceDefinition.h)、[Data/HodgeExperienceActionSet.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceActionSet.h)、[Data/HodgeExperienceManager.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceManager.h)、[Data/HodgeAssetManager.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManager.h)

定义候选（多行签名仅展示首行）：

- L63: `UHodgeExperienceManagerComponent::UHodgeExperienceManagerComponent(const FObjectInitializer& ObjectInitializer)`
- L70: `void UHodgeExperienceManagerComponent::SetCurrentExperience(FPrimaryAssetId ExperienceId)`
- L101: `void UHodgeExperienceManagerComponent::CallOrRegister_OnExperienceLoaded_HighPriority(`
- L117: `void UHodgeExperienceManagerComponent::CallOrRegister_OnExperienceLoaded(FOnHodgeExperienceLoaded::FDelegate&& Delegate)`
- L132: `void UHodgeExperienceManagerComponent::CallOrRegister_OnExperienceLoaded_LowPriority(`
- L148: `const UHodgeExperienceDefinition* UHodgeExperienceManagerComponent::GetCurrentExperienceChecked() const`
- L161: `bool UHodgeExperienceManagerComponent::IsExperienceLoaded() const`
- L168: `void UHodgeExperienceManagerComponent::OnRep_CurrentExperience()`
- L175: `void UHodgeExperienceManagerComponent::StartExperienceLoad()`
- L314: `void UHodgeExperienceManagerComponent::OnExperienceLoadComplete()`
- L399: `void UHodgeExperienceManagerComponent::OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result)`
- L412: `void UHodgeExperienceManagerComponent::OnExperienceFullLoadCompleted()`
- L511: `void UHodgeExperienceManagerComponent::OnActionDeactivationCompleted()`
- L527: `void UHodgeExperienceManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L536: `void UHodgeExperienceManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L627: `bool UHodgeExperienceManagerComponent::ShouldShowLoadingScreen(FString& OutReason) const`
- L645: `void UHodgeExperienceManagerComponent::OnAllActionsDeactivated()`

## HodgeHealthComponent.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Component/HodgeHealthComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeHealthComponent.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeHealthComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHealthComponent.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[Data/HodgeAssetManager.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManager.h)、[Data/HodgeGameData.h](../../../Source/Hodgepodge/Public/Data/HodgeGameData.h)

定义候选（多行签名仅展示首行）：

- L30: `UHodgeHealthComponent::UHodgeHealthComponent(const FObjectInitializer& ObjectInitializer)`
- L53: `void UHodgeHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L63: `void UHodgeHealthComponent::OnUnregister()`
- L73: `void UHodgeHealthComponent::InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC)`
- L142: `void UHodgeHealthComponent::UninitializeFromAbilitySystem()`
- L168: `void UHodgeHealthComponent::ClearGameplayTags()`
- L182: `float UHodgeHealthComponent::GetHealth() const`
- L189: `float UHodgeHealthComponent::GetMaxHealth() const`
- L196: `float UHodgeHealthComponent::GetHealthNormalized() const`
- L216: `void UHodgeHealthComponent::HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser,`
- L225: `void UHodgeHealthComponent::HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser,`
- L234: `void UHodgeHealthComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser,`
- L308: `void UHodgeHealthComponent::OnRep_DeathState(EHodgeDeathState OldDeathState)`
- L380: `void UHodgeHealthComponent::StartDeath()`
- L412: `void UHodgeHealthComponent::FinishDeath()`
- L444: `void UHodgeHealthComponent::DamageSelfDestruct(bool bFellOutOfWorld)`

## HodgeHeroComponent.cpp

玩家 Init State 协调、ASC 接入、输入与相机；额外输入句柄持久化并在移除/EndPlay 解绑。新增“移动意图”信号（HasMoveIntent / GetMoveIntent / OnMoveIntentChanged），记 Input_Move 原始输入量、Completed/Canceled 清零，供移动取消后摇消费。

源码：[Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)、[Core/PlayerController/HodgePlayerController.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)、[Core/LocalPlayer/HodgeLocalPlayerBase.h](../../../Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)、[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[Input/HodgeInputConfig.h](../../../Source/Hodgepodge/Public/Input/HodgeInputConfig.h)、[Input/HodgeInputComponent.h](../../../Source/Hodgepodge/Public/Input/HodgeInputComponent.h)、[Camera/HodgeCameraComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraComponent.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Camera/HodgeCameraMode.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode.h)

定义候选（多行签名仅展示首行）：

- L81: `const FName UHodgeHeroComponent::NAME_BindInputsNow("BindInputsNow");`
- L84: `const FName UHodgeHeroComponent::NAME_ActorFeatureName("Hero");`
- L87: `UHodgeHeroComponent::UHodgeHeroComponent(const FObjectInitializer& ObjectInitializer)`
- L98: `void UHodgeHeroComponent::OnRegister()`
- L142: `bool UHodgeHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,`
- L262: `void UHodgeHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,`
- L329: `void UHodgeHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)`
- L345: `void UHodgeHeroComponent::CheckDefaultInitialization()`
- L365: `void UHodgeHeroComponent::BeginPlay()`
- L389: `void UHodgeHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L413: `void UHodgeHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)`
- L639: `void UHodgeHeroComponent::AddAdditionalInputConfig(const UHodgeInputConfig* InputConfig)`
- L730: `void UHodgeHeroComponent::RemoveAdditionalInputConfig(const UHodgeInputConfig* InputConfig)`
- L759: `bool UHodgeHeroComponent::IsReadyToBindInputs() const`
- L766: `void UHodgeHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)`
- L789: `void UHodgeHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)`
- L817: `void UHodgeHeroComponent::Input_Move(const FInputActionValue& InputActionValue)`
- L890: `void UHodgeHeroComponent::Input_MoveStopped(const FInputActionValue&                     )`
- L898: `bool UHodgeHeroComponent::HasMoveIntent(float Threshold) const`
- L905: `void UHodgeHeroComponent::SetMoveIntent(const FVector2D& NewValue)`
- L912: `void UHodgeHeroComponent::RefreshMoveIntent()`
- L926: `void UHodgeHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)`
- L965: `void UHodgeHeroComponent::Input_LookStick(const FInputActionValue& InputActionValue)`
- L1012: `void UHodgeHeroComponent::Input_Crouch(const FInputActionValue& InputActionValue)`
- L1025: `void UHodgeHeroComponent::Input_AutoRun(const FInputActionValue& InputActionValue)`
- L1045: `TSubclassOf<UHodgeCameraMode> UHodgeHeroComponent::DetermineCameraMode() const`
- L1078: `void UHodgeHeroComponent::SetAbilityCameraMode(TSubclassOf<UHodgeCameraMode> CameraMode,`
- L1093: `void UHodgeHeroComponent::ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle)`

## HodgeInteractionComponentBase.cpp

交互组件基础占位；完整扫描、交互规则与 UI 需另行实现。

源码：[Source/Hodgepodge/Private/Component/HodgeInteractionComponentBase.cpp](../../../Source/Hodgepodge/Private/Component/HodgeInteractionComponentBase.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeInteractionComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeInteractionComponentBase.h)

定义候选（多行签名仅展示首行）：


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

- L18: `const FName UHodgePawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");`
- L20: `UHodgePawnExtensionComponent::UHodgePawnExtensionComponent(const FObjectInitializer& ObjectInitializer)`
- L34: `void UHodgePawnExtensionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L42: `void UHodgePawnExtensionComponent::OnRegister()`
- L61: `void UHodgePawnExtensionComponent::BeginPlay()`
- L75: `void UHodgePawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L86: `void UHodgePawnExtensionComponent::SetPawnData(const UHodgePawnData* InPawnData)`
- L116: `void UHodgePawnExtensionComponent::OnRep_PawnData()`
- L122: `void UHodgePawnExtensionComponent::InitializeAbilitySystem(UHodgeAbilitySystemComponent* InASC,`
- L182: `void UHodgePawnExtensionComponent::UninitializeAbilitySystem()`
- L227: `void UHodgePawnExtensionComponent::HandleControllerChanged()`
- L251: `void UHodgePawnExtensionComponent::HandlePlayerStateReplicated()`
- L257: `void UHodgePawnExtensionComponent::SetupPlayerInputComponent()`
- L263: `void UHodgePawnExtensionComponent::CheckDefaultInitialization()`
- L278: `bool UHodgePawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager,`
- L338: `void UHodgePawnExtensionComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager,`
- L348: `void UHodgePawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)`
- L360: `void UHodgePawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(`
- L376: `void UHodgePawnExtensionComponent::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)`
- L385: `void UHodgePawnExtensionComponent::UnregisterAbilitySystemDelegates(const UObject* Subscriber)`

## HodgeActorComponentBase.h

通用 ActorComponent 基础访问与扩展。

源码：[Source/Hodgepodge/Public/Component/HodgeActorComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeActorComponentBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   9: #pragma once
  11: #include "CoreMinimal.h"
  12: #include "Components/ActorComponent.h"
  13: #include "HodgeActorComponentBase.generated.h"
  15: class UAbilitySystemComponent;
  37: UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
  38: class HODGEPODGE_API UHodgeActorComponentBase : public UActorComponent
  39: {
  40: 	GENERATED_BODY()
  42: public:
  48: 	UHodgeActorComponentBase();
  50: protected:
  57: 	virtual void BeginPlay() override;
  59: public:
  67: 	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
  68: 	                           FActorComponentTickFunction* ThisTickFunction) override;
  74: 	AActor* GetOwnerCharacter();
  81: 	virtual void InitializeComponent() override;
  89: 	virtual void OnReady();
  95: 	bool IsServer() const
  96: 	{
  97: 		return GetOwner()->HasAuthority();
  98: 	}
  99: };
```

## HodgeCharacterMovementComponent.h

地面距离/加速度、最终旋转过滤、SavedMove 旋转策略重放与权威校正。

源码：[Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "GameFramework/CharacterMovementComponent.h"
   6: #include "NativeGameplayTags.h"
   8: #include "HodgeCharacterMovementComponent.generated.h"
  10: class UObject;
  11: struct FFrame;
  14: HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_MovementStopped);
  20: USTRUCT(BlueprintType)
  21: struct FHodgeCharacterGroundInfo
  22: {
  23: 	GENERATED_BODY()
  26: 	FHodgeCharacterGroundInfo()
  27: 		: LastUpdateFrame(0)
  28: 		  , GroundDistance(0.0f)
  29: 	{
  30: 	}
  33: 	uint64 LastUpdateFrame;
  36: 	UPROPERTY(BlueprintReadOnly)
  37: 	FHitResult GroundHitResult;
  40: 	UPROPERTY(BlueprintReadOnly)
  41: 	float GroundDistance;
  42: };
  50: UCLASS(Config = Game)
  51: class HODGEPODGE_API UHodgeCharacterMovementComponent : public UCharacterMovementComponent
  52: {
  53: 	GENERATED_BODY()
  55: public:
  57: 	UHodgeCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);
  60: 	virtual void SimulateMovement(float DeltaTime) override;
  63: 	virtual bool CanAttemptJump() const override;
  66: 	UFUNCTION(BlueprintCallable, Category = "Hodge|CharacterMovement")
  67: 	const FHodgeCharacterGroundInfo& GetGroundInfo();
  70: 	void SetReplicatedAcceleration(const FVector& InAcceleration);
  75: 	virtual FRotator GetDeltaRotation(float DeltaTime) const override;
  78: 	virtual float GetMaxSpeed() const override;
  80: 	virtual void PhysicsRotation(float DeltaTime) override;
  81: 	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
  82: 	virtual bool ClientUpdatePositionAfterServerUpdate() override;
  83: 	virtual void SmoothCorrection(const FVector& OldLocation, const FQuat& OldRotation,
  84: 		const FVector& NewLocation, const FQuat& NewRotation) override;
  88: protected:
  90: 	virtual void InitializeComponent() override;
  92: 	virtual bool MoveUpdatedComponentImpl(const FVector& Delta, const FQuat& NewRotation, bool bSweep,
  93: 		FHitResult* OutHit = nullptr, ETeleportType Teleport = ETeleportType::None) override;
  95: protected:
  97: 	FHodgeCharacterGroundInfo CachedGroundInfo;
 100: 	UPROPERTY(Transient)
 101: 	bool bHasReplicatedAcceleration = false;
 103: 	bool bApplyingRotationCorrection = false;
 104: };
```

## HodgeCharacterRotationComponent.h

ASC 旋转标签约束、锁定 Yaw、恢复、权威复制与移动重放状态。

源码：[Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   3: #include "CoreMinimal.h"
   4: #include "Components/PawnComponent.h"
   5: #include "GameplayTagContainer.h"
   6: #include "HodgeCharacterRotationComponent.generated.h"
   8: class UHodgeAbilitySystemComponent;
  11: USTRUCT(BlueprintType)
  12: struct FHodgeCharacterRotationState
  13: {
  14: 	GENERATED_BODY()
  16: 	UPROPERTY(BlueprintReadOnly)
  17: 	bool bYawLocked = false;
  19: 	UPROPERTY(BlueprintReadOnly)
  20: 	bool bRecoveringFacing = false;
  22: 	UPROPERTY(BlueprintReadOnly)
  23: 	float LockedYaw = 0.f;
  24: };
  30: UCLASS(BlueprintType, Meta = (BlueprintSpawnableComponent))
  31: class HODGEPODGE_API UHodgeCharacterRotationComponent : public UPawnComponent
  32: {
  33: 	GENERATED_BODY()
  35: public:
  36: 	UHodgeCharacterRotationComponent(const FObjectInitializer& ObjectInitializer);
  38: 	void InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC);
  39: 	void UninitializeFromAbilitySystem();
  41: 	UFUNCTION(BlueprintPure, Category = "Hodge|Rotation")
  42: 	bool IsYawLocked() const;
  44: 	UFUNCTION(BlueprintPure, Category = "Hodge|Rotation")
  45: 	bool IsRecoveringFacing() const;
  47: 	UFUNCTION(BlueprintPure, Category = "Hodge|Rotation")
  48: 	float GetLockedYaw() const;
  50: 	UFUNCTION(BlueprintPure, Category = "Hodge|Rotation")
  51: 	FHodgeCharacterRotationState GetResolvedState() const;
  53: 	FRotator FilterControlRotation(const FRotator& DesiredRotation, float DeltaSeconds) const;
  54: 	void NotifyFacingApplied(float DesiredYaw);
  55: 	float GetRecoveryTurnRate() const { return RecoveryTurnRate; }
  57: 	void SetMoveReplayState(const FHodgeCharacterRotationState& State);
  58: 	void ClearMoveReplayState();
  60: 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
  62: protected:
  63: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  65: private:
  66: 	void HandleConstraintTagChanged(FGameplayTag Tag, int32 NewCount);
  67: 	void RefreshLocalState();
  68: 	void PublishAuthorityState();
  69: 	bool IsReplayingMove() const;
  71: 	UFUNCTION()
  72: 	void OnRep_RotationState();
  74: 	UPROPERTY(EditDefaultsOnly, Category = "Hodge|Rotation", Meta = (ClampMin = "1", Units = "deg/s"))
  75: 	float RecoveryTurnRate = 360.f;
  77: 	UPROPERTY(Transient)
  78: 	TWeakObjectPtr<UHodgeAbilitySystemComponent> AbilitySystemComponent;
  80: 	UPROPERTY(ReplicatedUsing = OnRep_RotationState)
  81: 	FHodgeCharacterRotationState ReplicatedState;
  83: 	FHodgeCharacterRotationState LocalState;
  84: 	FHodgeCharacterRotationState ReplayState;
  85: 	FDelegateHandle RotationTagHandle;
  86: 	FDelegateHandle MovementStoppedTagHandle;
  87: 	bool bHasReplayState = false;
  88: };
```

## HodgeCombatComponentBase.h

Experience 注入 Pawn 的统一战斗协调者；连段输入/记忆/预测校正，服务器采样、去重与结果上报；GA 负责效果施加。

源码：[Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)

项目内直接 include（不是运行调用关系）：[Component/HodgeActorComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeActorComponentBase.h)、[Combat/HodgeHitDetection.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitDetection.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   3: #include "CoreMinimal.h"
   4: #include "Component/HodgeActorComponentBase.h"
   5: #include "Combat/HodgeHitDetection.h"
   6: #include "GameplayAbilitySpecHandle.h"
   7: #include "HodgeCombatComponentBase.generated.h"
   9: class UHodgeWeaponInstance;
  10: class USceneComponent;
  11: class UHodgeAbilitySystemComponent;
  12: class UHodgeComboDefinition;
  13: class UHodgeGameplayAbility_Definition;
  14: class UHodgePawnExtensionComponent;
  15: struct FGameplayEventData;
  16: struct FHodgeComboTransition;
  18: USTRUCT()
  19: struct FHodgeComboMemoryState
  20: {
  21: 	GENERATED_BODY()
  22: 	UPROPERTY() FGameplayTag Node;
  23: 	UPROPERTY() double ExpiresAt = 0;
  24: 	UPROPERTY() int16 ExecutionKey = 0;
  25: };
  27: USTRUCT()
  28: struct FHodgeHitDetectionSession
  29: {
  30: 	GENERATED_BODY()
  31: 	UPROPERTY() FGuid ExecutionId;
  32: 	UPROPERTY() int32 EventIndex = INDEX_NONE;
  33: 	UPROPERTY() FHodgeHitDetectionRequest Request;
  34: 	UPROPERTY() FHodgeHitSource Source;
  35: 	UPROPERTY() TWeakObjectPtr<USceneComponent> SourceComponent;
  36: 	UPROPERTY() TWeakObjectPtr<UHodgeWeaponInstance> Weapon;
  37: 	bool bWeaponSource = false;
  38: 	int32 SampleSequence = 0;
  39: 	FHodgeHitGeometry Previous;
  40: };
  43: UCLASS(Blueprintable, ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
  44: class HODGEPODGE_API UHodgeCombatComponentBase : public UHodgeActorComponentBase
  45: {
  46: 	GENERATED_BODY()
  47: public:
  48: 	UHodgeCombatComponentBase();
  51: 	void Configure(UHodgeAbilitySystemComponent* InASC, const UHodgeComboDefinition* InDefinition);
  52: 	void Shutdown();
  53: 	bool InputPressed(FGameplayTag InputTag);
  54: 	void ClearInput();
  55: 	void ExecutionStarted(UHodgeGameplayAbility_Definition* Ability);
  56: 	void ExecutionEnded(UHodgeGameplayAbility_Definition* Ability);
  57: 	void WindowsChanged(UHodgeGameplayAbility_Definition* Ability);
  58: 	void TimelineEvent(UHodgeGameplayAbility_Definition* Ability, FGameplayTag Event);
  59: 	bool IsAuthorized(FGameplayAbilitySpecHandle Handle) const;
  60: 	bool PrepareServerActivation(FGameplayAbilitySpecHandle Handle, const FGameplayEventData* Payload);
  61: 	void RejectServerActivation(const FGameplayEventData* Payload);
  62: 	bool PrepareConfirmedActivation(FGameplayAbilitySpecHandle Handle, const FGameplayEventData& Payload);
  63: 	void CompleteServerActivation();
  64: 	void HandlePredictionRejected();
  65: 	UFUNCTION(BlueprintPure, Category="Hodge|Combat")
  66: 	FGameplayTag GetCurrentComboTag() const { return CurrentComboTag; }
  67: 	UFUNCTION(BlueprintPure, Category="Hodge|Combat")
  68: 	FGameplayTag GetRememberedComboTag() const;
  69: 	UFUNCTION(BlueprintPure, Category="Hodge|Combat")
  70: 	float GetComboMemoryRemainingTime() const;
  72: 	static UHodgeCombatComponentBase* FindCombatComponent(const AActor* Avatar);
  73: 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
  76: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Combat", meta=(TitleProperty="SourceTag"))
  77: 	TArray<FHodgeHitSource> HitSources;
  79: 	uint64 CreateDetectionSession(const FGuid& ExecutionId, int32 EventIndex, const FHodgeHitDetectionRequest& Request);
  80: 	bool SampleDetection(uint64 Handle, const FGuid& ExecutionId, FHodgeHitDetectionBatch& OutBatch);
  81: 	bool ResetDetectionHistory(uint64 Handle, const FGuid& ExecutionId);
  82: 	bool IsDetectionSessionValid(uint64 Handle, const FGuid& ExecutionId) const;
  83: 	USceneComponent* GetDetectionSourceComponent(uint64 Handle, const FGuid& ExecutionId) const;
  84: 	void EndDetectionSession(uint64 Handle, const FGuid& ExecutionId);
  85: 	void EndDetectionSessionsForExecution(const FGuid& ExecutionId);
  86: 	void EndAllDetectionSessions();
  87: protected:
  88: 	virtual void OnRegister() override;
  89: 	virtual void OnUnregister() override;
  90: 	virtual void BeginPlay() override;
  91: 	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* TickFunction) override;
  92: 	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
  93: private:
  94: 	void BindPawnExtension();
  95: 	bool IsComboReady() const;
  96: 	void HandleAbilitySystemInitialized();
  97: 	void HandleAbilitySystemUninitialized();
  98: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgePawnExtensionComponent> PawnExtension;
  99: 	FGameplayTagContainer AppliedObserverTags;
 102: 	friend struct FHodgeComboTestAccess;
 104: 	struct FQueuedEvent
 105: 	{
 106: 		TWeakObjectPtr<UHodgeGameplayAbility_Definition> Ability;
 107: 		int16 Key = 0;
 108: 		FGameplayTag Tag;
 109: 	};
 111: 	void DrainEvents();
 112: 	TArray<FQueuedEvent> QueuedEvents;
 113: 	bool bDrainingEvents = false;
 114: 	const FHodgeComboTransition* SelectTransition(FGameplayTag Trigger, bool bEvent) const;
 115: 	bool TryTransition(FGameplayTag Trigger, bool bEvent);
 116: 	bool PrepareTransition(const FHodgeComboTransition& Edge, FGameplayAbilitySpecHandle Handle);
 117: 	void ResetSession(bool bEndAbility);
 118: 	void EndCurrentExecution();
 119: 	void RetainComboMemory();
 120: 	void ExpireComboMemory();
 121: 	void PublishComboMemory();
 122: 	bool HasResumeTransition(FGameplayTag Node) const;
 123: 	FGameplayTag TransitionSourceNode() const;
 124: 	double ComboTime() const;
 125: 	void SetNode(FGameplayTag Node);
 126: 	int16 ExecutionKey() const;
 127: 	UFUNCTION(Server, Reliable)
 128: 	void ServerMoveCancel(AActor* Avatar, FGameplayAbilitySpecHandle Handle, int32 Key);
 129: 	UFUNCTION(Server, Reliable)
 130: 	void ServerReturnToEntry(AActor* Avatar, FGameplayTag SourceNode, int32 Key, FGameplayTag Intent);
 131: 	UFUNCTION(Client, Reliable)
 132: 	void ClientMoveCancelResult();
 133: 	UFUNCTION(Server, Reliable)
 134: 	void ServerSynchronizeComboMemory();
 135: 	UFUNCTION(Client, Reliable)
 136: 	void ClientCorrectComboMemory(FHodgeComboMemoryState State);
 137: 	UFUNCTION()
 138: 	void OnRep_ComboMemory();
 139: 	UFUNCTION()
 140: 	void OnRep_ObserverTags(const FGameplayTagContainer& Previous);
 141: 	UPROPERTY(Transient)
 142: 	TObjectPtr<UHodgeAbilitySystemComponent> ASC;
 143: 	UPROPERTY(Transient)
 144: 	TObjectPtr<const UHodgeComboDefinition> Definition;
 145: 	UPROPERTY(Transient)
 146: 	TObjectPtr<UHodgeGameplayAbility_Definition> CurrentAbility;
 147: 	UPROPERTY(Transient)
 148: 	FGameplayTag CurrentComboTag;
 149: 	UPROPERTY(ReplicatedUsing=OnRep_ComboMemory)
 150: 	FHodgeComboMemoryState ReplicatedComboMemory;
 151: 	FHodgeComboMemoryState ComboMemory;
 152: 	FHodgeComboMemoryState PreviousComboMemory;
 153: 	UPROPERTY(ReplicatedUsing=OnRep_ObserverTags)
 154: 	FGameplayTagContainer ObserverTags;
 155: 	FGameplayTagContainer OwnedNodeTags;
 156: 	FGameplayTag BufferedInput;
 157: 	double InputExpiresAt = 0;
 158: 	FGameplayAbilitySpecHandle AuthorizedHandle;
 159: 	FGameplayTag PendingNode;
 160: 	bool bSwitching = false;
 161: 	bool bTransitionStarted = false;
 162: 	bool bMemoryCorrectionPending = false;
 163: 	bool bMoveRequestPending = false;
 164: 	bool bEvaluating = false;
 165: 	bool bShuttingDown = false;
 167: 	bool ResolveSource(FGameplayTag Tag, FHodgeHitDetectionSession& Session) const;
 168: 	bool IsSourceValid(const FHodgeHitDetectionSession& Session) const;
 169: 	UPROPERTY(Transient)
 170: 	TMap<uint64, FHodgeHitDetectionSession> Sessions;
 171: 	uint64 NextHandle = 0;
 172: };
```

## HodgeExperienceManagerComponent.h

GameState 上的 Experience 复制、资源加载、插件激活、Action 执行与 Loaded 委托。

源码：[Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h)

项目内直接 include（不是运行调用关系）：[Interface/LoadingProcessInterface.h](../../../Source/Hodgepodge/Public/Interface/LoadingProcessInterface.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CoreMinimal.h"
   6: #include "Components/GameStateComponent.h"
   7: #include "Interface/LoadingProcessInterface.h"
   8: #include "HodgeExperienceManagerComponent.generated.h"
  11: namespace UE::GameFeatures
  12: {
  13:     struct FResult;
  14: }
  17: class UHodgeExperienceDefinition;
  20: DECLARE_MULTICAST_DELEGATE_OneParam(FOnHodgeExperienceLoaded, const UHodgeExperienceDefinition*               );
  23: enum class EHodgeExperienceLoadState
  24: {
  25:     Unloaded,
  26:     Loading,
  27:     LoadingGameFeatures,
  28:     LoadingChaosTestingDelay,
  29:     ExecutingActions,
  30:     Loaded,
  31:     Deactivating
  32: };
  35: UCLASS()
  36: class HODGEPODGE_API UHodgeExperienceManagerComponent final : public UGameStateComponent,
  37:                                                               public ILoadingProcessInterface
  38: {
  39:     GENERATED_BODY()
  41: public:
  43:     UHodgeExperienceManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  47:     virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  52:     virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
  56:     void SetCurrentExperience(FPrimaryAssetId ExperienceId);
  59:     void CallOrRegister_OnExperienceLoaded_HighPriority(FOnHodgeExperienceLoaded::FDelegate&& Delegate);
  62:     void CallOrRegister_OnExperienceLoaded(FOnHodgeExperienceLoaded::FDelegate&& Delegate);
  65:     void CallOrRegister_OnExperienceLoaded_LowPriority(FOnHodgeExperienceLoaded::FDelegate&& Delegate);
  68:     const UHodgeExperienceDefinition* GetCurrentExperienceChecked() const;
  71:     bool IsExperienceLoaded() const;
  73: private:
  75:     UFUNCTION()
  76:     void OnRep_CurrentExperience();
  79:     void StartExperienceLoad();
  82:     void OnExperienceLoadComplete();
  85:     void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
  88:     void OnExperienceFullLoadCompleted();
  91:     void OnActionDeactivationCompleted();
  94:     void OnAllActionsDeactivated();
  96: private:
  98:     UPROPERTY(ReplicatedUsing=OnRep_CurrentExperience)
  99:     TObjectPtr<const UHodgeExperienceDefinition> CurrentExperience;
 102:     EHodgeExperienceLoadState LoadState = EHodgeExperienceLoadState::Unloaded;
 105:     int32 NumGameFeaturePluginsLoading = 0;
 108:     TArray<FString> GameFeaturePluginURLs;
 111:     int32 NumObservedPausers = 0;
 114:     int32 NumExpectedPausers = 0;
 117:     FOnHodgeExperienceLoaded OnExperienceLoaded_HighPriority;
 120:     FOnHodgeExperienceLoaded OnExperienceLoaded;
 123:     FOnHodgeExperienceLoaded OnExperienceLoaded_LowPriority;
 124: };
```

## HodgeHealthComponent.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Component/HodgeHealthComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHealthComponent.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   6: #include "CoreMinimal.h"
   9: #include "Components/GameFrameworkComponent.h"
  11: #include "HodgeHealthComponent.generated.h"
  15: class UHodgeAbilitySystemComponent;
  18: class UHodgeHealthSet;
  21: class UObject;
  24: struct FFrame;
  27: struct FGameplayEffectSpec;
  31: DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHodgeHealth_DeathEvent, AActor*, OwningActor);
  37: DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FHodgeHealth_AttributeChanged, UHodgeHealthComponent*, HealthComponent,
  38:                                               float, OldValue, float, NewValue, AActor*, Instigator);
  49: UENUM(BlueprintType)
  50: enum class EHodgeDeathState : uint8
  51: {
  53: 	NotDead = 0,
  56: 	DeathStarted,
  59: 	DeathFinished
  60: };
  74: UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
  75: class HODGEPODGE_API UHodgeHealthComponent : public UGameFrameworkComponent
  76: {
  77: 	GENERATED_BODY()
  79: public:
  81: 	UHodgeHealthComponent(const FObjectInitializer& ObjectInitializer);
  85: 	UFUNCTION(BlueprintPure, Category = "Hodge|Health")
  86: 	static UHodgeHealthComponent* FindHealthComponent(const AActor* Actor)
  87: 	{
  89: 		return (Actor ? Actor->FindComponentByClass<UHodgeHealthComponent>() : nullptr);
  90: 	}
  94: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Health")
  95: 	void InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC);
  99: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Health")
 100: 	void UninitializeFromAbilitySystem();
 104: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Health")
 105: 	float GetHealth() const;
 109: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Health")
 110: 	float GetMaxHealth() const;
 114: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Health")
 115: 	float GetHealthNormalized() const;
 118: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Health")
 119: 	EHodgeDeathState GetDeathState() const { return DeathState; }
 123: 	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Hodge|Health",
 124: 		Meta = (ExpandBoolAsExecs = "ReturnValue"))
 125: 	bool IsDeadOrDying() const { return (DeathState > EHodgeDeathState::NotDead); }
 129: 	virtual void StartDeath();
 133: 	virtual void FinishDeath();
 137: 	virtual void DamageSelfDestruct(bool bFellOutOfWorld = false);
 139: public:
 142: 	UPROPERTY(BlueprintAssignable)
 143: 	FHodgeHealth_AttributeChanged OnHealthChanged;
 147: 	UPROPERTY(BlueprintAssignable)
 148: 	FHodgeHealth_AttributeChanged OnMaxHealthChanged;
 152: 	UPROPERTY(BlueprintAssignable)
 153: 	FHodgeHealth_DeathEvent OnDeathStarted;
 157: 	UPROPERTY(BlueprintAssignable)
 158: 	FHodgeHealth_DeathEvent OnDeathFinished;
 160: protected:
 162: 	virtual void OnUnregister() override;
 165: 	void ClearGameplayTags();
 169: 	virtual void HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser,
 170: 	                                 const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue,
 171: 	                                 float NewValue);
 174: 	virtual void HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser,
 175: 	                                    const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude,
 176: 	                                    float OldValue, float NewValue);
 179: 	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser,
 180: 	                               const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue,
 181: 	                               float NewValue);
 185: 	UFUNCTION()
 186: 	virtual void OnRep_DeathState(EHodgeDeathState OldDeathState);
 188: protected:
 191: 	UPROPERTY()
 192: 	TObjectPtr<UHodgeAbilitySystemComponent> AbilitySystemComponent;
 197: 	UPROPERTY()
 198: 	TObjectPtr<const UHodgeHealthSet> HealthSet;
 202: 	UPROPERTY(ReplicatedUsing = OnRep_DeathState)
 203: 	EHodgeDeathState DeathState;
 204: };
```

## HodgeHeroComponent.h

玩家 Init State 协调、ASC 接入、输入与相机；额外输入句柄持久化并在移除/EndPlay 解绑。新增“移动意图”信号（HasMoveIntent / GetMoveIntent / OnMoveIntentChanged），记 Input_Move 原始输入量、Completed/Canceled 清零，供移动取消后摇消费。

源码：[Source/Hodgepodge/Public/Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)

项目内直接 include（不是运行调用关系）：[GameFeatures/GameFeatureAction_AddInputContextMapping.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddInputContextMapping.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   6: #include "Components/GameFrameworkInitStateInterface.h"
   9: #include "Components/PawnComponent.h"
  12: #include "GameFeatures/GameFeatureAction_AddInputContextMapping.h"
  15: #include "GameplayAbilitySpecHandle.h"
  17: #include "HodgeHeroComponent.generated.h"
  20: namespace EEndPlayReason
  21: {
  22: 	enum Type : int;
  23: }
  27: class UGameFrameworkComponentManager;
  30: class UInputComponent;
  33: class UHodgeCameraMode;
  36: class UHodgeInputConfig;
  38: class UObject;
  41: struct FActorInitStateChangedParams;
  43: struct FFrame;
  46: struct FGameplayTag;
  49: struct FInputActionValue;
  57: DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHodgeMoveIntentChanged, bool, bHasMoveIntent);
  66: UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
  67: class HODGEPODGE_API UHodgeHeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
  68: {
  69: 	GENERATED_BODY()
  71: public:
  73: 	UHodgeHeroComponent(const FObjectInitializer& ObjectInitializer);
  77: 	UFUNCTION(BlueprintPure, Category = "Hodge|Hero")
  78: 	static UHodgeHeroComponent* FindHeroComponent(const AActor* Actor)
  79: 	{
  81: 		return (Actor ? Actor->FindComponentByClass<UHodgeHeroComponent>() : nullptr);
  82: 	}
  86: 	void SetAbilityCameraMode(TSubclassOf<UHodgeCameraMode> CameraMode,
  87: 	                          const FGameplayAbilitySpecHandle& OwningSpecHandle);
  91: 	void ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle);
  95: 	void AddAdditionalInputConfig(const UHodgeInputConfig* InputConfig);
  99: 	void RemoveAdditionalInputConfig(const UHodgeInputConfig* InputConfig);
 103: 	bool IsReadyToBindInputs() const;
 116: 	UFUNCTION(BlueprintPure, Category = "Hodge|Hero|Input")
 117: 	bool HasMoveIntent(float Threshold = 0.1f) const;
 120: 	UFUNCTION(BlueprintPure, Category = "Hodge|Hero|Input")
 121: 	FVector2D GetMoveIntent() const { return CurrentMoveInput; }
 124: 	UPROPERTY(BlueprintAssignable, Category = "Hodge|Hero|Input")
 125: 	FHodgeMoveIntentChanged OnMoveIntentChanged;
 129: 	static const FName NAME_BindInputsNow;
 133: 	static const FName NAME_ActorFeatureName;
 138: 	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
 141: 	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
 142: 	                                FGameplayTag DesiredState) const override;
 145: 	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
 146: 	                                   FGameplayTag DesiredState) override;
 149: 	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
 152: 	virtual void CheckDefaultInitialization() override;
 156: protected:
 158: 	virtual void OnRegister() override;
 161: 	virtual void BeginPlay() override;
 164: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
 167: 	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);
 170: 	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
 173: 	void Input_AbilityInputTagReleased(FGameplayTag InputTag);
 176: 	void Input_Move(const FInputActionValue& InputActionValue);
 180: 	void Input_MoveStopped(const FInputActionValue& InputActionValue);
 183: 	void Input_LookMouse(const FInputActionValue& InputActionValue);
 186: 	void Input_LookStick(const FInputActionValue& InputActionValue);
 189: 	void Input_Crouch(const FInputActionValue& InputActionValue);
 192: 	void Input_AutoRun(const FInputActionValue& InputActionValue);
 195: 	TSubclassOf<UHodgeCameraMode> DetermineCameraMode() const;
 198: 	void SetMoveIntent(const FVector2D& NewValue);
 201: 	void RefreshMoveIntent();
 203: protected:
 205: 	UPROPERTY(EditAnywhere)
 206: 	TArray<FInputMappingContextAndPriority> DefaultInputMappings;
 210: 	UPROPERTY()
 211: 	TSubclassOf<UHodgeCameraMode> AbilityCameraMode;
 215: 	FGameplayAbilitySpecHandle AbilityCameraModeOwningSpecHandle;
 219: 	bool bReadyToBindInputs;
 227: 	FVector2D CurrentMoveInput = FVector2D::ZeroVector;
 230: 	bool bLastMoveIntent = false;
 240: 	TMap<const UHodgeInputConfig*, TArray<uint32>> AdditionalInputConfigHandles;
 241: };
```

## HodgeInteractionComponentBase.h

交互组件基础占位；完整扫描、交互规则与 UI 需另行实现。

源码：[Source/Hodgepodge/Public/Component/HodgeInteractionComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeInteractionComponentBase.h)

项目内直接 include（不是运行调用关系）：[Component/HodgeActorComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeActorComponentBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   8: #pragma once
  10: #include "CoreMinimal.h"
  11: #include "Component/HodgeActorComponentBase.h"
  12: #include "HodgeInteractionComponentBase.generated.h"
  24: UCLASS()
  25: class HODGEPODGE_API UHodgeInteractionComponentBase : public UHodgeActorComponentBase
  26: {
  27: 	GENERATED_BODY()
  28: };
```

## HodgeMovementComponentBase.h

通用移动组件基础占位，与 CharacterMovement 派生类需区分。

源码：[Source/Hodgepodge/Public/Component/HodgeMovementComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeMovementComponentBase.h)

项目内直接 include（不是运行调用关系）：[Component/HodgeActorComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeActorComponentBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   8: #pragma once
  10: #include "CoreMinimal.h"
  11: #include "Component/HodgeActorComponentBase.h"
  12: #include "HodgeMovementComponentBase.generated.h"
  24: UCLASS()
  25: class HODGEPODGE_API UHodgeMovementComponentBase : public UHodgeActorComponentBase
  26: {
  27: 	GENERATED_BODY()
  28: };
```

## HodgePawnExtensionComponent.h

PawnData 复制、Init State、ASC 关联/解除、TagRelationshipMapping 与 ClearAbilityInput；需验证退出顺序。

源码：[Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CoreMinimal.h"
   6: #include "Components/GameFrameworkInitStateInterface.h"
   7: #include "Components/PawnComponent.h"
   8: #include "HodgePawnExtensionComponent.generated.h"
  10: class UHodgePawnData;
  11: class UHodgeAbilitySystemComponent;
  14: namespace EEndPlayReason
  15: {
  16: 	enum Type : int;
  17: }
  23: UCLASS()
  24: class HODGEPODGE_API UHodgePawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
  25: {
  26: 	GENERATED_BODY()
  28: public:
  30: 	UHodgePawnExtensionComponent(const FObjectInitializer& ObjectInitializer);
  33: 	static const FName NAME_ActorFeatureName;
  38: 	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
  41: 	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
  42: 	                                FGameplayTag DesiredState) const override;
  45: 	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
  46: 	                                   FGameplayTag DesiredState) override;
  49: 	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
  52: 	virtual void CheckDefaultInitialization() override;
  57: 	UFUNCTION(BlueprintPure, Category = "Hodge|Pawn")
  58: 	static UHodgePawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor)
  59: 	{
  61: 		return (Actor ? Actor->FindComponentByClass<UHodgePawnExtensionComponent>() : nullptr);
  62: 	}
  65: 	template <class T>
  66: 	const T* GetPawnData() const { return Cast<T>(PawnData); }
  69: 	void SetPawnData(const UHodgePawnData* InPawnData);
  72: 	UFUNCTION(BlueprintPure, Category = "Hodge|Pawn")
  73: 	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponent() const { return AbilitySystemComponent; }
  76: 	void InitializeAbilitySystem(UHodgeAbilitySystemComponent* InASC, AActor* InOwnerActor);
  79: 	void UninitializeAbilitySystem();
  82: 	void HandleControllerChanged();
  85: 	void HandlePlayerStateReplicated();
  88: 	void SetupPlayerInputComponent();
  91: 	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);
  94: 	void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);
  97: 	void UnregisterAbilitySystemDelegates(const UObject* Subscriber);
  99: protected:
 101: 	virtual void OnRegister() override;
 104: 	virtual void BeginPlay() override;
 107: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
 110: 	UFUNCTION()
 111: 	void OnRep_PawnData();
 114: 	FSimpleMulticastDelegate OnAbilitySystemInitialized;
 117: 	FSimpleMulticastDelegate OnAbilitySystemUninitialized;
 120: 	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_PawnData, Category = "Hodge|Pawn")
 121: 	TObjectPtr<const UHodgePawnData> PawnData;
 124: 	UPROPERTY(Transient)
 125: 	TObjectPtr<UHodgeAbilitySystemComponent> AbilitySystemComponent;
 126: };
```
