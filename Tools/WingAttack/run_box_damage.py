"""用真实五段输入验证翼面单独命中、范围外不命中、去重和取消后无伤害。"""
import json
import time
import traceback
from pathlib import Path
import unreal

WORK = Path(globals().get('WING_WORK', 'E:/Project/Git/Hodgepodge/Saved/WingBox'))
mode = globals().get('WING_MODE', 'single')
worlds = unreal.EditorLevelLibrary.get_pie_worlds(True)
server = next(w for w in worlds if unreal.SystemLibrary.is_server(w))
clients = [w for w in worlds if w != server]
local = next(a for a in unreal.GameplayStatics.get_all_actors_of_class(clients[0] if clients else server, unreal.HodgeHeroCharacter)
             if not clients or a.get_local_role() == unreal.NetRole.ROLE_AUTONOMOUS_PROXY)
player_id = local.get_editor_property('PlayerState').get_editor_property('PlayerId')
hero = next(a for a in unreal.GameplayStatics.get_all_actors_of_class(server, unreal.HodgeHeroCharacter)
            if a.get_editor_property('PlayerState').get_editor_property('PlayerId') == player_id)
enemies = unreal.GameplayStatics.get_all_actors_of_class(server, unreal.HodgeEnemyCharacter)
target = enemies[0]
for enemy in enemies:
    controller = enemy.get_controller()
    if controller and controller.brain_component:
        controller.stop_movement()
        controller.brain_component.stop_logic('Wing box validation isolation')
    enemy.set_actor_location(unreal.Vector(5000, 5000, 300), False, True)
for w in worlds:
    for actor in unreal.GameplayStatics.get_all_actors_of_class(w, unreal.HodgeHeroCharacter):
        if actor.get_editor_property('PlayerState').get_editor_property('PlayerId') != player_id:
            actor.set_actor_location(unreal.Vector(-5000, -5000, 300), False, True)
