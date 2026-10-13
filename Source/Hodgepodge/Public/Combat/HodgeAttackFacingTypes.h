/** @file HodgeAttackFacingTypes.h @brief 攻击前摇旋转 Warp 的配置、方向载荷与校验。 */
#pragma once
#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GameplayTagContainer.h"
#include "HodgeAttackFacingTypes.generated.h"

/** 朝向驱动保持不变，此枚举仅选择动作旋转的执行者。 */
UENUM(BlueprintType)
enum class EHodgeFacingExecutionMode : uint8 { PhysicalTurn, MontageWarp };

/** 用于诊断及服务器验证方向来源，不自动搜索候选。 */
UENUM()
enum class EHodgeAttackFacingSource : uint8 { Actor, Camera, MoveInput, LockedTarget };

/** 默认关闭，窗口源时间由 Montage 的原生 Motion Warping 通知提供。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAttackFacingConfig
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool bEnabled = false;
	// 同名目标只在当前动作占用中有效，不能按名称清理其他执行。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName WarpTargetName = TEXT("HodgeAttackFacing");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1",ClampMax="179",Units="deg")) float MaxCorrectionAngle = 90.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1",Units="deg/s")) float MaxRotationRate = 1080.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.01")) float RotationTimeMultiplier = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.1",Units="s")) float TargetDataTimeout = 1.f;
	// 起手位置存在网络差值时允许有限误差，不能借此提交任意起始 Yaw。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0",ClampMax="90",Units="deg")) float StartYawTolerance = 45.f;
	/** 校验基础配置；Montage 窗口与命中时序由 Definition 另行检查。 */
	bool Validate(FString& Error) const;
	// 锁定目标的位置存在网络误差时只容忍有限方向偏差。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0",ClampMax="90",Units="deg")) float TargetYawTolerance = 30.f;
};

/** 仅传方向，不接受客户端提供落点或 Warp Transform。 */
USTRUCT()
struct HODGEPODGE_API FHodgeAttackFacingTargetData : public FGameplayAbilityTargetData
{
	GENERATED_BODY()
	UPROPERTY() TWeakObjectPtr<AActor> SourceAvatar;
	UPROPERTY() TWeakObjectPtr<AActor> LockedTarget;
	UPROPERTY() float StartYaw = 0.f;
	UPROPERTY() float DesiredYaw = 0.f;
	UPROPERTY() EHodgeAttackFacingSource Source = EHodgeAttackFacingSource::Actor;
	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct(); }
	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);
};
template<> struct TStructOpsTypeTraits<FHodgeAttackFacingTargetData> : TStructOpsTypeTraitsBase2<FHodgeAttackFacingTargetData>
{ enum { WithNetSerializer = true, WithCopy = true }; };

namespace HodgeAttackFacing
{
	/** 返回有界目标 Yaw；非法值不传播到角色或根运动。 */
	HODGEPODGE_API bool ResolveYaw(float StartYaw, float DesiredYaw, float MaxAngle, float& OutYaw);
}
