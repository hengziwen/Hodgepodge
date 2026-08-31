#include "ALS/Utility/HodgeAlsUtility.h"

#include "GameplayTagsManager.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "ALS/Utility/HodgeAlsMacros.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAlsUtility)

FString UHodgeAlsUtility::NameToDisplayString(const FName& Name, const bool bNameIsBool)
{
	return FName::NameToDisplayString(Name.ToString(), bNameIsBool);
}

float UHodgeAlsUtility::GetAnimationCurveValueFromCharacter(const ACharacter* Character, const FName& CurveName)
{
	const auto* Mesh{IsValid(Character) ? Character->GetMesh() : nullptr};
	const auto* AnimationInstance{IsValid(Mesh) ? Mesh->GetAnimInstance() : nullptr};

	return HODGEALS_ENSURE(IsValid(AnimationInstance)) ? AnimationInstance->GetCurveValue(CurveName) : 0.0f;
}

FGameplayTagContainer UHodgeAlsUtility::GetChildTags(const FGameplayTag& Tag)
{
	return UGameplayTagsManager::Get().RequestGameplayTagChildren(Tag);
}

FName UHodgeAlsUtility::GetSimpleTagName(const FGameplayTag& Tag)
{
	const auto TagNode{UGameplayTagsManager::Get().FindTagNode(Tag)};

	return TagNode.IsValid() ? TagNode->GetSimpleTagName() : NAME_None;
}

float UHodgeAlsUtility::GetFirstPlayerPingSeconds(const UObject* WorldContext)
{
	const auto* World{IsValid(WorldContext) ? WorldContext->GetWorld() : nullptr};
	const auto* Player{IsValid(World) ? World->GetFirstPlayerController() : nullptr};
	const auto* PlayerState{IsValid(Player) ? Player->PlayerState.Get() : nullptr};

	return IsValid(PlayerState) ? PlayerState->GetPingInMilliseconds() * 0.001f : 0.0f;
}

bool UHodgeAlsUtility::TryGetMovementBaseRotationSpeed(const FBasedMovementInfo& BasedMovement, FRotator& RotationSpeed)
{
	if (!MovementBaseUtility::IsDynamicBase(BasedMovement.MovementBase))
	{
		RotationSpeed = FRotator::ZeroRotator;
		return false;
	}

	const auto* Body{BasedMovement.MovementBase->GetBodyInstance(BasedMovement.BoneName)};
	if (Body == nullptr)
	{
		RotationSpeed = FRotator::ZeroRotator;
		return false;
	}

	const auto AngularVelocityVector{Body->GetUnrealWorldAngularVelocityInRadians()};
	if (AngularVelocityVector.IsNearlyZero())
	{
		RotationSpeed = FRotator::ZeroRotator;
		return false;
	}

	RotationSpeed.Roll = FMath::RadiansToDegrees(AngularVelocityVector.X);
	RotationSpeed.Pitch = FMath::RadiansToDegrees(AngularVelocityVector.Y);
	RotationSpeed.Yaw = FMath::RadiansToDegrees(AngularVelocityVector.Z);

	return true;
}
