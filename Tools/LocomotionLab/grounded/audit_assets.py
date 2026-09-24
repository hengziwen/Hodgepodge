import unreal,json,math
from pathlib import Path
E=unreal.EditorAssetLibrary;A=unreal.AnimationLibrary
bp=E.load_asset('/Game/CodexText/AnimInstance/ALS/ABP_ALS_Grounded');hero=E.load_asset('/Game/CodexText/Locomotion/BP_LocomotionHero')
assert unreal.get_default_object(hero.generated_class()).get_component_by_class(unreal.SkeletalMeshComponent).get_editor_property('anim_class')==bp.generated_class()
report={'blueprint':bp.get_path_name(),'animations':{},'graphs':{}}
for path in E.list_assets('/Game/CodexText/Anim/Grounded',True,False):
 a=E.load_asset(path)
 if not isinstance(a,unreal.AnimSequence):continue
 curves=[str(n) for n in A.get_animation_curve_names(a,unreal.RawCurveTrackTypes.RCT_FLOAT)]
 assert all(n in curves for n in ['FootLock_L','FootLock_R','FootContact_L','FootContact_R'])
 roots=[]
 for t in [0,a.get_play_length()*.5,a.get_play_length()]:
  pose=unreal.AnimPoseExtensions.get_anim_pose_at_time(a,t,unreal.AnimPoseEvaluationOptions());root=unreal.AnimPoseExtensions.get_bone_pose(pose,'root',unreal.AnimPoseSpaces.WORLD)
  roots.append([root.translation.length(),root.rotation.rotator().yaw])
 assert all(abs(p)<.01 and abs(y)<.01 for p,y in roots),(path,roots)
 markers=[[str(m.marker_name),m.time] for m in A.get_animation_sync_markers(a)]
 if a.get_name().startswith(('Walk_','Run_')):assert {m[0] for m in markers}=={'L','R'}
 report['animations'][a.get_name()]={'length':a.get_play_length(),'markers':markers,'root_samples':roots}
for node in unreal.ObjectIterator(unreal.EdGraphNode):
 if node.get_path_name().startswith(bp.get_path_name()+':'):
  g=node.get_outer().get_path_name();c=node.get_class().get_name();report['graphs'].setdefault(g,{});report['graphs'][g][c]=report['graphs'][g].get(c,0)+1
report['dirty']=[p.get_path_name() for p in unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()+unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()]
Path('D:/CodexUE55Lab/grounded-final-audit.json').write_text(json.dumps(report,indent=2));print('ASSETS_AUDITED',len(report['animations']),'DIRTY',report['dirty'])
