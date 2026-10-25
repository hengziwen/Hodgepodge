/** @file HodgeEncounterDefinition.h
 * @brief 保存交互与区域刷怪点的波次、初始化、重复与单物品奖励配置。
 */
#pragma once

#include "CoreMinimal.h"
#include "Encounter/HodgeEncounterTypes.h"
#include "Engine/DataAsset.h"
#include "HodgeEncounterDefinition.generated.h"

class UHodgeInventoryItemDefinition;

/** 静态校验不替代运行时 EnemyInit、控制器与 Brain 的实际就绪检查。 */
UCLASS(BlueprintType)
class HODGEPODGE_API UHodgeEncounterDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Encounter")
	EHodgeEncounterMode Mode = EHodgeEncounterMode::Interaction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Encounter")
	TArray<FHodgeEncounterWave> Waves;

	/** 依赖未就绪与初始化失败分别报告，超时不计合法击杀。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Encounter", meta=(ClampMin="0.001", Units="s"))
	float EnemyInitializationTimeout = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Encounter|Area", meta=(EditCondition="Mode == EHodgeEncounterMode::Area", Units="cm"))
	FVector AreaHalfExtent = FVector(300.0, 300.0, 150.0);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Encounter|Area", meta=(EditCondition="Mode == EHodgeEncounterMode::Area"))
	EHodgeEncounterTriggerEdge TriggerEdge = EHodgeEncounterTriggerEdge::Enter;

	/** 仅区域使用重复策略；交互型领奖成功后总是恢复初始状态。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Encounter|Area", meta=(EditCondition="Mode == EHodgeEncounterMode::Area"))
	bool bRepeatable = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Encounter|Area", meta=(EditCondition="Mode == EHodgeEncounterMode::Area", ClampMin="0", Units="s"))
	float CooldownSeconds = 30.0f;

	/** 奖励仅交互型使用，区域型清场自动完成且不进入领取状态。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Encounter|Reward", meta=(EditCondition="Mode == EHodgeEncounterMode::Interaction"))
	TSubclassOf<UHodgeInventoryItemDefinition> RewardDefinition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Encounter|Reward", meta=(EditCondition="Mode == EHodgeEncounterMode::Interaction", ClampMin="1"))
	int32 RewardCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Encounter|Reward", meta=(EditCondition="Mode == EHodgeEncounterMode::Interaction", ClampMin="1"))
	int32 RewardLevel = 1;

	/** Runtime 与编辑器复用同一配置校验，不修改资产或限制额外批次数量。 */
	bool Validate(FString& OutError) const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
