# Camera 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeCameraComponent.cpp

相机模式栈宿主与最终视图输出，依赖默认模式委托。

源码：[Source/Hodgepodge/Private/Camera/HodgeCameraComponent.cpp](../../../Source/Hodgepodge/Private/Camera/HodgeCameraComponent.cpp)

项目内直接 include（不是运行调用关系）：[Camera/HodgeCameraComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraComponent.h)、[Camera/HodgeCameraMode.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode.h)

定义候选（多行签名仅展示首行）：

- L12: `UHodgeCameraComponent::UHodgeCameraComponent(const FObjectInitializer& ObjectInitializer)`
- L22: `void UHodgeCameraComponent::OnRegister()`
- L36: `void UHodgeCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)`
- L111: `void UHodgeCameraComponent::UpdateCameraModes()`
- L132: `void UHodgeCameraComponent::DrawDebug(UCanvas* Canvas) const`
- L168: `void UHodgeCameraComponent::GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const`

## HodgeCameraMode.cpp

相机视图、模式实例、混合和模式栈。

源码：[Source/Hodgepodge/Private/Camera/HodgeCameraMode.cpp](../../../Source/Hodgepodge/Private/Camera/HodgeCameraMode.cpp)

项目内直接 include（不是运行调用关系）：[Camera/HodgeCameraMode.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode.h)、[Camera/HodgeCameraComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraComponent.h)、[Camera/HodgePlayerCameraManager.h](../../../Source/Hodgepodge/Public/Camera/HodgePlayerCameraManager.h)

定义候选（多行签名仅展示首行）：

- L20: `FHodgeCameraModeView::FHodgeCameraModeView()`
- L29: `void FHodgeCameraModeView::Blend(const FHodgeCameraModeView& Other, float OtherWeight)`
- L68: `UHodgeCameraMode::UHodgeCameraMode()`
- L96: `UHodgeCameraComponent* UHodgeCameraMode::GetHodgeCameraComponent() const`
- L103: `UWorld* UHodgeCameraMode::GetWorld() const`
- L110: `AActor* UHodgeCameraMode::GetTargetActor() const`
- L120: `FVector UHodgeCameraMode::GetPivotLocation() const`
- L166: `FRotator UHodgeCameraMode::GetPivotRotation() const`
- L184: `void UHodgeCameraMode::UpdateCameraMode(float DeltaTime)`
- L194: `void UHodgeCameraMode::UpdateView(float DeltaTime)`
- L219: `void UHodgeCameraMode::SetBlendWeight(float Weight)`
- L258: `void UHodgeCameraMode::UpdateBlending(float DeltaTime)`
- L309: `void UHodgeCameraMode::DrawDebug(UCanvas* Canvas) const`
- L330: `UHodgeCameraModeStack::UHodgeCameraModeStack()`
- L337: `void UHodgeCameraModeStack::ActivateStack()`
- L358: `void UHodgeCameraModeStack::DeactivateStack()`
- L379: `void UHodgeCameraModeStack::PushCameraMode(TSubclassOf<UHodgeCameraMode> CameraModeClass)`
- L467: `bool UHodgeCameraModeStack::EvaluateStack(float DeltaTime, FHodgeCameraModeView& OutCameraModeView)`
- L486: `UHodgeCameraMode* UHodgeCameraModeStack::GetCameraModeInstance(TSubclassOf<UHodgeCameraMode> CameraModeClass)`
- L513: `void UHodgeCameraModeStack::UpdateStack(float DeltaTime)`
- L572: `void UHodgeCameraModeStack::BlendStack(FHodgeCameraModeView& OutCameraModeView) const`
- L603: `void UHodgeCameraModeStack::DrawDebug(UCanvas* Canvas) const`
- L631: `void UHodgeCameraModeStack::GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const`

## HodgeCameraMode_ThirdPerson.cpp

第三人称偏移与防穿透逻辑。

源码：[Source/Hodgepodge/Private/Camera/HodgeCameraMode_ThirdPerson.cpp](../../../Source/Hodgepodge/Private/Camera/HodgeCameraMode_ThirdPerson.cpp)

