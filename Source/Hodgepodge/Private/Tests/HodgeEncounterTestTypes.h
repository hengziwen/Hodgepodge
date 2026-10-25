/** @file HodgeEncounterTestTypes.h @brief 刷怪回归使用的正常敌人依赖与同步异常注入。 */
#pragma once

#include "AI/HodgeEnemyAIController.h"
#include "Character/HodgeEnemyCharacter.h"
#include "Inventory/HodgeInventoryItemDefinition.h"
#include "HodgeEncounterTestTypes.generated.h"

/** 注入发生在真实构造和初始化链中，不代替刷怪器成员登记或结算。 */
enum class EHodgeEncounterEnemyTestMode : uint8
{
	Normal,
	DeathDuringFirstConstruction,
	DeathThenDestroyDuringConstruction,
	DestroyDuringConstruction,
	InvalidInitialization,
	WaitForCombat,
	InvalidBrain
};

/** 每个隔离用例配对重置，避免 CDO 或正式资产被修改。 */
struct FHodgeEncounterEnemyTestControl
{
	static EHodgeEncounterEnemyTestMode Mode;
	static int32 ConstructionCount;
	static int32 FailureOrdinal;
	static TArray<TWeakObjectPtr<AHodgeEnemyCharacter>> ConstructedEnemies;
	static void Reset();
};

/** 只在 Possess 前准备一个等待行为树，Brain 启动仍由生产控制器执行。 */
UCLASS(Transient, NotBlueprintable)
class AHodgeEncounterTestAIController : public AHodgeEnemyAIController
{
	GENERATED_BODY()
protected:
	virtual void OnPossess(APawn* InPawn) override;
};

/** 普通敌人测试类在构造时接上实际依赖，供生产 deferred Spawn 使用。 */
UCLASS(Transient, NotBlueprintable)
class AHodgeEncounterTestEnemy : public AHodgeEnemyCharacter
{
	GENERATED_BODY()
public:
	AHodgeEncounterTestEnemy(const FObjectInitializer& ObjectInitializer);
protected:
	virtual void OnConstruction(const FTransform& Transform) override;
};

/** 保留真实抽象类标记，用于配置预检拒绝不可实例化的敌人类。 */
UCLASS(Abstract, Transient, NotBlueprintable)
class AHodgeEncounterAbstractTestEnemy : public AHodgeEnemyCharacter
{
	GENERATED_BODY()
};

/** 原生非装备物品使奖励专项不依赖正式二进制资产。 */
UCLASS(Transient, NotBlueprintable)
class UHodgeEncounterTestReward : public UHodgeInventoryItemDefinition
{
	GENERATED_BODY()
public:
	UHodgeEncounterTestReward();
};

/** 独立定义占满唯一库存条目，验证领取失败保留待领取状态。 */
UCLASS(Transient, NotBlueprintable)
class UHodgeEncounterTestOtherItem : public UHodgeInventoryItemDefinition
{
	GENERATED_BODY()
};
