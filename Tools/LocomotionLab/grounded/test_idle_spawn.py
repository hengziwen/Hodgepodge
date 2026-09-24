import unreal,json,math,traceback
from pathlib import Path
class IdleSpawnTest:
 def __init__(self):self.rows=[];self.start=None
 def tick(self,dt):
  try:
   worlds=[w for w in unreal.ObjectIterator(unreal.World) if 'UEDPIE' in w.get_path_name() and unreal.GameplayStatics.get_player_controller(w,0)]
   if not worlds:return
   w=worlds[0];p=unreal.GameplayStatics.get_player_controller(w,0).get_controlled_pawn()
   if not p:return
   m=p.get_component_by_class(unreal.SkeletalMeshComponent);a=m.get_anim_instance()
   if not isinstance(a,unreal.HodgeGroundedLocomotion):return
   now=unreal.GameplayStatics.get_time_seconds(w)
   if self.start is None:self.start=now
   pos=lambda name:m.get_socket_transform(name,unreal.RelativeTransformSpace.RTS_COMPONENT).translation
   vec=lambda v:[v.x,v.y,v.z]
   left=pos('Bip001LFoot');right=pos('Bip001RFoot')
   row={'t':now-self.start,'state':str(a.get_grounded_state()),'speed':p.get_velocity().length(),'alpha':[a.get_editor_property('foot_alpha_l'),a.get_editor_property('foot_alpha_r')],'left':vec(left),'right':vec(right),'raw_left':vec(pos('VB ik_foot_left')),'raw_right':vec(pos('VB ik_foot_right')),'lateral_separation_cm':left.x-right.x}
   self.rows.append(row)
   row['terrain_alpha']=a.get_editor_property('terrain_alpha')
   row['toe_clearance']=[]
   for bone in ['Bip001LToe0','Bip001RToe0']:
    foot=m.get_socket_location(bone)
    hit=unreal.SystemLibrary.line_trace_single(p,foot+unreal.Vector(0,0,40),foot-unreal.Vector(0,0,60),unreal.TraceTypeQuery.TRACE_TYPE_QUERY1,False,[p],unreal.DrawDebugTrace.NONE)
    row['toe_clearance'].append(None if hit is None else foot.z-hit.to_tuple()[5].z)
   if row['t']>3:
    samples=[r for r in self.rows if r['t']>.25 and min(r['alpha'])>.99]
    def valid(r):
     planar_ok=math.dist(r['left'][:2],r['raw_left'][:2])<2 and math.dist(r['right'][:2],r['raw_right'][:2])<2
     ground_ok=all(v is not None and -1.5<v<4 for v in r['toe_clearance']) if r['terrain_alpha']>.99 else math.dist(r['left'],r['raw_left'])<2 and math.dist(r['right'],r['raw_right'])<2
     return r['state']=='Idle' and r['lateral_separation_cm']>20 and planar_ok and ground_ok
    result={'passed':bool(samples) and all(valid(r) for r in samples),'min_separation_cm':min((r['lateral_separation_cm'] for r in samples),default=None),'frames':self.rows}
    Path('D:/CodexUE55Lab/idle-spawn-test.json').write_text(json.dumps(result,indent=2));unreal.unregister_slate_post_tick_callback(self.handle);print('IDLE_SPAWN',result['passed'],result['min_separation_cm'])
  except Exception:
   Path('D:/CodexUE55Lab/idle-spawn-error.txt').write_text(traceback.format_exc());unreal.unregister_slate_post_tick_callback(self.handle);raise
idle_spawn_test=IdleSpawnTest();idle_spawn_test.handle=unreal.register_slate_post_tick_callback(idle_spawn_test.tick)
unreal.get_default_object(unreal.load_class(None,'/Script/UnrealEd.EditorPerformanceSettings')).set_editor_property('bThrottleCPUWhenNotForeground',False)
print('Waiting for fresh PIE; no movement or teleport injected')
