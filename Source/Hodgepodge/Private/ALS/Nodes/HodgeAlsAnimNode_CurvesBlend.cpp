#include "ALS/Nodes/HodgeAlsAnimNode_CurvesBlend.h"

#include "Animation/AnimTrace.h"
#include "ALS/Utility/HodgeAlsEnumUtility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAlsAnimNode_CurvesBlend)

void FHodgeAlsAnimNode_CurvesBlend::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_FUNC()

	Super::Initialize_AnyThread(Context);

	SourcePose.Initialize(Context);
	CurvesPose.Initialize(Context);
}

void FHodgeAlsAnimNode_CurvesBlend::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_FUNC()

	Super::CacheBones_AnyThread(Context);

	SourcePose.CacheBones(Context);
	CurvesPose.CacheBones(Context);
}

void FHodgeAlsAnimNode_CurvesBlend::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_FUNC()

	Super::Update_AnyThread(Context);

	GetEvaluateGraphExposedInputs().Execute(Context);

	SourcePose.Update(Context);

	const auto CurrentBlendAmount{GetBlendAmount()};
	if (FAnimWeight::IsRelevant(CurrentBlendAmount))
	{
		CurvesPose.Update(Context);
	}

	TRACE_ANIM_NODE_VALUE(Context, TEXT("Blend Amount"), CurrentBlendAmount);
	TRACE_ANIM_NODE_VALUE(Context, TEXT("Blend Mode"), *HodgeAlsEnumUtility::GetNameStringByValue(GetBlendMode()));
}

void FHodgeAlsAnimNode_CurvesBlend::Evaluate_AnyThread(FPoseContext& Output)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_FUNC()
	ANIM_MT_SCOPE_CYCLE_COUNTER_VERBOSE(CurvesBlend, !IsInGameThread());

	Super::Evaluate_AnyThread(Output);

	SourcePose.Evaluate(Output);

	const auto CurrentBlendAmount{GetBlendAmount()};
	if (!FAnimWeight::IsRelevant(CurrentBlendAmount))
	{
		return;
	}

	auto CurvesPoseContext{Output};
	CurvesPose.Evaluate(CurvesPoseContext);

	switch (GetBlendMode())
	{
		case EHodgeAlsCurvesBlendMode::BlendByAmount:
			Output.Curve.Accumulate(CurvesPoseContext.Curve, CurrentBlendAmount);
			break;

		case EHodgeAlsCurvesBlendMode::Combine:
			Output.Curve.Combine(CurvesPoseContext.Curve);
			break;

		case EHodgeAlsCurvesBlendMode::CombinePreserved:
			Output.Curve.CombinePreserved(CurvesPoseContext.Curve);
			break;

		case EHodgeAlsCurvesBlendMode::UseMaxValue:
			Output.Curve.UseMaxValue(CurvesPoseContext.Curve);
			break;

		case EHodgeAlsCurvesBlendMode::UseMinValue:
			Output.Curve.UseMinValue(CurvesPoseContext.Curve);
			break;

		case EHodgeAlsCurvesBlendMode::Override:
			Output.Curve.Override(CurvesPoseContext.Curve);
			break;
	}
}

void FHodgeAlsAnimNode_CurvesBlend::GatherDebugData(FNodeDebugData& DebugData)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(GatherDebugData)

	TStringBuilder<256> DebugItemBuilder{InPlace, DebugData.GetNodeName(this), TEXTVIEW(": Blend Amount: ")};

	DebugItemBuilder.Appendf(TEXT("%.2f"), GetBlendAmount());

	DebugData.AddDebugItem(FString{DebugItemBuilder});
	SourcePose.GatherDebugData(DebugData.BranchFlow(1.0f));
	CurvesPose.GatherDebugData(DebugData.BranchFlow(GetBlendAmount()));
}

float FHodgeAlsAnimNode_CurvesBlend::GetBlendAmount() const
{
	return GET_ANIM_NODE_DATA(float, BlendAmount);
}

EHodgeAlsCurvesBlendMode FHodgeAlsAnimNode_CurvesBlend::GetBlendMode() const
{
	return GET_ANIM_NODE_DATA(EHodgeAlsCurvesBlendMode, BlendMode);
}
