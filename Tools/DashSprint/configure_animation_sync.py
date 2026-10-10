"""Add stride markers to owned movement assets and bind Sprint's isolated sync group."""
import unreal, json
from pathlib import Path

assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
folder='/Game/Main/Character/Hero/Anim/Movement/'
track='HodgeSprintSync'
markers={
    'A_Hero_Sprint_F': [('Hodge.RightFoot',.25),('Hodge.LeftFoot',.50)],
    'A_Hero_Dash_F': [('Hodge.LeftFoot',0.),('Hodge.RightFoot',.40),('Hodge.LeftFoot',.533333)],
    'A_Hero_Dash_B': [('Hodge.RightFoot',0.),('Hodge.LeftFoot',.033333),('Hodge.RightFoot',.433333),('Hodge.LeftFoot',.733333)],
    'A_Hero_Sprint_Pivot_RootMotion': [('Hodge.LeftFoot',.83),('Hodge.RightFoot',1.10),('Hodge.LeftFoot',1.30),('Hodge.RightFoot',1.55)],
}
modified=[]
for name,entries in markers.items():
    a=unreal.load_asset(folder+name)
    assert a and isinstance(a,unreal.AnimSequence)
    if track not in [str(n) for n in unreal.AnimationLibrary.get_animation_notify_track_names(a)]:
        unreal.AnimationLibrary.add_animation_notify_track(a,track,unreal.LinearColor(.2,.7,1.,1.))
    unreal.AnimationLibrary.remove_animation_sync_markers_by_track(a,track)
    for marker,t in entries:
        assert 0<=t<a.get_play_length(),(name,t)
        unreal.AnimationLibrary.add_animation_sync_marker(a,marker,t,track)
    modified.append(a)
for name in ['AM_Hero_Dash_F','AM_Hero_Dash_B','AM_Hero_Sprint_Pivot']:
    a=unreal.load_asset(folder+name);assert a
    assert unreal.HodgeAnimationAuthoringLibrary.configure_hero_movement_montage_sync(a),name
    value=a.get_editor_property('blend_out');value.import_text('(BlendTime='+str(.16 if name.endswith('Pivot') else .18)+')')
    a.set_editor_property('blend_out',value)
    if name.endswith('Pivot'):
        value=a.get_editor_property('blend_in');value.import_text('(BlendTime=.12)');a.set_editor_property('blend_in',value)
    modified.append(a)
profile=unreal.load_asset('/Game/Main/Character/Hero/Ability/Sprint/DA_Hero_SprintAbility');assert profile
profile.set_editor_property('PivotRecoveryEndTime',.9)
profile.set_editor_property('SprintTurnBlendOutTime',.16)
modified.append(profile)
layer=unreal.load_asset('/Game/Main/Character/Hero/Anim/Layer/ABP_Pover_LocomotionBase');assert layer
graph=unreal.HodgeAnimationAuthoringLibrary.configure_hero_sprint_sync(layer)
assert graph.startswith('OK'),graph
diagnostic=unreal.HodgeAnimationAuthoringLibrary.compile_animation_blueprint(layer)
assert diagnostic.startswith('ERRORS=0'),diagnostic
modified.append(layer)
for a in modified:assert unreal.EditorAssetLibrary.save_loaded_asset(a,False),a.get_path_name()
result={'markers':markers,'graph':graph,'diagnostic':diagnostic,'saved':[a.get_path_name() for a in modified],
        'pivot_recovery_end':.9,'pivot_blend_out':.16,'dash_blend_out':.18}
Path(unreal.Paths.project_saved_dir()).joinpath('AnimationSync/configuration.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
print(json.dumps(result))
