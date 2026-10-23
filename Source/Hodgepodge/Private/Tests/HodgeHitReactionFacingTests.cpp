/** @file HodgeHitReactionFacingTests.cpp
 * @brief 验证受击有限补转配置的数值边界、休眠兼容和完整资产检查入口。
 */
#if WITH_DEV_AUTOMATION_TESTS

#include "Data/HodgeHitReactionProfile.h"
#include "Combat/HodgeHitReactionPlanBuilder.h"
#include "Misc/AutomationTest.h"
#include "UObject/UnrealType.h"
#include <limits>

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitReactionFacingPresetValidationTest, "Hodge.HitReaction.FacingConfig.PresetBounds",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 覆盖非法浮点及开闭边界，避免擅自给正时窗或正速率附加上限。 */
bool FHodgeHitReactionFacingPresetValidationTest::RunTest(const FString& Parameters)
{
	const float NaN = std::numeric_limits<float>::quiet_NaN();
	const float Infinity = std::numeric_limits<float>::infinity();
	for (auto Member : {&FHodgeHitReactionFacingPreset::MaxCorrectionAngle,
		&FHodgeHitReactionFacingPreset::WindowDuration, &FHodgeHitReactionFacingPreset::MaxYawSpeed})
	{
		for (const float Invalid : {NaN, Infinity, -Infinity, -1.f, 0.f})
		{
			FHodgeHitReactionFacingPreset Preset;
			Preset.*Member = Invalid;
			TArray<FText> Errors;
			TestFalse(TEXT("每个已消费字段都拒绝非有限或非正数值"), Preset.Validate(Errors));
			TestEqual(TEXT("单字段非法值产生一项诊断"), Errors.Num(), 1);
		}
	}
	FHodgeHitReactionFacingPreset Preset;
	Preset.MaxCorrectionAngle = 180.f;
	Preset.WindowDuration = std::numeric_limits<float>::min();
	Preset.MaxYawSpeed = std::numeric_limits<float>::max();
	TArray<FText> Errors;
	TestTrue(TEXT("180度闭边界以及极小正时窗和极大有限速率合法"), Preset.Validate(Errors));
	Preset.MaxCorrectionAngle = 180.01f;
	TestFalse(TEXT("角度超过180度无效"), Preset.Validate(Errors));
	Preset.MaxCorrectionAngle = std::numeric_limits<float>::min();
	TestTrue(TEXT("大于零的极小有限角度不受额外下限限制"), Preset.Validate(Errors));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitReactionFacingSettingsValidationTest, "Hodge.HitReaction.FacingConfig.SettingsAndDormantCompatibility",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 禁用时不消费休眠坏值，启用后同时核对每组角度与公共死区。 */
bool FHodgeHitReactionFacingSettingsValidationTest::RunTest(const FString& Parameters)
{
	FHodgeHitReactionFacingSettings Dormant;
	Dormant.bEnabled = false;
	Dormant.DeadZoneAngle = std::numeric_limits<float>::quiet_NaN();
	Dormant.Light.MaxCorrectionAngle = 0.f;
	Dormant.Heavy.WindowDuration = std::numeric_limits<float>::infinity();
	Dormant.Launch.MaxYawSpeed = -1.f;
	TArray<FText> Errors;
	TestTrue(TEXT("禁用时休眠非法数值不阻止旧配置"), Dormant.Validate(Errors));
	TestTrue(TEXT("禁用检查不追加诊断"), Errors.IsEmpty());
	TestTrue(TEXT("检查不修正或清空休眠死区"), FMath::IsNaN(Dormant.DeadZoneAngle));
	TestEqual(TEXT("检查不裁剪休眠角度"), Dormant.Light.MaxCorrectionAngle, 0.f);
	Dormant.bEnabled = true;
	TestFalse(TEXT("启用后全部休眠非法字段恢复校验"), Dormant.Validate(Errors));
	for (const TCHAR* Path : {TEXT("Facing.Light"), TEXT("Facing.Heavy"), TEXT("Facing.Launch"), TEXT("Facing.DeadZoneAngle")})
	{
		TestTrue(TEXT("诊断保留具体预设或死区路径"), Errors.ContainsByPredicate([Path](const FText& Error)
		{ return Error.ToString().Contains(Path); }));
	}
	for (auto Member : {&FHodgeHitReactionFacingSettings::Light,
		&FHodgeHitReactionFacingSettings::Heavy, &FHodgeHitReactionFacingSettings::Launch})
	{
		FHodgeHitReactionFacingSettings Settings;
		Settings.bEnabled = true;
		Settings.Light.MaxCorrectionAngle = Settings.Heavy.MaxCorrectionAngle = Settings.Launch.MaxCorrectionAngle = 180.f;
		(Settings.*Member).MaxCorrectionAngle = 20.f;
		Settings.DeadZoneAngle = 20.f;
		Errors.Reset();
		TestTrue(TEXT("死区允许等于任一组形成的最小角"), Settings.Validate(Errors));
		Settings.DeadZoneAngle = 20.01f;
		TestFalse(TEXT("死区超过任一组最大角即无效"), Settings.Validate(Errors));
	}
	FHodgeHitReactionFacingSettings Settings;
	Settings.bEnabled = true;
	Settings.DeadZoneAngle = 0.f;
	Errors.Reset();
	TestTrue(TEXT("零死区合法"), Settings.Validate(Errors));
	for (const float Invalid : {-0.01f, std::numeric_limits<float>::quiet_NaN(),
		std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()})
	{
		Settings.DeadZoneAngle = Invalid;
		Errors.Reset();
		TestFalse(TEXT("死区拒绝负值和非有限值"), Settings.Validate(Errors));
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitReactionFacingDiagnosticAppendTest, "Hodge.HitReaction.FacingConfig.AppendDiagnostics",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 调用者已有错误不影响本次成功，新的错误只能追加而不能覆盖旧诊断。 */
bool FHodgeHitReactionFacingDiagnosticAppendTest::RunTest(const FString& Parameters)
{
	const FText PriorError = FText::FromString(TEXT("调用者既有诊断"));
	TArray<FText> Errors = {PriorError};
	FHodgeHitReactionFacingPreset Preset;
	TestTrue(TEXT("有效预设不因调用者旧错误返回失败"), Preset.Validate(Errors));
	TestEqual(TEXT("有效预设保留既有诊断数量"), Errors.Num(), 1);
	Preset.WindowDuration = 0.f;
	TestFalse(TEXT("非法预设返回本次失败"), Preset.Validate(Errors));
	TestEqual(TEXT("非法预设仅追加新诊断"), Errors.Num(), 2);
	TestTrue(TEXT("既有诊断内容保持"), Errors[0].EqualTo(PriorError));
	FHodgeHitReactionFacingSettings Settings;
	Settings.bEnabled = true;
	TestTrue(TEXT("有效配置同样忽略调用者旧错误"), Settings.Validate(Errors));
	TestEqual(TEXT("有效配置不删除或重复旧诊断"), Errors.Num(), 2);
	Settings.Heavy.MaxYawSpeed = 0.f;
	TestFalse(TEXT("配置将预设错误追加到同一容器"), Settings.Validate(Errors));
	TestEqual(TEXT("配置错误只追加一次"), Errors.Num(), 3);
	Settings.bEnabled = false;
	TestTrue(TEXT("禁用配置仍不因旧错误失败"), Settings.Validate(Errors));
	TestEqual(TEXT("禁用配置保留全部旧诊断"), Errors.Num(), 3);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitReactionFacingProfileIntegrationTest, "Hodge.HitReaction.FacingConfig.ProfileValidationEntry",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 完整资产检查纳入已启用转向，旧动画检查仍独立生效。 */
bool FHodgeHitReactionFacingProfileIntegrationTest::RunTest(const FString& Parameters)
{
	auto* Profile = NewObject<UHodgeHitReactionProfile>();
	FHodgeHitReactionAnimation Animation;
	Animation.Type = EHodgeImpactType::HitStun;
	Animation.bAllowWithoutMontage = true;
	Profile->Animations.Add(Animation);
	const FText PriorError = FText::FromString(TEXT("外层资产既有诊断"));
	TArray<FText> Errors = {PriorError};
	TestTrue(TEXT("合法旧动画资产保留增量成功语义"), Profile->Validate(Errors));
	Profile->Facing.bEnabled = true;
	Profile->Facing.Launch.WindowDuration = 0.f;
	TestFalse(TEXT("完整资产检查纳入尚未消费的已启用预设"), Profile->Validate(Errors));
	TestTrue(TEXT("完整资产诊断可定位Launch时窗"), Errors.ContainsByPredicate([](const FText& Error)
	{ return Error.ToString().Contains(TEXT("Facing.Launch")) && Error.ToString().Contains(TEXT("WindowDuration")); }));
	TestTrue(TEXT("完整资产检查保留调用者诊断"), Errors[0].EqualTo(PriorError));
	Profile->Facing.bEnabled = false;
	TestTrue(TEXT("关闭转向后非法休眠预设不影响合法旧动画"), Profile->Validate(Errors));
	Profile->Animations[0].DefaultDuration = 0.f;
	const int32 Before = Errors.Num();
	TestFalse(TEXT("关闭转向也不能绕过原动画合法性检查"), Profile->Validate(Errors));
	TestTrue(TEXT("原动画错误继续追加"), Errors.Num() > Before);
	return true;
}

namespace HodgeFacingTests
{
	/** 仅用位置值构造候选输入，不创建来源Actor或运行世界。 */
	FHodgeHitReactionPlanContext MakeContext(UHodgeHitReactionProfile* Profile)
	{
		FHodgeHitReactionPlanContext Context;
		Context.Profile = Profile;
		Context.FacingSource.SourceLocation = FVector(0., 100., 1000.);
		Context.FacingSource.TargetLocation = FVector::ZeroVector;
		Context.FacingSource.bValid = true;
		return Context;
	}

	/** 对原计划的反射字段逐项核对，不遗漏世界运动及后续阶段。 */
	void ExpectSamePlan(FAutomationTestBase& Test, const FHodgeHitReactionState& Before, const FHodgeHitReactionState& After)
	{
		for (TFieldIterator<FProperty> It(FHodgeHitReactionState::StaticStruct()); It; ++It)
		{ Test.TestTrue(TEXT("可选候选不改原计划.") + It->GetName(), It->Identical_InContainer(&Before, &After)); }
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitReactionFacingCandidateMappingTest, "Hodge.HitReaction.FacingCandidate.FinalPlanMapping",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 映射依据最终受击类型与等级，不借请求数组或输入模式猜测。 */
bool FHodgeHitReactionFacingCandidateMappingTest::RunTest(const FString& Parameters)
{
	auto* Profile = NewObject<UHodgeHitReactionProfile>();
	Profile->Facing.bEnabled = true;
	auto Context = HodgeFacingTests::MakeContext(Profile);
	FHodgeImpactSpec Requested;
	Requested.Type = EHodgeImpactType::Slam;
	Context.Request.Impacts = {Requested};
	struct FCase
	{
		EHodgeImpactType Type;
		EHodgeReactionLevel Level;
		EHodgeHitReactionPhase Phase;
		EHodgeHitReactionFacingPreset Expected;
	};
	const FCase Cases[] = {
		{EHodgeImpactType::HitStun, EHodgeReactionLevel::None, EHodgeHitReactionPhase::Controlled, EHodgeHitReactionFacingPreset::Light},
		{EHodgeImpactType::HitStun, EHodgeReactionLevel::Light, EHodgeHitReactionPhase::Controlled, EHodgeHitReactionFacingPreset::Light},
		{EHodgeImpactType::HitStun, EHodgeReactionLevel::Heavy, EHodgeHitReactionPhase::Controlled, EHodgeHitReactionFacingPreset::Heavy},
		{EHodgeImpactType::Knockback, EHodgeReactionLevel::None, EHodgeHitReactionPhase::Controlled, EHodgeHitReactionFacingPreset::Heavy},
		{EHodgeImpactType::Knockback, EHodgeReactionLevel::Light, EHodgeHitReactionPhase::Controlled, EHodgeHitReactionFacingPreset::Heavy},
		{EHodgeImpactType::Knockback, EHodgeReactionLevel::Heavy, EHodgeHitReactionPhase::Controlled, EHodgeHitReactionFacingPreset::Heavy},
		{EHodgeImpactType::Launch, EHodgeReactionLevel::None, EHodgeHitReactionPhase::Airborne, EHodgeHitReactionFacingPreset::Launch},
		{EHodgeImpactType::Launch, EHodgeReactionLevel::Heavy, EHodgeHitReactionPhase::Airborne, EHodgeHitReactionFacingPreset::Launch},
		{EHodgeImpactType::Slam, EHodgeReactionLevel::None, EHodgeHitReactionPhase::Airborne, EHodgeHitReactionFacingPreset::None},
		{EHodgeImpactType::AirHit, EHodgeReactionLevel::None, EHodgeHitReactionPhase::Airborne, EHodgeHitReactionFacingPreset::None},
		{EHodgeImpactType::Knockdown, EHodgeReactionLevel::None, EHodgeHitReactionPhase::Downed, EHodgeHitReactionFacingPreset::None},
		{EHodgeImpactType::HitStun, EHodgeReactionLevel::Break, EHodgeHitReactionPhase::Controlled, EHodgeHitReactionFacingPreset::None},
		{EHodgeImpactType::Knockback, static_cast<EHodgeReactionLevel>(255), EHodgeHitReactionPhase::Controlled, EHodgeHitReactionFacingPreset::None}
	};
	for (const auto Mode : {EHodgeReactionInputMode::Legacy, EHodgeReactionInputMode::PoiseDriven, EHodgeReactionInputMode::ExplicitImpact})
	{
		Context.Request.InputMode = Mode;
		for (const FCase& Case : Cases)
		{
			FHodgeHitReactionState Plan;
			Plan.Type = Case.Type;
			Plan.Phase = Case.Phase;
			Plan.ReactionLevel = Case.Level;
			FHodgeHitReactionFacingCandidate Candidate;
			TArray<FText> Errors;
			TestTrue(TEXT("各输入模式共用最终计划映射且无额外错误"), FHodgeHitReactionPlanBuilder::BuildFacingCandidate(Context, Plan, 0.f, Candidate, Errors));
			TestEqual(TEXT("预设匹配最终计划而非Slam请求"), Candidate.Preset, Case.Expected);
			TestEqual(TEXT("仅合法映射产生方向目标"), Candidate.bHasTarget, Case.Expected != EHodgeHitReactionFacingPreset::None);
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitReactionFacingCandidateEligibilityTest, "Hodge.HitReaction.FacingCandidate.ExclusionsAndAttempt",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 初次地面候选可建立，空中、倒地、起身与根运动不重开；无源仍保留本级尝试。 */
bool FHodgeHitReactionFacingCandidateEligibilityTest::RunTest(const FString& Parameters)
{
	auto* Profile = NewObject<UHodgeHitReactionProfile>();
	Profile->Facing.bEnabled = true;
	auto Context = HodgeFacingTests::MakeContext(Profile);
	FHodgeHitReactionState Plan;
	Plan.Phase = EHodgeHitReactionPhase::Controlled;
	FHodgeHitReactionFacingCandidate Candidate;
	TArray<FText> Errors;
	for (const auto Phase : {EHodgeHitReactionPhase::Airborne, EHodgeHitReactionPhase::Downed, EHodgeHitReactionPhase::GettingUp})
	{
		Context.State.Phase = Phase;
		Candidate.bHasTarget = true;
		TestTrue(TEXT("排除阶段不制造配置错误"), FHodgeHitReactionPlanBuilder::BuildFacingCandidate(Context, Plan, 0.f, Candidate, Errors));
		TestEqual(TEXT("排除阶段没有新预设"), Candidate.Preset, EHodgeHitReactionFacingPreset::None);
		TestFalse(TEXT("排除阶段清除上次输出的目标"), Candidate.bHasTarget);
	}
	Context.State = {};
	Context.bAir = true;
	Plan.Phase = EHodgeHitReactionPhase::Airborne;
	Plan.Type = EHodgeImpactType::Launch;
	TestTrue(TEXT("自然空中或排队击飞不新增配置错误"), FHodgeHitReactionPlanBuilder::BuildFacingCandidate(Context, Plan, 0.f, Candidate, Errors));
	TestEqual(TEXT("bAir阻止首次Launch预设重开"), Candidate.Preset, EHodgeHitReactionFacingPreset::None);
	Context.bAir = false;
	Plan.Phase = EHodgeHitReactionPhase::Controlled;
	Plan.Type = EHodgeImpactType::HitStun;
	Plan.MotionSource = EHodgeReactionMotionSource::AnimationRootMotion;
	TestTrue(TEXT("根运动受击维持旧规则"), FHodgeHitReactionPlanBuilder::BuildFacingCandidate(Context, Plan, 0.f, Candidate, Errors));
	TestFalse(TEXT("根运动计划没有补转目标"), Candidate.bHasTarget);
	Plan.MotionSource = EHodgeReactionMotionSource::Impact;
	Profile->Facing.bEnabled = false;
	TestTrue(TEXT("关闭补转保持无副作用成功"), FHodgeHitReactionPlanBuilder::BuildFacingCandidate(Context, Plan, 17.f, Candidate, Errors));
	TestEqual(TEXT("关闭仍保留最终计划所属预设"), Candidate.Preset, EHodgeHitReactionFacingPreset::Light);
	TestFalse(TEXT("关闭不产生目标"), Candidate.bHasTarget);
	Profile->Facing.bEnabled = true;
	Context.FacingSource.bValid = false;
	Context.bSourceValid = true;
	TestTrue(TEXT("新补转不借旧运动来源回退"), FHodgeHitReactionPlanBuilder::BuildFacingCandidate(Context, Plan, 17.f, Candidate, Errors));
	TestEqual(TEXT("无源仍记录Light本级尝试"), Candidate.Preset, EHodgeHitReactionFacingPreset::Light);
	TestFalse(TEXT("无源不会重建目标"), Candidate.bHasTarget);
	Context.FacingSource.bValid = true;
	Context.bSourceValid = false;
	TestTrue(TEXT("旧来源对象失效不改变冻结值"), FHodgeHitReactionPlanBuilder::BuildFacingCandidate(Context, Plan, 17.f, Candidate, Errors));
	TestTrue(TEXT("冻结值有效时仍有目标"), Candidate.bHasTarget);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitReactionFacingCandidateGeometryTest, "Hodge.HitReaction.FacingCandidate.FrozenXYAndAngles",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 检查水平值快照、实际起点限幅、角度跨界、背向固定侧及死区不补齐。 */
bool FHodgeHitReactionFacingCandidateGeometryTest::RunTest(const FString& Parameters)
{
	auto* Profile = NewObject<UHodgeHitReactionProfile>();
	Profile->Facing.bEnabled = true;
	Profile->Facing.Light.MaxCorrectionAngle = 45.f;
	auto Context = HodgeFacingTests::MakeContext(Profile);
	Context.Origin = FVector(-10000., 10000., 0.);
	Context.TargetLocation = FVector(10000., -10000., 0.);
	Context.TargetForward = -FVector::ForwardVector;
	FHodgeHitReactionState Plan;
	Plan.Phase = EHodgeHitReactionPhase::Controlled;
	Plan.MotionVelocity = FVector(17., -83., 24.);
	const auto Original = Plan;
	FHodgeHitReactionFacingCandidate Candidate;
	TArray<FText> Errors;
	TestTrue(TEXT("候选只读独立XY快照"), FHodgeHitReactionPlanBuilder::BuildFacingCandidate(Context, Plan, 15.f, Candidate, Errors));
	TestTrue(TEXT("来源Yaw为命中时的正Y而非当前目标位置"), FMath::IsNearlyEqual(Candidate.SourceYaw, 90.f));
	TestTrue(TEXT("实际15度起点最多补45度"), FMath::IsNearlyEqual(Candidate.TargetYaw, 60.f));
	Context.FacingSource.SourceLocation = FVector(-100., 0., std::numeric_limits<double>::quiet_NaN());
	Context.FacingSource.TargetLocation.Z = std::numeric_limits<double>::infinity();
	TestTrue(TEXT("高度与非有限Z不参与水平候选"), FHodgeHitReactionPlanBuilder::BuildFacingCandidate(Context, Plan, 0.f, Candidate, Errors));
	TestTrue(TEXT("背向180度固定正Yaw并限幅"), FMath::IsNearlyEqual(Candidate.TargetYaw, 45.f));
	Context.FacingSource.SourceLocation = FVector(0., -100., 0.);
	TestTrue(TEXT("负180度差同样稳定选正Yaw"), FHodgeHitReactionPlanBuilder::BuildFacingCandidate(Context, Plan, 90.f, Candidate, Errors));
	TestTrue(TEXT("90度起点的背向补转朝135度推进"), FMath::IsNearlyEqual(Candidate.TargetYaw, 135.f));
	Profile->Facing.DeadZoneAngle = 0.f;
	Context.FacingSource.SourceLocation = FRotator(0., -179., 0.).Vector() * 100.;
	Context.FacingSource.TargetLocation = FVector::ZeroVector;
	TestTrue(TEXT("正负180跨界取最短弧"), FHodgeHitReactionPlanBuilder::BuildFacingCandidate(Context, Plan, 179.f, Candidate, Errors));
	TestTrue(TEXT("跨界目标保持来源方向而非绕远"), FMath::IsNearlyEqual(Candidate.TargetYaw, -179.f, .001f));
	Profile->Facing.DeadZoneAngle = 3.f;
	Context.FacingSource.SourceLocation = FRotator(0., 3., 0.).Vector() * 100.;
	TestTrue(TEXT("死区边界无需正常转向"), FHodgeHitReactionPlanBuilder::BuildFacingCandidate(Context, Plan, 0.f, Candidate, Errors));
	TestFalse(TEXT("等于死区时不启动补转"), Candidate.bHasTarget);
	TestTrue(TEXT("死区保留实际起点而非补到来源"), FMath::IsNearlyZero(Candidate.TargetYaw));
	Context.FacingSource.SourceLocation = FVector(0., 0., 100.);
	TestTrue(TEXT("水平位置重合不借高度制造Yaw"), FHodgeHitReactionPlanBuilder::BuildFacingCandidate(Context, Plan, 24.f, Candidate, Errors));
	TestFalse(TEXT("水平退化方向无目标"), Candidate.bHasTarget);
	TestTrue(TEXT("退化方向保持实际24度"), FMath::IsNearlyEqual(Candidate.TargetYaw, 24.f));
	HodgeFacingTests::ExpectSamePlan(*this, Original, Plan);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitReactionFacingCandidateValidationTest, "Hodge.HitReaction.FacingCandidate.SelectedConfigIsolation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 实际只消费选中预设，无关坏配置不能取消合法候选，错误仍追加而不覆盖。 */
bool FHodgeHitReactionFacingCandidateValidationTest::RunTest(const FString& Parameters)
{
	auto* Profile = NewObject<UHodgeHitReactionProfile>();
	Profile->Facing.bEnabled = true;
	Profile->Facing.Heavy.WindowDuration = std::numeric_limits<float>::quiet_NaN();
	Profile->Facing.Launch.MaxCorrectionAngle = 0.f;
	auto Context = HodgeFacingTests::MakeContext(Profile);
	FHodgeHitReactionState Plan;
	Plan.Phase = EHodgeHitReactionPhase::Controlled;
	const FText Prior = FText::FromString(TEXT("调用方既有诊断"));
	TArray<FText> Errors = {Prior};
	FHodgeHitReactionFacingCandidate Candidate;
	TestTrue(TEXT("未选中的Heavy与Launch非法不影响Light"), FHodgeHitReactionPlanBuilder::BuildFacingCandidate(Context, Plan, 0.f, Candidate, Errors));
	TestTrue(TEXT("选中Light仍产生目标"), Candidate.bHasTarget);
	TestEqual(TEXT("有效可选候选不重复或清空旧错误"), Errors.Num(), 1);
	Plan.Type = EHodgeImpactType::Knockback;
	TestFalse(TEXT("选中的Heavy非法只使可选候选失败"), FHodgeHitReactionPlanBuilder::BuildFacingCandidate(Context, Plan, 0.f, Candidate, Errors));
	TestEqual(TEXT("非法配置也保留Heavy尝试身份"), Candidate.Preset, EHodgeHitReactionFacingPreset::Heavy);
	TestFalse(TEXT("非法配置不保留上次目标"), Candidate.bHasTarget);
	TestTrue(TEXT("非法配置追加具体预设诊断"), Errors.Last().ToString().Contains(TEXT("Facing.Heavy")));
	TestTrue(TEXT("既有错误内容保持"), Errors[0].EqualTo(Prior));
	Plan.Type = EHodgeImpactType::HitStun;
	Profile->Facing.DeadZoneAngle = 31.f;
	TestFalse(TEXT("死区超过选中Light角度无效"), FHodgeHitReactionPlanBuilder::BuildFacingCandidate(Context, Plan, 0.f, Candidate, Errors));
	Profile->Facing.DeadZoneAngle = 3.f;
	TestFalse(TEXT("已消费的实际起点Yaw必须有限"), FHodgeHitReactionPlanBuilder::BuildFacingCandidate(Context, Plan,
		std::numeric_limits<float>::infinity(), Candidate, Errors));
	Context.FacingSource.SourceLocation.X = std::numeric_limits<double>::quiet_NaN();
	TestFalse(TEXT("声明有效的快照仍拒绝非有限XY"), FHodgeHitReactionPlanBuilder::BuildFacingCandidate(Context, Plan, 0.f, Candidate, Errors));
	Profile->Facing.bEnabled = false;
	const int32 Before = Errors.Num();
	TestTrue(TEXT("关闭时不消费休眠XY与Yaw错误"), FHodgeHitReactionPlanBuilder::BuildFacingCandidate(Context, Plan,
		std::numeric_limits<float>::quiet_NaN(), Candidate, Errors));
	TestEqual(TEXT("关闭不追加未消费数值的错误"), Errors.Num(), Before);
	TestFalse(TEXT("关闭不会从旧输出恢复目标"), Candidate.bHasTarget);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitReactionFacingBuildIsolationTest, "Hodge.HitReaction.FacingCandidate.OriginalBuildContract",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 原构建不消费补转；可选失败不改变已成功计划，原部分失败输出继续保留。 */
bool FHodgeHitReactionFacingBuildIsolationTest::RunTest(const FString& Parameters)
{
	auto* Profile = NewObject<UHodgeHitReactionProfile>();
	FHodgeHitReactionAnimation Animation;
	Animation.bAllowWithoutMontage = true;
	Profile->Animations.Add(Animation);
	auto Context = HodgeFacingTests::MakeContext(Profile);
	Context.ServerTime = 10.f;
	Context.Request.Impacts.AddDefaulted();
	FHodgeHitReactionState Before;
	TArray<EHodgeImpactType> BeforeApplied = {EHodgeImpactType::Knockdown};
	TArray<FText> Errors;
	TestTrue(TEXT("关闭补转时原构建成功"), FHodgeHitReactionPlanBuilder::Build(Context, Before, BeforeApplied, Errors));
	Profile->Facing.bEnabled = true;
	Profile->Facing.Light.MaxCorrectionAngle = std::numeric_limits<float>::quiet_NaN();
	FHodgeHitReactionState After;
	TArray<EHodgeImpactType> AfterApplied = {EHodgeImpactType::Knockdown};
	TestTrue(TEXT("补转坏配置不介入原构建"), FHodgeHitReactionPlanBuilder::Build(Context, After, AfterApplied, Errors));
	HodgeFacingTests::ExpectSamePlan(*this, Before, After);
	TestTrue(TEXT("原Applied顺序和已有内容保持"), BeforeApplied == AfterApplied);
	FHodgeHitReactionFacingCandidate Candidate;
	TestFalse(TEXT("只有独立候选报告选中坏配置"), FHodgeHitReactionPlanBuilder::BuildFacingCandidate(Context, After, 0.f, Candidate, Errors));
	HodgeFacingTests::ExpectSamePlan(*this, Before, After);
	Profile->Animations[0].DefaultDuration = 0.f;
	FHodgeHitReactionState Partial;
	Partial.MotionVelocity = FVector(1., 2., 3.);
	TArray<EHodgeImpactType> PartialApplied = {EHodgeImpactType::Knockdown};
	TestFalse(TEXT("原控制时长错误仍拒绝构建"), FHodgeHitReactionPlanBuilder::Build(Context, Partial, PartialApplied, Errors));
	TestEqual(TEXT("失败仍保留已有Applied与刚解析类型"), PartialApplied.Num(), 2);
	TestEqual(TEXT("部分失败顺序仍保留原首项"), PartialApplied[0], EHodgeImpactType::Knockdown);
	TestEqual(TEXT("部分失败仍留下本次HitStun"), PartialApplied[1], EHodgeImpactType::HitStun);
	TestTrue(TEXT("失败不清空未写入的世界速度"), Partial.MotionVelocity.Equals(FVector(1., 2., 3.)));
	TestEqual(TEXT("原构建仍清空旧诊断只返回本次失败"), Errors.Num(), 1);
	return true;
}

#endif
