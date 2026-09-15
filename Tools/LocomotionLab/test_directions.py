import unreal,math,json
from pathlib import Path
w=next(w for w in unreal.ObjectIterator(unreal.World) if 'UEDPIE' in w.get_path_name() and unreal.GameplayStatics.get_player_controller(w,0))
pc=unreal.GameplayStatics.get_player_controller(w,0);pawn=pc.get_controlled_pawn();comp=pawn.get_component_by_class(unreal.HodgeLocomotionLabComponent);move=pawn.get_component_by_class(unreal.CharacterMovementComponent);mesh=pawn.get_component_by_class(unreal.SkeletalMeshComponent);anim=mesh.get_anim_instance()
assert isinstance(anim,unreal.HodgeALSLocomotion),str(anim)
cases=[(g,f,a) for g in [180,420] for f in [True,False] for a in [0,45,90,135,180,-135,-90,-45]]
results=[];idx=0;elapsed=0;setup=True
err=lambda a,b:abs((a-b+180)%360-180)
def step(dt):
 global idx,elapsed,setup
 w=next(w for w in unreal.ObjectIterator(unreal.World) if "UEDPIE" in w.get_path_name() and unreal.GameplayStatics.get_player_controller(w,0))
 pc=unreal.GameplayStatics.get_player_controller(w,0);pawn=pc.get_controlled_pawn();comp=pawn.get_component_by_class(unreal.HodgeLocomotionLabComponent);move=pawn.get_component_by_class(unreal.CharacterMovementComponent);anim=pawn.get_component_by_class(unreal.SkeletalMeshComponent).get_anim_instance()
 try:
  if idx>=len(cases):
   comp.set_combat_facing(True);comp.set_walking(False);comp.set_keyboard_controls_enabled(True);move.stop_movement_immediately()
   pawn.set_actor_location(unreal.Vector(0,0,100),False,True)
   Path('D:/CodexUE55Lab/als-directions.json').write_text(json.dumps(results,indent=2))
   unreal.unregister_slate_post_tick_callback(als_test_handle);print('ALS_DIRECTIONS_DONE',len(results));return
  speed,combat,angle=cases[idx]
  if setup:
   comp.set_keyboard_controls_enabled(False);comp.set_combat_facing(combat);comp.set_walking(speed==180);move.stop_movement_immediately()
   pawn.set_actor_location(unreal.Vector(0,0,100),False,True);pawn.set_actor_rotation(unreal.Rotator(yaw=30),False);pc.set_control_rotation(unreal.Rotator(yaw=30))
   elapsed=unreal.GameplayStatics.get_time_seconds(w);setup=False
  r=math.radians(angle+30);pawn.add_movement_input(unreal.Vector(math.cos(r),math.sin(r),0),1,False)
  if unreal.GameplayStatics.get_time_seconds(w)-elapsed>=2.0:
   v=pawn.get_velocity();yaw=pawn.get_actor_rotation().yaw
   actual=math.hypot(v.x,v.y)
   weights=[float(anim.get_editor_property(p)) for p in ['lab_forward','lab_backward','lab_left','lab_right']]
   state=str(anim.get_lab_active_state());offset=float(anim.get_editor_property('lab_yaw_offset'))
   local=(angle+30-yaw+180)%360-180
   ok=abs(actual-speed)<15 and max(weights)>0.96 and err(local,round(local/90)*90)<6 and abs(sum(weights)-1)<0.02
   results.append(dict(speed=speed,combat=combat,angle=angle,actual_speed=actual,yaw=yaw,local_angle=local,state=state,weights=weights,offset=offset,passed=ok))
   idx+=1;setup=True
   Path("D:/CodexUE55Lab/als-progress.json").write_text(json.dumps(results,indent=2))
 except Exception as e:
  Path('D:/CodexUE55Lab/als-test-error.txt').write_text(str(e));unreal.unregister_slate_post_tick_callback(als_test_handle);raise
als_test_handle=unreal.register_slate_post_tick_callback(step)
unreal.get_default_object(unreal.load_class(None,'/Script/UnrealEd.EditorPerformanceSettings')).set_editor_property('bThrottleCPUWhenNotForeground',False)
print('ALS TEST START',anim.get_path_name())
