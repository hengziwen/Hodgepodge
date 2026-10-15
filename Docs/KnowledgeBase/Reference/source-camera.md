# Camera 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeCameraComponent.cpp

相机模式栈宿主与最终视图输出，依赖默认模式委托。

源码：[Source/Hodgepodge/Private/Camera/HodgeCameraComponent.cpp](../../../Source/Hodgepodge/Private/Camera/HodgeCameraComponent.cpp)

项目内直接 include（不是运行调用关系）：[Camera/HodgeCameraComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraComponent.h)、[Camera/HodgeCameraMode.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode.h)、[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)、[Data/HodgeCinematicCameraProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeCinematicCameraProfile.h)、[Camera/HodgeCameraMode_Cinematic.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode_Cinematic.h)、[UI/Subsystem/HodgeUIManagerSubsystem.h](../../../Source/Hodgepodge/Public/UI/Subsystem/HodgeUIManagerSubsystem.h)

定义候选（多行签名仅展示首行）：

- L21: `UHodgeCameraComponent::UHodgeCameraComponent(const FObjectInitializer& ObjectInitializer)`
- L32: `void UHodgeCameraComponent::OnRegister()`
- L47: `void UHodgeCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)`
- L133: `TSubclassOf<UHodgeCameraMode> UHodgeCameraComponent::GetActiveCameraModeClass() const`
- L137: `bool UHodgeCameraComponent::GetLastCameraModeView(FHodgeCameraModeView& OutView) const`
- L143: `void UHodgeCameraComponent::QueueCinematicReturn(float Seconds, FRotator GameplayRotation)`
- L149: `bool UHodgeCameraComponent::GetGameplayReturnRotation(FRotator& Out) const`
- L156: `void UHodgeCameraComponent::EndCinematicPresentation(const FHodgeAbilityCameraHandle& Handle)`
- L165: `void UHodgeCameraComponent::UpdateCameraModes()`
- L188: `void UHodgeCameraComponent::DrawDebug(UCanvas* Canvas) const`
- L225: `void UHodgeCameraComponent::GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const`

## HodgeCameraMode.cpp

相机视图、模式实例、混合和模式栈。

源码：[Source/Hodgepodge/Private/Camera/HodgeCameraMode.cpp](../../../Source/Hodgepodge/Private/Camera/HodgeCameraMode.cpp)

项目内直接 include（不是运行调用关系）：[Camera/HodgeCameraMode.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode.h)、[Camera/HodgeCameraComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraComponent.h)、[Camera/HodgePlayerCameraManager.h](../../../Source/Hodgepodge/Public/Camera/HodgePlayerCameraManager.h)

定义候选（多行签名仅展示首行）：

- L23: `FHodgeCameraModeView::FHodgeCameraModeView()`
- L32: `void FHodgeCameraModeView::Blend(const FHodgeCameraModeView& Other, float OtherWeight)`
- L71: `UHodgeCameraMode::UHodgeCameraMode()`
- L99: `UHodgeCameraComponent* UHodgeCameraMode::GetHodgeCameraComponent() const`
- L106: `UWorld* UHodgeCameraMode::GetWorld() const`
- L113: `AActor* UHodgeCameraMode::GetTargetActor() const`
- L123: `FVector UHodgeCameraMode::GetPivotLocation() const`
- L169: `FRotator UHodgeCameraMode::GetPivotRotation() const`
- L187: `void UHodgeCameraMode::UpdateCameraMode(float DeltaTime)`
- L197: `void UHodgeCameraMode::UpdateView(float DeltaTime)`
- L222: `void UHodgeCameraMode::SetBlendWeight(float Weight)`
- L261: `void UHodgeCameraMode::UpdateBlending(float DeltaTime)`
- L312: `void UHodgeCameraMode::DrawDebug(UCanvas* Canvas) const`
- L333: `UHodgeCameraModeStack::UHodgeCameraModeStack()`
- L340: `void UHodgeCameraModeStack::ActivateStack()`
- L361: `void UHodgeCameraModeStack::DeactivateStack()`
- L382: `void UHodgeCameraModeStack::PushCameraMode(TSubclassOf<UHodgeCameraMode> CameraModeClass, float TransitionSeconds)`
- L472: `bool UHodgeCameraModeStack::EvaluateStack(float DeltaTime, FHodgeCameraModeView& OutCameraModeView)`
- L491: `UHodgeCameraMode* UHodgeCameraModeStack::FindExistingMode(TSubclassOf<UHodgeCameraMode> Class) const`
- L497: `UHodgeCameraMode* UHodgeCameraModeStack::GetCameraModeInstance(TSubclassOf<UHodgeCameraMode> CameraModeClass)`
- L524: `void UHodgeCameraModeStack::UpdateStack(float DeltaTime)`
- L583: `void UHodgeCameraModeStack::BlendStack(FHodgeCameraModeView& OutCameraModeView) const`
- L614: `void UHodgeCameraModeStack::DrawDebug(UCanvas* Canvas) const`
- L642: `void UHodgeCameraModeStack::GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const`
- L669: `TSubclassOf<UHodgeCameraMode> UHodgeCameraModeStack::GetTopCameraModeClass() const`

