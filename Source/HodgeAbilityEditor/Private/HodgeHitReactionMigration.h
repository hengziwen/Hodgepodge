/** @file HodgeHitReactionMigration.h @brief 预览并执行不改变既有受击行为的单资产布局迁移。 */
#pragma once

#include "CoreMinimal.h"
#include "Data/HodgeHitReactionProfile.h"

/** 预览只保存弱目标和差异；执行前重新核实来源及全部转换条件。 */
struct FHodgeHitReactionMigrationPlan
{
	TWeakObjectPtr<UHodgeHitReactionProfile> Profile;
	FString SourceFingerprint;
	FString BackupPackageName;
	FHodgeCompactReactionAnimations Compact;
	TArray<FString> Referencers;
	TArray<FText> Changes;
	TArray<FText> Errors;
	bool bAlreadyCompact = false;

	/** 已使用精简布局或存在任何诊断时，不提供转换操作。 */
	bool CanConvert() const;
};

/** 仅写布局开关及 Compact；旧数组和其余属性均由原资产继续保存。 */
class FHodgeHitReactionMigration
{
public:
	/** 读取配置、引用者、无损差异及备份目标；绝不创建、修改或保存资产。 */
	static bool BuildPlan(UHodgeHitReactionProfile* Profile, const FString& BackupDirectory,
		FHodgeHitReactionMigrationPlan& OutPlan);
	/** 复核预览，先保存备份，再在可撤销事务中转换并自动保存目标。 */
	static bool Execute(const FHodgeHitReactionMigrationPlan& Plan, TArray<FText>& OutMessages);
};
