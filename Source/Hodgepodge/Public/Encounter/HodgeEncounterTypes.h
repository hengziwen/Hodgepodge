/** @file HodgeEncounterTypes.h
 * @brief 定义两种刷怪入口共用的配置条目与服务器运行快照。
 */
#pragma once

#include "CoreMinimal.h"
#include "HodgeEncounterTypes.generated.h"

class AHodgePlayerState;
class UHodgePawnData;

/** 入口只决定启动与结算方式，波次运行职责由刷怪点统一执行。 */
UENUM(BlueprintType)
enum class EHodgeEncounterMode : uint8
{
	Interaction,
	Area
};

/** 区域只响应配置的真实重叠边沿，不通过持续扫描重复启动。 */
UENUM(BlueprintType)
enum class EHodgeEncounterTriggerEdge : uint8
{
	Enter,
	Exit
};

/** 服务器持有唯一事实状态，客户端仅依据快照更新开关与文字表现。 */
UENUM(BlueprintType)
enum class EHodgeEncounterState : uint8
{
	Inactive,
	Active,
	Spawning,
	AwaitingReward,
	Completed
};

/** 单个普通敌人的配置；出生变换相对刷怪点，等级沿用 PawnData 属性曲线。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeEncounterSpawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Encounter")
	TObjectPtr<UHodgePawnData> PawnData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Encounter", meta=(ClampMin="1"))
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Encounter")
	FTransform RelativeTransform = FTransform::Identity;
};

/** 上一波真实清空后等待 DelayBeforeSpawn，再生成本波全部配置条目。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeEncounterWave
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Encounter", meta=(ClampMin="0", Units="s"))
	float DelayBeforeSpawn = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Encounter")
	TArray<FHodgeEncounterSpawn> Enemies;
};

/** 复制给客户端的当前轮次投影，不保存成员对象或客户端结算权限。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeEncounterSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Hodge|Encounter")
	EHodgeEncounterState State = EHodgeEncounterState::Inactive;

	UPROPERTY(BlueprintReadOnly, Category="Hodge|Encounter")
	FGuid RunId;

	UPROPERTY(BlueprintReadOnly, Category="Hodge|Encounter")
	int32 Revision = 0;

	UPROPERTY(BlueprintReadOnly, Category="Hodge|Encounter")
	int32 WaveIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category="Hodge|Encounter")
	int32 AliveCount = 0;

	UPROPERTY(BlueprintReadOnly, Category="Hodge|Encounter")
	int32 PendingCount = 0;

	UPROPERTY(BlueprintReadOnly, Category="Hodge|Encounter")
	TObjectPtr<AHodgePlayerState> OwnerPlayerState = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Hodge|Encounter")
	double CooldownEndServerTime = 0.0;

	UPROPERTY(BlueprintReadOnly, Category="Hodge|Encounter")
	bool bNeedsExit = false;

	UPROPERTY(BlueprintReadOnly, Category="Hodge|Encounter")
	FString LastFailure;
};
