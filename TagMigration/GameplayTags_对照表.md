# Lyra GameplayTag 全量对照表

> 由 `LyraStarterGame` (UE 5.5) 自动汇总，合并日期 2026-09-25。
> 可直接导入的 ini 见同目录 `DefaultGameplayTags.ini`。

- 标签总数：**286**
- 来源：`DefaultGameplayTags.ini` 99 条、`ShooterCoreTags.ini` 60 条、`ShooterExplorerTags.ini` 43 条、`TopDownArenaTags.ini` 8 条、`PluginTags.ini` 2 条、DataTable 8 条、C++ 原生 77 条


## Ability （40 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `Ability.ActivateFail.ActivationGroup` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | Ability failed to activate because of its activation group. |
| `Ability.ActivateFail.CellAlreadyContainsBomb` | INI | TopDownArenaTags.ini |  |
| `Ability.ActivateFail.Cooldown` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | Ability failed to activate because it is on cool down. |
| `Ability.ActivateFail.Cost` | C++ Native | Source/LyraGame/AbilitySystem/Abilities/LyraAbilityCost_ItemTagStack.cpp<br>Source/LyraGame/LyraGameplayTags.cpp | Ability failed to activate because it did not pass the cost checks. |
| `Ability.ActivateFail.IsDead` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | Ability failed to activate because its owner is dead. |
| `Ability.ActivateFail.MagazineFull` | INI | ShooterCoreTags.ini |  |
| `Ability.ActivateFail.Networking` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | Ability failed to activate because it did not pass the network checks. |
| `Ability.ActivateFail.NoSpareAmmo` | INI | ShooterCoreTags.ini |  |
| `Ability.ActivateFail.TagsBlocked` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | Ability failed to activate because tags are blocking it. |
| `Ability.ActivateFail.TagsMissing` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | Ability failed to activate because tags are missing. |
| `Ability.Behavior.SurvivesDeath` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | An ability with this type tag should not be canceled due to death. |
| `Ability.Dash.Duration.Message` | INI | DefaultGameplayTags.ini | Dash Cooldown Duration |
| `Ability.Grenade.Duration.Message` | INI | DefaultGameplayTags.ini | Cooldown Grenade |
| `Ability.Interaction.Activate` | INI + C++ Native | DefaultGameplayTags.ini<br>Source/LyraGame/Interaction/Abilities/LyraGameplayAbility_Interact.cpp |  |
| `Ability.Interaction.Duration.Message` | C++ Native | Source/LyraGame/Interaction/Abilities/LyraGameplayAbility_Interact.cpp |  |
| `Ability.PlayMontageOnActivateFail.Message` | C++ Native | Source/LyraGame/AbilitySystem/Abilities/LyraGameplayAbility.cpp |  |
| `Ability.Respawn.Completed.Message` | INI | DefaultGameplayTags.ini | Message tag broadcast when a respawn process is completed. |
| `Ability.Respawn.Duration.Message` | INI | DefaultGameplayTags.ini |  |
| `Ability.Type` | INI | DefaultGameplayTags.ini | Applied to Gameplay Abilities to indicate a functional category |
| `Ability.Type.Action` | INI | DefaultGameplayTags.ini | Action type abilities cause the avatar to perform an action in the game world, possibly involving equipment such as a weapon |
| `Ability.Type.Action.ADS` | INI | DefaultGameplayTags.ini |  |
| `Ability.Type.Action.Dash` | INI | DefaultGameplayTags.ini |  |
| `Ability.Type.Action.Drop` | INI | DefaultGameplayTags.ini |  |
| `Ability.Type.Action.Emote` | INI | DefaultGameplayTags.ini |  |
| `Ability.Type.Action.Grenade` | INI | DefaultGameplayTags.ini |  |
| `Ability.Type.Action.Jump` | INI | DefaultGameplayTags.ini |  |
| `Ability.Type.Action.Melee` | INI | DefaultGameplayTags.ini |  |
| `Ability.Type.Action.Reload` | INI | DefaultGameplayTags.ini |  |
| `Ability.Type.Action.WeaponFire` | INI | DefaultGameplayTags.ini |  |
| `Ability.Type.Info` | INI | DefaultGameplayTags.ini | Info abilities are used to summon information for the players, and do not directly affect gameplay |
| `Ability.Type.Info.ShowLeaderboard` | INI | DefaultGameplayTags.ini |  |
| `Ability.Type.Passive` | INI | DefaultGameplayTags.ini | Passive abilities are always active on the owner |
| `Ability.Type.Passive.AutoReload` | INI | DefaultGameplayTags.ini | Polls for active weapon to be empty, triggers reload input if enough time has elapsed |
| `Ability.Type.Passive.AutoRespawn` | INI | DefaultGameplayTags.ini | Passive ability that listens for a player\'s pawn to die or otherwise be removed from play, and automatically respawns. |
| `Ability.Type.Passive.ChangeQuickbarSlot` | INI | DefaultGameplayTags.ini | Handles requests for any type of slot change, whether directly indexed or cycling forward/backward |
| `Ability.Type.StatusChange` | INI | DefaultGameplayTags.ini | Abilities of this type are used when a status if imposed on the avatar or owner, such as death or spawning |
| `Ability.Type.StatusChange.Death` | INI | DefaultGameplayTags.ini | Covers the transition period when health goes to 0 until the avatar is removed from the world |
| `Ability.Type.StatusChange.Spawning` | INI | DefaultGameplayTags.ini | Covers the time when a new hero is created, spawning into the world, and when it\'s actively controllable |
| `Ability.UserFacingSimpleActivateFail.Message` | C++ Native | Source/LyraGame/AbilitySystem/Abilities/LyraGameplayAbility.cpp |  |
| `Ability.Weapon.NoFiring` | C++ Native | Source/LyraGame/Weapons/LyraGameplayAbility_RangedWeapon.cpp |  |

