import unreal,json,math,traceback
from pathlib import Path
class TerrainTest:
 def __init__(self):
  self.w=next(w for w in unreal.ObjectIterator(unreal.World) if 'UEDPIE' in w.get_path_name() and unreal.GameplayStatics.get_player_controller(w,0));self.pc=unreal.GameplayStatics.get_player_controller(self.w,0);self.p=self.pc.get_controlled_pawn();self.mesh=self.p.get_component_by_class(unreal.SkeletalMeshComponent);self.a=self.mesh.get_anim_instance();self.c=self.p.get_component_by_class(unreal.HodgeLocomotionLabComponent);self.m=self.p.get_component_by_class(unreal.CharacterMovementComponent);self.c.set_keyboard_controls_enabled(False)
  self.cases=[(n,pos,enabled) for n,pos in [('flat',(0,0,110)),('slope',(2000,0,230)),('step',(2700,0,240))] for enabled in [False,True]];self.i=-1;self.rows=[];self.next()
 def next(self):
  self.i+=1;name,pos,enabled=self.cases[self.i];self.a.set_editor_property('enable_terrain_ik',enabled);self.m.stop_movement_immediately();self.p.set_actor_location(unreal.Vector(*pos),False,True);self.p.set_actor_rotation(unreal.Rotator(yaw=90 if name=='slope' else 0),False);self.pc.set_control_rotation(unreal.Rotator(yaw=90,pitch=-20));self.start=unreal.GameplayStatics.get_time_seconds(self.w)
 def tick(self,dt):
  try:
   t=unreal.GameplayStatics.get_time_seconds(self.w)-self.start
   if t>1.2:
    feet=[]
    for b in ['Bip001LFoot','Bip001RFoot','Bip001LToe0','Bip001RToe0']:
     v=self.mesh.get_socket_location(b);hit=unreal.SystemLibrary.line_trace_single(self.p,v+unreal.Vector(0,0,55),v-unreal.Vector(0,0,100),unreal.TraceTypeQuery.TRACE_TYPE_QUERY1,False,[self.p],unreal.DrawDebugTrace.NONE)
     feet.append({'bone':b,'xyz':[v.x,v.y,v.z],'clearance':None if hit is None else v.z-hit.to_tuple()[5].z})
    self.rows.append({'case':self.cases[self.i][0],'enabled':self.cases[self.i][2],'t':t,'alpha':self.a.get_editor_property('terrain_alpha'),'state':str(self.a.get_grounded_state()),'feet':feet})
   if t>3:
    if self.i+1<len(self.cases):self.next()
    else:
     self.a.set_editor_property('enable_terrain_ik',True);self.c.set_keyboard_controls_enabled(True);self.p.set_actor_location(unreal.Vector(0,0,110),False,True);Path('D:/CodexUE55Lab/terrain-results.json').write_text(json.dumps(self.rows,indent=2));unreal.unregister_slate_post_tick_callback(self.handle);print('TERRAIN_TEST_DONE')
  except Exception:
   Path('D:/CodexUE55Lab/terrain-test-error.txt').write_text(traceback.format_exc());unreal.unregister_slate_post_tick_callback(self.handle);raise
terrain_test=TerrainTest();terrain_test.handle=unreal.register_slate_post_tick_callback(terrain_test.tick)
