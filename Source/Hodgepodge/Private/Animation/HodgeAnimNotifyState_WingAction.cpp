/** @file HodgeAnimNotifyState_WingAction.cpp
 * @brief 将无状态的源动画窗口通知交给各角色独立的翼组件。
 */
#include "Animation/HodgeAnimNotifyState_WingAction.h"
#include "Component/HodgeWingComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"
#include "Engine/SkeletalMesh.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAnimNotifyState_WingAction)

UHodgeAnimNotifyState_WingAction::UHodgeAnimNotifyState_WingAction()
{
	bIsNativeBranchingPoint = true;
}

bool UHodgeAnimNotifyState_WingAction::Validate(const USkeletalMesh* Mesh, TArray<FText>& Errors) const
{
	if (!WingMontage || !Mesh || WingMontage->GetSkeleton() != Mesh->GetSkeleton() || WingMontage->SlotAnimTracks.IsEmpty())
	{ Errors.Add(FText::FromString(TEXT("Wing window requires a montage with matching wing skeleton and a slot"))); }
	if (WingMontage && WingMontage->SlotAnimTracks.ContainsByPredicate([](const FSlotAnimationTrack& Track) { return Track.SlotName != TEXT("DefaultSlot"); }))
	{ Errors.Add(FText::FromString(TEXT("Wing montage must use DefaultSlot, matching the wing AnimBP"))); }
	if (!FMath::IsFinite(PlayRate) || PlayRate <= 0.f || !FMath::IsFinite(StartPosition) || StartPosition < 0.f ||
		(WingMontage && StartPosition >= WingMontage->GetPlayLength()) || !FMath::IsFinite(FadeOutSeconds) ||
		(FadeOutSeconds < 0.f && FadeOutSeconds != -1.f))
	{ Errors.Add(FText::FromString(TEXT("Wing window requires positive finite rate, valid start and fade >= 0 or -1"))); }
	return Errors.IsEmpty();
}

float UHodgeAnimNotifyState_WingAction::EvaluateWingTime(float SourceTime, float WindowStart) const
{
	if (!WingMontage || !FMath::IsFinite(SourceTime) || !FMath::IsFinite(WindowStart) ||
		!FMath::IsFinite(StartPosition) || !FMath::IsFinite(PlayRate) || PlayRate <= 0.f) { return 0.f; }
	const double Position = double(StartPosition) + FMath::Max(0., double(SourceTime) - WindowStart) * PlayRate;
	return float(FMath::Clamp(Position, 0., double(WingMontage->GetPlayLength())));
}

FString UHodgeAnimNotifyState_WingAction::GetNotifyName_Implementation() const
{
	return WingMontage ? FString::Printf(TEXT("Wing: %s"), *WingMontage->GetName()) : TEXT("Wing: Select Montage");
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