项目内直接 include（不是运行调用关系）：[Camera/HodgeCameraMode_ThirdPerson.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode_ThirdPerson.h)、[Camera/HodgeCameraAssistInterface.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraAssistInterface.h)、[Camera/HodgePenetrationAvoidanceFeeler.h](../../../Source/Hodgepodge/Public/Camera/HodgePenetrationAvoidanceFeeler.h)

定义候选（多行签名仅展示首行）：

- L20: `UHodgeCameraMode_ThirdPerson::UHodgeCameraMode_ThirdPerson()`
- L54: `void UHodgeCameraMode_ThirdPerson::UpdateView(float DeltaTime)`
- L109: `void UHodgeCameraMode_ThirdPerson::UpdateForTarget(float DeltaTime)`
- L135: `void UHodgeCameraMode_ThirdPerson::DrawDebug(UCanvas* Canvas) const`
- L158: `void UHodgeCameraMode_ThirdPerson::UpdatePreventPenetration(float DeltaTime)`
- L249: `void UHodgeCameraMode_ThirdPerson::PreventCameraPenetration(class AActor const& ViewTarget, FVector const& SafeLoc,`
- L511: `void UHodgeCameraMode_ThirdPerson::SetTargetCrouchOffset(FVector NewTargetOffset)`
- L523: `void UHodgeCameraMode_ThirdPerson::UpdateCrouchOffset(float DeltaTime)`

## HodgePlayerCameraManager.cpp

项目相机管理器；实际控制器类选择需验证。

源码：[Source/Hodgepodge/Private/Camera/HodgePlayerCameraManager.cpp](../../../Source/Hodgepodge/Private/Camera/HodgePlayerCameraManager.cpp)

项目内直接 include（不是运行调用关系）：[Camera/HodgePlayerCameraManager.h](../../../Source/Hodgepodge/Public/Camera/HodgePlayerCameraManager.h)、[Camera/HodgeCameraComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraComponent.h)、[Camera/HodgeUICameraManagerComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeUICameraManagerComponent.h)

定义候选（多行签名仅展示首行）：

- L17: `AHodgePlayerCameraManager::AHodgePlayerCameraManager(const FObjectInitializer& ObjectInitializer)`
- L33: `UHodgeUICameraManagerComponent* AHodgePlayerCameraManager::GetUICameraComponent() const`
- L39: `void AHodgePlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)`
- L58: `void AHodgePlayerCameraManager::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL,`

## HodgeUICameraManagerComponent.cpp

UI 相机管理扩展，不代表 UI 系统已接入。

源码：[Source/Hodgepodge/Private/Camera/HodgeUICameraManagerComponent.cpp](../../../Source/Hodgepodge/Private/Camera/HodgeUICameraManagerComponent.cpp)

项目内直接 include（不是运行调用关系）：[Camera/HodgeUICameraManagerComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeUICameraManagerComponent.h)、[Camera/HodgePlayerCameraManager.h](../../../Source/Hodgepodge/Public/Camera/HodgePlayerCameraManager.h)

定义候选（多行签名仅展示首行）：

- L15: `UHodgeUICameraManagerComponent* UHodgeUICameraManagerComponent::GetComponent(APlayerController* PC)`
- L31: `UHodgeUICameraManagerComponent::UHodgeUICameraManagerComponent()`
- L48: `void UHodgeUICameraManagerComponent::InitializeComponent()`
- L55: `void UHodgeUICameraManagerComponent::SetViewTarget(AActor* InViewTarget, FViewTargetTransitionParams TransitionParams)`
- L68: `bool UHodgeUICameraManagerComponent::NeedsToUpdateViewTarget() const`
- L75: `void UHodgeUICameraManagerComponent::UpdateViewTarget(struct FTViewTarget& OutVT, float DeltaTime)`
- L81: `void UHodgeUICameraManagerComponent::OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo,`

## HodgeCameraAssistInterface.h

相机辅助接口契约。

