/** @file HodgeInteractionTypes.h
 * @brief 保存交互目标提供的运行身份与服务器执行结果。
 */
#pragma once

#include "CoreMinimal.h"
#include "HodgeInteractionTypes.generated.h"

/** 目标按当前状态生成选项，客户端只能回传所看到的运行与修订身份。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeInteractionOption
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Hodge|Interaction")
	FGuid RunId;

	UPROPERTY(BlueprintReadOnly, Category="Hodge|Interaction")
	int32 Revision = 0;

	UPROPERTY(BlueprintReadOnly, Category="Hodge|Interaction")
	FText Text;
};

/** 结果仅用于请求反馈；刷怪和奖励的事实仍由目标服务器状态持有。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeInteractionResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Hodge|Interaction")
	bool bSucceeded = false;

	UPROPERTY(BlueprintReadOnly, Category="Hodge|Interaction")
	FString Reason;
};
