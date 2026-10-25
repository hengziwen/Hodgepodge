/** @file HodgeEncounterDefinition.cpp
 * @brief 校验刷怪静态配置，等级与奖励复用项目已有定义规则。
 */
#include "Data/HodgeEncounterDefinition.h"

#include "Character/HodgeEnemyCharacter.h"
#include "Data/HodgeCharacterStatProfile.h"
#include "Data/HodgePawnData.h"
#include "Inventory/HodgeInventoryItemDefinition.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeEncounterDefinition)

bool UHodgeEncounterDefinition::Validate(FString& OutError) const
{
	OutError.Reset();
	if (Mode != EHodgeEncounterMode::Interaction && Mode != EHodgeEncounterMode::Area)
	{
		OutError = TEXT("刷怪入口模式无效");
		return false;
	}
	if (!FMath::IsFinite(EnemyInitializationTimeout) || EnemyInitializationTimeout <= 0.0f)
	{
		OutError = TEXT("敌人初始化超时必须为有限正数");
		return false;
	}
	if (Waves.IsEmpty())
	{
		OutError = TEXT("刷怪配置至少需要一波敌人");
		return false;
	}
	for (int32 WaveIndex = 0; WaveIndex < Waves.Num(); ++WaveIndex)
	{
		const FHodgeEncounterWave& Wave = Waves[WaveIndex];
		if (!FMath::IsFinite(Wave.DelayBeforeSpawn) || Wave.DelayBeforeSpawn < 0.0f || Wave.Enemies.IsEmpty())
		{
			OutError = FString::Printf(TEXT("第 %d 波必须包含敌人且生成等待时间须为有限非负数"), WaveIndex + 1);
			return false;
		}
		for (int32 EnemyIndex = 0; EnemyIndex < Wave.Enemies.Num(); ++EnemyIndex)
		{
			const FHodgeEncounterSpawn& Enemy = Wave.Enemies[EnemyIndex];
			const UHodgePawnData* PawnData = Enemy.PawnData;
			const UClass* PawnClass = IsValid(PawnData) ? PawnData->PawnClass.Get() : nullptr;
			if (!IsValid(PawnData) || !PawnClass || PawnClass->HasAnyClassFlags(CLASS_Abstract) ||
				!PawnClass->IsChildOf(AHodgeEnemyCharacter::StaticClass()))
			{
				OutError = FString::Printf(TEXT("第 %d 波第 %d 个条目需要非抽象普通敌人的 PawnData/PawnClass"), WaveIndex + 1, EnemyIndex + 1);
				return false;
			}
			if (Enemy.RelativeTransform.ContainsNaN())
			{
				OutError = FString::Printf(TEXT("第 %d 波第 %d 个敌人的相对出生变换包含非有限值"), WaveIndex + 1, EnemyIndex + 1);
				return false;
			}
			TArray<FText> StatErrors;
			float Health = 0.0f;
			float Damage = 0.0f;
			// 合法等级由已有 StatProfile 决定，不另设刷怪专用等级上限。
			if (Enemy.Level < 1 || !PawnData->StatProfile || !PawnData->StatProfile->Validate(StatErrors) ||
				!PawnData->StatProfile->Evaluate(Enemy.Level, Health, Damage))
			{
				const FString Details = StatErrors.IsEmpty() ? FString() : FString::Printf(TEXT(": %s"), *StatErrors[0].ToString());
				OutError = FString::Printf(TEXT("第 %d 波第 %d 个敌人的等级或 StatProfile 无效%s"), WaveIndex + 1, EnemyIndex + 1,
					*Details);
				return false;
			}
		}
	}
	if (Mode == EHodgeEncounterMode::Area)
	{
		// 区域参数不约束交互型资产，区域也不要求配置不会使用的奖励。
		if (AreaHalfExtent.ContainsNaN() || AreaHalfExtent.X <= 0.0 || AreaHalfExtent.Y <= 0.0 || AreaHalfExtent.Z <= 0.0 ||
			!FMath::IsFinite(CooldownSeconds) || CooldownSeconds < 0.0f ||
			(TriggerEdge != EHodgeEncounterTriggerEdge::Enter && TriggerEdge != EHodgeEncounterTriggerEdge::Exit))
		{
			OutError = TEXT("区域半尺寸必须为有限正数，冷却须为有限非负数，触发边沿必须有效");
			return false;
		}
	}
	else
	{
		const UHodgeInventoryItemDefinition* Reward = RewardDefinition ? GetDefault<UHodgeInventoryItemDefinition>(RewardDefinition) : nullptr;
		FString RewardError;
		if (!Reward || RewardDefinition->HasAnyClassFlags(CLASS_Abstract) || RewardCount < 1 || RewardLevel < 1 ||
			!Reward->Validate(RewardLevel, RewardError))
		{
			OutError = RewardError.IsEmpty() ? TEXT("交互型需要合法的非抽象奖励定义、正数量与合法等级")
				: FString::Printf(TEXT("交互奖励配置无效: %s"), *RewardError);
			return false;
		}
	}
	return true;
}

#if WITH_EDITOR
EDataValidationResult UHodgeEncounterDefinition::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult Parent = Super::IsDataValid(Context);
	FString Error;
	if (!Validate(Error))
	{
		Context.AddError(FText::FromString(Error));
		return EDataValidationResult::Invalid;
	}
	return Parent == EDataValidationResult::Invalid ? EDataValidationResult::Invalid : EDataValidationResult::Valid;
}
#endif
