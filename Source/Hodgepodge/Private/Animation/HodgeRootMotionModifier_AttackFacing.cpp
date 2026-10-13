/** @file HodgeRootMotionModifier_AttackFacing.cpp @brief 按实际倍率和本次执行配置应用有界旋转。 */
#include "Animation/HodgeRootMotionModifier_AttackFacing.h"
#include "Component/HodgeCharacterRotationComponent.h"
#include "Data/HodgeAbilityDefinition.h"
#include "GameFramework/Actor.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeRootMotionModifier_AttackFacing)

void UHodgeRootMotionModifier_AttackFacing::Update(const FMotionWarpingUpdateContext& Context)
{
	const auto* Actor=GetActorOwner(); const auto* Rotation=Actor ? Actor->FindComponentByClass<UHodgeCharacterRotationComponent>() : nullptr;
	const auto FacingState=Rotation ? Rotation->GetResolvedState() : FHodgeCharacterRotationState();
	const auto* Data=FacingState.WarpDefinition.Get();
	if (!Data || FacingState.bYawLocked || FacingState.ExecutionMode!=EHodgeFacingExecutionMode::MontageWarp || Animation!=Data->ExecutionConfig.Montage ||
		!FMath::IsFinite(Context.PlayRate) || Context.PlayRate<=0.f)
	{ SetState(ERootMotionModifierState::Disabled); return; }
	bWarpTranslation=false; bWarpRotation=true; RotationType=EMotionWarpRotationType::Default;
	RotationMethod=EMotionWarpRotationMethod::SlerpWithClampedRate; WarpMaxRotationRate=Data->FacingWarp.MaxRotationRate;
	// 引擎使用动画剩余时间计算旋转；转换倍率后与实际游戏步长保持一致。
	WarpRotationTimeMultiplier=Data->FacingWarp.RotationTimeMultiplier/Context.PlayRate;
	Super::Update(Context);
}
