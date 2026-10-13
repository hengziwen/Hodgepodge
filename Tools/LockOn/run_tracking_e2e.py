"""记录真实 CameraMode 栈及输出视角，验证软镜头和临时模式；只操作 PIE 副本。"""
import unreal,json,time,traceback,math,gc
from pathlib import Path
WORK=Path(globals().get('LOCK_ON_WORK','E:/Project/Git/Hodgepodge/Saved/LockOnCamera'))
mode=globals().get('LOCK_ON_MODE','single')
worlds=unreal.EditorLevelLibrary.get_pie_worlds(True)
server=next(w for w in worlds if unreal.SystemLibrary.is_server(w))
clients=[w for w in worlds if w!=server]
if mode=='listen-lag':
 for w in worlds:unreal.SystemLibrary.execute_console_command(w,'Net PktLag=100')
local=next(a for a in unreal.GameplayStatics.get_all_actors_of_class(clients[0] if clients else server,unreal.HodgeHeroCharacter) if not clients or a.get_local_role()==unreal.NetRole.ROLE_AUTONOMOUS_PROXY)
pc=local.get_controller();camera=local.get_component_by_class(unreal.HodgeCameraComponent)
hero=local.get_component_by_class(unreal.HodgeHeroComponent);lock=local.get_component_by_class(unreal.HodgeLockOnComponent)
pid=local.get_editor_property('PlayerState').get_editor_property('PlayerId')
peers=[a for w in worlds for a in unreal.GameplayStatics.get_all_actors_of_class(w,unreal.HodgeHeroCharacter) if a.get_editor_property('PlayerState').get_editor_property('PlayerId')==pid]
source=next(p for p in peers if p.get_world()==server)
for w in worlds:
 for p in unreal.GameplayStatics.get_all_actors_of_class(w,unreal.HodgeHeroCharacter):
  if p not in peers:p.get_component_by_class(unreal.CharacterMovementComponent).disable_movement()
enemies=sorted(unreal.GameplayStatics.get_all_actors_of_class(server,unreal.HodgeEnemyCharacter),key=lambda a:a.get_name())
assert enemies
origin=unreal.Vector(1000,1000,100)
for p in [source,local]:p.set_actor_location(origin,False,True);p.get_controller().set_control_rotation(unreal.Rotator(0,0,0))
for i,e in enumerate(enemies):
 if e.get_controller() and e.get_controller().brain_component:e.get_controller().brain_component.stop_logic('Soft camera validation')
 e.get_component_by_class(unreal.CharacterMovementComponent).disable_movement()
 e.set_actor_location(origin+unreal.Vector(600,400 if i==0 else 3000,0),False,True)
target=enemies[0]
subsystem_lib=unreal.get_default_object(unreal.load_class(None,'/Script/Engine.SubsystemBlueprintLibrary'))
inputs=subsystem_lib.call_method('GetLocalPlayerSubSystemFromPlayerController',(pc,unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
config=unreal.load_asset('/Game/Main/Input/DA_HodgeInputConfig')
look=next(r.get_editor_property('InputAction') for r in config.get_editor_property('NativeInputActions') if str(r.get_editor_property('InputTag').get_editor_property('TagName'))=='InputTag.Look.Mouse')

# 本脚本只改变 PIE 副本，控轨迹测试与实际 Enhanced Input 测试分别记录。
for p in peers:p.get_component_by_class(unreal.CharacterMovementComponent).disable_movement()
local.set_actor_rotation(unreal.Rotator(0,180,0),False)
profile=lock.get_profile()
checks=[];samples=[]
state={'wall':time.monotonic(),'stage':0,'at':unreal.GameplayStatics.get_time_seconds(server),'key':False,'last_yaw':None,'last_time':None,'max_speed':0.,'orbit_turn':0.,'zone_max':0.}
move=unreal.load_asset('/Game/Main/Input/InputAction/IA_Move')
dash=unreal.load_asset('/Game/Main/Input/InputAction/IA_Sprint')
replica=next(a for a in unreal.GameplayStatics.get_all_actors_of_class(local.get_world(),unreal.HodgeEnemyCharacter) if a.get_name()==target.get_name())
for a in [target,replica]:a.get_component_by_class(unreal.CharacterMovementComponent).disable_movement()

def diff(a,b):return (a-b+180)%360-180

def key(value):
 if state['key']!=value:
  state['key']=value
  assert unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'LockOnKey',1 if value else 0)

def inject(action,value=unreal.Vector()):inputs.inject_input_vector_for_action(action,value,[],[])

def start(stage,now):state.update({'stage':stage,'at':now,'max_speed':0.,'zone_max':0.})

def check(name,ok,detail=None):
 checks.append({'check':name,'passed':bool(ok),'detail':detail});assert ok,(name,detail)

def aim_point():
 mesh=replica.get_component_by_class(unreal.SkeletalMeshComponent)
 socket=replica.get_editor_property('LockOnSocket')
 return mesh.get_socket_location(socket) if mesh and str(socket)!='None' and mesh.does_socket_exist(socket) else replica.get_actor_location()+unreal.Vector(0,0,50)

def screen_error():
 screen=unreal.GameplayStatics.project_world_to_screen(pc,aim_point(),True)
 # UE Python 为仅含输出参数的 bool 函数返回 Vector2D 或 None。
 if isinstance(screen,tuple):screen=screen[0] if isinstance(screen[0],unreal.Vector2D) else screen[1]
 size=pc.get_viewport_size()
 return [(screen.x/size[0]-.5)*2,(screen.y/size[1]-.5)*2] if screen and size[0] and size[1] else [10.,10.]

def same():return all(p.get_component_by_class(unreal.HodgeLockOnComponent).get_confirmed_target() and p.get_component_by_class(unreal.HodgeLockOnComponent).get_confirmed_target().get_name()==target.get_name() for p in peers)

def soft():return camera.get_active_camera_mode_class()==unreal.HodgeCameraMode_LockOn.static_class()

def place_player(pos):
 for p in [source,local]:p.set_actor_location(pos,False,True)

def place_target(pos):
 for a in [target,replica]:a.set_actor_location(pos,False,True)

def finish(error=None):
 unreal.unregister_slate_post_tick_callback(state['callback']);inject(look);inject(move);inject(dash);key(False)
 unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'CameraOverride',0)
 result={'mode':mode,'passed':not error and all(c['passed'] for c in checks),'checks':checks,'samples':samples,'error':error,'trajectory_note':'环绕与目标横移使用受控 PIE 轨迹；移动与 Dash/Sprint 另用 Enhanced Input。'}
 (WORK/(mode+'-tracking-e2e.json')).write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
 print(json.dumps({'mode':mode,'passed':result['passed'],'checks':len(checks),'error':error},ensure_ascii=False))
 for n in ['worlds','clients','peers','source','local','server','pc','camera','hero','lock','target','enemies','inputs','replica','w','p','e','a','profile']:globals()[n]=None
 gc.collect()

