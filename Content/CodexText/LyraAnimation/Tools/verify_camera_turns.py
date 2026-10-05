import unreal, json, math, traceback
from pathlib import Path
out = Path(unreal.Paths.project_saved_dir(), 'LyraAnimationWork', 'Fixes-20261004')
out.mkdir(parents=True, exist_ok=True)
world = unreal.EditorLevelLibrary.get_pie_worlds(False)[0]
hero = unreal.GameplayStatics.get_player_character(world, 0)
controller = unreal.GameplayStatics.get_player_controller(world, 0)
anim = hero.mesh.get_anim_instance()
layer = anim.get_linked_anim_layer_instance_by_class(unreal.load_asset('/Game/Main/Character/Hero/Anim/Layer/ABP_Pover_LocomotionBase').generated_class())
hero.character_movement.stop_movement_immediately()
hero.set_actor_location(unreal.Vector(-600,0,100), False, True)
controller.set_control_rotation(unreal.Rotator(pitch=-20, yaw=0))
start = unreal.GameplayStatics.get_time_seconds(world)
samples = []
last_sample = -1
last_step = -1
yaws = [0, 100, 200, 300, 40, -60]
def finish(error=None):
    unreal.unregister_slate_post_tick_callback(handle)
    (out/'camera-turn-verification.json').write_text(json.dumps({'samples': samples, 'error': error}, indent=2), encoding='utf-8')
def tick(delta):
    global last_sample, last_step
    try:
        t = unreal.GameplayStatics.get_time_seconds(world) - start
        step = min(int(t/3),len(yaws)-1)
        if t > 18:
            finish()
            return
        if step != last_step:
            last_step = step
            controller.set_control_rotation(unreal.Rotator(pitch=-20,yaw=yaws[step]))
        if t - last_sample > 0.06:
            last_sample = t
            pose = {}
            for bone in ['Bip001LFoot','Bip001LToe0','Bip001RFoot','Bip001RToe0']:
                v = hero.mesh.get_socket_location(bone)
                pose[bone] = [v.x,v.y,v.z]
                assert all(math.isfinite(x) for x in pose[bone])
            samples.append({'time':round(t,3),'step':step,'camera_yaw':controller.get_control_rotation().yaw,'actor_yaw':hero.get_actor_rotation().yaw,'root_yaw':anim.get_editor_property('RootYawOffset'),'turn_time':layer.get_editor_property('TurnInPlaceAnimTime'),'turn_direction':layer.get_editor_property('TurnInPlaceRotationDirection'),'weight':anim.get_curve_value('TurnYawWeight'),'remaining_yaw':anim.get_curve_value('RemainingTurnYaw'),'feet':pose})
    except Exception:
        finish(traceback.format_exc())
handle = unreal.register_slate_post_tick_callback(tick)
print('Six camera headings queued, including reverse direction')
