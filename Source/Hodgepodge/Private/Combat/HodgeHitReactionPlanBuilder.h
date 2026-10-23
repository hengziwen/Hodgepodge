/** @file HodgeHitReactionPlanBuilder.h @brief 同步读取受击上下文快照，计算现有权威计划的候选值。 */
#pragma once

#include "CoreMinimal.h"
#include "Combat/HodgeHitReactionTypes.h"

class UHodgeHitReactionProfile;
class USkeletalMesh;

/** 仅限游戏线程的一次同步调用；Profile/Mesh 为借用引用，不持久化或延长对象生命周期。 */
struct FHodgeHitReactionPlanContext
{
	FHodgeHitReactionConfig Request;
	FHodgeHitPresentation Presentation;
	// 受击补转单独消费命中时冻结的位置，不改变选片或世界运动来源。
	FHodgeHitReactionFacingSource FacingSource;
	EHodgeReactionLevel ReactionLevel = EHodgeReactionLevel::None;
	FHodgeHitReactionState State;
	const UHodgeHitReactionProfile* Profile = nullptr;
	const USkeletalMesh* Mesh = nullptr;
	// Origin 沿用命中快照；目标与有效来源的朝向在构建时同步采样。
	FVector Origin = FVector::ZeroVector;
	FVector TargetLocation = FVector::ZeroVector;
	FVector TargetForward = FVector::ForwardVector;
	FVector SourceForward = FVector::ForwardVector;
	float ServerTime = 0.f;
	// 只参与变体选择，真正的序号递增仍由组件提交计划负责。
	int32 Variation = 0;
	bool bSourceValid = false;
	// 包含已提交 Launch 但尚待下一次物理更新的空中语义。
	bool bAir = false;
	bool bCanMove = false;
	// 仅在最终主片非空时消费；合法无动画兼容路径不因此失败。
	bool bHasAnimInstance = false;
};

/** 只描述成功计划的可选补转值；不授予朝向权限，也不承担控制链身份。 */
struct FHodgeHitReactionFacingCandidate
{
	EHodgeHitReactionFacingPreset Preset = EHodgeHitReactionFacingPreset::None;
	float SourceYaw = 0.f;
	float TargetYaw = 0.f;
	float MaxCorrectionAngle = 0.f;
	float WindowDuration = 0.f;
	float MaxYawSpeed = 0.f;
	float DeadZoneAngle = 0.f;
	bool bHasTarget = false;
};

/** 只计算计划，不授权、不提交状态、不执行动画或运动，也不保存跨帧数据。 */
class FHodgeHitReactionPlanBuilder
{
public:
	/** 每次清空诊断，保留现有 Plan/Applied 的顺序写入与部分失败输出；仅 true 的输出可提交。 */
	static bool Build(const FHodgeHitReactionPlanContext& Context, FHodgeHitReactionState& OutPlan,
		TArray<EHodgeImpactType>& OutAppliedImpacts, TArray<FText>& OutErrors);
	/** 原计划和GA提交成功后仅计算可选值；重置Out，保留本级尝试，错误追加且不拒绝原受击。
	 * @return true表示未新增已消费数值的错误；关闭、排除或无目标仍可成功，实际转向须检查bHasTarget。
	 */
	static bool BuildFacingCandidate(const FHodgeHitReactionPlanContext& Context, const FHodgeHitReactionState& SuccessfulPlan,
		float ActualStartYaw, FHodgeHitReactionFacingCandidate& Out, TArray<FText>& Errors);
};
