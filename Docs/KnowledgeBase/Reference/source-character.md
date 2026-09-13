# Character 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeCharacterBase.cpp

原生 Character 基础、替换移动组件、Receiver 生命周期。EndPlay 事件配对需修正。

源码：[Source/Hodgepodge/Private/Character/HodgeCharacterBase.cpp](../../../Source/Hodgepodge/Private/Character/HodgeCharacterBase.cpp)

项目内直接 include（不是运行调用关系）：[Character/HodgeCharacterBase.h](../../../Source/Hodgepodge/Public/Character/HodgeCharacterBase.h)、[Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)

定义候选（多行签名仅展示首行）：

- L29: `AHodgeCharacterBase::AHodgeCharacterBase(const FObjectInitializer& ObjectInitializer)`
- L41: `void AHodgeCharacterBase::PreInitializeComponents()`
- L55: `void AHodgeCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L69: `void AHodgeCharacterBase::BeginPlay()`

## HodgeCombatCharacter.cpp

PawnExtension、相机、ASC 查询、移动标签、复制与死亡占位逻辑。

源码：[Source/Hodgepodge/Private/Character/HodgeCombatCharacter.cpp](../../../Source/Hodgepodge/Private/Character/HodgeCombatCharacter.cpp)

项目内直接 include（不是运行调用关系）：[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Camera/HodgeCameraComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraComponent.h)、[Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Core/PlayerController/HodgePlayerController.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)

定义候选（多行签名仅展示首行）：

- L36: `AHodgeCombatCharacter::AHodgeCombatCharacter(const FObjectInitializer& ObjectInitializer)`
- L154: `void AHodgeCombatCharacter::PreInitializeComponents()`
- L161: `void AHodgeCombatCharacter::BeginPlay()`
- L184: `void AHodgeCombatCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L207: `void AHodgeCombatCharacter::Reset()`
- L220: `void AHodgeCombatCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L232: `void AHodgeCombatCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)`
- L259: `void AHodgeCombatCharacter::NotifyControllerChanged()`
- L279: `AHodgePlayerController* AHodgeCombatCharacter::GetHodgePlayerController() const`
- L286: `AHodgePlayerState* AHodgeCombatCharacter::GetHodgePlayerState() const`
- L293: `UHodgeAbilitySystemComponent* AHodgeCombatCharacter::GetHodgeAbilitySystemComponent() const`
- L300: `UAbilitySystemComponent* AHodgeCombatCharacter::GetAbilitySystemComponent() const`
- L312: `void AHodgeCombatCharacter::OnAbilitySystemInitialized()`
- L326: `void AHodgeCombatCharacter::OnAbilitySystemUninitialized()`
- L333: `void AHodgeCombatCharacter::PossessedBy(AController* NewController)`
- L357: `void AHodgeCombatCharacter::UnPossessed()`
- L383: `void AHodgeCombatCharacter::OnRep_Controller()`
- L392: `void AHodgeCombatCharacter::OnRep_PlayerState()`
- L401: `void AHodgeCombatCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)`
- L410: `void AHodgeCombatCharacter::InitializeGameplayTags()`
- L445: `void AHodgeCombatCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const`
- L455: `bool AHodgeCombatCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const`
- L467: `bool AHodgeCombatCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const`
- L479: `bool AHodgeCombatCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const`
- L491: `void AHodgeCombatCharacter::FellOutOfWorld(const class UDamageType& dmgType)`
- L498: `void AHodgeCombatCharacter::OnDeathStarted(AActor*)`
- L505: `void AHodgeCombatCharacter::OnDeathFinished(AActor*)`
- L512: `void AHodgeCombatCharacter::DisableMovementAndCollision()`
- L542: `void AHodgeCombatCharacter::DestroyDueToDeath()`
- L552: `void AHodgeCombatCharacter::UninitAndDestroy()`
- L580: `void AHodgeCombatCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)`
- L596: `void AHodgeCombatCharacter::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)`
- L625: `void AHodgeCombatCharacter::ToggleCrouch()`
- L644: `void AHodgeCombatCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)`
- L658: `void AHodgeCombatCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)`
- L672: `bool AHodgeCombatCharacter::CanJumpInternal_Implementation() const`
- L679: `void AHodgeCombatCharacter::OnRep_ReplicatedAcceleration()`
- L709: `void AHodgeCombatCharacter::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)`
- L722: `void AHodgeCombatCharacter::OnRep_MyTeamID(FGenericTeamId OldTeamID)`
- L729: `bool AHodgeCombatCharacter::UpdateSharedReplication()`
- L762: `void AHodgeCombatCharacter::FastSharedReplication_Implementation(const FSharedRepMovement& SharedRepMovement)`
- L804: `FSharedRepMovement::FSharedRepMovement()`
- L811: `bool FSharedRepMovement::FillForCharacter(ACharacter* Character)`
- L858: `bool FSharedRepMovement::Equals(const FSharedRepMovement& Other, ACharacter* Character) const`
- L901: `bool FSharedRepMovement::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)`

## HodgeEnemyCharacter.cpp

敌人移动与自动 AI 控制配置，尚未完成敌人 ASC 初始化。

源码：[Source/Hodgepodge/Private/Character/HodgeEnemyCharacter.cpp](../../../Source/Hodgepodge/Private/Character/HodgeEnemyCharacter.cpp)

项目内直接 include（不是运行调用关系）：[Character/HodgeEnemyCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeEnemyCharacter.h)

定义候选（多行签名仅展示首行）：

- L10: `AHodgeEnemyCharacter::AHodgeEnemyCharacter(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)`
- L41: `void AHodgeEnemyCharacter::BeginPlay()`
- L46: `void AHodgeEnemyCharacter::PossessedBy(AController* NewController)`
- L53: `void AHodgeEnemyCharacter::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)`

## HodgeHeroCharacter.cpp

构造挂载 HeroComponent；仍保留 PossessedBy/OnRep_PlayerState 直接初始化 ASC 的旧路径。

源码：[Source/Hodgepodge/Private/Character/HodgeHeroCharacter.cpp](../../../Source/Hodgepodge/Private/Character/HodgeHeroCharacter.cpp)

项目内直接 include（不是运行调用关系）：[Character/HodgeHeroCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeHeroCharacter.h)、[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)

定义候选（多行签名仅展示首行）：

- L23: `AHodgeHeroCharacter::AHodgeHeroCharacter(const FObjectInitializer& ObjectInitializer)`
- L39: `void AHodgeHeroCharacter::PossessedBy(AController* NewController)`
- L60: `void AHodgeHeroCharacter::OnRep_PlayerState()`

## HodgeCharacterBase.h

原生 Character 基础、替换移动组件、Receiver 生命周期。EndPlay 事件配对需修正。

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

PawnExtension、相机、ASC 查询、移动标签、复制与死亡占位逻辑。

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
  10: class UHodgePawnExtensionComponent;
  11: class UHodgeCameraComponent;
  12: class AHodgePlayerController;
  13: class AHodgePlayerState;
  21: USTRUCT()
  22: struct FHodgeReplicatedAcceleration
  23: {
  24: 	GENERATED_BODY()
  27: 	UPROPERTY()
  28: 	uint8 AccelXYRadians = 0;
  31: 	UPROPERTY()
  32: 	uint8 AccelXYMagnitude = 0;
  35: 	UPROPERTY()
  36: 	int8 AccelZ = 0;
  37: };
  44: USTRUCT()
  45: struct FSharedRepMovement
  46: {
  47: 	GENERATED_BODY()
  50: 	FSharedRepMovement();
  53: 	bool FillForCharacter(ACharacter* Character);
  56: 	bool Equals(const FSharedRepMovement& Other, ACharacter* Character) const;
  59: 	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
  62: 	UPROPERTY(Transient)
  63: 	FRepMovement RepMovement;
  66: 	UPROPERTY(Transient)
  67: 	float RepTimeStamp = 0.0f;
  70: 	UPROPERTY(Transient)
  71: 	uint8 RepMovementMode = 0;
  74: 	UPROPERTY(Transient)
  75: 	bool bProxyIsJumpForceApplied = false;
  78: 	UPROPERTY(Transient)
  79: 	bool bIsCrouched = false;
  80: };
  83: template <>
  84: struct TStructOpsTypeTraits<FSharedRepMovement> : public TStructOpsTypeTraitsBase2<FSharedRepMovement>
  85: {
  86: 	enum
  87: 	{
  89: 		WithNetSerializer = true,
  92: 		WithNetSharedSerialization = true,
  93: 	};
  94: };
 110: UCLASS()
 111: class HODGEPODGE_API AHodgeCombatCharacter : public AHodgeCharacterBase,
 112:                                              public IAbilitySystemInterface,
 113:                                              public IGameplayCueInterface,
 114:                                              public IGameplayTagAssetInterface
 115: {
 116: 	GENERATED_BODY()
 118: public:
 120: 	AHodgeCombatCharacter(const FObjectInitializer& ObjectInitializer);
 123: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
 124: 	AHodgePlayerController* GetHodgePlayerController() const;
 127: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
 128: 	AHodgePlayerState* GetHodgePlayerState() const;
 131: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
 132: 	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponent() const;
 135: 	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
 138: 	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
 141: 	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
 144: 	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
 147: 	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
 150: 	void ToggleCrouch();
 155: 	virtual void PreInitializeComponents() override;
 158: 	virtual void BeginPlay() override;
 161: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
 164: 	virtual void Reset() override;
 167: 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
 170: 	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
 177: 	virtual void NotifyControllerChanged() override;
 182: 	UFUNCTION(NetMulticast, unreliable)
 183: 	void FastSharedReplication(const FSharedRepMovement& SharedRepMovement);
 186: 	FSharedRepMovement LastSharedReplication;
 189: 	virtual bool UpdateSharedReplication();
 191: protected:
 193: 	virtual void OnAbilitySystemInitialized();
 196: 	virtual void OnAbilitySystemUninitialized();
 199: 	virtual void PossessedBy(AController* NewController) override;
 202: 	virtual void UnPossessed() override;
 205: 	virtual void OnRep_Controller() override;
 208: 	virtual void OnRep_PlayerState() override;
 211: 	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
 214: 	void InitializeGameplayTags();
 217: 	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;
 220: 	UFUNCTION()
 221: 	virtual void OnDeathStarted(AActor* OwningActor);
 224: 	UFUNCTION()
 225: 	virtual void OnDeathFinished(AActor* OwningActor);
 228: 	void DisableMovementAndCollision();
 231: 	void DestroyDueToDeath();
 234: 	void UninitAndDestroy();
 237: 	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnDeathFinished"))
 238: 	void K2_OnDeathFinished();
 241: 	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
 244: 	void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);
 247: 	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
 250: 	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
 253: 	virtual bool CanJumpInternal_Implementation() const;
 255: private:
 257: 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hodge|Character", Meta = (AllowPrivateAccess = "true"))
 258: 	TObjectPtr<UHodgePawnExtensionComponent> PawnExtComponent;
 265: 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hodge|Character", Meta = (AllowPrivateAccess = "true"))
 266: 	TObjectPtr<UHodgeCameraComponent> CameraComponent;
 269: 	UPROPERTY(Transient, ReplicatedUsing = OnRep_ReplicatedAcceleration)
 270: 	FHodgeReplicatedAcceleration ReplicatedAcceleration;
 273: 	UPROPERTY(ReplicatedUsing = OnRep_MyTeamID)
 274: 	FGenericTeamId MyTeamID;
 276: protected:
 278: 	virtual FGenericTeamId DetermineNewTeamAfterPossessionEnds(FGenericTeamId OldTeamID) const
 279: 	{
 281: 		return FGenericTeamId::NoTeam;
 282: 	}
 284: private:
 286: 	UFUNCTION()
 287: 	void OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);
 290: 	UFUNCTION()
 291: 	void OnRep_ReplicatedAcceleration();
 294: 	UFUNCTION()
 295: 	void OnRep_MyTeamID(FGenericTeamId OldTeamID);
 296: };
```

## HodgeEnemyCharacter.h

敌人移动与自动 AI 控制配置，尚未完成敌人 ASC 初始化。

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

构造挂载 HeroComponent；仍保留 PossessedBy/OnRep_PlayerState 直接初始化 ASC 的旧路径。

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
