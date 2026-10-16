// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "HodgeInventoryTypes.generated.h"

class UHodgeInventoryItemDefinition;
class UHodgeInventoryItemInstance;

/** 库存操作的可观察结果；等待依赖不等于已装备。 */
UENUM(BlueprintType)
enum class EHodgeInventoryResult : uint8
{
	Success, Waiting, NoAuthority, Busy, InvalidConfiguration, InvalidQuantity,
	CapacityExceeded, InsufficientQuantity, NotOwned, NotEquippable, ActionBlocked,
	EquipmentFailed, RecoveryFailed
};

/** 一次已提交数量变化，移除后仍可按稳定身份识别。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeInventoryChange
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) FGuid ItemId;
	UPROPERTY(BlueprintReadOnly) int32 OldCount = 0;
	UPROPERTY(BlueprintReadOnly) int32 NewCount = 0;
};

/** 只传递结果和值，不要求结果通知先于物品子对象复制。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeInventoryOperation
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) EHodgeInventoryResult Code = EHodgeInventoryResult::Success;
	UPROPERTY(BlueprintReadOnly) FString Reason;
	UPROPERTY(BlueprintReadOnly) TArray<FHodgeInventoryChange> Changes;
	bool Succeeded() const { return Code == EHodgeInventoryResult::Success; }
};

/** 现有 PawnData 内的服务器初始发放项；不作为客户端发物品协议。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeInventoryDefaultItem
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSubclassOf<UHodgeInventoryItemDefinition> Definition;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1")) int32 Count = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1")) int32 Level = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bEquip = false;
};

/** 消费者先订阅再读取的本地快照；Revision 不充当跨连接逐操作账本。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeInventoryItemView
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) FGuid ItemId;
	UPROPERTY(BlueprintReadOnly) TObjectPtr<UHodgeInventoryItemInstance> Instance;
	UPROPERTY(BlueprintReadOnly) int32 Count = 0;
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FHodgeInventoryChanged, const TArray<FHodgeInventoryChange>&, int64);
DECLARE_MULTICAST_DELEGATE(FHodgeEquipmentSelectionChanged);