# 缩小测试目标用于隔离翼盒与手部球，生产资产的碰撞配置不改变。
capsule = target.get_component_by_class(unreal.CapsuleComponent)
capsule.set_capsule_size(5.0, 6.0, True)
target.character_movement.disable_movement()
lib = unreal.get_default_object(unreal.load_class(None, '/Script/Engine.SubsystemBlueprintLibrary'))
inputs = lib.call_method('GetLocalPlayerSubSystemFromPlayerController', (local.get_controller(), unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
attack = unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack')
dash = unreal.load_asset('/Game/Main/Input/InputAction/IA_Sprint')
profile = unreal.load_asset('/Game/Main/Character/Hero/Wing/DA_PoverWing_Anzhu')
montage = profile.attack_montage
configs = [e.notify_state_class.hit for e in unreal.AnimationLibrary.get_animation_notify_events(montage)
           if isinstance(e.notify_state_class, unreal.HodgeAnimNotifyState_HitCheck)]
wing_config = next(c for c in configs if c.source == unreal.HodgeAnimHitSource.NAMED_COMPONENT and str(c.component_name) == 'WingMesh')
hand_config = next(c for c in configs if c.source == unreal.HodgeAnimHitSource.CHARACTER_MESH_SOCKET)
cal = json.loads((WORK / 'configuration.json').read_text(encoding='utf-8'))
wing = hero.get_component_by_class(unreal.HodgeWingComponent)
body = hero.get_component_by_class(unreal.SkeletalMeshComponent)
assert body != wing.get_wing_mesh()
health = target.get_component_by_class(unreal.HodgeHealthComponent)
checks = []
samples = []
state = {'wall': time.monotonic(), 'phase': 'inside', 'seen': False, 'last_input': -100.0,
         'base_hp': health.get_health(), 'last_hp': health.get_health(), 'drops': 0, 'minimum_hand_distance': 1.e10,
         'cancel_sent': False, 'wait_at': None, 'prepared': False}

def check(name, passed, detail=None):
    checks.append({'requirement': 'WING-HIT-03', 'check': name, 'passed': bool(passed), 'detail': detail})
    assert passed, (name, detail)

def finish(error=None):
    unreal.unregister_slate_post_tick_callback(state['callback'])
    result = {'mode': mode, 'passed': not error and all(c['passed'] for c in checks), 'checks': checks,
              'samples': samples, 'error': error, 'isolated_target_capsule': [5.0, 6.0]}
    (WORK / (mode + '-damage.json')).write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
    print(json.dumps({'mode': mode, 'damage_passed': result['passed'], 'checks': len(checks), 'error': error}, ensure_ascii=False))

def place_in_wing_box():
    mesh = wing.get_wing_mesh()
    anchor = mesh.get_socket_transform(wing_config.bone_or_socket, unreal.RelativeTransformSpace.RTS_WORLD)
    anchor.set_editor_property('scale3d', unreal.Vector(1, 1, 1))
    hand = body.get_socket_location(hand_config.bone_or_socket)
    center = wing_config.local_transform.translation
    extent = wing_config.box_half_extent
    points = [unreal.MathLibrary.transform_location(anchor, center + unreal.Vector(extent.x * x, extent.y * y, extent.z * z))
              for x in [-.8, 0, .8] for y in [-.6, .6] for z in [-.6, 0, .6]]
    point = max(points, key=lambda p: (p - hand).length())
    target.set_actor_location(point, False, True)
    return (point - hand).length()

def tick(delta):
    try:
        assert time.monotonic() - state['wall'] < 150, '翼盒伤害测试超时'
        now = unreal.GameplayStatics.get_time_seconds(server)
        main = unreal.HodgeCombatValidationLibrary.inspect_montage_state(hero, montage)
        current_hp = health.get_health()
        if current_hp < state['last_hp'] - .001:
            state['drops'] += 1
        state['last_hp'] = current_hp
        hand_distance = None
        if main.instance_id >= 0:
            state['seen'] = True
            if state['phase'] in ['inside', 'cancel']:
                previous = target.get_actor_location()
                hand_now = body.get_socket_location(hand_config.bone_or_socket)
                if cal['begin'] <= main.position <= cal['end'] + .05 and state['phase'] == 'inside':
                    state['minimum_hand_distance'] = min(state['minimum_hand_distance'], (previous - hand_now).length())
                hand_distance = place_in_wing_box()
            if cal['begin'] + .02 < main.position < cal['end']:
                pose = wing.get_presentation_state()
                if pose.pose_evaluations > 0 and abs(pose.source_time - main.position) < .03:
                    state['prepared'] = True
            if state['phase'] == 'cancel' and .2 < main.position < cal['begin'] - .1 and not state['cancel_sent']:
                inputs.inject_input_vector_for_action(dash, unreal.Vector(1, 0, 0), [], [])
                state['cancel_sent'] = True
                state['wait_at'] = now
        else:
            if state['seen'] and state['wait_at'] is None:
                state['wait_at'] = now
            elif not state['seen'] and now - state['last_input'] > .12:
                inputs.inject_input_vector_for_action(attack, unreal.Vector(1, 0, 0), [], [])
                state['last_input'] = now
        samples.append({'time': now, 'phase': state['phase'], 'source': main.position, 'instance': main.instance_id,
                        'health': current_hp, 'drops': state['drops'], 'hand_distance': hand_distance,
                        'authority_source': wing.get_presentation_state().source_time,
                        'authority_evaluations': wing.get_presentation_state().pose_evaluations})
        if state['seen'] and main.instance_id < 0 and state['wait_at'] is not None and now - state['wait_at'] > 1.0:
            if state['phase'] == 'inside':
                check('翼盒单独造成伤害', current_hp < state['base_hp'], [state['base_hp'], current_hp])
                check('多帧扫掠每目标本刀一次', state['drops'] == 1, state['drops'])
                check('目标避开手部球判定', state['minimum_hand_distance'] > hand_config.radius + 6.0, state['minimum_hand_distance'])
                check('服务器翼骨骼按当前主动画准备', state['prepared'])
                state['phase'] = 'outside'
                target.set_actor_location(unreal.Vector(5000, 5000, 300), False, True)
            elif state['phase'] == 'outside':
                check('范围外目标不受伤', abs(current_hp - state['base_hp']) < .001)
                state['phase'] = 'cancel'
            else:
                check('伤害窗口之前成功取消', state['cancel_sent'])
                check('取消及淡出期间不产生伤害', abs(current_hp - state['base_hp']) < .001)
                if mode == 'dedicated':
                    check('专服采样翼网格不渲染', not wing.get_wing_mesh().visible and wing.get_wing_mesh().hidden_in_game)
                for index, client in enumerate(clients):
                    remote = next(e for e in unreal.GameplayStatics.get_all_actors_of_class(client, unreal.HodgeEnemyCharacter)
                                  if e.get_editor_property('EnemyPawnData') == target.get_editor_property('EnemyPawnData'))
                    check('客户端生命值复制_' + str(index), abs(remote.get_component_by_class(unreal.HodgeHealthComponent).get_health() - current_hp) < .001)
                finish()
                return
            state.update({'seen': False, 'wait_at': None, 'last_input': now, 'base_hp': current_hp,
                          'last_hp': current_hp, 'drops': 0})
    except Exception:
        finish(traceback.format_exc())

state['callback'] = unreal.register_slate_post_tick_callback(tick)
print('启动真实翼盒伤害测试：' + mode)
