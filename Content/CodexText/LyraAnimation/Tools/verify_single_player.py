import unreal, json, math, traceback
from pathlib import Path

out = Path(unreal.Paths.project_saved_dir()) / 'LyraAnimationWork'
out.mkdir(parents=True, exist_ok=True)
worlds = unreal.EditorLevelLibrary.get_pie_worlds(False)
assert len(worlds) == 1, 'Run this script in single-player lab PIE'
world = worlds[0]
hero = unreal.GameplayStatics.get_player_character(world, 0)
assert hero and hero.get_class().get_name() in ['BP_Pover_LyraHero_C', 'BP_Hero_Pover_C']
controller = unreal.GameplayStatics.get_player_controller(world, 0)
anim = hero.mesh.get_anim_instance()
layer_class = unreal.load_asset('/Game/Main/Character/Hero/Anim/Layer/ABP_Pover_LocomotionBase').generated_class()
layer = anim.get_linked_anim_layer_instance_by_class(layer_class)
asc = controller.player_state.get_component_by_class(unreal.load_class(None, '/Script/Hodgepodge.HodgeAbilitySystemComponent'))
movement = hero.character_movement
movement.stop_movement_immediately()
hero.set_actor_location(unreal.Vector(-600, 0, 100), False, True)
old_orient = movement.get_editor_property('orient_rotation_to_movement')
movement.set_editor_property('orient_rotation_to_movement', False)
hero.set_actor_rotation(unreal.Rotator(pitch=0, yaw=0, roll=0), False)
controller.set_control_rotation(unreal.Rotator(pitch=-12, yaw=0, roll=0))
start = unreal.GameplayStatics.get_time_seconds(world)
samples = []
events = {}
last_sample = -1
previous_phase = None
phases = [(1, 'idle'), (3, 'forward'), (4, 'stop'), (5, 'restart'), (6, 'reverse'), (7, 'stop2'), (8, 'left'), (9, 'right'), (10, 'turn'), (12.5, 'jump'), (16, 'attack'), (17, 'equipment'), (18, 'final')]
extra_equipment = None

def phase_at(t):
    return next((name for end, name in phases if t < end), 'done')

def finish(error=None):
    unreal.unregister_slate_post_tick_callback(handle)
    if unreal.SystemLibrary.is_valid(hero):
        movement.set_editor_property('orient_rotation_to_movement', old_orient)
        hero.stop_jumping()
        if extra_equipment:
            equipment = hero.get_component_by_class(unreal.load_class(None, '/Script/Hodgepodge.HodgeEquipmentManagerComponent'))
            equipment.unequip_item(extra_equipment)
    report = {'events': events, 'samples': samples, 'error': error}
    (out / 'runtime-verification.json').write_text(json.dumps(report, indent=2, ensure_ascii=False), encoding='utf-8')

def tick(delta):
    global previous_phase, last_sample, extra_equipment
    try:
        t = unreal.GameplayStatics.get_time_seconds(world) - start
        phase = phase_at(t)
        if phase == 'done':
            finish()
            return
        if phase != previous_phase:
            if previous_phase == 'jump':
                hero.stop_jumping()
            previous_phase = phase
            if phase == 'turn':
                controller.set_control_rotation(unreal.Rotator(pitch=-20, yaw=100, roll=0))
            elif phase == 'jump':
                hero.jump()
            elif phase == 'attack':
                library = unreal.get_default_object(unreal.load_class(None, '/Script/Engine.SubsystemBlueprintLibrary'))
                subsystem = library.call_method('GetLocalPlayerSubSystemFromPlayerController', (controller, unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
                action = unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack')
                assert action, 'Attack input asset missing'
                subsystem.inject_input_vector_for_action(action, unreal.Vector(1, 0, 0), [], [])
                events['attack_input_injected'] = True
            elif phase == 'equipment':
                equipment = hero.get_component_by_class(unreal.load_class(None, '/Script/Hodgepodge.HodgeEquipmentManagerComponent'))
                definition = unreal.load_asset('/Game/Main/Data/Equipments/BP_Equipment_Sword').generated_class()
                before = anim.get_linked_anim_layer_instance_by_class(layer_class)
                extra_equipment = equipment.equip_item(definition)
                events['equipped_extra'] = bool(extra_equipment)
                events['fixed_layer_after_equip'] = before == anim.get_linked_anim_layer_instance_by_class(layer_class)
                if extra_equipment:
                    equipment.unequip_item(extra_equipment)
                    extra_equipment = None
                events['fixed_layer_after_unequip'] = before == anim.get_linked_anim_layer_instance_by_class(layer_class)
        direction = {'forward': (1, 0), 'restart': (1, 0), 'reverse': (-1, 0), 'left': (0, -1), 'right': (0, 1)}.get(phase)
        if direction:
            hero.add_movement_input(unreal.Vector(direction[0], direction[1], 0), 1, False)
        if t - last_sample > 0.08:
            last_sample = t
            vel = hero.get_velocity()
            montage = anim.get_current_active_montage()
            row = {'time': round(t, 3), 'phase': phase, 'velocity': [vel.x, vel.y, vel.z], 'montage': montage.get_name() if montage else None, 'slot_weight': anim.blueprint_get_slot_montage_local_weight('FullBody')}
            for name in ['HasVelocity', 'HasAcceleration', 'IsOnGround', 'IsJumping', 'IsFalling', 'GroundDistance', 'RootYawOffset', 'GameplayTag_IsMelee', 'TimeToJumpApex', 'LocalVelocityDirection', 'RootYawOffsetMode']:
                try:
                    value = anim.get_editor_property(name)
                    row[name] = value if isinstance(value, (bool, int, float, str)) else str(value)
                except Exception:
                    pass
            row['curves'] = {name: anim.get_curve_value(name) for name in ['Distance', 'RemainingTurnYaw', 'TurnYawWeight', 'GroundDistance']}
            feet = {}
            for bone in ['Root', 'Bip001Pelvis', 'Bip001LFoot', 'Bip001RFoot', 'Bip001LToe0', 'Bip001RToe0']:
                v = hero.mesh.get_socket_location(bone)
                feet[bone] = [v.x, v.y, v.z]
                assert all(math.isfinite(a) for a in feet[bone]), 'Non-finite pose'
            row['bones'] = feet
            row['turn_time'] = layer.get_editor_property('TurnInPlaceAnimTime')
            row['fixed_layer'] = layer == anim.get_linked_anim_layer_instance_by_class(layer_class)
            samples.append(row)
    except Exception:
        finish(traceback.format_exc())

handle = unreal.register_slate_post_tick_callback(tick)
print('Runtime verification callback registered; finishes after 18 game seconds')