## AnimEffect （4 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `AnimEffect.Footstep.Jog` | DataTable | Content/ContextEffects/DT_AnimEffectTags.uasset |  |
| `AnimEffect.Footstep.Jump` | DataTable | Content/ContextEffects/DT_AnimEffectTags.uasset |  |
| `AnimEffect.Footstep.Land` | DataTable | Content/ContextEffects/DT_AnimEffectTags.uasset |  |
| `AnimEffect.Footstep.Walk` | DataTable | Content/ContextEffects/DT_AnimEffectTags.uasset |  |

## Cheat （2 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `Cheat.GodMode` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | GodMode cheat is active on the owner. |
| `Cheat.UnlimitedHealth` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | UnlimitedHealth cheat is active on the owner. |

## Cosmetic （4 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `Cosmetic` | INI | DefaultGameplayTags.ini |  |
| `Cosmetic.AnimationStyle.Feminine` | INI | DefaultGameplayTags.ini |  |
| `Cosmetic.AnimationStyle.Masculine` | INI | DefaultGameplayTags.ini |  |
| `Cosmetic.BodyStyle.Medium` | INI | DefaultGameplayTags.ini |  |

## Event （5 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `Event.Movement.ADS` | INI | ShooterCoreTags.ini |  |
| `Event.Movement.Dash` | INI | ShooterCoreTags.ini |  |
| `Event.Movement.Melee` | INI | ShooterCoreTags.ini |  |
| `Event.Movement.Reload` | INI | ShooterCoreTags.ini |  |
| `Event.Movement.WeaponFire` | INI | ShooterCoreTags.ini |  |

## Explorer （43 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `Explorer` | INI | ShooterExplorerTags.ini |  |
| `Explorer.Mood.Angry` | INI | ShooterExplorerTags.ini |  |
| `Explorer.Mood.Awesome` | INI | ShooterExplorerTags.ini |  |
| `Explorer.Mood.Grumpy` | INI | ShooterExplorerTags.ini |  |
| `Explorer.Mood.Happy` | INI | ShooterExplorerTags.ini |  |
| `Explorer.Mood.None` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Activity` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Activity.Actions` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Activity.Actions.Eat` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Activity.Actions.Emote` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Activity.Actions.Sit` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Activity.Actions.Sleep` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Activity.Actions.Sleep.AtNight` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Activity.Actions.Sleep.DayTime` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Activity.General` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Activity.General.Default` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Event` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Event.Interaction` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Event.Interaction.Ended` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Event.Interaction.Required` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Event.Interaction.Required.ToDisable` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Event.Interaction.Required.ToEnable` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Event.Interaction.Scripted` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Event.Interaction.Scripted.ToDisable` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Event.Interaction.Scripted.ToEnable` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Event.Interaction.Started` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Interact` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Interact.NPC` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Interact.Player` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Slot` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Slot.Sync` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Slot.Sync.Action` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Slot.Sync.Dependent` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Slot.Sync.Primary` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Slot.Sync.Ready` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.Slot.Sync.Stop` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.State` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.State.Broken` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.State.Enabled` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.State.Interaction` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.State.Interaction.Required` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.State.Interaction.Required.ToDisable` | INI | ShooterExplorerTags.ini |  |
| `Explorer.SmartObject.State.Interaction.Required.ToEnable` | INI | ShooterExplorerTags.ini |  |

