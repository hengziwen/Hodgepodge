"""真实R键与PIE多端验证大招成本、镜头、翼同步、取消和死亡恢复。"""
import gc
import json
import time
import traceback
from pathlib import Path
import unreal

WORK = Path(globals()['LOCK_ON_WORK'])
MODE = globals()['LOCK_ON_MODE']
CAPTURE_ONLY=globals().get('CINEMATIC_CAPTURE_ONLY',False)
REPORT_MODE='capture' if CAPTURE_ONLY else MODE
SCREEN_DIR=Path(unreal.Paths.project_saved_dir())/'Screenshots/WindowsEditor'
screens_before={path.name for path in SCREEN_DIR.glob('HighresScreenshot*.png')}
ROOT = '/Game/Main/Character/Hero/Ability/ResonanceLiberation'
worlds = unreal.EditorLevelLibrary.get_pie_worlds(True)
server = next(world for world in worlds if unreal.SystemLibrary.is_server(world))
clients = [world for world in worlds if world != server]
owner = next(pawn for pawn in unreal.GameplayStatics.get_all_actors_of_class(clients[0] if clients else server, unreal.HodgeHeroCharacter)
             if not clients or pawn.get_local_role() == unreal.NetRole.ROLE_AUTONOMOUS_PROXY)
pid = owner.get_editor_property('PlayerState').get_editor_property('PlayerId')
peers = [pawn for world in worlds for pawn in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.HodgeHeroCharacter)
         if pawn.get_editor_property('PlayerState').get_editor_property('PlayerId') == pid]