## HodgeCameraMode_Cinematic.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Camera/HodgeCameraMode_Cinematic.cpp](../../../Source/Hodgepodge/Private/Camera/HodgeCameraMode_Cinematic.cpp)

项目内直接 include（不是运行调用关系）：[Camera/HodgeCameraMode_Cinematic.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode_Cinematic.h)、[Camera/HodgeCameraComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraComponent.h)、[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)、[Data/HodgeCinematicCameraProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeCinematicCameraProfile.h)、[UI/Subsystem/HodgeUIManagerSubsystem.h](../../../Source/Hodgepodge/Public/UI/Subsystem/HodgeUIManagerSubsystem.h)

定义候选（多行签名仅展示首行）：

- L42: `UHodgeCameraMode_Cinematic::UHodgeCameraMode_Cinematic()`
- L47: `bool UHodgeCameraMode_Cinematic::KeepUnderlyingModes() const`
- L53: `void UHodgeCameraMode_Cinematic::UpdateView(float DeltaTime)`
- L129: `void UHodgeCameraMode_Cinematic::UpdateBlending(float DeltaTime)`
- L134: `void UHodgeCameraMode_Cinematic::ReleaseLocalEffects()`
- L141: `void UHodgeCameraMode_Cinematic::OnDeactivation()`
- L146: `void UHodgeCameraMode_Cinematic::EndPresentation(const FHodgeAbilityCameraHandle& Handle)`

## HodgeCameraMode_LockOn.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Camera/HodgeCameraMode_LockOn.cpp](../../../Source/Hodgepodge/Private/Camera/HodgeCameraMode_LockOn.cpp)

项目内直接 include（不是运行调用关系）：[Camera/HodgeCameraMode_LockOn.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode_LockOn.h)、[Camera/HodgeCameraComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraComponent.h)、[Component/HodgeLockOnComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeLockOnComponent.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)、[Data/HodgeLockOnProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeLockOnProfile.h)

定义候选（多行签名仅展示首行）：

- L14: `void UHodgeCameraMode_LockOn::OnBecomeTop()`
- L34: `FRotator UHodgeCameraMode_LockOn::GetPivotRotation() const { return LookRotation; }`
- L35: `void UHodgeCameraMode_LockOn::UpdateView(float Delta)`

## HodgeCameraMode_ThirdPerson.cpp

第三人称偏移与防穿透逻辑；C++ 不设默认偏移曲线，改由蓝图/PawnData 提供。

源码：[Source/Hodgepodge/Private/Camera/HodgeCameraMode_ThirdPerson.cpp](../../../Source/Hodgepodge/Private/Camera/HodgeCameraMode_ThirdPerson.cpp)

项目内直接 include（不是运行调用关系）：[Camera/HodgeCameraMode_ThirdPerson.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode_ThirdPerson.h)、[Camera/HodgeCameraAssistInterface.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraAssistInterface.h)、[Camera/HodgePenetrationAvoidanceFeeler.h](../../../Source/Hodgepodge/Public/Camera/HodgePenetrationAvoidanceFeeler.h)

定义候选（多行签名仅展示首行）：

- L24: `UHodgeCameraMode_ThirdPerson::UHodgeCameraMode_ThirdPerson()`
- L59: `void UHodgeCameraMode_ThirdPerson::UpdateView(float DeltaTime)`
- L115: `void UHodgeCameraMode_ThirdPerson::UpdateForTarget(float DeltaTime)`
- L142: `void UHodgeCameraMode_ThirdPerson::DrawDebug(UCanvas* Canvas) const`
- L166: `void UHodgeCameraMode_ThirdPerson::UpdatePreventPenetration(float DeltaTime)`
- L258: `void UHodgeCameraMode_ThirdPerson::PreventCameraPenetration(class AActor const& ViewTarget, FVector const& SafeLoc,`
- L521: `void UHodgeCameraMode_ThirdPerson::SetTargetCrouchOffset(FVector NewTargetOffset)`
- L534: `void UHodgeCameraMode_ThirdPerson::UpdateCrouchOffset(float DeltaTime)`
- L557: `void UHodgeCameraMode_ThirdPerson::CopyViewSettingsFrom(const UHodgeCameraMode_ThirdPerson* Source)`

