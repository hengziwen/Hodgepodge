"""真实 Enhanced Input 下验证锁定、切换、Dash/Sprint 和多端确认；只操作 PIE 副本。"""
import unreal,json,time,traceback
from pathlib import Path
WORK=Path(globals().get('LOCK_ON_WORK','E:/Project/Git/Hodgepodge/Saved/LockOn'));mode=globals().get('LOCK_ON_MODE',globals().get('WING_MODE','single'))
worlds=unreal.EditorLevelLibrary.get_pie_worlds(True);server=next(w for w in worlds if unreal.SystemLibrary.is_server(w))
clients=[w for w in worlds if w!=server]
if mode=='listen-lag':
 for w in worlds:unreal.SystemLibrary.execute_console_command(w,'Net PktLag=100')
local=next(a for a in unreal.GameplayStatics.get_all_actors_of_class(clients[0] if clients else server,unreal.HodgeHeroCharacter) if not clients or a.get_local_role()==unreal.NetRole.ROLE_AUTONOMOUS_PROXY)
pid=local.get_editor_property('PlayerState').get_editor_property('PlayerId')
peers=[a for w in worlds for a in unreal.GameplayStatics.get_all_actors_of_class(w,unreal.HodgeHeroCharacter) if a.get_editor_property('PlayerState').get_editor_property('PlayerId')==pid]
source=next(p for p in peers if p.get_world()==server)
for w in worlds:
 for hero in unreal.GameplayStatics.get_all_actors_of_class(w,unreal.HodgeHeroCharacter):
  if hero not in peers:hero.set_actor_location(unreal.Vector(-3000,-3000,100),False,True)
# 只使用正式怪物的 PIE 实例，禁用其决策以隔离锁定功能。
enemies=sorted(unreal.GameplayStatics.get_all_actors_of_class(server,unreal.HodgeEnemyCharacter),key=lambda a:a.get_name())
assert len(enemies)>=2
origin=unreal.Vector(1000,1000,100)
for p in [source,local]:p.set_actor_location(origin,False,True);p.get_controller().set_control_rotation(unreal.Rotator(0,0,0))
for i,enemy in enumerate(enemies):
 c=enemy.get_controller()
 if c and c.brain_component:c.brain_component.stop_logic('LockOn validation isolation')
 enemy.set_actor_location(origin+unreal.Vector(600,0 if i==0 else 300 if i==1 else 2200,0),False,True)
 target=enemies[0]
