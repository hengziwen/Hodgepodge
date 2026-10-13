/** @file HodgeAttributeTypes.h
 * @brief 声明属性成长与就绪数据相关的类型、接口与现有处理流程。
 */
#pragma once

#include "CoreMinimal.h"
#include "HodgeAttributeTypes.generated.h"

class APawn;
class UHodgeEquipmentDefinition;

/** 定义属性更新Reason的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeAttributeUpdateReason : uint8
{
	// First生成选项。
	FirstSpawn,
	// Respawn选项。
	Respawn,
	// Rebind选项。
	Rebind,
	// 等级Up选项。
	LevelUp,
	// 装备变更选项。
	EquipmentChange,
	// 恢复选项。
	Restore
};

/** 保存角色成长的数据和接口契约。 */
USTRUCT(BlueprintType)
struct FHodgeCharacterProgression
{
	GENERATED_BODY()
	// 角色标识的数据或对象引用。
	UPROPERTY(BlueprintReadOnly) FGuid CharacterId;
	// 配置或运行实例的等级。
	UPROPERTY(BlueprintReadOnly) int32 Level = 1;
	// 修订号的数值或阈值。
	UPROPERTY(BlueprintReadOnly) int32 Revision = 0;
};

/** 保存属性就绪状态的数据和接口契约。 */
USTRUCT(BlueprintType)
struct FHodgeAttributeReadyState
{
	GENERATED_BODY()
	// 表现角色的数据或对象引用。
	UPROPERTY(BlueprintReadOnly) TObjectPtr<APawn> Avatar = nullptr;
	// Life初始化代的数值或阈值。
	UPROPERTY(BlueprintReadOnly) int32 LifeGeneration = 0;
	// 修订号的数值或阈值。
	UPROPERTY(BlueprintReadOnly) int32 Revision = 0;
	// 依赖是否已准备完成。
	UPROPERTY(BlueprintReadOnly) bool bReady = false;
};

/** 保存Owned装备状态的数据和接口契约。 */
USTRUCT(BlueprintType)
struct FHodgeOwnedEquipmentState
{
	GENERATED_BODY()
	// 用于区分运行实例的标识。
	UPROPERTY(BlueprintReadOnly) FGuid InstanceId;
	// 当前操作使用的定义配置。
	UPROPERTY(BlueprintReadOnly) TSubclassOf<UHodgeEquipmentDefinition> Definition;
	// 配置或运行实例的等级。
	UPROPERTY(BlueprintReadOnly) int32 Level = 1;
};
