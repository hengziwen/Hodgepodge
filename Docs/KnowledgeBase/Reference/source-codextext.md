# CodexText 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeALSLocomotion.cpp

CodexText 实验：6 向地面运动动画实例，不属于主 Hero 动画链。

源码：[Source/Hodgepodge/Private/CodexText/HodgeALSLocomotion.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeALSLocomotion.cpp)

项目内直接 include（不是运行调用关系）：[CodexText/HodgeALSLocomotion.h](../../../Source/Hodgepodge/Public/CodexText/HodgeALSLocomotion.h)、[CodexText/HodgeLocomotionLab.h](../../../Source/Hodgepodge/Public/CodexText/HodgeLocomotionLab.h)

定义候选（多行签名仅展示首行）：

- L28: `FName UHodgeALSLocomotion::GetLabActiveState()`
- L34: `void UHodgeALSLocomotion::NativeUpdateAnimation(float Dt)`
- L107: `bool UHodgeALSAuthoring::BuildLocomotionGraph(UObject* Blueprint, const TArray<UObject*>& Animations, UObject* Idle, UObject* Fall)`

## HodgeGroundedAuthoring.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/CodexText/HodgeGroundedAuthoring.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeGroundedAuthoring.cpp)

项目内直接 include（不是运行调用关系）：[CodexText/HodgeGroundedLocomotion.h](../../../Source/Hodgepodge/Public/CodexText/HodgeGroundedLocomotion.h)

定义候选（多行签名仅展示首行）：

- L39: `bool UHodgeGroundedAuthoring::AddGroundedLayer(UObject* Blueprint, const TArray<UObject*>& Actions, UObject* Idle, UObject* Fall, UObject* PreviewMesh)`

## HodgeGroundedLocomotion.cpp

CodexText 实验：在 ALS 基础动画上增加平地起停/转身/脚锁。

源码：[Source/Hodgepodge/Private/CodexText/HodgeGroundedLocomotion.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeGroundedLocomotion.cpp)

项目内直接 include（不是运行调用关系）：[CodexText/HodgeGroundedLocomotion.h](../../../Source/Hodgepodge/Public/CodexText/HodgeGroundedLocomotion.h)、[CodexText/HodgeLocomotionLab.h](../../../Source/Hodgepodge/Public/CodexText/HodgeLocomotionLab.h)

定义候选（多行签名仅展示首行）：

- L10: `void UHodgeGroundedLocomotion::NativeInitializeAnimation()`
- L18: `FName UHodgeGroundedLocomotion::GetGroundedState()`
- L24: `void UHodgeGroundedLocomotion::NativePostEvaluateAnimation()`
- L36: `void UHodgeGroundedLocomotion::NativeUpdateAnimation(float Dt)`

## HodgeLocomotionLab.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/CodexText/HodgeLocomotionLab.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeLocomotionLab.cpp)

项目内直接 include（不是运行调用关系）：[CodexText/HodgeLocomotionLab.h](../../../Source/Hodgepodge/Public/CodexText/HodgeLocomotionLab.h)、[CodexText/HodgeALSLocomotion.h](../../../Source/Hodgepodge/Public/CodexText/HodgeALSLocomotion.h)、[CodexText/HodgeGroundedLocomotion.h](../../../Source/Hodgepodge/Public/CodexText/HodgeGroundedLocomotion.h)、[Data/HodgeExperienceDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceDefinition.h)

定义候选（多行签名仅展示首行）：

- L27: `UHodgeLocomotionLabComponent::UHodgeLocomotionLabComponent()`
- L33: `void UHodgeLocomotionLabComponent::BeginPlay()`
- L42: `void UHodgeLocomotionLabComponent::SetCombatFacing(bool bEnabled)`
- L55: `void UHodgeLocomotionLabComponent::SetWalking(bool bEnabled)`
- L64: `void UHodgeLocomotionLabComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)`
- L103: `void AHodgeLocomotionLabMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)`
- L110: `bool UHodgeLocomotionLabAuthoring::RemapCopy(UObject* Asset, const TArray<UObject*>& Sources, const TArray<UObject*>& Copies)`
- L147: `bool UHodgeLocomotionLabAuthoring::ConfigureGroundBlend(UObject* Blueprint, UObject* BlendSpace)`

