import unreal, json, math, traceback
from pathlib import Path

out = Path(unreal.Paths.project_saved_dir(),'LyraAnimationWork','Attack-20261005')
out.mkdir(parents=True,exist_ok=True)
worlds = unreal.EditorLevelLibrary.get_pie_worlds(False)
assert len(worlds)==1, 'Use single-player lab PIE'
world=worlds[0]
hero=unreal.GameplayStatics.get_player_character(world,0)
assert hero and hero.get_class().get_name() in ['BP_Pover_LyraHero_C', 'BP_Hero_Pover_C']
controller=unreal.GameplayStatics.get_player_controller(world,0)
anim=hero.mesh.get_anim_instance()
layer=anim.get_linked_anim_layer_instance_by_class(unreal.load_asset('/Game/Main/Character/Hero/Anim/Layer/ABP_Pover_LocomotionBase').generated_class())
mesh=unreal.load_asset('/Game/Main/Character/Hero/Anim/Model/SKM_Pover_LyraLab')
settings=unreal.get_default_object(unreal.load_class(None,'/Script/UnrealEd.EditorPerformanceSettings'))
old_throttle=settings.get_editor_property('bThrottleCPUWhenNotForeground')
settings.set_editor_property('bThrottleCPUWhenNotForeground',False)
hero.character_movement.stop_movement_immediately()
hero.set_actor_location(unreal.Vector(-600,0,100),False,True)
controller.set_control_rotation(unreal.Rotator(pitch=-20,yaw=0))
anim.montage_stop(0.0)
tracked=['Bip001Pelvis','Bip001LThigh','Bip001LCalf','Bip001LFoot','Bip001RThigh','Bip001RCalf','Bip001RFoot']
parents={}
def include(bone):
    if bone in parents: return
    parent=str(hero.mesh.get_parent_bone(bone))
    if parent!='None': include(parent)
    parents[bone]=parent
for bone in tracked: include(bone)
names=list(parents)
montages=[unreal.load_asset('/Game/Main/Character/Hero/Anim/Montages/AM_Attack%02d_Montage'%i) for i in range(1,6)]
schedule=[]
end=0.75
for montage in montages:
    begin=end
    end=begin+montage.get_play_length()+0.65
    schedule.append((begin,end,montage))
start=unreal.GameplayStatics.get_time_seconds(world)
rows=[]
events=[]
last=-1.0
active=-1

def finish(error=None):
    unreal.unregister_slate_post_tick_callback(handle)
    settings.set_editor_property('bThrottleCPUWhenNotForeground',old_throttle)
    (out/'attack-current.json').write_text(json.dumps(dict(samples=rows,events=events,error=error),indent=2),encoding='utf-8')

def tick(delta):
    global last,active
    try:
        t=unreal.GameplayStatics.get_time_seconds(world)-start
        if t>end+0.75:
            finish()
            return
        index=next((i for i,(begin,stop,m) in enumerate(schedule) if begin<=t<stop),-1)
        if index!=active and index>=0:
            active=index
            montage=montages[index]
            played=anim.montage_play(montage,1.0)
            assert played>0,montage.get_name()
            events.append(dict(time=t,montage=montage.get_name(),played_length=played))
        if t-last<0.03: return
        last=t
        montage=anim.get_current_active_montage()
        row=dict(time=t,montage=montage.get_name() if montage else None,
            position=anim.montage_get_position(montage) if montage else 0.0,
            fullbody_weight=anim.blueprint_get_slot_montage_local_weight('FullBody'),
            upperbody_weight=anim.blueprint_get_slot_montage_local_weight('UpperBody'),
            disable_leg_curve=anim.get_curve_value('DisableLegIK'),foot_enabled=layer.call_method('ShouldEnableFootPlacement',()),
            bones={},expected={},errors={})
        root=hero.mesh.get_socket_transform('Root',unreal.RelativeTransformSpace.RTS_COMPONENT)
        for bone in tracked:
            transform=hero.mesh.get_socket_transform(bone,unreal.RelativeTransformSpace.RTS_COMPONENT)
            position=unreal.MathLibrary.inverse_transform_location(root,transform.translation)
            row['bones'][bone]=[position.x,position.y,position.z]
        if montage and row['fullbody_weight']>.95:
            seq=montage.get_first_anim_reference()
            poses=unreal.AnimationLibrary.get_bone_poses_for_time(seq,names,row['position'],False,mesh)
            component={}
            for bone,pose in zip(names,poses):
                parent=parents[bone]
                component[bone]=unreal.MathLibrary.compose_transforms(pose,component[parent]) if parent!='None' else pose
            for bone in tracked:
                position=unreal.MathLibrary.inverse_transform_location(component['Root'],component[bone].translation)
                expected=[position.x,position.y,position.z]
                row['expected'][bone]=expected
                row['errors'][bone]=math.dist(row['bones'][bone],expected)
        rows.append(row)
    except Exception:
        finish(traceback.format_exc())

handle=unreal.register_slate_post_tick_callback(tick)
print('Five FullBody attacks and source/final pelvis-thigh-calf-foot comparison scheduled:',end+0.75,'game seconds')
