# Character 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeCharacterBase.cpp

原生 Character 基础、替换移动组件；Receiver 在 PreInit 注册、EndPlay 成对移除。

源码：[Source/Hodgepodge/Private/Character/HodgeCharacterBase.cpp](../../../Source/Hodgepodge/Private/Character/HodgeCharacterBase.cpp)

项目内直接 include（不是运行调用关系）：[Character/HodgeCharacterBase.h](../../../Source/Hodgepodge/Public/Character/HodgeCharacterBase.h)、[Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)、[Equipment/HodgeEquipmentManagerComponent.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h)

定义候选（多行签名仅展示首行）：

- L31: `AHodgeCharacterBase::AHodgeCharacterBase(const FObjectInitializer& ObjectInitializer)`
- L43: `void AHodgeCharacterBase::PreInitializeComponents()`
- L62: `void AHodgeCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L75: `void AHodgeCharacterBase::BeginPlay()`

## HodgeCombatCharacter.cpp

PawnExtension、相机、HealthComponent、原生 RotationComponent；ASC、移动/旋转约束、复制与死亡清理。

源码：[Source/Hodgepodge/Private/Character/HodgeCombatCharacter.cpp](../../../Source/Hodgepodge/Private/Character/HodgeCombatCharacter.cpp)

项目内直接 include（不是运行调用关系）：[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[AbilitySystem/Stats/HodgeAttributeCoordinator.h](../../../Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeAttributeCoordinator.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Camera/HodgeCameraComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraComponent.h)、[Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)、[Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)、[Component/HodgeHealthComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHealthComponent.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Core/PlayerController/HodgePlayerController.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)、[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)、[Equipment/HodgeEquipmentDefinition.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentDefinition.h)、[Equipment/HodgeEquipmentInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h)、[Equipment/HodgeEquipmentManagerComponent.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h)、[Equipment/HodgeWeaponInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h)

定义候选（多行签名仅展示首行）：

- L46: `AHodgeCombatCharacter::AHodgeCombatCharacter(const FObjectInitializer& ObjectInitializer)`
- L164: `void AHodgeCombatCharacter::PreInitializeComponents()`
- L171: `void AHodgeCombatCharacter::BeginPlay()`
- L194: `void AHodgeCombatCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L229: `void AHodgeCombatCharacter::Reset()`
- L242: `void AHodgeCombatCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L254: `void AHodgeCombatCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)`
- L281: `void AHodgeCombatCharacter::NotifyControllerChanged()`
- L301: `AHodgePlayerController* AHodgeCombatCharacter::GetHodgePlayerController() const`
- L308: `AHodgePlayerState* AHodgeCombatCharacter::GetHodgePlayerState() const`
- L315: `UHodgeAbilitySystemComponent* AHodgeCombatCharacter::GetHodgeAbilitySystemComponent() const`
- L322: `UAbilitySystemComponent* AHodgeCombatCharacter::GetAbilitySystemComponent() const`
- L334: `void AHodgeCombatCharacter::OnAbilitySystemInitialized()`
- L351: `void AHodgeCombatCharacter::OnAbilitySystemUninitialized()`
- L365: `void AHodgeCombatCharacter::FaceRotation(FRotator NewControlRotation, float DeltaTime)`
- L376: `void AHodgeCombatCharacter::InitializeDefaultEquipment()`
- L440: `void AHodgeCombatCharacter::UninitializeDefaultEquipment()`
- L462: `void AHodgeCombatCharacter::PossessedBy(AController* NewController)`
- L488: `void AHodgeCombatCharacter::UnPossessed()`
- L514: `void AHodgeCombatCharacter::OnRep_Controller()`
- L523: `void AHodgeCombatCharacter::OnRep_PlayerState()`
- L532: `void AHodgeCombatCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)`
- L543: `void AHodgeCombatCharacter::InitializeGameplayTags()`
- L578: `void AHodgeCombatCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const`
- L588: `bool AHodgeCombatCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const`
- L600: `bool AHodgeCombatCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const`
- L612: `bool AHodgeCombatCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const`
- L624: `void AHodgeCombatCharacter::FellOutOfWorld(const class UDamageType& dmgType)`
- L631: `void AHodgeCombatCharacter::OnDeathStarted(AActor*)`
- L640: `void AHodgeCombatCharacter::OnDeathFinished(AActor*)`
- L647: `void AHodgeCombatCharacter::DisableMovementAndCollision()`
- L677: `void AHodgeCombatCharacter::DestroyDueToDeath()`
- L687: `void AHodgeCombatCharacter::UninitAndDestroy()`
- L715: `void AHodgeCombatCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)`
- L731: `void AHodgeCombatCharacter::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)`
- L760: `void AHodgeCombatCharacter::ToggleCrouch()`
- L779: `void AHodgeCombatCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)`
- L793: `void AHodgeCombatCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)`
- L807: `bool AHodgeCombatCharacter::CanJumpInternal_Implementation() const`
- L814: `void AHodgeCombatCharacter::OnRep_ReplicatedAcceleration()`
- L844: `void AHodgeCombatCharacter::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)`
- L857: `void AHodgeCombatCharacter::OnRep_MyTeamID(FGenericTeamId OldTeamID)`
- L864: `bool AHodgeCombatCharacter::UpdateSharedReplication()`
- L897: `void AHodgeCombatCharacter::FastSharedReplication_Implementation(const FSharedRepMovement& SharedRepMovement)`
- L939: `FSharedRepMovement::FSharedRepMovement()`
- L946: `bool FSharedRepMovement::FillForCharacter(ACharacter* Character)`
- L993: `bool FSharedRepMovement::Equals(const FSharedRepMovement& Other, ACharacter* Character) const`
- L1036: `bool FSharedRepMovement::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)`
- L1073: `void AHodgeCombatCharacter::TryInitializeAttributesAndEquipment()`

## HodgeEnemyCharacter.cpp

仅设置 AI 自动控制；旋转/移动参数继承 Combat 基类，尚未完成敌人 ASC 初始化。

源码：[Source/Hodgepodge/Private/Character/HodgeEnemyCharacter.cpp](../../../Source/Hodgepodge/Private/Character/HodgeEnemyCharacter.cpp)

项目内直接 include（不是运行调用关系）：[Character/HodgeEnemyCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeEnemyCharacter.h)

定义候选（多行签名仅展示首行）：

- L10: `AHodgeEnemyCharacter::AHodgeEnemyCharacter(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)`
- L41: `void AHodgeEnemyCharacter::BeginPlay()`
- L46: `void AHodgeEnemyCharacter::PossessedBy(AController* NewController)`
- L54: `void AHodgeEnemyCharacter::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)`

## HodgeHeroCharacter.cpp

构造挂载 HeroComponent；PossessedBy/OnRep_PlayerState 只调用 Super，ASC 接入已收敛。

源码：[Source/Hodgepodge/Private/Character/HodgeHeroCharacter.cpp](../../../Source/Hodgepodge/Private/Character/HodgeHeroCharacter.cpp)

项目内直接 include（不是运行调用关系）：[Character/HodgeHeroCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeHeroCharacter.h)、[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)

定义候选（多行签名仅展示首行）：

- L23: `AHodgeHeroCharacter::AHodgeHeroCharacter(const FObjectInitializer& ObjectInitializer)`
- L39: `void AHodgeHeroCharacter::PossessedBy(AController* NewController)`
- L55: `void AHodgeHeroCharacter::OnRep_PlayerState()`

## HodgeCharacterBase.h

原生 Character 基础、替换移动组件；Receiver 在 PreInit 注册、EndPlay 成对移除。

源码：[Source/Hodgepodge/Public/Character/HodgeCharacterBase.h](../../../Source/Hodgepodge/Public/Character/HodgeCharacterBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   9: #pragma once
  11: #include "CoreMinimal.h"
  12: #include "GameFramework/Character.h"
  13: #include "HodgeCharacterBase.generated.h"
  33: UCLASS()
  34: class HODGEPODGE_API AHodgeCharacterBase : public ACharacter
  35: {
  36: 	GENERATED_BODY()
  38: public:
  45: 	explicit AHodgeCharacterBase(const FObjectInitializer& ObjectInitializer);
  53: 	virtual void PreInitializeComponents() override;
  61: 	virtual void BeginPlay() override;
  71: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  72: };
```

