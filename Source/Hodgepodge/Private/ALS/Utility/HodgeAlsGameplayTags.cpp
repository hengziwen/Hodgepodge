#include "ALS/Utility/HodgeAlsGameplayTags.h"

namespace HodgeAlsViewModeTags
{
	UE_DEFINE_GAMEPLAY_TAG(FirstPerson, FName{TEXTVIEW("HodgeAls.ViewMode.FirstPerson")})
	UE_DEFINE_GAMEPLAY_TAG(ThirdPerson, FName{TEXTVIEW("HodgeAls.ViewMode.ThirdPerson")})
}

namespace HodgeAlsLocomotionModeTags
{
	UE_DEFINE_GAMEPLAY_TAG(Grounded, FName{TEXTVIEW("HodgeAls.LocomotionMode.Grounded")})
	UE_DEFINE_GAMEPLAY_TAG(InAir, FName{TEXTVIEW("HodgeAls.LocomotionMode.InAir")})
}

namespace HodgeAlsRotationModeTags
{
	UE_DEFINE_GAMEPLAY_TAG(VelocityDirection, FName{TEXTVIEW("HodgeAls.RotationMode.VelocityDirection")})
	UE_DEFINE_GAMEPLAY_TAG(ViewDirection, FName{TEXTVIEW("HodgeAls.RotationMode.ViewDirection")})
	UE_DEFINE_GAMEPLAY_TAG(Aiming, FName{TEXTVIEW("HodgeAls.RotationMode.Aiming")})
}

namespace HodgeAlsStanceTags
{
	UE_DEFINE_GAMEPLAY_TAG(Standing, FName{TEXTVIEW("HodgeAls.Stance.Standing")})
	UE_DEFINE_GAMEPLAY_TAG(Crouching, FName{TEXTVIEW("HodgeAls.Stance.Crouching")})
}

namespace HodgeAlsGaitTags
{
	UE_DEFINE_GAMEPLAY_TAG(Walking, FName{TEXTVIEW("HodgeAls.Gait.Walking")})
	UE_DEFINE_GAMEPLAY_TAG(Running, FName{TEXTVIEW("HodgeAls.Gait.Running")})
	UE_DEFINE_GAMEPLAY_TAG(Sprinting, FName{TEXTVIEW("HodgeAls.Gait.Sprinting")})
}

namespace HodgeAlsOverlayModeTags
{
	UE_DEFINE_GAMEPLAY_TAG(Default, FName{TEXTVIEW("HodgeAls.OverlayMode.Default")})
	UE_DEFINE_GAMEPLAY_TAG(Masculine, FName{TEXTVIEW("HodgeAls.OverlayMode.Masculine")})
	UE_DEFINE_GAMEPLAY_TAG(Feminine, FName{TEXTVIEW("HodgeAls.OverlayMode.Feminine")})
	UE_DEFINE_GAMEPLAY_TAG(Injured, FName{TEXTVIEW("HodgeAls.OverlayMode.Injured")})
	UE_DEFINE_GAMEPLAY_TAG(HandsTied, FName{TEXTVIEW("HodgeAls.OverlayMode.HandsTied")})
	UE_DEFINE_GAMEPLAY_TAG(Rifle, FName{TEXTVIEW("HodgeAls.OverlayMode.Rifle")})
	UE_DEFINE_GAMEPLAY_TAG(PistolOneHanded, FName{TEXTVIEW("HodgeAls.OverlayMode.PistolOneHanded")})
	UE_DEFINE_GAMEPLAY_TAG(PistolTwoHanded, FName{TEXTVIEW("HodgeAls.OverlayMode.PistolTwoHanded")})
	UE_DEFINE_GAMEPLAY_TAG(Bow, FName{TEXTVIEW("HodgeAls.OverlayMode.Bow")})
	UE_DEFINE_GAMEPLAY_TAG(Torch, FName{TEXTVIEW("HodgeAls.OverlayMode.Torch")})
	UE_DEFINE_GAMEPLAY_TAG(Binoculars, FName{TEXTVIEW("HodgeAls.OverlayMode.Binoculars")})
	UE_DEFINE_GAMEPLAY_TAG(Box, FName{TEXTVIEW("HodgeAls.OverlayMode.Box")})
	UE_DEFINE_GAMEPLAY_TAG(Barrel, FName{TEXTVIEW("HodgeAls.OverlayMode.Barrel")})
}

namespace HodgeAlsLocomotionActionTags
{
	UE_DEFINE_GAMEPLAY_TAG(Mantling, FName{TEXTVIEW("HodgeAls.LocomotionAction.Mantling")})
	UE_DEFINE_GAMEPLAY_TAG(Ragdolling, FName{TEXTVIEW("HodgeAls.LocomotionAction.Ragdolling")})
	UE_DEFINE_GAMEPLAY_TAG(GettingUp, FName{TEXTVIEW("HodgeAls.LocomotionAction.GettingUp")})
	UE_DEFINE_GAMEPLAY_TAG(Rolling, FName{TEXTVIEW("HodgeAls.LocomotionAction.Rolling")})
}

namespace HodgeAlsGroundedEntryModeTags
{
	UE_DEFINE_GAMEPLAY_TAG(FromRoll, FName{TEXTVIEW("HodgeAls.GroundedEntryMode.FromRoll")})
}
