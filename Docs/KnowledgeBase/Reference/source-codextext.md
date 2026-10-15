# CodexText 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeALSLocomotion.cpp

CodexText 实验：6 向地面运动动画实例，不属于主 Hero 动画链。

源码：[Source/Hodgepodge/Private/CodexText/HodgeALSLocomotion.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeALSLocomotion.cpp)

项目内直接 include（不是运行调用关系）：[CodexText/HodgeALSLocomotion.h](../../../Source/Hodgepodge/Public/CodexText/HodgeALSLocomotion.h)、[CodexText/HodgeLocomotionLab.h](../../../Source/Hodgepodge/Public/CodexText/HodgeLocomotionLab.h)

定义候选（多行签名仅展示首行）：

- L32: `FName UHodgeALSLocomotion::GetLabActiveState()`
- L39: `void UHodgeALSLocomotion::NativeUpdateAnimation(float Dt)`
- L113: `bool UHodgeALSAuthoring::BuildLocomotionGraph(UObject* Blueprint, const TArray<UObject*>& Animations, UObject* Idle, UObject* Fall)`

## HodgeGroundedAuthoring.cpp

CodexText 实验：编辑器辅助库，含 Grounded 图层与停步/转身精修、步幅、地形、战斗图层等编辑期构建函数。

源码：[Source/Hodgepodge/Private/CodexText/HodgeGroundedAuthoring.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeGroundedAuthoring.cpp)

项目内直接 include（不是运行调用关系）：[CodexText/HodgeGroundedLocomotion.h](../../../Source/Hodgepodge/Public/CodexText/HodgeGroundedLocomotion.h)

定义候选（多行签名仅展示首行）：

- L69: `bool UHodgeGroundedAuthoring::RepairGroundedLegIK(UObject* Blueprint)`
- L120: `bool UHodgeGroundedAuthoring::RefineGroundedTransitions(UObject* Blueprint)`
- L184: `bool UHodgeGroundedAuthoring::AddStrideLayer(UObject* Blueprint)`
- L244: `bool UHodgeGroundedAuthoring::AddTerrainLayer(UObject* Blueprint)`
- L307: `bool UHodgeGroundedAuthoring::AddCombatLayer(UObject* Blueprint, UObject* Overlay)`
- L364: `bool UHodgeGroundedAuthoring::AddGroundedLayer(UObject* Blueprint, const TArray<UObject*>& Actions, UObject* Idle, UObject* Fall, UObject* PreviewMesh)`

## HodgeGroundedLocomotion.cpp

CodexText 实验：在 ALS 基础动画上增加平地起停/转身/脚锁，并扩展上半身 Overlay、左手握持、步幅与地形 IK。

源码：[Source/Hodgepodge/Private/CodexText/HodgeGroundedLocomotion.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeGroundedLocomotion.cpp)

项目内直接 include（不是运行调用关系）：[CodexText/HodgeGroundedLocomotion.h](../../../Source/Hodgepodge/Public/CodexText/HodgeGroundedLocomotion.h)、[CodexText/HodgeLocomotionLab.h](../../../Source/Hodgepodge/Public/CodexText/HodgeLocomotionLab.h)

定义候选（多行签名仅展示首行）：

- L14: `void UHodgeGroundedLocomotion::NativeInitializeAnimation()`
- L35: `FName UHodgeGroundedLocomotion::GetGroundedState()`
- L42: `void UHodgeGroundedLocomotion::SetLeftHandGrip(USceneComponent* Component, FName Socket, float Weight)`
- L48: `void UHodgeGroundedLocomotion::NativeUninitializeAnimation()`
- L59: `void UHodgeGroundedLocomotion::CacheFinalizedFootPose()`
- L72: `void UHodgeGroundedLocomotion::NativeUpdateAnimation(float Dt)`

## HodgeLocomotionLab.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/CodexText/HodgeLocomotionLab.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeLocomotionLab.cpp)

