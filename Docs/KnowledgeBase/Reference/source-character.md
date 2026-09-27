# Character 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeCharacterBase.cpp

原生 Character 基础、替换移动组件；Receiver 在 PreInit 注册、EndPlay 成对移除。

源码：[Source/Hodgepodge/Private/Character/HodgeCharacterBase.cpp](../../../Source/Hodgepodge/Private/Character/HodgeCharacterBase.cpp)

项目内直接 include（不是运行调用关系）：[Character/HodgeCharacterBase.h](../../../Source/Hodgepodge/Public/Character/HodgeCharacterBase.h)、[Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)

定义候选（多行签名仅展示首行）：

- L29: `AHodgeCharacterBase::AHodgeCharacterBase(const FObjectInitializer& ObjectInitializer)`
- L41: `void AHodgeCharacterBase::PreInitializeComponents()`
- L55: `void AHodgeCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L68: `void AHodgeCharacterBase::BeginPlay()`

## HodgeCombatCharacter.cpp

PawnExtension、相机、ASC 查询、移动标签、复制与死亡占位逻辑。

源码：[Source/Hodgepodge/Private/Character/HodgeCombatCharacter.cpp](../../../Source/Hodgepodge/Private/Character/HodgeCombatCharacter.cpp)

项目内直接 include（不是运行调用关系）：[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Camera/HodgeCameraComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraComponent.h)、[Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)、[Component/HodgeHealthComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHealthComponent.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Core/PlayerController/HodgePlayerController.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)

定义候选（多行签名仅展示首行）：

- L37: `AHodgeCombatCharacter::AHodgeCombatCharacter(const FObjectInitializer& ObjectInitializer)`
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
- L359: `void AHodgeCombatCharacter::UnPossessed()`
- L385: `void AHodgeCombatCharacter::OnRep_Controller()`
- L394: `void AHodgeCombatCharacter::OnRep_PlayerState()`
- L403: `void AHodgeCombatCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)`
- L414: `void AHodgeCombatCharacter::InitializeGameplayTags()`
- L449: `void AHodgeCombatCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const`
- L459: `bool AHodgeCombatCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const`
- L471: `bool AHodgeCombatCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const`
- L483: `bool AHodgeCombatCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const`
- L495: `void AHodgeCombatCharacter::FellOutOfWorld(const class UDamageType& dmgType)`
- L502: `void AHodgeCombatCharacter::OnDeathStarted(AActor*)`
- L509: `void AHodgeCombatCharacter::OnDeathFinished(AActor*)`
- L516: `void AHodgeCombatCharacter::DisableMovementAndCollision()`
- L546: `void AHodgeCombatCharacter::DestroyDueToDeath()`
- L556: `void AHodgeCombatCharacter::UninitAndDestroy()`
- L584: `void AHodgeCombatCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)`
- L600: `void AHodgeCombatCharacter::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)`
- L629: `void AHodgeCombatCharacter::ToggleCrouch()`
- L648: `void AHodgeCombatCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)`
- L662: `void AHodgeCombatCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)`
- L676: `bool AHodgeCombatCharacter::CanJumpInternal_Implementation() const`
- L683: `void AHodgeCombatCharacter::OnRep_ReplicatedAcceleration()`
- L713: `void AHodgeCombatCharacter::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)`
- L726: `void AHodgeCombatCharacter::OnRep_MyTeamID(FGenericTeamId OldTeamID)`
- L733: `bool AHodgeCombatCharacter::UpdateSharedReplication()`
- L766: `void AHodgeCombatCharacter::FastSharedReplication_Implementation(const FSharedRepMovement& SharedRepMovement)`
- L808: `FSharedRepMovement::FSharedRepMovement()`
- L815: `bool FSharedRepMovement::FillForCharacter(ACharacter* Character)`
- L862: `bool FSharedRepMovement::Equals(const FSharedRepMovement& Other, ACharacter* Character) const`
- L905: `bool FSharedRepMovement::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)`

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
  10: class UHodgeHealthComponent;
  11: class UHodgePawnExtensionComponent;
  12: class UHodgeCameraComponent;
  13: class AHodgePlayerController;
  14: class AHodgePlayerState;
  22: USTRUCT()
  23: struct FHodgeReplicatedAcceleration
  24: {
  25: 	GENERATED_BODY()
  28: 	UPROPERTY()
  29: 	uint8 AccelXYRadians = 0;
  32: 	UPROPERTY()
  33: 	uint8 AccelXYMagnitude = 0;
  36: 	UPROPERTY()
  37: 	int8 AccelZ = 0;
  38: };
  45: USTRUCT()
  46: struct FSharedRepMovement
  47: {
  48: 	GENERATED_BODY()
  51: 	FSharedRepMovement();
  54: 	bool FillForCharacter(ACharacter* Character);
  57: 	bool Equals(const FSharedRepMovement& Other, ACharacter* Character) const;
  60: 	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
  63: 	UPROPERTY(Transient)
  64: 	FRepMovement RepMovement;
  67: 	UPROPERTY(Transient)
  68: 	float RepTimeStamp = 0.0f;
  71: 	UPROPERTY(Transient)
  72: 	uint8 RepMovementMode = 0;
  75: 	UPROPERTY(Transient)
  76: 	bool bProxyIsJumpForceApplied = false;
  79: 	UPROPERTY(Transient)
  80: 	bool bIsCrouched = false;
  81: };
  84: template <>
  85: struct TStructOpsTypeTraits<FSharedRepMovement> : public TStructOpsTypeTraitsBase2<FSharedRepMovement>
  86: {
  87: 	enum
  88: 	{
  90: 		WithNetSerializer = true,
  93: 		WithNetSharedSerialization = true,
  94: 	};
  95: };
 111: UCLASS()
 112: class HODGEPODGE_API AHodgeCombatCharacter : public AHodgeCharacterBase,
 113:                                              public IAbilitySystemInterface,
 114:                                              public IGameplayCueInterface,
 115:                                              public IGameplayTagAssetInterface
 116: {
 117: 	GENERATED_BODY()
 119: public:
 121: 	AHodgeCombatCharacter(const FObjectInitializer& ObjectInitializer);
 124: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
 125: 	AHodgePlayerController* GetHodgePlayerController() const;
 128: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
 129: 	AHodgePlayerState* GetHodgePlayerState() const;
 132: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Character")
 133: 	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponent() const;
 136: 	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
 139: 	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
 142: 	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
 145: 	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
 148: 	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
 151: 	void ToggleCrouch();
 156: 	virtual void PreInitializeComponents() override;
 159: 	virtual void BeginPlay() override;
 162: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
 165: 	virtual void Reset() override;
 168: 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
 171: 	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
 178: 	virtual void NotifyControllerChanged() override;
 183: 	UFUNCTION(NetMulticast, unreliable)
 184: 	void FastSharedReplication(const FSharedRepMovement& SharedRepMovement);
 187: 	FSharedRepMovement LastSharedReplication;
 190: 	virtual bool UpdateSharedReplication();
 192: protected:
 194: 	virtual void OnAbilitySystemInitialized();
 197: 	virtual void OnAbilitySystemUninitialized();
 200: 	virtual void PossessedBy(AController* NewController) override;
 203: 	virtual void UnPossessed() override;
 206: 	virtual void OnRep_Controller() override;
 209: 	virtual void OnRep_PlayerState() override;
 212: 	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
 215: 	void InitializeGameplayTags();
 218: 	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;
 221: 	UFUNCTION()
 222: 	virtual void OnDeathStarted(AActor* OwningActor);
 225: 	UFUNCTION()
 226: 	virtual void OnDeathFinished(AActor* OwningActor);
 229: 	void DisableMovementAndCollision();
 232: 	void DestroyDueToDeath();
 235: 	void UninitAndDestroy();
 238: 	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnDeathFinished"))
 239: 	void K2_OnDeathFinished();
 242: 	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
 245: 	void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);
 248: 	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
 251: 	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
 254: 	virtual bool CanJumpInternal_Implementation() const;
 256: private:
 258: 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hodge|Character", Meta = (AllowPrivateAccess = "true"))
 259: 	TObjectPtr<UHodgePawnExtensionComponent> PawnExtComponent;
 262: 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hodge|Character", Meta = (AllowPrivateAccess = "true"))
 263: 	TObjectPtr<UHodgeHealthComponent> HealthComponent;
 266: 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hodge|Character", Meta = (AllowPrivateAccess = "true"))
 267: 	TObjectPtr<UHodgeCameraComponent> CameraComponent;
 270: 	UPROPERTY(Transient, ReplicatedUsing = OnRep_ReplicatedAcceleration)
 271: 	FHodgeReplicatedAcceleration ReplicatedAcceleration;
 274: 	UPROPERTY(ReplicatedUsing = OnRep_MyTeamID)
 275: 	FGenericTeamId MyTeamID;
 277: protected:
 279: 	virtual FGenericTeamId DetermineNewTeamAfterPossessionEnds(FGenericTeamId OldTeamID) const
 280: 	{
 282: 		return FGenericTeamId::NoTeam;
 283: 	}
 285: private:
 287: 	UFUNCTION()
 288: 	void OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);
 291: 	UFUNCTION()
 292: 	void OnRep_ReplicatedAcceleration();
 295: 	UFUNCTION()
 296: 	void OnRep_MyTeamID(FGenericTeamId OldTeamID);
 297: };
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
