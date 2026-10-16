// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HodgeInventoryValidationLibrary.generated.h"

class AHodgePlayerController;

/** 只在真实 PIE 本地 Controller 发送测试请求，不模拟服务端结算。 */
UCLASS()
class UHodgeInventoryValidationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/** 返回是否完成发送调用；实际成功与否必须读取带 RequestId 的客户端结果。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Validation")
	static bool DispatchInventoryRequest(AHodgePlayerController* Controller, FGuid ItemId, FGuid RequestId, bool bUnequip);
};