项目内直接 include（不是运行调用关系）：[CodexText/HodgeLocomotionLab.h](../../../Source/Hodgepodge/Public/CodexText/HodgeLocomotionLab.h)、[CodexText/HodgeALSLocomotion.h](../../../Source/Hodgepodge/Public/CodexText/HodgeALSLocomotion.h)、[CodexText/HodgeGroundedLocomotion.h](../../../Source/Hodgepodge/Public/CodexText/HodgeGroundedLocomotion.h)、[Data/HodgeExperienceDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceDefinition.h)

定义候选（多行签名仅展示首行）：

- L31: `UHodgeLocomotionLabComponent::UHodgeLocomotionLabComponent()`
- L38: `void UHodgeLocomotionLabComponent::BeginPlay()`
- L53: `void UHodgeLocomotionLabComponent::SetCombatFacing(bool bEnabled)`
- L67: `void UHodgeLocomotionLabComponent::SetWalking(bool bEnabled)`
- L77: `void UHodgeLocomotionLabComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)`
- L121: `void AHodgeLocomotionLabMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)`
- L129: `bool UHodgeLocomotionLabAuthoring::RemapCopy(UObject* Asset, const TArray<UObject*>& Sources, const TArray<UObject*>& Copies)`
- L167: `bool UHodgeLocomotionLabAuthoring::ConfigureGroundBlend(UObject* Blueprint, UObject* BlendSpace)`

## HodgeSurvivor.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/CodexText/HodgeSurvivor.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeSurvivor.cpp)

项目内直接 include（不是运行调用关系）：[CodexText/HodgeSurvivor.h](../../../Source/Hodgepodge/Public/CodexText/HodgeSurvivor.h)

定义候选（多行签名仅展示首行）：

- L26: `AHodgeSurvivorHero::AHodgeSurvivorHero()`
- L54: `void AHodgeSurvivorHero::SetupPlayerInputComponent(UInputComponent* Input)`
- L77: `void AHodgeSurvivorHero::Forward(const FInputActionValue& Value)`
- L83: `void AHodgeSurvivorHero::Right(const FInputActionValue& Value)`
- L89: `void AHodgeSurvivorHero::Dash()`
- L99: `void AHodgeSurvivorHero::Tick(float Dt)`
- L117: `void AHodgeSurvivorHero::EndPlay(const EEndPlayReason::Type Reason)`
- L126: `AHodgeSurvivorMode::AHodgeSurvivorMode()`
- L137: `void AHodgeSurvivorMode::StartPlay()`
- L152: `AActor* AHodgeSurvivorMode::MakeShape(FVector Location, FVector Scale, UMaterialInterface* Material, float Life)`
- L168: `void AHodgeSurvivorMode::SpawnEnemy(bool Elite)`
- L190: `void AHodgeSurvivorMode::HitEnemy(int32 Index, float Amount)`
- L208: `void AHodgeSurvivorMode::DamageHero(float Amount)`
- L216: `void AHodgeSurvivorMode::GrantExperience(int32 Amount)`
- L229: `FString AHodgeSurvivorMode::GetUpgradeText(int32 Slot) const`
- L235: `void AHodgeSurvivorMode::ChooseUpgrade(int32 Slot)`
- L251: `void AHodgeSurvivorMode::SetRunState(EHodgeSurvivorState NewState)`
- L272: `void AHodgeSurvivorMode::TogglePause()`
- L278: `void AHodgeSurvivorMode::RestartRun()`
- L283: `void AHodgeSurvivorMode::ReturnToMenu()`
- L288: `void AHodgeSurvivorMode::Tick(float Dt)`
- L374: `void AHodgeSurvivorMode::EndPlay(const EEndPlayReason::Type Reason)`

## HodgeSurvivorHUD.cpp

CodexText 实验：代码构建的 Survivor HUD UserWidget。

源码：[Source/Hodgepodge/Private/CodexText/HodgeSurvivorHUD.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeSurvivorHUD.cpp)