## Gameplay （11 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `Gameplay.Damage` | C++ Native | Source/LyraGame/AbilitySystem/Attributes/LyraHealthSet.cpp |  |
| `Gameplay.Damage.FellOutOfWorld` | C++ Native | Source/LyraGame/AbilitySystem/Attributes/LyraHealthSet.cpp |  |
| `Gameplay.Damage.SelfDestruct` | C++ Native | Source/LyraGame/AbilitySystem/Attributes/LyraHealthSet.cpp |  |
| `Gameplay.DamageImmunity` | C++ Native | Source/LyraGame/AbilitySystem/Attributes/LyraHealthSet.cpp |  |
| `Gameplay.Message.ADS` | INI | ShooterCoreTags.ini | Message to UI, Reticle |
| `Gameplay.Message.Nameplate.Add` | INI | ShooterCoreTags.ini | Register Nameplate Source |
| `Gameplay.Message.Nameplate.Discover` | INI | ShooterCoreTags.ini | Looking for nameplates |
| `Gameplay.Message.Nameplate.Remove` | INI | ShooterCoreTags.ini | Unregister Nameplate Source |
| `Gameplay.MovementStopped` | C++ Native | Source/LyraGame/Character/LyraCharacterMovementComponent.cpp |  |
| `Gameplay.Zone` | C++ Native | Source/LyraGame/Weapons/LyraWeaponStateComponent.cpp |  |
| `Gameplay.Zone.WeakSpot` | INI | DefaultGameplayTags.ini |  |

## GameplayCue （28 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `GameplayCue.Character.DamageTaken` | INI | DefaultGameplayTags.ini |  |
| `GameplayCue.Character.Dash` | INI | DefaultGameplayTags.ini |  |
| `GameplayCue.Character.Dash.Cooldown` | INI | DefaultGameplayTags.ini | Cooldown for character dash |
| `GameplayCue.Character.Death` | INI | DefaultGameplayTags.ini |  |
| `GameplayCue.Character.Heal` | INI | DefaultGameplayTags.ini |  |
| `GameplayCue.Character.Melee.Cooldown` | INI | DefaultGameplayTags.ini |  |
| `GameplayCue.Character.Spawn` | INI | ShooterCoreTags.ini | At spawning of the player in shooter game |
| `GameplayCue.ShooterGame.Interact.Collect` | INI | ShooterCoreTags.ini |  |
| `GameplayCue.ShooterGame.Interact.WeaponPickup` | INI | ShooterCoreTags.ini | GCN for weapon pick FX attached to pawn |
| `GameplayCue.ShooterGame.UserMessage.MatchDecided` | INI | ShooterCoreTags.ini |  |
| `GameplayCue.ShooterGame.UserMessage.WaitingForPlayers` | INI | ShooterCoreTags.ini |  |
| `GameplayCue.Test.Burst` | INI | DefaultGameplayTags.ini |  |
| `GameplayCue.Test.BurstLatent` | INI | DefaultGameplayTags.ini |  |
| `GameplayCue.Test.Looping` | INI | DefaultGameplayTags.ini |  |
| `GameplayCue.TopDownArenaGame.PickupAcquired` | INI | TopDownArenaTags.ini |  |
| `GameplayCue.TopDownArenaGame.UserMessage.GameOver` | INI | TopDownArenaTags.ini |  |
| `GameplayCue.TopDownArenaGame.UserMessage.GetReady` | INI | TopDownArenaTags.ini |  |
| `GameplayCue.TopDownArenaGame.UserMessage.WaitingForPlayers` | INI | TopDownArenaTags.ini |  |
| `GameplayCue.Weapon.Grenade.Cooldown` | INI | DefaultGameplayTags.ini |  |
| `GameplayCue.Weapon.Grenade.Detonate` | INI | DefaultGameplayTags.ini | Grenade Explosion |
| `GameplayCue.Weapon.Melee.Hit` | INI | DefaultGameplayTags.ini | hit melee CS, placed on default as the GCN is there -needs to move to Shooter |
| `GameplayCue.Weapon.Melee.Impact` | INI | DefaultGameplayTags.ini | Cue triggering on target when hit by a melee attack |
| `GameplayCue.Weapon.Pistol.Fire` | INI | DefaultGameplayTags.ini |  |
| `GameplayCue.Weapon.Rifle.Fire` | INI | DefaultGameplayTags.ini |  |
| `GameplayCue.Weapon.Rifle.Impact` | INI | DefaultGameplayTags.ini |  |
| `GameplayCue.Weapon.Shotgun.Fire` | INI | DefaultGameplayTags.ini |  |
| `GameplayCue.World.Launcher.Activate` | INI | DefaultGameplayTags.ini | Fires when Launcher (Up or Forward) activates |
| `GameplayCue.World.Teleporter.Activate` | INI | DefaultGameplayTags.ini | Triggered when teleporter is activated |

