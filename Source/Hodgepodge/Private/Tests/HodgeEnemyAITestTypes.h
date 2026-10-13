/** @file HodgeEnemyAITestTypes.h
 * @brief 声明怪物测试专用配置与Actor相关的类型、接口与现有处理流程。
 */
#pragma once
#include "Data/HodgeAbilitySet.h"
#include "Equipment/HodgeEquipmentDefinition.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "GameplayEffect.h"
#include "GameFramework/Actor.h"
#include "HodgeEnemyAITestTypes.generated.h"

/** 封装敌人测试能力设置的状态与操作接口。 */
UCLASS(Transient, NotBlueprintable)
class UHodgeEnemyTestAbilitySet : public UHodgeAbilitySet
{
	GENERATED_BODY()
public:
	/** 添加能力。
	 * @param Class 类型，由调用方传入。
	 */
	void AddAbility(TSubclassOf<UHodgeGameplayAbility> Class);
	/** 添加效果。
	 * @param Class 类型，由调用方传入。
	 */
	void AddEffect(TSubclassOf<UGameplayEffect> Class);
};

/** 封装敌人测试装备的状态与操作接口。 */
UCLASS(Transient, NotBlueprintable)
class UHodgeEnemyTestEquipment : public UHodgeEquipmentDefinition
{
	GENERATED_BODY()
public:
	/** 构造敌人测试装备并初始化其默认状态。 */
	UHodgeEnemyTestEquipment();
};

/** 封装敌人Rebind事件授予能力的状态与操作接口。 */
UCLASS(Transient, NotBlueprintable)
class UHodgeEnemyRebindOnGrantAbility : public UHodgeGameplayAbility
{
	GENERATED_BODY()
protected:
	/** 处理授予能力。
	 * @param Info 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param Spec 规格，由调用方传入。
	 */
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* Info, const FGameplayAbilitySpec& Spec) override;
};

/** 封装敌人事件生成测试能力的状态与操作接口。 */
UCLASS(Transient, NotBlueprintable)
class UHodgeEnemyOnSpawnTestAbility : public UHodgeGameplayAbility
{
	GENERATED_BODY()
public:
	/** 构造敌人事件生成测试能力并初始化其默认状态。 */
	UHodgeEnemyOnSpawnTestAbility();
};

/** 封装敌人装备测试Actor对象的状态与操作接口。 */
UCLASS(Transient, NotBlueprintable)
class AHodgeEnemyEquipmentTestActor : public AActor
{
	GENERATED_BODY()
public:
	/** 构造敌人装备测试Actor对象并初始化其默认状态。 */
	AHodgeEnemyEquipmentTestActor();
};

/** 封装敌人Instant测试效果的状态与操作接口。 */
UCLASS(Transient, NotBlueprintable)
class UHodgeEnemyInstantTestEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	/** 构造敌人Instant测试效果并初始化其默认状态。 */
	UHodgeEnemyInstantTestEffect();
};

/** 封装敌人装备FailActor对象的状态与操作接口。 */
UCLASS(Transient, NotBlueprintable)
class AHodgeEnemyEquipmentFailActor : public AHodgeEnemyEquipmentTestActor
{
	GENERATED_BODY()
protected:
	/** 处理Construction。
	 * @param Transform 变换，由调用方传入。
	 */
	virtual void OnConstruction(const FTransform& Transform) override;
};
