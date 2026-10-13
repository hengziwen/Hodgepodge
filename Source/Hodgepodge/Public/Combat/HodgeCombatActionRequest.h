/** @file HodgeCombatActionRequest.h
 * @brief 声明共享战斗请求数据相关的类型、接口与现有处理流程。
 */
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpecHandle.h"
#include "HodgeCombatActionRequest.generated.h"

/** 定义战斗动作请求类型的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeCombatActionRequestType : uint8 { /* 独立执行能力选项。 */ StandaloneAbility, /* 连段意图选项。 */ ComboIntent };

/** 定义战斗动作请求状态的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeCombatActionRequestStatus : uint8 { /* 请求已进入等待队列。 */ Queued, /* 当前执行已经开始。 */ Started, /* 当前操作正常完成。 */ Completed, /* 当前操作已取消。 */ Cancelled, /* 超过当前有效时间。 */ Expired, /* 请求被拒绝。 */ Rejected };

/** 请求是本地执行记录，玩家预测与服务器执行仍由 GAS 的原有协议关联。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeCombatActionRequestState
{
	GENERATED_BODY()
	// 本次请求的唯一身份。
	UPROPERTY(BlueprintReadOnly) FGuid RequestId;
	// 类型的枚举配置或当前取值。
	UPROPERTY(BlueprintReadOnly) EHodgeCombatActionRequestType Type = EHodgeCombatActionRequestType::StandaloneAbility;
	// 动作标签的数据或对象引用。
	UPROPERTY(BlueprintReadOnly) FGameplayTag ActionTag;
	// 状态的枚举配置或当前取值。
	UPROPERTY(BlueprintReadOnly) EHodgeCombatActionRequestStatus Status = EHodgeCombatActionRequestStatus::Rejected;
	// 已授予能力的Spec句柄。
	UPROPERTY(BlueprintReadOnly) FGameplayAbilitySpecHandle AbilityHandle;
	// 本次动作执行的唯一身份。
	UPROPERTY(BlueprintReadOnly) FGuid ExecutionId;
	// Reason的数据或对象引用。
	UPROPERTY(BlueprintReadOnly) FString Reason;
	/** 判断Terminal。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsTerminal() const { return Status != EHodgeCombatActionRequestStatus::Queued && Status != EHodgeCombatActionRequestStatus::Started; }
};

DECLARE_MULTICAST_DELEGATE_OneParam(FHodgeCombatActionRequestChanged, const FHodgeCombatActionRequestState&);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHodgeCombatActionRequestChangedBP, const FHodgeCombatActionRequestState&, State);
