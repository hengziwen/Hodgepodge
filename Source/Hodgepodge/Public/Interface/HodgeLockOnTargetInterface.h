/** @file HodgeLockOnTargetInterface.h @brief 怪物提供的可锁定资格与锚点契约。 */
#pragma once
#include "UObject/Interface.h"
#include "HodgeLockOnTargetInterface.generated.h"
UINTERFACE(BlueprintType)
class HODGEPODGE_API UHodgeLockOnTargetInterface : public UInterface { GENERATED_BODY() };
/** 目标自身决定资格；位置查询失败必须返回 false，不能返回世界原点。 */
class HODGEPODGE_API IHodgeLockOnTargetInterface
{
	GENERATED_BODY()
public:
	/** 目标是否允许当前 Pawn 锁定，伤害与队伍规则仍需由调用方检查。 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Hodge|LockOn") bool CanBeLockedBy(APawn* SourcePawn) const;
	/** 返回锚点世界位置；首版使用 NAME_None 指代默认锚点。 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Hodge|LockOn") bool GetLockPoint(FName LockPointId, FVector& OutLocation) const;
	/** 返回归一化优先级，超出 0～1 的值由评分方截断。 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Hodge|LockOn") float GetLockPriority() const;
};
