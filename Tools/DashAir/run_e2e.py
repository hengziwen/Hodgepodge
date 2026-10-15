"""真实 Enhanced Input 和临时碰撞平台验证 Dash 离地、重力、落地衔接与空中拒绝。"""
import gc
import json
import math
import time
import traceback
from pathlib import Path
import unreal

WORK = Path(globals()['LOCK_ON_WORK'])
mode = globals()['LOCK_ON_MODE']
worlds = unreal.EditorLevelLibrary.get_pie_worlds(True)
server = next(world for world in worlds if unreal.SystemLibrary.is_server(world))
clients = [world for world in worlds if world != server]
owner = next(pawn for pawn in unreal.GameplayStatics.get_all_actors_of_class(clients[0] if clients else server, unreal.HodgeHeroCharacter)
             if not clients or pawn.get_local_role() == unreal.NetRole.ROLE_AUTONOMOUS_PROXY)
pid = owner.get_editor_property('PlayerState').get_editor_property('PlayerId')
peers = [pawn for world in worlds for pawn in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.HodgeHeroCharacter)
         if pawn.get_editor_property('PlayerState').get_editor_property('PlayerId') == pid]
authority = next(pawn for pawn in peers if pawn.get_world() == server)
bridge = unreal.get_default_object(unreal.load_class(None, '/Script/GameplayAbilities.AbilitySystemBlueprintLibrary'))
inputs = unreal.get_default_object(unreal.load_class(None, '/Script/Engine.SubsystemBlueprintLibrary')).call_method(
    'GetLocalPlayerSubSystemFromPlayerController', (owner.get_controller(), unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
dash_input = unreal.load_asset('/Game/Main/Input/InputAction/IA_Sprint')
move_input = unreal.load_asset('/Game/Main/Input/InputAction/IA_Move')
profile = authority.get_component_by_class(unreal.HodgeLocomotionPolicyComponent).get_profile()
duration = profile.get_editor_property('Duration')
distance = profile.get_editor_property('Distance')
montage = profile.get_editor_property('ForwardMontage')
cube = unreal.load_asset('/Engine/BasicShapes/Cube')
checks, samples, platforms = [], [], []
cases = ['flat_tap', 'edge_early', 'edge_late', 'land_handoff', 'late_land_no_handoff', 'air_start_rejected']
state = {'case': -1, 'stage': 0, 'wall': time.monotonic(), 'at': unreal.GameplayStatics.get_time_seconds(server)}

for world in worlds:
    if mode == 'listen-lag':
        unreal.SystemLibrary.execute_console_command(world, 'Net PktLag=100')
    for pawn in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.HodgeHeroCharacter):
        if pawn not in peers:
            pawn.get_component_by_class(unreal.CharacterMovementComponent).disable_movement()
    for enemy in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.HodgeEnemyCharacter):
        enemy.get_component_by_class(unreal.CharacterMovementComponent).disable_movement()
        if enemy.get_controller() and enemy.get_controller().brain_component:
            enemy.get_controller().brain_component.stop_logic('DashAir test isolation')

def inject(action, pressed=False):
    """每帧注入输入值，复用项目正常 Started／Completed 流程。"""
    inputs.inject_input_vector_for_action(action, unreal.Vector(1, 0, 0) if pressed and action == dash_input else unreal.Vector(0, 1, 0) if pressed else unreal.Vector(), [], [])

def check(name, condition, detail=None):
    """失败立即停止，保留失败前的完整连续采样。"""
    checks.append({'check': name, 'passed': bool(condition), 'detail': detail})
    assert condition, (name, detail)

