/** @file HodgeEncounterTestTypes.cpp @brief 通过真实 EnemyInit、Possess、Health 事件注入刷怪生命周期边界。 */
#include "HodgeEncounterTestTypes.h"
#include "HodgeEnemyAITestSupport.h"
#include "Component/HodgeCombatComponentBase.h"
#include "Component/HodgeHealthComponent.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Data/HodgePawnData.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/Composites/BTComposite_Sequence.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeEncounterTestTypes)

EHodgeEncounterEnemyTestMode FHodgeEncounterEnemyTestControl::Mode = EHodgeEncounterEnemyTestMode::Normal;
int32 FHodgeEncounterEnemyTestControl::ConstructionCount = 0;
int32 FHodgeEncounterEnemyTestControl::FailureOrdinal = 1;
TArray<TWeakObjectPtr<AHodgeEnemyCharacter>> FHodgeEncounterEnemyTestControl::ConstructedEnemies;

void FHodgeEncounterEnemyTestControl::Reset()
{
	Mode = EHodgeEncounterEnemyTestMode::Normal;
	ConstructionCount = 0;
	FailureOrdinal = 1;
	ConstructedEnemies.Reset();
}

void AHodgeEncounterTestAIController::OnPossess(APawn* InPawn)
{
	// 沿用敌人初始化专项的最小等待树，动作与属性不做测试侧直接授予。
	if (FHodgeEncounterEnemyTestControl::Mode != EHodgeEncounterEnemyTestMode::InvalidBrain)
	{
		auto* Tree = NewObject<UBehaviorTree>(this);
		Tree->BlackboardAsset = NewObject<UBlackboardData>(Tree);
		auto* Root = NewObject<UBTComposite_Sequence>(Tree);
		auto* Wait = NewObject<UBTTask_Wait>(Tree);
		Wait->WaitTime = 90.f;
		FBTCompositeChild Child;
		Child.ChildTask = Wait;
		Root->Children.Add(Child);
		Root->InitializeNode(nullptr, 0, 0, 0);
		Wait->InitializeNode(Root, 1, 0, 1);
		Root->InitializeComposite(1);
		Tree->RootNode = Root;
		BehaviorTree = Tree;
	}
	Super::OnPossess(InPawn);
}

AHodgeEncounterTestEnemy::AHodgeEncounterTestEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 隔离场景使用自身控制器配置，不修改普通敌人的默认类或正式资产。
	AIControllerClass = AHodgeEncounterTestAIController::StaticClass();
}

void AHodgeEncounterTestEnemy::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	const int32 Ordinal = ++FHodgeEncounterEnemyTestControl::ConstructionCount;
	FHodgeEncounterEnemyTestControl::ConstructedEnemies.Add(this);
	const auto Mode = FHodgeEncounterEnemyTestControl::Mode;
	if (Mode == EHodgeEncounterEnemyTestMode::DestroyDuringConstruction && Ordinal == FHodgeEncounterEnemyTestControl::FailureOrdinal)
	{
		// FinishSpawning 中真实销毁，覆盖首个或中途生成失效后的整轮回收。
		Destroy();
		return;
	}
	auto* Init = GetEnemyInitComponent();
	FHodgeEnemyInitTestAccess::WithoutExperience(Init);
	if (Mode == EHodgeEncounterEnemyTestMode::InvalidInitialization)
	{
		// 只替换本实例配置，制造配置加载后初始化发现坏依赖的真实失败。
		EnemyPawnData = NewObject<UHodgePawnData>(this);
	}
	if (Mode != EHodgeEncounterEnemyTestMode::WaitForCombat)
	{
		auto* Combat = NewObject<UHodgeCombatComponentBase>(this);
		AddInstanceComponent(Combat);
		Combat->RegisterComponent();
	}
	// 构造期只尝试等待依赖；控制器由原生 PostInitialize 创建，Spawned 由 PawnExtension BeginPlay 推进。
	Init->TryInitialize();
	if (Mode == EHodgeEncounterEnemyTestMode::DeathDuringFirstConstruction && Ordinal == 1)
	{
		FindComponentByClass<UHodgeHealthComponent>()->StartDeath();
	}
	if (Mode == EHodgeEncounterEnemyTestMode::DeathThenDestroyDuringConstruction && Ordinal == FHodgeEncounterEnemyTestControl::FailureOrdinal)
	{
		// 先发死亡再构造失效也必须判为生成失败，不能利用已发生的事件伪装清波。
		FindComponentByClass<UHodgeHealthComponent>()->StartDeath();
		Destroy();
	}
}

UHodgeEncounterTestReward::UHodgeEncounterTestReward()
{
	// 保持单纯堆叠物品，奖励专项只验证已有库存提交和状态转换。
	DisplayName = FText::FromString(TEXT("刷怪测试奖励"));
	MaxStackCount = 20;
}
