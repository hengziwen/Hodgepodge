"""锁定后逐方向测试从按键会话开始到动画／位移的延迟，包含真实 Designer 控件。"""
import unreal,json,time,math,traceback,gc
from pathlib import Path
WORK=Path(globals().get('LOCK_ON_WORK','E:/Project/Git/Hodgepodge/Saved/LockOnDash'));mode=globals().get('LOCK_ON_MODE','single')
worlds=unreal.EditorLevelLibrary.get_pie_worlds(True);server=next(w for w in worlds if unreal.SystemLibrary.is_server(w));clients=[w for w in worlds if w!=server]
if mode=='listen-lag':
 for w in worlds:unreal.SystemLibrary.execute_console_command(w,'Net PktLag=100')
local=next(p for p in unreal.GameplayStatics.get_all_actors_of_class(clients[0] if clients else server,unreal.HodgeHeroCharacter) if not clients or p.get_local_role()==unreal.NetRole.ROLE_AUTONOMOUS_PROXY)
pc=local.get_controller();hero=local.get_component_by_class(unreal.HodgeHeroComponent);lock=local.get_component_by_class(unreal.HodgeLockOnComponent)
pid=local.get_editor_property('PlayerState').get_editor_property('PlayerId')
peers=[p for w in worlds for p in unreal.GameplayStatics.get_all_actors_of_class(w,unreal.HodgeHeroCharacter) if p.get_editor_property('PlayerState').get_editor_property('PlayerId')==pid]
source=next(p for p in peers if p.get_world()==server)
enemies=sorted(unreal.GameplayStatics.get_all_actors_of_class(server,unreal.HodgeEnemyCharacter),key=lambda p:p.get_name());target=enemies[0]
origin=unreal.Vector(1000,1000,100)
for i,e in enumerate(enemies):
 if e.get_controller() and e.get_controller().brain_component:e.get_controller().brain_component.stop_logic('Dash direction validation')
 e.get_component_by_class(unreal.CharacterMovementComponent).disable_movement()
 e.set_actor_location(origin+unreal.Vector(1000,0 if i==0 else 3000,0),False,True)
for w in worlds:
 for e in unreal.GameplayStatics.get_all_actors_of_class(w,unreal.HodgeEnemyCharacter):
  # 临时使用胶囊 Bounds 锚点，避免胸口骨骼的细小摆动令正负135度边界随机换 F/B。
  e.set_editor_property('LockOnSocket','None')
 for p in unreal.GameplayStatics.get_all_actors_of_class(w,unreal.HodgeHeroCharacter):
  if p not in peers:
   p.get_component_by_class(unreal.CharacterMovementComponent).disable_movement();p.set_actor_location(unreal.Vector(-3000,-3000,100),False,True)
