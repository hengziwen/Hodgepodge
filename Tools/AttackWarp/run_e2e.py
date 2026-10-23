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
definition=unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_1');facing=definition.get_editor_property('FacingWarp')
windows=unreal.MotionWarpingUtilities.get_motion_warping_windows_for_warp_target_from_animation(montage,facing.get_editor_property('WarpTargetName'))
assert len(windows)==1,'正式首招必须保留唯一原Warp窗口'
warp_start=windows[0].start_time;warp_end=windows[0].end_time;max_angle=facing.get_editor_property('MaxCorrectionAngle');max_rate=facing.get_editor_property('MaxRotationRate')
ability_lib=unreal.get_default_object(unreal.AbilitySystemLibrary)
# 经批准仅把限幅输入移到现135度上限之外，其他用例输入和倍率保持不变。
cases=[('camera45',45.,1.),('camera-45',-45.,1.),('angle_limit',165.,1.),('rate2',45.,2.),('rateHalf',45.,.5),('move_intent',0.,1.),('locked',0.,1.),('dash_cancel',45.,1.),('move_cancel',45.,1.)]
assert max_angle<165.<180.,'限幅用例输入必须超过正式资产角度上限'
if clients:cases += [('reject_inside',45.,.25),('reject_after',45.,1.)]
checks=[];samples=[];state={'wall':time.monotonic(),'case':-1,'stage':-1,'at':0.,'frames':0,'key':False}
origin=unreal.Vector(1000,1000,100)

def diff(a,b):return (a-b+180)%360-180

def inject(action,value=unreal.Vector()):inputs.inject_input_vector_for_action(action,value,[],[])

def check(name,ok,detail=None):
 checks.append({'check':name,'passed':bool(ok),'detail':detail});assert ok,(name,detail)

def start(stage,now):state.update({'stage':stage,'at':now})

def root_mode(p):return 'MONTAGE_WARP' in str(p.get_component_by_class(unreal.HodgeCharacterRotationComponent).get_resolved_state().execution_mode)

def inspect_rate_limited_turn(actor,goal):
 # 固定倍率下以Montage推进量还原远端移动模拟时间；编辑器帧时间不能替代ServerMove步长。
 selected=[peer for row in samples if row['case']==state['case'] and row['stage']==3 for peer in row['peers']
           if peer['actor']==actor.get_path_name() and peer['instance']>=0 and not peer['stopped']]
 detail={'actor':actor.get_path_name(),'goal':goal,'max_rate':max_rate,'clock':'Montage progression / observed effective rate',
         'samples':selected,'intervals':[],'violations':[]}
 if len(selected)<2:
  detail['violations'].append('有效相邻观测样本不足');return detail
 start_yaw=selected[0]['yaw'];distance=diff(goal,start_yaw);direction=1. if distance>0 else -1.
 if abs(distance)<=.01:detail['violations'].append('没有可验收的大角度转向距离')
 if len({(row['instance'],row['request']) for row in selected})!=1:detail['violations'].append('采样跨越Montage实例或动作请求')
 if not any(warp_start<=row['position']<warp_end for row in selected) or selected[-1]['position']<warp_end:
  detail['violations'].append('缺少窗内采样或跨窗口末尾采样')
 for row in selected:
  progress=direction*diff(row['yaw'],start_yaw)
  if not math.isfinite(progress) or progress<-.01 or progress>abs(distance)+.01:
   detail['violations'].append({'reason':'方向反退或越过目标','position':row['position'],'yaw':row['yaw']})
  if not math.isfinite(row['effective_rate']) or abs(row['effective_rate']-1.)>.000001:
   detail['violations'].append({'reason':'大角度观测总倍率必须为1','sample':row})
 # 0.01度仅吸收浮点采样误差；所有区间均保留，不跳过方向纠正或时间倒退。
 for previous,row in zip(selected,selected[1:]):
  world_dt=row['world_time']-previous['world_time'];dt=row['position']-previous['position'];step=diff(row['yaw'],previous['yaw'])
  interval={'world_dt':world_dt,'simulation_dt':dt,'step':step,'limit':max_rate*dt}
  detail['intervals'].append(interval)
  if not all(math.isfinite(x) for x in (dt,world_dt,step)) or dt<0 or world_dt<0 or (world_dt==0 and (dt!=0 or abs(step)>.01)) or abs(step)>max_rate*dt+.01 or direction*step<-.01:
   detail['violations'].append({'reason':'相邻观测速率、时钟或推进方向非法','interval':interval})
 detail.update(start_yaw=start_yaw,final_yaw=selected[-1]['yaw'],progress=direction*diff(selected[-1]['yaw'],start_yaw))
 if not any(abs(row['step'])>.01 and row['simulation_dt']>0 for row in detail['intervals']):
  detail['violations'].append('未观察到真实转向推进区间')
 return detail