项目内直接 include（不是运行调用关系）：[CodexText/HodgeSurvivor.h](../../../Source/Hodgepodge/Public/CodexText/HodgeSurvivor.h)

定义候选（多行签名仅展示首行）：

- L41: `AHodgeSurvivorMode* UHodgeSurvivorHUD::Mode() const { return GetWorld()?GetWorld()->GetAuthGameMode<AHodgeSurvivorMode>():nullptr; }`
- L43: `void UHodgeSurvivorHUD::Text(FName Name,const FString& Value)`
- L48: `UButton* UHodgeSurvivorHUD::AddButton(UVerticalBox* Box,const FString& Label,UTextBlock*& OutLabel)`
- L63: `void UHodgeSurvivorHUD::NativeConstruct()`
- L134: `void UHodgeSurvivorHUD::Refresh(AHodgeSurvivorMode* M)`
- L174: `void UHodgeSurvivorHUD::NativeDestruct()`
- L184: `void UHodgeSurvivorHUD::ChooseOne() { if (auto* M=Mode()) M->ChooseUpgrade(0); }`
- L186: `void UHodgeSurvivorHUD::ChooseTwo() { if (auto* M=Mode()) M->ChooseUpgrade(1); }`
- L188: `void UHodgeSurvivorHUD::ChooseThree() { if (auto* M=Mode()) M->ChooseUpgrade(2); }`
- L190: `void UHodgeSurvivorHUD::Retry() { if (auto* M=Mode()) M->RestartRun(); }`
- L192: `void UHodgeSurvivorHUD::Menu() { if (auto* M=Mode()) M->ReturnToMenu(); }`
- L194: `void UHodgeSurvivorHUD::Pause() { if (auto* M=Mode()) M->TogglePause(); }`

## HodgeALSLocomotion.h

CodexText 实验：6 向地面运动动画实例，不属于主 Hero 动画链。

源码：[Source/Hodgepodge/Public/CodexText/HodgeALSLocomotion.h](../../../Source/Hodgepodge/Public/CodexText/HodgeALSLocomotion.h)