## GameplayEffect （10 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `GameplayEffect.DamageTrait.Instant` | INI | DefaultGameplayTags.ini |  |
| `GameplayEffect.DamageTrait.Periodic` | INI | DefaultGameplayTags.ini |  |
| `GameplayEffect.DamageType.Basic` | INI | DefaultGameplayTags.ini |  |
| `GameplayEffect.DamageType.Grenade` | INI | DefaultGameplayTags.ini | Damage effect coming from a grenade explosion |
| `GameplayEffect.DamageType.Melee` | INI | DefaultGameplayTags.ini | Damage effect coming from a melee strike |
| `GameplayEffect.DamageType.Pistol` | INI | DefaultGameplayTags.ini | Damage effect coming from a pistol bullet |
| `GameplayEffect.DamageType.Rifle` | INI | DefaultGameplayTags.ini | Damage effect coming from a rifle bullet |
| `GameplayEffect.DamageType.Shotgun` | INI | DefaultGameplayTags.ini | Damage effect coming from shotgun pellets |
| `GameplayEffect.Heal.Instant` | INI | DefaultGameplayTags.ini |  |
| `GameplayEffect.Heal.Periodic` | INI | DefaultGameplayTags.ini |  |

## GameplayEvent （5 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `GameplayEvent.Death` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | Event that fires on death. This event only fires on the server. |
| `GameplayEvent.MeleeHit` | INI | DefaultGameplayTags.ini |  |
| `GameplayEvent.ReloadDone` | INI | ShooterCoreTags.ini |  |
| `GameplayEvent.RequestReset` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | Event to request a player's pawn to be instantly replaced with a new one at a valid spawn location. |
| `GameplayEvent.Reset` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | Event that fires once a player reset is executed. |

## GameSettings （2 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `GameSettings.Action.EditBrightness` | C++ Native | Source/LyraGame/Settings/LyraGameSettingRegistry_Video.cpp |  |
| `GameSettings.Action.EditSafeZone` | INI + C++ Native | DefaultGameplayTags.ini<br>Source/LyraGame/Settings/LyraGameSettingRegistry_Video.cpp |  |

## HUD （15 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `HUD.Slot.EliminationFeed` | INI | ShooterCoreTags.ini |  |
| `HUD.Slot.Equipment` | INI | ShooterCoreTags.ini |  |
| `HUD.Slot.ExtraEquipment` | INI | DefaultGameplayTags.ini |  |
| `HUD.Slot.InfrequentAbilities` | INI | DefaultGameplayTags.ini |  |
| `HUD.Slot.LeftSideTouchInputs` | INI | DefaultGameplayTags.ini |  |
| `HUD.Slot.LeftSideTouchRegion` | INI | DefaultGameplayTags.ini |  |
| `HUD.Slot.ModeStatus` | INI | ShooterCoreTags.ini |  |
| `HUD.Slot.PerfStats.Graph` | INI | ShooterCoreTags.ini |  |
| `HUD.Slot.PerfStats.Text` | INI | ShooterCoreTags.ini |  |
| `HUD.Slot.RespawnTimer` | INI | DefaultGameplayTags.ini |  |
| `HUD.Slot.Reticle` | INI | ShooterCoreTags.ini |  |
| `HUD.Slot.RightSideTouchInputs` | INI | DefaultGameplayTags.ini |  |
| `HUD.Slot.RightSideTouchRegion` | INI | DefaultGameplayTags.ini |  |
| `HUD.Slot.TeamScore` | INI | ShooterCoreTags.ini |  |
| `HUD.Slot.TopAccolades` | INI | ShooterCoreTags.ini |  |

