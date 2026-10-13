/** @file HodgeCharacterFacingTypes.h
 * @brief 声明朝向请求与移动意图数据相关的类型、接口与现有处理流程。
 */
#pragma once

#include "CoreMinimal.h"
#include "HodgeCharacterFacingTypes.generated.h"

/** 朝向驱动与移动动画风格独立，冻结作为约束叠加。 */
UENUM(BlueprintType)
enum class EHodgeCharacterFacingDriver : uint8 { Movement, Controller, ActionDirection };

/** 区分自由朝向与预留八向移动的动画风格。 */
UENUM(BlueprintType)
enum class EHodgeLocomotionStyle : uint8 { /* 自由朝向的移动表现。 */ FreeDirectional, /* 保留八向侧移的移动表现。 */ ReservedStrafe };

/** 记录朝向请求的准备、应用、阻塞与结束结果。 */
UENUM(BlueprintType)
enum class EHodgeFacingRequestStatus : uint8 { /* 无效状态或身份。 */ Invalid, /* 等待依赖或应用结果。 */ Pending, /* 请求已经应用。 */ Applied, /* 当前条件阻塞执行。 */ Blocked, /* 来源已主动释放。 */ Released, /* 请求被拒绝。 */ Rejected };

/** 句柄只能由创建它的 Pawn 初始化代消费。 */
USTRUCT(BlueprintType)
struct FHodgeFacingRequestHandle
{
	GENERATED_BODY()
	// 当前记录的唯一标识。
	UPROPERTY(BlueprintReadOnly) FGuid Id;
	// 用于拒绝旧Pawn回调的初始化代号。
	UPROPERTY(BlueprintReadOnly) int32 AvatarGeneration = 0;
	/** 检查当前对象、配置或句柄是否满足其有效性条件。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsValid() const { return Id.IsValid() && AvatarGeneration > 0; }
};

/** 原始输入不随角色转向改义；方向在输入采样时转换。 */
USTRUCT(BlueprintType)
struct FHodgeMoveIntentSnapshot
{
	GENERATED_BODY()
	// 输入采样时的原始二维方向。
	UPROPERTY(BlueprintReadOnly) FVector2D RawInput2D = FVector2D::ZeroVector;
	// 采样时的控制器Yaw，用于保持输入空间一致。
	UPROPERTY(BlueprintReadOnly) float ControllerYawAtSample = 0.f;
	// 由采样输入得到的世界水平期望方向。
	UPROPERTY(BlueprintReadOnly) FVector DesiredDirectionWorld = FVector::ZeroVector;
	// 原始输入的幅度。
	UPROPERTY(BlueprintReadOnly) float InputMagnitude = 0.f;
	// 输入或状态采样的时间。
	UPROPERTY(BlueprintReadOnly) double SampleTime = 0.;
	// 用于拒绝旧Pawn回调的初始化代号。
	UPROPERTY(BlueprintReadOnly) int32 AvatarGeneration = 0;
};

/** 动画线程只读的模式结果，不包含目标或 GA 引用。 */
USTRUCT(BlueprintType)
struct FHodgeFacingPresentationSnapshot
{
	GENERATED_BODY()
	// 当前移动动画风格。
	UPROPERTY(BlueprintReadOnly) EHodgeLocomotionStyle Style = EHodgeLocomotionStyle::FreeDirectional;
	// 当前朝向的驱动来源。
	UPROPERTY(BlueprintReadOnly) EHodgeCharacterFacingDriver Driver = EHodgeCharacterFacingDriver::Movement;
	// 当前是否存在有效的Yaw冻结约束。
	UPROPERTY(BlueprintReadOnly) bool bYawLocked = false;
	// 是否正在按恢复规则回到基础朝向。
	UPROPERTY(BlueprintReadOnly) bool bRecoveringFacing = false;
	// 本状态快照的修订序号。
	UPROPERTY(BlueprintReadOnly) int32 StateVersion = 0;
};

namespace HodgeFacing
{
	/** 校验并归一化方向，输出用于后续方向计算。
	 * @param Input 输入待处理的数据。
	 * @param Output 输出计算得到的数据。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	HODGEPODGE_API bool NormalizeDirection(const FVector& Input, FVector& Output);
	/** 构造移动意图。
	 * @param Input 输入待处理的数据。
	 * @param ControlYaw 控制Yaw角，由调用方传入。
	 * @param Time 当前处理使用的时间值。
	 * @param Generation 初始化代，由调用方传入。
	 * @return 本次计算、查询或创建得到的移动意图快照。
	 */
	HODGEPODGE_API FHodgeMoveIntentSnapshot MakeMoveIntent(FVector2D Input, float ControlYaw, double Time, int32 Generation);
	/** 判断基础驱动。
	 * @param Driver 当前朝向的驱动来源。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	HODGEPODGE_API bool IsBaseDriver(EHodgeCharacterFacingDriver Driver);
}
