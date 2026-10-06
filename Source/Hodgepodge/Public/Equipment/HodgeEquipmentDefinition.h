// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// 提供 TSubclassOf，用于限制可配置的 UClass 必须继承指定父类。
#include "Templates/SubclassOf.h"

#include "HodgeEquipmentDefinition.generated.h"

class AActor;
class UHodgeAbilitySet;
class UHodgeEquipmentInstance;
class UHodgeEquipmentStatProfile;

/**
 * 描述装备时需要生成并挂接到 Pawn 上的一个 Actor。
 * 主要用于配置武器模型、装备模型等世界表现对象。
 */
USTRUCT()
struct FHodgeEquipmentActorToSpawn
{
	GENERATED_BODY()

	// 默认构造函数。
	FHodgeEquipmentActorToSpawn()
	{
	}

	// 装备时需要生成的 Actor 类型。
	UPROPERTY(EditAnywhere, Category=Equipment)
	TSubclassOf<AActor> ActorToSpawn;

	// 生成的 Actor 需要挂接到 Pawn 上的 Socket 名称。
	UPROPERTY(EditAnywhere, Category=Equipment)
	FName AttachSocket;

	// Actor 挂接到 Socket 后使用的额外相对位置、旋转和缩放偏移。
	UPROPERTY(EditAnywhere, Category=Equipment)
	FTransform AttachTransform;
};


/**
 * UHodgeEquipmentDefinition
 *
 * Definition of a piece of equipment that can be applied to a pawn
 *
 * 装备的静态定义类，用于描述一件装备应用到 Pawn 后应该提供哪些内容。
 * Definition 只保存装备配置，具体运行时状态由 UHodgeEquipmentInstance 负责。
 */
UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class UHodgeEquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:
	// 构造装备定义对象。
	UHodgeEquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Class to spawn

	// 装备时需要创建的运行时 EquipmentInstance 类型。
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TSubclassOf<UHodgeEquipmentInstance> InstanceType;

	// Gameplay ability sets to grant when this is equipped

	// 装备时需要授予给 Pawn AbilitySystemComponent 的 AbilitySet 列表。
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TArray<TObjectPtr<const UHodgeAbilitySet>> AbilitySetsToGrant;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Attributes")
	TObjectPtr<UHodgeEquipmentStatProfile> StatProfile;

	// Actors to spawn on the pawn when this is equipped

	// 装备时需要生成并挂接到 Pawn 上的 Actor 配置列表。
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TArray<FHodgeEquipmentActorToSpawn> ActorsToSpawn;
};