def sample_locked_facing(p,role):
 # 原窗口结束后的GA值是最后合法Yaw；只读当前执行身份，不重新计算或写入方向。
 info=unreal.HodgeCombatValidationLibrary.inspect_montage_state(p,montage)
 if info.instance_id<0 or info.stopped:return
 rotation=p.get_component_by_class(unreal.HodgeCharacterRotationComponent).get_resolved_state()
 # Python不能读取受保护Spec字段；从本Avatar的已授予定义中查询唯一真实活动实例。
 asc=p.get_hodge_ability_system_component();matches=[]
 for handle in asc.get_all_abilities():
  if asc.find_ability_definition(handle)!=definition:continue
  ga=ability_lib.call_method('GetGameplayAbilityFromSpecHandle',(asc,handle))[0]
  if ga and ability_lib.call_method('IsGameplayAbilityActive',(ga,)) and ga.get_avatar_actor_from_actor_info()==p:
   matches.append((handle,ga))
 assert len(matches)==1,'锁定采样必须找到本Avatar唯一活动首刀实例'
 handle,ga=matches[0]
 assert ga.get_definition()==definition,'锁定采样不能跨到其他攻击'
 confirmed=p.get_component_by_class(unreal.HodgeLockOnComponent).get_confirmed_target();execution_target=ga.get_execution_lock_target()
 assert confirmed and execution_target==confirmed and execution_target.get_name()==state['locked_target'],'本刀执行目标或确认身份改变'
 request=rotation.get_editor_property('ActionRequestId').export_text()
 identity={'ability':ga.get_path_name(),'spec':handle.export_text(),'request':request,'instance':info.instance_id,'target':execution_target.get_name()}
 previous=state['locked_facing'].get(role)
 if previous is None:previous=state['locked_facing'][role]={'identity':identity,'samples':[]}
 assert previous['identity']==identity,'窗口末采样不能混入下一执行或实例'
 yaw=ga.get_attack_facing_yaw();assert math.isfinite(yaw),'最后合法Yaw必须有限'
 if 'final_yaw' in previous:assert abs(diff(yaw,previous['final_yaw']))<.001,'Warp窗口结束后最后合法Yaw不能继续更新'
 previous['samples'].append({'position':info.position,'yaw':yaw,'confirmed':confirmed.get_path_name(),'execution_target':execution_target.get_path_name()})
 if info.position>=warp_end and 'final_yaw' not in previous:
  previous.update(final_yaw=yaw,final_position=info.position)

def finish(error=None):
 unreal.unregister_slate_post_tick_callback(state['callback']);inject(attack);inject(dash);inject(move)
 if state['key']:unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'LockOnKey',0)
 result={'mode':mode,'passed':not error and all(c['passed'] for c in checks),'checks':checks,'samples':samples,'error':error,
         'facing_configuration':{'max_angle':max_angle,'max_rate':max_rate,'warp_start':warp_start,'warp_end':warp_end},
         'angle_limit_result':state.get('angle_limit_result'),'locked_window_result':state.get('locked_window_result')}
 (WORK/(mode+'-attack-warp-e2e.json')).write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
 print(json.dumps({'mode':mode,'passed':result['passed'],'checks':len(checks),'error':error},ensure_ascii=False))
 for n in ['worlds','clients','peers','source','local','server','pc','lock','inputs','target','replica','enemies','w','p','a']:globals()[n]=None
 gc.collect()

