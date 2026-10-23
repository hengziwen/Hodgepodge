/** @file HodgeAbilityDefinition.cpp
 * @brief 实现单段技能执行定义相关的类型、接口与现有处理流程。
 */
#include "Data/HodgeAbilityDefinition.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "Animation/AnimMontage.h"
#include "MotionWarpingComponent.h"
#include "AnimNotifyState_MotionWarping.h"
#include "Animation/HodgeRootMotionModifier_AttackFacing.h"
#include "Animation/HodgeCombatAnimNotifies.h"
#include "Animation/HodgeAnimNotifyState_CinematicCamera.h"
#include "Data/HodgeCinematicCameraProfile.h"
#include "Combat/HodgeAttackRule.h"
#include "Combat/HodgeLegacyAttackRuleAdapter.h"
#include "AbilitySystem/HodgeGameplayTags.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAbilityDefinition)

/** 标准窗口沿用现有通知标签，取消混合仍由 ExecutionConfig 决定。 */
UHodgeAbilityDefinition::UHodgeAbilityDefinition()
{
	StandaloneMoveCancelWindowTag = HodgeGameplayTags::Status_Attack_Cancel_Move;
	HodgeLegacyAttackRules::InitializeDefaults(*this);
}

/** 构造混合。 */
FAlphaBlend FHodgeAbilityBlendSettings::MakeBlend() const
{
	FAlphaBlend Result;
	Result.SetBlendTime(Time);
	Result.SetBlendOption(Curve);
	Result.SetCustomCurve(CustomCurve);
	return Result;
}

/** 检查当前对象、配置或句柄是否满足其有效性条件。 */
bool FHodgeAbilityBlendSettings::IsValid() const
{
	return FMath::IsFinite(Time) && Time >= 0.f
		&& Mode == EMontageBlendMode::Standard
		&& (Curve != EAlphaBlendOption::Custom || CustomCurve != nullptr);
}

/** 根据当前配置计算动作持续时间。 */
float UHodgeAbilityDefinition::GetDuration() const
{
	return ExecutionConfig.Montage ? ExecutionConfig.Montage->GetPlayLength() : 0.f;
}

/** 窗口仅来自原生通知，避免配置和动画轨维护两份时间。 */
bool UHodgeAbilityDefinition::GetFacingWarpWindow(float& OutStart, float& OutEnd) const
{
	if (!ExecutionConfig.Montage) { return false; }
	TArray<FMotionWarpingWindowData> Windows;
	UMotionWarpingUtilities::GetMotionWarpingWindowsForWarpTargetFromAnimation(ExecutionConfig.Montage,FacingWarp.WarpTargetName,Windows);
	if (Windows.Num()!=1 || !Windows[0].AnimNotify || !Windows[0].AnimNotify->RootMotionModifier ||
		!Windows[0].AnimNotify->RootMotionModifier->IsA<UHodgeRootMotionModifier_AttackFacing>()) { return false; }
	OutStart=Windows[0].StartTime; OutEnd=Windows[0].EndTime; return true;
}

