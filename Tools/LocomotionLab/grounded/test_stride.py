import unreal,json,traceback
from pathlib import Path
class StrideTest:
 def __init__(self):
  self.w=next(w for w in unreal.ObjectIterator(unreal.World) if 'UEDPIE' in w.get_path_name() and unreal.GameplayStatics.get_player_controller(w,0));self.pc=unreal.GameplayStatics.get_player_controller(self.w,0);self.p=self.pc.get_controlled_pawn();self.mesh=self.p.get_component_by_class(unreal.SkeletalMeshComponent);self.a=self.mesh.get_anim_instance();self.c=self.p.get_component_by_class(unreal.HodgeLocomotionLabComponent);self.m=self.p.get_component_by_class(unreal.CharacterMovementComponent);self.c.set_keyboard_controls_enabled(False);self.c.set_combat_facing(True);self.c.set_walking(True);self.p.set_actor_location(unreal.Vector(0,0,110),False,True);self.pc.set_control_rotation(unreal.Rotator(yaw=0));self.start=unreal.GameplayStatics.get_time_seconds(self.w);self.rows=[]
 def tick(self,dt):
  try:
   t=unreal.GameplayStatics.get_time_seconds(self.w)-self.start
   self.p.add_movement_input(unreal.Vector(1,0,0) if t<2 else unreal.Vector(.70710678,.70710678,0),.5,False)
   self.rows.append({'t':t,'speed':self.p.get_velocity().length(),'scale':self.a.get_editor_property('stride_scale'),'diagonal':self.a.get_editor_property('diagonal_scale'),'rate':self.a.get_editor_property('lab_play_rate'),'alpha':self.a.get_editor_property('stride_alpha')})
   if t>4:
    self.m.stop_movement_immediately();self.c.set_walking(False);self.c.set_keyboard_controls_enabled(True);Path('D:/CodexUE55Lab/stride-results.json').write_text(json.dumps(self.rows));unreal.unregister_slate_post_tick_callback(self.handle);print('STRIDE_TEST_DONE')
  except Exception:
   Path('D:/CodexUE55Lab/stride-error.txt').write_text(traceback.format_exc());unreal.unregister_slate_post_tick_callback(self.handle);raise
unreal.get_default_object(unreal.load_class(None,'/Script/UnrealEd.EditorPerformanceSettings')).set_editor_property('bThrottleCPUWhenNotForeground',False)
stride_test=StrideTest();stride_test.handle=unreal.register_slate_post_tick_callback(stride_test.tick)
