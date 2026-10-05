import unreal, json, math, traceback
from pathlib import Path

out=Path(unreal.Paths.project_saved_dir(),'LyraAnimationWork','Attack-20261005')
worlds=unreal.EditorLevelLibrary.get_pie_worlds(False)
assert len(worlds)==1,'Use single-player lab PIE'
world=worlds[0]
hero=unreal.GameplayStatics.get_player_character(world,0)
assert hero.get_class().get_name() in ['BP_Pover_LyraHero_C', 'BP_Hero_Pover_C']
controller=unreal.GameplayStatics.get_player_controller(world,0)
anim=hero.mesh.get_anim_instance()
library=unreal.get_default_object(unreal.load_class(None,'/Script/Engine.SubsystemBlueprintLibrary'))
subsystem=library.call_method('GetLocalPlayerSubSystemFromPlayerController',(controller,unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
settings=unreal.get_default_object(unreal.load_class(None,'/Script/UnrealEd.EditorPerformanceSettings'))
old_throttle=settings.get_editor_property('bThrottleCPUWhenNotForeground')
settings.set_editor_property('bThrottleCPUWhenNotForeground',False)
hero.character_movement.stop_movement_immediately()
hero.set_actor_location(unreal.Vector(-600,0,100),False,True)
controller.set_control_rotation(unreal.Rotator(pitch=-20,yaw=0))
anim.montage_stop(0.0)
start=unreal.GameplayStatics.get_time_seconds(world)
rows=[]
events={}
last=-1.0

def finish(error=None):
    unreal.unregister_slate_post_tick_callback(handle)
    settings.set_editor_property('bThrottleCPUWhenNotForeground',old_throttle)
    (out/'input-recovery.json').write_text(json.dumps(dict(samples=rows,events=events,error=error),indent=2),encoding='utf-8')

def tick(delta):
    global last
    try:
        t=unreal.GameplayStatics.get_time_seconds(world)-start
        if t>11:
            finish()
            return
        if t>=0.75 and 'attack_input' not in events:
            subsystem.inject_input_vector_for_action(unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack'),unreal.Vector(1,0,0),[],[])
            events['attack_input']=t
        if t>=6 and 'upperbody' not in events:
            seq=unreal.load_asset('/Game/Main/Character/Hero/Anim/Sequences/Attack/AM_Attack01')
            result=anim.play_slot_animation_as_dynamic_montage(seq,'UpperBody',0.1,0.2,1.0,1)
            assert result,'UpperBody transient montage failed'
            events['upperbody']=t
        if 6<=t<9.5:
            hero.add_movement_input(unreal.Vector(1,0,0),1,False)
        if t-last<0.03:return
        last=t
        phase='before' if t<0.75 else 'input_attack' if t<4.5 else 'recovery' if t<6 else 'upperbody_move' if t<9.5 else 'after'
        montage=anim.get_current_active_montage()
        velocity=hero.get_velocity()
        row=dict(time=t,phase=phase,montage=montage.get_path_name() if montage else None,
            fullbody_weight=anim.blueprint_get_slot_montage_local_weight('FullBody'),
            upperbody_weight=anim.blueprint_get_slot_montage_local_weight('UpperBody'),
            attack_tag=anim.get_editor_property('GameplayTag_IsMelee'),speed=math.hypot(velocity.x,velocity.y),
            grounded=anim.get_editor_property('IsOnGround'),feet={})
        for side in ['L','R']:
            fk=hero.mesh.get_socket_transform('Bip001'+side+'Foot',unreal.RelativeTransformSpace.RTS_COMPONENT).translation
            ik=hero.mesh.get_socket_transform('VB Root_Bip001'+side+'Foot',unreal.RelativeTransformSpace.RTS_COMPONENT).translation
            row['feet'][side]=dict(fk=[fk.x,fk.y,fk.z],ik=[ik.x,ik.y,ik.z],distance=math.dist([fk.x,fk.y,fk.z],[ik.x,ik.y,ik.z]))
        rows.append(row)
    except Exception:
        finish(traceback.format_exc())

handle=unreal.register_slate_post_tick_callback(tick)
print('Attack input/GAS, IK recovery and moving UpperBody montage regression scheduled (11 game seconds)')
