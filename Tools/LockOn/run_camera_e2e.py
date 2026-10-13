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
  if p not in peers:p.set_actor_location(unreal.Vector(-3000,-3000,100),False,True)
enemies=sorted(unreal.GameplayStatics.get_all_actors_of_class(server,unreal.HodgeEnemyCharacter),key=lambda a:a.get_name())
assert enemies
origin=unreal.Vector(1000,1000,100)
for p in [source,local]:p.set_actor_location(origin,False,True);p.get_controller().set_control_rotation(unreal.Rotator(0,0,0))
for i,e in enumerate(enemies):
 if e.get_controller() and e.get_controller().brain_component:e.get_controller().brain_component.stop_logic('Soft camera validation')
 e.set_actor_location(origin+unreal.Vector(600,0 if i==0 else 3000,0),False,True)
target=enemies[0]
subsystem_lib=unreal.get_default_object(unreal.load_class(None,'/Script/Engine.SubsystemBlueprintLibrary'))
inputs=subsystem_lib.call_method('GetLocalPlayerSubSystemFromPlayerController',(pc,unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
config=unreal.load_asset('/Game/Main/Input/DA_HodgeInputConfig')
look=next(r.get_editor_property('InputAction') for r in config.get_editor_property('NativeInputActions') if str(r.get_editor_property('InputTag').get_editor_property('TagName'))=='InputTag.Look.Mouse')
checks=[];samples=[]
capture=Path('E:/Project/Git/Hodgepodge/Saved/UIFoundation/UI-Slate.png')
backup=WORK/'UI-Slate-before.png'
if capture.exists() and not backup.exists():backup.write_bytes(capture.read_bytes())
state={'wall':time.monotonic(),'stage':0,'at':unreal.GameplayStatics.get_time_seconds(server),'last_yaw':None,'turn':0.,'key':False,'max_jump':0.}

def key(pressed):
 if state['key']!=pressed:
  state['key']=pressed
  assert unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'LockOnKey',1 if pressed else 0)

def inject(x=0.,y=0.):inputs.inject_input_vector_for_action(look,unreal.Vector(x,y,0),[],[])
def diff(a,b):return (a-b+180)%360-180
def check(name,passed,detail=None):
 checks.append({'check':name,'passed':bool(passed),'detail':detail});assert passed,(name,detail)
def start(stage,now):state.update({'stage':stage,'at':now,'max_jump':0.})
def same():return all(p.get_component_by_class(unreal.HodgeLockOnComponent).get_confirmed_target() and p.get_component_by_class(unreal.HodgeLockOnComponent).get_confirmed_target().get_name()==target.get_name() for p in peers)
def soft_mode():
 c=camera.get_active_camera_mode_class();return c and c==unreal.HodgeCameraMode_LockOn.static_class()

def finish(error=None):
 unreal.unregister_slate_post_tick_callback(state['callback']);inject();key(False)
 unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'CameraOverride',0)
 result={'mode':mode,'passed':not error and all(c['passed'] for c in checks),'checks':checks,'samples':samples,'error':error}
 (WORK/(mode+'-camera-e2e.json')).write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
 if capture.exists() and capture.stat().st_mtime>=state.get('capture_at',float('inf')):
  (WORK/(mode+'-soft-camera.png')).write_bytes(capture.read_bytes())
 print(json.dumps({'mode':mode,'passed':result['passed'],'checks':len(checks),'error':error},ensure_ascii=False))
 for n in ['worlds','clients','peers','source','local','server','pc','camera','hero','lock','target','enemies','inputs','w','p','e']:globals()[n]=None
 gc.collect()

