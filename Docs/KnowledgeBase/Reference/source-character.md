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

项目内直接 include（不是运行调用关系）：[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Camera/HodgeCameraComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraComponent.h)、[Component/HodgeCharacterMovementComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterMovementComponent.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Core/PlayerController/HodgePlayerControllerBase.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerControllerBase.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)

定义候选（多行签名仅展示首行）：

- L36: `AHodgeCombatCharacter::AHodgeCombatCharacter(const FObjectInitializer& ObjectInitializer)`
- L154: `void AHodgeCombatCharacter::PreInitializeComponents()`
- L161: `void AHodgeCombatCharacter::BeginPlay()`
- L184: `void AHodgeCombatCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L207: `void AHodgeCombatCharacter::Reset()`
- L220: `void AHodgeCombatCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L232: `void AHodgeCombatCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)`
- L259: `void AHodgeCombatCharacter::NotifyControllerChanged()`
- L279: `AHodgePlayerControllerBase* AHodgeCombatCharacter::GetHodgePlayerController() const`
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

定义候选（多行签名仅展示首行）：


## HodgeHeroCharacter.cpp

玩家角色，当前保留 PossessedBy / OnRep_PlayerState 直接初始化 ASC 的旧入口。

源码：[Source/Hodgepodge/Private/Character/HodgeHeroCharacter.cpp](../../../Source/Hodgepodge/Private/Character/HodgeHeroCharacter.cpp)

项目内直接 include（不是运行调用关系）：[Character/HodgeHeroCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeHeroCharacter.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)

定义候选（多行签名仅展示首行）：

