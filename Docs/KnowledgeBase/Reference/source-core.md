# Core 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeGameInstanceBase.cpp

注册 Init State 顺序、主控制器访问和全局生命周期扩展。

源码：[Source/Hodgepodge/Private/Core/GameInstance/HodgeGameInstanceBase.cpp](../../../Source/Hodgepodge/Private/Core/GameInstance/HodgeGameInstanceBase.cpp)

定义候选（多行签名仅展示首行）：


## HodgeGameModeBase.cpp

服务器玩法选择、等待 Experience、取得 PawnData、默认 Pawn 生成与重生。数据注入仍注释。

源码：[Source/Hodgepodge/Private/Core/GameMode/HodgeGameModeBase.cpp](../../../Source/Hodgepodge/Private/Core/GameMode/HodgeGameModeBase.cpp)

项目内直接 include（不是运行调用关系）：[Core/GameMode/HodgeGameModeBase.h](../../../Source/Hodgepodge/Public/Core/GameMode/HodgeGameModeBase.h)、[Character/HodgeCharacterBase.h](../../../Source/Hodgepodge/Public/Character/HodgeCharacterBase.h)、[Component/HodgeExperienceManagerComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h)、[Core/GameState/HodgeGameState.h](../../../Source/Hodgepodge/Public/Core/GameState/HodgeGameState.h)、[Core/GameState/HodgeGameStateBase.h](../../../Source/Hodgepodge/Public/Core/GameState/HodgeGameStateBase.h)、[Core/HUD/HodgeHUDBase.h](../../../Source/Hodgepodge/Public/Core/HUD/HodgeHUDBase.h)、[Core/PlayerController/HodgePlayerControllerBase.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerControllerBase.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)、[Data/HodgeAssetManager.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManager.h)、[Data/HodgeExperienceDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceDefinition.h)

定义候选（多行签名仅展示首行）：

- L20: `AHodgeGameModeBase::AHodgeGameModeBase(const FObjectInitializer& ObjectInitializer)`
- L45: `const UHodgePawnData* AHodgeGameModeBase::GetPawnDataForController(const AController* InController) const`
- L86: `void AHodgeGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)`
- L95: `void AHodgeGameModeBase::HandleMatchAssignmentIfNotExpectingOne()`
- L192: `bool AHodgeGameModeBase::TryDedicatedServerLogin()`
- L336: `void AHodgeGameModeBase::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource)`
- L360: `void AHodgeGameModeBase::OnExperienceLoaded(const UHodgeExperienceDefinition* CurrentExperience)`
- L381: `bool AHodgeGameModeBase::IsExperienceLoaded() const`
- L395: `UClass* AHodgeGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)`
- L411: `APawn* AHodgeGameModeBase::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer,`
- L468: `bool AHodgeGameModeBase::ShouldSpawnAtStartSpot(AController* Player)`
- L474: `void AHodgeGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)`
- L484: `AActor* AHodgeGameModeBase::ChoosePlayerStart_Implementation(AController* Player)`
- L497: `void AHodgeGameModeBase::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)`
- L510: `bool AHodgeGameModeBase::PlayerCanRestart_Implementation(APlayerController* Player)`
- L516: `bool AHodgeGameModeBase::ControllerCanRestart(AController* Controller)`
- L545: `void AHodgeGameModeBase::InitGameState()`
- L560: `void AHodgeGameModeBase::GenericPlayerInitialization(AController* NewPlayer)`
- L569: `void AHodgeGameModeBase::RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset)`
- L590: `bool AHodgeGameModeBase::UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage)`
- L596: `void AHodgeGameModeBase::FailedToRestartPlayer(AController* NewPlayer)`

## HodgeGameState.cpp

创建 ExperienceManager 和世界状态 ASC，处理游戏状态复制/扩展。

源码：[Source/Hodgepodge/Private/Core/GameState/HodgeGameState.cpp](../../../Source/Hodgepodge/Private/Core/GameState/HodgeGameState.cpp)

