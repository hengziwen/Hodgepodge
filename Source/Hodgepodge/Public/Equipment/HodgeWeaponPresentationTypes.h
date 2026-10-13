/** @file HodgeWeaponPresentationTypes.h
 * @brief 声明武器表现状态数据相关的类型、接口与现有处理流程。
 */
#pragma once

#include "CoreMinimal.h"
#include "HodgeWeaponPresentationTypes.generated.h"

/** 每个手持通知独立选择释放后如何消隐，默认保持旧资产行为。 */
UENUM(BlueprintType)
enum class EHodgeWeaponHandEndBehavior : uint8
{
	ReturnToBack UMETA(DisplayName="回背部后延迟淡出"),
	FadeAtHand UMETA(DisplayName="在手部直接淡出")
};

/** 定义武器表现Phase的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeWeaponPresentationPhase : uint8
{
	// Hidden选项。
	Hidden,
	// 手部选项。
	Hand,
	// Returning选项。
	Returning,
	// Hovering选项。
	Hovering,
	// Fading选项。
	Fading
};

/** 持续表现的复制快照，身份由所属 WeaponInstance 和本次激活 Key 共同确定。 */
USTRUCT(BlueprintType)
struct FHodgeWeaponPresentationState
{
	GENERATED_BODY()
	// Phase的枚举配置或当前取值。
	UPROPERTY(BlueprintReadOnly) EHodgeWeaponPresentationPhase Phase = EHodgeWeaponPresentationPhase::Hidden;
	/** 复制释放方式，决定淡出时的挂接位置。 */
	UPROPERTY(BlueprintReadOnly) EHodgeWeaponHandEndBehavior EndBehavior = EHodgeWeaponHandEndBehavior::ReturnToBack;
	// 起始时间的数值或阈值。
	UPROPERTY(BlueprintReadOnly) double StartTime = 0;
	// 起始相对变换的几何或变换数据。
	UPROPERTY(BlueprintReadOnly) FTransform StartRelativeTransform = FTransform::Identity;
	// 起始可见性的数值或阈值。
	UPROPERTY(BlueprintReadOnly) float StartVisibility = 0.f;
	// 修订号的数值或阈值。
	UPROPERTY(BlueprintReadOnly) int32 Revision = 0;
	// 本次激活的关联键。
	UPROPERTY(BlueprintReadOnly) int32 ActivationKey = 0;
};