def snapshot(pawn):
    """读取实际 GA、CMC 和动画状态，排除已结束实例的残留字段。"""
    movement = pawn.get_component_by_class(unreal.CharacterMovementComponent)
    asc = pawn.get_hodge_ability_system_component()
    active = committed = False
    for handle in asc.get_all_abilities():
        ability = bridge.call_method('GetGameplayAbilityFromSpecHandle', (asc, handle))[0]
        if ability and isinstance(ability, unreal.HodgeGameplayAbility_Dash) and bridge.call_method('IsGameplayAbilityActive', (ability,)):
            active = True
            committed = ability.has_committed_movement()
    info = unreal.HodgeCombatValidationLibrary.inspect_montage_state(pawn, montage)
    position, velocity = pawn.get_actor_location(), movement.velocity
    return {'x': position.x, 'z': position.z, 'vx': velocity.x, 'vz': velocity.z,
            'mode': str(movement.movement_mode), 'falling': movement.is_falling(),
            'dash': active, 'committed': committed, 'montage': info.position, 'stopped': info.stopped,
            'sprint': pawn.get_component_by_class(unreal.HodgeLocomotionPolicyComponent).get_resolved_policy().sprinting}

def create_platform(world, top, half_length):
    """各 PIE 世界建立相同碰撞夹具，移动复制仍使用真实服务器链路。"""
    transform = unreal.Transform(location=unreal.Vector(1000, 1000, top - 20), scale=unreal.Vector(half_length / 50, 10, .4))
    # 内部蓝图生成入口未暴露 Python 方法名，使用现有 UFunction 反射调用。
    statics = unreal.get_default_object(unreal.GameplayStatics)
    actor = statics.call_method('BeginDeferredActorSpawnFromClass', (world, unreal.StaticMeshActor.static_class(), transform,
                                unreal.SpawnActorCollisionHandlingMethod.ALWAYS_SPAWN, None, unreal.SpawnActorScaleMethod.MULTIPLY_WITH_ROOT))
    component = actor.get_component_by_class(unreal.StaticMeshComponent)
    component.set_mobility(unreal.ComponentMobility.MOVABLE)
    component.set_static_mesh(cube)
    component.set_collision_profile_name('BlockAll')
    statics.call_method('FinishSpawningActor', (actor, transform, unreal.SpawnActorScaleMethod.MULTIPLY_WITH_ROOT))
    platforms.append(actor)

def prepare(now):
    """定位到平台内侧，再由真实 Dash 运动走出平台，禁止用 Falling 强制模拟台阶。"""
    for actor in platforms:
        actor.destroy_actor()
    platforms.clear()
    name = cases[state['case']]
    top = 70 if name == 'land_handoff' else 700
    half_length = 1500 if name == 'flat_tap' else 150
    radius = owner.get_component_by_class(unreal.CapsuleComponent).get_scaled_capsule_radius()
    start_x = 1000 if name == 'flat_tap' else 1000 + half_length - 70 if name != 'edge_late' else 1000 + half_length + radius - distance * .85
    for world in worlds:
        create_platform(world, top, half_length)
    for pawn in {owner, authority}:
        movement = pawn.get_component_by_class(unreal.CharacterMovementComponent)
        movement.stop_movement_immediately()
        height = pawn.get_component_by_class(unreal.CapsuleComponent).get_scaled_capsule_half_height()
        pawn.set_actor_location(unreal.Vector(start_x, 1000, top + height + 2.5), False, True)
        pawn.set_actor_rotation(unreal.Rotator(), False)
        pawn.get_controller().set_control_rotation(unreal.Rotator())
        movement.set_movement_mode(unreal.MovementMode.MOVE_WALKING)
    state.update({'stage': 1, 'at': now, 'name': name, 'start_x': start_x, 'air_seen': False,
                  'continued': False, 'gravity': False, 'z_reset': False, 'sprint_seen': False,
                  'server_continued': False, 'previous': None, 'dash_at': None, 'end_vz_checked': False})
    state['observer_progress'] = [False for pawn in peers if pawn not in {owner, authority}]

def finish(error=None):
    """测试结束撤销输入和 Python 回调，报告保存在 Saved。"""
    unreal.unregister_slate_post_tick_callback(state['callback'])
    inject(dash_input)
    inject(move_input)
    result = {'mode': mode, 'passed': not error, 'checks': checks, 'samples': samples, 'error': error,
              'profile': {'duration': duration, 'distance': distance}, 'fixture': 'PIE 各世界一致临时碰撞平台，真实能力和运动复制'}
    (WORK / (mode + '-dash-air-e2e.json')).write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
    print(json.dumps({'mode': mode, 'passed': result['passed'], 'checks': len(checks), 'error': error}, ensure_ascii=False))
    for name in ['worlds', 'clients', 'peers', 'owner', 'authority', 'server', 'inputs', 'platforms', 'world', 'pawn', 'enemy']:
        globals()[name] = None
    gc.collect()

