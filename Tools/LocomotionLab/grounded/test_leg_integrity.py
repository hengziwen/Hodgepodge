import unreal,json,math,traceback
from pathlib import Path
class LegIntegrityTest:
 def __init__(self):
  self.w=next(w for w in unreal.ObjectIterator(unreal.World) if 'UEDPIE' in w.get_path_name() and unreal.GameplayStatics.get_player_controller(w,0))
  self.pc=unreal.GameplayStatics.get_player_controller(self.w,0);self.p=self.pc.get_controlled_pawn();self.mesh=self.p.get_component_by_class(unreal.SkeletalMeshComponent);self.a=self.mesh.get_anim_instance();self.c=self.p.get_component_by_class(unreal.HodgeLocomotionLabComponent);self.m=self.p.get_component_by_class(unreal.CharacterMovementComponent)
  self.c.set_keyboard_controls_enabled(False);self.c.set_combat_facing(True)
  self.cases=[('move',walk,angle,(0,0,110)) for walk in [True,False] for angle in [0,45,90,135,180,-135,-90,-45]]+[('slope',True,angle,(2000,0,230)) for angle in [0,90,180,270]]+[('step',True,0,(2700,0,240))]
  self.rows=[];self.i=-1;self.next()
 def next(self):
  self.i+=1;self.kind,walk,self.angle,pos=self.cases[self.i];self.c.set_walking(walk);self.m.stop_movement_immediately();self.p.set_actor_location(unreal.Vector(*pos),False,True);yaw=0 if self.kind=='move' else self.angle;self.p.set_actor_rotation(unreal.Rotator(yaw=yaw),False);self.pc.set_control_rotation(unreal.Rotator(yaw=yaw,pitch=-12));self.start=unreal.GameplayStatics.get_time_seconds(self.w)
 def tick(self,dt):
  try:
   t=unreal.GameplayStatics.get_time_seconds(self.w)-self.start
   if self.kind=='move':
    r=math.radians(self.angle);self.p.add_movement_input(unreal.Vector(math.cos(r),math.sin(r),0),1,False)
   if t>.5:
    row={'case':self.i,'kind':self.kind,'walk':self.cases[self.i][1],'angle':self.angle,'t':t,'speed':self.p.get_velocity().length(),'state':str(self.a.get_grounded_state()),'terrain':self.a.get_editor_property('terrain_alpha'),'legs':{}}
    for s in ['L','R']:
     pts=[self.mesh.get_socket_transform('Bip001'+s+b,unreal.RelativeTransformSpace.RTS_COMPONENT).translation for b in ['Thigh','Calf','Foot']]
     row['legs'][s]={'thigh':(pts[1]-pts[0]).length(),'calf':(pts[2]-pts[1]).length(),'points':[[v.x,v.y,v.z] for v in pts]}
    self.rows.append(row)
   if t>1.8:
    if self.i+1<len(self.cases):self.next()
    else:
     self.m.stop_movement_immediately();self.c.set_keyboard_controls_enabled(True);self.c.set_walking(False);Path('D:/CodexUE55Lab/leg-integrity-after.json').write_text(json.dumps(self.rows));unreal.unregister_slate_post_tick_callback(self.handle);print('LEG_INTEGRITY_DONE',len(self.rows))
  except Exception:
   Path('D:/CodexUE55Lab/leg-integrity-error.txt').write_text(traceback.format_exc());unreal.unregister_slate_post_tick_callback(self.handle);raise
unreal.get_default_object(unreal.load_class(None,'/Script/UnrealEd.EditorPerformanceSettings')).set_editor_property('bThrottleCPUWhenNotForeground',False)
leg_integrity=LegIntegrityTest();leg_integrity.handle=unreal.register_slate_post_tick_callback(leg_integrity.tick)