项目内直接 include（不是运行调用关系）：[Animation/HodgeAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimInstance.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "Animation/HodgeAnimInstance.h"
   8: #include "Kismet/BlueprintFunctionLibrary.h"
   9: #include "HodgeALSLocomotion.generated.h"
  12: UENUM(BlueprintType)
  13: enum class EHodgeALSDirection : uint8 {             Forward,             Backward,                 LeftForward,                 LeftBackward,                  RightForward,                  RightBackward };
  17: UCLASS()
  18: class HODGEPODGE_API UHodgeALSLocomotion : public UHodgeAnimInstance
  19: {
  20: 	GENERATED_BODY()
  22: public:
  26: 	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
  30: 	UFUNCTION(BlueprintPure, Category="ALS Lab")
  31: 	FName GetLabActiveState();
  35: 	UFUNCTION(BlueprintCallable, Category="ALS Lab")
  36: 	void SetHipBias(float Bias) { HipOrientationBias = FMath::Clamp(Bias, -1.f, 1.f); }
  39: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  40: 	EHodgeALSDirection LabDirection = EHodgeALSDirection::Forward;
  42: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  43: 	float LabForward = 1.f;
  45: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  46: 	float LabBackward = 0.f;
  48: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  49: 	float LabLeft = 0.f;
  51: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  52: 	float LabRight = 0.f;
  54: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  55: 	float LabGait = 0.f;
  57: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  58: 	float LabPlayRate = 1.f;
  60: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  61: 	float LabSpeed = 0.f;
  63: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  64: 	float LabViewAngle = 0.f;
  66: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  67: 	float LabYawOffset = 0.f;
  69: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  70: 	float LabFeetCrossing = 0.f;
  72: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALS Lab", meta=(ClampMin="-1", ClampMax="1"))
  73: 	float HipOrientationBias = 0.f;
  75: 	UPROPERTY(EditDefaultsOnly, Category="ALS Lab")
  76: 	TObjectPtr<class UCurveFloat> ForwardBackwardYawCurve;
  78: 	UPROPERTY(EditDefaultsOnly, Category="ALS Lab")
  79: 	TObjectPtr<class UCurveFloat> LeftYawCurve;
  81: 	UPROPERTY(EditDefaultsOnly, Category="ALS Lab")
  82: 	TObjectPtr<class UCurveFloat> RightYawCurve;
  84: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  85: 	bool bLabIdle = true;
  87: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  88: 	bool bLabFall = false;
  90: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  91: 	bool bLabF = false;
  93: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  94: 	bool bLabB = false;
  96: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  97: 	bool bLabLF = false;
  99: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
 100: 	bool bLabLB = false;
 102: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
 103: 	bool bLabRF = false;
 105: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
 106: 	bool bLabRB = false;
 108: private:
 110: 	float DirectionHoldRemaining = 0.f;
 111: };
 114: UCLASS()
 115: class HODGEPODGE_API UHodgeALSAuthoring : public UBlueprintFunctionLibrary
 116: {
 117: 	GENERATED_BODY()
 119: public:
 128: 	UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
 129: 	static bool BuildLocomotionGraph(UObject* Blueprint, const TArray<UObject*>& WalkRunAnimations, UObject* Idle,
 130: 	                                 UObject* Fall);
 131: };
```

## HodgeGroundedLocomotion.h

CodexText 实验：在 ALS 基础动画上增加平地起停/转身/脚锁，并扩展上半身 Overlay、左手握持、步幅与地形 IK。

源码：[Source/Hodgepodge/Public/CodexText/HodgeGroundedLocomotion.h](../../../Source/Hodgepodge/Public/CodexText/HodgeGroundedLocomotion.h)

项目内直接 include（不是运行调用关系）：[CodexText/HodgeALSLocomotion.h](../../../Source/Hodgepodge/Public/CodexText/HodgeALSLocomotion.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   5: #include "CoreMinimal.h"
   6: #include "CodexText/HodgeALSLocomotion.h"
   7: #include "HodgeGroundedLocomotion.generated.h"
  11: UCLASS()
  12: class HODGEPODGE_API UHodgeGroundedLocomotion : public UHodgeALSLocomotion
  13: {
  14: 	GENERATED_BODY()
  16: public:
  18: 	virtual void NativeInitializeAnimation() override;
  22: 	virtual void NativeUpdateAnimation(float Dt) override;
  24:     virtual void NativeUninitializeAnimation() override;
  28: 	UFUNCTION(BlueprintPure)
  29: 	FName GetGroundedState();
  33: 	UFUNCTION(BlueprintPure)
  34: 	float GetGroundedRotationTarget() const { return RotationTarget; }
  39: 	UFUNCTION(BlueprintPure)
  40: 	bool IsTurningInPlace() const { return bGroundTurnL || bGroundTurnR; }
  44: 	UFUNCTION(BlueprintCallable, Category="Grounded|Combat")
  45: 	void SetOverlayEnabled(bool bEnabled) { OverlayTarget = bEnabled ? 1.f : 0.f; }
  51: 	UFUNCTION(BlueprintCallable, Category="Grounded|Combat")
  52: 	void SetLeftHandGrip(class USceneComponent* Component, FName Socket, float Weight = 1.f);
  54: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grounded|Combat")
  55: 	TObjectPtr<class UAnimSequence> OverlayPose;
  57: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded|Combat")
  58: 	float OverlayAlpha = 0.f;
  60: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded|Combat")
  61: 	float HandIKAlpha = 0.f;
  63: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded|Combat")
  64: 	FVector HandGripLocation = FVector::ZeroVector;
  66: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded|Combat")
  67: 	FRotator HandGripRotation = FRotator::ZeroRotator;
  70: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  71: 	bool bGroundIdle = true;
  73: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  74: 	bool bGroundMove = false;
  76: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  77: 	bool bGroundStopWalkL = false;
  79: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  80: 	bool bGroundStopWalkR = false;
  82: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  83: 	bool bGroundStopRunL = false;
  85: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  86: 	bool bGroundStopRunR = false;
  88: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  89: 	bool bGroundTurnL = false;
  91: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  92: 	bool bGroundTurnR = false;
  94: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  95: 	float GroundActionTime = 0.f;
  97: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  98: 	float GroundActionPlayRate = 1.f;
 100: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
 101: 	bool bStopPlanted = false;
 103: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
 104: 	bool bStopAirborne = true;
 106: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded|Stride")
 107: 	float StrideScale = 1.f;
 109: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded|Stride")
 110: 	float StrideAlpha = 0.f;
 112: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded|Stride")
 113: 	float DiagonalScale = 1.f;
 115: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded|Stride")
 116: 	FVector StrideDirection = FVector::ForwardVector;
 118: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grounded|Terrain")
 119: 	bool bEnableTerrainIK = true;
 121: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded|Terrain")
 122: 	float TerrainAlpha = 0.f;
 124: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
 125: 	float FootAlphaL = 0.f;
 127: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
 128: 	float FootAlphaR = 0.f;
 130: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
 131: 	FVector FootTargetL = FVector::ZeroVector;
 133: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
 134: 	FVector FootTargetR = FVector::ZeroVector;
 136: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
 137: 	FRotator FootRotationL = FRotator::ZeroRotator;
 139: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
 140: 	FRotator FootRotationR = FRotator::ZeroRotator;
 142: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
 143: 	bool bSupportLeft = true;
 145: 	UPROPERTY(EditDefaultsOnly, Category="Grounded")
 146: 	TArray<TObjectPtr<class UAnimSequence>> GroundActions;
 148: 	UPROPERTY(EditDefaultsOnly, Category="Grounded")
 149: 	TObjectPtr<class UCurveFloat> TurnYawLeft;
 151: 	UPROPERTY(EditDefaultsOnly, Category="Grounded")
 152: 	TObjectPtr<class UCurveFloat> TurnYawRight;
 154: 	UPROPERTY(EditDefaultsOnly, Category="Grounded")
 155: 	float TurnThreshold = 70.f;
 157: private:
 159:     TWeakObjectPtr<class USceneComponent> HandGripComponent;
 161:     FName HandGripSocket;
 163:     float HandGripWeight = 0.f;
 165:     float OverlayTarget = 0.f;
 167:     void CacheFinalizedFootPose();
 169:     TWeakObjectPtr<class USkeletalMeshComponent> PoseMesh;
 171:     FDelegateHandle FinalizedPoseHandle;
 173: 	FTransform PreviousRawFeet[2];
 175: 	FTransform LockedFeet[2];
 177: 	bool bHavePose = false;
 179: 	bool bHadMovement = false;
 181: 	bool bLastWalk = false;
 183: 	bool bLocked[2] = {false, false};
 185: 	float PreviousLockCurve[2] = {0.f, 0.f};
 187: 	float RotationTarget = 0.f;
 189: 	float TurnStartYaw = 0.f;
 191: 	float TurnDelay = 0.f;
 193: 	float StopEntryContact[2] = {0.f, 0.f};
 195: 	int32 ActionIndex = INDEX_NONE;
 197: 	FVector PreviousActorLocation = FVector::ZeroVector;
 198: };
 201: UCLASS()
 202: class HODGEPODGE_API UHodgeGroundedAuthoring : public UBlueprintFunctionLibrary
 203: {
 204: 	GENERATED_BODY()
 206: public:
 211: 	UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
 212: 	static bool RepairGroundedLegIK(UObject* Blueprint);
 217: 	UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
 218: 	static bool RefineGroundedTransitions(UObject* Blueprint);
 223: 	UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
 224: 	static bool AddStrideLayer(UObject* Blueprint);
 229: 	UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
 230: 	static bool AddTerrainLayer(UObject* Blueprint);
 236: 	UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
 237: 	static bool AddCombatLayer(UObject* Blueprint, UObject* Overlay);
 247: 	UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
 248: 	static bool AddGroundedLayer(UObject* Blueprint, const TArray<UObject*>& Actions, UObject* Idle, UObject* Fall,
 249: 	                             UObject* PreviewMesh);
 250: };
```

## HodgeLocomotionLab.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/CodexText/HodgeLocomotionLab.h](../../../Source/Hodgepodge/Public/CodexText/HodgeLocomotionLab.h)

项目内直接 include（不是运行调用关系）：[Core/GameMode/HodgeGameModeBase.h](../../../Source/Hodgepodge/Public/Core/GameMode/HodgeGameModeBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "Components/ActorComponent.h"
   8: #include "Core/GameMode/HodgeGameModeBase.h"
   9: #include "Kismet/BlueprintFunctionLibrary.h"
  10: #include "HodgeLocomotionLab.generated.h"
  14: UCLASS(Blueprintable, ClassGroup=(Hodge), meta=(BlueprintSpawnableComponent))
  15: class HODGEPODGE_API UHodgeLocomotionLabComponent : public UActorComponent
  16: {
  17:     GENERATED_BODY()
  18: public:
  20:     UHodgeLocomotionLabComponent();
  22:     virtual void BeginPlay() override;
  28:     virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
  30:     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Locomotion") float WalkSpeed = 180.f;
  32:     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Locomotion") float RunSpeed = 420.f;
  34:     UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Locomotion") bool bCombatFacing = true;
  36:     UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Locomotion") bool bWalking = false;
  40:     UFUNCTION(BlueprintCallable) void SetCombatFacing(bool bEnabled);
  44:     UFUNCTION(BlueprintCallable) void SetWalking(bool bEnabled);
  48:     UFUNCTION(BlueprintCallable) void SetKeyboardControlsEnabled(bool bEnabled) { bKeyboardControlsEnabled = bEnabled; }
  49: private:
  51:     bool bKeyboardControlsEnabled = true;
  52: };
  56: UCLASS()
  57: class HODGEPODGE_API AHodgeLocomotionLabMode : public AHodgeGameModeBase
  58: {
  59:     GENERATED_BODY()
  60: public:
  66:     virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
  67: };
  71: UCLASS()
  72: class HODGEPODGE_API UHodgeLocomotionLabAuthoring : public UBlueprintFunctionLibrary
  73: {
  74:     GENERATED_BODY()
  75: public:
  82:     UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
  83:     static bool RemapCopy(UObject* Asset, const TArray<UObject*>& Sources, const TArray<UObject*>& Copies);
  89:     UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
  90:     static bool ConfigureGroundBlend(UObject* Blueprint, UObject* BlendSpace);
  91: };
```

## HodgeSurvivor.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/CodexText/HodgeSurvivor.h](../../../Source/Hodgepodge/Public/CodexText/HodgeSurvivor.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "GameFramework/Character.h"
   8: #include "GameFramework/GameModeBase.h"
   9: #include "Blueprint/UserWidget.h"
  10: #include "HodgeSurvivor.generated.h"
  12: class UInputAction;
  13: class UInputMappingContext;
  14: struct FInputActionValue;
  15: class UAnimationAsset;
  16: class UMaterialInterface;
  17: class UStaticMesh;
  18: class UCanvasPanel;
  19: class UVerticalBox;
  20: class UTextBlock;
  21: class UButton;
  22: class UProgressBar;
  25: UCLASS()
  26: class HODGEPODGE_API AHodgeSurvivorHero : public ACharacter
  27: {
  28:     GENERATED_BODY()
  29: public:
  31:     AHodgeSurvivorHero();
  35:     virtual void Tick(float DeltaSeconds) override;
  39:     virtual void SetupPlayerInputComponent(UInputComponent* Input) override;
  43:     virtual void EndPlay(const EEndPlayReason::Type Reason) override;
  45:     UFUNCTION(BlueprintCallable) void Dash();
  47:     UPROPERTY(BlueprintReadOnly) float DashCooldown = 0;
  49:     UPROPERTY(BlueprintReadOnly) float Invulnerability = 0;
  50: private:
  54:     void Forward(const FInputActionValue& Value);
  58:     void Right(const FInputActionValue& Value);
  60:     UPROPERTY() TObjectPtr<UInputMappingContext> Mapping;
  62:     UPROPERTY() TObjectPtr<UInputAction> ForwardAction;
  64:     UPROPERTY() TObjectPtr<UInputAction> RightAction;
  66:     UPROPERTY() TObjectPtr<UAnimationAsset> Idle;
  68:     UPROPERTY() TObjectPtr<UAnimationAsset> Run;
  70:     bool bRunning = false;
  71: };
  74: UENUM(BlueprintType)
  75: enum class EHodgeSurvivorState : uint8 {                  Playing,                  Upgrade,                 Paused,                 Defeat,                  Victory };
  78: struct FHodgeSurvivorEnemy
  79: {
  81:     TWeakObjectPtr<AActor> Actor;
  83:     float Health = 30;
  85:     float Speed = 130;
  87:     bool bElite = false;
  88: };
  91: struct FHodgeSurvivorOrb
  92: {
  94:     TWeakObjectPtr<AActor> Actor;
  96:     int32 Value = 1;
  97: };
 100: UCLASS()
 101: class HODGEPODGE_API AHodgeSurvivorMode : public AGameModeBase
 102: {
 103:     GENERATED_BODY()
 104: public:
 106:     AHodgeSurvivorMode();
 108:     virtual void StartPlay() override;
 112:     virtual void Tick(float DeltaSeconds) override;
 116:     virtual void EndPlay(const EEndPlayReason::Type Reason) override;
 120:     UFUNCTION(BlueprintCallable) void ChooseUpgrade(int32 Slot);
 122:     UFUNCTION(BlueprintCallable) void TogglePause();
 124:     UFUNCTION(BlueprintCallable) void RestartRun();
 126:     UFUNCTION(BlueprintCallable) void ReturnToMenu();
 130:     UFUNCTION(BlueprintCallable) void GrantExperience(int32 Amount);
 134:     UFUNCTION(BlueprintCallable) void DamageHero(float Amount);
 138:     UFUNCTION(BlueprintCallable) void SpawnEnemy(bool bElite = false);
 142:     UFUNCTION(BlueprintPure) int32 GetEnemyCount() const { return Enemies.Num(); }
 146:     UFUNCTION(BlueprintPure) int32 GetOrbCount() const { return Orbs.Num(); }
 151:     UFUNCTION(BlueprintPure) FString GetUpgradeText(int32 Slot) const;
 153:     UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Survivor") float RoundDuration = 180;
 155:     UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Survivor") float SpawnInterval = 1.15f;
 157:     UPROPERTY(EditDefaultsOnly, Category="Survivor") TSubclassOf<UUserWidget> InterfaceClass;
 159:     UPROPERTY(EditDefaultsOnly, Category="Survivor") TObjectPtr<UMaterialInterface> EnemyMaterial;
 161:     UPROPERTY(EditDefaultsOnly, Category="Survivor") TObjectPtr<UMaterialInterface> EliteMaterial;
 163:     UPROPERTY(EditDefaultsOnly, Category="Survivor") TObjectPtr<UMaterialInterface> EnergyMaterial;
 165:     UPROPERTY(BlueprintReadOnly) EHodgeSurvivorState State = EHodgeSurvivorState::Playing;
 167:     UPROPERTY(BlueprintReadOnly) float Health = 100;
 169:     UPROPERTY(BlueprintReadOnly) float MaxHealth = 100;
 171:     UPROPERTY(BlueprintReadOnly) float Elapsed = 0;
 173:     UPROPERTY(BlueprintReadOnly) int32 Kills = 0;
 175:     UPROPERTY(BlueprintReadOnly) int32 Level = 1;
 177:     UPROPERTY(BlueprintReadOnly) int32 Experience = 0;
 179:     UPROPERTY(BlueprintReadOnly) int32 NextLevelXP = 5;
 181:     UPROPERTY(BlueprintReadOnly) float Damage = 22;
 183:     UPROPERTY(BlueprintReadOnly) float AttackInterval = .65f;
 185:     UPROPERTY(BlueprintReadOnly) float PickupRadius = 320;
 187:     UPROPERTY(BlueprintReadOnly) int32 PulseLevel = 0;
 189:     UPROPERTY(BlueprintReadOnly) TObjectPtr<AHodgeSurvivorHero> Hero;
 191:     UPROPERTY(BlueprintReadOnly) TObjectPtr<UUserWidget> Interface;
 193:     TArray<int32> Choices;
 194: private:
 198:     void SetRunState(EHodgeSurvivorState NewState);
 203:     void HitEnemy(int32 Index, float Amount);
 211:     AActor* MakeShape(FVector Location, FVector Scale, UMaterialInterface* Material, float Life = 0);
 213:     UPROPERTY() TObjectPtr<UStaticMesh> Sphere;
 215:     UPROPERTY() TObjectPtr<USkeletalMesh> EnemyMesh;
 217:     UPROPERTY() TObjectPtr<UAnimationAsset> EnemyRun;
 219:     TArray<FHodgeSurvivorEnemy> Enemies;
 221:     TArray<FHodgeSurvivorOrb> Orbs;
 223:     float SpawnClock = 0;
 225:     float AttackClock = 0;
 227:     float HurtClock = 0;
 229:     float PulseClock = 0;
 231:     float InterfaceClock = 0;
 233:     int32 NextElite = 35;
 234: };
 237: UCLASS()
 238: class HODGEPODGE_API UHodgeSurvivorHUD : public UUserWidget
 239: {
 240:     GENERATED_BODY()
 241: public:
 243:     virtual void NativeConstruct() override;
 245:     virtual void NativeDestruct() override;
 249:     void Refresh(AHodgeSurvivorMode* Mode);
 251:     UFUNCTION() void ChooseOne();
 253:     UFUNCTION() void ChooseTwo();
 255:     UFUNCTION() void ChooseThree();
 257:     UFUNCTION() void Retry();
 259:     UFUNCTION() void Menu();
 261:     UFUNCTION() void Pause();
 262: private:
 266:     AHodgeSurvivorMode* Mode() const;
 271:     void Text(FName Name, const FString& Value);
 278:     UButton* AddButton(UVerticalBox* Box, const FString& Label, UTextBlock*& OutLabel);
 280:     UPROPERTY() TObjectPtr<UCanvasPanel> Modal;
 282:     UPROPERTY() TObjectPtr<UTextBlock> ModalTitle;
 284:     UPROPERTY() TObjectPtr<UTextBlock> ModalDescription;
 286:     UPROPERTY() TArray<TObjectPtr<UButton>> ChoiceButtons;
 288:     UPROPERTY() TArray<TObjectPtr<UTextBlock>> ChoiceLabels;
 290:     UPROPERTY() TObjectPtr<UButton> RetryButton;
 292:     UPROPERTY() TObjectPtr<UButton> MenuButton;
 294:     UPROPERTY() TObjectPtr<UButton> PauseButton;
 296:     UPROPERTY() TObjectPtr<UProgressBar> XPBar;
 298:     UPROPERTY() TObjectPtr<UTextBlock> XPText;
 300:     UPROPERTY() TObjectPtr<UTextBlock> PauseLabel;
 301: };
```