right=enemies[1]
lib=unreal.get_default_object(unreal.load_class(None,'/Script/Engine.SubsystemBlueprintLibrary'))
inputs=lib.call_method('GetLocalPlayerSubSystemFromPlayerController',(local.get_controller(),unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
lock_action=unreal.load_asset('/Game/Main/Input/InputAction/IA_LockOn')
look=next(r.get_editor_property('InputAction') for r in unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData').get_editor_property('InputConfig').get_editor_property('NativeInputActions') if str(r.get_editor_property('InputTag').get_editor_property('TagName'))=='InputTag.Look.Mouse')
move=unreal.load_asset('/Game/Main/Input/InputAction/IA_Move');dash=unreal.load_asset('/Game/Main/Input/InputAction/IA_Sprint')
locks=[p.get_component_by_class(unreal.HodgeLockOnComponent) for p in peers]
capture=Path('E:/Project/Git/Hodgepodge/Saved/UIFoundation/UI-Slate.png');capture.parent.mkdir(parents=True,exist_ok=True)
backup=WORK/'UI-Slate-before.png'
if capture.exists() and not backup.exists():backup.write_bytes(capture.read_bytes())
checks=[];samples=[];state={'wall':time.monotonic(),'stage':-1,'at':0.,'dash_seen':False,'sprint_seen':False,'fixture_at':unreal.GameplayStatics.get_time_seconds(server)}
def inject(action,x=0.,y=0.):
 if action==lock_action:
  pressed=bool(x)
  if state.get('key_down',False)!=pressed:
   state['key_down']=pressed
   assert unreal.HodgeUIAuthoringLibrary.queue_ui_action(local.get_controller(),'LockOnKey',1. if pressed else 0.)
  return
 inputs.inject_input_vector_for_action(action,unreal.Vector(x,y,0),[],[])
def check(name,passed,detail=None):
 checks.append({'check':name,'passed':bool(passed),'detail':detail});assert passed,(name,detail)
def start(stage,now):state.update({'stage':stage,'at':now})
def matches(actor):return all(l.get_confirmed_target() and l.get_confirmed_target().get_name()==actor.get_name() for l in locks)
def finish(error=None):
 unreal.unregister_slate_post_tick_callback(state['callback'])
 for action in [lock_action,move,dash,look]:inject(action)
 result={'mode':mode,'passed':not error and all(c['passed'] for c in checks),'checks':checks,'samples':samples,'error':error}
 (WORK/(mode+'-e2e.json')).write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
 print(json.dumps({'mode':mode,'passed':result['passed'],'checks':len(checks),'error':error},ensure_ascii=False))
 # 已保存的逐帧结果只包含数值，释放测试闭包的 UObject 引用，避免后续切图被 Python 保活。
 for name in ['worlds','clients','peers','locks','enemies','source','local','server','inputs','target','right','w','p','hero','enemy','c','markers']:globals()[name]=None
 import gc;gc.collect()
def tick(delta):
 try:
  assert time.monotonic()-state['wall']<150,'锁定端到端测试超时'
  now=unreal.GameplayStatics.get_time_seconds(server);age=now-state['at'];stage=state['stage']
  if state.get('capture_at') and capture.exists() and capture.stat().st_mtime>=state['capture_at']:
   (WORK/(mode+'-hud.png')).write_bytes(capture.read_bytes());state['capture_at']=None
  rows=[{'role':str(p.get_local_role()),'ready':l.is_ready(),'confirmed':l.get_confirmed_target().get_name() if l.get_confirmed_target() else None,'presentation':l.get_presentation_target().get_name() if l.get_presentation_target() else None,'version':l.get_confirmed_state().state_version,'pending':l.is_command_pending(),'style':str(p.get_component_by_class(unreal.HodgeCharacterRotationComponent).get_resolved_state().style),'yaw':p.get_actor_rotation().yaw,'position':[p.get_actor_location().x,p.get_actor_location().y,p.get_actor_location().z],'speed':p.get_velocity().length(),'session':str(p.get_component_by_class(unreal.HodgeHeroComponent).get_sprint_input_session().state)} for p,l in zip(peers,locks)]
  samples.append({'time':now,'stage':stage,'peers':rows})
  if stage==-1:
   # 等待服务器摆放的验证怪物复制到客户端，不能在未知候选上发起获取。
   if now-state['fixture_at']<.8 or not all(l.is_ready() for l in locks):return
   for w in clients:
    replicas={a.get_name():a for a in unreal.GameplayStatics.get_all_actors_of_class(w,unreal.HodgeEnemyCharacter)}
    if any(e.get_name() not in replicas or abs(replicas[e.get_name()].get_actor_location().x-e.get_actor_location().x)>3 or abs(replicas[e.get_name()].get_actor_location().y-e.get_actor_location().y)>3 for e in [target,right]):return
   check('LOCK-07_各端初始化就绪',all(l.is_ready() for l in locks));inject(lock_action,1);start(0,now)
  elif stage==0:
   if age<.25:inject(lock_action,1);return
   inject(lock_action)
   if matches(target):
    check('LOCK-01_中键获取中心敌人并复制',True)
    check('LOCK-04_普通锁定侧移风格',all('RESERVED_STRAFE' in r['style'] for r in rows))
    start(1,now)
   elif age>4:raise AssertionError(('中键获取失败',rows,[e.get_name() for e in enemies]))
  elif stage==1:
   if age<.3:return
   check('LOCK-01_松开中键仍保留目标',matches(target))
   widget_lib=unreal.get_default_object(unreal.load_class(None,'/Script/UMG.WidgetBlueprintLibrary'))
   markers=widget_lib.call_method('GetAllWidgetsOfClass',(local,unreal.HodgeLockOnIndicatorWidget.static_class(),False))
   check('LOCK-05_原生标记实例已进入HUD',len(markers)==1,len(markers))
   state['capture_at']=time.time()
   assert unreal.HodgeUIAuthoringLibrary.queue_ui_action(local.get_controller(),'Capture',0)
   inject(look,2);start(2,now)
  elif stage==2:
   if age<.3:return
   check('LOCK-01_轻微鼠标移动不切换',matches(target));inject(look,40);start(3,now)
  elif stage==3:
   if matches(right):check('LOCK-01_向右甩动切换并复制',True);start(4,now)
   elif age>4:raise AssertionError(('向右甩动未切换',rows))
  elif stage==4:
   if age<.5:return
   inject(look,-40);start(5,now)
  elif stage==5:
   if matches(target):check('LOCK-01_向左甩动切回',True);inject(dash,1);start(6,now)
   elif age>4:raise AssertionError(('向左甩动未切换',rows))
  elif stage==6:
   inject(dash)
   if 'FREE_DIRECTIONAL' in rows[peers.index(local)]['style']:state['dash_seen']=True
   if age<1.5:return
   check('LOCK-04_点按Dash期间自由风格',state['dash_seen'])
   check('LOCK-04_Dash结束锁定目标保留',matches(target))
   check('LOCK-04_Dash结束恢复侧移',all('RESERVED_STRAFE' in r['style'] for r in rows))
   for p in [source,local]:p.set_actor_location(origin,False,True)
   inject(move,-1,0);inject(dash,1);start(7,now)
  elif stage==7:
   inject(move,-1,0);inject(dash,1)
   session=local.get_component_by_class(unreal.HodgeHeroComponent).get_sprint_input_session()
   if session.state==unreal.HodgeSprintInputState.SPRINT_ACTIVE:state['sprint_seen']=True
   if age<1.2 or not state['sprint_seen']:
    if age>4:raise AssertionError(('Sprint衔接失败',str(session.state),rows))
    return
   check('LOCK-04_长按衔接Sprint',True)
   check('LOCK-04_Sprint保留锁定',matches(target))
   check('LOCK-04_Sprint保持自由风格',all('FREE_DIRECTIONAL' in r['style'] for r in rows))
   inject(dash);inject(move);start(8,now)
  elif stage==8:
   if age<1.:return
   check('LOCK-04_松开Sprint恢复侧移',all('RESERVED_STRAFE' in r['style'] for r in rows));inject(lock_action,1);start(9,now)
  elif stage==9:
   inject(lock_action)
   if all(l.get_confirmed_target() is None for l in locks):
    check('LOCK-01_中键解除各端确认',True);check('LOCK-04_解除恢复自由风格',all('FREE_DIRECTIONAL' in r['style'] for r in rows))
    for p in [source,local]:p.set_actor_location(origin,False,True);p.get_controller().set_control_rotation(unreal.Rotator(0,0,0))
    start(10,now)
   elif age>4:raise AssertionError(('中键解除失败',rows))
  elif stage==10:
   if age<.7:return
   # 解除混合期间的一次 SetControlRotation 会被实际画面写回，待默认模式稳定再摆夹具。
   if not state.get('reacquire_camera_set'):
    for p in [source,local]:p.get_controller().set_control_rotation(unreal.Rotator(0,0,0))
    state['reacquire_camera_set']=now;return
   if now-state['reacquire_camera_set']<.15:return
   inject(lock_action,1);start(11,now)
  elif stage==11:
   inject(lock_action)
   if matches(target):
    check('LOCK-01_再次获取',True)
    event=unreal.GameplayTag();assert event.import_text('(TagName="GameplayEvent.Death")')
    assert unreal.HodgeCombatValidationLibrary.queue_gameplay_event(target.get_hodge_ability_system_component(),event)
    start(12,now)
   elif age>4:raise AssertionError(('死亡测试前重新获取失败',rows))
  elif stage==12:
   if all(l.get_confirmed_target() is None for l in locks):
    check('LOCK-07_真实死亡GA清除各端目标',True)
    check('LOCK-02_死亡没有自动换到邻居',all(l.get_presentation_target() is None for l in locks))
    finish()
   elif age>4:raise AssertionError(('目标死亡未解除',rows))
 except Exception:finish(traceback.format_exc())
state['callback']=unreal.register_slate_post_tick_callback(tick)
print('已开始锁定测试 '+mode)
