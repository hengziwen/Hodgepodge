/** @file HodgeEncounterDefinitionTests.cpp
 * @brief 对真实刷怪配置 Validate 执行合法与拒绝矩阵，不复制生产校验算法。
 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"

#include "HodgeEncounterTestTypes.h"
#include "Character/HodgeEnemyCharacter.h"
#include "Data/HodgeCharacterStatProfile.h"
#include "Data/HodgeEncounterDefinition.h"
#include "Data/HodgePawnData.h"
#include "GameFramework/Pawn.h"
#include "UObject/StrongObjectPtr.h"
#include <limits>

namespace HodgeEncounterDefinitionTests
{
	/** 只创建真实配置对象；不加载正式资产，也不把静态合法性当作运行就绪。 */
	struct FFixture
	{
		TStrongObjectPtr<UHodgeEncounterDefinition> Definition;
		UHodgePawnData* PawnData = nullptr;

		FFixture() : Definition(NewObject<UHodgeEncounterDefinition>())
		{
			PawnData = NewObject<UHodgePawnData>(Definition.Get());
			PawnData->PawnClass = AHodgeEnemyCharacter::StaticClass();
			PawnData->StatProfile = NewObject<UHodgeCharacterStatProfile>(PawnData);
			FHodgeEncounterSpawn Enemy;
			Enemy.PawnData = PawnData;
			FHodgeEncounterWave Wave;
			Wave.Enemies.Add(Enemy);
			Definition->Waves.Add(Wave);
			Definition->RewardDefinition = UHodgeEncounterTestReward::StaticClass();
		}

		FHodgeEncounterSpawn& Spawn() { return Definition->Waves[0].Enemies[0]; }
	};

	/** 每例从合法配置独立变更一次，失败必须由生产 API 返回且带可定位诊断。 */
	void Rejects(FAutomationTestBase& Test, const TCHAR* Name, TFunctionRef<void(FFixture&)> Change)
	{
		FFixture F;
		Change(F);
		FString Error;
		Test.TestFalse(Name, F.Definition->Validate(Error));
		Test.TestFalse(FString::Printf(TEXT("%s 包含失败诊断"), Name), Error.IsEmpty());
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEncounterDefinitionValidTest,
	"Hodge.Encounter.Configuration.ValidModesAndDormantFields", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeEncounterDefinitionValidTest::RunTest(const FString& Parameters)
{
	using namespace HodgeEncounterDefinitionTests;
	FFixture F;
	FString Error = TEXT("旧诊断");
	TestTrue(TEXT("交互默认配置通过真实校验"), F.Definition->Validate(Error));
	TestTrue(TEXT("成功清空旧诊断"), Error.IsEmpty());
	F.Spawn().Level = F.PawnData->StatProfile->MaxLevel;
	F.Spawn().RelativeTransform = FTransform(FRotator(0.0, 45.0, 0.0), FVector(100.0, -50.0, 20.0), FVector(1.0, 2.0, 1.0));
	TestTrue(TEXT("已有等级上边界和有限出生变换合法"), F.Definition->Validate(Error));
	F.Definition->Mode = EHodgeEncounterMode::Area;
	F.Definition->RewardDefinition = nullptr;
	F.Definition->RewardCount = 0;
	F.Definition->RewardLevel = 0;
	TestTrue(TEXT("区域不要求休眠奖励字段"), F.Definition->Validate(Error));
	F.Definition->TriggerEdge = EHodgeEncounterTriggerEdge::Exit;
	F.Definition->bRepeatable = false;
	F.Definition->CooldownSeconds = 0.0f;
	TestTrue(TEXT("区域允许离开边沿、一次性配置和零冷却"), F.Definition->Validate(Error));
	// 交互型不读取区域专属配置，避免休眠参数阻止合法资产使用。
	F.Definition->Mode = EHodgeEncounterMode::Interaction;
	F.Definition->RewardDefinition = UHodgeEncounterTestReward::StaticClass();
	F.Definition->RewardCount = 1;
	F.Definition->RewardLevel = 1;
	F.Definition->AreaHalfExtent = FVector::ZeroVector;
	F.Definition->CooldownSeconds = std::numeric_limits<float>::quiet_NaN();
	F.Definition->TriggerEdge = static_cast<EHodgeEncounterTriggerEdge>(255);
	TestTrue(TEXT("交互忽略休眠区域字段"), F.Definition->Validate(Error));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEncounterDefinitionCompositionTest,
	"Hodge.Encounter.Configuration.RejectsWavePawnAndStats", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeEncounterDefinitionCompositionTest::RunTest(const FString& Parameters)
{
	using namespace HodgeEncounterDefinitionTests;
	Rejects(*this, TEXT("拒绝空波次列表"), [](FFixture& F) { F.Definition->Waves.Reset(); });
	Rejects(*this, TEXT("拒绝空敌人波"), [](FFixture& F) { F.Definition->Waves[0].Enemies.Reset(); });
	Rejects(*this, TEXT("拒绝空 PawnData"), [](FFixture& F) { F.Spawn().PawnData = nullptr; });
	Rejects(*this, TEXT("拒绝空 PawnClass"), [](FFixture& F) { F.PawnData->PawnClass = nullptr; });
	Rejects(*this, TEXT("拒绝普通敌人以外的 PawnClass"), [](FFixture& F) { F.PawnData->PawnClass = APawn::StaticClass(); });
	Rejects(*this, TEXT("拒绝真实抽象敌人类"), [](FFixture& F) { F.PawnData->PawnClass = AHodgeEncounterAbstractTestEnemy::StaticClass(); });
	Rejects(*this, TEXT("拒绝空 StatProfile"), [](FFixture& F) { F.PawnData->StatProfile = nullptr; });
	Rejects(*this, TEXT("拒绝非法属性等级范围"), [](FFixture& F) { F.PawnData->StatProfile->MinLevel = 0; });
	Rejects(*this, TEXT("拒绝缺失正式属性初始化效果"), [](FFixture& F) { F.PawnData->StatProfile->InitializationEffect = nullptr; });
	Rejects(*this, TEXT("拒绝无生命值的属性配置"), [](FFixture& F) { F.PawnData->StatProfile->MaxHealth = FScalableFloat(0.0f); });
	Rejects(*this, TEXT("拒绝零等级"), [](FFixture& F) { F.Spawn().Level = 0; });
	Rejects(*this, TEXT("拒绝现有属性曲线范围外等级"), [](FFixture& F) { F.Spawn().Level = F.PawnData->StatProfile->MaxLevel + 1; });
	Rejects(*this, TEXT("拒绝属性曲线下界之外等级"), [](FFixture& F) { F.PawnData->StatProfile->MinLevel = 2; });
	Rejects(*this, TEXT("拒绝非法入口枚举"), [](FFixture& F) { F.Definition->Mode = static_cast<EHodgeEncounterMode>(255); });
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEncounterDefinitionTimingAreaTest,
	"Hodge.Encounter.Configuration.RejectsInvalidTimingAndArea", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeEncounterDefinitionTimingAreaTest::RunTest(const FString& Parameters)
{
	using namespace HodgeEncounterDefinitionTests;
	for (const float Delay : {-1.0f, std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::infinity()})
	{
		const FString Name = FString::Printf(TEXT("拒绝负数或非有限波次等待 %g"), Delay);
		Rejects(*this, *Name, [Delay](FFixture& F) { F.Definition->Waves[0].DelayBeforeSpawn = Delay; });
	}
	for (const float Timeout : {0.0f, -1.0f, std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::infinity()})
	{
		const FString Name = FString::Printf(TEXT("拒绝非正或非有限初始化超时 %g"), Timeout);
		Rejects(*this, *Name, [Timeout](FFixture& F) { F.Definition->EnemyInitializationTimeout = Timeout; });
	}
	for (const float Cooldown : {-1.0f, std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::infinity()})
	{
		const FString Name = FString::Printf(TEXT("拒绝区域负数或非有限冷却 %g"), Cooldown);
		Rejects(*this, *Name, [Cooldown](FFixture& F)
		{ F.Definition->Mode = EHodgeEncounterMode::Area; F.Definition->CooldownSeconds = Cooldown; });
	}
	for (int32 Axis = 0; Axis < 3; ++Axis)
	{
		for (const double Extent : {0.0, -1.0, std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::infinity()})
		{
			const FString Name = FString::Printf(TEXT("区域轴 %d 拒绝非正或非有限范围 %g"), Axis, Extent);
			Rejects(*this, *Name, [Axis, Extent](FFixture& F)
			{ F.Definition->Mode = EHodgeEncounterMode::Area; F.Definition->AreaHalfExtent[Axis] = Extent; });
		}
	}
	Rejects(*this, TEXT("区域拒绝非法触发边沿"), [](FFixture& F)
	{ F.Definition->Mode = EHodgeEncounterMode::Area; F.Definition->TriggerEdge = static_cast<EHodgeEncounterTriggerEdge>(255); });
	// 非有限 FTransform 的构造与写入会触发引擎诊断，本矩阵不通过篡改底层内存伪造数据。
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeEncounterDefinitionRewardTest,
	"Hodge.Encounter.Configuration.InteractionRewardOnly", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeEncounterDefinitionRewardTest::RunTest(const FString& Parameters)
{
	using namespace HodgeEncounterDefinitionTests;
	Rejects(*this, TEXT("交互拒绝缺失奖励"), [](FFixture& F) { F.Definition->RewardDefinition = nullptr; });
	Rejects(*this, TEXT("交互拒绝抽象物品基类"), [](FFixture& F) { F.Definition->RewardDefinition = UHodgeInventoryItemDefinition::StaticClass(); });
	Rejects(*this, TEXT("交互拒绝零奖励数量"), [](FFixture& F) { F.Definition->RewardCount = 0; });
	Rejects(*this, TEXT("交互拒绝负奖励数量"), [](FFixture& F) { F.Definition->RewardCount = -1; });
	Rejects(*this, TEXT("交互拒绝非法奖励等级"), [](FFixture& F) { F.Definition->RewardLevel = 0; });
	FFixture Area;
	Area.Definition->Mode = EHodgeEncounterMode::Area;
	Area.Definition->RewardDefinition = UHodgeInventoryItemDefinition::StaticClass();
	Area.Definition->RewardCount = -1;
	Area.Definition->RewardLevel = 0;
	FString Error;
	TestTrue(TEXT("区域忽略抽象定义及非法休眠奖励数值"), Area.Definition->Validate(Error));
	return true;
}
#endif
