import unreal,math,json
from pathlib import Path
world=next(w for w in unreal.ObjectIterator(unreal.World) if 'UEDPIE' in w.get_path_name() and unreal.GameplayStatics.get_player_controller(w,0))
pc=unreal.GameplayStatics.get_player_controller(world,0);pawn=pc.get_controlled_pawn();sub=next(s for s in unreal.ObjectIterator(unreal.EnhancedInputLocalPlayerSubsystem) if 'HodgeLocalPlayer' in s.get_path_name())
act=unreal.load_object(None,'/Game/Main/Input/InputAction/IA_Move.IA_Move')
print('SUBSYSTEM',sub,'ACTION',act)
print(unreal.EnhancedInputLocalPlayerSubsystem.inject_input_vector_for_action.__doc__)
comp=pawn.get_component_by_class(unreal.HodgeLocomotionLabComponent);comp.set_combat_facing(True)
angles=[0,45,90,135,180,-135,-90,-45];idx=0;t=0;rows=[]
def check_input(dt):
 global idx,t
 try:
  if idx==len(angles):
   sub.inject_input_vector_for_action(act,unreal.Vector(),[],[])
   Path('D:/CodexUE55Lab/grounded-input-results.json').write_text(json.dumps(rows,indent=2));unreal.unregister_slate_post_tick_callback(input_handle);return
  if t==0:
   pawn.set_actor_location(unreal.Vector(0,0,100),False,True)
   pc.set_control_rotation(unreal.Rotator(yaw=90));pawn.set_actor_rotation(unreal.Rotator(yaw=90),False)
  rad=math.radians(angles[idx]);sub.inject_input_vector_for_action(act,unreal.Vector(math.sin(rad),math.cos(rad),0),[],[])
  t+=dt
  if t>1.5:
   vel=pawn.get_velocity();speed=math.hypot(vel.x,vel.y);direction=math.degrees(math.atan2(vel.y,vel.x));err=abs((direction-angles[idx]-90+180)%360-180)
   rows.append(dict(direction=angles[idx],speed=speed,world_angle=direction,passed=speed>390 and err<8));idx+=1;t=0
 except Exception as e:
  Path('D:/CodexUE55Lab/als-input-error.txt').write_text(str(e));unreal.unregister_slate_post_tick_callback(input_handle);raise
input_handle=unreal.register_slate_post_tick_callback(check_input)


