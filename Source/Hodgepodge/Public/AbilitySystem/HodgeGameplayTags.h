#pragma once

#include "NativeGameplayTags.h"

namespace HodgeGameplayTags
{
	HODGEPODGE_API FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

	// =============================================================================
	// Ability Activation Failure Tags
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_IsDead);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cooldown);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cost);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsBlocked);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsMissing);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Networking);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_ActivationGroup);

	// =============================================================================
	// Ability Behavior Tags
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Behavior_SurvivesDeath);

	// =============================================================================
	// Ability Duration/Message Tags
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Dash_Duration_Message);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Grenade_Duration_Message);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Interaction_Activate);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Respawn_Completed_Message);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Respawn_Duration_Message);

	// =============================================================================
	// Ability Type Tags
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_ADS);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Dash);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Drop);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Emote);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Grenade);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Jump);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Melee);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Reload);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_WeaponFire);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Info);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Info_ShowLeaderboard);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Passive);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Passive_AutoReload);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Passive_AutoRespawn);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Passive_ChangeQuickbarSlot);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_StatusChange);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_StatusChange_Death);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_StatusChange_Spawning);

	// =============================================================================
	// Cosmetic Tags
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cosmetic);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cosmetic_AnimationStyle_Feminine);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cosmetic_AnimationStyle_Masculine);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cosmetic_BodyStyle_Medium);

	// =============================================================================
	// Gameplay Zone Tags
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_Zone_WeakSpot);

	// =============================================================================
	// GameplayCue Tags — Character
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_DamageTaken);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Dash);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Dash_Cooldown);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Death);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Heal);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Melee_Cooldown);

	// =============================================================================
	// GameplayCue Tags — Test
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Test_Burst);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Test_BurstLatent);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Test_Looping);

	// =============================================================================
	// GameplayCue Tags — Weapon
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Grenade_Cooldown);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Grenade_Detonate);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Melee_Hit);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Melee_Impact);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Pistol_Fire);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Rifle_Fire);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Rifle_Impact);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Shotgun_Fire);

	// =============================================================================
	// GameplayCue Tags — World
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_World_Launcher_Activate);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_World_Teleporter_Activate);

	// =============================================================================
	// GameplayEffect Tags
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageTrait_Instant);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageTrait_Periodic);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Basic);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Grenade);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Melee);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Pistol);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Rifle);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Shotgun);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Heal_Instant);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Heal_Periodic);

	// =============================================================================
	// GameplayEvent Tags
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_MeleeHit);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Death);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Reset);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_RequestReset);

	// =============================================================================
	// GameSettings Tags
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameSettings_Action_EditSafeZone);

	// =============================================================================
	// HUD Slot Tags
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_ExtraEquipment);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_InfrequentAbilities);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_LeftSideTouchInputs);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_LeftSideTouchRegion);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_RespawnTimer);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_RightSideTouchInputs);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_RightSideTouchRegion);

	// =============================================================================
	// InputTag — Ability
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Dash);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Heal);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Melee);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Quickslot_Drop);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Jump);

	// =============================================================================
	// InputTag — Weapon
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_ADS);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_Fire);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_FireAuto);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_Grenade);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_Reload);

	// =============================================================================
	// InputTag — Movement / Look / Basic
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Mouse);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Stick);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Crouch);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_AutoRun);

	// =============================================================================
	// InputTag — ALS (Advanced Locomotion System)
	//
	// 这些 Tag 用于驱动 ALS 的移动逻辑,需在 UHodgeInputConfig 的
	// NativeInputActions 中为它们配置对应的 UInputAction。
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Sprint);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Walk);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Aim);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ragdoll);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Roll);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_RotationMode);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_ViewMode);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_SwitchShoulder);

	// =============================================================================
	// InitState Tags
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvailable);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);

	// =============================================================================
	// Lyra Tags
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Lyra_Damage_Taken_Message);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Lyra_HUD_PlayerHUD);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Lyra_HUD_TempTopWidgets);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Lyra_Player);

	// =============================================================================
	// Platform Trait Tags
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_BinauralSettingControlledByOS);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_CanExitApplication);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_PrimarlyController);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_HasStrictControllerPairing);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_PrimarlyTouchScreen);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_SupportsMouseAndKeyboard);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_HardwareCursor);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_SupportsBackgroundAudio);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_SupportsChangingAudioOutputDevice);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_SupportsWindowedMode);

	// =============================================================================
	// ShooterGame Tags
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ShooterGame_GamePhase_MatchBeginCountdown);

	// =============================================================================
	// Status Tags
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_SpawningIn);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Crouching);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_AutoRunning);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dying);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dead);

	// =============================================================================
	// SetByCaller Tags
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Heal);

	// =============================================================================
	// Cheat Tags
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cheat_GodMode);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cheat_UnlimitedHealth);

	// =============================================================================
	// UI Tags
	// =============================================================================
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Action_Back);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_Game);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_GameMenu);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_Menu);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_Modal);

	// =============================================================================
	// Movement Mode Tags & Mappings
	// =============================================================================
	HODGEPODGE_API extern const TMap<uint8, FGameplayTag> MovementModeTagMap;
	HODGEPODGE_API extern const TMap<uint8, FGameplayTag> CustomMovementModeTagMap;

	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Walking);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_NavWalking);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Falling);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Swimming);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Flying);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Custom);
};