## InitState （4 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `InitState.DataAvailable` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | 2: All required data has been loaded/replicated and is ready for initialization |
| `InitState.DataInitialized` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | 3: The available data has been initialized for this actor/component, but it is not ready for full gameplay |
| `InitState.GameplayReady` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | 4: The actor/component is fully ready for active gameplay |
| `InitState.Spawned` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | 1: Actor/component has initially spawned and can be extended |

## InputTag （24 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `InputTag.Ability.Dash` | INI | DefaultGameplayTags.ini |  |
| `InputTag.Ability.Emote` | INI | ShooterCoreTags.ini |  |
| `InputTag.Ability.Heal` | INI | DefaultGameplayTags.ini |  |
| `InputTag.Ability.Interact` | INI | ShooterCoreTags.ini |  |
| `InputTag.Ability.Melee` | INI | DefaultGameplayTags.ini | Melee Input |
| `InputTag.Ability.Quickslot.CycleBackward` | INI | ShooterCoreTags.ini |  |
| `InputTag.Ability.Quickslot.CycleForward` | INI | ShooterCoreTags.ini |  |
| `InputTag.Ability.Quickslot.Drop` | INI | DefaultGameplayTags.ini |  |
| `InputTag.Ability.Quickslot.SelectSlot` | INI | ShooterCoreTags.ini | Used to directly select one of the quickbar slots. Intended to be accompanied with a 0-based slot index. |
| `InputTag.Ability.ShowLeaderboard` | INI | ShooterCoreTags.ini |  |
| `InputTag.Ability.ToggleInventory` | INI | ShooterCoreTags.ini |  |
| `InputTag.Ability.ToggleMap` | INI | ShooterCoreTags.ini |  |
| `InputTag.Ability.ToggleMarkerInWorld` | INI | ShooterCoreTags.ini |  |
| `InputTag.AutoRun` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | Auto-run input. |
| `InputTag.Crouch` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | Crouch input. |
| `InputTag.Jump` | INI | DefaultGameplayTags.ini |  |
| `InputTag.Look.Mouse` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | Look (mouse) input. |
| `InputTag.Look.Stick` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | Look (stick) input. |
| `InputTag.Move` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | Move input. |
| `InputTag.Weapon.ADS` | INI | DefaultGameplayTags.ini |  |
| `InputTag.Weapon.Fire` | INI | DefaultGameplayTags.ini |  |
| `InputTag.Weapon.FireAuto` | INI | DefaultGameplayTags.ini |  |
| `InputTag.Weapon.Grenade` | INI | DefaultGameplayTags.ini |  |
| `InputTag.Weapon.Reload` | INI | DefaultGameplayTags.ini |  |

