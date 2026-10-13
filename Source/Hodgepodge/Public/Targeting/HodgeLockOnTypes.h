/** @file HodgeLockOnTypes.h @brief 锁定命令、确认快照与鼠标手势的数据契约。 */
#pragma once
#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "NativeGameplayTags.h"
#include "HodgeLockOnTypes.generated.h"

class UHodgeLockOnProfile;
namespace HodgeLockOnTags
{
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Locked);
}
/** 线上只传明确操作，避免客户端与服务器将 Toggle 解释为相反动作。 */
UENUM(BlueprintType)
enum class EHodgeLockOnOperation : uint8 { Acquire, Switch, Clear };
/** 用于日志、调试和拒绝恢复的原因，不承担运行时行动状态。 */
UENUM(BlueprintType)
enum class EHodgeLockOnReason : uint8 { None, Accepted, UserClear, NotReady, InvalidSource, StaleRequest, StaleVersion, InvalidTarget, OutOfRange, Occluded, Death, Destroyed, Timeout, ActivationFailed, Uninitialized };
/** 长期目标事实；即使目标未变化，也复制已处理命令序号。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeLockOnState
{
	GENERATED_BODY()
	// 当前确认的目标；空值表示未锁定。
	UPROPERTY(BlueprintReadOnly) TObjectPtr<AActor> Target = nullptr;
	// 区分服务器已解除与目标引用尚未复制到达。
	UPROPERTY(BlueprintReadOnly) bool bHasTarget = false;
	// 目标契约提供的锚点身份。
	UPROPERTY(BlueprintReadOnly) FName LockPointId;
	// 服务器状态修订，包含主动解除与拒绝结果。
	UPROPERTY(BlueprintReadOnly) int32 StateVersion = 0;
	// 当前 Pawn 已处理的客户端请求序号。
	UPROPERTY(BlueprintReadOnly) int32 RequestSequence = 0;
	// 最近一次处理原因。
	UPROPERTY(BlueprintReadOnly) EHodgeLockOnReason Reason = EHodgeLockOnReason::None;
	// 命令是否被业务规则接受，与 GAS 激活接受区分。
	UPROPERTY(BlueprintReadOnly) bool bAccepted = false;
};
/** 一次 GAS 激活的一条目标数据；对象引用通过 PackageMap 序列化。 */
USTRUCT()
struct HODGEPODGE_API FHodgeLockOnTargetData : public FGameplayAbilityTargetData
{
	GENERATED_BODY()
	UPROPERTY() TWeakObjectPtr<AActor> SourceAvatar;
	UPROPERTY() TWeakObjectPtr<AActor> CandidateActor;
	UPROPERTY() FName LockPointId;
	UPROPERTY() int32 RequestSequence = 0;
	UPROPERTY() int32 BaseStateVersion = 0;
	UPROPERTY() EHodgeLockOnOperation Operation = EHodgeLockOnOperation::Acquire;
	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct(); }
	/** 返回序列化是否完整；不能将尚未解析的引用默认为合法目标。 */
	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);
};
template<> struct TStructOpsTypeTraits<FHodgeLockOnTargetData> : TStructOpsTypeTraitsBase2<FHodgeLockOnTargetData>
{ enum { WithNetSerializer = true, WithCopy = true }; };
/** 归一化鼠标角位移的滚动手势；不绑定设备，也不逐帧计数。 */
struct HODGEPODGE_API FHodgeLockOnGesture
{
	/** 输入等效角位移和时间；返回 -1/0/1 对应左切、不切、右切。 */
	int32 Sample(FVector2D Degrees, double Now, const UHodgeLockOnProfile& Profile);
	/** 清理手势，禁止上一锁定会话的输入流入下一会话。 */
	void Reset();
private:
	struct FSample { double Time; double X; };
	TArray<FSample> Samples;
	double LastTrigger = -DBL_MAX;
	bool bArmed = true;
};
/** 上一帧实际 POV 与当前目标；投影采用当前视口的水平 FOV 和宽高比。 */
struct HODGEPODGE_API FHodgeLockCameraFrame
{
	FVector CameraLocation = FVector::ZeroVector;
	FVector PivotLocation = FVector::ZeroVector;
	FVector TargetLocation = FVector::ZeroVector;
	FRotator ViewRotation = FRotator::ZeroRotator;
	float HorizontalFOV = 80.f;
	float AspectRatio = 16.f/9.f;
};
/** 纯本地获取与跟随状态；不改变目标、Actor 或 Controller。 */
struct HODGEPODGE_API FHodgeSoftLockCamera
{
	/** 使用切换前实际输出的视角开始取景，保留同帧尚未求值的输入。 */
	void Seed(FRotator CurrentView, double Now);
	/** 累计当前帧等效角度；无效输入不污染镜头状态。 */
	void AddLook(FVector2D Degrees, double Now);
	/** 更新局部视角并返回结果；不修改目标、Actor 或 Controller。 */
	FRotator Update(const FHodgeLockCameraFrame& Frame, float DeltaSeconds, double Now, const UHodgeLockOnProfile& Profile, bool bAllowAssist);
	/** 仅在目标身份改变时开始获取；鼠标输入可以立即终止获取。 */
	void BeginAcquire();
	bool IsAcquiring() const { return bAcquiring; }
	void Reset();
	FRotator GetRotation() const { return Rotation; }
private:
	FRotator Rotation = FRotator::ZeroRotator;
	FVector2D PendingLook = FVector2D::ZeroVector;
	double LastManualAt = -DBL_MAX;
	float AcquireElapsed = 0.f;
	bool bAcquiring = false;
};