authority = next(pawn for pawn in peers if pawn.get_world() == server)
authority_state = authority.get_editor_property('PlayerState')
authority_asc = authority.get_hodge_ability_system_component()
owner_hero = owner.get_component_by_class(unreal.HodgeHeroComponent)
bridge = unreal.get_default_object(unreal.load_class(None, '/Script/GameplayAbilities.AbilitySystemBlueprintLibrary'))
inputs = unreal.get_default_object(unreal.load_class(None, '/Script/Engine.SubsystemBlueprintLibrary')).call_method(
    'GetLocalPlayerSubSystemFromPlayerController', (owner.get_controller(), unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
actions = {name: unreal.load_asset('/Game/Main/Input/InputAction/' + path) for name, path in
           [('ultimate', 'IA_Resonance_Liberation'), ('attack', 'IA_Attack'), ('dash', 'IA_Sprint'), ('look', 'IA_LookMouse'), ('lock', 'IA_LockOn')]}
definition = unreal.load_asset(ROOT + '/DA_Resonance_Liberation')
montage = definition.execution_config.montage
camera_profile=unreal.load_asset(ROOT+'/DA_Resonance_Liberation_Camera')
cooldown = unreal.load_class(None, ROOT + '/GE_Resonance_Liberation_Cooldown.GE_Resonance_Liberation_Cooldown_C')
lib = unreal.HodgeCombatValidationLibrary
enemies = unreal.GameplayStatics.get_all_actors_of_class(server, unreal.HodgeEnemyCharacter)
for world in worlds:
    if MODE == 'listen-lag':
        unreal.SystemLibrary.execute_console_command(world, 'Net PktLag=100')
    for enemy in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.HodgeEnemyCharacter):
        if enemy.get_controller() and enemy.get_controller().brain_component:
            enemy.get_controller().brain_component.stop_logic('Cinematic ultimate validation')
        enemy.character_movement.disable_movement()
        enemy.set_actor_location(unreal.Vector(2300, 2300, 100), False, True)
for pawn in {owner, authority}:
    pawn.set_actor_location(unreal.Vector(1000, 1000, 100), False, True)
    pawn.set_actor_rotation(unreal.Rotator(), False)
    pawn.get_controller().set_control_rotation(unreal.Rotator())


def tag(name):
    """查询已有原生标签，不向配置注入临时标签。"""
    value = unreal.GameplayTag()
    value.import_text('(TagName="' + name + '")')
    return value


def read(pawn):
    """读取真实GA、相机状态、资源和角色蒙太奇，模拟代理只检查复制表现。"""
    asc = pawn.get_hodge_ability_system_component()
    handle = next((entry for entry in asc.get_all_abilities() if asc.find_ability_definition(entry) == definition), None)
    ability = bridge.call_method('GetGameplayAbilityFromSpecHandle', (asc, handle))[0] if handle else None
    active = bridge.call_method('IsGameplayAbilityActive', (ability,)) if ability else False
    movie = pawn.get_component_by_class(unreal.HodgeHeroComponent).get_cinematic_camera_state()
    resource = lib.inspect_resonance_energy(pawn)
    pose = lib.inspect_montage_state(pawn, montage)
    wing = pawn.get_component_by_class(unreal.HodgeWingComponent)
    mesh = wing.get_wing_mesh() if wing else None
    return {'active': active, 'camera': movie.active, 'shot': str(movie.shot_id), 'camera_position': movie.source_position,
            'generation': movie.handle.generation, 'weight': movie.weight, 'energy': resource.x, 'capacity': resource.y,
            'position': pose.position, 'instance': pose.instance_id, 'montage_weight': pose.weight,
            'cooldown': asc.call_method('GetGameplayEffectCount', (cooldown, None, True)),
            'body': asc.has_matching_gameplay_tag(tag('State.Combat.Body.Vajra')),
            'wing': bool(mesh and mesh.visible and not mesh.hidden_in_game),
            'control_yaw': pawn.get_controller().get_control_rotation().yaw if pawn.get_controller() else 0.}


checks, samples = [], []
cases = ['free', 'lock', 'pause', 'dash', 'replay', 'death']
if CAPTURE_ONLY:cases=['free']
state = {'wall': time.monotonic(), 'phase': 'warmup', 'at': unreal.GameplayStatics.get_time_seconds(server), 'index': 0,
         'shots': set(), 'last_sample': -1., 'generations': set(), 'screenshot_shots': set()}


def inject(name, pressed=False):
    """通过EnhancedInput触发实际GAS预测，Slate回调不直接发激活RPC。"""
    if actions[name]:
        if name == 'lock':
            assert unreal.HodgeUIAuthoringLibrary.queue_ui_action(owner.get_controller(), 'LockOnKey', 1. if pressed else 0.)
            return
        inputs.inject_input_vector_for_action(actions[name], unreal.Vector(1, 0, 0) if pressed else unreal.Vector(), [], [])


def check(requirement, name, condition, detail=None):
    """逐项保存需求映射，失败中止当前模式并保留采样。"""
    checks.append({'requirement': requirement, 'case': name, 'passed': bool(condition), 'detail': detail})
    assert condition, (name, detail)


def finish(error=None):
    """只释放测试输入和回调，不修改正式技能数值。"""
    unreal.unregister_slate_post_tick_callback(state['callback'])
    for name in actions:
        inject(name)
    report = {'mode': MODE, 'passed': not error and all(row['passed'] for row in checks), 'checks': checks, 'samples': samples, 'error': error, 'bootstrap':state.get('bootstrap')}
    if CAPTURE_ONLY:
        # 渲染文件和分镜请求按发生顺序对应，不把递增截图编号写死在导出工具。
        images=sorted(path.name for path in SCREEN_DIR.glob('HighresScreenshot*.png') if path.name not in screens_before)
        report['screenshots']=[{'shot':name,'file':'Saved/Screenshots/WindowsEditor/'+file} for name,file in zip(state.get('capture_shots',[]),images)]
    (WORK / (REPORT_MODE + '-e2e.json')).write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
    print(json.dumps({'mode': MODE, 'passed': report['passed'], 'checks': len(checks), 'error': error}, ensure_ascii=False))
    for name in ['worlds', 'clients', 'owner', 'authority', 'peers', 'server', 'inputs', 'enemies', 'world', 'pawn', 'enemy']:
        globals()[name] = None
    gc.collect()


def tick(delta):
    """状态机只驱动测试夹具；技能时序继续由真实蒙太奇和原有GAS管理。"""
    try:
        now = unreal.GameplayStatics.get_time_seconds(server)
        if time.monotonic() - state['wall'] > 220:
            raise TimeoutError(state)
        if state.get('case') == 'death' and state.get('interrupted'):
            if now - state['interrupt_at'] <= .4:
                return
            check('CIN-03', '死亡清理拥有者电影相机', not unreal.SystemLibrary.is_valid(owner_hero) or not owner_hero.get_cinematic_camera_state().active)
            resource = lib.inspect_resonance_energy(authority_state)
            check('RES-03', '死亡清空充能且保留PlayerState冷却', resource.x == 0. and authority_asc.call_method('GetGameplayEffectCount', (cooldown, None, True)) == 1)
            finish()
            return
        local, remote = read(owner), read(authority)
        if now - state['last_sample'] > .1:
            samples.append({'phase': state['phase'], 'time': now, 'owner': local, 'authority': remote})
            state['last_sample'] = now
        phase = state['phase']
        if phase == 'warmup' and now - state['at'] > 2.:
            # 失败报告保留各世界Pawn身份与Definition授予情况，便于区分复制等待和权限缺口。
            state['bootstrap']=[{'world':pawn.get_world().get_path_name(),'pawn':pawn.get_path_name(),'state':pawn.get_editor_property('PlayerState').get_path_name(),'resource':list(lib.inspect_resonance_energy(pawn).to_tuple()),'definitions':[str(pawn.get_hodge_ability_system_component().find_ability_definition(handle)) for handle in pawn.get_hodge_ability_system_component().get_all_abilities()]} for pawn in peers]
            check('RES-01', '拥有者与服务器容量初始化100', local['capacity'] == 100 and remote['capacity'] == 100, [local, remote])
            check('RES-01', '新玩家零充能', local['energy'] == 0 and remote['energy'] == 0)
            # 检查真实Widget生成的控件，防止资产存在但运行时空Canvas不显示。
            ui=json.loads(unreal.HodgeUIAuthoringLibrary.inspect_player_ui(owner.get_controller()))
            check('RES-04','右下方充能Widget存在且显示真实0/100',ui.get('resonance_widget') and ui.get('resonance_percent')==0. and '0/100' in ui.get('resonance_text','') and ui.get('resonance_opacity',0)>0.,ui)
            check('RES-04','零充能按钮禁用',ui.get('resonance_enabled') is False)
            check('RES-04','充能条位于屏幕右下区域',ui.get('resonance_center_x',0)>.65 and ui.get('resonance_center_y',0)>.7,ui)
            unreal.HodgeUIAuthoringLibrary.queue_ui_action(owner.get_controller(),'Capture')
            if MODE=='single':
                if CAPTURE_ONLY:
                    names=['Hodge.Combat.DebugDraw.Weapon','Hodge.Combat.DebugDraw.Body','Hodge.Combat.DebugDraw.HitBox']
                    backup=WORK/'debug-restore.json'
                    if not backup.exists():backup.write_text(json.dumps({name:unreal.SystemLibrary.get_console_variable_int_value(name) for name in names}),encoding='utf-8')
                    for name in names:unreal.SystemLibrary.execute_console_command(owner,name+' 0')
                # 验证地图只有单向灯，临时PIE补光用于读取特写构图，不保存生产场景。
                transform=unreal.Transform(location=unreal.Vector(1250,850,270))
                gameplay=unreal.get_default_object(unreal.load_class(None,'/Script/Engine.GameplayStatics'))
                light=gameplay.call_method('BeginDeferredActorSpawnFromClass',(owner,unreal.PointLight.static_class(),transform,unreal.SpawnActorCollisionHandlingMethod.ALWAYS_SPAWN,None))
                gameplay.call_method('FinishSpawningActor',(light,transform))
                component=light.get_component_by_class(unreal.PointLightComponent)
                component.set_intensity(40000.)
                component.set_attenuation_radius(1200.)
                state['light']=light
            enemies[0].set_actor_location(unreal.Vector(1120,1000,100),False,True)
            state['health_before']=enemies[0].get_component_by_class(unreal.HodgeHealthComponent).get_health()
            inject('attack',True)
            state.update(phase='gain_attack',at=now)
        elif phase == 'gain_attack' and now-state['at']>1.6:
            inject('attack')
            actual_health=enemies[0].get_component_by_class(unreal.HodgeHealthComponent).get_health()
            check('RES-01','真实普攻伤害按阶段回能5',actual_health<state['health_before'] and remote['energy']==5.,{'health_before':state['health_before'],'health':actual_health,'resource':remote})
            enemies[0].set_actor_location(unreal.Vector(2300,2300,100),False,True)
            lib.queue_resonance_energy_delta(authority,99.-remote['energy'])
            state.update(phase='cost_prepare',at=now)
        elif phase == 'cost_prepare' and local['energy'] == 99:
            inject('ultimate', True)
            state.update(phase='cost_fail', at=now)
        elif phase == 'cost_fail' and now - state['at'] > .7:
            inject('ultimate')
            check('RES-02', '99能量R键不激活且不产生CD', not local['active'] and not remote['active'] and not remote['cooldown'], [local, remote])
            state.update(phase='ready', at=now)
        elif phase == 'ready':
            inject('ultimate')
            inject('dash')
            if remote['cooldown'] or local['cooldown'] or local['active'] or now - state['at'] < .8:
                return
            if state['index'] >= len(cases):
                finish()
                return
            state['case'] = cases[state['index']]
            if state['case']=='lock':
                enemies[0].set_actor_location(unreal.Vector(1500,1000,100),False,True)
                state.update(phase='lock_target_prepare',at=now)
                return
            lib.queue_resonance_energy_delta(authority, 100. - remote['energy'])
            state.update(phase='fill', at=now)
        elif phase=='lock_target_prepare':
            # 网络延迟下先等待目标位置复制到选择端，避免对仍在范围外的旧位置发锁定输入。
            target=next(enemy for enemy in unreal.GameplayStatics.get_all_actors_of_class(owner.get_world(),unreal.HodgeEnemyCharacter) if enemy.get_name()==enemies[0].get_name())
            if (target.get_actor_location()-enemies[0].get_actor_location()).length()>10.:
                assert now-state['at']<3.,'锁定夹具位置复制超时'
                return
            inject('lock',True)
            state.update(phase='lock_prepare',at=now)
        elif phase=='lock_prepare' and now-state['at']>.8:
            inject('lock')
            target=owner.get_component_by_class(unreal.HodgeLockOnComponent).get_confirmed_target()
            check('CIN-02','真实中键选目标完成锁定',bool(target),str(target))
            lib.queue_resonance_energy_delta(authority,100.-remote['energy'])
            state.update(phase='fill',at=now)
        elif phase == 'fill' and local['energy'] == 100:
            state['control_before']=local['control_yaw']
            inject('ultimate', True)
            state.update(phase='start', at=now, shots=set(), paused=False, resumed=False, interrupted=False, max_control_yaw=0., other_camera=False, had_post_window=False, had_wing=False)
            state.update(box_prepared=False,box_drops=0)
        elif phase == 'start':
            inject('ultimate')
            if not local['active'] or not remote['active']:
                if now - state['at'] > 2.:
                    check('RES-02', state['case'] + '_R键在两端成功激活', False, [local, remote])
                return
            check('RES-02', state['case'] + '_只消耗一次满充并产生CD', local['energy'] == 0 and remote['energy'] == 0 and remote['cooldown'] == 1, [local, remote])
            check('CIN-05', state['case'] + '_自身金刚体存在', local['body'] and remote['body'])
            state.update(phase='playing', at=now)
        elif phase == 'playing':
            if CAPTURE_ONLY:
                # 特写期间注入真实鼠标轴，现有电影Yaw断言同时验证输入抑制。
                if local['camera'] and .6<local['position']<1.2:inject('look',True)
                else:inject('look')
                if not local['camera'] and local['active'] and local['position']>montage.get_play_length()*.55:
                    if not state.get('look_started'):
                        state.update(look_started=now,look_before=local['control_yaw'])
                    if now-state['look_started']<.12:inject('look',True)
                    elif not state.get('look_checked'):
                        difference=(local['control_yaw']-state['look_before']+180.)%360.-180.
                        check('CIN-02','电影退出后真实鼠标输入恢复',abs(difference)>.1,difference)
                        state['look_checked']=True
            if state['case']=='lock':
                if not state['box_prepared'] and remote['position']>montage.get_play_length()*.60:
                    # 目标只放置一次，随后保持固定世界位置，不随玩家追踪查询盒。
                    point=authority.get_actor_location()+authority.get_actor_forward_vector()*400.
                    enemies[0].set_actor_location(point,False,True)
                    state.update(box_prepared=True,box_health=enemies[0].get_component_by_class(unreal.HodgeHealthComponent).get_health())
                if state['box_prepared']:
                    health=enemies[0].get_component_by_class(unreal.HodgeHealthComponent).get_health()
                    if health<state['box_health']-.001:state['box_drops']+=1
                    state['box_health']=health
            if local['camera']:
                state['shots'].add(local['shot'])
                state['generations'].add(local['generation'])
                difference=(local['control_yaw']-state['control_before']+180.)%360.-180.
                state['max_control_yaw'] = max(state['max_control_yaw'], abs(difference))
                others = [pawn for world in worlds for pawn in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.HodgeHeroCharacter) if pawn != owner]
                state['other_camera'] |= any(pawn.get_component_by_class(unreal.HodgeHeroComponent).get_cinematic_camera_state().active for pawn in others)
                shot=next(value for value in camera_profile.shots if str(value.shot_id)==local['shot'])
                normalized=local['camera_position']/(montage.get_play_length()*.5)
                if MODE == 'single' and normalized>(shot.start+shot.end)*.5 and local['shot'] not in state['screenshot_shots']:
                    state['screenshot_shots'].add(local['shot'])
                    state.setdefault('capture_shots',[]).append(local['shot'])
                    unreal.SystemLibrary.execute_console_command(owner, 'HighResShot 1280x720')
            if state['case'] == 'pause' and not state['paused'] and local['position'] > .65:
                for pawn in {owner, authority}:
                    lib.queue_cinematic_source_probe(pawn, 1., 0.)
                state.update(paused=True, pause_at=now)
            if state['case'] == 'pause' and state['paused'] and not state['resumed'] and now - state['pause_at'] > .6:
                check('CIN-04', '暂停精确源实例后电影位置停止', abs(local['camera_position'] - 1.) < .02 and local['camera'], local)
                for pawn in {owner, authority}:
                    lib.queue_cinematic_source_probe(pawn, 1., 1.5)
                state['resumed'] = True
            if state['case'] == 'dash' and not state['interrupted'] and local['position'] > .5:
                inject('dash', True)
                state.update(interrupted=True, interrupt_at=now)
            if state['case'] == 'death' and not state['interrupted'] and local['position'] > .5:
                lib.queue_gameplay_event(authority.get_hodge_ability_system_component(), tag('GameplayEvent.Death'))
                state.update(interrupted=True, interrupt_at=now)
                return
            if state['case'] == 'death' and state['interrupted'] and now - state['interrupt_at'] > .5:
                check('CIN-03', '死亡清理拥有者电影相机', not local['camera'], local)
                check('RES-03', '死亡保持零充能及原CD', remote['energy'] == 0 and remote['cooldown'] == 1, remote)
                finish()
                return
            if local['active'] and local['position'] > montage.get_play_length() * .55:
                state['had_post_window'] = not local['camera']
                state['had_wing'] |= local['wing']
                if state['case']=='lock':
                    mode=owner.get_component_by_class(unreal.HodgeCameraComponent).get_active_camera_mode_class()
                    state['lock_restored']=mode==unreal.HodgeCameraMode_LockOn.static_class()
            if not local['active'] and not remote['active'] and now - state['at'] > .5:
                inject('dash')
                check('CIN-03', state['case'] + '_结束清理电影与金刚体', not local['camera'] and not local['body'] and not remote['body'], [local, remote])
                check('RES-02', state['case'] + '_打断或打空不返还能量与CD', remote['energy'] == 0 and remote['cooldown'] == 1, remote)
                check('CIN-02', state['case'] + '_电影Yaw不写回游戏方向', state['max_control_yaw'] < 1., state['max_control_yaw'])
                check('CIN-07', state['case'] + '_其他玩家和专服没有电影相机', not state['other_camera'])
                if state['case'] != 'dash':
                    check('CIN-04', state['case'] + '_后半技能继续且翼仍显示', state['had_post_window'] and state['had_wing'])
                else:
                    check('CIN-03', '实际Dash在电影阶段取消大招', state['interrupted'] and local['position'] < 2.3, local)
                if state['case'] == 'free':
                    check('CIN-01', '正常播放覆盖三个分镜', state['shots'] == {'Acquire', 'FaceHands', 'WingReveal'}, sorted(state['shots']))
                    if CAPTURE_ONLY:
                        check('CIN-02','电影Look占用完整释放',state.get('look_checked',False))
                        finish()
                        return
                if state['case'] == 'replay':
                    check('CIN-03', '重复释放生成新的相机会话', len(state['generations']) >= 3, sorted(state['generations']))
                if state['case']=='lock':
                    check('CIN-02','电影结束恢复仍有效的锁定CameraMode',state.get('lock_restored',False))
                    check('RES-05','大范围世界快照Box只结算一次实际伤害且大招不回能',state['box_prepared'] and state['box_drops']==1 and remote['energy']==0.,state['box_drops'])
                    inject('lock',True)
                    enemies[0].set_actor_location(unreal.Vector(2300,2300,100),False,True)
                state['index'] += 1
                state.update(phase='ready', at=now)
    except Exception:
        finish(traceback.format_exc())


state['callback'] = unreal.register_slate_post_tick_callback(tick)