## HodgePlayerCameraManager.cpp

由 HodgePlayerController 构造选用的项目相机管理器；运行效果待验收。

源码：[Source/Hodgepodge/Private/Camera/HodgePlayerCameraManager.cpp](../../../Source/Hodgepodge/Private/Camera/HodgePlayerCameraManager.cpp)

项目内直接 include（不是运行调用关系）：[Camera/HodgePlayerCameraManager.h](../../../Source/Hodgepodge/Public/Camera/HodgePlayerCameraManager.h)、[Camera/HodgeCameraComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraComponent.h)、[Camera/HodgeUICameraManagerComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeUICameraManagerComponent.h)

定义候选（多行签名仅展示首行）：

- L21: `AHodgePlayerCameraManager::AHodgePlayerCameraManager(const FObjectInitializer& ObjectInitializer)`
- L38: `UHodgeUICameraManagerComponent* AHodgePlayerCameraManager::GetUICameraComponent() const`
- L45: `void AHodgePlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)`
- L65: `void AHodgePlayerCameraManager::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL,`

## HodgeUICameraManagerComponent.cpp

UI 相机管理扩展，不代表 UI 系统已接入。

源码：[Source/Hodgepodge/Private/Camera/HodgeUICameraManagerComponent.cpp](../../../Source/Hodgepodge/Private/Camera/HodgeUICameraManagerComponent.cpp)

项目内直接 include（不是运行调用关系）：[Camera/HodgeUICameraManagerComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeUICameraManagerComponent.h)、[Camera/HodgePlayerCameraManager.h](../../../Source/Hodgepodge/Public/Camera/HodgePlayerCameraManager.h)

定义候选（多行签名仅展示首行）：

- L18: `UHodgeUICameraManagerComponent* UHodgeUICameraManagerComponent::GetComponent(APlayerController* PC)`
- L35: `UHodgeUICameraManagerComponent::UHodgeUICameraManagerComponent()`
- L52: `void UHodgeUICameraManagerComponent::InitializeComponent()`
- L59: `void UHodgeUICameraManagerComponent::SetViewTarget(AActor* InViewTarget, FViewTargetTransitionParams TransitionParams)`
- L72: `bool UHodgeUICameraManagerComponent::NeedsToUpdateViewTarget() const`
- L79: `void UHodgeUICameraManagerComponent::UpdateViewTarget(struct FTViewTarget& OutVT, float DeltaTime)`
- L85: `void UHodgeUICameraManagerComponent::OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo,`

## HodgeCameraAssistInterface.h

相机辅助接口契约。

