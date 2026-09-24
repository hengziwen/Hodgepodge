import unreal,math,json,traceback
from pathlib import Path
class GroundedTest:
 def __init__(self):
  self.w=next(w for w in unreal.ObjectIterator(unreal.World) if 'UEDPIE' in w.get_path_name() and unreal.GameplayStatics.get_player_controller(w,0))
  self.pc=unreal.GameplayStatics.get_player_controller(self.w,0);self.p=self.pc.get_controlled_pawn();assert self.p
  self.c=self.p.get_component_by_class(unreal.HodgeLocomotionLabComponent);self.m=self.p.get_component_by_class(unreal.CharacterMovementComponent);self.mesh=self.p.get_component_by_class(unreal.SkeletalMeshComponent);self.a=self.mesh.get_anim_instance();assert isinstance(self.a,unreal.HodgeGroundedLocomotion)
  self.c.set_keyboard_controls_enabled(False);self.c.set_combat_facing(True)
  self.cases=[('stop',True,.6),('stop',False,.6)]
  self.i=-1;self.rows=[];self.results=[];self.next()
 def next(self):
  self.i+=1
  if self.i>=len(self.cases):return
  self.kind,self.walk,self.arg=self.cases[self.i];self.start=unreal.GameplayStatics.get_time_seconds(self.w)
  self.m.stop_movement_immediately();self.p.set_actor_location(unreal.Vector(0,0,100),False,True);self.p.set_actor_rotation(unreal.Rotator(yaw=0),False);self.pc.set_control_rotation(unreal.Rotator(yaw=0));self.c.set_walking(self.walk);self.event=False
  self.current=[]
 def tick(self,dt):
  try:
   if self.i>=len(self.cases):
    self.c.set_keyboard_controls_enabled(True);self.c.set_walking(False);self.m.stop_movement_immediately()
    Path('D:/CodexUE55Lab/grounded-stop-interrupt.json').write_text(json.dumps({'results':self.results,'frames':self.rows},indent=2))
    unreal.unregister_slate_post_tick_callback(self.handle);print('GROUNDED_TEST_DONE');return
   t=unreal.GameplayStatics.get_time_seconds(self.w)-self.start
   if 1.7<t<2.0:self.p.add_movement_input(unreal.Vector(1,0,0),1,False)
   if self.kind=='stop':
    if .5<t<.5+self.arg:self.p.add_movement_input(unreal.Vector(1,0,0),1,False)
    duration=.5+self.arg+2.7
   else:
    duration=3.5
    if t>.6 and not self.event:
     self.event=True
     if self.kind in ['turn','interrupt']:self.pc.set_control_rotation(unreal.Rotator(yaw=self.arg))
     if self.kind=='fall':self.p.launch_character(unreal.Vector(0,0,450),False,True)
     if self.kind=='teleport':self.p.set_actor_location(unreal.Vector(300,0,100),False,True)
    if self.kind=='interrupt' and 1.1<t<1.65:self.p.add_movement_input(unreal.Vector(0,1,0),1,False)
   def vec(v):return [v.x,v.y,v.z]
   r={'case':self.i,'kind':self.kind,'t':t,'state':str(self.a.get_grounded_state()),'cycle':str(self.a.get_lab_active_state()),'speed':self.p.get_velocity().length(),'yaw':self.p.get_actor_rotation().yaw,'action':self.a.get_editor_property('ground_action_time'),'alpha':[self.a.get_editor_property('foot_alpha_l'),self.a.get_editor_property('foot_alpha_r')],'feet':[vec(self.mesh.get_socket_location(b)) for b in ['Bip001LFoot','Bip001RFoot']],'raw':[vec(self.mesh.get_socket_location(b)) for b in ['VB ik_foot_left','VB ik_foot_right']],'targets':[vec(self.a.get_editor_property(p)) for p in ['foot_target_l','foot_target_r']]}
   self.rows.append(r);self.current.append(r)
   if t>duration:
    states=sorted(set(r['state'] for r in self.current));self.results.append({'case':self.cases[self.i],'states':states,'final_yaw':r['yaw'],'final_alpha':r['alpha']})
    Path('D:/CodexUE55Lab/grounded-stop-interrupt-progress.json').write_text(json.dumps(self.results,indent=2));self.next()
  except Exception:
   Path('D:/CodexUE55Lab/grounded-test-error.txt').write_text(traceback.format_exc());unreal.unregister_slate_post_tick_callback(self.handle);raise
grounded_test=GroundedTest()
grounded_test.handle=unreal.register_slate_post_tick_callback(grounded_test.tick)
unreal.get_default_object(unreal.load_class(None,'/Script/UnrealEd.EditorPerformanceSettings')).set_editor_property('bThrottleCPUWhenNotForeground',False)
print('GROUNDED_TEST_STARTED')