inputs=unreal.get_default_object(unreal.load_class(None,'/Script/Engine.SubsystemBlueprintLibrary')).call_method('GetLocalPlayerSubSystemFromPlayerController',(pc,unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
move=unreal.load_asset('/Game/Main/Input/InputAction/IA_Move');dash=unreal.load_asset('/Game/Main/Input/InputAction/IA_Sprint')
profile=unreal.load_asset('/Game/Main/Character/Hero/Ability/Sprint/DA_Hero_SprintAbility')
forward=profile.get_editor_property('ForwardMontage');backward=profile.get_editor_property('BackwardMontage');anim=local.mesh.get_anim_instance()
angles=[0,45,90,130,135,180,-135,-90,-45];checks=[];samples=[];timings=[]
state={'wall':time.monotonic(),'stage':0,'at':unreal.GameplayStatics.get_time_seconds(server),'case':0,'key':False}

def input(action,x=0.,y=0.):inputs.inject_input_vector_for_action(action,unreal.Vector(x,y,0),[],[])
def key(value):
 if state['key']!=value:
  state['key']=value;assert unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'LockOnKey',1 if value else 0)
def same():return all(p.get_component_by_class(unreal.HodgeLockOnComponent).get_confirmed_target() and p.get_component_by_class(unreal.HodgeLockOnComponent).get_confirmed_target().get_name()==target.get_name() for p in peers)
def check(name,value,detail=None):checks.append({'check':name,'passed':bool(value),'detail':detail});assert value,(name,detail)
def stage(value,now):state.update({'stage':value,'at':now})
def finish(error=None):
 unreal.unregister_slate_post_tick_callback(state['callback']);input(move);input(dash);key(False)
 r={'mode':mode,'passed':not error and all(c['passed'] for c in checks),'checks':checks,'timings':timings,'samples':samples,'error':error}
 (WORK/(mode+'-dash-directions.json')).write_text(json.dumps(r,ensure_ascii=False,indent=2),encoding='utf-8')
 print(json.dumps({'mode':mode,'passed':r['passed'],'checks':len(checks),'timings':timings,'error':error},ensure_ascii=False))
 for n in ['worlds','clients','peers','source','local','server','pc','hero','lock','target','enemies','inputs','anim','w','p','e','marker','box','glyph']:globals()[n]=None
 gc.collect()

def tick(delta):
 try:
  assert time.monotonic()-state['wall']<180,'方向测试超时'
  now=unreal.GameplayStatics.get_time_seconds(server);local_now=unreal.GameplayStatics.get_time_seconds(local);age=now-state['at'];s=state['stage']
  session=hero.get_sprint_input_session();f=anim.montage_is_playing(forward);b=anim.montage_is_playing(backward)
  pos=local.get_actor_location()
  samples.append({'time':now,'local_time':local_now,'case':state['case'],'stage':s,'session':str(session.state),'pressed_at':session.pressed_at,'forward':f,'backward':b,'yaw':local.get_actor_rotation().yaw,'position':[pos.x,pos.y,pos.z]})
  if s==0:
   input(move);input(dash)
   if lock.get_presentation_target():key(True);stage(1,now);return
   stage(1,now)
  elif s==1:
   key(False)
   if age<.8:return
   for p in [source,local]:p.set_actor_location(origin,False,True);p.get_controller().set_control_rotation(unreal.Rotator(0,0,0))
   stage(2,now)
  elif s==2:
   if age<.2:return
   key(True);stage(3,now)
  elif s==3:
   key(False)
   if age<.4 or not same():
    if age>5:raise AssertionError('锁定未完成')
    return
   if state['case']==0 and globals().get('VERIFY_MARKER',False):
    if not state.get('paint_requested'):
     state['paint_requested']=True;assert unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'Capture',0);return
    widgets=unreal.get_default_object(unreal.load_class(None,'/Script/UMG.WidgetBlueprintLibrary')).call_method('GetAllWidgetsOfClass',(local,unreal.HodgeLockOnIndicatorWidget.static_class(),False))
    expected_class='/Game/Main/UI/Targeting/WBP_HeroLockOnIndicator.WBP_HeroLockOnIndicator_C'
    marker=next((w for w in widgets if w.get_class().get_path_name()==expected_class),None)
    if not marker:
     if age>5:raise AssertionError('标记软类加载未完成')
     return
    tree=unreal.find_object(marker,'WidgetTree_0') or unreal.find_object(marker,'WidgetTree')
    box=unreal.find_object(tree,'MarkerSize');glyph=unreal.find_object(tree,'MarkerGlyph')
    check('UI-01_正式WBP实例进入Indicator',bool(box and glyph))
    box.set_width_override(64.);box.set_height_override(64.)
    state['marker']=marker;state['box']=box;state['glyph']=glyph;stage(4,now);return
   stage(5,now)
  elif s==4:
   if age<.15:return
   size=state['marker'].get_desired_size()
   check('UI-02_Designer尺寸在实际Slate生效',abs(size.x-64.)<.1 and abs(size.y-64.)<.1,str(size))
   state['box'].set_width_override(36.);state['box'].set_height_override(36.)
   state['marker']=state['box']=state['glyph']=None;stage(5,now)
  elif s==5:
   angle=angles[state['case']];r=math.radians(angle);input(move,math.sin(r),math.cos(r));input(dash,1)
   state['started_session']=hero.get_sprint_input_session().session_id;state['origin']=local.get_actor_location();state['animation_at']=state['movement_at']=None
   stage(6,now)
  elif s==6:
   input(move);input(dash)
   if session.session_id>state['started_session']:
    elapsed=local_now-session.pressed_at
    if state['animation_at'] is None and (f or b):state['animation_at']=elapsed;state['variant']='B' if b else 'F'
    if state['movement_at'] is None and (pos-state['origin']).length()>2.:state['movement_at']=elapsed
   if age<1.4:return
   angle=angles[state['case']];expected='B' if profile.get_editor_property('bEnableBackwardVariant') and math.cos(math.radians(180-abs(angle)))+1.e-4>=math.cos(math.radians(profile.get_editor_property('BackwardConeAngle'))) else 'F'
   check('DASH-01_方向'+str(angle)+'动画即时开始',state['animation_at'] is not None and state['animation_at']<.08,state['animation_at'])
   check('DASH-02_方向'+str(angle)+'位移即时开始',state['movement_at'] is not None and state['movement_at']<.08,state['movement_at'])
   check('DASH-03_方向'+str(angle)+'F_B语义保留',state.get('variant')==expected,(state.get('variant'),expected))
   check('DASH-04_方向'+str(angle)+'各端锁定保留',same())
   timings.append({'angle':angle,'animation_delay':state['animation_at'],'movement_delay':state['movement_at'],'variant':state.get('variant')})
   state['case']+=1
   if state['case']>=len(angles):finish()
   else:stage(0,now)
 except Exception:finish(traceback.format_exc())
state['callback']=unreal.register_slate_post_tick_callback(tick)
print('开始锁定各方向Dash时序验证：'+mode)
