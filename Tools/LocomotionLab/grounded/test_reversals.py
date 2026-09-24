import unreal,math,json
from pathlib import Path
class ReversalTest:
 def __init__(self):
  self.t0=None;self.rows=[]
  # Neutral LF -> RB -> RF, RF -> LB -> LF; then both hip biases.
  self.steps=[(-90,0,1.1),(90,0,1.4),(-90,0,1.4),(90,0,1.4),(-90,1,1.5),(90,1,1.5),(90,-1,1.5),(-90,-1,1.5)]
  self.i=0;self.handle=unreal.register_slate_post_tick_callback(self.tick)
 def tick(self,dt):
  try:
   w=next(w for w in unreal.ObjectIterator(unreal.World) if 'UEDPIE' in w.get_path_name() and unreal.GameplayStatics.get_player_controller(w,0))
   pc=unreal.GameplayStatics.get_player_controller(w,0);p=pc.get_controlled_pawn();c=p.get_component_by_class(unreal.HodgeLocomotionLabComponent);m=p.get_component_by_class(unreal.CharacterMovementComponent);a=p.get_component_by_class(unreal.SkeletalMeshComponent).get_anim_instance()
   if self.i==len(self.steps):
    c.set_keyboard_controls_enabled(True);c.set_walking(False);a.set_hip_bias(0);m.stop_movement_immediately()
    Path('D:/CodexUE55Lab/grounded-reversals.json').write_text(json.dumps(self.rows,indent=2));unreal.unregister_slate_post_tick_callback(self.handle);print('REVERSALS DONE');return
   angle,bias,duration=self.steps[self.i];now=unreal.GameplayStatics.get_time_seconds(w)
   if self.t0 is None:
    self.t0=now;c.set_keyboard_controls_enabled(False);c.set_combat_facing(True);c.set_walking(False);pc.set_control_rotation(unreal.Rotator(yaw=0))
    if self.i==0:p.set_actor_location(unreal.Vector(0,0,100),False,True);m.stop_movement_immediately()
    a.set_hip_bias(bias)
   r=math.radians(angle);p.add_movement_input(unreal.Vector(math.cos(r),math.sin(r),0),1,False)
   self.rows.append(dict(step=self.i,time=now-self.t0,angle=angle,bias=bias,state=str(a.get_lab_active_state()),direction=str(a.get_editor_property('lab_direction')),cross=a.get_editor_property('lab_feet_crossing'),speed=a.get_editor_property('lab_speed')))
   if now-self.t0>duration:self.i+=1;self.t0=None
  except Exception as e:
   Path('D:/CodexUE55Lab/als-reversal-error.txt').write_text(str(e));unreal.unregister_slate_post_tick_callback(self.handle);raise
reversal_test=ReversalTest()
print('REVERSAL TEST START')
