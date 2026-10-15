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
bridge = unreal.get_default_object(unreal.load_class(None, '/Script/GameplayAbilities.AbilitySystemBlueprintLibrary'))
profile = unreal.load_asset('/Game/Main/Character/Hero/Wing/DA_PoverWing_Anzhu')
montage = unreal.load_asset('/Game/Main/Character/Hero/Anim/Montages/AM_Attack04_Montage')
configs = [e.notify_state_class.hit for e in unreal.AnimationLibrary.get_animation_notify_events(montage)
           if isinstance(e.notify_state_class, unreal.HodgeAnimNotifyState_HitCheck)]
wing_config = next(c for c in configs if c.source == unreal.HodgeAnimHitSource.NAMED_COMPONENT and str(c.component_name) == 'WingMesh')
hand_config = next((c for c in configs if c.source == unreal.HodgeAnimHitSource.CHARACTER_MESH_SOCKET), None)
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
state['requested_instances']=set()
next_attack=unreal.GameplayTag();next_attack.import_text('(TagName="Status.Attack.Cancel.NextAttack")')

def request_next(now):
    """仅在当前段连段许可窗口请求下一段，保留真实输入与服务器授权。"""
    active=local.get_component_by_class(unreal.SkeletalMeshComponent).get_anim_instance().get_current_active_montage()
    info=unreal.HodgeCombatValidationLibrary.inspect_montage_state(local,active) if active else None
    idle=not info or info.instance_id<0 or info.stopped
    ready=not idle and local.get_hodge_ability_system_component().has_matching_gameplay_tag(next_attack) and info.instance_id not in state['requested_instances']
    if (idle and now-state['last_input']>.6) or ready:
        inputs.inject_input_vector_for_action(attack,unreal.Vector(1,0,0),[],[])
        state['last_input']=now
        if ready:state['requested_instances'].add(info.instance_id)
# 稳定出生点及攻击方向，测试仅隔离翼盒，不依赖地图玩家出生旋转。
for pawn in {local,hero}:
    pawn.set_actor_location(unreal.Vector(1000,1000,100),False,True)
    pawn.set_actor_rotation(unreal.Rotator(),False)
    if pawn.get_controller():pawn.get_controller().set_control_rotation(unreal.Rotator())

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
    hand = body.get_socket_location(hand_config.bone_or_socket) if hand_config else hero.get_actor_location()
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
        inputs.inject_input_vector_for_action(attack, unreal.Vector(), [], [])
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
                hand_now = body.get_socket_location(hand_config.bone_or_socket) if hand_config else hero.get_actor_location()
                if cal['begin'] <= main.position <= cal['end'] + .05 and state['phase'] == 'inside':
                    state['minimum_hand_distance'] = min(state['minimum_hand_distance'], (previous - hand_now).length())
                hand_distance = place_in_wing_box()
            if cal['begin'] + .02 < main.position < cal['end']:
                pose = wing.get_presentation_state()
                if pose.pose_evaluations > 0 and abs(pose.source_time - main.position) < .03:
                    state['prepared'] = True
            if state['phase'] == 'cancel' and .33 < main.position < cal['begin'] - .1 and not state['cancel_sent']:
                # 此项隔离技能结束清理；真实 Dash 输入已由表现 E2E 单独验证。
                asc=hero.get_hodge_ability_system_component()
                handles=[handle for handle in asc.get_all_abilities() if asc.find_ability_definition(handle) and asc.find_ability_definition(handle).execution_config.montage==montage]
                handle=next(handle for handle in handles if bridge.call_method('IsGameplayAbilityActive',(bridge.call_method('GetGameplayAbilityFromSpecHandle',(asc,handle))[0],)))
                assert unreal.HodgeCombatValidationLibrary.queue_ability_action(asc,handle,True)
                state['cancel_sent'] = True
                state['wait_at'] = now
        else:
            if state['seen'] and state['wait_at'] is None:
                state['wait_at'] = now
            elif not state['seen']:request_next(now)
        samples.append({'time': now, 'phase': state['phase'], 'source': main.position, 'instance': main.instance_id,
                        'health': current_hp, 'drops': state['drops'], 'hand_distance': hand_distance,
                        'authority_source': wing.get_presentation_state().source_time,
                        'authority_evaluations': wing.get_presentation_state().pose_evaluations})
        if state['seen'] and main.instance_id < 0 and state['wait_at'] is not None and now - state['wait_at'] > 1.0:
            if state['phase'] == 'inside':
                check('翼盒单独造成伤害', current_hp < state['base_hp'], [state['base_hp'], current_hp])
                check('多帧扫掠每目标本刀一次', state['drops'] == 1, state['drops'])
                if hand_config:
                    check('目标避开手部球判定', state['minimum_hand_distance'] > hand_config.radius + 6.0, state['minimum_hand_distance'])
                else:
                    # 当前用户已移除手部通知，单一翼来源就是直接的伤害隔离证据。
                    check('本段只有翼 Box 伤害来源', len(configs)==1)
                check('服务器翼骨骼按当前主动画准备', state['prepared'])
                state['phase'] = 'outside'
                target.set_actor_location(unreal.Vector(5000, 5000, 300), False, True)
            elif state['phase'] == 'outside':
                check('范围外目标不受伤', abs(current_hp - state['base_hp']) < .001)
                state['phase'] = 'cancel'
            else:
                check('伤害窗口前请求服务器技能取消', state['cancel_sent'])
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
