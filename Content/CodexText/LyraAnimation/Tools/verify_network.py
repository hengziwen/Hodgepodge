import unreal, json, traceback
from pathlib import Path
Path(unreal.Paths.project_saved_dir(), 'LyraAnimationWork').mkdir(parents=True, exist_ok=True)
worlds = unreal.EditorLevelLibrary.get_pie_worlds(False)
assert len(worlds) == 2, 'Expected two PIE worlds, got ' + str(len(worlds))
locals = [(world, unreal.GameplayStatics.get_player_character(world, 0), unreal.GameplayStatics.get_player_controller(world, 0)) for world in worlds]
assert all(hero for world, hero, controller in locals)
layer_class = unreal.load_asset('/Game/Main/Character/Hero/Anim/Layer/ABP_Pover_LocomotionBase').generated_class()
hero_class = locals[0][1].get_class()
start = unreal.GameplayStatics.get_time_seconds(worlds[0])
samples = []
previous = None
last_sample = -1

def finish(error=None):
    unreal.unregister_slate_post_tick_callback(handle)
    for world, hero, controller in locals:
        if unreal.SystemLibrary.is_valid(hero):
            hero.stop_jumping()
    Path(unreal.Paths.project_saved_dir(), 'LyraAnimationWork', 'network-verification.json').write_text(json.dumps({'samples': samples, 'error': error}, indent=2), encoding='utf-8')

def tick(delta):
    global previous, last_sample
    try:
        t = unreal.GameplayStatics.get_time_seconds(worlds[0]) - start
        phase = 'move' if t < 2 else 'jump' if t < 3.8 else 'attack' if t < 6.8 else 'land'
        if t > 8:
            finish()
            return
        if phase != previous:
            previous = phase
            for world, hero, controller in locals:
                if phase == 'jump':
                    hero.jump()
                elif phase == 'attack':
                    hero.stop_jumping()
                    library = unreal.get_default_object(unreal.load_class(None, '/Script/Engine.SubsystemBlueprintLibrary'))
                    subsystem = library.call_method('GetLocalPlayerSubSystemFromPlayerController', (controller, unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
                    subsystem.inject_input_vector_for_action(unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack'), unreal.Vector(1, 0, 0), [], [])
        if phase == 'move':
            for world, hero, controller in locals:
                hero.add_movement_input(unreal.Vector(1, 0, 0), 1, False)
        if t - last_sample > 0.12:
            last_sample = t
            for world in worlds:
                for hero in unreal.GameplayStatics.get_all_actors_of_class(world, hero_class):
                    anim = hero.mesh.get_anim_instance()
                    layer = anim.get_linked_anim_layer_instance_by_class(layer_class)
                    velocity = hero.get_velocity()
                    montage = anim.get_current_active_montage()
                    samples.append({'time': round(t, 3), 'phase': phase, 'world': world.get_path_name(), 'hero': hero.get_name(), 'role': str(hero.get_local_role()), 'velocity': [velocity.x, velocity.y, velocity.z], 'anim': anim.get_class().get_name(), 'layer': layer.get_class().get_name() if layer else None, 'grounded': anim.get_editor_property('IsOnGround'), 'jumping': anim.get_editor_property('IsJumping'), 'falling': anim.get_editor_property('IsFalling'), 'attack_tag': anim.get_editor_property('GameplayTag_IsMelee'), 'montage': montage.get_name() if montage else None, 'slot_weight': anim.blueprint_get_slot_montage_local_weight('FullBody'), 'equipment_actors': [a.get_class().get_name() for a in hero.get_attached_actors()]})
    except Exception:
        finish(traceback.format_exc())

handle = unreal.register_slate_post_tick_callback(tick)
print('Two-player runtime verification scheduled')