def tick(delta):
 try:
  assert time.monotonic()-state['wall']<130,'Attack Warp E2E timeout'
  state['frames']+=1
  now=unreal.GameplayStatics.get_time_seconds(server);age=now-state['at'];stage=state['stage']
  rows=[]
  for p in peers:
   m=unreal.HodgeCombatValidationLibrary.inspect_montage_state(p,montage);r=p.get_component_by_class(unreal.HodgeCharacterRotationComponent).get_resolved_state()
   rows.append({'actor':p.get_path_name(),'world_time':unreal.GameplayStatics.get_time_seconds(p.get_world()),
                'role':str(p.get_local_role()),'yaw':p.get_actor_rotation().yaw,'mode':str(r.execution_mode),'target_yaw':r.action_yaw,'position':m.position,'instance':m.instance_id,'stopped':m.stopped,
                'request':r.get_editor_property('ActionRequestId').export_text()})
   if state.get('name')=='angle_limit' and m.instance_id>=0 and not m.stopped:
    mesh=p.get_component_by_class(unreal.SkeletalMeshComponent);anim=mesh.get_anim_instance()
    rows[-1].update(instance_rate=anim.montage_get_play_rate(montage),asset_rate=montage.get_editor_property('RateScale'),mesh_rate=mesh.get_editor_property('GlobalAnimRateScale'))
    rows[-1]['effective_rate']=rows[-1]['instance_rate']*rows[-1]['asset_rate']*rows[-1]['mesh_rate']
  samples.append({'time':now,'case':state['case'],'stage':stage,'peers':rows})
  if stage==-1:
   state['case']+=1
   if state['case']>=len(cases):finish();return
   name,angle,rate=cases[state['case']];state.update({'name':name,'angle':angle,'rate':rate,'first':None,'rate_set':set(),'peer_seen':set(),'turn_samples':[],'recovery_seen':False,'move_prepared':False,'restore_checked':False,'locked_facing':{}})
   if lock.get_presentation_target():
    state['key']=True;assert unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'LockOnKey',1)
   for p in [source,local]:p.set_actor_location(origin,False,True);p.set_actor_rotation(unreal.Rotator(),False);p.get_controller().set_control_rotation(unreal.Rotator(yaw=angle))
   start(0,now)
  elif stage==0:
   if state['key']:unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'LockOnKey',0);state['key']=False
   if age<.8:return
   name=state['name']
   if name=='locked' and not lock.get_presentation_target():
    for a in [target,replica]:a.set_actor_location(origin+unreal.Vector(600,250,0),False,True)
    state['key']=True;assert unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'LockOnKey',1);start(1,now);return
   start(2,now)
  elif stage==1:
   if state['key']:unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'LockOnKey',0);state['key']=False
   if not lock.get_confirmed_target() or age<.4:return
   start(2,now)
  elif stage==2:
   # 在输入帧设置测试姿势，避免准备阶段的基础模式提前完成转向。
   for p in [source,local]:
    p.set_actor_rotation(unreal.Rotator(),False)
    if state['name']!='locked':p.get_controller().set_control_rotation(unreal.Rotator(yaw=state['angle']))
   if state['name']=='move_intent' and not state['move_prepared']:
    state['move_prepared']=True;inject(move,unreal.Vector(1,0,0));return
   if state['name']=='locked':
    assert lock.get_confirmed_target(),'锁定用例起手必须已有确认目标'
    state['locked_target']=lock.get_confirmed_target().get_name()
   if state['name']=='angle_limit':print('ATTACK_WARP_RATE_BEGIN')
   inject(attack,unreal.Vector(1,0,0));state['pressed']=now;state['pressed_frame']=state['frames'];start(3,now)
  elif stage==3:
   inject(attack)
   for i,p in enumerate(peers):
    info=unreal.HodgeCombatValidationLibrary.inspect_montage_state(p,montage)
    if info.instance_id>=0 and not info.stopped:
     state['peer_seen'].add(i)
     if i not in state['rate_set'] and state['rate']!=1.:
      p.get_component_by_class(unreal.SkeletalMeshComponent).get_anim_instance().montage_set_play_rate(montage,state['rate']);state['rate_set'].add(i)
   primary=unreal.HodgeCombatValidationLibrary.inspect_montage_state(local,montage)
   if primary.instance_id>=0 and not primary.stopped and state['first'] is None:
    state['first']=now;state['first_yaw']=local.get_actor_rotation().yaw;state['goal']=local.get_component_by_class(unreal.HodgeCharacterRotationComponent).get_resolved_state().action_yaw
    check(state['name']+'_AW01_没有额外物理转身等待',state['frames']-state['pressed_frame']<=2,{'game_seconds':now-state['pressed'],'frames':state['frames']-state['pressed_frame']})
   if state['first'] is None:
    if age>3:raise AssertionError('正式输入没有播放第1段攻击')
    return
   if state['name']=='locked':
    sample_locked_facing(local,'owner');sample_locked_facing(source,'authority')
   state['turn_samples'].append(local.get_actor_rotation().yaw)
   if state['name']=='move_intent' and age>.12:inject(move)
   if state['name'].startswith('reject_'):
    server_info=unreal.HodgeCombatValidationLibrary.inspect_montage_state(source,montage)
    ready=server_info.instance_id>=0 and not server_info.stopped and len(state['peer_seen'])==len(peers)
    eligible=(.025<primary.position<.09 and server_info.position<.09) if state['name']=='reject_inside' else (.25<primary.position<.55 and server_info.position>.12)
    if ready and eligible:
     state['old_instance']=primary.instance_id;state['restore_goal']=state['goal']
     assert unreal.HodgeCombatValidationLibrary.queue_attack_facing_probe(local,'Suspend');start(7,now);return
    if age>5:raise AssertionError('无法建立取消恢复夹具')
    return
   if age<.45:return
   goal=state['goal'];authority_goal=goal
   if state['name']=='locked':
    final=state['locked_facing'];valid=all('final_yaw' in final.get(role,{}) for role in ('owner','authority'))
    check('locked_AW02_窗口末合法方向与目标身份正确',valid and abs(final['owner']['final_yaw'])>10.,final)
    goal=final['owner']['final_yaw'];authority_goal=final['authority']['final_yaw'];state['locked_window_result']=final
   else:
    expected=90. if state['name']=='move_intent' else max(-max_angle,min(max_angle,state['angle'])) if state['name']=='angle_limit' else state['angle']
    check(state['name']+'_AW02_固定方向正确',abs(diff(goal,expected))<5.,goal)
   if state['name']=='angle_limit':
    bounded={'owner':inspect_rate_limited_turn(local,goal),'authority':inspect_rate_limited_turn(source,goal)}
    state['angle_limit_result']=bounded
    check('angle_limit_AW01_按速率上限推进且不越过目标',all(not row['violations'] for row in bounded.values()),bounded)
   else:check(state['name']+'_AW01_角色根运动转向目标',abs(diff(local.get_actor_rotation().yaw,goal))<6.,[local.get_actor_rotation().yaw,goal])
   check(state['name']+'_AW03_按动画阶段持有占用',root_mode(local)==(primary.position<.6),[root_mode(local),primary.position])
   check(state['name']+'_AW05_各网络角色出现Montage',len(state['peer_seen'])==len(peers),list(state['peer_seen']))
   if state['name']=='locked':
    # 锁定两端各对窗口末Yaw验运动结果，并继续保留原8度跨端一致性约束。
    check('locked_AW05_权威朝向一致',abs(diff(source.get_actor_rotation().yaw,authority_goal))<8. and abs(diff(source.get_actor_rotation().yaw,goal))<8.,{'yaw':source.get_actor_rotation().yaw,'authority_goal':authority_goal,'owner_goal':goal})
   elif state['name']=='angle_limit':
    # 大角度保留8度真实跨端一致性，不要求短窗口内完成超出速率预算的旋转。
    check('angle_limit_AW05_权威朝向一致',abs(diff(source.get_actor_rotation().yaw,local.get_actor_rotation().yaw))<8.,
          {'authority':source.get_actor_rotation().yaw,'owner':local.get_actor_rotation().yaw})
   else:check(state['name']+'_AW05_权威朝向一致',abs(diff(source.get_actor_rotation().yaw,state['goal']))<8.,source.get_actor_rotation().yaw)
   if state['name']=='angle_limit':print('ATTACK_WARP_RATE_END')
   if state['name']=='dash_cancel':inject(dash,unreal.Vector(1,0,0));start(5,now)
   else:start(4,now)
  elif stage==4:
   # 等待实际动画源时间进入 Recovery，倍率改变不使用固定墙钟恢复。
   primary=unreal.HodgeCombatValidationLibrary.inspect_montage_state(local,montage)
   if primary.position<.62 and not primary.stopped:return
   check(state['name']+'_AW03_Recovery释放占用',not root_mode(local))
   if state['name']=='move_cancel':inject(move,unreal.Vector(0,1,0));start(6,now)
   else:inject(dash,unreal.Vector(1,0,0));start(5,now)
  elif stage==5:
   inject(dash)
   if age<1.3:return
   check(state['name']+'_AW04_Dash取消无旧Warp',not root_mode(local))
   start(-1,now)
  elif stage==7:
   primary=unreal.HodgeCombatValidationLibrary.inspect_montage_state(local,montage)
   if not primary.stopped and age<.15:return
   check(state['name']+'_AW06_预测暂停动画与执行权',primary.stopped and not root_mode(local))
   assert unreal.HodgeCombatValidationLibrary.queue_attack_facing_probe(local,'RestoreFromAuthority');start(8,now)
  elif stage==8:
   primary=unreal.HodgeCombatValidationLibrary.inspect_montage_state(local,montage)
   if age<.2:return
   if not state['restore_checked']:
    state['restore_checked']=True
    check(state['name']+'_AW06_权威拒绝结果恢复实际Montage',not primary.stopped and primary.instance_id!=state['old_instance'],primary.instance_id)
    if state['name']=='reject_after':check('reject_after_AW06_恢复到窗口后不重复转向',abs(diff(local.get_actor_rotation().yaw,state['restore_goal']))<6.)
   if age<.65:return
   check(state['name']+'_AW06_恢复后仍保持固定目标',abs(diff(local.get_actor_rotation().yaw,state['restore_goal']))<6.)
   inject(dash,unreal.Vector(1,0,0));start(5,now)
  elif stage==6:
   if age<.4:return
   inject(move)
   check('move_cancel_AW06_预测退出无残留占用',not root_mode(local))
   inject(dash,unreal.Vector(1,0,0));start(5,now)
 except Exception:finish(traceback.format_exc())
state['callback']=unreal.register_slate_post_tick_callback(tick)
print('开始真实攻击旋转 Warp 验证：'+mode)
