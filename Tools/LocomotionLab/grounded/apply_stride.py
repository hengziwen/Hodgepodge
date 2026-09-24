import unreal,json,math
from pathlib import Path
E=unreal.EditorAssetLibrary
bp=E.load_asset('/Game/CodexText/AnimInstance/ALS/ABP_ALS_Grounded')
assert unreal.HodgeGroundedAuthoring.add_stride_layer(bp)
idle=E.load_asset('/Game/CodexText/Anim/Grounded/Stand1')
opts=unreal.AnimPoseEvaluationOptions()
ip=unreal.AnimPoseExtensions.get_anim_pose_at_time(idle,0,opts)
bones=['Bip001LFoot','Bip001RFoot','Bip001LCalf','Bip001RCalf']
base=[unreal.AnimPoseExtensions.get_bone_pose(ip,b,unreal.AnimPoseSpaces.WORLD).translation for b in bones]
starts={}
for n in list(unreal.ObjectIterator(unreal.EdGraphNode)):
 if not n.get_path_name().startswith(bp.get_path_name()+':'):continue
 if n.get_class().get_name()=='AnimGraphNode_SequencePlayer':
  v=n.get_editor_property('node');a=v.get_editor_property('sequence')
  if not a or a.get_name() not in [g+'_'+d for g in ['Walk','Run'] for d in ['F','B','LF','LB','RF','RB']]:continue
  name=a.get_name()
  if name not in starts:
   scored=[]
   for j in range(60):
    t=a.get_play_length()*j/60;p=unreal.AnimPoseExtensions.get_anim_pose_at_time(a,t,opts)
    score=sum((unreal.AnimPoseExtensions.get_bone_pose(p,b,unreal.AnimPoseSpaces.WORLD).translation-x).length_squared() for b,x in zip(bones,base))
    scored.append((score,t))
   starts[name]=min(scored)[1]
  v.set_editor_property('start_position',starts[name]);n.set_editor_property('node',v)
 if n.get_class().get_name()=='AnimStateTransitionNode':
  # Existing immediate-stop branches are the only zero-duration transitions in this graph.
  if abs(n.get_editor_property('crossfade_duration'))<.0001:n.set_editor_property('crossfade_duration',.1)
assert unreal.HodgeLocomotionLabAuthoring.remap_copy(bp,[],[])
assert E.save_loaded_asset(bp,False)
Path('D:/CodexUE55Lab/stride-start-positions.json').write_text(json.dumps(starts,indent=2))
print('STRIDE_SAVED',starts)
print('TOES',[str(b) for b in unreal.AnimationLibrary.get_animation_track_names(idle) if 'toe' in str(b).lower()])
