#pragma once

#include "CoreMinimal.h"
#include "HodgeCharacterFacingTypes.generated.h"

/** 朝向驱动与移动动画风格独立，冻结作为约束叠加。 */
UENUM(BlueprintType)
enum class EHodgeCharacterFacingDriver : uint8 { Movement, Controller, ActionDirection };

UENUM(BlueprintType)
enum class EHodgeLocomotionStyle : uint8 { FreeDirectional, ReservedStrafe };

UENUM(BlueprintType)
enum class EHodgeFacingRequestStatus : uint8 { Invalid, Pending, Applied, Blocked, Released, Rejected };

/** 句柄只能由创建它的 Pawn 初始化代消费。 */
USTRUCT(BlueprintType)
struct FHodgeFacingRequestHandle
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) FGuid Id;
	UPROPERTY(BlueprintReadOnly) int32 AvatarGeneration = 0;
	bool IsValid() const { return Id.IsValid() && AvatarGeneration > 0; }
};

/** 原始输入不随角色转向改义；方向在输入采样时转换。 */
USTRUCT(BlueprintType)
struct FHodgeMoveIntentSnapshot
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) FVector2D RawInput2D = FVector2D::ZeroVector;
	UPROPERTY(BlueprintReadOnly) float ControllerYawAtSample = 0.f;
	UPROPERTY(BlueprintReadOnly) FVector DesiredDirectionWorld = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly) float InputMagnitude = 0.f;
	UPROPERTY(BlueprintReadOnly) double SampleTime = 0.;
	UPROPERTY(BlueprintReadOnly) int32 AvatarGeneration = 0;
};

/** 动画线程只读的模式结果，不包含目标或 GA 引用。 */
USTRUCT(BlueprintType)
struct FHodgeFacingPresentationSnapshot
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) EHodgeLocomotionStyle Style = EHodgeLocomotionStyle::FreeDirectional;
	UPROPERTY(BlueprintReadOnly) EHodgeCharacterFacingDriver Driver = EHodgeCharacterFacingDriver::Movement;
	UPROPERTY(BlueprintReadOnly) bool bYawLocked = false;
	UPROPERTY(BlueprintReadOnly) bool bRecoveringFacing = false;
	UPROPERTY(BlueprintReadOnly) int32 StateVersion = 0;
};

namespace HodgeFacing
{
	HODGEPODGE_API bool NormalizeDirection(const FVector& Input, FVector& Output);
	HODGEPODGE_API FHodgeMoveIntentSnapshot MakeMoveIntent(FVector2D Input, float ControlYaw, double Time, int32 Generation);
	HODGEPODGE_API bool IsBaseDriver(EHodgeCharacterFacingDriver Driver);
}
