/** @file HodgeInteractableTarget.h
 * @brief 沿用 Lyra 目标查询与请求执行分离的最小原生交互契约。
 */
// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Interaction/HodgeInteractionTypes.h"
#include "UObject/Interface.h"
#include "HodgeInteractableTarget.generated.h"

class APawn;

UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UHodgeInteractableTarget : public UInterface
{
	GENERATED_BODY()
};

/** 查询不改变状态；执行仍须在服务器复核目标自己的状态与结算身份。 */
class HODGEPODGE_API IHodgeInteractableTarget
{
	GENERATED_BODY()

public:
	virtual bool GetInteractionOption(const APawn* RequestingPawn, FHodgeInteractionOption& OutOption) const = 0;
	virtual FHodgeInteractionResult ExecuteInteraction(APawn* RequestingPawn, const FGuid& ExpectedRunId, int32 ExpectedRevision) = 0;
};
