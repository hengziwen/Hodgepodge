# Character 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeCharacterBase.cpp

原生 Character 基础、替换移动组件；Receiver 在 PreInit 注册、EndPlay 成对移除。

源码：[Source/Hodgepodge/Private/Character/HodgeCharacterBase.cpp](../../../Source/Hodgepodge/Private/Character/HodgeCharacterBase.cpp)

项目内直接 include（不是运行调用关系）：[Character/HodgeCharacterBase.h](../../../Source/Hodgepodge/Public/Character/HodgeCharacterBase.h)、[Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)、[Equipment/HodgeEquipmentManagerComponent.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h)

定义候选（多行签名仅展示首行）：

- L34: `AHodgeCharacterBase::AHodgeCharacterBase(const FObjectInitializer& ObjectInitializer)`
- L46: `void AHodgeCharacterBase::PreInitializeComponents()`
- L65: `void AHodgeCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L78: `void AHodgeCharacterBase::BeginPlay()`

## HodgeCombatCharacter.cpp

PawnExtension、相机、HealthComponent、原生 RotationComponent；ASC、移动/旋转约束、复制与死亡清理。

源码：[Source/Hodgepodge/Private/Character/HodgeCombatCharacter.cpp](../../../Source/Hodgepodge/Private/Character/HodgeCombatCharacter.cpp)

项目内直接 include（不是运行调用关系）：[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[Character/HodgeEnemyCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeEnemyCharacter.h)、[Component/HodgeEnemyInitComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeEnemyInitComponent.h)、[AbilitySystem/Stats/HodgeAttributeCoordinator.h](../../../Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeAttributeCoordinator.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Camera/HodgeCameraComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraComponent.h)、[Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)、[Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)、[Component/HodgeAttackMotionWarpingComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeAttackMotionWarpingComponent.h)、[Component/HodgeLocomotionPolicyComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLocomotionPolicyComponent.h)、[Component/HodgeDefenseComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeDefenseComponent.h)、[Component/HodgeHealthComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHealthComponent.h)、[Component/HodgeHitReactionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHitReactionComponent.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Core/PlayerController/HodgePlayerController.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)、[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)、[Equipment/HodgeEquipmentDefinition.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentDefinition.h)、[Equipment/HodgeEquipmentInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h)、[Equipment/HodgeEquipmentManagerComponent.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h)、[Equipment/HodgeWeaponInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h)

定义候选（多行签名仅展示首行）：

- L55: `AHodgeCombatCharacter::AHodgeCombatCharacter(const FObjectInitializer& ObjectInitializer)`
- L177: `void AHodgeCombatCharacter::PreInitializeComponents()`
- L184: `void AHodgeCombatCharacter::BeginPlay()`
- L207: `void AHodgeCombatCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L245: `void AHodgeCombatCharacter::Reset()`
- L258: `void AHodgeCombatCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L270: `void AHodgeCombatCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)`
- L297: `void AHodgeCombatCharacter::NotifyControllerChanged()`
- L317: `AHodgePlayerController* AHodgeCombatCharacter::GetHodgePlayerController() const`
- L324: `AHodgePlayerState* AHodgeCombatCharacter::GetHodgePlayerState() const`
- L331: `UHodgeAbilitySystemComponent* AHodgeCombatCharacter::GetHodgeAbilitySystemComponent() const`
- L338: `UAbilitySystemComponent* AHodgeCombatCharacter::GetAbilitySystemComponent() const`
- L350: `void AHodgeCombatCharacter::OnAbilitySystemInitialized()`
- L370: `void AHodgeCombatCharacter::OnAbilitySystemUninitialized()`
- L388: `void AHodgeCombatCharacter::FaceRotation(FRotator NewControlRotation, float DeltaTime)`
- L401: `void AHodgeCombatCharacter::InitializeDefaultEquipment()`
- L469: `void AHodgeCombatCharacter::UninitializeDefaultEquipment()`
- L492: `void AHodgeCombatCharacter::PossessedBy(AController* NewController)`
- L518: `void AHodgeCombatCharacter::UnPossessed()`
- L544: `void AHodgeCombatCharacter::OnRep_Controller()`
- L553: `void AHodgeCombatCharacter::OnRep_PlayerState()`
- L562: `void AHodgeCombatCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)`
- L573: `void AHodgeCombatCharacter::InitializeGameplayTags()`
- L608: `void AHodgeCombatCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const`
- L618: `bool AHodgeCombatCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const`
- L630: `bool AHodgeCombatCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const`
- L642: `bool AHodgeCombatCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const`
- L654: `void AHodgeCombatCharacter::FellOutOfWorld(const class UDamageType& dmgType)`
- L661: `void AHodgeCombatCharacter::OnDeathStarted(AActor*)`
- L670: `void AHodgeCombatCharacter::OnDeathFinished(AActor*)`
- L677: `void AHodgeCombatCharacter::DisableMovementAndCollision()`
- L707: `void AHodgeCombatCharacter::DestroyDueToDeath()`
- L717: `void AHodgeCombatCharacter::UninitAndDestroy()`
- L745: `void AHodgeCombatCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)`
- L761: `void AHodgeCombatCharacter::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)`
- L790: `void AHodgeCombatCharacter::ToggleCrouch()`
- L809: `void AHodgeCombatCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)`
- L823: `void AHodgeCombatCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)`
- L837: `bool AHodgeCombatCharacter::CanJumpInternal_Implementation() const`
- L844: `void AHodgeCombatCharacter::OnRep_ReplicatedAcceleration()`
- L874: `void AHodgeCombatCharacter::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)`
- L887: `void AHodgeCombatCharacter::OnRep_MyTeamID(FGenericTeamId OldTeamID)`
- L894: `bool AHodgeCombatCharacter::UpdateSharedReplication()`
- L927: `void AHodgeCombatCharacter::FastSharedReplication_Implementation(const FSharedRepMovement& SharedRepMovement)`
- L969: `FSharedRepMovement::FSharedRepMovement()`
- L976: `bool FSharedRepMovement::FillForCharacter(ACharacter* Character)`
- L1023: `bool FSharedRepMovement::Equals(const FSharedRepMovement& Other, ACharacter* Character) const`
- L1066: `bool FSharedRepMovement::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)`
- L1104: `void AHodgeCombatCharacter::TryInitializeAttributesAndEquipment()`
- L1126: `bool AHodgeCombatCharacter::PrepareDefaultEquipment()`
- L1133: `bool AHodgeCombatCharacter::IsDefaultEquipmentReady() const`

## HodgeEnemyCharacter.cpp

仅设置 AI 自动控制；旋转/移动参数继承 Combat 基类，尚未完成敌人 ASC 初始化。

源码：[Source/Hodgepodge/Private/Character/HodgeEnemyCharacter.cpp](../../../Source/Hodgepodge/Private/Character/HodgeEnemyCharacter.cpp)

项目内直接 include（不是运行调用关系）：[Character/HodgeEnemyCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeEnemyCharacter.h)、[Component/HodgeHealthComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHealthComponent.h)、[Component/HodgeEnemyInitComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeEnemyInitComponent.h)、[AI/HodgeEnemyAIController.h](../../../Source/Hodgepodge/Public/AI/HodgeEnemyAIController.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)、[Data/HodgeCharacterStatProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeCharacterStatProfile.h)

定义候选（多行签名仅展示首行）：

- L27: `AHodgeEnemyCharacter::AHodgeEnemyCharacter(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)`
- L66: `void AHodgeEnemyCharacter::BeginPlay()`
- L73: `void AHodgeEnemyCharacter::PossessedBy(AController* NewController)`
- L80: `UAbilitySystemComponent* AHodgeEnemyCharacter::GetAbilitySystemComponent() const`
- L88: `bool AHodgeEnemyCharacter::InitializeEnemyAbilitySystem()`
- L94: `void AHodgeEnemyCharacter::OnRep_EnemyPawnData() { EnemyInitComponent->TryInitialize(); }`
- L97: `void AHodgeEnemyCharacter::EndPlay(const EEndPlayReason::Type Reason)`
- L103: `void AHodgeEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L111: `void AHodgeEnemyCharacter::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)`
- L138: `bool AHodgeEnemyCharacter::CanBeLockedBy_Implementation(APawn* SourcePawn) const`
- L144: `bool AHodgeEnemyCharacter::GetLockPoint_Implementation(FName Point, FVector& OutLocation) const`
- L152: `float AHodgeEnemyCharacter::GetLockPriority_Implementation() const { return LockOnPriority; }`

