// 111 屎山代码来袭

#include "Animation/HodgeAnimInstance.h"
#include "Component/HodgeLocomotionPolicyComponent.h"
#include "Data/HodgeSprintAbilityProfile.h"
#include "SequenceEvaluatorLibrary.h"
#include "SequencePlayerLibrary.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimMontage.h"

#include "AbilitySystemGlobals.h"
#include "Character/HodgeCharacterBase.h"
#include "Character/HodgeCombatCharacter.h"
#include "Component/HodgeCharacterMovementComponent.h"
#include "Component/HodgeCharacterRotationComponent.h"
#include "Misc/DataValidation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAnimInstance)

// 构造动画实例
UHodgeAnimInstance::UHodgeAnimInstance(const FObjectInitializer& ObjectInitializer)
{
}

// 将动画实例与角色的 AbilitySystemComponent 绑定
void UHodgeAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	// ASC 必须有效，否则动画实例无法获取 GameplayTag 状态
	check(ASC);

	// 初始化 GameplayTag 到动画实例属性的自动映射
	GameplayTagPropertyMap.Initialize(this, ASC);
}

#if WITH_EDITOR
// 编辑器中检查 GameplayTagPropertyMap 等动画实例配置是否有效
EDataValidationResult UHodgeAnimInstance::IsDataValid(class FDataValidationContext& Context) const
{
	// 先执行父类的数据验证
	Super::IsDataValid(Context);

	// 验证 GameplayTagPropertyMap 的配置是否正确
	GameplayTagPropertyMap.IsDataValid(this, Context);

	// 只要验证过程中产生错误，就认为当前动画实例配置无效
	return ((Context.GetNumErrors() > 0) ? EDataValidationResult::Invalid : EDataValidationResult::Valid);
}
#endif // WITH_EDITOR

// 动画实例初始化时调用
void UHodgeAnimInstance::NativeInitializeAnimation()
{
	// 执行 UAnimInstance 默认初始化逻辑
	Super::NativeInitializeAnimation();

	// 获取当前动画实例所属的 Actor
	if (AActor* OwningActor = GetOwningActor())
	{
		// 从 Owner 上获取对应的 AbilitySystemComponent
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
		{
			// 将当前动画实例与 ASC 绑定，使 GameplayTag 状态能够自动同步到动画变量
			InitializeWithAbilitySystem(ASC);
		}
	}
}

