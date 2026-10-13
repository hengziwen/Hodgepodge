"""真实普攻输入与角色运动采样，只修改 PIE 副本和动画实例倍率。"""
import unreal,json,time,traceback,math,gc
from pathlib import Path
WORK=Path(globals().get('LOCK_ON_WORK','E:/Project/Git/Hodgepodge/Saved/AttackWarp'));mode=globals().get('LOCK_ON_MODE','single')
worlds=unreal.EditorLevelLibrary.get_pie_worlds(True);server=next(w for w in worlds if unreal.SystemLibrary.is_server(w));clients=[w for w in worlds if w!=server]
if mode=='listen-lag':
 for w in worlds:unreal.SystemLibrary.execute_console_command(w,'Net PktLag=100')
local=next(a for a in unreal.GameplayStatics.get_all_actors_of_class(clients[0] if clients else server,unreal.HodgeHeroCharacter) if not clients or a.get_local_role()==unreal.NetRole.ROLE_AUTONOMOUS_PROXY)
pid=local.get_editor_property('PlayerState').get_editor_property('PlayerId')
peers=[a for w in worlds for a in unreal.GameplayStatics.get_all_actors_of_class(w,unreal.HodgeHeroCharacter) if a.get_editor_property('PlayerState').get_editor_property('PlayerId')==pid]
source=next(p for p in peers if p.get_world()==server);pc=local.get_controller();lock=local.get_component_by_class(unreal.HodgeLockOnComponent)
for w in worlds:
 for a in unreal.GameplayStatics.get_all_actors_of_class(w,unreal.HodgeHeroCharacter):
  if a not in peers:a.get_component_by_class(unreal.CharacterMovementComponent).disable_movement()
 for a in unreal.GameplayStatics.get_all_actors_of_class(w,unreal.HodgeEnemyCharacter):
  a.get_component_by_class(unreal.CharacterMovementComponent).disable_movement()
  if a.get_controller() and a.get_controller().brain_component:a.get_controller().brain_component.stop_logic('Attack Warp validation')
  a.set_actor_location(unreal.Vector(2300,2300,100),False,True)
enemies=unreal.GameplayStatics.get_all_actors_of_class(server,unreal.HodgeEnemyCharacter);target=enemies[0]
replica=next(a for a in unreal.GameplayStatics.get_all_actors_of_class(local.get_world(),unreal.HodgeEnemyCharacter) if a.get_name()==target.get_name())
lib=unreal.get_default_object(unreal.load_class(None,'/Script/Engine.SubsystemBlueprintLibrary'))
inputs=lib.call_method('GetLocalPlayerSubSystemFromPlayerController',(pc,unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
attack=unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack');dash=unreal.load_asset('/Game/Main/Input/InputAction/IA_Sprint');move=unreal.load_asset('/Game/Main/Input/InputAction/IA_Move')
montage=unreal.load_asset('/Game/Main/Character/Hero/Anim/Montages/AM_Attack01_Montage')
checks=[];samples=[];state={'wall':time.monotonic(),'at':unreal.GameplayStatics.get_time_seconds(server),'last_press':-10.,'seen':set()}
montages=[unreal.load_asset('/Game/Main/Character/Hero/Anim/Montages/AM_Attack0'+str(i)+'_Montage') for i in range(1,6)]
for p in [source,local]:p.set_actor_location(unreal.Vector(1000,1000,100),False,True);p.set_actor_rotation(unreal.Rotator(),False);p.get_controller().set_control_rotation(unreal.Rotator(yaw=45))
def finish(error=None):
 unreal.unregister_slate_post_tick_callback(state['callback']);inputs.inject_input_vector_for_action(attack,unreal.Vector(),[],[])
 result={'mode':mode,'passed':not error,'checks':checks,'samples':samples,'error':error}
 (WORK/(mode+'-combo-regression.json')).write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
 print(json.dumps({'mode':mode,'passed':result['passed'],'checks':len(checks),'error':error},ensure_ascii=False))
 for n in ['worlds','clients','peers','source','local','server','pc','lock','inputs','target','replica','enemies','w','p','a']:globals()[n]=None
 gc.collect()
def tick(delta):
 try:
  assert time.monotonic()-state['wall']<90,'Combo timeout'
  now=unreal.GameplayStatics.get_time_seconds(server);age=now-state['at']
  node=str(local.get_component_by_class(unreal.HodgeCombatComponentBase).get_current_combo_tag().get_editor_property('TagName'))
  instances=[]
  for i,m in enumerate(montages):
   info=unreal.HodgeCombatValidationLibrary.inspect_montage_state(local,m)
   if info.instance_id>=0 and not info.stopped:state['seen'].add(i+1);instances.append({'attack':i+1,'position':info.position})
  samples.append({'time':now,'node':node,'instances':instances,'root_mode':str(local.get_component_by_class(unreal.HodgeCharacterRotationComponent).get_resolved_state().execution_mode)})
  if age<1:return
  if len(state['seen'])==5:
   checks.append({'check':'AW07_五段连续真实输入通过','passed':True,'detail':sorted(state['seen'])});finish();return
  if age>15:raise AssertionError(('五段未完成',sorted(state['seen']),node))
  if now-state['last_press']>.12:
   state['last_press']=now;inputs.inject_input_vector_for_action(attack,unreal.Vector(1,0,0),[],[])
  else:inputs.inject_input_vector_for_action(attack,unreal.Vector(),[],[])
 except Exception:finish(traceback.format_exc())
state['callback']=unreal.register_slate_post_tick_callback(tick)
