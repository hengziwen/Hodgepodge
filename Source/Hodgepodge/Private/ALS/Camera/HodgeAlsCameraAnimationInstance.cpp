#include "ALS/Camera/HodgeAlsCameraAnimationInstance.h"

#include "ALS/Camera/HodgeAlsCameraComponent.h"
#include "ALS/HodgeAlsCharacter.h"
#include "Engine/World.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAlsCameraAnimationInstance)

void UHodgeAlsCameraAnimationInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<AHodgeAlsCharacter>(GetOwningActor());
	Camera = Cast<UHodgeAlsCameraComponent>(GetSkelMeshComponent());

#if WITH_EDITOR
	const auto* World{GetWorld()};

	if (IsValid(World) && !World->IsGameWorld())
	{
		// Use default objects for editor preview.

		if (!IsValid(Character))
		{
			Character = GetMutableDefault<AHodgeAlsCharacter>();
		}

		if (!IsValid(Camera))
		{
			Camera = GetMutableDefault<UHodgeAlsCameraComponent>();
		}
	}
#endif
}

void UHodgeAlsCameraAnimationInstance::NativeUpdateAnimation(const float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (!IsValid(Character) || !IsValid(Camera))
	{
		return;
	}

	ViewMode = Character->GetViewMode();
	LocomotionMode = Character->GetLocomotionMode();

	if (ViewMode != HodgeAlsViewModeTags::FirstPerson)
	{
		RotationMode = Character->GetRotationMode();
	}
	else
	{
		// In first-person mode, the rotation mode is always view direction, which can lead to weird camera movement when switching between
		// the first-person view mode and the velocity direction rotation mode, so to fix this we use the desired rotation mode here.

		RotationMode = Character->GetDesiredRotationMode();
	}

	Stance = Character->GetStance();
	Gait = Character->GetGait();
	LocomotionAction = Character->GetLocomotionAction();

	bRightShoulder = Camera->IsRightShoulder();
}