// 每帧更新动画实例中的角色状态数据
void UHodgeAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	// 执行 UAnimInstance 默认的动画更新逻辑
	Super::NativeUpdateAnimation(DeltaSeconds);
	const AHodgeCombatCharacter* CombatCharacter = Cast<AHodgeCombatCharacter>(GetOwningActor());
	const UHodgeCharacterRotationComponent* Rotation = CombatCharacter
		                                                   ? CombatCharacter->GetCharacterRotationComponent()
		                                                   : nullptr;
	const float FullBodyWeight = FMath::Clamp(GetSlotMontageLocalWeight(FName(TEXT("FullBody"))), 0.f, 1.f);
	FacingPresentation = Rotation ? Rotation->GetFacingPresentationSnapshot() : FHodgeFacingPresentationSnapshot();
	const auto* Policy = CombatCharacter ? CombatCharacter->FindComponentByClass<UHodgeLocomotionPolicyComponent>() : nullptr;
	const auto Locomotion = Policy ? Policy->GetResolvedPolicy() : FHodgeLocomotionState();
	const auto* MovementProfile = Policy ? Policy->GetProfile() : nullptr;
	float PoseFullBodyWeight = 0.f; float MovementMontageWeight = 0.f;
	for (const FAnimMontageInstance* Instance : MontageInstances)
	{
		if (!Instance || !Instance->Montage || !Instance->Montage->SlotAnimTracks.ContainsByPredicate(
			[](const FSlotAnimationTrack& Track) { return Track.SlotName == TEXT("FullBody"); })) { continue; }
		const float Weight = Instance->GetWeight(); PoseFullBodyWeight += Weight;
		if (MovementProfile && (Instance->Montage == MovementProfile->ForwardMontage || Instance->Montage == MovementProfile->BackwardMontage ||
			Instance->Montage == MovementProfile->SprintPivotMontage))
		{ MovementMontageWeight += Weight; }
	}
	LocomotionFootIKAlpha = ResolveFootIKAlpha(PoseFullBodyWeight, MovementMontageWeight);
	bSprinting = Locomotion.bSprinting;
	bSprintPivotAllowed = bSprinting && Locomotion.bPivotAllowed;
	// 根运动 Pivot 由 Sprint GA 负责，避免状态机同时叠加另一套掉头姿势。
	bSprintPivotAllowed &= !MovementProfile || !MovementProfile->SprintPivotMontage;
	SprintCycle = MovementProfile ? MovementProfile->SprintCycle.Get() : nullptr;
	SprintTurn = MovementProfile ? MovementProfile->SprintTurn.Get() : nullptr;
	SprintTurnPlayRate = MovementProfile ? MovementProfile->SprintTurnPlayRate : 1.f;
	SprintTurnBlendOutTime = MovementProfile ? MovementProfile->SprintTurnBlendOutTime : .12f;
	SprintCyclePlayRate = MovementProfile ? MovementProfile->SprintCyclePlayRate : 1.f;
	if (MovementProfile && CombatCharacter)
	{ SprintCyclePlayRate = MatchCycleRate(CombatCharacter->GetVelocity().Size2D(), MovementProfile->SprintCycleReferenceSpeed,
		MovementProfile->SprintCyclePlayRate, MovementProfile->SprintCycleMinPlayRate, MovementProfile->SprintCycleMaxPlayRate); }
	bUseStrafeLocomotion = FacingPresentation.Style == EHodgeLocomotionStyle::ReservedStrafe;
	const float TargetWeight = bUseStrafeLocomotion ? 1.f : 0.f;
	StrafeLocomotionWeight = FacingModeBlendTime > 0.f ? FMath::FInterpConstantTo(StrafeLocomotionWeight,
		TargetWeight, FMath::Max(0.f, DeltaSeconds), 1.f / FacingModeBlendTime) : TargetWeight;
	bSuppressLocomotionYaw = FullBodyWeight > KINDA_SMALL_NUMBER
		|| !bUseStrafeLocomotion || FacingPresentation.bYawLocked || FacingPresentation.bRecoveringFacing;
	LocomotionRootYawScale = (1.f - FullBodyWeight) * StrafeLocomotionWeight;
	bAllowLocomotionPivot = !bUseStrafeLocomotion && !FacingPresentation.bYawLocked &&
		!FacingPresentation.bRecoveringFacing && FullBodyWeight <= KINDA_SMALL_NUMBER;
	bSprintPivotAllowed &= bAllowLocomotionPivot;
	if (!bSuppressLocomotionYaw) { bResetLocomotionYaw = false; }
	else { bResetLocomotionYaw = FullBodyWeight >= 1.f - KINDA_SMALL_NUMBER || LocomotionRootYawScale <= KINDA_SMALL_NUMBER; }

	// 获取当前动画实例所属的 HodgeCharacter
	const AHodgeCharacterBase* Character = Cast<AHodgeCharacterBase>(GetOwningActor());
	if (!Character)
	{
		// Owner 不是 HodgeCharacter 时无法获取角色移动数据，直接结束更新
		return;
	}

	// 获取角色使用的 HodgeCharacterMovementComponent
	UHodgeCharacterMovementComponent* CharMoveComp = CastChecked<UHodgeCharacterMovementComponent>(
		Character->GetCharacterMovement());

	// 获取角色当前脚下的地面信息
	const FHodgeCharacterGroundInfo& GroundInfo = CharMoveComp->GetGroundInfo();

	// 将移动组件计算出的离地距离同步到动画实例，供动画蓝图使用
	GroundDistance = GroundInfo.GroundDistance;
}

float UHodgeAnimInstance::ResolveFootIKAlpha(float FullBodyWeight, float DashWeight)
{
	if (!FMath::IsFinite(FullBodyWeight) || !FMath::IsFinite(DashWeight)) { return 0.f; }
	return 1.f - FMath::Clamp(FullBodyWeight - FMath::Clamp(DashWeight, 0.f, FMath::Max(0.f, FullBodyWeight)), 0.f, 1.f);
}