def tick(delta):
    """按游戏时间推进测试阶段，所有断言读取实际执行结果。"""
    try:
        assert time.monotonic() - state['wall'] < 150, 'DashAir timeout'
        now = unreal.GameplayStatics.get_time_seconds(server)
        age = now - state['at']
        if state['stage'] == 0:
            if age < 1:
                return
            state['case'] += 1
            if state['case'] == len(cases):
                finish()
                return
            prepare(now)
            return
        name = state['name']
        held = name in ['land_handoff', 'late_land_no_handoff']
        if state['stage'] == 1:
            if age < .6:
                return
            if name == 'air_start_rejected':
                for pawn in {owner, authority}:
                    movement = pawn.get_component_by_class(unreal.CharacterMovementComponent)
                    pawn.set_actor_location(pawn.get_actor_location() + unreal.Vector(0, 0, 180), False, True)
                    movement.set_movement_mode(unreal.MovementMode.MOVE_FALLING)
            inject(move_input, held)
            inject(dash_input, True)
            state.update({'stage': 2, 'at': now})
            return
        inject(dash_input, held)
        inject(move_input, held)
        local = snapshot(owner)
        remote = snapshot(authority)
        observers = [snapshot(pawn) for pawn in peers if pawn not in {owner, authority}]
        samples.append({'case': name, 'time': now, 'owner': local, 'authority': remote, 'observers': observers})
        for index, observed in enumerate(observers):
            state['observer_progress'][index] |= observed['x'] - state['start_x'] >= distance * .7 and observed['montage'] > .05 and not observed['stopped']
        if local['committed'] and state['dash_at'] is None:
            state['dash_at'] = now
            check(name + '_地面起手提交成功', name != 'air_start_rejected')
        dash_age = now - state['dash_at'] if state['dash_at'] is not None else 0
        if local['falling'] and state['dash_at'] is not None:
            state['air_seen'] = True
            state['continued'] |= local['dash'] and local['committed'] and .02 < dash_age < duration
            state['gravity'] |= local['vz'] < -20 and local['dash']
            previous = state['previous']
            if previous and previous['falling'] and previous['vz'] < -80 and local['vz'] > previous['vz'] + 80:
                state['z_reset'] = True
            if dash_age > duration and local['vz'] < -30:
                state['end_vz_checked'] = True
        state['server_continued'] |= remote['falling'] and remote['dash'] and remote['committed']
        state['sprint_seen'] |= local['sprint']
        check_air_sprint = local['sprint'] and local['falling']
        assert not check_air_sprint, (name, '空中不应进入 Sprint')
        state['previous'] = local
        if age < (1.8 if held else .95):
            return
        if name == 'air_start_rejected':
            check(name + '_空中不能新起手', state['dash_at'] is None)
        else:
            check(name + '_位移未因离地被截断', local['x'] - state['start_x'] >= distance * .85, local['x'] - state['start_x'])
            if observers:
                check(name + '_模拟代理位移和Dash动画连续', all(state['observer_progress']))
            if name != 'flat_tap':
                check(name + '_实际走出台阶进入Falling', state['air_seen'])
                check(name + '_离地仍保持已提交Dash', state['continued'])
                check(name + '_服务器也续冲', state['server_continued'])
                check(name + '_Dash中正常下落', state['gravity'])
                check(name + '_无下落速度突然清零', not state['z_reset'])
            if name in ['edge_early', 'edge_late', 'late_land_no_handoff']:
                check(name + '_RMS结束后仍正常下落', state['end_vz_checked'])
            if name == 'land_handoff':
                check(name + '_落地有效窗口内衔接Sprint', state['sprint_seen'])
            if name == 'late_land_no_handoff':
                check(name + '_窗口过期不自动Sprint', not state['sprint_seen'])
        inject(dash_input)
        inject(move_input)
        state.update({'stage': 0, 'at': now})
    except Exception:
        finish(traceback.format_exc())

state['callback'] = unreal.register_slate_post_tick_callback(tick)
