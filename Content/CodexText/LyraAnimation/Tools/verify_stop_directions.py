import unreal, json, math, traceback
from pathlib import Path

out = Path(unreal.Paths.project_saved_dir(), 'LyraAnimationWork', 'StopTurn-20261004')
worlds = unreal.EditorLevelLibrary.get_pie_worlds(False)
assert len(worlds) == 1, 'Use single-player lab PIE'
world = worlds[0]
hero = unreal.GameplayStatics.get_player_character(world, 0)
assert hero and hero.get_class().get_name() in ['BP_Pover_LyraHero_C', 'BP_Hero_Pover_C']
controller = unreal.GameplayStatics.get_player_controller(world, 0)
anim = hero.mesh.get_anim_instance()
layer = anim.get_linked_anim_layer_instance_by_class(unreal.load_asset('/Game/Main/Character/Hero/Anim/Layer/ABP_Pover_LocomotionBase').generated_class())
hero.character_movement.stop_movement_immediately()
controller.set_control_rotation(unreal.Rotator(pitch=-20,yaw=0))
hero.set_actor_location(unreal.Vector(-1200,-800,100),False,True)
directions = [('F',unreal.Vector(1,0,0)),('B',unreal.Vector(-1,0,0)),('L',unreal.Vector(0,-1,0)),('R',unreal.Vector(0,1,0))]
start = unreal.GameplayStatics.get_time_seconds(world)
rows = []
last = -1.0

def finish(error=None):
    unreal.unregister_slate_post_tick_callback(handle)
    (out/'stop-directions.json').write_text(json.dumps(dict(samples=rows,error=error),indent=2),encoding='utf-8')

def tick(delta):
    global last
    try:
        t = unreal.GameplayStatics.get_time_seconds(world)-start
        if t >= 24:
            finish()
            return
        index = int(t/6)
        side, direction = directions[index]
        local = t-index*6
        if 0.75 <= local < 2:
            hero.add_movement_input(direction,1,False)
        if t-last >= 0.03:
            last = t
            velocity = hero.get_velocity()
            rows.append(dict(time=t,side=side,local_time=local,speed=math.hypot(velocity.x,velocity.y),
                stop_time=layer.get_editor_property('StopAnimTime'),stop_weight=layer.get_editor_property('StopAnimWeight'),
                acceleration=anim.get_editor_property('HasAcceleration'),distance=anim.get_curve_value('Distance'),
                grounded=anim.get_editor_property('IsOnGround'),root_yaw=anim.get_editor_property('RootYawOffset'),
                feet={name:str(hero.mesh.get_socket_location(name)) for name in ['Bip001LFoot','Bip001RFoot']}))
    except Exception:
        finish(traceback.format_exc())

handle = unreal.register_slate_post_tick_callback(tick)
print('Four directions: move, release, complete Stop, resume directly in Cycle (24 game seconds)')