## Lyra （28 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `Lyra.AddNotification.KillFeed` | INI | ShooterCoreTags.ini | SendKillFeedInfo to UI |
| `Lyra.AddNotification.Message` | C++ Native | Source/LyraGame/Messages/LyraNotificationMessage.cpp |  |
| `Lyra.Assist.Message` | C++ Native | Plugins/GameFeatures/ShooterCore/Source/ShooterCoreRuntime/Private/MessageProcessors/AssistProcessor.cpp |  |
| `Lyra.Damage.Message` | C++ Native | Source/LyraGame/AbilitySystem/Attributes/LyraHealthSet.cpp<br>Plugins/GameFeatures/ShooterCore/Source/ShooterCoreRuntime/Private/MessageProcessors/AssistProcessor.cpp |  |
| `Lyra.Damage.Taken.Message` | INI | DefaultGameplayTags.ini |  |
| `Lyra.Elimination.Message` | C++ Native | Source/LyraGame/Character/LyraHealthComponent.cpp<br>Plugins/GameFeatures/ShooterCore/Source/ShooterCoreRuntime/Private/MessageProcessors/AssistProcessor.cpp<br>Plugins/GameFeatures/ShooterCore/Source/ShooterCoreRuntime/Private/MessageProcessors/ElimChainProcessor.cpp<br>Plugins/GameFeatures/ShooterCore/Source/ShooterCoreRuntime/Private/MessageProcessors/ElimStreakProcessor.cpp |  |
| `Lyra.HUD.PlayerHUD` | INI | DefaultGameplayTags.ini |  |
| `Lyra.HUD.TempTopWidgets` | INI | DefaultGameplayTags.ini |  |
| `Lyra.Inventory.Message.StackChanged` | C++ Native | Source/LyraGame/Inventory/LyraInventoryManagerComponent.cpp |  |
| `Lyra.Player` | INI | DefaultGameplayTags.ini |  |
| `Lyra.QuickBar.Message.ActiveIndexChanged` | C++ Native | Source/LyraGame/Equipment/LyraQuickBarComponent.cpp |  |
| `Lyra.QuickBar.Message.SlotsChanged` | C++ Native | Source/LyraGame/Equipment/LyraQuickBarComponent.cpp |  |
| `Lyra.ShooterGame.Accolade` | C++ Native | Plugins/GameFeatures/ShooterCore/Source/ShooterCoreRuntime/Private/Accolades/LyraAccoladeHostWidget.cpp |  |
| `Lyra.ShooterGame.Accolade.EliminationChain` | INI | ShooterCoreTags.ini |  |
| `Lyra.ShooterGame.Accolade.EliminationChain.2x` | INI | ShooterCoreTags.ini |  |
| `Lyra.ShooterGame.Accolade.EliminationChain.3x` | INI | ShooterCoreTags.ini |  |
| `Lyra.ShooterGame.Accolade.EliminationChain.4x` | INI | ShooterCoreTags.ini |  |
| `Lyra.ShooterGame.Accolade.EliminationChain.5x` | INI | ShooterCoreTags.ini |  |
| `Lyra.ShooterGame.Accolade.EliminationStreak` | INI | ShooterCoreTags.ini |  |
| `Lyra.ShooterGame.Accolade.EliminationStreak.5` | INI | ShooterCoreTags.ini |  |
| `Lyra.ShooterGame.Accolade.EliminationStreak.10` | INI | ShooterCoreTags.ini |  |
| `Lyra.ShooterGame.Accolade.EliminationStreak.15` | INI | ShooterCoreTags.ini |  |
| `Lyra.ShooterGame.Accolade.EliminationStreak.20` | INI | ShooterCoreTags.ini |  |
| `Lyra.ShooterGame.TDM.TeamScore` | INI | ShooterCoreTags.ini |  |
| `Lyra.ShooterGame.Weapon.MagazineAmmo` | INI | ShooterCoreTags.ini |  |
| `Lyra.ShooterGame.Weapon.MagazineSize` | INI | ShooterCoreTags.ini |  |
| `Lyra.ShooterGame.Weapon.SpareAmmo` | INI | ShooterCoreTags.ini |  |
| `Lyra.Weapon.SteadyAimingCamera` | C++ Native | Source/LyraGame/Weapons/LyraRangedWeaponInstance.cpp |  |

## Movement （6 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `Movement.Mode.Custom` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | This is invalid and should be replaced with custom tags.  See LyraGameplayTags::CustomMovementModeTagMap. |
| `Movement.Mode.Falling` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | Default Character movement tag |
| `Movement.Mode.Flying` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | Default Character movement tag |
| `Movement.Mode.NavWalking` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | Default Character movement tag |
| `Movement.Mode.Swimming` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | Default Character movement tag |
| `Movement.Mode.Walking` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | Default Character movement tag |

