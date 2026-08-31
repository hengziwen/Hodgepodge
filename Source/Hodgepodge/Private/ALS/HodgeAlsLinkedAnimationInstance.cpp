#include "ALS/HodgeAlsLinkedAnimationInstance.h"

#include "ALS/HodgeAlsAnimationInstance.h"
#include "ALS/HodgeAlsAnimationInstanceProxy.h"
#include "ALS/HodgeAlsCharacter.h"
#include "ALS/Utility/HodgeAlsMacros.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAlsLinkedAnimationInstance)

UHodgeAlsLinkedAnimationInstance::UHodgeAlsLinkedAnimationInstance()
{
	bUseMainInstanceMontageEvaluationData = true;
}

void UHodgeAlsLinkedAnimationInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Parent = Cast<UHodgeAlsAnimationInstance>(GetSkelMeshComponent()->GetAnimInstance());
	Character = Cast<AHodgeAlsCharacter>(GetOwningActor());

#if WITH_EDITOR
	const auto* World{GetWorld()};

	if (IsValid(World) && !World->IsGameWorld())
	{
		// Use default objects for editor preview.

		if (!Parent.IsValid())
		{
			Parent = GetMutableDefault<UHodgeAlsAnimationInstance>();
		}

		if (!IsValid(Character))
		{
			Character = GetMutableDefault<AHodgeAlsCharacter>();
		}
	}
#endif
}

void UHodgeAlsLinkedAnimationInstance::NativeBeginPlay()
{
	HODGEALS_ENSURE_MESSAGE(Parent.IsValid(),
	                   TEXT("%s (%s) should only be used as a linked animation instance within the %s animation blueprint!"),
	                   HODGEALS_GET_TYPE_STRING(UHodgeAlsLinkedAnimationInstance).GetData(), *GetClass()->GetName(),
	                   HODGEALS_GET_TYPE_STRING(UHodgeAlsAnimationInstance).GetData());

	Super::NativeBeginPlay();
}

FAnimInstanceProxy* UHodgeAlsLinkedAnimationInstance::CreateAnimInstanceProxy()
{
	return new FHodgeAlsAnimationInstanceProxy{this};
}

void UHodgeAlsLinkedAnimationInstance::InitializeLook()
{
	if (Parent.IsValid())
	{
		Parent->InitializeLook();
	}
}

void UHodgeAlsLinkedAnimationInstance::RefreshLook()
{
	if (Parent.IsValid())
	{
		Parent->RefreshLook();
	}
}

void UHodgeAlsLinkedAnimationInstance::InitializeLean()
{
	if (Parent.IsValid())
	{
		Parent->InitializeLean();
	}
}

void UHodgeAlsLinkedAnimationInstance::InitializeGrounded()
{
	if (Parent.IsValid())
	{
		Parent->InitializeGrounded();
	}
}

void UHodgeAlsLinkedAnimationInstance::RefreshGrounded()
{
	if (Parent.IsValid())
	{
		Parent->RefreshGrounded();
	}
}

void UHodgeAlsLinkedAnimationInstance::ResetGroundedEntryMode()
{
	if (Parent.IsValid())
	{
		Parent->ResetGroundedEntryMode();
	}
}

void UHodgeAlsLinkedAnimationInstance::RefreshGroundedMovement()
{
	if (Parent.IsValid())
	{
		Parent->RefreshGroundedMovement();
	}
}

void UHodgeAlsLinkedAnimationInstance::SetHipsDirection(const EHodgeAlsHipsDirection NewHipsDirection)
{
	if (Parent.IsValid())
	{
		Parent->SetHipsDirection(NewHipsDirection);
	}
}

void UHodgeAlsLinkedAnimationInstance::InitializeStandingMovement()
{
	if (Parent.IsValid())
	{
		Parent->InitializeStandingMovement();
	}
}

void UHodgeAlsLinkedAnimationInstance::RefreshStandingMovement()
{
	if (Parent.IsValid())
	{
		Parent->RefreshStandingMovement();
	}
}

void UHodgeAlsLinkedAnimationInstance::ResetPivot()
{
	if (Parent.IsValid())
	{
		Parent->ResetPivot();
	}
}

void UHodgeAlsLinkedAnimationInstance::RefreshCrouchingMovement()
{
	if (Parent.IsValid())
	{
		Parent->RefreshCrouchingMovement();
	}
}

void UHodgeAlsLinkedAnimationInstance::RefreshInAir()
{
	if (Parent.IsValid())
	{
		Parent->RefreshInAir();
	}
}

void UHodgeAlsLinkedAnimationInstance::RefreshDynamicTransitions()
{
	if (Parent.IsValid())
	{
		Parent->RefreshDynamicTransitions();
	}
}

void UHodgeAlsLinkedAnimationInstance::RefreshRotateInPlace()
{
	if (Parent.IsValid())
	{
		Parent->RefreshRotateInPlace();
	}
}

void UHodgeAlsLinkedAnimationInstance::InitializeTurnInPlace()
{
	if (Parent.IsValid())
	{
		Parent->InitializeTurnInPlace();
	}
}

void UHodgeAlsLinkedAnimationInstance::RefreshTurnInPlace()
{
	if (Parent.IsValid())
	{
		Parent->RefreshTurnInPlace();
	}
}
