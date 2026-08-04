/**
 * @file HodgeGameplayTags.cpp
 * @brief 集中定义所有 Hodge 项目使用的 Native GameplayTag
 *
 * 每个 UE_DEFINE_GAMEPLAY_TAG 将 C++ 变量与字符串形式的 Tag 绑定，
 * 引擎会在启动时自动将这些 Tag 注册到 GameplayTagManager 中。
 */

#include "AbilitySystem/HodgeGameplayTags.h"

namespace HodgeGameplayTags
{
	// =============================================================================
	// Ability Activation Failure Tags
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_IsDead, "Ability.ActivateFail.IsDead");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_Cooldown, "Ability.ActivateFail.Cooldown");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_Cost, "Ability.ActivateFail.Cost");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_TagsBlocked, "Ability.ActivateFail.TagsBlocked");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_TagsMissing, "Ability.ActivateFail.TagsMissing");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_Networking, "Ability.ActivateFail.Networking");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_ActivationGroup, "Ability.ActivateFail.ActivationGroup");

	// =============================================================================
	// Ability Behavior Tags
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(Ability_Behavior_SurvivesDeath, "Ability.Behavior.SurvivesDeath");

	// =============================================================================
	// Ability Duration/Message Tags
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(Ability_Dash_Duration_Message, "Ability.Dash.Duration.Message");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Grenade_Duration_Message, "Ability.Grenade.Duration.Message");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Interaction_Activate, "Ability.Interaction.Activate");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Respawn_Completed_Message, "Ability.Respawn.Completed.Message");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Respawn_Duration_Message, "Ability.Respawn.Duration.Message");

	// =============================================================================
	// Ability Type Tags
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(Ability_Type, "Ability.Type");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Type_Action, "Ability.Type.Action");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Type_Action_ADS, "Ability.Type.Action.ADS");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Type_Action_Dash, "Ability.Type.Action.Dash");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Type_Action_Drop, "Ability.Type.Action.Drop");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Type_Action_Emote, "Ability.Type.Action.Emote");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Type_Action_Grenade, "Ability.Type.Action.Grenade");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Type_Action_Jump, "Ability.Type.Action.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Type_Action_Melee, "Ability.Type.Action.Melee");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Type_Action_Reload, "Ability.Type.Action.Reload");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Type_Action_WeaponFire, "Ability.Type.Action.WeaponFire");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Type_Info, "Ability.Type.Info");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Type_Info_ShowLeaderboard, "Ability.Type.Info.ShowLeaderboard");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Type_Passive, "Ability.Type.Passive");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Type_Passive_AutoReload, "Ability.Type.Passive.AutoReload");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Type_Passive_AutoRespawn, "Ability.Type.Passive.AutoRespawn");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Type_Passive_ChangeQuickbarSlot, "Ability.Type.Passive.ChangeQuickbarSlot");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Type_StatusChange, "Ability.Type.StatusChange");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Type_StatusChange_Death, "Ability.Type.StatusChange.Death");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Type_StatusChange_Spawning, "Ability.Type.StatusChange.Spawning");

	// =============================================================================
	// Cosmetic Tags
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(Cosmetic, "Cosmetic");
	UE_DEFINE_GAMEPLAY_TAG(Cosmetic_AnimationStyle_Feminine, "Cosmetic.AnimationStyle.Feminine");
	UE_DEFINE_GAMEPLAY_TAG(Cosmetic_AnimationStyle_Masculine, "Cosmetic.AnimationStyle.Masculine");
	UE_DEFINE_GAMEPLAY_TAG(Cosmetic_BodyStyle_Medium, "Cosmetic.BodyStyle.Medium");

	// =============================================================================
	// Gameplay Zone Tags
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(Gameplay_Zone_WeakSpot, "Gameplay.Zone.WeakSpot");

	// =============================================================================
	// GameplayCue Tags — Character
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Character_DamageTaken, "GameplayCue.Character.DamageTaken");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Character_Dash, "GameplayCue.Character.Dash");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Character_Dash_Cooldown, "GameplayCue.Character.Dash.Cooldown");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Character_Death, "GameplayCue.Character.Death");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Character_Heal, "GameplayCue.Character.Heal");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Character_Melee_Cooldown, "GameplayCue.Character.Melee.Cooldown");

	// =============================================================================
	// GameplayCue Tags — Test
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Test_Burst, "GameplayCue.Test.Burst");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Test_BurstLatent, "GameplayCue.Test.BurstLatent");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Test_Looping, "GameplayCue.Test.Looping");

	// =============================================================================
	// GameplayCue Tags — Weapon
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_Grenade_Cooldown, "GameplayCue.Weapon.Grenade.Cooldown");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_Grenade_Detonate, "GameplayCue.Weapon.Grenade.Detonate");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_Melee_Hit, "GameplayCue.Weapon.Melee.Hit");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_Melee_Impact, "GameplayCue.Weapon.Melee.Impact");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_Pistol_Fire, "GameplayCue.Weapon.Pistol.Fire");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_Rifle_Fire, "GameplayCue.Weapon.Rifle.Fire");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_Rifle_Impact, "GameplayCue.Weapon.Rifle.Impact");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_Shotgun_Fire, "GameplayCue.Weapon.Shotgun.Fire");

	// =============================================================================
	// GameplayCue Tags — World
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_World_Launcher_Activate, "GameplayCue.World.Launcher.Activate");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_World_Teleporter_Activate, "GameplayCue.World.Teleporter.Activate");

	// =============================================================================
	// GameplayEffect Tags
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_DamageTrait_Instant, "GameplayEffect.DamageTrait.Instant");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_DamageTrait_Periodic, "GameplayEffect.DamageTrait.Periodic");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_DamageType_Basic, "GameplayEffect.DamageType.Basic");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_DamageType_Grenade, "GameplayEffect.DamageType.Grenade");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_DamageType_Melee, "GameplayEffect.DamageType.Melee");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_DamageType_Pistol, "GameplayEffect.DamageType.Pistol");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_DamageType_Rifle, "GameplayEffect.DamageType.Rifle");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_DamageType_Shotgun, "GameplayEffect.DamageType.Shotgun");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_Heal_Instant, "GameplayEffect.Heal.Instant");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_Heal_Periodic, "GameplayEffect.Heal.Periodic");

	// =============================================================================
	// GameplayEvent Tags
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_MeleeHit, "GameplayEvent.MeleeHit");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Death, "GameplayEvent.Death");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Reset, "GameplayEvent.Reset");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_RequestReset, "GameplayEvent.RequestReset");

	// =============================================================================
	// GameSettings Tags
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(GameSettings_Action_EditSafeZone, "GameSettings.Action.EditSafeZone");

	// =============================================================================
	// HUD Slot Tags
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(HUD_Slot_ExtraEquipment, "HUD.Slot.ExtraEquipment");
	UE_DEFINE_GAMEPLAY_TAG(HUD_Slot_InfrequentAbilities, "HUD.Slot.InfrequentAbilities");
	UE_DEFINE_GAMEPLAY_TAG(HUD_Slot_LeftSideTouchInputs, "HUD.Slot.LeftSideTouchInputs");
	UE_DEFINE_GAMEPLAY_TAG(HUD_Slot_LeftSideTouchRegion, "HUD.Slot.LeftSideTouchRegion");
	UE_DEFINE_GAMEPLAY_TAG(HUD_Slot_RespawnTimer, "HUD.Slot.RespawnTimer");
	UE_DEFINE_GAMEPLAY_TAG(HUD_Slot_RightSideTouchInputs, "HUD.Slot.RightSideTouchInputs");
	UE_DEFINE_GAMEPLAY_TAG(HUD_Slot_RightSideTouchRegion, "HUD.Slot.RightSideTouchRegion");

	// =============================================================================
	// InputTag — Ability
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ability_Dash, "InputTag.Ability.Dash");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ability_Heal, "InputTag.Ability.Heal");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ability_Melee, "InputTag.Ability.Melee");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ability_Quickslot_Drop, "InputTag.Ability.Quickslot.Drop");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Jump, "InputTag.Jump");

	// =============================================================================
	// InputTag — Weapon
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Weapon_ADS, "InputTag.Weapon.ADS");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Weapon_Fire, "InputTag.Weapon.Fire");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Weapon_FireAuto, "InputTag.Weapon.FireAuto");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Weapon_Grenade, "InputTag.Weapon.Grenade");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Weapon_Reload, "InputTag.Weapon.Reload");

	// =============================================================================
	// InputTag — Movement / Look / Basic
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look_Mouse, "InputTag.Look.Mouse");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look_Stick, "InputTag.Look.Stick");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Crouch, "InputTag.Crouch");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_AutoRun, "InputTag.AutoRun");

	// =============================================================================
	// InputTag — ALS (Advanced Locomotion System)
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Sprint, "InputTag.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Walk, "InputTag.Walk");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Aim, "InputTag.Aim");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ragdoll, "InputTag.Ragdoll");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Roll, "InputTag.Roll");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_RotationMode, "InputTag.RotationMode");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_ViewMode, "InputTag.ViewMode");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_SwitchShoulder, "InputTag.SwitchShoulder");

	// =============================================================================
	// InitState Tags
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(InitState_Spawned, "InitState.Spawned");
	UE_DEFINE_GAMEPLAY_TAG(InitState_DataAvailable, "InitState.DataAvailable");
	UE_DEFINE_GAMEPLAY_TAG(InitState_DataInitialized, "InitState.DataInitialized");
	UE_DEFINE_GAMEPLAY_TAG(InitState_GameplayReady, "InitState.GameplayReady");

	// =============================================================================
	// Lyra Tags
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(Lyra_Damage_Taken_Message, "Lyra.Damage.Taken.Message");
	UE_DEFINE_GAMEPLAY_TAG(Lyra_HUD_PlayerHUD, "Lyra.HUD.PlayerHUD");
	UE_DEFINE_GAMEPLAY_TAG(Lyra_HUD_TempTopWidgets, "Lyra.HUD.TempTopWidgets");
	UE_DEFINE_GAMEPLAY_TAG(Lyra_Player, "Lyra.Player");

	// =============================================================================
	// Platform Trait Tags
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(Platform_Trait_BinauralSettingControlledByOS,
	                       "Platform.Trait.BinauralSettingControlledByOS");
	UE_DEFINE_GAMEPLAY_TAG(Platform_Trait_CanExitApplication, "Platform.Trait.CanExitApplication");
	UE_DEFINE_GAMEPLAY_TAG(Platform_Trait_Input_PrimarlyController, "Platform.Trait.Input.PrimarlyController");
	UE_DEFINE_GAMEPLAY_TAG(Platform_Trait_Input_HasStrictControllerPairing,
	                       "Platform.Trait.Input.HasStrictControllerPairing");
	UE_DEFINE_GAMEPLAY_TAG(Platform_Trait_Input_PrimarlyTouchScreen, "Platform.Trait.Input.PrimarlyTouchScreen");
	UE_DEFINE_GAMEPLAY_TAG(Platform_Trait_Input_SupportsMouseAndKeyboard,
	                       "Platform.Trait.Input.SupportsMouseAndKeyboard");
	UE_DEFINE_GAMEPLAY_TAG(Platform_Trait_Input_HardwareCursor, "Platform.Trait.Input.HardwareCursor");
	UE_DEFINE_GAMEPLAY_TAG(Platform_Trait_SupportsBackgroundAudio, "Platform.Trait.SupportsBackgroundAudio");
	UE_DEFINE_GAMEPLAY_TAG(Platform_Trait_SupportsChangingAudioOutputDevice,
	                       "Platform.Trait.SupportsChangingAudioOutputDevice");
	UE_DEFINE_GAMEPLAY_TAG(Platform_Trait_SupportsWindowedMode, "Platform.Trait.SupportsWindowedMode");

	// =============================================================================
	// ShooterGame Tags
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(ShooterGame_GamePhase_MatchBeginCountdown, "ShooterGame.GamePhase.MatchBeginCountdown");

	// =============================================================================
	// Status Tags
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(Status_SpawningIn, "Status.SpawningIn");
	UE_DEFINE_GAMEPLAY_TAG(Status_Crouching, "Status.Crouching");
	UE_DEFINE_GAMEPLAY_TAG(Status_AutoRunning, "Status.AutoRunning");
	UE_DEFINE_GAMEPLAY_TAG(Status_Death, "Status.Death");
	UE_DEFINE_GAMEPLAY_TAG(Status_Death_Dying, "Status.Death.Dying");
	UE_DEFINE_GAMEPLAY_TAG(Status_Death_Dead, "Status.Death.Dead");

	// =============================================================================
	// SetByCaller Tags
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Damage, "SetByCaller.Damage");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Heal, "SetByCaller.Heal");

	// =============================================================================
	// Cheat Tags
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(Cheat_GodMode, "Cheat.GodMode");
	UE_DEFINE_GAMEPLAY_TAG(Cheat_UnlimitedHealth, "Cheat.UnlimitedHealth");

	// =============================================================================
	// UI Tags
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(UI_Action_Back, "UI.Action.Back");
	UE_DEFINE_GAMEPLAY_TAG(UI_Layer_Game, "UI.Layer.Game");
	UE_DEFINE_GAMEPLAY_TAG(UI_Layer_GameMenu, "UI.Layer.GameMenu");
	UE_DEFINE_GAMEPLAY_TAG(UI_Layer_Menu, "UI.Layer.Menu");
	UE_DEFINE_GAMEPLAY_TAG(UI_Layer_Modal, "UI.Layer.Modal");

	// =============================================================================
	// Movement Mode Tags
	// =============================================================================
	UE_DEFINE_GAMEPLAY_TAG(Movement_Mode_Walking, "Movement.Mode.Walking");
	UE_DEFINE_GAMEPLAY_TAG(Movement_Mode_NavWalking, "Movement.Mode.NavWalking");
	UE_DEFINE_GAMEPLAY_TAG(Movement_Mode_Falling, "Movement.Mode.Falling");
	UE_DEFINE_GAMEPLAY_TAG(Movement_Mode_Swimming, "Movement.Mode.Swimming");
	UE_DEFINE_GAMEPLAY_TAG(Movement_Mode_Flying, "Movement.Mode.Flying");
	UE_DEFINE_GAMEPLAY_TAG(Movement_Mode_Custom, "Movement.Mode.Custom");

	// =============================================================================
	// Movement Mode Tag Maps
	// =============================================================================
	const TMap<uint8, FGameplayTag> MovementModeTagMap =
	{
		{0, Movement_Mode_Walking},
		{1, Movement_Mode_NavWalking},
		{2, Movement_Mode_Falling},
		{3, Movement_Mode_Swimming},
		{4, Movement_Mode_Flying},
	};

	const TMap<uint8, FGameplayTag> CustomMovementModeTagMap =
	{
		{0, Movement_Mode_Custom},
	};

	// =============================================================================
	// FindTagByString - 按字符串查找 Tag（支持部分匹配）
	// =============================================================================
	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString)
	{
		const UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
		FGameplayTag Tag = Manager.RequestGameplayTag(FName(*TagString), false);
		if (!Tag.IsValid() && bMatchPartialString)
		{
			FGameplayTagContainer AllTags;
			Manager.RequestAllGameplayTags(AllTags, true);
			for (const FGameplayTag& T : AllTags)
			{
				if (T.GetTagName().ToString().Contains(TagString))
				{
					Tag = T;
					break;
				}
			}
		}
		return Tag;
	}
}