源码：[Source/Hodgepodge/Public/Camera/HodgeCameraAssistInterface.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraAssistInterface.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CoreMinimal.h"
   6: #include "UObject/Interface.h"
   8: #include "HodgeCameraAssistInterface.generated.h"
  11: UINTERFACE(BlueprintType)
  12: class UHodgeCameraAssistInterface : public UInterface
  13: {
  14: 	GENERATED_BODY()
  15: };
  18: class IHodgeCameraAssistInterface
  19: {
  20: 	GENERATED_BODY()
  22: public:
  24: 	virtual void GetIgnoredActorsForCameraPentration(TArray<const AActor*>& OutActorsAllowPenetration) const
  25: 	{
  26: 	}
  29: 	virtual TOptional<AActor*> GetCameraPreventPenetrationTarget() const
  30: 	{
  32: 		return TOptional<AActor*>();
  33: 	}
  36: 	virtual void OnCameraPenetratingTarget()
  37: 	{
  38: 	}
  39: };
```

## HodgeCameraComponent.h

相机模式栈宿主与最终视图输出，依赖默认模式委托。

源码：[Source/Hodgepodge/Public/Camera/HodgeCameraComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraComponent.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CoreMinimal.h"
   6: #include "Camera/CameraComponent.h"
   7: #include "HodgeCameraComponent.generated.h"
  10: class UCanvas;
  11: class UHodgeCameraMode;
  12: class UHodgeCameraModeStack;
  13: class UObject;
  14: struct FFrame;
  15: struct FGameplayTag;
  16: struct FMinimalViewInfo;
  17: template <class TClass>
  18: class TSubclassOf;
  21: DECLARE_DELEGATE_RetVal(TSubclassOf<UHodgeCameraMode>, FHodgeCameraModeDelegate);
  29: UCLASS()
  30: class UHodgeCameraComponent : public UCameraComponent
  31: {
  32: 	GENERATED_BODY()
  34: public:
  35: 	UHodgeCameraComponent(const FObjectInitializer& ObjectInitializer);
  38: 	UFUNCTION(BlueprintPure, Category = "Hodge|Camera")
  39: 	static UHodgeCameraComponent* FindCameraComponent(const AActor* Actor)
  40: 	{
  41: 		return (Actor ? Actor->FindComponentByClass<UHodgeCameraComponent>() : nullptr);
  42: 	}
  45: 	virtual AActor* GetTargetActor() const { return GetOwner(); }
  48: 	FHodgeCameraModeDelegate DetermineCameraModeDelegate;
  51: 	void AddFieldOfViewOffset(float FovOffset) { FieldOfViewOffset += FovOffset; }
  54: 	virtual void DrawDebug(UCanvas* Canvas) const;
  57: 	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;
  59: protected:
  61: 	virtual void OnRegister() override;
  64: 	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;
  67: 	virtual void UpdateCameraModes();
  69: protected:
  71: 	UPROPERTY()
  72: 	TObjectPtr<UHodgeCameraModeStack> CameraModeStack;
  75: 	float FieldOfViewOffset;
  76: };
```

## HodgeCameraMode.h

相机视图、模式实例、混合和模式栈。

源码：[Source/Hodgepodge/Public/Camera/HodgeCameraMode.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CoreMinimal.h"
   6: #include "GameplayTagContainer.h"
   7: #include "UObject/Object.h"
   8: #include "HodgeCameraMode.generated.h"
  10: class AActor;
  11: class UCanvas;
  12: class UHodgeCameraComponent;
  17: UENUM(BlueprintType)
  18: enum class EHodgeCameraModeBlendFunction : uint8
  19: {
  21: 	Linear,
  24: 	EaseIn,
  27: 	EaseOut,
  30: 	EaseInOut,
  33: 	COUNT UMETA(Hidden)
  34: };
  40: struct FHodgeCameraModeView
  41: {
  42: public:
  44: 	FHodgeCameraModeView();
  47: 	void Blend(const FHodgeCameraModeView& Other, float OtherWeight);
  49: public:
  51: 	FVector Location;
  54: 	FRotator Rotation;
  57: 	FRotator ControlRotation;
  60: 	float FieldOfView;
  61: };
  67: UCLASS(Abstract, NotBlueprintable)
  68: class HODGEPODGE_API UHodgeCameraMode : public UObject
  69: {
  70: 	GENERATED_BODY()
  72: public:
  74: 	UHodgeCameraMode();
  77: 	UHodgeCameraComponent* GetHodgeCameraComponent() const;
  80: 	virtual UWorld* GetWorld() const override;
  83: 	AActor* GetTargetActor() const;
  86: 	const FHodgeCameraModeView& GetCameraModeView() const { return View; }
  89: 	virtual void OnActivation()
  90: 	{
  91: 	};
  94: 	virtual void OnDeactivation()
  95: 	{
  96: 	};
  99: 	void UpdateCameraMode(float DeltaTime);
 102: 	float GetBlendTime() const { return BlendTime; }
 105: 	float GetBlendWeight() const { return BlendWeight; }
 108: 	void SetBlendWeight(float Weight);
 111: 	FGameplayTag GetCameraTypeTag() const
 112: 	{
 113: 		return CameraTypeTag;
 114: 	}
 117: 	virtual void DrawDebug(UCanvas* Canvas) const;
 119: protected:
 121: 	virtual FVector GetPivotLocation() const;
 124: 	virtual FRotator GetPivotRotation() const;
 127: 	virtual void UpdateView(float DeltaTime);
 130: 	virtual void UpdateBlending(float DeltaTime);
 132: protected:
 134: 	UPROPERTY(EditDefaultsOnly, Category = "Blending")
 135: 	FGameplayTag CameraTypeTag;
 138: 	FHodgeCameraModeView View;
 141: 	UPROPERTY(EditDefaultsOnly, Category = "View",
 142: 		Meta = (UIMin = "5.0", UIMax = "170", ClampMin = "5.0", ClampMax = "170.0"))
 143: 	float FieldOfView;
 146: 	UPROPERTY(EditDefaultsOnly, Category = "View",
 147: 		Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
 148: 	float ViewPitchMin;
 151: 	UPROPERTY(EditDefaultsOnly, Category = "View",
 152: 		Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
 153: 	float ViewPitchMax;
 156: 	UPROPERTY(EditDefaultsOnly, Category = "Blending")
 157: 	float BlendTime;
 160: 	UPROPERTY(EditDefaultsOnly, Category = "Blending")
 161: 	EHodgeCameraModeBlendFunction BlendFunction;
 164: 	UPROPERTY(EditDefaultsOnly, Category = "Blending")
 165: 	float BlendExponent;
 168: 	float BlendAlpha;
 171: 	float BlendWeight;
 173: protected:
 175: 	UPROPERTY(transient)
 176: 	uint32 bResetInterpolation : 1;
 177: };
 183: UCLASS()
 184: class UHodgeCameraModeStack : public UObject
 185: {
 186: 	GENERATED_BODY()
 188: public:
 190: 	UHodgeCameraModeStack();
 193: 	void ActivateStack();
 196: 	void DeactivateStack();
 199: 	bool IsStackActivate() const { return bIsActive; }
 202: 	void PushCameraMode(TSubclassOf<UHodgeCameraMode> CameraModeClass);
 205: 	bool EvaluateStack(float DeltaTime, FHodgeCameraModeView& OutCameraModeView);
 208: 	void DrawDebug(UCanvas* Canvas) const;
 211: 	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;
 213: protected:
 215: 	UHodgeCameraMode* GetCameraModeInstance(TSubclassOf<UHodgeCameraMode> CameraModeClass);
 218: 	void UpdateStack(float DeltaTime);
 221: 	void BlendStack(FHodgeCameraModeView& OutCameraModeView) const;
 223: protected:
 225: 	bool bIsActive;
 228: 	UPROPERTY()
 229: 	TArray<TObjectPtr<UHodgeCameraMode>> CameraModeInstances;
 232: 	UPROPERTY()
 233: 	TArray<TObjectPtr<UHodgeCameraMode>> CameraModeStack;
 234: };
```

## HodgeCameraMode_ThirdPerson.h

第三人称偏移与防穿透逻辑。

源码：[Source/Hodgepodge/Public/Camera/HodgeCameraMode_ThirdPerson.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode_ThirdPerson.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CoreMinimal.h"
   6: #include "HodgeCameraMode.h"
   7: #include "HodgeCameraMode_ThirdPerson.generated.h"
  10: struct FHodgePenetrationAvoidanceFeeler;
  11: class UCurveVector;
  16: UCLASS(Abstract, Blueprintable)
  17: class UHodgeCameraMode_ThirdPerson : public UHodgeCameraMode
  18: {
  19: 	GENERATED_BODY()
  21: public:
  22: 	UHodgeCameraMode_ThirdPerson();
  24: protected:
  26: 	virtual void UpdateView(float DeltaTime) override;
  29: 	void UpdateForTarget(float DeltaTime);
  32: 	void UpdatePreventPenetration(float DeltaTime);
  35: 	void PreventCameraPenetration(class AActor const& ViewTarget, FVector const& SafeLoc, FVector& CameraLoc,
  36: 	                              float const& DeltaTime, float& DistBlockedPct, bool bSingleRayOnly);
  39: 	virtual void DrawDebug(UCanvas* Canvas) const override;
  41: protected:
  43: 	UPROPERTY(EditDefaultsOnly, Category = "Third Person", Meta = (EditCondition = "!bUseRuntimeFloatCurves"))
  44: 	TObjectPtr<const UCurveVector> TargetOffsetCurve;
  47: 	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
  48: 	bool bUseRuntimeFloatCurves;
  51: 	UPROPERTY(EditDefaultsOnly, Category = "Third Person", Meta = (EditCondition = "bUseRuntimeFloatCurves"))
  52: 	FRuntimeFloatCurve TargetOffsetX;
  55: 	UPROPERTY(EditDefaultsOnly, Category = "Third Person", Meta = (EditCondition = "bUseRuntimeFloatCurves"))
  56: 	FRuntimeFloatCurve TargetOffsetY;
  59: 	UPROPERTY(EditDefaultsOnly, Category = "Third Person", Meta = (EditCondition = "bUseRuntimeFloatCurves"))
  60: 	FRuntimeFloatCurve TargetOffsetZ;
  63: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Third Person")
  64: 	float CrouchOffsetBlendMultiplier = 5.0f;
  67: public:
  69: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision")
  70: 	float PenetrationBlendInTime = 0.1f;
  73: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision")
  74: 	float PenetrationBlendOutTime = 0.15f;
  77: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision")
  78: 	bool bPreventPenetration = true;
  81: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision")
  82: 	bool bDoPredictiveAvoidance = true;
  85: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
  86: 	float CollisionPushOutDistance = 2.f;
  89: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
  90: 	float ReportPenetrationPercent = 0.f;
  93: 	UPROPERTY(EditDefaultsOnly, Category = "Collision")
  94: 	TArray<FHodgePenetrationAvoidanceFeeler> PenetrationAvoidanceFeelers;
  97: 	UPROPERTY(Transient)
  98: 	float AimLineToDesiredPosBlockedPct;
 101: 	UPROPERTY(Transient)
 102: 	TArray<TObjectPtr<const AActor>> DebugActorsHitDuringCameraPenetration;
 104: #if ENABLE_DRAW_DEBUG
 106: 	mutable float LastDrawDebugTime = -MAX_FLT;
 107: #endif
 109: protected:
 111: 	void SetTargetCrouchOffset(FVector NewTargetOffset);
 114: 	void UpdateCrouchOffset(float DeltaTime);
 117: 	FVector InitialCrouchOffset = FVector::ZeroVector;
 120: 	FVector TargetCrouchOffset = FVector::ZeroVector;
 123: 	float CrouchOffsetBlendPct = 1.0f;
 126: 	FVector CurrentCrouchOffset = FVector::ZeroVector;
 127: };
```

## HodgePenetrationAvoidanceFeeler.h

相机防穿透探测参数结构。

源码：[Source/Hodgepodge/Public/Camera/HodgePenetrationAvoidanceFeeler.h](../../../Source/Hodgepodge/Public/Camera/HodgePenetrationAvoidanceFeeler.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CoreMinimal.h"
   7: #include "HodgePenetrationAvoidanceFeeler.generated.h"
  12: USTRUCT()
  13: struct FHodgePenetrationAvoidanceFeeler
  14: {
  15: 	GENERATED_BODY()
  18: 	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
  19: 	FRotator AdjustmentRot;
  22: 	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
  23: 	float WorldWeight;
  26: 	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
  27: 	float PawnWeight;
  30: 	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
  31: 	float Extent;
  34: 	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
  35: 	int32 TraceInterval;
  38: 	UPROPERTY(transient)
  39: 	int32 FramesUntilNextTrace;
  43: 	FHodgePenetrationAvoidanceFeeler()
  44: 	   : AdjustmentRot(ForceInit)
  45: 	   , WorldWeight(0)
  46: 	   , PawnWeight(0)
  47: 	   , Extent(0)
  48: 	   , TraceInterval(0)
  49: 	   , FramesUntilNextTrace(0)
  50: 	{
  51: 	}
  54: 	FHodgePenetrationAvoidanceFeeler(const FRotator& InAdjustmentRot,
  55: 							const float& InWorldWeight,
  56: 							const float& InPawnWeight,
  57: 							const float& InExtent,
  58: 							const int32& InTraceInterval = 0,
  59: 							const int32& InFramesUntilNextTrace = 0)
  60: 	   : AdjustmentRot(InAdjustmentRot)
  61: 	   , WorldWeight(InWorldWeight)
  62: 	   , PawnWeight(InPawnWeight)
  63: 	   , Extent(InExtent)
  64: 	   , TraceInterval(InTraceInterval)
  65: 	   , FramesUntilNextTrace(InFramesUntilNextTrace)
  66: 	{
  67: 	}
  68: };
```

## HodgePlayerCameraManager.h

项目相机管理器；实际控制器类选择需验证。

源码：[Source/Hodgepodge/Public/Camera/HodgePlayerCameraManager.h](../../../Source/Hodgepodge/Public/Camera/HodgePlayerCameraManager.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CoreMinimal.h"
   6: #include "Camera/PlayerCameraManager.h"
   7: #include "HodgePlayerCameraManager.generated.h"
  10: class FDebugDisplayInfo;
  11: class UCanvas;
  12: class UObject;
  15: #define HODGE_CAMERA_DEFAULT_FOV          (80.0f)
  16: #define HODGE_CAMERA_DEFAULT_PITCH_MIN  (-89.0f)
  17: #define HODGE_CAMERA_DEFAULT_PITCH_MAX  (89.0f)
  19: class UHodgeUICameraManagerComponent;
  26: UCLASS(notplaceable, MinimalAPI)
  27: class AHodgePlayerCameraManager : public APlayerCameraManager
  28: {
  29: 	GENERATED_BODY()
  31: public:
  32: 	AHodgePlayerCameraManager(const FObjectInitializer& ObjectInitializer);
  35: 	UHodgeUICameraManagerComponent* GetUICameraComponent() const;
  37: protected:
  39: 	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
  42: 	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
  44: private:
  46: 	UPROPERTY(Transient)
  47: 	TObjectPtr<UHodgeUICameraManagerComponent> UICamera;
  48: };
```

## HodgeUICameraManagerComponent.h

UI 相机管理扩展，不代表 UI 系统已接入。

源码：[Source/Hodgepodge/Public/Camera/HodgeUICameraManagerComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeUICameraManagerComponent.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "Components/ActorComponent.h"
   8: #include "HodgeUICameraManagerComponent.generated.h"
  10: class AHodgePlayerCameraManager;
  12: class AActor;
  13: class AHUD;
  14: class APlayerController;
  15: class FDebugDisplayInfo;
  16: class UCanvas;
  17: class UObject;
  20: UCLASS(Transient, Within=HodgePlayerCameraManager)
  21: class UHodgeUICameraManagerComponent : public UActorComponent
  22: {
  23: 	GENERATED_BODY()
  25: public:
  27: 	static UHodgeUICameraManagerComponent* GetComponent(APlayerController* PC);
  29: public:
  31: 	UHodgeUICameraManagerComponent();
  34: 	virtual void InitializeComponent() override;
  37: 	bool IsSettingViewTarget() const { return bUpdatingViewTarget; }
  40: 	AActor* GetViewTarget() const { return ViewTarget; }
  43: 	void SetViewTarget(AActor* InViewTarget,
  44: 	                   FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams());
  47: 	bool NeedsToUpdateViewTarget() const;
  50: 	void UpdateViewTarget(struct FTViewTarget& OutVT, float DeltaTime);
  53: 	void OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos);
  55: private:
  57: 	UPROPERTY(Transient)
  58: 	TObjectPtr<AActor> ViewTarget;
  61: 	UPROPERTY(Transient)
  62: 	bool bUpdatingViewTarget;
  63: };
```