def tick(delta):
 try:
  assert time.monotonic()-state['wall']<120,'镜头测试超时'
  now=unreal.GameplayStatics.get_time_seconds(server);age=now-state['at'];stage=state['stage']
  rot=pc.player_camera_manager.get_camera_rotation();yaw=rot.yaw
  if state['last_time'] is not None and now>state['last_time']:
   state['max_speed']=max(state['max_speed'],abs(diff(yaw,state['last_yaw']))/(now-state['last_time']))
  state['last_yaw']=yaw;state['last_time']=now
  error=screen_error() if lock.get_presentation_target() else [0.,0.]
  samples.append({'time':now,'stage':stage,'yaw':yaw,'pitch':rot.pitch,'screen_error':error,'control_yaw':pc.get_control_rotation().yaw,'actor_yaw':local.get_actor_rotation().yaw,'active_mode':camera.get_active_camera_mode_class().get_path_name(),'target':lock.get_presentation_target().get_name() if lock.get_presentation_target() else None})
  if stage==0:
   if age<1.:return
   for p in [source,local]:p.set_actor_rotation(unreal.Rotator(0,180,0),False)
   check('SOFT-01_角色背向目标使用中键获取',abs(diff(local.get_actor_rotation().yaw,0.))>100.)
   state['acquire_start']=yaw;key(True);start(1,now)
  elif stage==1:
   key(False)
   if age<.7:return
   check('SOFT-01_镜头通过现有CameraMode获取目标',same() and soft())
   check('SOFT-01_获取转向明显且没有Snap',abs(diff(yaw,state['acquire_start']))>15. and state['max_speed']<800.,state['max_speed'])
   check('SOFT-01_获取结束目标位于中心附近',abs(error[0])<.12 and abs(error[1])<.18,error)
   state['orbit_last']=yaw;state['orbit_turn']=0.;state['orbit_z']=local.get_actor_location().z;start(2,now)
  elif stage==2:
   state['orbit_turn']+=diff(yaw,state['orbit_last']);state['orbit_last']=yaw
   if age<12.:
    angle=math.atan2(-400.,-600.)+age*2*math.pi/12.
    center=target.get_actor_location();place_player(unreal.Vector(center.x+720*math.cos(angle),center.y+720*math.sin(angle),state['orbit_z']))
    if age>1.:state['zone_max']=max(state['zone_max'],abs(error[0]))
    return
   check('SOFT-02_整圈环绕无需鼠标跟随',abs(state['orbit_turn'])>300.,state['orbit_turn'])
   check('SOFT-02_目标保持软区附近',state['zone_max']<.7,state['zone_max'])
   check('SOFT-02_环绕没有剧烈自动旋转',state['max_speed']<160.,state['max_speed'])
   place_player(origin);place_target(origin+unreal.Vector(600,0,0));start(3,now)
  elif stage==3:
   if age<1.5:return
   state['lateral_start']=yaw;start(4,now)
  elif stage==4:
   if age<3.:
    place_target(origin+unreal.Vector(600,age*120,0));return
   check('SOFT-03_目标横向移动镜头有辅助',abs(diff(yaw,state['lateral_start']))>5.,diff(yaw,state['lateral_start']))
   check('SOFT-03_横移目标维持合理区域',abs(error[0])<.6,error)
   state['mouse_start']=yaw;state['mouse_turn']=0.;state['mouse_last']=yaw;start(5,now)
  elif stage==5:
   state['mouse_turn']+=diff(yaw,state['mouse_last']);state['mouse_last']=yaw
   if age<2.:
    inject(look,unreal.Vector(60*delta,0,0));return
   inject(look)
   check('SOFT-04_鼠标优先可连续环绕',state['mouse_turn']>95.,state['mouse_turn'])
   check('SOFT-04_普通慢速鼠标不切换目标',same())
   state['hold_yaw']=yaw;start(6,now)
  elif stage==6:
   if age<.12:return
   check('SOFT-04_停手宽限无拉扯',abs(diff(yaw,state['hold_yaw']))<1.,diff(yaw,state['hold_yaw']))
   state['recovery_yaw']=yaw;start(7,now)
  elif stage==7:
   if age<2.:return
   check('SOFT-04_停手恢复柔和跟随',abs(diff(yaw,state['recovery_yaw']))>20. and abs(error[0])<.65,(diff(yaw,state['recovery_yaw']),error))
   check('SOFT-04_恢复角速度受限制',state['max_speed']<150.,state['max_speed'])
   assert unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'CameraOverride',1);start(8,now)
  elif stage==8:
   if age<.5:return
   check('SOFT-06_技能CameraMode正常接管',not soft() and same())
   state['override_yaw']=yaw;inject(look,unreal.Vector(12,0,0));start(9,now)
  elif stage==9:
   if age<.2:return
   check('SOFT-06_技能镜头输入正常',abs(diff(yaw,state['override_yaw'])-12.)<.4,diff(yaw,state['override_yaw']))
   state['resume_yaw']=yaw;assert unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'CameraOverride',0);start(10,now)
  elif stage==10:
   if age<.15:return
   check('SOFT-06_返回软镜头连续且不重复获取',soft() and abs(diff(yaw,state['resume_yaw']))<1.,diff(yaw,state['resume_yaw']))
   state['clear_yaw']=yaw;key(True);start(11,now)
  elif stage==11:
   key(False)
   if age<.5:return
   check('SOFT-05_取消恢复默认镜头且不跳变',not soft() and not lock.get_presentation_target() and abs(diff(yaw,state['clear_yaw']))<1.5,diff(yaw,state['clear_yaw']))
   state['free_yaw']=yaw;inject(look,unreal.Vector(10,0,0));start(12,now)
  elif stage==12:
   if age<.15:return
   check('SOFT-05_自由镜头输入不受影响',abs(diff(yaw,state['free_yaw'])-10.)<.4)
   # 再次使用中键获取，并以真实移动输入检查动态跟随。
   place_player(origin);place_target(origin+unreal.Vector(600,300,0));key(True);start(13,now)
  elif stage==13:
   key(False)
   if age<.7:return
   check('SOFT-07_重复锁定仍正常',same())
   for p in peers:p.get_component_by_class(unreal.CharacterMovementComponent).set_movement_mode(unreal.MovementMode.MOVE_WALKING)
   state['move_start']=local.get_actor_location();start(14,now)
  elif stage==14:
   if age<1.:
    inject(move,unreal.Vector(1,0,0));return
   inject(move)
   check('SOFT-07_真实移动期间目标保留且跟随',same() and (local.get_actor_location()-state['move_start']).length()>150. and abs(error[0])<.8,error)
   state['dash_seen']=False;state['sprint_seen']=False;start(15,now)
  elif stage==15:
   session=str(hero.get_sprint_input_session().state)
   state['dash_seen']|='DASH' in session;state['sprint_seen']|='SPRINT' in session
   if age<1.2:
    inject(move,unreal.Vector(0,1,0));inject(dash,unreal.Vector(1,0,0));return
   inject(move);inject(dash)
   check('SOFT-07_Dash与Sprint仍执行并保留软镜头',state['dash_seen'] and state['sprint_seen'] and same() and soft(),[state['dash_seen'],state['sprint_seen'],error])
   finish()
 except Exception:finish(traceback.format_exc())
state['callback']=unreal.register_slate_post_tick_callback(tick)
print('开始获取与屏幕区域跟随测试：'+mode)