FName UHodgeAnimInstance::ResolveCycleSyncGroup(bool bSprint)
{ return bSprint ? FName(TEXT("HodgeSprint")) : FName(TEXT("Locomotion")); }

float UHodgeAnimInstance::AdvanceTurnTime(float Time, float DeltaTime, float Length, float Rate)
{
	if (!FMath::IsFinite(Time) || !FMath::IsFinite(DeltaTime) || !FMath::IsFinite(Length) ||
		!FMath::IsFinite(Rate) || Length <= 0.f || DeltaTime < 0.f || Rate <= 0.f) { return FMath::Max(0.f, FMath::IsFinite(Time) ? Time : 0.f); }
	return FMath::Clamp(Time + DeltaTime * Rate, 0.f, Length);
}
float UHodgeAnimInstance::MatchCycleRate(float Speed, float ReferenceSpeed, float BaseRate, float Minimum, float Maximum)
{
	if (!FMath::IsFinite(Speed) || !FMath::IsFinite(ReferenceSpeed) || !FMath::IsFinite(BaseRate) || !FMath::IsFinite(Minimum) ||
		!FMath::IsFinite(Maximum) || Speed < 0.f || ReferenceSpeed <= 0.f || BaseRate <= 0.f || Minimum <= 0.f || Maximum < Minimum) { return 1.f; }
	return FMath::Clamp(Speed / ReferenceSpeed * BaseRate, Minimum, Maximum);
}
bool UHodgeAnimInstance::UpdateSprintPivot(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!bSprinting || !SprintTurn) { return false; }
	EAnimNodeReferenceConversionResult Result;
	const auto Evaluator = USequenceEvaluatorLibrary::ConvertToSequenceEvaluator(Node, Result);
	if (Result != EAnimNodeReferenceConversionResult::Succeeded || !Context.GetContext()) { return true; }
	if (USequenceEvaluatorLibrary::GetSequence(Evaluator) != SprintTurn)
	{
		USequenceEvaluatorLibrary::SetSequence(Evaluator, SprintTurn);
		USequenceEvaluatorLibrary::SetExplicitTime(Evaluator, 0.f);
	}
	USequenceEvaluatorLibrary::SetExplicitTime(Evaluator, AdvanceTurnTime(USequenceEvaluatorLibrary::GetAccumulatedTime(Evaluator),
		Context.GetContext()->GetDeltaTime(), SprintTurn->GetPlayLength(), SprintTurnPlayRate));
	return true;
}

bool UHodgeAnimInstance::IsTurnComplete(float Elapsed, float Length, float Rate, float BlendOut)
{
	return FMath::IsFinite(Elapsed) && FMath::IsFinite(Length) && FMath::IsFinite(Rate) && FMath::IsFinite(BlendOut) &&
		Elapsed >= 0.f && Length > 0.f && Rate > 0.f && BlendOut >= 0.f && Elapsed >= FMath::Max(0.f, Length / Rate - BlendOut);
}
bool UHodgeAnimInstance::UpdateSprintCycle(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!bSprinting || !SprintCycle) { return false; }
	EAnimNodeReferenceConversionResult Result;
	const auto Player = USequencePlayerLibrary::ConvertToSequencePlayer(Node, Result);
	if (Result != EAnimNodeReferenceConversionResult::Succeeded) { return true; }
	if (USequencePlayerLibrary::GetSequencePure(Player) != SprintCycle)
	{ USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, Player, SprintCycle, .12f); }
	USequencePlayerLibrary::SetPlayRate(Player, SprintCyclePlayRate);
	return true;
}
bool UHodgeAnimInstance::ShouldExitSprintPivot()
{
	if (!bSprintPivotAllowed || !SprintTurn) { return true; }
	const int32 Machine = GetStateMachineIndex(TEXT("LocomotionSM"));
	return Machine != INDEX_NONE && IsTurnComplete(GetInstanceCurrentStateElapsedTime(Machine),
		SprintTurn->GetPlayLength(), SprintTurnPlayRate, SprintTurnBlendOutTime);
}
