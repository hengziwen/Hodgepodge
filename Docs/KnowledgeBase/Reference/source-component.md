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

CharacterMovement 扩展、地面距离、加速度和能力系统相关移动入口。

源码：[Source/Hodgepodge/Private/Component/HodgeCharacterMovementComponent.cpp](../../../Source/Hodgepodge/Private/Component/HodgeCharacterMovementComponent.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)

定义候选（多行签名仅展示首行）：

- L27: `UHodgeCharacterMovementComponent::UHodgeCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)`
- L33: `void UHodgeCharacterMovementComponent::SimulateMovement(float DeltaTime)`
- L54: `bool UHodgeCharacterMovementComponent::CanAttemptJump() const`
- L63: `void UHodgeCharacterMovementComponent::InitializeComponent()`
- L70: `const FHodgeCharacterGroundInfo& UHodgeCharacterMovementComponent::GetGroundInfo()`
- L151: `void UHodgeCharacterMovementComponent::SetReplicatedAcceleration(const FVector& InAcceleration)`
- L161: `FRotator UHodgeCharacterMovementComponent::GetDeltaRotation(float DeltaTime) const`
- L179: `float UHodgeCharacterMovementComponent::GetMaxSpeed() const`

## HodgeCombatComponentBase.cpp

战斗组件占位，当前构造关闭 Tick，尚无完整命中或连招实现。

源码：[Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp](../../../Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)

定义候选（多行签名仅展示首行）：

- L23: `UHodgeCombatComponentBase::UHodgeCombatComponentBase()`

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
- L247: `void UHodgeHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,`
- L312: `void UHodgeHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)`
- L328: `void UHodgeHeroComponent::CheckDefaultInitialization()`
- L346: `void UHodgeHeroComponent::BeginPlay()`
- L368: `void UHodgeHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L392: `void UHodgeHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)`
- L618: `void UHodgeHeroComponent::AddAdditionalInputConfig(const UHodgeInputConfig* InputConfig)`
- L709: `void UHodgeHeroComponent::RemoveAdditionalInputConfig(const UHodgeInputConfig* InputConfig)`
- L738: `bool UHodgeHeroComponent::IsReadyToBindInputs() const`
- L745: `void UHodgeHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)`
- L768: `void UHodgeHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)`
- L796: `void UHodgeHeroComponent::Input_Move(const FInputActionValue& InputActionValue)`
- L859: `void UHodgeHeroComponent::Input_MoveStopped(const FInputActionValue&                     )`
- L867: `bool UHodgeHeroComponent::HasMoveIntent(float Threshold) const`
- L874: `void UHodgeHeroComponent::SetMoveIntent(const FVector2D& NewValue)`
- L881: `void UHodgeHeroComponent::RefreshMoveIntent()`
- L895: `void UHodgeHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)`
- L934: `void UHodgeHeroComponent::Input_LookStick(const FInputActionValue& InputActionValue)`
- L981: `void UHodgeHeroComponent::Input_Crouch(const FInputActionValue& InputActionValue)`
- L994: `void UHodgeHeroComponent::Input_AutoRun(const FInputActionValue& InputActionValue)`
- L1014: `TSubclassOf<UHodgeCameraMode> UHodgeHeroComponent::DetermineCameraMode() const`
- L1047: `void UHodgeHeroComponent::SetAbilityCameraMode(TSubclassOf<UHodgeCameraMode> CameraMode,`
- L1062: `void UHodgeHeroComponent::ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle)`

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

CharacterMovement 扩展、地面距离、加速度和能力系统相关移动入口。

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
  82: protected:
  84: 	virtual void InitializeComponent() override;
  86: protected:
  88: 	FHodgeCharacterGroundInfo CachedGroundInfo;
  91: 	UPROPERTY(Transient)
  92: 	bool bHasReplicatedAcceleration = false;
  93: };
```

## HodgeCombatComponentBase.h

战斗组件占位，当前构造关闭 Tick，尚无完整命中或连招实现。

源码：[Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)

项目内直接 include（不是运行调用关系）：[Component/HodgeActorComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeActorComponentBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   9: #pragma once
  11: #include "CoreMinimal.h"
  12: #include "Component/HodgeActorComponentBase.h"
  13: #include "HodgeCombatComponentBase.generated.h"
  29: UCLASS()
  30: class HODGEPODGE_API UHodgeCombatComponentBase : public UHodgeActorComponentBase
  31: {
  32: 	GENERATED_BODY()
  34: public:
  41: 	UHodgeCombatComponentBase();
  42: };
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
  96: protected:
  98: 	virtual void OnRegister() override;
 101: 	virtual void BeginPlay() override;
 104: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
 107: 	UFUNCTION()
 108: 	void OnRep_PawnData();
 111: 	FSimpleMulticastDelegate OnAbilitySystemInitialized;
 114: 	FSimpleMulticastDelegate OnAbilitySystemUninitialized;
 117: 	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_PawnData, Category = "Hodge|Pawn")
 118: 	TObjectPtr<const UHodgePawnData> PawnData;
 121: 	UPROPERTY(Transient)
 122: 	TObjectPtr<UHodgeAbilitySystemComponent> AbilitySystemComponent;
 123: };
```