def tick(delta):
 try:
  assert time.monotonic()-state['wall']<100,'软镜头测试超时'
  now=unreal.GameplayStatics.get_time_seconds(server);age=now-state['at'];stage=state['stage']
  rotation=pc.player_camera_manager.get_camera_rotation();yaw=rotation.yaw
  active=camera.get_active_camera_mode_class();desired=hero.get_desired_camera_mode()
  if state['last_yaw'] is not None:state['max_jump']=max(state['max_jump'],abs(diff(yaw,state['last_yaw'])))
  state['last_yaw']=yaw
  samples.append({'time':now,'stage':stage,'camera_yaw':yaw,'camera_pitch':rotation.pitch,'control_yaw':pc.get_control_rotation().yaw,
                  'actor_yaw':local.get_actor_rotation().yaw,'active_mode':active.get_path_name() if active else None,
                  'desired_mode':desired.get_path_name() if desired else None,'pending':lock.is_command_pending(),
                  'target':lock.get_confirmed_target().get_name() if lock.get_confirmed_target() else None})
  if stage==0:
   if age<1.:return
   check('CAM-01_正式Hero使用项目CameraMode组件',bool(camera and active))
   check('CAM-01_自由状态选择PawnData默认模式',active==unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData').get_editor_property('DefaultCameraMode'))
   state['before_acquire']=yaw;key(True);start(1,now)
  elif stage==1:
   key(False)
   if soft_mode() and not state.get('acquire_look_sent'):
    state['acquire_look_sent']=now;state['acquire_look_yaw']=yaw;inject(6);return
   if state.get('acquire_look_sent') and not state.get('acquire_look_checked') and now>state['acquire_look_sent']:
    check('CAM-08_获取混合期间不削弱本帧鼠标位移',abs(diff(yaw,state['acquire_look_yaw'])-6.)<.2,diff(yaw,state['acquire_look_yaw']))
    state['acquire_look_checked']=True
   if not same() or age<.45:
    if age>5:raise AssertionError('中键未确认目标')
    return
   check('CAM-02_锁定选择进入真实CameraMode栈',soft_mode())
   check('CAM-02_获取不瞬间跳到角色朝向',state['max_jump']<8.,state['max_jump'])
   state['input_yaw']=yaw;inject(12);start(2,now)
  elif stage==2:
   if age<.2:return
   check('CAM-03_锁定后鼠标能越过旧八度限制',abs(diff(yaw,state['input_yaw']))>9.,diff(yaw,state['input_yaw']))
   check('CAM-03_小幅输入不切换目标',same())
   state['hold_yaw']=yaw;start(3,now)
  elif stage==3:
   if age<.25:return
   check('CAM-03_停手宽限内不强拉回',abs(diff(yaw,state['hold_yaw']))<.5,diff(yaw,state['hold_yaw']))
   state['orbit_last']=yaw;state['turn']=0.;start(4,now)
  elif stage==4:
   state['turn']+=diff(yaw,state['orbit_last']);state['orbit_last']=yaw
   if age<4.:
    inject(80.*delta);return
   inject()
   check('CAM-03_自由环绕累计转角超过240度',state['turn']>240.,state['turn'])
   check('CAM-03_慢速环绕不误触甩动切换',same())
   check('CAM-04_角色朝向不随镜头环绕抢写',abs(diff(local.get_actor_rotation().yaw,0.))<8.,local.get_actor_rotation().yaw)
   # 结束环绕后主动看向更偏离目标的位置，确保接下来确实覆盖自动构图而非死区内静止。
   inject(90);start(5,now)
  elif stage==5:
   if not state.get('orbit_hold_set'):
    state['orbit_hold_set']=True;state['hold_yaw']=yaw;start(5,now);return
   if age<.3:return
   check('CAM-03_环绕后停手仍保留手动视角',abs(diff(yaw,state['hold_yaw']))<.5,diff(yaw,state['hold_yaw']))
   state['assist_error']=abs(diff(yaw,0.));start(6,now)
  elif stage==6:
   if age<1.7:return
   check('CAM-05_停手后柔和恢复构图',abs(diff(yaw,0.))<state['assist_error']-.5,(state['assist_error'],abs(diff(yaw,0.))))
   check('CAM-05_自动修正没有大角度跳变',state['max_jump']<8.,state['max_jump'])
   state['capture_at']=time.time();assert unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'Capture',0)
   assert unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'CameraOverride',1);start(7,now)
  elif stage==7:
   if not soft_mode() and not state.get('override_look_sent'):
    state['override_look_sent']=now;state['override_look_start']=yaw;inject(6);return
   if state.get('override_look_sent') and not state.get('override_look_checked') and now>state['override_look_sent']:
    check('CAM-08_技能混合期间不削弱本帧鼠标位移',abs(diff(yaw,state['override_look_start'])-6.)<.2,diff(yaw,state['override_look_start']))
    state['override_look_checked']=True
   if age<.7:return
   check('CAM-06_临时技能镜头实际成为栈顶',not soft_mode() and active==desired)
   state['override_yaw']=yaw;inject(25);start(8,now)
  elif stage==8:
   if age<.2:return
   check('CAM-06_临时模式能接管鼠标输入',abs(diff(yaw,state['override_yaw']))>18.,diff(yaw,state['override_yaw']))
   check('CAM-06_临时镜头期间锁定目标保留',same())
   state['before_resume']=yaw;assert unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'CameraOverride',0);start(9,now)
  elif stage==9:
   if age<.4:return
   check('CAM-06_结束覆盖返回锁定CameraMode',soft_mode())
   check('CAM-06_返回时继承实际取景',abs(diff(yaw,state['before_resume']))<2.,diff(yaw,state['before_resume']))
   check('CAM-06_返回混合没有突然跳变',state['max_jump']<8.,state['max_jump'])
   state['before_clear']=yaw;key(True);start(10,now)
  elif stage==10:
   key(False)
   if age<.7:return
   check('CAM-07_解除恢复默认模式',not soft_mode() and not lock.get_presentation_target())
   check('CAM-07_解除没有跳回敌人方位',abs(diff(yaw,state['before_clear']))<2.,diff(yaw,state['before_clear']))
   state['free_yaw']=yaw;inject(10);start(11,now)
  elif stage==11:
   if age<.2:return
   check('CAM-07_自由视角鼠标仍能工作',abs(diff(yaw,state['free_yaw']))>8.,diff(yaw,state['free_yaw']))
   finish()
 except Exception:finish(traceback.format_exc())
state['callback']=unreal.register_slate_post_tick_callback(tick)
print('开始实际软镜头与模式切换测试：'+mode)