## HodgeHeroCharacter.cpp

构造挂载 HeroComponent；PossessedBy/OnRep_PlayerState 只调用 Super，ASC 接入已收敛。

源码：[Source/Hodgepodge/Private/Character/HodgeHeroCharacter.cpp](../../../Source/Hodgepodge/Private/Character/HodgeHeroCharacter.cpp)

项目内直接 include（不是运行调用关系）：[Character/HodgeHeroCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeHeroCharacter.h)、[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)、[Component/HodgeLockOnComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLockOnComponent.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)

定义候选（多行签名仅展示首行）：

- L22: `void AHodgeHeroCharacter::FaceRotation(FRotator NewControlRotation, float DeltaTime)`
- L33: `AHodgeHeroCharacter::AHodgeHeroCharacter(const FObjectInitializer& ObjectInitializer)`
- L50: `void AHodgeHeroCharacter::PossessedBy(AController* NewController)`
- L66: `void AHodgeHeroCharacter::OnRep_PlayerState()`

## HodgeCharacterBase.h

原生 Character 基础、替换移动组件；Receiver 在 PreInit 注册、EndPlay 成对移除。

源码：[Source/Hodgepodge/Public/Character/HodgeCharacterBase.h](../../../Source/Hodgepodge/Public/Character/HodgeCharacterBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  12: #pragma once
  14: #include "CoreMinimal.h"
  15: #include "GameFramework/Character.h"
  16: #include "HodgeCharacterBase.generated.h"
  36: UCLASS()
  37: class HODGEPODGE_API AHodgeCharacterBase : public ACharacter
  38: {
  39: 	GENERATED_BODY()
  41: public:
  51: 	explicit AHodgeCharacterBase(const FObjectInitializer& ObjectInitializer);
  60: 	virtual void PreInitializeComponents() override;
  69: 	virtual void BeginPlay() override;
  82: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  83: };
```

## HodgeCombatCharacter.h

PawnExtension、相机、HealthComponent、原生 RotationComponent；ASC、移动/旋转约束、复制与死亡清理。

源码：[Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[Character/HodgeCharacterBase.h](../../../Source/Hodgepodge/Public/Character/HodgeCharacterBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "AbilitySystemInterface.h"
   8: #include "GenericTeamAgentInterface.h"
   9: #include "AbilitySystem/HodgeAbilitySystemComponent.h"
  10: #include "Character/HodgeCharacterBase.h"
  11: #include "HodgeCombatCharacter.generated.h"
  13: class UHodgeHealthComponent;
  14: class UHodgeHitReactionComponent;
  15: class UHodgeCharacterRotationComponent;
  16: class UHodgeAttackMotionWarpingComponent;
  17: class UHodgeLocomotionPolicyComponent;
  18: class UHodgeDefenseComponent;
  19: class UHodgeEquipmentInstance;
  20: class UHodgeEquipmentManagerComponent;
  21: class UHodgePawnExtensionComponent;
  22: class UHodgeCameraComponent;
  23: class AHodgePlayerController;
  24: class AHodgePlayerState;
  32: USTRUCT()
  33: struct FHodgeReplicatedAcceleration
  34: {
  35: 	GENERATED_BODY()
  38: 	UPROPERTY()
  39: 	uint8 AccelXYRadians = 0;
  42: 	UPROPERTY()
  43: 	uint8 AccelXYMagnitude = 0;
  46: 	UPROPERTY()
  47: 	int8 AccelZ = 0;
  48: };
  55: USTRUCT()
  56: struct FSharedRepMovement
  57: {
  58: 	GENERATED_BODY()
  62: 	FSharedRepMovement();
  69: 	bool FillForCharacter(ACharacter* Character);
  77: 	bool Equals(const FSharedRepMovement& Other, ACharacter* Character) const;
  86: 	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
  89: 	UPROPERTY(Transient)
  90: 	FRepMovement RepMovement;
  93: 	UPROPERTY(Transient)
  94: 	float RepTimeStamp = 0.0f;
  97: 	UPROPERTY(Transient)
  98: 	uint8 RepMovementMode = 0;
 101: 	UPROPERTY(Transient)
 102: 	bool bProxyIsJumpForceApplied = false;
 105: 	UPROPERTY(Transient)
 106: 	bool bIsCrouched = false;
 107: };
 110: template <>
 112: struct TStructOpsTypeTraits<FSharedRepMovement> : public TStructOpsTypeTraitsBase2<FSharedRepMovement>
 113: {
 114: 	enum
 115: 	{
 117: 		WithNetSerializer = true,
 120: 		WithNetSharedSerialization = true,
 121: 	};
 122: };
 138: UCLASS()
 139: class HODGEPODGE_API AHodgeCombatCharacter : public AHodgeCharacterBase,
 140:                                              public IAbilitySystemInterface,
 141:                                              public IGameplayCueInterface,
 142:                                              public IGameplayTagAssetInterface
 143: {
 144: 	GENERATED_BODY()
 146: 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hodge|Facing", meta=(AllowPrivateAccess="true")) TObjectPtr<UHodgeAttackMotionWarpingComponent> AttackMotionWarpingComponent;
 148: public:
 153: 	AHodgeCombatCharacter(const FObjectInitializer& ObjectInitializer);
 159: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
 160: 	AHodgePlayerController* GetHodgePlayerController() const;
 166: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
 167: 	AHodgePlayerState* GetHodgePlayerState() const;
 173: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
 174: 	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponent() const;
 179: 	UFUNCTION(BlueprintPure, Category = "Hodge|Character")
 180: 	UHodgeCharacterRotationComponent* GetCharacterRotationComponent() const { return RotationComponent; }
 184: 	UFUNCTION(BlueprintPure, Category="Hodge|Character")
 185: 	UHodgeHitReactionComponent* GetHitReactionComponent() const { return HitReactionComponent; }
 187: 	void TryInitializeAttributesAndEquipment();
 191: 	bool PrepareDefaultEquipment();
 195: 	bool IsDefaultEquipmentReady() const;
 201: 	virtual void FaceRotation(FRotator NewControlRotation, float DeltaTime = 0.f) override;
 207: 	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
 213: 	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
 220: 	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
 227: 	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
 234: 	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
 238: 	void ToggleCrouch();
 244: 	virtual void PreInitializeComponents() override;
 248: 	virtual void BeginPlay() override;
 254: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
 258: 	virtual void Reset() override;
 264: 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
 270: 	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
 278: 	virtual void NotifyControllerChanged() override;
 286: 	UFUNCTION(NetMulticast, unreliable)
 287: 	void FastSharedReplication(const FSharedRepMovement& SharedRepMovement);
 290: 	FSharedRepMovement LastSharedReplication;
 296: 	virtual bool UpdateSharedReplication();
 298: protected:
 299: 	friend class UHodgeEnemyInitComponent;
 302: 	virtual void OnAbilitySystemInitialized();
 306: 	virtual void OnAbilitySystemUninitialized();
 310: 	void InitializeDefaultEquipment();
 314: 	void UninitializeDefaultEquipment();
 320: 	virtual void PossessedBy(AController* NewController) override;
 324: 	virtual void UnPossessed() override;
 328: 	virtual void OnRep_Controller() override;
 332: 	virtual void OnRep_PlayerState() override;
 338: 	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
 342: 	void InitializeGameplayTags();
 348: 	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;
 354: 	UFUNCTION()
 355: 	virtual void OnDeathStarted(AActor* OwningActor);
 361: 	UFUNCTION()
 362: 	virtual void OnDeathFinished(AActor* OwningActor);
 366: 	void DisableMovementAndCollision();
 370: 	void DestroyDueToDeath();
 374: 	void UninitAndDestroy();
 378: 	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnDeathFinished"))
 379: 	void K2_OnDeathFinished();
 386: 	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
 394: 	void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);
 401: 	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
 408: 	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
 414: 	virtual bool CanJumpInternal_Implementation() const;
 416: private:
 418: 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hodge|Character", meta=(AllowPrivateAccess="true"))
 419: 	TObjectPtr<UHodgeHitReactionComponent> HitReactionComponent;
 421: 	UPROPERTY(Transient)
 422: 	TObjectPtr<UHodgeEquipmentInstance> DefaultWeaponInstance;
 425: 	UPROPERTY(Transient)
 426: 	TWeakObjectPtr<UHodgeEquipmentManagerComponent> DefaultEquipmentManager;
 429: 	bool bInitializingDefaultEquipment = false;
 432: 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hodge|Character", Meta = (AllowPrivateAccess = "true"))
 433: 	TObjectPtr<UHodgePawnExtensionComponent> PawnExtComponent;
 436: 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hodge|Character", Meta = (AllowPrivateAccess = "true"))
 437: 	TObjectPtr<UHodgeHealthComponent> HealthComponent;
 440: 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hodge|Character", Meta = (AllowPrivateAccess = "true"))
 441: 	TObjectPtr<UHodgeCharacterRotationComponent> RotationComponent;
 443: 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true")) TObjectPtr<UHodgeLocomotionPolicyComponent> LocomotionPolicyComponent;
 445: 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true")) TObjectPtr<UHodgeDefenseComponent> DefenseComponent;
 448: 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hodge|Character", Meta = (AllowPrivateAccess = "true"))
 449: 	TObjectPtr<UHodgeCameraComponent> CameraComponent;
 452: 	UPROPERTY(Transient, ReplicatedUsing = OnRep_ReplicatedAcceleration)
 453: 	FHodgeReplicatedAcceleration ReplicatedAcceleration;
 456: 	UPROPERTY(ReplicatedUsing = OnRep_MyTeamID)
 457: 	FGenericTeamId MyTeamID;
 459: protected:
 465: 	virtual FGenericTeamId DetermineNewTeamAfterPossessionEnds(FGenericTeamId OldTeamID) const
 466: 	{
 468: 		return FGenericTeamId::NoTeam;
 469: 	}
 471: private:
 478: 	UFUNCTION()
 479: 	void OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);
 483: 	UFUNCTION()
 484: 	void OnRep_ReplicatedAcceleration();
 490: 	UFUNCTION()
 491: 	void OnRep_MyTeamID(FGenericTeamId OldTeamID);
 492: };
```

## HodgeEnemyCharacter.h

仅设置 AI 自动控制；旋转/移动参数继承 Combat 基类，尚未完成敌人 ASC 初始化。

源码：[Source/Hodgepodge/Public/Character/HodgeEnemyCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeEnemyCharacter.h)

项目内直接 include（不是运行调用关系）：[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[Data/HodgeAbilitySet.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilitySet.h)、[Interface/HodgeLockOnTargetInterface.h](../../../Source/Hodgepodge/Public/Interface/HodgeLockOnTargetInterface.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "CoreMinimal.h"
   9: #include "Character/HodgeCombatCharacter.h"
  10: #include "Data/HodgeAbilitySet.h"
  11: #include "TimerManager.h"
  12: #include "Interface/HodgeLockOnTargetInterface.h"
  13: #include "HodgeEnemyCharacter.generated.h"
  15: class UHodgePawnData;
  16: class UHodgeEnemyInitComponent;
  22: UCLASS()
  23: class HODGEPODGE_API AHodgeEnemyCharacter : public AHodgeCombatCharacter, public IHodgeLockOnTargetInterface
  24: {
  25: 	GENERATED_BODY()
  27: public:
  29: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|LockOn") FName LockOnSocket;
  30: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|LockOn") bool bAllowLockOnBoundsFallback = true;
  31: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|LockOn") bool bAllowPlayerLockOn = true;
  32: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|LockOn", meta=(ClampMin="0",ClampMax="1")) float LockOnPriority = .5f;
  33: 	virtual bool CanBeLockedBy_Implementation(APawn* SourcePawn) const override;
  34: 	virtual bool GetLockPoint_Implementation(FName LockPointId, FVector& OutLocation) const override;
  35: 	virtual float GetLockPriority_Implementation() const override;
  39: 	explicit AHodgeEnemyCharacter(const FObjectInitializer& ObjectInitializer);
  41:     UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_EnemyPawnData, Category="Hodge|Enemy") TObjectPtr<UHodgePawnData> EnemyPawnData;
  43:     UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Enemy", meta=(ClampMin="1")) int32 InitialLevel = 1;
  47:     UFUNCTION(BlueprintCallable, Category="Hodge|Enemy") bool InitializeEnemyAbilitySystem();
  51:     UFUNCTION(BlueprintPure, Category="Hodge|Enemy") UHodgeEnemyInitComponent* GetEnemyInitComponent() const { return EnemyInitComponent; }
  55:     UHodgeAbilitySystemComponent* GetNativeEnemyAbilitySystem() const { return EnemyAbilitySystem; }
  59:     virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
  63:     virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  67:     virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
  69: protected:
  72: 	virtual void BeginPlay() override;
  79: 	virtual void PossessedBy(AController* NewController) override;
  82: private:
  83:     friend class UHodgeEnemyInitComponent;
  85:     UPROPERTY(VisibleAnywhere, Category="Hodge|Enemy") TObjectPtr<UHodgeEnemyInitComponent> EnemyInitComponent;
  87:     UPROPERTY(VisibleAnywhere, Category="Hodge|Enemy") TObjectPtr<UHodgeAbilitySystemComponent> EnemyAbilitySystem;
  89:     UPROPERTY() TObjectPtr<class UHodgeHealthSet> EnemyHealthSet;
  91:     UPROPERTY() TObjectPtr<class UHodgeCombatSet> EnemyCombatSet;
  93:     UFUNCTION() void OnRep_EnemyPawnData();
  95: protected:
  97: #if WITH_EDITOR
 103: 	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
 105: #endif
 149: };
```

## HodgeHeroCharacter.h

构造挂载 HeroComponent；PossessedBy/OnRep_PlayerState 只调用 Super，ASC 接入已收敛。

源码：[Source/Hodgepodge/Public/Character/HodgeHeroCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeHeroCharacter.h)

项目内直接 include（不是运行调用关系）：[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  16: #pragma once
  18: #include "CoreMinimal.h"
  19: #include "Character/HodgeCombatCharacter.h"
  20: #include "HodgeHeroCharacter.generated.h"
  39: UCLASS()
  40: class HODGEPODGE_API AHodgeHeroCharacter : public AHodgeCombatCharacter
  41: {
  42: 	GENERATED_BODY()
  44: public:
  53: 	explicit AHodgeHeroCharacter(const FObjectInitializer& ObjectInitializer);
  59: public:
  73: 	virtual void PossessedBy(AController* NewController) override;
  81: 	virtual void OnRep_PlayerState() override;
  83: 	virtual void FaceRotation(FRotator NewControlRotation, float DeltaTime = 0.f) override;
  85: private:
  87: 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hodge|Targeting", meta=(AllowPrivateAccess="true"))
  88: 	TObjectPtr<class UHodgeLockOnComponent> LockOnComponent;
  90: 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hodge|Character", Meta = (AllowPrivateAccess = "true"))
  91: 	TObjectPtr<UHodgeHeroComponent> HeroComponent = nullptr;
  92: };
```
