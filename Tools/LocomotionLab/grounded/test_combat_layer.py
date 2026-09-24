import unreal,json,math,traceback
from pathlib import Path
class CombatTest:
 def __init__(self):
  self.w=next(w for w in unreal.ObjectIterator(unreal.World) if 'UEDPIE' in w.get_path_name() and unreal.GameplayStatics.get_player_controller(w,0));self.pc=unreal.GameplayStatics.get_player_controller(self.w,0);self.p=self.pc.get_controlled_pawn();self.mesh=self.p.get_component_by_class(unreal.SkeletalMeshComponent);self.a=self.mesh.get_anim_instance();self.c=self.p.get_component_by_class(unreal.HodgeLocomotionLabComponent);self.m=self.p.get_component_by_class(unreal.CharacterMovementComponent);self.c.set_keyboard_controls_enabled(False);self.rows=[];self.i=-1;self.next()
 def next(self):
  self.i+=1;self.start=unreal.GameplayStatics.get_time_seconds(self.w);self.event=False;self.m.stop_movement_immediately();self.p.set_actor_location(unreal.Vector(0,0,110),False,True);self.p.set_actor_rotation(unreal.Rotator(yaw=0),False);self.pc.set_control_rotation(unreal.Rotator(yaw=0));self.a.set_overlay_enabled(self.i in [1,2]);self.a.set_left_hand_grip(self.mesh if self.i in [3,4] else None,'Bip001RForearm',1)
 def tick(self,dt):
  try:
   t=unreal.GameplayStatics.get_time_seconds(self.w)-self.start
   if self.i==2 and t>.6:self.p.add_movement_input(unreal.Vector(1,0,0),1,False)
   if self.i==4 and t>.6 and not self.event:
    self.event=True;self.a.play_slot_animation_as_dynamic_montage(unreal.load_asset('/Game/CodexText/Anim/Attack01'),'DefaultSlot',.1,.1,1)
   loc=lambda b:self.mesh.get_socket_transform(b,unreal.RelativeTransformSpace.RTS_COMPONENT).translation
   lh=loc('Bip001LHand');rh=loc('Bip001RForearm')
   self.rows.append({'case':self.i,'t':t,'overlay':self.a.get_editor_property('overlay_alpha'),'hand_alpha':self.a.get_editor_property('hand_ik_alpha'),'grip_error':(lh-rh).length(),'hand':[lh.x,lh.y,lh.z],'state':str(self.a.get_grounded_state()),'montage':self.a.is_any_montage_playing(),'foot_alpha':[self.a.get_editor_property('foot_alpha_l'),self.a.get_editor_property('foot_alpha_r')]})
   if t>3:
    if self.i<4:self.next()
    else:
     self.a.set_overlay_enabled(False);self.a.set_left_hand_grip(None,'None',0);self.c.set_keyboard_controls_enabled(True);self.m.stop_movement_immediately();Path('D:/CodexUE55Lab/combat-layer-results.json').write_text(json.dumps(self.rows,indent=2));unreal.unregister_slate_post_tick_callback(self.handle);print('COMBAT_TEST_DONE')
  except Exception:
   Path('D:/CodexUE55Lab/combat-layer-error.txt').write_text(traceback.format_exc());unreal.unregister_slate_post_tick_callback(self.handle);raise
unreal.get_default_object(unreal.load_class(None,'/Script/UnrealEd.EditorPerformanceSettings')).set_editor_property('bThrottleCPUWhenNotForeground',False)
combat_test=CombatTest();combat_test.handle=unreal.register_slate_post_tick_callback(combat_test.tick)