## HodgeSurvivor.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/CodexText/HodgeSurvivor.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeSurvivor.cpp)

项目内直接 include（不是运行调用关系）：[CodexText/HodgeSurvivor.h](../../../Source/Hodgepodge/Public/CodexText/HodgeSurvivor.h)

定义候选（多行签名仅展示首行）：

- L22: `AHodgeSurvivorHero::AHodgeSurvivorHero()`
- L49: `void AHodgeSurvivorHero::SetupPlayerInputComponent(UInputComponent* Input)`
- L70: `void AHodgeSurvivorHero::Forward(const FInputActionValue& Value)`
- L75: `void AHodgeSurvivorHero::Right(const FInputActionValue& Value)`
- L80: `void AHodgeSurvivorHero::Dash()`
- L89: `void AHodgeSurvivorHero::Tick(float Dt)`
- L106: `void AHodgeSurvivorHero::EndPlay(const EEndPlayReason::Type Reason)`
- L114: `AHodgeSurvivorMode::AHodgeSurvivorMode()`
- L124: `void AHodgeSurvivorMode::StartPlay()`
- L137: `AActor* AHodgeSurvivorMode::MakeShape(FVector Location, FVector Scale, UMaterialInterface* Material, float Life)`
- L151: `void AHodgeSurvivorMode::SpawnEnemy(bool Elite)`
- L172: `void AHodgeSurvivorMode::HitEnemy(int32 Index, float Amount)`
- L188: `void AHodgeSurvivorMode::DamageHero(float Amount)`
- L195: `void AHodgeSurvivorMode::GrantExperience(int32 Amount)`
- L207: `FString AHodgeSurvivorMode::GetUpgradeText(int32 Slot) const`
- L212: `void AHodgeSurvivorMode::ChooseUpgrade(int32 Slot)`
- L226: `void AHodgeSurvivorMode::SetRunState(EHodgeSurvivorState NewState)`
- L246: `void AHodgeSurvivorMode::TogglePause()`
- L251: `void AHodgeSurvivorMode::RestartRun()`
- L255: `void AHodgeSurvivorMode::ReturnToMenu()`
- L259: `void AHodgeSurvivorMode::Tick(float Dt)`
- L342: `void AHodgeSurvivorMode::EndPlay(const EEndPlayReason::Type Reason)`

## HodgeSurvivorHUD.cpp

CodexText 实验：代码构建的 Survivor HUD UserWidget。

源码：[Source/Hodgepodge/Private/CodexText/HodgeSurvivorHUD.cpp](../../../Source/Hodgepodge/Private/CodexText/HodgeSurvivorHUD.cpp)

项目内直接 include（不是运行调用关系）：[CodexText/HodgeSurvivor.h](../../../Source/Hodgepodge/Public/CodexText/HodgeSurvivor.h)

定义候选（多行签名仅展示首行）：

- L27: `AHodgeSurvivorMode* UHodgeSurvivorHUD::Mode() const { return GetWorld()?GetWorld()->GetAuthGameMode<AHodgeSurvivorMode>():nullptr; }`
- L28: `void UHodgeSurvivorHUD::Text(FName Name,const FString& Value)`
- L32: `UButton* UHodgeSurvivorHUD::AddButton(UVerticalBox* Box,const FString& Label,UTextBlock*& OutLabel)`
- L46: `void UHodgeSurvivorHUD::NativeConstruct()`
- L116: `void UHodgeSurvivorHUD::Refresh(AHodgeSurvivorMode* M)`
- L155: `void UHodgeSurvivorHUD::NativeDestruct()`
- L163: `void UHodgeSurvivorHUD::ChooseOne() { if (auto* M=Mode()) M->ChooseUpgrade(0); }`
- L164: `void UHodgeSurvivorHUD::ChooseTwo() { if (auto* M=Mode()) M->ChooseUpgrade(1); }`
- L165: `void UHodgeSurvivorHUD::ChooseThree() { if (auto* M=Mode()) M->ChooseUpgrade(2); }`
- L166: `void UHodgeSurvivorHUD::Retry() { if (auto* M=Mode()) M->RestartRun(); }`
- L167: `void UHodgeSurvivorHUD::Menu() { if (auto* M=Mode()) M->ReturnToMenu(); }`
- L168: `void UHodgeSurvivorHUD::Pause() { if (auto* M=Mode()) M->TogglePause(); }`