## HodgeCombatCharacter.h

PawnExtension、相机、HealthComponent、原生 RotationComponent；ASC、移动/旋转约束、复制与死亡清理。

源码：[Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[Character/HodgeCharacterBase.h](../../../Source/Hodgepodge/Public/Character/HodgeCharacterBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   3: #include "CoreMinimal.h"
   4: #include "AbilitySystemInterface.h"
   5: #include "GenericTeamAgentInterface.h"
   6: #include "AbilitySystem/HodgeAbilitySystemComponent.h"
   7: #include "Character/HodgeCharacterBase.h"
   8: #include "HodgeCombatCharacter.generated.h"
  10: class UHodgeHealthComponent;
  11: class UHodgeCharacterRotationComponent;
  12: class UHodgeEquipmentInstance;
  13: class UHodgeEquipmentManagerComponent;
  14: class UHodgePawnExtensionComponent;
  15: class UHodgeCameraComponent;
  16: class AHodgePlayerController;
  17: class AHodgePlayerState;
  25: USTRUCT()
  26: struct FHodgeReplicatedAcceleration
  27: {
  28: 	GENERATED_BODY()
  31: 	UPROPERTY()
  32: 	uint8 AccelXYRadians = 0;
  35: 	UPROPERTY()
  36: 	uint8 AccelXYMagnitude = 0;
  39: 	UPROPERTY()
  40: 	int8 AccelZ = 0;
  41: };
  48: USTRUCT()
  49: struct FSharedRepMovement
  50: {
  51: 	GENERATED_BODY()
  54: 	FSharedRepMovement();
  57: 	bool FillForCharacter(ACharacter* Character);
  60: 	bool Equals(const FSharedRepMovement& Other, ACharacter* Character) const;
  63: 	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
  66: 	UPROPERTY(Transient)
  67: 	FRepMovement RepMovement;
  70: 	UPROPERTY(Transient)
  71: 	float RepTimeStamp = 0.0f;
  74: 	UPROPERTY(Transient)
  75: 	uint8 RepMovementMode = 0;
  78: 	UPROPERTY(Transient)
  79: 	bool bProxyIsJumpForceApplied = false;
  82: 	UPROPERTY(Transient)
  83: 	bool bIsCrouched = false;
  84: };
  87: template <>
  88: struct TStructOpsTypeTraits<FSharedRepMovement> : public TStructOpsTypeTraitsBase2<FSharedRepMovement>
  89: {
  90: 	enum
  91: 	{
  93: 		WithNetSerializer = true,
  96: 		WithNetSharedSerialization = true,
  97: 	};
  98: };
 114: UCLASS()
 115: class HODGEPODGE_API AHodgeCombatCharacter : public AHodgeCharacterBase,
 116:                                              public IAbilitySystemInterface,
 117:                                              public IGameplayCueInterface,
 118:                                              public IGameplayTagAssetInterface
 119: {
 120: 	GENERATED_BODY()
 122: public:
 124: 	AHodgeCombatCharacter(const FObjectInitializer& ObjectInitializer);
 127: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
 128: 	AHodgePlayerController* GetHodgePlayerController() const;
 131: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
 132: 	AHodgePlayerState* GetHodgePlayerState() const;
 135: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
 136: 	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponent() const;
 138: 	UFUNCTION(BlueprintPure, Category = "Hodge|Character")
 139: 	UHodgeCharacterRotationComponent* GetCharacterRotationComponent() const { return RotationComponent; }
 140: 	void TryInitializeAttributesAndEquipment();
 142: 	virtual void FaceRotation(FRotator NewControlRotation, float DeltaTime = 0.f) override;
 145: 	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
 148: 	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
 151: 	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
 154: 	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
 157: 	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
 160: 	void ToggleCrouch();
 165: 	virtual void PreInitializeComponents() override;
 168: 	virtual void BeginPlay() override;
 171: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
 174: 	virtual void Reset() override;
 177: 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
 180: 	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
 187: 	virtual void NotifyControllerChanged() override;
 192: 	UFUNCTION(NetMulticast, unreliable)
 193: 	void FastSharedReplication(const FSharedRepMovement& SharedRepMovement);
 196: 	FSharedRepMovement LastSharedReplication;
 199: 	virtual bool UpdateSharedReplication();
 201: protected:
 203: 	virtual void OnAbilitySystemInitialized();
 206: 	virtual void OnAbilitySystemUninitialized();
 209: 	void InitializeDefaultEquipment();
 212: 	void UninitializeDefaultEquipment();
 215: 	virtual void PossessedBy(AController* NewController) override;
 218: 	virtual void UnPossessed() override;
 221: 	virtual void OnRep_Controller() override;
 224: 	virtual void OnRep_PlayerState() override;
 227: 	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
 230: 	void InitializeGameplayTags();
 233: 	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;
 236: 	UFUNCTION()
 237: 	virtual void OnDeathStarted(AActor* OwningActor);
 240: 	UFUNCTION()
 241: 	virtual void OnDeathFinished(AActor* OwningActor);
 244: 	void DisableMovementAndCollision();
 247: 	void DestroyDueToDeath();
 250: 	void UninitAndDestroy();
 253: 	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnDeathFinished"))
 254: 	void K2_OnDeathFinished();
 257: 	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
 260: 	void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);
 263: 	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
 266: 	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
 269: 	virtual bool CanJumpInternal_Implementation() const;
 271: private:
 273: 	UPROPERTY(Transient)
 274: 	TObjectPtr<UHodgeEquipmentInstance> DefaultWeaponInstance;
 277: 	UPROPERTY(Transient)
 278: 	TWeakObjectPtr<UHodgeEquipmentManagerComponent> DefaultEquipmentManager;
 281: 	bool bInitializingDefaultEquipment = false;
 284: 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hodge|Character", Meta = (AllowPrivateAccess = "true"))
 285: 	TObjectPtr<UHodgePawnExtensionComponent> PawnExtComponent;
 288: 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hodge|Character", Meta = (AllowPrivateAccess = "true"))
 289: 	TObjectPtr<UHodgeHealthComponent> HealthComponent;
 291: 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hodge|Character", Meta = (AllowPrivateAccess = "true"))
 292: 	TObjectPtr<UHodgeCharacterRotationComponent> RotationComponent;
 295: 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hodge|Character", Meta = (AllowPrivateAccess = "true"))
 296: 	TObjectPtr<UHodgeCameraComponent> CameraComponent;
 299: 	UPROPERTY(Transient, ReplicatedUsing = OnRep_ReplicatedAcceleration)
 300: 	FHodgeReplicatedAcceleration ReplicatedAcceleration;
 303: 	UPROPERTY(ReplicatedUsing = OnRep_MyTeamID)
 304: 	FGenericTeamId MyTeamID;
 306: protected:
 308: 	virtual FGenericTeamId DetermineNewTeamAfterPossessionEnds(FGenericTeamId OldTeamID) const
 309: 	{
 311: 		return FGenericTeamId::NoTeam;
 312: 	}
 314: private:
 316: 	UFUNCTION()
 317: 	void OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);
 320: 	UFUNCTION()
 321: 	void OnRep_ReplicatedAcceleration();
 324: 	UFUNCTION()
 325: 	void OnRep_MyTeamID(FGenericTeamId OldTeamID);
 326: };
