import unreal,json
from pathlib import Path
E=unreal.EditorAssetLibrary;A=unreal.AnimationLibrary
bp=E.load_asset('/Game/CodexText/AnimInstance/ALS/ABP_ALS_Grounded')
assert unreal.HodgeGroundedAuthoring.add_terrain_layer(bp)
for p in E.list_assets('/Game/CodexText/Anim/Grounded',True,False):
 a=E.load_asset(p)
 if not isinstance(a,unreal.AnimSequence):continue
 for side in ['L','R']:
  t,v=A.get_float_keys(a,'FootContact_'+side)
  name='FootSpeed_'+side
  if A.does_curve_exist(a,name,unreal.RawCurveTrackTypes.RCT_FLOAT):A.remove_curve(a,name,False)
  A.add_curve(a,name);A.add_float_curve_keys(a,name,t,[220*(1-x) for x in v])
 assert E.save_loaded_asset(a,False)
assert E.save_loaded_asset(bp,False)
print('TERRAIN_SAVED')
s=unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
existing={a.get_actor_label() for a in s.get_all_level_actors()}
cube=E.load_asset('/Engine/BasicShapes/Cube')
for label,pos,scale,rot in [('CodexTerrain_Slope',(2000,0,80),(6,6,.5),(15,0,0)),('CodexTerrain_StepLow',(2700,-150,50),(6,3,1),(0,0,0)),('CodexTerrain_StepHigh',(2700,150,57.5),(6,3,1.15),(0,0,0))]:
 if label in existing:continue
 actor=s.spawn_actor_from_class(unreal.StaticMeshActor,unreal.Vector(*pos),unreal.Rotator(pitch=rot[0],yaw=rot[1],roll=rot[2]))
 actor.set_actor_label(label);actor.static_mesh_component.set_static_mesh(cube);actor.set_actor_scale3d(unreal.Vector(*scale));actor.static_mesh_component.set_collision_profile_name('BlockAll')
assert unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).save_current_level()
print('TERRAIN_FIXTURES_SAVED')