/** 检查定义配置及其引用资源是否满足执行契约。 */
bool UHodgeAbilityDefinition::ValidateDefinition(TArray<FText>& Errors) const
{
	const int32 Before = Errors.Num();
	TSet<UClass*> Types;
	for (const UHodgeAttackRuleConfig* Rule : AttackRules)
	{
		if (!IsValid(Rule) || Types.Contains(Rule->GetClass())) { Errors.Add(FText::FromString(TEXT("Missing or duplicate attack rule configuration"))); continue; }
		Types.Add(Rule->GetClass()); Rule->ValidateRule(*this, Errors);
	}
	HodgeLegacyAttackRules::Validate(*this, Errors);
	if (RequiredSourceTags.HasAny(BlockedSourceTags))
	{ Errors.Add(FText::FromString(TEXT("Variant required and blocked tags conflict"))); }
	auto Error = [&Errors](const FString& Message)
	{
		Errors.Add(FText::FromString(Message));
	};
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!AbilityTag.IsValid() || !AbilityClass)
	{
		Error(TEXT("AbilityTag / AbilityClass is required"));
	}
	if (AbilityClass && !AbilityClass->IsChildOf(UHodgeGameplayAbility_Definition::StaticClass()))
	{
		Error(TEXT("AbilityClass must derive from HodgeGameplayAbility_Definition"));
	}
	if (AbilityClass)
	{
		const auto* Ability = AbilityClass->GetDefaultObject<UHodgeGameplayAbility>();
		if (Ability->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::InstancedPerActor
			|| Ability->GetNetExecutionPolicy() != EGameplayAbilityNetExecutionPolicy::LocalPredicted)
		{
			Error(TEXT("Definition abilities require InstancedPerActor and LocalPredicted"));
		}
	}
	FString FacingError; if (!FacingWarp.Validate(FacingError)) { Error(FacingError); }
	if (FacingWarp.bEnabled)
	{
		float Start=0.f, End=0.f;
		if (!GetFacingWarpWindow(Start,End) || !FMath::IsFinite(Start) || !FMath::IsFinite(End) || Start<0.f || End<=Start || End>GetDuration())
		{ Error(TEXT("Attack facing requires one valid Hodge rotation-warp window in the Montage.")); }
		if (!ExecutionConfig.Montage || !ExecutionConfig.Montage->HasRootMotion()) { Error(TEXT("Attack facing requires a root-motion Montage.")); }
		float FirstHit=MAX_flt, Recovery=MAX_flt;
		if (ExecutionConfig.Montage) for (const auto& Notify : ExecutionConfig.Montage->Notifies)
		{
			const float Time=Notify.GetTriggerTime();
			if ((Notify.NotifyStateClass && Notify.NotifyStateClass->IsA<UHodgeAnimNotifyState_HitCheck>()) || (Notify.Notify && Notify.Notify->IsA<UHodgeAnimNotify_Hit>())) { FirstHit=FMath::Min(FirstHit,Time); }
			if (const auto* Tag=Cast<UHodgeAnimNotifyState_GameplayTag>(Notify.NotifyStateClass))
			{
				if (Tag->StateTag==HodgeGameplayTags::Status_Attack_Recovery) { Recovery=FMath::Min(Recovery,Time); }
				if (Tag->StateTag==HodgeGameplayTags::Status_Rotation_Locked && Time<End && Time+Notify.GetDuration()>Start) { Error(TEXT("Attack rotation lock must not overlap the Warp window.")); }
			}
		}
		if (FirstHit==MAX_flt || End>=FirstHit || Recovery<End) { Error(TEXT("Warp must end before the first hit and before recovery.")); }
	}
	DefaultHitConfig.Reaction.Validate(Errors);
	if (ExecutionBodyTag.IsValid() && HodgeHitReaction::BodyRank(ExecutionBodyTag) == 0)
	{ Error(TEXT("ExecutionBodyTag must be an exact State.Combat.Body leaf tag.")); }
	const auto& C = ExecutionConfig;
	if (!C.Montage) { Error(TEXT("Montage is required")); }
	if (!FMath::IsFinite(C.PlayRate) || C.PlayRate <= 0.f)
	{
		Error(TEXT("PlayRate must be finite and positive"));
	}
	if (!C.BlendIn.IsValid() || !C.NaturalBlendOut.IsValid() || !C.StopBlendOut.IsValid())
	{
		Error(
			TEXT("Invalid blend settings: V1 requires Standard mode, finite nonnegative time, and a curve for Custom"));
	}
	if (C.Montage)
	{
		if (!FMath::IsFinite(GetDuration()) || GetDuration() <= 0.f)
		{
			Error(TEXT("Montage length must be finite and positive"));
		}
		if (C.Montage->BlendModeIn != EMontageBlendMode::Standard)
		{
			Error(TEXT("V1 requires a Standard Montage BlendModeIn"));
		}
		if (!FMath::IsFinite(C.Montage->RateScale) || C.Montage->RateScale <= 0.f)
		{
			Error(TEXT("Montage RateScale must be finite and positive"));
		}
	}
	if (ExecutionRoute == EHodgeAbilityExecutionRoute::ComboCoordinated && InputTag.IsValid()) { Error(TEXT("InputTag is for Standalone definitions only.")); }
	// 非法阈值拒绝授予；空窗口标签是显式关闭，不是配置错误。
	if (ExecutionRoute == EHodgeAbilityExecutionRoute::Standalone &&
		(!FMath::IsFinite(StandaloneMoveIntentThreshold) || StandaloneMoveIntentThreshold <= 0.f || StandaloneMoveIntentThreshold > 1.f))
	{ Error(TEXT("StandaloneMoveIntentThreshold must be finite and in (0, 1].")); }
	// 能力自行校验效果契约，通用检测配置不限定 GE 的执行方式。
	if (AbilityClass && AbilityClass->IsChildOf(UHodgeGameplayAbility_Definition::StaticClass()))
	{
		AbilityClass->GetDefaultObject<UHodgeGameplayAbility_Definition>()->ValidateExecutionConfiguration(*this, Errors);
	}
	return Before == Errors.Num();
}
#if WITH_EDITOR
/** 在编辑器校验当前配置并收集诊断。 */
EDataValidationResult UHodgeAbilityDefinition::IsDataValid(FDataValidationContext& Context) const
{
	const auto Parent = Super::IsDataValid(Context);
	TArray<FText> Errors;
	ValidateDefinition(Errors);
	// 编辑器报告电影配置错误，运行时镜头失败不替代正式技能和伤害权限。
	int32 CameraWindows=0;
	if (ExecutionConfig.Montage)
	{
		for (const auto& Event : ExecutionConfig.Montage->Notifies)
		{
			if (const auto* Window=Cast<UHodgeAnimNotifyState_CinematicCamera>(Event.NotifyStateClass))
			{
				++CameraWindows;
				if (!Window->Profile) { Errors.Add(FText::FromString(TEXT("Cinematic window requires a profile"))); }
				else { Window->Profile->Validate(Errors); }
				if (Event.GetTriggerTime()<0.f || Event.GetEndTriggerTime()>ExecutionConfig.Montage->GetPlayLength()*.5f+UE_KINDA_SMALL_NUMBER || Event.GetDuration()<=0.f)
				{ Errors.Add(FText::FromString(TEXT("Cinematic window must have positive duration within montage first half"))); }
			}
		}
	}
	if (CameraWindows>1) { Errors.Add(FText::FromString(TEXT("First cinematic version supports one continuous window per montage"))); }
	for (const FText& Error : Errors)
	{
		Context.AddError(Error);
	}
	return Errors.IsEmpty() && Parent != EDataValidationResult::Invalid
		       ? EDataValidationResult::Valid
		       : EDataValidationResult::Invalid;
}
#endif

/** 新配置是唯一执行来源；兼容层只补充尚未迁移的规则。 */
bool UHodgeAbilityDefinition::CreateAttackRules(UHodgeGameplayAbility_Definition* Source, TArray<TObjectPtr<UHodgeAttackRule>>& OutRules) const
{
 OutRules.Reset();
 for (const UHodgeAttackRuleConfig* Config : AttackRules)
 {
  if (!IsValid(Config)) { return false; }
  auto* Rule = Config->CreateRule(Source);
  if (!Rule) { return false; }
  OutRules.Add(Rule); Rule->Initialize(Source, Config);
 }
 HodgeLegacyAttackRules::CreateRules(*this, Source, OutRules);
 return true;
}
