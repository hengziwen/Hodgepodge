import unreal, json, traceback, math
from pathlib import Path
out = Path(unreal.Paths.project_saved_dir(), 'LyraAnimationWork', 'StopTurn-20261004')
out.mkdir(parents=True, exist_ok=True)
worlds = unreal.EditorLevelLibrary.get_pie_worlds(False)
assert len(worlds) == 1, 'Use single-player lab PIE'
world = worlds[0]
hero = unreal.GameplayStatics.get_player_character(world, 0)
assert hero and hero.get_class().get_name() in ['BP_Pover_LyraHero_C', 'BP_Hero_Pover_C']
controller = unreal.GameplayStatics.get_player_controller(world, 0)
anim = hero.mesh.get_anim_instance()
layer = anim.get_linked_anim_layer_instance_by_class(unreal.load_asset('/Game/Main/Character/Hero/Anim/Layer/ABP_Pover_LocomotionBase').generated_class())
hero.character_movement.stop_movement_immediately()
hero.set_actor_location(unreal.Vector(-600,0,100),False,True)
controller.set_control_rotation(unreal.Rotator(pitch=-20,yaw=0))
start = unreal.GameplayStatics.get_time_seconds(world)
samples = []
last_sample = -1.0
def phase_at(t):
    if t < 0.75: return 'idle'
    if t < 2.0: return 'move'
    if t < 5.0: return 'stop'
    if t < 11.0: return 'continuous_right'
    if t < 17.0: return 'continuous_left'
    if t < 22.0: return 'alternate'
    return 'settle'
def yaw_at(t):
    if t < 5: return 0
    if t < 11: return (t-5)*90
    if t < 17: return 540-(t-11)*90
    if t < 22:
        elapsed = t-17
        cycle = int(elapsed/0.75)
        f = elapsed-cycle*0.75
        return f*90 if cycle%2 == 0 else 67.5-f*90
    return 45
def finish(error=None):
    unreal.unregister_slate_post_tick_callback(handle)
    report = {'samples':samples,'error':error}
    (out/'continuous-current.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
def tick(delta):
    global last_sample
    try:
        t = unreal.GameplayStatics.get_time_seconds(world)-start
        if t > 25:
            finish()
            return
        phase = phase_at(t)
        if phase == 'move':
            hero.add_movement_input(unreal.Vector(1,0,0),1,False)
        controller.set_control_rotation(unreal.Rotator(pitch=-20,yaw=yaw_at(t)))
        if t-last_sample > 0.03:
            last_sample = t
            velocity = hero.get_velocity()
            row = {'time': t, 'phase':phase,'speed':math.hypot(velocity.x,velocity.y),'actor_yaw':hero.get_actor_rotation().yaw,'root_yaw':anim.get_editor_property('RootYawOffset'),'turn_curve':anim.get_editor_property('TurnYawCurveValue'),'weight':anim.get_curve_value('TurnYawWeight'),'remaining_yaw':anim.get_curve_value('RemainingTurnYaw'),'distance':anim.get_curve_value('Distance'),'turn_time':layer.get_editor_property('TurnInPlaceAnimTime'),'turn_direction':layer.get_editor_property('TurnInPlaceRotationDirection'),'recovery_direction':layer.get_editor_property('TurnInPlaceRecoveryDirection'),'stop_time':layer.get_editor_property('StopAnimTime'),'stop_weight':layer.get_editor_property('StopAnimWeight'),'acceleration':anim.get_editor_property('HasAcceleration'),'bones':{}}
            for name in ['Root','Bip001Pelvis','Bip001LFoot','Bip001RFoot']:
                transform = hero.mesh.get_socket_transform(name,unreal.RelativeTransformSpace.RTS_WORLD)
                row['bones'][name] = {'position':[transform.translation.x,transform.translation.y,transform.translation.z], 'quat':[transform.rotation.x,transform.rotation.y,transform.rotation.z,transform.rotation.w]}
            samples.append(row)
    except Exception:
        finish(traceback.format_exc())
handle = unreal.register_slate_post_tick_callback(tick)
print('25 second move/stop and continuously changing camera regression scheduled')
