/** @file HodgeAnimNotifyState_WingAction.cpp
 * @brief 将无状态的源动画窗口通知交给各角色独立的翼组件。
 */
#include "Animation/HodgeAnimNotifyState_WingAction.h"
#include "Component/HodgeWingComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAnimNotifyState_WingAction)

UHodgeAnimNotifyState_WingAction::UHodgeAnimNotifyState_WingAction()
{
	bIsNativeBranchingPoint = true;
}

void UHodgeAnimNotifyState_WingAction::RefreshOwner(USkeletalMeshComponent* Mesh) const
{
	const auto* Character = Mesh ? Cast<ACharacter>(Mesh->GetOwner()) : nullptr;
	if (!Character || Character->GetMesh() != Mesh) { return; }
	if (auto* Wing = Character->FindComponentByClass<UHodgeWingComponent>())
	{
		Wing->RequestPresentationRefresh();
	}
}

void UHodgeAnimNotifyState_WingAction::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float Duration, const FAnimNotifyEventReference& Reference)
{
	RefreshOwner(Mesh);
}

void UHodgeAnimNotifyState_WingAction::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference)
{
	RefreshOwner(Mesh);
}

void UHodgeAnimNotifyState_WingAction::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload)
{
	RefreshOwner(Payload.SkelMeshComponent);
}

void UHodgeAnimNotifyState_WingAction::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload)
{
	RefreshOwner(Payload.SkelMeshComponent);
}
