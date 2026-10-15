# Targeting 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeLockOnTypes.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Targeting/HodgeLockOnTypes.cpp](../../../Source/Hodgepodge/Private/Targeting/HodgeLockOnTypes.cpp)

项目内直接 include（不是运行调用关系）：[Targeting/HodgeLockOnTypes.h](../../../Source/Hodgepodge/Public/Targeting/HodgeLockOnTypes.h)、[Data/HodgeLockOnProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeLockOnProfile.h)

定义候选（多行签名仅展示首行）：

- L13: `bool FHodgeLockOnTargetData::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)`
- L26: `void FHodgeLockOnGesture::Reset() { Samples.Reset(); LastTrigger = -DBL_MAX; bArmed = true; }`
- L27: `int32 FHodgeLockOnGesture::Sample(FVector2D Degrees, double Now, const UHodgeLockOnProfile& Profile)`
- L46: `void FHodgeSoftLockCamera::Seed(FRotator CurrentView, double Now)`
- L55: `void FHodgeSoftLockCamera::AddLook(FVector2D Degrees, double Now)`
- L61: `void FHodgeSoftLockCamera::BeginAcquire()`
- L67: `FRotator FHodgeSoftLockCamera::Update(const FHodgeLockCameraFrame& Frame, float DeltaSeconds, double Now, const UHodgeLockOnProfile& Profile, bool bAllowAssist)`
- L140: `void FHodgeSoftLockCamera::Reset()`

## HodgeLockOnTypes.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Targeting/HodgeLockOnTypes.h](../../../Source/Hodgepodge/Public/Targeting/HodgeLockOnTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "CoreMinimal.h"
   4: #include "Abilities/GameplayAbilityTargetTypes.h"
   5: #include "NativeGameplayTags.h"
   6: #include "HodgeLockOnTypes.generated.h"
   8: class UHodgeLockOnProfile;
   9: namespace HodgeLockOnTags
  10: {
  11: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input);
  12: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability);
  13: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Locked);
  14: }
  16: UENUM(BlueprintType)
  17: enum class EHodgeLockOnOperation : uint8 { Acquire, Switch, Clear };
  19: UENUM(BlueprintType)
  20: enum class EHodgeLockOnReason : uint8 { None, Accepted, UserClear, NotReady, InvalidSource, StaleRequest, StaleVersion, InvalidTarget, OutOfRange, Occluded, Death, Destroyed, Timeout, ActivationFailed, Uninitialized };
  22: USTRUCT(BlueprintType)
  23: struct HODGEPODGE_API FHodgeLockOnState
  24: {
  25: 	GENERATED_BODY()
  27: 	UPROPERTY(BlueprintReadOnly) TObjectPtr<AActor> Target = nullptr;
  29: 	UPROPERTY(BlueprintReadOnly) bool bHasTarget = false;
  31: 	UPROPERTY(BlueprintReadOnly) FName LockPointId;
  33: 	UPROPERTY(BlueprintReadOnly) int32 StateVersion = 0;
  35: 	UPROPERTY(BlueprintReadOnly) int32 RequestSequence = 0;
  37: 	UPROPERTY(BlueprintReadOnly) EHodgeLockOnReason Reason = EHodgeLockOnReason::None;
  39: 	UPROPERTY(BlueprintReadOnly) bool bAccepted = false;
  40: };
  42: USTRUCT()
  43: struct HODGEPODGE_API FHodgeLockOnTargetData : public FGameplayAbilityTargetData
  44: {
  45: 	GENERATED_BODY()
  46: 	UPROPERTY() TWeakObjectPtr<AActor> SourceAvatar;
  47: 	UPROPERTY() TWeakObjectPtr<AActor> CandidateActor;
  48: 	UPROPERTY() FName LockPointId;
  49: 	UPROPERTY() int32 RequestSequence = 0;
  50: 	UPROPERTY() int32 BaseStateVersion = 0;
  51: 	UPROPERTY() EHodgeLockOnOperation Operation = EHodgeLockOnOperation::Acquire;
  52: 	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct(); }
  54: 	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);
  55: };
  56: template<> struct TStructOpsTypeTraits<FHodgeLockOnTargetData> : TStructOpsTypeTraitsBase2<FHodgeLockOnTargetData>
  57: { enum { WithNetSerializer = true, WithCopy = true }; };
  59: struct HODGEPODGE_API FHodgeLockOnGesture
  60: {
  62: 	int32 Sample(FVector2D Degrees, double Now, const UHodgeLockOnProfile& Profile);
  64: 	void Reset();
  65: private:
  66: 	struct FSample { double Time; double X; };
  67: 	TArray<FSample> Samples;
  68: 	double LastTrigger = -DBL_MAX;
  69: 	bool bArmed = true;
  70: };
  72: struct HODGEPODGE_API FHodgeLockCameraFrame
  73: {
  74: 	FVector CameraLocation = FVector::ZeroVector;
  75: 	FVector PivotLocation = FVector::ZeroVector;
  76: 	FVector TargetLocation = FVector::ZeroVector;
  77: 	FRotator ViewRotation = FRotator::ZeroRotator;
  78: 	float HorizontalFOV = 80.f;
  79: 	float AspectRatio = 16.f/9.f;
  80: };
  82: struct HODGEPODGE_API FHodgeSoftLockCamera
  83: {
  85: 	void Seed(FRotator CurrentView, double Now);
  87: 	void AddLook(FVector2D Degrees, double Now);
  89: 	FRotator Update(const FHodgeLockCameraFrame& Frame, float DeltaSeconds, double Now, const UHodgeLockOnProfile& Profile, bool bAllowAssist);
  91: 	void BeginAcquire();
  92: 	bool IsAcquiring() const { return bAcquiring; }
  93: 	void Reset();
  94: 	FRotator GetRotation() const { return Rotation; }
  95: private:
  96: 	FRotator Rotation = FRotator::ZeroRotator;
  97: 	FVector2D PendingLook = FVector2D::ZeroVector;
  98: 	double LastManualAt = -DBL_MAX;
  99: 	float AcquireElapsed = 0.f;
 100: 	bool bAcquiring = false;
 101: };
```