## HodgeALSLocomotion.h

CodexText 实验：6 向地面运动动画实例，不属于主 Hero 动画链。

源码：[Source/Hodgepodge/Public/CodexText/HodgeALSLocomotion.h](../../../Source/Hodgepodge/Public/CodexText/HodgeALSLocomotion.h)

项目内直接 include（不是运行调用关系）：[Animation/HodgeAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimInstance.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   3: #include "CoreMinimal.h"
   4: #include "Animation/HodgeAnimInstance.h"
   5: #include "Kismet/BlueprintFunctionLibrary.h"
   6: #include "HodgeALSLocomotion.generated.h"
   8: UENUM(BlueprintType)
   9: enum class EHodgeALSDirection : uint8 { Forward, Backward, LeftForward, LeftBackward, RightForward, RightBackward };
  12: UCLASS()
  13: class HODGEPODGE_API UHodgeALSLocomotion : public UHodgeAnimInstance
  14: {
  15: 	GENERATED_BODY()
  17: public:
  18: 	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
  19: 	UFUNCTION(BlueprintPure, Category="ALS Lab")
  20: 	FName GetLabActiveState();
  21: 	UFUNCTION(BlueprintCallable, Category="ALS Lab")
  22: 	void SetHipBias(float Bias) { HipOrientationBias = FMath::Clamp(Bias, -1.f, 1.f); }
  24: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  25: 	EHodgeALSDirection LabDirection = EHodgeALSDirection::Forward;
  26: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  27: 	float LabForward = 1.f;
  28: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  29: 	float LabBackward = 0.f;
  30: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  31: 	float LabLeft = 0.f;
  32: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  33: 	float LabRight = 0.f;
  34: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  35: 	float LabGait = 0.f;
  36: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  37: 	float LabPlayRate = 1.f;
  38: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  39: 	float LabSpeed = 0.f;
  40: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  41: 	float LabViewAngle = 0.f;
  42: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  43: 	float LabYawOffset = 0.f;
  44: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  45: 	float LabFeetCrossing = 0.f;
  46: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALS Lab", meta=(ClampMin="-1", ClampMax="1"))
  47: 	float HipOrientationBias = 0.f;
  48: 	UPROPERTY(EditDefaultsOnly, Category="ALS Lab")
  49: 	TObjectPtr<class UCurveFloat> ForwardBackwardYawCurve;
  50: 	UPROPERTY(EditDefaultsOnly, Category="ALS Lab")
  51: 	TObjectPtr<class UCurveFloat> LeftYawCurve;
  52: 	UPROPERTY(EditDefaultsOnly, Category="ALS Lab")
  53: 	TObjectPtr<class UCurveFloat> RightYawCurve;
  54: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  55: 	bool bLabIdle = true;
  56: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  57: 	bool bLabFall = false;
  58: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  59: 	bool bLabF = false;
  60: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  61: 	bool bLabB = false;
  62: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  63: 	bool bLabLF = false;
  64: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  65: 	bool bLabLB = false;
  66: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  67: 	bool bLabRF = false;
  68: 	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
  69: 	bool bLabRB = false;
  71: private:
  72: 	float DirectionHoldRemaining = 0.f;
  73: };
  75: UCLASS()
  76: class HODGEPODGE_API UHodgeALSAuthoring : public UBlueprintFunctionLibrary
  77: {
  78: 	GENERATED_BODY()
  80: public:
  82: 	UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
  83: 	static bool BuildLocomotionGraph(UObject* Blueprint, const TArray<UObject*>& WalkRunAnimations, UObject* Idle,
  84: 	                                 UObject* Fall);
  85: };
```

## HodgeGroundedLocomotion.h

CodexText 实验：在 ALS 基础动画上增加平地起停/转身/脚锁。

源码：[Source/Hodgepodge/Public/CodexText/HodgeGroundedLocomotion.h](../../../Source/Hodgepodge/Public/CodexText/HodgeGroundedLocomotion.h)

项目内直接 include（不是运行调用关系）：[CodexText/HodgeALSLocomotion.h](../../../Source/Hodgepodge/Public/CodexText/HodgeALSLocomotion.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   2: #include "CoreMinimal.h"
   3: #include "CodexText/HodgeALSLocomotion.h"
   4: #include "HodgeGroundedLocomotion.generated.h"
   7: UCLASS()
   8: class HODGEPODGE_API UHodgeGroundedLocomotion : public UHodgeALSLocomotion
   9: {
  10: 	GENERATED_BODY()
  12: public:
  13: 	virtual void NativeInitializeAnimation() override;
  14: 	virtual void NativeUpdateAnimation(float Dt) override;
  15: 	virtual void NativePostEvaluateAnimation() override;
  16: 	UFUNCTION(BlueprintPure)
  17: 	FName GetGroundedState();
  18: 	UFUNCTION(BlueprintPure)
  19: 	float GetGroundedRotationTarget() const { return RotationTarget; }
  21: 	UFUNCTION(BlueprintPure)
  22: 	bool IsTurningInPlace() const { return bGroundTurnL || bGroundTurnR; }
  24: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  25: 	bool bGroundIdle = true;
  26: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  27: 	bool bGroundMove = false;
  28: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  29: 	bool bGroundStopWalkL = false;
  30: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  31: 	bool bGroundStopWalkR = false;
  32: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  33: 	bool bGroundStopRunL = false;
  34: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  35: 	bool bGroundStopRunR = false;
  36: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  37: 	bool bGroundTurnL = false;
  38: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  39: 	bool bGroundTurnR = false;
  40: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  41: 	float GroundActionTime = 0.f;
  42: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  43: 	float FootAlphaL = 0.f;
  44: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  45: 	float FootAlphaR = 0.f;
  46: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  47: 	FVector FootTargetL = FVector::ZeroVector;
  48: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  49: 	FVector FootTargetR = FVector::ZeroVector;
  50: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  51: 	FRotator FootRotationL = FRotator::ZeroRotator;
  52: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  53: 	FRotator FootRotationR = FRotator::ZeroRotator;
  54: 	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
  55: 	bool bSupportLeft = true;
  56: 	UPROPERTY(EditDefaultsOnly, Category="Grounded")
  57: 	TArray<TObjectPtr<class UAnimSequence>> GroundActions;
  58: 	UPROPERTY(EditDefaultsOnly, Category="Grounded")
  59: 	TObjectPtr<class UCurveFloat> TurnYawLeft;
  60: 	UPROPERTY(EditDefaultsOnly, Category="Grounded")
  61: 	TObjectPtr<class UCurveFloat> TurnYawRight;
  62: 	UPROPERTY(EditDefaultsOnly, Category="Grounded")
  63: 	float TurnThreshold = 70.f;
  65: private:
  66: 	FTransform PreviousRawFeet[2];
  67: 	FTransform LockedFeet[2];
  68: 	bool bHavePose = false;
  69: 	bool bHadMovement = false;
  70: 	bool bLastWalk = false;
  71: 	bool bLocked[2] = {false, false};
  72: 	float PreviousLockCurve[2] = {0.f, 0.f};
  73: 	float RotationTarget = 0.f;
  74: 	float TurnStartYaw = 0.f;
  75: 	float TurnDelay = 0.f;
  76: 	int32 ActionIndex = INDEX_NONE;
  77: 	FVector PreviousActorLocation = FVector::ZeroVector;
  78: };
  80: UCLASS()
  81: class HODGEPODGE_API UHodgeGroundedAuthoring : public UBlueprintFunctionLibrary
  82: {
  83: 	GENERATED_BODY()
  85: public:
  86: 	UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
  87: 	static bool AddGroundedLayer(UObject* Blueprint, const TArray<UObject*>& Actions, UObject* Idle, UObject* Fall,
  88: 	                             UObject* PreviewMesh);
  89: };
```

## HodgeLocomotionLab.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/CodexText/HodgeLocomotionLab.h](../../../Source/Hodgepodge/Public/CodexText/HodgeLocomotionLab.h)

项目内直接 include（不是运行调用关系）：[Core/GameMode/HodgeGameModeBase.h](../../../Source/Hodgepodge/Public/Core/GameMode/HodgeGameModeBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   3: #include "CoreMinimal.h"
   4: #include "Components/ActorComponent.h"
   5: #include "Core/GameMode/HodgeGameModeBase.h"
   6: #include "Kismet/BlueprintFunctionLibrary.h"
   7: #include "HodgeLocomotionLab.generated.h"
  10: UCLASS(Blueprintable, ClassGroup=(Hodge), meta=(BlueprintSpawnableComponent))
  11: class HODGEPODGE_API UHodgeLocomotionLabComponent : public UActorComponent
  12: {
  13:     GENERATED_BODY()
  14: public:
  15:     UHodgeLocomotionLabComponent();
  16:     virtual void BeginPlay() override;
  17:     virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
  18:     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Locomotion") float WalkSpeed = 180.f;
  19:     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Locomotion") float RunSpeed = 420.f;
  20:     UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Locomotion") bool bCombatFacing = true;
  21:     UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Locomotion") bool bWalking = false;
  22:     UFUNCTION(BlueprintCallable) void SetCombatFacing(bool bEnabled);
  23:     UFUNCTION(BlueprintCallable) void SetWalking(bool bEnabled);
  24:     UFUNCTION(BlueprintCallable) void SetKeyboardControlsEnabled(bool bEnabled) { bKeyboardControlsEnabled = bEnabled; }
  25: private:
  26:     bool bKeyboardControlsEnabled = true;
  27: };
  30: UCLASS()
  31: class HODGEPODGE_API AHodgeLocomotionLabMode : public AHodgeGameModeBase
  32: {
  33:     GENERATED_BODY()
  34: public:
  35:     virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
  36: };
  39: UCLASS()
  40: class HODGEPODGE_API UHodgeLocomotionLabAuthoring : public UBlueprintFunctionLibrary
  41: {
  42:     GENERATED_BODY()
  43: public:
  44:     UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
  45:     static bool RemapCopy(UObject* Asset, const TArray<UObject*>& Sources, const TArray<UObject*>& Copies);
  46:     UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
  47:     static bool ConfigureGroundBlend(UObject* Blueprint, UObject* BlendSpace);
  48: };
```

## HodgeSurvivor.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/CodexText/HodgeSurvivor.h](../../../Source/Hodgepodge/Public/CodexText/HodgeSurvivor.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   3: #include "CoreMinimal.h"
   4: #include "GameFramework/Character.h"
   5: #include "GameFramework/GameModeBase.h"
   6: #include "Blueprint/UserWidget.h"
   7: #include "HodgeSurvivor.generated.h"
   9: class UInputAction;
  10: class UInputMappingContext;
  11: struct FInputActionValue;
  12: class UAnimationAsset;
  13: class UMaterialInterface;
  14: class UStaticMesh;
  15: class UCanvasPanel;
  16: class UVerticalBox;
  17: class UTextBlock;
  18: class UButton;
  19: class UProgressBar;
  21: UCLASS()
  22: class HODGEPODGE_API AHodgeSurvivorHero : public ACharacter
  23: {
  24:     GENERATED_BODY()
  25: public:
  26:     AHodgeSurvivorHero();
  27:     virtual void Tick(float DeltaSeconds) override;
  28:     virtual void SetupPlayerInputComponent(UInputComponent* Input) override;
  29:     virtual void EndPlay(const EEndPlayReason::Type Reason) override;
  30:     UFUNCTION(BlueprintCallable) void Dash();
  31:     UPROPERTY(BlueprintReadOnly) float DashCooldown = 0;
  32:     UPROPERTY(BlueprintReadOnly) float Invulnerability = 0;
  33: private:
  34:     void Forward(const FInputActionValue& Value);
  35:     void Right(const FInputActionValue& Value);
  36:     UPROPERTY() TObjectPtr<UInputMappingContext> Mapping;
  37:     UPROPERTY() TObjectPtr<UInputAction> ForwardAction;
  38:     UPROPERTY() TObjectPtr<UInputAction> RightAction;
  39:     UPROPERTY() TObjectPtr<UAnimationAsset> Idle;
  40:     UPROPERTY() TObjectPtr<UAnimationAsset> Run;
  41:     bool bRunning = false;
  42: };
  44: UENUM(BlueprintType)
  45: enum class EHodgeSurvivorState : uint8 { Playing, Upgrade, Paused, Defeat, Victory };
  47: struct FHodgeSurvivorEnemy
  48: {
  49:     TWeakObjectPtr<AActor> Actor;
  50:     float Health = 30;
  51:     float Speed = 130;
  52:     bool bElite = false;
  53: };
  55: struct FHodgeSurvivorOrb
  56: {
  57:     TWeakObjectPtr<AActor> Actor;
  58:     int32 Value = 1;
  59: };
  61: UCLASS()
  62: class HODGEPODGE_API AHodgeSurvivorMode : public AGameModeBase
  63: {
  64:     GENERATED_BODY()
  65: public:
  66:     AHodgeSurvivorMode();
  67:     virtual void StartPlay() override;
  68:     virtual void Tick(float DeltaSeconds) override;
  69:     virtual void EndPlay(const EEndPlayReason::Type Reason) override;
  70:     UFUNCTION(BlueprintCallable) void ChooseUpgrade(int32 Slot);
  71:     UFUNCTION(BlueprintCallable) void TogglePause();
  72:     UFUNCTION(BlueprintCallable) void RestartRun();
  73:     UFUNCTION(BlueprintCallable) void ReturnToMenu();
  74:     UFUNCTION(BlueprintCallable) void GrantExperience(int32 Amount);
  75:     UFUNCTION(BlueprintCallable) void DamageHero(float Amount);
  76:     UFUNCTION(BlueprintCallable) void SpawnEnemy(bool bElite = false);
  77:     UFUNCTION(BlueprintPure) int32 GetEnemyCount() const { return Enemies.Num(); }
  78:     UFUNCTION(BlueprintPure) int32 GetOrbCount() const { return Orbs.Num(); }
  79:     UFUNCTION(BlueprintPure) FString GetUpgradeText(int32 Slot) const;
  80:     UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Survivor") float RoundDuration = 180;
  81:     UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Survivor") float SpawnInterval = 1.15f;
  82:     UPROPERTY(EditDefaultsOnly, Category="Survivor") TSubclassOf<UUserWidget> InterfaceClass;
  83:     UPROPERTY(EditDefaultsOnly, Category="Survivor") TObjectPtr<UMaterialInterface> EnemyMaterial;
  84:     UPROPERTY(EditDefaultsOnly, Category="Survivor") TObjectPtr<UMaterialInterface> EliteMaterial;
  85:     UPROPERTY(EditDefaultsOnly, Category="Survivor") TObjectPtr<UMaterialInterface> EnergyMaterial;
  86:     UPROPERTY(BlueprintReadOnly) EHodgeSurvivorState State = EHodgeSurvivorState::Playing;
  87:     UPROPERTY(BlueprintReadOnly) float Health = 100;
  88:     UPROPERTY(BlueprintReadOnly) float MaxHealth = 100;
  89:     UPROPERTY(BlueprintReadOnly) float Elapsed = 0;
  90:     UPROPERTY(BlueprintReadOnly) int32 Kills = 0;
  91:     UPROPERTY(BlueprintReadOnly) int32 Level = 1;
  92:     UPROPERTY(BlueprintReadOnly) int32 Experience = 0;
  93:     UPROPERTY(BlueprintReadOnly) int32 NextLevelXP = 5;
  94:     UPROPERTY(BlueprintReadOnly) float Damage = 22;
  95:     UPROPERTY(BlueprintReadOnly) float AttackInterval = .65f;
  96:     UPROPERTY(BlueprintReadOnly) float PickupRadius = 320;
  97:     UPROPERTY(BlueprintReadOnly) int32 PulseLevel = 0;
  98:     UPROPERTY(BlueprintReadOnly) TObjectPtr<AHodgeSurvivorHero> Hero;
  99:     UPROPERTY(BlueprintReadOnly) TObjectPtr<UUserWidget> Interface;
 100:     TArray<int32> Choices;
 101: private:
 102:     void SetRunState(EHodgeSurvivorState NewState);
 103:     void HitEnemy(int32 Index, float Amount);
 104:     AActor* MakeShape(FVector Location, FVector Scale, UMaterialInterface* Material, float Life = 0);
 105:     UPROPERTY() TObjectPtr<UStaticMesh> Sphere;
 106:     UPROPERTY() TObjectPtr<USkeletalMesh> EnemyMesh;
 107:     UPROPERTY() TObjectPtr<UAnimationAsset> EnemyRun;
 108:     TArray<FHodgeSurvivorEnemy> Enemies;
 109:     TArray<FHodgeSurvivorOrb> Orbs;
 110:     float SpawnClock = 0;
 111:     float AttackClock = 0;
 112:     float HurtClock = 0;
 113:     float PulseClock = 0;
 114:     float InterfaceClock = 0;
 115:     int32 NextElite = 35;
 116: };
 118: UCLASS()
 119: class HODGEPODGE_API UHodgeSurvivorHUD : public UUserWidget
 120: {
 121:     GENERATED_BODY()
 122: public:
 123:     virtual void NativeConstruct() override;
 124:     virtual void NativeDestruct() override;
 125:     void Refresh(AHodgeSurvivorMode* Mode);
 126:     UFUNCTION() void ChooseOne();
 127:     UFUNCTION() void ChooseTwo();
 128:     UFUNCTION() void ChooseThree();
 129:     UFUNCTION() void Retry();
 130:     UFUNCTION() void Menu();
 131:     UFUNCTION() void Pause();
 132: private:
 133:     AHodgeSurvivorMode* Mode() const;
 134:     void Text(FName Name, const FString& Value);
 135:     UButton* AddButton(UVerticalBox* Box, const FString& Label, UTextBlock*& OutLabel);
 136:     UPROPERTY() TObjectPtr<UCanvasPanel> Modal;
 137:     UPROPERTY() TObjectPtr<UTextBlock> ModalTitle;
 138:     UPROPERTY() TObjectPtr<UTextBlock> ModalDescription;
 139:     UPROPERTY() TArray<TObjectPtr<UButton>> ChoiceButtons;
 140:     UPROPERTY() TArray<TObjectPtr<UTextBlock>> ChoiceLabels;
 141:     UPROPERTY() TObjectPtr<UButton> RetryButton;
 142:     UPROPERTY() TObjectPtr<UButton> MenuButton;
 143:     UPROPERTY() TObjectPtr<UButton> PauseButton;
 144:     UPROPERTY() TObjectPtr<UProgressBar> XPBar;
 145:     UPROPERTY() TObjectPtr<UTextBlock> XPText;
 146:     UPROPERTY() TObjectPtr<UTextBlock> PauseLabel;
 147: };
```