```

## HodgeEnemyCharacter.h

仅设置 AI 自动控制；旋转/移动参数继承 Combat 基类，尚未完成敌人 ASC 初始化。

源码：[Source/Hodgepodge/Public/Character/HodgeEnemyCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeEnemyCharacter.h)

项目内直接 include（不是运行调用关系）：[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CoreMinimal.h"
   6: #include "Character/HodgeCombatCharacter.h"
   7: #include "HodgeEnemyCharacter.generated.h"
  12: UCLASS()
  13: class HODGEPODGE_API AHodgeEnemyCharacter : public AHodgeCombatCharacter
  14: {
  15: 	GENERATED_BODY()
  17: public:
  18: 	explicit AHodgeEnemyCharacter(const FObjectInitializer& ObjectInitializer);
  20: protected:
  22: 	virtual void BeginPlay() override;
  26: 	virtual void PossessedBy(AController* NewController) override;
  29: #if WITH_EDITOR
  32: 	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
  34: #endif
  78: };
```

## HodgeHeroCharacter.h

构造挂载 HeroComponent；PossessedBy/OnRep_PlayerState 只调用 Super，ASC 接入已收敛。

源码：[Source/Hodgepodge/Public/Character/HodgeHeroCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeHeroCharacter.h)

项目内直接 include（不是运行调用关系）：[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  13: #pragma once
  15: #include "CoreMinimal.h"
  16: #include "Character/HodgeCombatCharacter.h"
  17: #include "HodgeHeroCharacter.generated.h"
  36: UCLASS()
  37: class HODGEPODGE_API AHodgeHeroCharacter : public AHodgeCombatCharacter
  38: {
  39: 	GENERATED_BODY()
  41: public:
  47: 	explicit AHodgeHeroCharacter(const FObjectInitializer& ObjectInitializer);
  53: public:
  64: 	virtual void PossessedBy(AController* NewController) override;
  71: 	virtual void OnRep_PlayerState() override;
  73: private:
  75: 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hodge|Character", Meta = (AllowPrivateAccess = "true"))
  76: 	TObjectPtr<UHodgeHeroComponent> HeroComponent = nullptr;
  77: };
```