- L22: `AHodgeHeroCharacter::AHodgeHeroCharacter(const FObjectInitializer& ObjectInitializer)`
- L37: `void AHodgeHeroCharacter::PossessedBy(AController* NewController)`
- L58: `void AHodgeHeroCharacter::OnRep_PlayerState()`

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
  12: class AHodgePlayerControllerBase;
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
 124: 	AHodgePlayerControllerBase* GetHodgePlayerController() const;
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

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: ��/ /   1 1 1 N\q\�Nxeg��
   2:  
   3:  
   4:  
   5:  # p r a g m a   o n c e 
   6:  
   7:  
   8:  
   9:  # i n c l u d e   " C o r e M i n i m a l . h " 
  10:  
  11:  # i n c l u d e   " C h a r a c t e r / H o d g e C o m b a t C h a r a c t e r . h " 
  12:  
  13:  # i n c l u d e   " H o d g e E n e m y C h a r a c t e r . g e n e r a t e d . h " 
  14:  
  15:  
  16:  
  17:  / * * 
  18:  
  19:    *   
  20:  
  21:    * / 
  22:  
  23:  U C L A S S ( ) 
  24:  
  25:  c l a s s   H O D G E P O D G E _ A P I   A H o d g e E n e m y C h a r a c t e r   :   p u b l i c   A H o d g e C o m b a t C h a r a c t e r 
  26:  
  27:  { 
  28:  
  29:  	 G E N E R A T E D _ B O D Y ( ) 
  30:  
  31:  
  32:  
  33:  p u b l i c : 
  34:  
  35:  	 e x p l i c i t   A H o d g e E n e m y C h a r a c t e r ( c o n s t   F O b j e c t I n i t i a l i z e r &   O b j e c t I n i t i a l i z e r ) ; 
  36:  
  37:  
  38:  
  39:  p r o t e c t e d : 
  40:  
  41:  	 / /   8nb _�Y�e�v
  42: R�YS
  43:  
  44:  	 v i r t u a l   v o i d   B e g i n P l a y ( )   o v e r r i d e ; 
  45:  
  46:  
  47:  
  48:  	 / / ~ B e g i n   A P a w n   �c�S͑�Q
  49:  
  50:  	 / /   S_�c6RhV�b	gdk҉r��e�(u
  51:  
  52:  	 v i r t u a l   v o i d   P o s s e s s e d B y ( A C o n t r o l l e r *   N e w C o n t r o l l e r )   o v e r r i d e ; 
  53:  
  54:  	 / / ~ E n d   A P a w n   �c�S
  55:  
  56:  
  57:  
  58:  # i f   W I T H _ E D I T O R 
  59:  
  60:  	 / / ~ B e g i n   U O b j e c t   �c�S͑�Q
  61:  
  62:  	 / /   S_^\'`(W��hV-N9e�S�e�(u
  63:  
  64:  	 v i r t u a l   v o i d   P o s t E d i t C h a n g e P r o p e r t y ( s t r u c t   F P r o p e r t y C h a n g e d E v e n t &   P r o p e r t y C h a n g e d E v e n t )   o v e r r i d e ; 
  65:  
  66:  	 / / ~ E n d   U O b j e c t   �c�S
  67:  
  68:  # e n d i f 
  69:  
  70:  
  71:  
  72:  	 / /   	 / /   �]Kb�x�d�v�(u�Nяb;e�Q�hKm	�
  73:  
  74:  	 / /   	 U P R O P E R T Y ( V i s i b l e A n y w h e r e ,   B l u e p r i n t R e a d O n l y ,   C a t e g o r y = " C o m b a t " ) 
  75:  
  76:  	 / /   	 U B o x C o m p o n e n t *   L e f t H a n d C o l l i s i o n B o x ; 
  77:  
  78:  	 / / 
  79:  
  80:  	 / /   	 / /   �]Kb�x�d�vD��R�v����
  81: T�y
  82:  
  83:  	 / /   	 U P R O P E R T Y ( E d i t D e f a u l t s O n l y ,   B l u e p r i n t R e a d O n l y ,   C a t e g o r y = " C o m b a t " ) 
  84:  
  85:  	 / /   	 F N a m e   L e f t H a n d C o l l i s i o n A t t a c h B o n e N a m e ; 
  86:  
  87:  	 / / 
  88:  
  89:  	 / /   	 / /   �SKb�x�d�v�(u�Nяb;e�Q�hKm	�
  90:  
  91:  	 / /   	 U P R O P E R T Y ( V i s i b l e A n y w h e r e ,   B l u e p r i n t R e a d O n l y ,   C a t e g o r y = " C o m b a t " ) 
  92:  
  93:  	 / /   	 U B o x C o m p o n e n t *   R i g h t H a n d C o l l i s i o n B o x ; 
  94:  
  95:  	 / / 
  96:  
  97:  	 / /   	 / /   �SKb�x�d�vD��R�v����
  98: T�y
  99:  
 100:  	 / /   	 U P R O P E R T Y ( E d i t D e f a u l t s O n l y ,   B l u e p r i n t R e a d O n l y ,   C a t e g o r y = " C o m b a t " ) 
 101:  
 102:  	 / /   	 F N a m e   R i g h t H a n d C o l l i s i o n A t t a c h B o n e N a m e ; 
 103:  
 104:  	 / / 
 105:  
 106:  	 / /   	 / /   Le�N@�ag�c�N�~�N�>f:y(W҉r�
 107: N�e	�
 108:  
 109:  	 / /   	 U P R O P E R T Y ( V i s i b l e A n y w h e r e ,   B l u e p r i n t R e a d O n l y ,   C a t e g o r y = " U I " ) 
 110:  
 111:  	 / /   	 U W i d g e t C o m p o n e n t *   E n e m y H e a l t h W i d g e t C o m p o n e n t ; 
 112:  
 113:  	 / / 
 114:  
 115:  	 / /   	 / /   �x�d�v͑�S�N�NYt�Qpe
 116:  
 117:  	 / /   	 U F U N C T I O N ( ) 
 118:  
 119:  	 / /   	 v i r t u a l   v o i d   O n C o m p o n e n t B e g i n O v e r l a p ( 
 120:  
 121:  	 / /   	 	 U P r i m i t i v e C o m p o n e n t *   O v e r l a p p e d C o m p o n e n t , 
 122:  
 123:  	 / /   	 	 A A c t o r *   O t h e r A c t o r , 
 124:  
 125:  	 / /   	 	 U P r i m i t i v e C o m p o n e n t *   O t h e r C o m p , 
 126:  
 127:  	 / /   	 	 i n t 3 2   O t h e r B o d y I n d e x , 
 128:  
 129:  	 / /   	 	 b o o l   b F r o m S w e e p , 
 130:  
 131:  	 / /   	 	 c o n s t   F H i t R e s u l t &   S w e e p R e s u l t 
 132:  
 133:  	 / /   	 ) ; 
 134:  
 135:  	 / / 
 136:  
 137:  	 / /   p r i v a t e : 
 138:  
 139:  	 / /   	 / /   
 140: R�YSLe�N
 141: R�Ypenc�_ek�R}�v^�^(u	�
 142:  
 143:  	 / /   	 v o i d   I n i t E n e m y S t a r t U p D a t a ( ) ; 
 144:  
 145:  	 / / 
 146:  
 147:  	 / /   p u b l i c : 
 148:  
 149:  	 / /   	 / /   ���SLe�Nb�e�~�N��QT��Qpe	�
 150:  
 151:  	 / /   	 F O R C E I N L I N E   U E n e m y C o m b a t C o m p o n e n t *   G e t E n e m y C o m b a t C o m p o n e n t ( )   c o n s t   {   r e t u r n   E n e m y C o m b a t C o m p o n e n t ;   } 
 152:  
 153:  	 / /   	 / /   ���S�]Kb�x�d�v
 154:  
 155:  	 / /   	 F O R C E I N L I N E   U B o x C o m p o n e n t *   G e t L e f t H a n d C o l l i s i o n B o x ( )   c o n s t   {   r e t u r n   L e f t H a n d C o l l i s i o n B o x ;   } 
 156:  
 157:  	 / /   	 / /   ���S�SKb�x�d�v
 158:  
 159:  	 / /   	 F O R C E I N L I N E   U B o x C o m p o n e n t *   G e t R i g h t H a n d C o l l i s i o n B o x ( )   c o n s t {   r e t u r n   R i g h t H a n d C o l l i s i o n B o x ; } 
 160:  
 161:  } ; 
 162:  
 163:  
```

## HodgeHeroCharacter.h

玩家角色，当前保留 PossessedBy / OnRep_PlayerState 直接初始化 ASC 的旧入口。

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
  72: };
```
