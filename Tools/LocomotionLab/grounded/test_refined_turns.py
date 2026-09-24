import unreal,json,traceback
from pathlib import Path
class RefineTest:
 def __init__(self):
  self.w=next(w for w in unreal.ObjectIterator(unreal.World) if 'UEDPIE' in w.get_path_name() and unreal.GameplayStatics.get_player_controller(w,0))
  self.pc=unreal.GameplayStatics.get_player_controller(self.w,0);self.p=self.pc.get_controlled_pawn();self.m=self.p.get_component_by_class(unreal.CharacterMovementComponent);self.mesh=self.p.get_component_by_class(unreal.SkeletalMeshComponent);self.a=self.mesh.get_anim_instance();self.c=self.p.get_component_by_class(unreal.HodgeLocomotionLabComponent);self.c.set_keyboard_controls_enabled(False);self.rows=[];self.i=-1;self.next()
 def next(self):
  self.i+=1;self.start=unreal.GameplayStatics.get_time_seconds(self.w);self.m.stop_movement_immediately();self.p.set_actor_location(unreal.Vector(0,0,100),False,True);self.p.set_actor_rotation(unreal.Rotator(yaw=0),False);self.pc.set_control_rotation(unreal.Rotator(yaw=0));self.c.set_combat_facing(True)
 def tick(self,dt):
  try:
   t=unreal.GameplayStatics.get_time_seconds(self.w)-self.start
   yaw=180 if self.i==0 else (110 if t<1.1 else -90)
   if t>.6:self.pc.set_control_rotation(unreal.Rotator(yaw=yaw))
   self.rows.append({'case':self.i,'t':t,'state':str(self.a.get_grounded_state()),'yaw':self.p.get_actor_rotation().yaw,'rate':self.a.get_editor_property('ground_action_play_rate'),'alpha':[self.a.get_editor_property('foot_alpha_l'),self.a.get_editor_property('foot_alpha_r')]})
   if t>5:
    if self.i==0:self.next()
    else:
     self.c.set_keyboard_controls_enabled(True);Path('D:/CodexUE55Lab/refine-turns.json').write_text(json.dumps(self.rows));unreal.unregister_slate_post_tick_callback(self.handle);print('REFINE_TURNS_DONE')
  except Exception:
   Path('D:/CodexUE55Lab/refine-turns-error.txt').write_text(traceback.format_exc());unreal.unregister_slate_post_tick_callback(self.handle);raise
refine_test=RefineTest();refine_test.handle=unreal.register_slate_post_tick_callback(refine_test.tick)