## Platform （16 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `Platform.Trait.BinauralSettingControlledByOS` | INI + C++ Native | DefaultGameplayTags.ini<br>Source/LyraGame/Settings/LyraSettingsLocal.cpp |  |
| `Platform.Trait.CanExitApplication` | INI | DefaultGameplayTags.ini | Can we show a quit option to exit the application? |
| `Platform.Trait.Input.HardwareCursor` | INI + C++ Native | DefaultGameplayTags.ini<br>Source/LyraGame/UI/LyraGameViewportClient.cpp | Does this platform use a hardware mouse cursor? |
| `Platform.Trait.Input.HasStrictControllerPairing` | INI + C++ Native | DefaultGameplayTags.ini<br>Source/LyraGame/UI/Foundation/LyraControllerDisconnectedScreen.cpp | True on platforms which will ignore input from devices not mapped to a user. |
| `Platform.Trait.Input.PrimarlyController` | INI + C++ Native | DefaultGameplayTags.ini<br>Source/LyraGame/UI/LyraHUDLayout.cpp |  |
| `Platform.Trait.Input.PrimarlyTouchScreen` | INI | DefaultGameplayTags.ini |  |
| `Platform.Trait.Input.SupportsGamepad` | C++ Native | Source/LyraGame/Settings/LyraGameSettingRegistry_Gamepad.cpp |  |
| `Platform.Trait.Input.SupportsMouseAndKeyboard` | INI | DefaultGameplayTags.ini |  |
| `Platform.Trait.Input.SupportsTriggerHaptics` | C++ Native | Source/LyraGame/Settings/LyraGameSettingRegistry_Gamepad.cpp |  |
| `Platform.Trait.NeedsBrightnessAdjustment` | C++ Native | Source/LyraGame/Settings/LyraGameSettingRegistry_Video.cpp |  |
| `Platform.Trait.ReplaySupport` | C++ Native | Source/LyraGame/Replays/LyraReplaySubsystem.cpp |  |
| `Platform.Trait.RequiresStrictControllerMapping` | C++ Native | Plugins/CommonUser/Source/CommonUser/Private/CommonUserSubsystem.cpp |  |
| `Platform.Trait.SingleOnlineUser` | C++ Native | Source/LyraGame/UI/Frontend/LyraFrontendStateComponent.cpp<br>Plugins/CommonUser/Source/CommonUser/Private/CommonUserSubsystem.cpp |  |
| `Platform.Trait.SupportsBackgroundAudio` | INI + C++ Native | DefaultGameplayTags.ini<br>Source/LyraGame/Settings/LyraGameSettingRegistry_Audio.cpp |  |
| `Platform.Trait.SupportsChangingAudioOutputDevice` | INI + C++ Native | DefaultGameplayTags.ini<br>Source/LyraGame/Settings/LyraGameSettingRegistry_Audio.cpp |  |
| `Platform.Trait.SupportsWindowedMode` | INI + C++ Native | DefaultGameplayTags.ini<br>Source/LyraGame/Settings/LyraGameSettingRegistry_Video.cpp | Can we switch between fullscreen and windowed modes? |

## SetByCaller （2 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `SetByCaller.Damage` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | SetByCaller tag used by damage gameplay effects. |
| `SetByCaller.Heal` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | SetByCaller tag used by healing gameplay effects. |

## ShooterGame （11 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `ShooterGame.ControlPoint.Captured.Message` | INI | ShooterCoreTags.ini | Fired when a control point has been captured by a team |
| `ShooterGame.ControlPoint.TeamScore` | INI | ShooterCoreTags.ini |  |
| `ShooterGame.ExtensionPoint.AbilityBar` | INI | ShooterCoreTags.ini |  |
| `ShooterGame.GamePhase.MatchBeginCountdown` | INI | DefaultGameplayTags.ini | When this tag is used in a gameplay message, an expected duration is included in the payload. |
| `ShooterGame.GamePhase.Playing` | INI | ShooterCoreTags.ini |  |
| `ShooterGame.GamePhase.PostGame` | INI | ShooterCoreTags.ini |  |
| `ShooterGame.GamePhase.Warmup` | INI | ShooterCoreTags.ini |  |
| `ShooterGame.Score.Assists` | INI | ShooterCoreTags.ini |  |
| `ShooterGame.Score.ControlPointCapture` | INI | ShooterCoreTags.ini |  |
| `ShooterGame.Score.Deaths` | INI | ShooterCoreTags.ini |  |
| `ShooterGame.Score.Eliminations` | INI | ShooterCoreTags.ini |  |

## Status （6 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `Status.AutoRunning` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | Target is auto-running. |
| `Status.Crouching` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | Target is crouching. |
| `Status.Death` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | Target has the death status. |
| `Status.Death.Dead` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | Target has finished the death process. |
| `Status.Death.Dying` | C++ Native | Source/LyraGame/LyraGameplayTags.cpp | Target has begun the death process. |
| `Status.SpawningIn` | INI | DefaultGameplayTags.ini | Covers the time when the pawn is created until it is controlled and unlocked for play |