项目内直接 include（不是运行调用关系）：[Core/GameState/HodgeGameState.h](../../../Source/Hodgepodge/Public/Core/GameState/HodgeGameState.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[Component/HodgeExperienceManagerComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h)、[Core/PlayState/HodgePlayerStateBase.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerStateBase.h)

定义候选（多行签名仅展示首行）：

- L25: `AHodgeGameState::AHodgeGameState(const FObjectInitializer& ObjectInitializer)`
- L50: `void AHodgeGameState::PreInitializeComponents()`
- L56: `void AHodgeGameState::PostInitializeComponents()`
- L67: `UAbilitySystemComponent* AHodgeGameState::GetAbilitySystemComponent() const`
- L73: `void AHodgeGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L79: `void AHodgeGameState::AddPlayerState(APlayerState* PlayerState)`
- L85: `void AHodgeGameState::RemovePlayerState(APlayerState* PlayerState)`
- L92: `void AHodgeGameState::SeamlessTravelTransitionCheckpoint(bool bToTransitionMap)`
- L108: `void AHodgeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L120: `void AHodgeGameState::Tick(float DeltaSeconds)`
- L147: `float AHodgeGameState::GetServerFPS() const`
- L153: `void AHodgeGameState::SetRecorderPlayerState(APlayerState* NewPlayerState)`
- L172: `APlayerState* AHodgeGameState::GetRecorderPlayerState() const`
- L178: `void AHodgeGameState::OnRep_RecorderPlayerState()`

## HodgeGameStateBase.cpp

GameState 基础扩展生命周期。

源码：[Source/Hodgepodge/Private/Core/GameState/HodgeGameStateBase.cpp](../../../Source/Hodgepodge/Private/Core/GameState/HodgeGameStateBase.cpp)

定义候选（多行签名仅展示首行）：


## HodgeHUDBase.cpp

项目 HUD 基类扩展入口，不代表 CommonUI 已完成。

源码：[Source/Hodgepodge/Private/Core/HUD/HodgeHUDBase.cpp](../../../Source/Hodgepodge/Private/Core/HUD/HodgeHUDBase.cpp)

定义候选（多行签名仅展示首行）：


## HodgeLocalPlayerBase.cpp

本地玩家对象及控制器、PlayerState、Pawn 就绪事件桥。

源码：[Source/Hodgepodge/Private/Core/LocalPlayer/HodgeLocalPlayerBase.cpp](../../../Source/Hodgepodge/Private/Core/LocalPlayer/HodgeLocalPlayerBase.cpp)

定义候选（多行签名仅展示首行）：


## HodgePlayerControllerBase.cpp

把控制器和 Pawn 生命周期桥接到 LocalPlayer 委托。技能输入每帧消费尚未接入。

源码：[Source/Hodgepodge/Private/Core/PlayerController/HodgePlayerControllerBase.cpp](../../../Source/Hodgepodge/Private/Core/PlayerController/HodgePlayerControllerBase.cpp)

项目内直接 include（不是运行调用关系）：[Core/PlayerController/HodgePlayerControllerBase.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerControllerBase.h)、[Core/LocalPlayer/HodgeLocalPlayerBase.h](../../../Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h)

定义候选（多行签名仅展示首行）：

- L22: `AHodgePlayerControllerBase::AHodgePlayerControllerBase()`
- L49: `void AHodgePlayerControllerBase::ReceivedPlayer()`
- L80: `void AHodgePlayerControllerBase::SetPawn(APawn* InPawn)`
- L115: `void AHodgePlayerControllerBase::OnPossess(class APawn* APawn)`
- L146: `void AHodgePlayerControllerBase::OnUnPossess()`
- L184: `void AHodgePlayerControllerBase::OnRep_PlayerState()`

## HodgePlayerState.cpp

玩家 ASC、HealthSet、PawnData、阵营/标签栈等持有者。基础能力授予循环仍停用。

源码：[Source/Hodgepodge/Private/Core/PlayState/HodgePlayerState.cpp](../../../Source/Hodgepodge/Private/Core/PlayState/HodgePlayerState.cpp)

项目内直接 include（不是运行调用关系）：[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[Component/HodgeExperienceManagerComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h)、[Core/GameMode/HodgeGameModeBase.h](../../../Source/Hodgepodge/Public/Core/GameMode/HodgeGameModeBase.h)、[Core/PlayerController/HodgePlayerControllerBase.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerControllerBase.h)

定义候选（多行签名仅展示首行）：

- L16: `const FName AHodgePlayerState::NAME_HodgeAbilityReady("HodgeAbilitiesReady");`
- L18: `AHodgePlayerState::AHodgePlayerState(const FObjectInitializer& ObjectInitializer)`
- L35: `AHodgePlayerControllerBase* AHodgePlayerState::GetHodgePlayerController() const`
- L40: `UAbilitySystemComponent* AHodgePlayerState::GetAbilitySystemComponent() const`
- L45: `void AHodgePlayerState::SetPawnData(const UHodgePawnData* InPawnData)`
- L78: `void AHodgePlayerState::PreInitializeComponents()`
- L97: `void AHodgePlayerState::PostInitializeComponents()`
- L102: `void AHodgePlayerState::Reset()`
- L107: `void AHodgePlayerState::ClientInitialize(AController* C)`
- L117: `void AHodgePlayerState::CopyProperties(APlayerState* PlayerState)`
- L123: `void AHodgePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L141: `void AHodgePlayerState::OnDeactivated()`
- L166: `void AHodgePlayerState::OnReactivated()`
- L175: `void AHodgePlayerState::SetPlayerConnectionType(EHodgePlayerConnectionType NewType)`
- L181: `void AHodgePlayerState::SetSquadID(int32 NewSquadID)`
- L191: `void AHodgePlayerState::AddStatTagStack(FGameplayTag Tag, int32 StackCount)`
- L196: `void AHodgePlayerState::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)`
- L201: `int32 AHodgePlayerState::GetStatTagStackCount(FGameplayTag Tag) const`
- L206: `bool AHodgePlayerState::HasStatTag(FGameplayTag Tag) const`
- L211: `FRotator AHodgePlayerState::GetReplicatedViewRotation() const`
- L216: `void AHodgePlayerState::SetReplicatedViewRotation(const FRotator& NewRotation)`
- L225: `void AHodgePlayerState::OnExperienceLoaded(const UHodgeExperienceDefinition* CurrentExperience)`
- L243: `void AHodgePlayerState::OnRep_PawnData()`
- L247: `void AHodgePlayerState::OnRep_MyTeamID(FGenericTeamId OldTeamID)`
- L252: `void AHodgePlayerState::OnRep_MySquadID()`

## HodgePlayerStateBase.cpp

PlayerState ModularGameplay Receiver 注册、注销及组件 Reset/CopyProperties。

源码：[Source/Hodgepodge/Private/Core/PlayState/HodgePlayerStateBase.cpp](../../../Source/Hodgepodge/Private/Core/PlayState/HodgePlayerStateBase.cpp)

项目内直接 include（不是运行调用关系）：[Core/PlayState/HodgePlayerStateBase.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerStateBase.h)

定义候选（多行签名仅展示首行）：

- L10: `void AHodgePlayerStateBase::PreInitializeComponents()`
- L19: `void AHodgePlayerStateBase::BeginPlay()`
- L29: `void AHodgePlayerStateBase::EndPlay(const EEndPlayReason::Type EndPlayReason)`
- L38: `void AHodgePlayerStateBase::Reset()`
- L55: `void AHodgePlayerStateBase::CopyProperties(APlayerState* PlayerState)`

## HodgeGameInstanceBase.h

注册 Init State 顺序、主控制器访问和全局生命周期扩展。

源码：[Source/Hodgepodge/Public/Core/GameInstance/HodgeGameInstanceBase.h](../../../Source/Hodgepodge/Public/Core/GameInstance/HodgeGameInstanceBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: ��/ * * 
   2:  
   3:  *   @ f i l e   H o d g e G a m e I n s t a n c e B a s e . h 
   4:  
   5:    *   @ b r i e f   U H o d g e G a m e I n s t a n c e B a s e   {|�v4Y�e�N
   6:  
   7:    * 
   8:  
   9:    *   8nb�[�O�W{|
  10: ��#��{t�sQaScENX[(W�vhQ@\8nb�r`0
  11:  
  12:    * / 
  13:  
  14:  
  15:  
  16:  # p r a g m a   o n c e 
  17:  
  18:  
  19:  
  20:  # i n c l u d e   " C o r e M i n i m a l . h " 
  21:  
  22:  # i n c l u d e   " E n g i n e / G a m e I n s t a n c e . h " 
  23:  
  24:  # i n c l u d e   " H o d g e G a m e I n s t a n c e B a s e . g e n e r a t e d . h " 
  25:  
  26:  
  27:  
  28:  c l a s s   A H o d g e P l a y e r C o n t r o l l e r B a s e ; 
  29:  
  30:  
  31:  
  32:  / * * 
  33:  
  34:    *   @ b r i e f   H o d g e p o d g e   Fh�g�v8nb�[�O�W{|
  35:  
  36:    * 
  37:  
  38:    *   �~b�  U G a m e I n s t a n c e 
  39: ��#��{t8nbu}ThTg�Qc�~X[(W�vhQ@\penc�T�|�~0
  40:  
  41:    *   G a m e I n s t a n c e   
  42: NO�V:NsQaSRbc
  43: � ��k
  44: ��TX[>e�sQaS�vpenc�ThQ@\�|�~0
  45:  
  46:    * / 
  47:  
  48:  U C L A S S ( ) 
  49:  
  50:  c l a s s   H O D G E P O D G E _ A P I   U H o d g e G a m e I n s t a n c e B a s e   :   p u b l i c   U G a m e I n s t a n c e 
  51:  
  52:  { 
  53:  
  54:  	 G E N E R A T E D _ B O D Y ( ) 
  55:  
  56:  
  57:  
  58:  p u b l i c : 
  59:  
  60:  	 / /   �g �8nb�[�Ov^ۏL��W@x
  61: R�YS0
  62:  
  63:  	 U H o d g e G a m e I n s t a n c e B a s e ( c o n s t   F O b j e c t I n i t i a l i z e r &   O b j e c t I n i t i a l i z e r ) ; 
  64:  
  65:  
  66:  
  67:  	 / /   ���SS_MR8nb�[�O�[�^�v;N�s�[  P l a y e r C o n t r o l l e r 0
  68:  
  69:  	 A H o d g e P l a y e r C o n t r o l l e r B a s e *   G e t P r i m a r y P l a y e r C o n t r o l l e r ( )   c o n s t ; 
  70:  
  71:  
  72:  
  73:  p r o t e c t e d : 
  74:  
  75:  	 / /   8nb�[�O
  76: R�YS�e�(u
  77: �(u�N
  78: R�YShQ@\8nb�|�~0
  79:  
  80:  	 v i r t u a l   v o i d   I n i t ( )   o v e r r i d e ; 
  81:  
  82:  
  83:  
  84:  	 / /   8nb�[�O ��kMR�(u
  85: �(u�NnthQ@\8nbD��n0
  86:  
  87:  	 v i r t u a l   v o i d   S h u t d o w n ( )   o v e r r i d e ; 
  88:  
  89:  
  90:  
  91:  	 / /   (u�NKmՋ  U E   �R�[�R���v�Ջ�[��
  92: �ck_y��v-N
  93: N��O(ulxx�[��0
  94:  
  95:  	 T A r r a y < u i n t 8 >   D e b u g T e s t E n c r y p t i o n K e y ; 
  96:  
  97:  } ; 
```

## HodgeGameModeBase.h

服务器玩法选择、等待 Experience、取得 PawnData、默认 Pawn 生成与重生。数据注入仍注释。

源码：[Source/Hodgepodge/Public/Core/GameMode/HodgeGameModeBase.h](../../../Source/Hodgepodge/Public/Core/GameMode/HodgeGameModeBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: ��/ * * 
   2:  
   3:    *   @ f i l e   H o d g e G a m e M o d e B a s e . h 
   4:  
   5:    *   @ b r i e f   A H o d g e G a m e M o d e B a s e   {|�v4Y�e�N
   6:  
   7:    * 
   8:  
   9:    *   8nb!j_�W{|
  10: �8nbAm
  11: z�TĉR�v�c6RhV
  12: ���n؞���v8nbFh�g{|v^�c�O
  13: R�YSibU\�p0
  14:  
  15:    * / 
  16:  
  17:  
  18:  
  19:  # p r a g m a   o n c e 
  20:  
  21:  
  22:  
  23:  # i n c l u d e   " C o r e M i n i m a l . h " 
  24:  
  25:  # i n c l u d e   " G a m e F r a m e w o r k / G a m e M o d e B a s e . h " 
  26:  
  27:  # i n c l u d e   " H o d g e G a m e M o d e B a s e . g e n e r a t e d . h " 
  28:  
  29:  
  30:  
  31:  c l a s s   U H o d g e E x p e r i e n c e D e f i n i t i o n ; 
  32:  
  33:  c l a s s   U H o d g e P a w n D a t a ; 
  34:  
  35:  
  36:  
  37:  / * * 
  38:  
  39:    *   @ b r i e f   �s�[{vU_�[b�N�N
  40: �(W�s�[b  B o t   �ReQ8nb�N�S�e
  41: / ^��e
  42: 0W�VRbc�[bT��S0
  43:  
  44:    *   @ n o t e   dk�N�N��S�e
  45: ��s�[�]�~�[b�W@x
  46: R�YS0
  47:  
  48:    * / 
  49:  
  50:  D E C L A R E _ M U L T I C A S T _ D E L E G A T E _ T w o P a r a m s ( F O n H o d g e G a m e M o d e P l a y e r I n i t i a l i z e d ,   A G a m e M o d e B a s e *   / * G a m e M o d e * / , 
  51:  
  52:                                                                            A C o n t r o l l e r *   / * N e w P l a y e r * / ) ; 
  53:  
  54:  
  55:  
  56:  U C L A S S ( ) 
  57:  
  58:  c l a s s   H O D G E P O D G E _ A P I   A H o d g e G a m e M o d e B a s e   :   p u b l i c   A G a m e M o d e B a s e 
  59:  
  60:  { 
  61:  
  62:  	 G E N E R A T E D _ B O D Y ( ) 
  63:  
  64:  
  65:  
  66:  p u b l i c : 
  67:  
  68:  	 / /   �g ��Qpe
  69: �
  70: R�YS  G a m e M o d e   �v؞��M�n0
  71:  
  72:  	 A H o d g e G a m e M o d e B a s e ( c o n s t   F O b j e c t I n i t i a l i z e r &   O b j e c t I n i t i a l i z e r   =   F O b j e c t I n i t i a l i z e r : : G e t ( ) ) ; 
  73:  
  74:  
  75:  
  76:  	 / /   9hnc  C o n t r o l l e r   ���S勩s�[�^�O(u�v  P a w n D a t a   M�n0
  77:  
  78:  	 U F U N C T I O N ( B l u e p r i n t C a l l a b l e ,   C a t e g o r y   =   " H o d g e | P a w n " ) 
  79:  
  80:  	 c o n s t   U H o d g e P a w n D a t a *   G e t P a w n D a t a F o r C o n t r o l l e r ( c o n s t   A C o n t r o l l e r *   I n C o n t r o l l e r )   c o n s t ; 
  81:  
  82:  
  83:  
  84:  	 / / ~ A G a m e M o d e B a s e   i n t e r f a c e 
  85:  
  86:  
  87:  
  88:  	 / /   
  89: R�YS  G a m e M o d e 
  90: ��S(u�N㉐g0W�V
  91: T�y�T/T�R�Spe
  92: �v^�QYS_MR8nb�v�W@xM�n0
  93:  
  94:  	 v i r t u a l   v o i d   I n i t G a m e ( c o n s t   F S t r i n g &   M a p N a m e ,   c o n s t   F S t r i n g &   O p t i o n s ,   F S t r i n g &   E r r o r M e s s a g e )   o v e r r i d e ; 
  95:  
  96:  
  97:  
  98:  	 / /   9hnc  C o n t r o l l e r   ���S؞��  P a w n   {|�W
  99: �(u�N�Q�[�s�[�^�ub�T N�y  P a w n 0
 100:  
 101:  	 v i r t u a l   U C l a s s *   G e t D e f a u l t P a w n C l a s s F o r C o n t r o l l e r _ I m p l e m e n t a t i o n ( A C o n t r o l l e r *   I n C o n t r o l l e r )   o v e r r i d e ; 
 102:  
 103:  
 104:  
 105:  	 / /   (Wc�[  T r a n s f o r m   MOnub�s�[؞��  P a w n 0
 106:  
 107:  	 v i r t u a l   A P a w n * 
 108:  
 109:  	 S p a w n D e f a u l t P a w n A t T r a n s f o r m _ I m p l e m e n t a t i o n ( A C o n t r o l l e r *   N e w P l a y e r ,   c o n s t   F T r a n s f o r m &   S p a w n T r a n s f o r m )   o v e r r i d e ; 
 110:  
 111:  
 112:  
 113:  	 / /   $R�e�s�[/f&T�^�ub(W  S t a r t S p o t   c�[�vMOn0
 114:  
 115:  	 v i r t u a l   b o o l   S h o u l d S p a w n A t S t a r t S p o t ( A C o n t r o l l e r *   P l a y e r )   o v e r r i d e ; 
 116:  
 117:  
 118:  
 119:  	 / /   Yt�e�s�[ۏeQ8nbT�v
 120: R�YSAm
 121: z0
 122:  
 123:  	 v i r t u a l   v o i d   H a n d l e S t a r t i n g N e w P l a y e r _ I m p l e m e n t a t i o n ( A P l a y e r C o n t r o l l e r *   N e w P l a y e r )   o v e r r i d e ; 
 124:  
 125:  
 126:  
 127:  	 / /   :Nc�[  C o n t r o l l e r   	��bT��v  P l a y e r S t a r t 0
 128:  
 129:  	 v i r t u a l   A A c t o r *   C h o o s e P l a y e r S t a r t _ I m p l e m e n t a t i o n ( A C o n t r o l l e r *   P l a y e r )   o v e r r i d e ; 
 130:  
 131:  
 132:  
 133:  	 / /   �[b�s�[͑uAm
 134: z
 135: �v^��n  P a w n   �v
 136: R�Y�el�0
 137:  
 138:  	 v i r t u a l   v o i d   F i n i s h R e s t a r t P l a y e r ( A C o n t r o l l e r *   N e w P l a y e r ,   c o n s t   F R o t a t o r &   S t a r t R o t a t i o n )   o v e r r i d e ; 
 139:  
 140:  
 141:  
 142:  	 / /   $R�ec�[�s�[S_MR/f&TAQ��gbL�͑u0
 143:  
 144:  	 v i r t u a l   b o o l   P l a y e r C a n R e s t a r t _ I m p l e m e n t a t i o n ( A P l a y e r C o n t r o l l e r *   P l a y e r )   o v e r r i d e ; 
 145:  
 146:  
 147:  
 148:  	 / /   R�^v^
 149: R�YSS_MR  G a m e M o d e   �[�^�v  G a m e S t a t e 0
 150:  
 151:  	 v i r t u a l   v o i d   I n i t G a m e S t a t e ( )   o v e r r i d e ; 
 152:  
 153:  
 154:  
 155:  	 / /   �f�e�s�[�v  P l a y e r S t a r t   MOn
 156: ��S(u�NYt  P o r t a l   I{yr�k�Qu�p;���0
 157:  
 158:  	 v i r t u a l   b o o l   U p d a t e P l a y e r S t a r t S p o t ( A C o n t r o l l e r *   P l a y e r ,   c o n s t   F S t r i n g &   P o r t a l ,   F S t r i n g &   O u t E r r o r M e s s a g e )   o v e r r i d e ; 
 159:  
 160:  
 161:  
 162:  	 / /   gbL��s�[�ReQ8nbT�v�(u
 163: R�YS;���0
 164:  
 165:  	 v i r t u a l   v o i d   G e n e r i c P l a y e r I n i t i a l i z a t i o n ( A C o n t r o l l e r *   N e w P l a y e r )   o v e r r i d e ; 
 166:  
 167:  
 168:  
 169:  	 / /   �s�[͑/T  P a w n   1Y%��e�(u
 170: �(u�NYt͑u1Y%�;���0
 171:  
 172:  	 v i r t u a l   v o i d   F a i l e d T o R e s t a r t P l a y e r ( A C o n t r o l l e r *   N e w P l a y e r )   o v e r r i d e ; 
 173:  
 174:  
 175:  
 176:  	 / / ~ E n d   o f   A G a m e M o d e B a s e   i n t e r f a c e 
 177:  
 178:  
 179:  
 180:  	 / /   ��Bl
 181: N N'^͑/Tc�[�s�[b  B o t 
 182: ��MQ(WS_MR8nbAm
 183: z-N�zsSgbL�͑u0
 184:  
 185:  	 / /   b F o r c e R e s e t   :N  t r u e   �eO�zsS͑n  C o n t r o l l e r 
 186: �v^>e_S_MRck(W�c6R�v  P a w n 0
 187:  
 188:  	 U F U N C T I O N ( B l u e p r i n t C a l l a b l e ) 
 189:  
 190:  	 v o i d   R e q u e s t P l a y e r R e s t a r t N e x t F r a m e ( A C o n t r o l l e r *   C o n t r o l l e r ,   b o o l   b F o r c e R e s e t   =   f a l s e ) ; 
 191:  
 192:  
 193:  
 194:  	 / /   �(u�v͑uag�N�h�g
 195: �
 196: T�e�(u�N�s�[�T  B o t 0
 197:  
 198:  	 v i r t u a l   b o o l   C o n t r o l l e r C a n R e s t a r t ( A C o n t r o l l e r *   C o n t r o l l e r ) ; 
 199:  
 200:  
 201:  
 202:  	 / /   �s�[�[b  G a m e M o d e   
 203: R�YST��S�v�N�N0
 204:  
 205:  	 F O n H o d g e G a m e M o d e P l a y e r I n i t i a l i z e d   O n G a m e M o d e P l a y e r I n i t i a l i z e d ; 
 206:  
 207:  
 208:  
 209:  p r o t e c t e d : 
 210:  
 211:  	 / /   E x p e r i e n c e   �R}��[bT�(u
 212: �(u�N9hncS_MR  E x p e r i e n c e   
 213: R�YS8nb�Q�[0
 214:  
 215:  	 v o i d   O n E x p e r i e n c e L o a d e d ( c o n s t   U H o d g e E x p e r i e n c e D e f i n i t i o n *   C u r r e n t E x p e r i e n c e ) ; 
 216:  
 217:  
 218:  
 219:  	 / /   $R�eS_MR  E x p e r i e n c e   /f&T�]�~�R}��[b0
 220:  
 221:  	 b o o l   I s E x p e r i e n c e L o a d e d ( )   c o n s t ; 
 222:  
 223:  
 224:  
 225:  	 / /   6e0R  M a t c h   RM��~�gTYt�[�^�v  E x p e r i e n c e 0
 226:  
 227:  	 v o i d   O n M a t c h A s s i g n m e n t G i v e n ( F P r i m a r y A s s e t I d   E x p e r i e n c e I d ,   c o n s t   F S t r i n g &   E x p e r i e n c e I d S o u r c e ) ; 
 228:  
 229:  
 230:  
 231:  	 / /   (WS_MR
 232: N ���I{�_  M a t c h   RM��e;N�RYt  M a t c h   RM�;���0
 233:  
 234:  	 v o i d   H a n d l e M a t c h A s s i g n m e n t I f N o t E x p e c t i n g O n e ( ) ; 
 235:  
 236:  
 237:  
 238:  	 / /   
 239: \ՋgbL�  D e d i c a t e d   S e r v e r   �v{vU_Am
 240: z0
 241:  
 242:  	 b o o l   T r y D e d i c a t e d S e r v e r L o g i n ( ) ; 
 243:  
 244:  
 245:  
 246:  	 / /   D e d i c a t e d   S e r v e r   ;Nc  M a t c h   �v�vsQ;���
 247: ��vMR�f*g/T(u0
 248:  
 249:  	 / /   v o i d   H o s t D e d i c a t e d S e r v e r M a t c h ( E C o m m o n S e s s i o n O n l i n e M o d e   O n l i n e M o d e ) ; 
 250:  
 251:  
 252:  
 253:  	 / /   D e d i c a t e d   S e r v e r   (u7b
 254: R�YS�[bT�v�V�
 255: ��vMR�f*g/T(u0
 256:  
 257:  	 / /   U F U N C T I O N ( ) 
 258:  
 259:  	 / /   v o i d   O n U s e r I n i t i a l i z e d F o r D e d i c a t e d S e r v e r ( c o n s t   U C o m m o n U s e r I n f o *   U s e r I n f o ,   b o o l   b S u c c e s s ,   F T e x t   E r r o r ,   E C o m m o n U s e r P r i v i l e g e   R e q u e s t e d P r i v i l e g e ,   E C o m m o n U s e r O n l i n e C o n t e x t   O n l i n e C o n t e x t ) ; 
 260:  
 261:  } ; 
 262:  
 263:  
```

## HodgeGameState.h

创建 ExperienceManager 和世界状态 ASC，处理游戏状态复制/扩展。

源码：[Source/Hodgepodge/Public/Core/GameState/HodgeGameState.h](../../../Source/Hodgepodge/Public/Core/GameState/HodgeGameState.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CoreMinimal.h"
   6: #include "AbilitySystemInterface.h"
   7: #include "HodgeGameStateBase.h"
   8: #include "HodgeGameState.generated.h"
  10: class UHodgeExperienceManagerComponent;
  11: class UHodgeAbilitySystemComponent;
  18: UCLASS()
  19: class HODGEPODGE_API AHodgeGameState : public AHodgeGameStateBase, public IAbilitySystemInterface
  20: {
  21: 	GENERATED_BODY()
  23: public:
  25: 	AHodgeGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  29: 	virtual void PreInitializeComponents() override;
  32: 	virtual void PostInitializeComponents() override;
  35: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  38: 	virtual void Tick(float DeltaSeconds) override;
  43: 	virtual void AddPlayerState(APlayerState* PlayerState) override;
  46: 	virtual void RemovePlayerState(APlayerState* PlayerState) override;
  49: 	virtual void SeamlessTravelTransitionCheckpoint(bool bToTransitionMap) override;
  54: 	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
  58: 	UFUNCTION(BlueprintCallable, Category = "Hodge|GameState")
  59: 	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponent() const { return AbilitySystemComponent; }
  70: 	float GetServerFPS() const;
  73: 	void SetRecorderPlayerState(APlayerState* NewPlayerState);
  76: 	APlayerState* GetRecorderPlayerState() const;
  79: 	DECLARE_MULTICAST_DELEGATE_OneParam(FOnRecorderPlayerStateChanged, APlayerState*);
  80: 	FOnRecorderPlayerStateChanged OnRecorderPlayerStateChangedEvent;
  82: private:
  84: 	UPROPERTY()
  85: 	TObjectPtr<UHodgeExperienceManagerComponent> ExperienceManagerComponent;
  88: 	UPROPERTY(VisibleAnywhere, Category = "Hodge|GameState")
  89: 	TObjectPtr<UHodgeAbilitySystemComponent> AbilitySystemComponent;
  91: protected:
  93: 	UPROPERTY(Replicated)
  94: 	float ServerFPS;
  98: 	UPROPERTY(Transient, ReplicatedUsing = OnRep_RecorderPlayerState)
  99: 	TObjectPtr<APlayerState> RecorderPlayerState;
 102: 	UFUNCTION()
 103: 	void OnRep_RecorderPlayerState();
 104: };
```

## HodgeGameStateBase.h

GameState 基础扩展生命周期。

源码：[Source/Hodgepodge/Public/Core/GameState/HodgeGameStateBase.h](../../../Source/Hodgepodge/Public/Core/GameState/HodgeGameStateBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: ��/ * * 
   2:  
   3:  *   @ f i l e   H o d g e G a m e S t a t e B a s e . h 
   4:  
   5:    *   @ b r i e f   A H o d g e G a m e S t a t e B a s e   {|�v4Y�e�N
   6:  
   7:    * 
   8:  
   9:    *   8nb�r`�W{|
  10: �ibU\�N
  11: g�RhV  F P S   �v�c0�e
  12: 0W�VRbcnt0�V>eU_6R���*�I{�R��0
  13:  
  14:    * / 
  15:  
  16:  
  17:  
  18:  # p r a g m a   o n c e 
  19:  
  20:  
  21:  
  22:  # i n c l u d e   " C o r e M i n i m a l . h " 
  23:  
  24:  # i n c l u d e   " G a m e F r a m e w o r k / G a m e S t a t e B a s e . h " 
  25:  
  26:  # i n c l u d e   " H o d g e G a m e S t a t e B a s e . g e n e r a t e d . h " 
  27:  
  28:  
  29:  
  30:  / * * 
  31:  
  32:    *   @ b r i e f   H o d g e p o d g e   Fh�g�v8nb�r`�W{|
  33:  
  34:    * 
  35:  
  36:    *   u}ThTg�f�
  37:  
  38:    *   -   T i c k ( �N
  39: g�RhV) ��k�y�f�e N!k  S e r v e r F P S   =   G A v e r a g e F P S ( ) 0
  40:  
  41:    *   -   S e a m l e s s T r a v e l T r a n s i t i o n C h e c k p o i n t �0W�VRbc�h�g�p
  42: �nt�eHe�s�[0
  43:  
  44:    *   -   O n R e p _ R e c o r d e r P l a y e r S t a t e ��V>eU_6R��r`
  45: Y6R�V�
  46: ���S�YXb��wU I I{P[�|�~0
  47:  
  48:    * / 
  49:  
  50:  U C L A S S ( ) 
  51:  
  52:  c l a s s   H O D G E P O D G E _ A P I   A H o d g e G a m e S t a t e B a s e   :   p u b l i c   A G a m e S t a t e B a s e 
  53:  
  54:  { 
  55:  
  56:  	 G E N E R A T E D _ B O D Y ( ) 
  57:  
  58:  
  59:  
  60:  p u b l i c : 
  61:  
  62:  	 / /   �g ��Qpe
  63: �
  64: R�YS  G a m e S t a t e   �v؞��M�n0
  65:  
  66:  	 A H o d g e G a m e S t a t e B a s e ( c o n s t   F O b j e c t I n i t i a l i z e r &   O b j e c t I n i t i a l i z e r ) ; 
  67:  
  68:  
  69:  
  70:  	 / / ~   B e g i n   A A c t o r   i n t e r f a c e 
  71:  
  72:  	 / /   G a m e S t a t e   �v�~�N
  73: R�YS�[bMR�(u
  74: ��S(u�N
  75: R�YS ����OV��~�N�v;���0
  76:  
  77:  	 v i r t u a l   v o i d   P r e I n i t i a l i z e C o m p o n e n t s ( )   o v e r r i d e ; 
  78:  
  79:  
  80:  
  81:  	 / /   G a m e S t a t e    _�YЏL��e�(u
  82: �(u�N
  83: R�YS8nb�r`�vsQ;���0
  84:  
  85:  	 v i r t u a l   v o i d   B e g i n P l a y ( )   o v e r r i d e ; 
  86:  
  87:  
  88:  
  89:  	 / /   G a m e S t a t e   sS\�~_gЏL��e�(u
  90: �(u�NntЏL��eD��n�T�d��vsQ�~�[0
  91:  
  92:  	 v i r t u a l   v o i d   E n d P l a y ( c o n s t   E E n d P l a y R e a s o n : : T y p e   E n d P l a y R e a s o n )   o v e r r i d e ; 
  93:  
  94:  	 / / ~   E n d   A A c t o r   i n t e r f a c e 
  95:  
  96:  } ; 
  97:  
  98:  
```

## HodgeHUDBase.h

项目 HUD 基类扩展入口，不代表 CommonUI 已完成。

源码：[Source/Hodgepodge/Public/Core/HUD/HodgeHUDBase.h](../../../Source/Hodgepodge/Public/Core/HUD/HodgeHUDBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: ��/ * * 
   2:  
   3:    *   @ f i l e   H o d g e H U D B a s e . h 
   4:  
   5:    *   @ b r i e f   A H o d g e H U D B a s e   {|�v4Y�e�N
   6:  
   7:    * 
   8:  
   9:    *   H U D   �W{|, 8nbLub��v�W{|, ��Yu�~P[{|�[�sꁚ[IN  U I 0
  10:  
  11:    * / 
  12:  
  13:  
  14:  
  15:  # p r a g m a   o n c e 
  16:  
  17:  
  18:  
  19:  # i n c l u d e   " C o r e M i n i m a l . h " 
  20:  
  21:  # i n c l u d e   " G a m e F r a m e w o r k / H U D . h " 
  22:  
  23:  # i n c l u d e   " H o d g e H U D B a s e . g e n e r a t e d . h " 
  24:  
  25:  
  26:  
  27:  / * * 
  28:  
  29:    *   @ b r i e f   H o d g e p o d g e   Fh�g�vH U D �W{|
  30:  
  31:    * 
  32:  
  33:    *   �~b�  A H U D , /f8nbLub��v�W{|0
  34:  
  35:    *   ��Yu�~P[{|�[�sꁚ[IN�vH U D ;���, �Y: 
  36:  
  37:    *   -   8nb;NLub�
  38:  
  39:    *   -   �Qf
  40:  
  41:    *   -   @�ag/ ݄ag>f:y
  42:  
  43:    *   -   \0W�V
  44:  
  45:    *   -   $O�[peW[
  46:  
  47:    * 
  48:  
  49:    *   O(u�e_: 
  50:  
  51:    *   1 .   �~bdk{|R�^wQSOH U D {|( �Y  A H o d g e G a m e H U D ) 
  52:  
  53:    *   2 .   (W  A H o d g e G a m e M o d e B a s e   -N��n:N؞��  H U D   {|
  54:  
  55:    *   3 .   ͑�Q  D r a w H U D ( )   bO(u  U M G   �c�NۏL�  U I   �~6R
  56:  
  57:    * / 
  58:  
  59:  U C L A S S ( ) 
  60:  
  61:  c l a s s   H O D G E P O D G E _ A P I   A H o d g e H U D B a s e   :   p u b l i c   A H U D 
  62:  
  63:  { 
  64:  
  65:  	 G E N E R A T E D _ B O D Y ( ) 
  66:  
  67:  } ; 
```

## HodgeLocalPlayerBase.h

本地玩家对象及控制器、PlayerState、Pawn 就绪事件桥。

源码：[Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h](../../../Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   9: #pragma once
  11: #include "CoreMinimal.h"
  12: #include "Engine/LocalPlayer.h"
  13: #include "HodgeLocalPlayerBase.generated.h"
  30: UCLASS()
  31: class HODGEPODGE_API UHodgeLocalPlayerBase : public ULocalPlayer
  32: {
  33: 	GENERATED_BODY()
  35: public:
  42: 	UHodgeLocalPlayerBase();
  52: 	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerControllerSetDelegate, UHodgeLocalPlayerBase* LocalPlayer,
  53: 	                                     APlayerController* PlayerController);
  54: 	FPlayerControllerSetDelegate OnPlayerControllerSet;
  63: 	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerStateSetDelegate, UHodgeLocalPlayerBase* LocalPlayer,
  64: 	                                     APlayerState* PlayerState);
  65: 	FPlayerStateSetDelegate OnPlayerStateSet;
  74: 	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerPawnSetDelegate, UHodgeLocalPlayerBase* LocalPlayer, APawn* Pawn);
  75: 	FPlayerPawnSetDelegate OnPlayerPawnSet;
  87: 	FDelegateHandle CallAndRegister_OnPlayerControllerSet(FPlayerControllerSetDelegate::FDelegate Delegate);
  98: 	FDelegateHandle CallAndRegister_OnPlayerStateSet(FPlayerStateSetDelegate::FDelegate Delegate);
 109: 	FDelegateHandle CallAndRegister_OnPlayerPawnSet(FPlayerPawnSetDelegate::FDelegate Delegate);
 111: public:
 124: 	virtual bool GetProjectionData(FViewport* Viewport, FSceneViewProjectionData& ProjectionData,
 125: 	                               int32 StereoViewIndex) const override;
 131: 	bool IsPlayerViewEnabled() const { return bIsPlayerViewEnabled; }
 141: 	void SetIsPlayerViewEnabled(bool bInIsPlayerViewEnabled) { bIsPlayerViewEnabled = bInIsPlayerViewEnabled; }
 146: private:
 153: 	bool bIsPlayerViewEnabled = true;
 154: };
```

## HodgePlayerControllerBase.h

把控制器和 Pawn 生命周期桥接到 LocalPlayer 委托。技能输入每帧消费尚未接入。

源码：[Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerControllerBase.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerControllerBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   9: #pragma once
  11: #include "CoreMinimal.h"
  12: #include "GameFramework/PlayerController.h"
  13: #include "HodgePlayerControllerBase.generated.h"
  38: UCLASS()
  39: class HODGEPODGE_API AHodgePlayerControllerBase : public APlayerController
  40: {
  41: 	GENERATED_BODY()
  43: public:
  50: 	AHodgePlayerControllerBase();
  65: 	virtual void ReceivedPlayer() override;
  78: 	virtual void SetPawn(APawn* InPawn) override;
  95: 	virtual void OnPossess(class APawn* APawn) override;
 109: 	virtual void OnUnPossess() override;
 111: protected:
 123: 	virtual void OnRep_PlayerState() override;
 124: };
```

## HodgePlayerState.h

玩家 ASC、HealthSet、PawnData、阵营/标签栈等持有者。基础能力授予循环仍停用。

源码：[Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/GameplayTagStack.h](../../../Source/Hodgepodge/Public/AbilitySystem/GameplayTagStack.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CoreMinimal.h"
   6: #include "AbilitySystemInterface.h"
   7: #include "GenericTeamAgentInterface.h"
   8: #include "HodgePlayerStateBase.h"
   9: #include "AbilitySystem/GameplayTagStack.h"
  10: #include "HodgePlayerState.generated.h"
  12: class UHodgeExperienceDefinition;
  13: class UHodgePawnData;
  14: class UHodgeAbilitySystemComponent;
  15: class AHodgePlayerControllerBase;
  20: UENUM()
  21: enum class EHodgePlayerConnectionType : uint8
  22: {
  24: 	Player = 0,
  27: 	LiveSpectator,
  30: 	ReplaySpectator,
  33: 	InactivePlayer
  34: };
  42: UCLASS(Config = Game)
  43: class AHodgePlayerState : public AHodgePlayerStateBase, public IAbilitySystemInterface
  44: {
  45: 	GENERATED_BODY()
  47: public:
  49: 	AHodgePlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  52: 	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerState")
  53: 	AHodgePlayerControllerBase* GetHodgePlayerController() const;
  56: 	UFUNCTION(BlueprintCallable, Category = "Hodge|PlayerState")
  57: 	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponent() const { return AbilitySystemComponent; }
  60: 	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
  63: 	template <class T>
  64: 	const T* GetPawnData() const { return Cast<T>(PawnData); }
  67: 	void SetPawnData(const UHodgePawnData* InPawnData);
  72: 	virtual void PreInitializeComponents() override;
  75: 	virtual void PostInitializeComponents() override;
  82: 	virtual void Reset() override;
  85: 	virtual void ClientInitialize(AController* C) override;
  88: 	virtual void CopyProperties(APlayerState* PlayerState) override;
  91: 	virtual void OnDeactivated() override;
  94: 	virtual void OnReactivated() override;
 112: 	static const FName NAME_HodgeAbilityReady;
 115: 	void SetPlayerConnectionType(EHodgePlayerConnectionType NewType);
 118: 	EHodgePlayerConnectionType GetPlayerConnectionType() const { return MyPlayerConnectionType; }
 121: 	UFUNCTION(BlueprintCallable)
 122: 	int32 GetSquadId() const
 123: 	{
 124: 		return MySquadID;
 125: 	}
 137: 	void SetSquadID(int32 NewSquadID);
 140: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
 141: 	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);
 144: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
 145: 	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);
 148: 	UFUNCTION(BlueprintCallable, Category=Teams)
 149: 	int32 GetStatTagStackCount(FGameplayTag Tag) const;
 152: 	UFUNCTION(BlueprintCallable, Category=Teams)
 153: 	bool HasStatTag(FGameplayTag Tag) const;
 161: 	FRotator GetReplicatedViewRotation() const;
 164: 	void SetReplicatedViewRotation(const FRotator& NewRotation);
 166: private:
 168: 	void OnExperienceLoaded(const UHodgeExperienceDefinition* CurrentExperience);
 170: protected:
 172: 	UFUNCTION()
 173: 	void OnRep_PawnData();
 176: 	UPROPERTY(ReplicatedUsing = OnRep_PawnData)
 177: 	TObjectPtr<const UHodgePawnData> PawnData;
 179: private:
 181: 	UPROPERTY(VisibleAnywhere, Category = "Hodge|PlayerState")
 182: 	TObjectPtr<UHodgeAbilitySystemComponent> AbilitySystemComponent;
 185: 	UPROPERTY()
 186: 	TObjectPtr<const class UHodgeHealthSet> HealthSet;
 189: 	UPROPERTY(Replicated)
 190: 	EHodgePlayerConnectionType MyPlayerConnectionType;
 196: 	UPROPERTY(ReplicatedUsing=OnRep_MyTeamID)
 197: 	FGenericTeamId MyTeamID;
 200: 	UPROPERTY(ReplicatedUsing=OnRep_MySquadID)
 201: 	int32 MySquadID;
 204: 	UPROPERTY(Replicated)
 205: 	FGameplayTagStackContainer StatTags;
 208: 	UPROPERTY(Replicated)
 209: 	FRotator ReplicatedViewRotation;
 211: private:
 213: 	UFUNCTION()
 214: 	void OnRep_MyTeamID(FGenericTeamId OldTeamID);
 217: 	UFUNCTION()
 218: 	void OnRep_MySquadID();
 219: };
```

## HodgePlayerStateBase.h

PlayerState ModularGameplay Receiver 注册、注销及组件 Reset/CopyProperties。

源码：[Source/Hodgepodge/Public/Core/PlayState/HodgePlayerStateBase.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerStateBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   3: #include "CoreMinimal.h"
   4: #include "GameFramework/PlayerState.h"
   5: #include "HodgePlayerStateBase.generated.h"
  11: UCLASS()
  12: class HODGEPODGE_API AHodgePlayerStateBase : public APlayerState
  13: {
  14: 	GENERATED_BODY()
  16: public:
  20: 	virtual void PreInitializeComponents() override;
  23: 	virtual void BeginPlay() override;
  26: 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  29: 	virtual void Reset() override;
  33: protected:
  37: 	virtual void CopyProperties(APlayerState* PlayerState);
  40: };
```
