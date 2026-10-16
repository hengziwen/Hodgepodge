// Copyright Epic Games, Inc. All Rights Reserved.
#include "HodgeInventoryValidationLibrary.h"
#include "Core/PlayerController/HodgePlayerController.h"
#include "Engine/World.h"
#include "CoreGlobals.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeInventoryValidationLibrary)

bool UHodgeInventoryValidationLibrary::DispatchInventoryRequest(AHodgePlayerController* Controller, FGuid ItemId, FGuid RequestId, bool bUnequip)
{
	if (!IsValid(Controller) || !Controller->GetWorld() || Controller->GetWorld()->WorldType != EWorldType::PIE || !Controller->IsLocalController()) { return false; }
	// UE 5.5 Actor.cpp 在编辑器脚本保护下强制本地 Callspace；只在测试发送作用域恢复正常网络路由。
	TGuardValue<bool> NetworkCallspace(GAllowActorScriptExecutionInEditor, false);
	if (bUnequip) { Controller->RequestInventoryUnequip(RequestId); }
	else { Controller->RequestInventoryEquip(ItemId, RequestId); }
	return true;
}
