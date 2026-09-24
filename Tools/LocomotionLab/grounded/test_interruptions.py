import unreal,json
from pathlib import Path
class GroundedInterruptionTest:
 def __init__(self):
  self.w=next(w for w in unreal.ObjectIterator(unreal.World) if 'UEDPIE' in w.get_path_name() and unreal.GameplayStatics.get_player_controller(w,0))
  self.p=unreal.GameplayStatics.get_player_controller(self.w,0).get_controlled_pawn();self.c=self.p.get_component_by_class(unreal.HodgeLocomotionLabComponent);self.c.set_keyboard_controls_enabled(False)
  self.p.get_component_by_class(unreal.CharacterMovementComponent).stop_movement_immediately()
  self.a=self.p.get_component_by_class(unreal.SkeletalMeshComponent).get_anim_instance()
  self.start=unreal.GameplayStatics.get_time_seconds(self.w);self.rows=[];self.played=False;self.stopped=False
 def tick(self,dt):
  t=unreal.GameplayStatics.get_time_seconds(self.w)-self.start
  if t>.5 and not self.played:
   self.played=True
   seq=unreal.load_object(None,'/Game/CodexText/Anim/Grounded/Stand1.Stand1')
   assert self.a.play_slot_animation_as_dynamic_montage(seq,'DefaultSlot',.1,.1,1,1)
   self.p.crouch(False)
  if t>1.5 and not self.stopped:self.stopped=True;self.a.montage_stop(.1)
  self.rows.append({'time':t,'montage':self.a.is_any_montage_playing(),'alpha':[self.a.get_editor_property('foot_alpha_l'),self.a.get_editor_property('foot_alpha_r')],'can_crouch':self.p.can_crouch(),'crouched':self.p.get_editor_property('is_crouched')})
  if t>2.5:
   self.p.un_crouch(False);self.c.set_keyboard_controls_enabled(True)
   Path('D:/CodexUE55Lab/grounded-interruptions.json').write_text(json.dumps(self.rows,indent=2));unreal.unregister_slate_post_tick_callback(self.handle)
grounded_interruptions=GroundedInterruptionTest();grounded_interruptions.handle=unreal.register_slate_post_tick_callback(grounded_interruptions.tick)