## Subtitle （2 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `Subtitle.TextColor.White` | INI | PluginTags.ini (GameSubtitles) |  |
| `Subtitle.TextColor.Yellow` | INI | PluginTags.ini (GameSubtitles) |  |

## SurfaceType （4 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `SurfaceType.Character` | DataTable | Content/ContextEffects/DT_SurfaceTypes.uasset |  |
| `SurfaceType.Concrete` | DataTable | Content/ContextEffects/DT_SurfaceTypes.uasset |  |
| `SurfaceType.Default` | DataTable | Content/ContextEffects/DT_SurfaceTypes.uasset |  |
| `SurfaceType.Glass` | DataTable | Content/ContextEffects/DT_SurfaceTypes.uasset |  |

## SystemMessage （4 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `SystemMessage.Display` | C++ Native | Plugins/CommonUser/Source/CommonUser/Private/CommonUserSubsystem.cpp |  |
| `SystemMessage.Error` | C++ Native | Plugins/CommonUser/Source/CommonUser/Private/CommonUserSubsystem.cpp |  |
| `SystemMessage.Error.InitializeLocalPlayerFailed` | C++ Native | Plugins/CommonUser/Source/CommonUser/Private/CommonUserSubsystem.cpp |  |
| `SystemMessage.Warning` | C++ Native | Plugins/CommonUser/Source/CommonUser/Private/CommonUserSubsystem.cpp |  |

## TopDownArena （3 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `TopDownArena.ExtensionPoint.AbilityBar` | INI | TopDownArenaTags.ini |  |
| `TopDownArena.ExtensionPoint.Players` | INI | TopDownArenaTags.ini |  |
| `TopDownArena.ExtensionPoint.StatsBar` | INI | TopDownArenaTags.ini |  |

## UI （6 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `UI.Action.Back` | INI | DefaultGameplayTags.ini |  |
| `UI.Action.Escape` | C++ Native | Source/LyraGame/UI/LyraHUDLayout.cpp |  |
| `UI.Layer.Game` | INI | DefaultGameplayTags.ini |  |
| `UI.Layer.GameMenu` | INI | DefaultGameplayTags.ini |  |
| `UI.Layer.Menu` | INI + C++ Native | DefaultGameplayTags.ini<br>Source/LyraGame/UI/Frontend/LyraFrontendStateComponent.cpp<br>Source/LyraGame/UI/LyraHUDLayout.cpp |  |
| `UI.Layer.Modal` | INI + C++ Native | DefaultGameplayTags.ini<br>Source/LyraGame/Settings/Widgets/LyraSettingsListEntrySetting_KeyboardInput.cpp<br>Source/LyraGame/UI/Subsystem/LyraUIMessaging.cpp |  |

## TODO （ 条）

| Tag | 定义来源 | 定义位置 | 备注 |
|---|---|---|---|
| `TODO.GameModeDamageImmunity` | INI | ShooterCoreTags.ini |  |

---

## 迁移清单

1. `Source/LyraGame/LyraGameplayTags.h` / `.cpp` 及所有出现 `UE_DEFINE_GAMEPLAY_TAG*` 的 C++ 文件（见上表 C++ Native 行）。
2. `Config/DefaultGameplayTags.ini` → 替换为同目录 `DefaultGameplayTags.ini`。
3. `Content/ContextEffects/DT_AnimEffectTags.uasset`、`DT_SurfaceTypes.uasset`（若拷贝，请取消合并 ini 中 `+GameplayTagTableList` 两行注释）。
4. 四个插件标签表：`ShooterCoreTags.ini` / `ShooterExplorerTags.ini` / `TopDownArenaTags.ini` / `PluginTags.ini`（已合并进新 ini，可不再单独拷贝；若要保留插件级隔离则带上，且需保证 GameFeature 插件挂载 / `AddTagIniSearchPath`）。
5. `Config/DefaultGame.ini` 的 `+GameplayCueNotifyPaths` 及 `Content/GameplayCueNotifies`、`Content/GameplayCues` 目录（GameplayCue 标签解析用）。

