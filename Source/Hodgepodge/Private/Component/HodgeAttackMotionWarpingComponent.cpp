/** @file HodgeAttackMotionWarpingComponent.cpp @brief 根运动求值前使用当前或历史攻击快照准备目标。 */
#include "Component/HodgeAttackMotionWarpingComponent.h"
#include "Component/HodgeCharacterRotationComponent.h"
#include "Component/HodgeCharacterMovementComponent.h"
#include "Animation/HodgeRootMotionModifier_AttackFacing.h"
#include "Data/HodgeAbilityDefinition.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "AnimNotifyState_MotionWarping.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "AbilitySystemGlobals.h"
#include "Components/SkeletalMeshComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAttackMotionWarpingComponent)

void UHodgeAttackMotionWarpingComponent::InitializeComponent()
{
	Super::InitializeComponent(); OnPreUpdate.AddUniqueDynamic(this,&ThisClass::PrepareAttackEvaluation);
}
void UHodgeAttackMotionWarpingComponent::ClearAttackWarp()
{
	// 原生组件在 OnPreUpdate 后才遍历修正器，此处只移除项目专用修正器。
	Modifiers.RemoveAll([](const TObjectPtr<URootMotionModifier>& M) { return M && M->IsA<UHodgeRootMotionModifier_AttackFacing>(); });
	if (!OwnedTargetName.IsNone()) { RemoveWarpTarget(OwnedTargetName); }
	OwnedTargetName=NAME_None; EvaluationRequest.Invalidate(); PreviousPosition=-1.f;
}
void UHodgeAttackMotionWarpingComponent::PrepareAttackEvaluation(UMotionWarpingComponent*)
{
	const auto* Character=Cast<ACharacter>(GetOwner());
	const auto* Rotation=Character ? Character->FindComponentByClass<UHodgeCharacterRotationComponent>() : nullptr;
	auto State=Rotation ? Rotation->GetResolvedState() : FHodgeCharacterRotationState();
	const auto* Move=Character ? Character->GetCharacterMovement() : nullptr;
	const UAnimMontage* Montage=nullptr; float Position=-1.f;
	if (Character && Character->bClientUpdating && Move && Move->GetCurrentReplayedSavedMove())
	{ const auto* Saved=Move->GetCurrentReplayedSavedMove(); Montage=Saved->RootMotionMontage.Get(); Position=Saved->RootMotionPreviousTrackPosition; }
	else if (Character && Character->GetMesh() && Character->GetMesh()->GetAnimInstance())
	{ if (const auto* Instance=Character->GetMesh()->GetAnimInstance()->GetRootMotionMontageInstance()) { Montage=Instance->Montage; Position=Instance->GetPreviousPosition(); } }
	// 标签通知可能因低帧率或位置校正被跳过，恢复边界仍以动画源位置判定。
	if (Character && !Character->bClientUpdating && Character->GetLocalRole()!=ROLE_SimulatedProxy)
	{
		auto* ASC=Cast<UHodgeAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Character));
		auto* Spec=ASC ? ASC->FindAbilitySpecFromHandle(State.ActionAbilityHandle) : nullptr;
		auto* Ability=Spec ? Cast<UHodgeGameplayAbility_Definition>(Spec->GetPrimaryInstance()) : nullptr;
		if (Ability) { Ability->UpdateAttackFacingLifecycle(); State=Rotation->GetResolvedState(); }
	}
	PrepareAttackTarget(State,Montage,Position);
}
void UHodgeAttackMotionWarpingComponent::PrepareAttackTarget(const FHodgeCharacterRotationState& State, const UAnimMontage* Montage, float Position)
{
	const auto* Character=Cast<ACharacter>(GetOwner()); const auto* Data=State.WarpDefinition.Get();
	if (!Character || !Montage || !FMath::IsFinite(Position) || Position<0.f || !Data || !Data->FacingWarp.bEnabled || State.bYawLocked || State.Driver!=EHodgeCharacterFacingDriver::ActionDirection ||
		State.ExecutionMode!=EHodgeFacingExecutionMode::MontageWarp || Montage!=Data->ExecutionConfig.Montage || !FMath::IsFinite(State.ActionYaw))
	{ ClearAttackWarp(); return; }
	// 回放到旧执行或向后跳转必须重建修正器，不能使用下一刀的缓存。
	if (EvaluationRequest!=State.ActionRequestId || (PreviousPosition>=0.f && Position+KINDA_SMALL_NUMBER<PreviousPosition) || OwnedTargetName!=Data->FacingWarp.WarpTargetName)
	{
		ClearAttackWarp();
		// 原生窗口发现先于OnPreUpdate；倒回清理后须在当步恢复刚被移除的合法历史窗口。
		for (const auto& Event : Montage->Notifies)
		{
			const auto* Notify=Cast<UAnimNotifyState_MotionWarping>(Event.NotifyStateClass);
			const auto* Template=Notify ? Cast<UHodgeRootMotionModifier_AttackFacing>(Notify->RootMotionModifier) : nullptr;
			if (!Template || Template->WarpTargetName!=Data->FacingWarp.WarpTargetName) { continue; }
			const float Start=FMath::Clamp(Event.GetTriggerTime(),0.f,Montage->GetPlayLength());
			const float End=FMath::Clamp(Event.GetEndTriggerTime(),0.f,Montage->GetPlayLength());
			if (Position>=Start && Position<End && !ContainsModifier(Montage,Start,End))
			{
				// 沿用通知模板与原生激活/更新/结束委托，不复制旋转算法或扩大窗口。
				Notify->OnBecomeRelevant(this,Montage,Start,End);
			}
		}
	}
	EvaluationRequest=State.ActionRequestId; OwnedTargetName=Data->FacingWarp.WarpTargetName; PreviousPosition=Position;
	AddOrUpdateWarpTargetFromLocationAndRotation(OwnedTargetName,Character->GetActorLocation(),FRotator(0.f,State.ActionYaw,0.f));
	// 记录写入目标时的输入，不能在移动结束后读取可能已切换下一刀的当前状态。
	if (!Character->bClientUpdating)
	{
		if (auto* Movement=Cast<UHodgeCharacterMovementComponent>(Character->GetCharacterMovement()))
		{ Movement->RecordAttackWarpEvaluation(State,Montage); }
	}
}
