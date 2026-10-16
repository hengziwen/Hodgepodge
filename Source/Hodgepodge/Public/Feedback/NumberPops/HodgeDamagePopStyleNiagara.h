// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "Engine/DataAsset.h"
#include "HodgeDamagePopStyleNiagara.generated.h"
class UNiagaraSystem;

/** 复用 Lyra Niagara 样式契约；由本地 Controller 软加载，不让专服加载表现依赖。 */
UCLASS(BlueprintType, Const)
class HODGEPODGE_API UHodgeDamagePopStyleNiagara : public UDataAsset
{
	GENERATED_BODY()
public:
	// 与迁入的 Lyra User.DamageInfo 数组一致，沿用原样式的非前缀名称。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName NiagaraArrayName = TEXT("DamageInfo");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UNiagaraSystem> TextNiagara;
	// 限制单次仿真前未消费的事件量，超过时优先保留最新事件。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="512")) int32 MaxBufferedRequests = 64;
};