源码：[Source/Hodgepodge/Public/Camera/HodgeCameraAssistInterface.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraAssistInterface.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "CoreMinimal.h"
   9: #include "UObject/Interface.h"
  11: #include "HodgeCameraAssistInterface.generated.h"
  14: UINTERFACE(BlueprintType)
  15: class UHodgeCameraAssistInterface : public UInterface
  16: {
  17: 	GENERATED_BODY()
  18: };
  21: class IHodgeCameraAssistInterface
  22: {
  23: 	GENERATED_BODY()
  25: public:
  30: 	virtual void GetIgnoredActorsForCameraPentration(TArray<const AActor*>& OutActorsAllowPenetration) const
  31: 	{
  32: 	}
  38: 	virtual TOptional<AActor*> GetCameraPreventPenetrationTarget() const
  39: 	{
  41: 		return TOptional<AActor*>();
  42: 	}
  46: 	virtual void OnCameraPenetratingTarget()
  47: 	{
  48: 	}
  49: };
```

## HodgeCameraComponent.h

相机模式栈宿主与最终视图输出，依赖默认模式委托。

源码：[Source/Hodgepodge/Public/Camera/HodgeCameraComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraComponent.h)

项目内直接 include（不是运行调用关系）：[Camera/HodgeCameraMode.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "CoreMinimal.h"
   9: #include "Camera/CameraComponent.h"
  10: #include "Camera/HodgeCameraMode.h"
  11: #include "HodgeCameraComponent.generated.h"
  14: class UCanvas;
  15: class UHodgeCameraMode;
  16: class UHodgeCameraModeStack;
  17: class UObject;
  18: struct FFrame;
  19: struct FGameplayTag;
  20: struct FMinimalViewInfo;
  21: template <class TClass>
  22: class TSubclassOf;
  25: DECLARE_DELEGATE_RetVal(TSubclassOf<UHodgeCameraMode>, FHodgeCameraModeDelegate);
  33: UCLASS()
  34: class UHodgeCameraComponent : public UCameraComponent
  35: {
  36: 	GENERATED_BODY()
  38: public:
  42: 	UHodgeCameraComponent(const FObjectInitializer& ObjectInitializer);
  49: 	UFUNCTION(BlueprintPure, Category = "Hodge|Camera")
  50: 	static UHodgeCameraComponent* FindCameraComponent(const AActor* Actor)
  51: 	{
  52: 		return (Actor ? Actor->FindComponentByClass<UHodgeCameraComponent>() : nullptr);
  53: 	}
  59: 	virtual AActor* GetTargetActor() const { return GetOwner(); }
  62: 	FHodgeCameraModeDelegate DetermineCameraModeDelegate;
  68: 	void AddFieldOfViewOffset(float FovOffset) { FieldOfViewOffset += FovOffset; }
  74: 	virtual void DrawDebug(UCanvas* Canvas) const;
  81: 	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;
  83: 	UFUNCTION(BlueprintPure, Category="Hodge|Camera") TSubclassOf<UHodgeCameraMode> GetActiveCameraModeClass() const;
  85: 	bool GetLastCameraModeView(FHodgeCameraModeView& OutView) const;
  87: 	void QueueCinematicReturn(float Seconds, FRotator GameplayRotation);
  89: 	bool GetGameplayReturnRotation(FRotator& Out) const;
  91: 	void EndCinematicPresentation(const struct FHodgeAbilityCameraHandle& Handle);
  93: protected:
  96: 	virtual void OnRegister() override;
 103: 	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;
 107: 	virtual void UpdateCameraModes();
 109: protected:
 111: 	UPROPERTY()
 112: 	TObjectPtr<UHodgeCameraModeStack> CameraModeStack;
 115: 	float FieldOfViewOffset;
 116: 	FHodgeCameraModeView LastCameraModeView;
 117: 	bool bHasCameraModeView = false;
 119: 	float PendingReturnSeconds = -1.f;
 120: 	FRotator PendingGameplayRotation = FRotator::ZeroRotator;
 121: };
```

## HodgeCameraMode.h

相机视图、模式实例、混合和模式栈。

源码：[Source/Hodgepodge/Public/Camera/HodgeCameraMode.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "CoreMinimal.h"
   9: #include "GameplayTagContainer.h"
  10: #include "UObject/Object.h"
  11: #include "HodgeCameraMode.generated.h"
  13: class AActor;
  14: class UCanvas;
  15: class UHodgeCameraComponent;
  20: UENUM(BlueprintType)
  21: enum class EHodgeCameraModeBlendFunction : uint8
  22: {
  24: 	Linear,
  27: 	EaseIn,
  30: 	EaseOut,
  33: 	EaseInOut,
  36: 	COUNT UMETA(Hidden)
  37: };
  43: struct FHodgeCameraModeView
  44: {
  45: public:
  48: 	FHodgeCameraModeView();
  55: 	void Blend(const FHodgeCameraModeView& Other, float OtherWeight);
  57: public:
  59: 	FVector Location;
  62: 	FRotator Rotation;
  65: 	FRotator ControlRotation;
  68: 	float FieldOfView;
  69: };
  75: UCLASS(Abstract, NotBlueprintable)
  76: class HODGEPODGE_API UHodgeCameraMode : public UObject
  77: {
  78: 	GENERATED_BODY()
  80: public:
  83: 	UHodgeCameraMode();
  89: 	UHodgeCameraComponent* GetHodgeCameraComponent() const;
  95: 	virtual UWorld* GetWorld() const override;
 101: 	AActor* GetTargetActor() const;
 107: 	const FHodgeCameraModeView& GetCameraModeView() const { return View; }
 111: 	virtual void OnActivation()
 112: 	{
 113: 	};
 115: 	virtual void OnBecomeTop() {}
 117: 	virtual bool KeepUnderlyingModes() const { return false; }
 119: 	void SetTransitionBlendTime(float Seconds) { TransitionBlendTime = Seconds; }
 123: 	virtual void OnDeactivation()
 124: 	{
 125: 	};
 131: 	void UpdateCameraMode(float DeltaTime);
 137: 	float GetBlendTime() const { return TransitionBlendTime >= 0.f ? TransitionBlendTime : BlendTime; }
 143: 	float GetBlendWeight() const { return BlendWeight; }
 149: 	void SetBlendWeight(float Weight);
 155: 	FGameplayTag GetCameraTypeTag() const
 156: 	{
 157: 		return CameraTypeTag;
 158: 	}
 164: 	virtual void DrawDebug(UCanvas* Canvas) const;
 166: protected:
 171: 	virtual FVector GetPivotLocation() const;
 177: 	virtual FRotator GetPivotRotation() const;
 183: 	virtual void UpdateView(float DeltaTime);
 189: 	virtual void UpdateBlending(float DeltaTime);
 191: protected:
 193: 	UPROPERTY(EditDefaultsOnly, Category = "Blending")
 194: 	FGameplayTag CameraTypeTag;
 197: 	FHodgeCameraModeView View;
 200: 	UPROPERTY(EditDefaultsOnly, Category = "View",
 201: 		Meta = (UIMin = "5.0", UIMax = "170", ClampMin = "5.0", ClampMax = "170.0"))
 203: 	float FieldOfView;
 206: 	UPROPERTY(EditDefaultsOnly, Category = "View",
 207: 		Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
 209: 	float ViewPitchMin;
 212: 	UPROPERTY(EditDefaultsOnly, Category = "View",
 213: 		Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
 215: 	float ViewPitchMax;
 218: 	UPROPERTY(EditDefaultsOnly, Category = "Blending")
 219: 	float BlendTime;
 222: 	UPROPERTY(EditDefaultsOnly, Category = "Blending")
 223: 	EHodgeCameraModeBlendFunction BlendFunction;
 226: 	UPROPERTY(EditDefaultsOnly, Category = "Blending")
 227: 	float BlendExponent;
 230: 	float BlendAlpha;
 233: 	float BlendWeight;
 235: 	float TransitionBlendTime = -1.f;
 237: protected:
 239: 	UPROPERTY(transient)
 240: 	uint32 bResetInterpolation : 1;
 241: };
 247: UCLASS()
 248: class UHodgeCameraModeStack : public UObject
 249: {
 250: 	GENERATED_BODY()
 252: public:
 255: 	UHodgeCameraModeStack();
 259: 	void ActivateStack();
 263: 	void DeactivateStack();
 269: 	bool IsStackActivate() const { return bIsActive; }
 275: 	void PushCameraMode(TSubclassOf<UHodgeCameraMode> CameraModeClass, float TransitionSeconds = -1.f);
 283: 	bool EvaluateStack(float DeltaTime, FHodgeCameraModeView& OutCameraModeView);
 289: 	void DrawDebug(UCanvas* Canvas) const;
 296: 	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;
 298: 	TSubclassOf<UHodgeCameraMode> GetTopCameraModeClass() const;
 300: 	UHodgeCameraMode* FindExistingMode(TSubclassOf<UHodgeCameraMode> Class) const;
 302: protected:
 308: 	UHodgeCameraMode* GetCameraModeInstance(TSubclassOf<UHodgeCameraMode> CameraModeClass);
 314: 	void UpdateStack(float DeltaTime);
 320: 	void BlendStack(FHodgeCameraModeView& OutCameraModeView) const;
 322: protected:
 324: 	bool bIsActive;
 327: 	UPROPERTY()
 328: 	TArray<TObjectPtr<UHodgeCameraMode>> CameraModeInstances;
 331: 	UPROPERTY()
 332: 	TArray<TObjectPtr<UHodgeCameraMode>> CameraModeStack;
 333: };
```

## HodgeCameraMode_Cinematic.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Camera/HodgeCameraMode_Cinematic.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode_Cinematic.h)

项目内直接 include（不是运行调用关系）：[Camera/HodgeCameraMode.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode.h)、[Camera/HodgeCinematicCameraTypes.h](../../../Source/Hodgepodge/Public/Camera/HodgeCinematicCameraTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "Camera/HodgeCameraMode.h"
   4: #include "Camera/HodgeCinematicCameraTypes.h"
   5: #include "HodgeCameraMode_Cinematic.generated.h"
   6: class UAudioComponent;
   7: class UCameraShakeBase;
   8: class APlayerCameraManager;
  11: UCLASS()
  12: class HODGEPODGE_API UHodgeCameraMode_Cinematic : public UHodgeCameraMode
  13: {
  14: 	GENERATED_BODY()
  15: public:
  17: 	UHodgeCameraMode_Cinematic();
  19: 	virtual bool KeepUnderlyingModes() const override;
  21: 	virtual void OnDeactivation() override;
  23: 	void EndPresentation(const FHodgeAbilityCameraHandle& Handle);
  24: protected:
  26: 	virtual void UpdateView(float DeltaTime) override;
  28: 	virtual void UpdateBlending(float DeltaTime) override;
  29: private:
  31: 	FHodgeAbilityCameraHandle BoundHandle;
  32: 	FHodgeCameraModeView EntryView;
  34: 	float SourceBlendWeight = 1.f;
  36: 	FGuid EventExecution;
  37: 	TSet<FName> ConsumedEvents;
  38: 	float PreviousEventTime = 0.f;
  40: 	UPROPERTY() TArray<TWeakObjectPtr<UAudioComponent>> OwnedAudio;
  41: 	UPROPERTY() TArray<TWeakObjectPtr<UCameraShakeBase>> OwnedShakes;
  42: 	UPROPERTY() TWeakObjectPtr<APlayerCameraManager> ShakeManager;
  44: 	void ReleaseLocalEffects();
  45: };
```

## HodgeCameraMode_LockOn.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Camera/HodgeCameraMode_LockOn.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode_LockOn.h)

项目内直接 include（不是运行调用关系）：[Camera/HodgeCameraMode_ThirdPerson.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode_ThirdPerson.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "Camera/HodgeCameraMode_ThirdPerson.h"
   4: #include "HodgeCameraMode_LockOn.generated.h"
   6: UCLASS(Blueprintable)
   7: class HODGEPODGE_API UHodgeCameraMode_LockOn : public UHodgeCameraMode_ThirdPerson
   8: {
   9: 	GENERATED_BODY()
  10: public:
  11: 	virtual void OnBecomeTop() override;
  12: protected:
  13: 	virtual void UpdateView(float DeltaTime) override;
  14: 	virtual FRotator GetPivotRotation() const override;
  15: private:
  17: 	FRotator LookRotation = FRotator::ZeroRotator;
  19: 	TWeakObjectPtr<AActor> CameraTarget;
  20: 	FVector PreviousPivot = FVector::ZeroVector;
  21: };
```

## HodgeCameraMode_ThirdPerson.h

第三人称偏移与防穿透逻辑；C++ 不设默认偏移曲线，改由蓝图/PawnData 提供。

源码：[Source/Hodgepodge/Public/Camera/HodgeCameraMode_ThirdPerson.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode_ThirdPerson.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "CoreMinimal.h"
   9: #include "HodgeCameraMode.h"
  10: #include "HodgeCameraMode_ThirdPerson.generated.h"
  13: struct FHodgePenetrationAvoidanceFeeler;
  14: class UCurveVector;
  19: UCLASS(Abstract, Blueprintable)
  20: class UHodgeCameraMode_ThirdPerson : public UHodgeCameraMode
  21: {
  22: 	GENERATED_BODY()
  24: public:
  26: 	UHodgeCameraMode_ThirdPerson();
  29: 	void CopyViewSettingsFrom(const UHodgeCameraMode_ThirdPerson* Source);
  31: protected:
  36: 	virtual void UpdateView(float DeltaTime) override;
  42: 	void UpdateForTarget(float DeltaTime);
  48: 	void UpdatePreventPenetration(float DeltaTime);
  59: 	void PreventCameraPenetration(class AActor const& ViewTarget, FVector const& SafeLoc, FVector& CameraLoc,
  60: 	                              float const& DeltaTime, float& DistBlockedPct, bool bSingleRayOnly);
  66: 	virtual void DrawDebug(UCanvas* Canvas) const override;
  68: protected:
  70: 	UPROPERTY(EditDefaultsOnly, Category = "Third Person", Meta = (EditCondition = "!bUseRuntimeFloatCurves"))
  71: 	TObjectPtr<const UCurveVector> TargetOffsetCurve;
  74: 	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
  75: 	bool bUseRuntimeFloatCurves;
  78: 	UPROPERTY(EditDefaultsOnly, Category = "Third Person", Meta = (EditCondition = "bUseRuntimeFloatCurves"))
  79: 	FRuntimeFloatCurve TargetOffsetX;
  82: 	UPROPERTY(EditDefaultsOnly, Category = "Third Person", Meta = (EditCondition = "bUseRuntimeFloatCurves"))
  83: 	FRuntimeFloatCurve TargetOffsetY;
  86: 	UPROPERTY(EditDefaultsOnly, Category = "Third Person", Meta = (EditCondition = "bUseRuntimeFloatCurves"))
  87: 	FRuntimeFloatCurve TargetOffsetZ;
  90: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Third Person")
  91: 	float CrouchOffsetBlendMultiplier = 5.0f;
  94: public:
  96: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision")
  97: 	float PenetrationBlendInTime = 0.1f;
 100: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision")
 101: 	float PenetrationBlendOutTime = 0.15f;
 104: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision")
 105: 	bool bPreventPenetration = true;
 108: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision")
 109: 	bool bDoPredictiveAvoidance = true;
 112: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
 113: 	float CollisionPushOutDistance = 2.f;
 116: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
 117: 	float ReportPenetrationPercent = 0.f;
 120: 	UPROPERTY(EditDefaultsOnly, Category = "Collision")
 121: 	TArray<FHodgePenetrationAvoidanceFeeler> PenetrationAvoidanceFeelers;
 124: 	UPROPERTY(Transient)
 125: 	float AimLineToDesiredPosBlockedPct;
 128: 	UPROPERTY(Transient)
 129: 	TArray<TObjectPtr<const AActor>> DebugActorsHitDuringCameraPenetration;
 131: #if ENABLE_DRAW_DEBUG
 133: 	mutable float LastDrawDebugTime = -MAX_FLT;
 134: #endif
 136: protected:
 141: 	void SetTargetCrouchOffset(FVector NewTargetOffset);
 147: 	void UpdateCrouchOffset(float DeltaTime);
 150: 	FVector InitialCrouchOffset = FVector::ZeroVector;
 153: 	FVector TargetCrouchOffset = FVector::ZeroVector;
 156: 	float CrouchOffsetBlendPct = 1.0f;
 159: 	FVector CurrentCrouchOffset = FVector::ZeroVector;
 160: };
```

## HodgeCinematicCameraTypes.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Camera/HodgeCinematicCameraTypes.h](../../../Source/Hodgepodge/Public/Camera/HodgeCinematicCameraTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "CoreMinimal.h"
   4: #include "GameplayAbilitySpecHandle.h"
   5: #include "HodgeCinematicCameraTypes.generated.h"
   8: USTRUCT(BlueprintType)
   9: struct HODGEPODGE_API FHodgeAbilityCameraHandle
  10: {
  11: 	GENERATED_BODY()
  13: 	UPROPERTY(BlueprintReadOnly) FGameplayAbilitySpecHandle SpecHandle;
  15: 	UPROPERTY(BlueprintReadOnly) FGuid ExecutionId;
  17: 	UPROPERTY(BlueprintReadOnly) int32 Generation = 0;
  19: 	bool IsValid() const { return SpecHandle.IsValid() && Generation > 0; }
  21: 	bool operator==(const FHodgeAbilityCameraHandle& Other) const
  22: 	{ return SpecHandle == Other.SpecHandle && ExecutionId == Other.ExecutionId && Generation == Other.Generation; }
  23: };
  26: struct HODGEPODGE_API FHodgeCinematicCameraSample
  27: {
  29: 	int32 ShotIndex = INDEX_NONE;
  31: 	FVector CameraOffset = FVector::ZeroVector;
  33: 	FVector LookAtOffset = FVector::ZeroVector;
  35: 	float FieldOfView = 60.f;
  36: };
  39: USTRUCT(BlueprintType)
  40: struct HODGEPODGE_API FHodgeCinematicCameraState
  41: {
  42: 	GENERATED_BODY()
  44: 	UPROPERTY(BlueprintReadOnly) FHodgeAbilityCameraHandle Handle;
  46: 	UPROPERTY(BlueprintReadOnly) int32 MontageInstanceId = INDEX_NONE;
  48: 	UPROPERTY(BlueprintReadOnly) float SourcePosition = 0.f;
  50: 	UPROPERTY(BlueprintReadOnly) float WindowStart = 0.f;
  51: 	UPROPERTY(BlueprintReadOnly) float WindowEnd = 0.f;
  53: 	UPROPERTY(BlueprintReadOnly) float Weight = 0.f;
  55: 	UPROPERTY(BlueprintReadOnly) FName ShotId;
  57: 	UPROPERTY(BlueprintReadOnly) bool bActive = false;
  58: };
```

## HodgePenetrationAvoidanceFeeler.h

相机防穿透探测参数结构。

源码：[Source/Hodgepodge/Public/Camera/HodgePenetrationAvoidanceFeeler.h](../../../Source/Hodgepodge/Public/Camera/HodgePenetrationAvoidanceFeeler.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "CoreMinimal.h"
  10: #include "HodgePenetrationAvoidanceFeeler.generated.h"
  15: USTRUCT()
  16: struct FHodgePenetrationAvoidanceFeeler
  17: {
  18: 	GENERATED_BODY()
  21: 	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
  22: 	FRotator AdjustmentRot;
  25: 	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
  26: 	float WorldWeight;
  29: 	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
  30: 	float PawnWeight;
  33: 	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
  34: 	float Extent;
  37: 	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
  38: 	int32 TraceInterval;
  41: 	UPROPERTY(transient)
  42: 	int32 FramesUntilNextTrace;
  47: 	FHodgePenetrationAvoidanceFeeler()
  48: 	   : AdjustmentRot(ForceInit)
  49: 	   , WorldWeight(0)
  50: 	   , PawnWeight(0)
  51: 	   , Extent(0)
  52: 	   , TraceInterval(0)
  53: 	   , FramesUntilNextTrace(0)
  54: 	{
  55: 	}
  66: 	FHodgePenetrationAvoidanceFeeler(const FRotator& InAdjustmentRot,
  67: 							const float& InWorldWeight,
  68: 							const float& InPawnWeight,
  69: 							const float& InExtent,
  70: 							const int32& InTraceInterval = 0,
  71: 							const int32& InFramesUntilNextTrace = 0)
  72: 	   : AdjustmentRot(InAdjustmentRot)
  73: 	   , WorldWeight(InWorldWeight)
  74: 	   , PawnWeight(InPawnWeight)
  75: 	   , Extent(InExtent)
  76: 	   , TraceInterval(InTraceInterval)
  77: 	   , FramesUntilNextTrace(InFramesUntilNextTrace)
  78: 	{
  79: 	}
  80: };
```

## HodgePlayerCameraManager.h

由 HodgePlayerController 构造选用的项目相机管理器；运行效果待验收。

源码：[Source/Hodgepodge/Public/Camera/HodgePlayerCameraManager.h](../../../Source/Hodgepodge/Public/Camera/HodgePlayerCameraManager.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "CoreMinimal.h"
   9: #include "Camera/PlayerCameraManager.h"
  10: #include "HodgePlayerCameraManager.generated.h"
  13: class FDebugDisplayInfo;
  14: class UCanvas;
  15: class UObject;
  18: #define HODGE_CAMERA_DEFAULT_FOV          (80.0f)
  19: #define HODGE_CAMERA_DEFAULT_PITCH_MIN  (-89.0f)
  20: #define HODGE_CAMERA_DEFAULT_PITCH_MAX  (89.0f)
  22: class UHodgeUICameraManagerComponent;
  29: UCLASS(notplaceable, MinimalAPI)
  30: class AHodgePlayerCameraManager : public APlayerCameraManager
  31: {
  32: 	GENERATED_BODY()
  34: public:
  38: 	AHodgePlayerCameraManager(const FObjectInitializer& ObjectInitializer);
  44: 	UHodgeUICameraManagerComponent* GetUICameraComponent() const;
  46: protected:
  52: 	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
  61: 	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
  63: private:
  65: 	UPROPERTY(Transient)
  66: 	TObjectPtr<UHodgeUICameraManagerComponent> UICamera;
  67: };
```

## HodgeUICameraManagerComponent.h

UI 相机管理扩展，不代表 UI 系统已接入。

源码：[Source/Hodgepodge/Public/Camera/HodgeUICameraManagerComponent.h](../../../Source/Hodgepodge/Public/Camera/HodgeUICameraManagerComponent.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   9: #include "CoreMinimal.h"
  10: #include "Components/ActorComponent.h"
  11: #include "HodgeUICameraManagerComponent.generated.h"
  13: class AHodgePlayerCameraManager;
  15: class AActor;
  16: class AHUD;
  17: class APlayerController;
  18: class FDebugDisplayInfo;
  19: class UCanvas;
  20: class UObject;
  23: UCLASS(Transient, Within=HodgePlayerCameraManager)
  24: class UHodgeUICameraManagerComponent : public UActorComponent
  25: {
  26: 	GENERATED_BODY()
  28: public:
  34: 	static UHodgeUICameraManagerComponent* GetComponent(APlayerController* PC);
  36: public:
  39: 	UHodgeUICameraManagerComponent();
  43: 	virtual void InitializeComponent() override;
  49: 	bool IsSettingViewTarget() const { return bUpdatingViewTarget; }
  55: 	AActor* GetViewTarget() const { return ViewTarget; }
  62: 	void SetViewTarget(AActor* InViewTarget,
  63: 	                   FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams());
  69: 	bool NeedsToUpdateViewTarget() const;
  76: 	void UpdateViewTarget(struct FTViewTarget& OutVT, float DeltaTime);
  86: 	void OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos);
  88: private:
  90: 	UPROPERTY(Transient)
  91: 	TObjectPtr<AActor> ViewTarget;
  94: 	UPROPERTY(Transient)
  95: 	bool bUpdatingViewTarget;
  96: };
```
