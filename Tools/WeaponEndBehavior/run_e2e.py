"""通过真实攻击输入验证两种通知结束方式、取消与再次攻击的多端表现。"""
import json
import time
import traceback
from pathlib import Path
import unreal

WORK = Path('E:/Project/Git/Hodgepodge/Saved/WeaponEndBehavior')
mode = globals().get('WING_MODE', 'single')
worlds = unreal.EditorLevelLibrary.get_pie_worlds(True)
server = next(w for w in worlds if unreal.SystemLibrary.is_server(w))
clients = [w for w in worlds if w != server]
local = next(a for a in unreal.GameplayStatics.get_all_actors_of_class(clients[0] if clients else server, unreal.HodgeHeroCharacter)
             if not clients or a.get_local_role() == unreal.NetRole.ROLE_AUTONOMOUS_PROXY)
player_id = local.get_editor_property('PlayerState').get_editor_property('PlayerId')
peers = [a for w in worlds for a in unreal.GameplayStatics.get_all_actors_of_class(w, unreal.HodgeHeroCharacter)
         if a.get_editor_property('PlayerState').get_editor_property('PlayerId') == player_id]
for w in worlds:
    for actor in unreal.GameplayStatics.get_all_actors_of_class(w, unreal.HodgeHeroCharacter):
        if actor not in peers:
            actor.set_actor_location(unreal.Vector(-2000, -2000, 100), False, True)
lib = unreal.get_default_object(unreal.load_class(None, '/Script/Engine.SubsystemBlueprintLibrary'))
inputs = lib.call_method('GetLocalPlayerSubSystemFromPlayerController', (local.get_controller(), unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
attack = unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack')
dash = unreal.load_asset('/Game/Main/Input/InputAction/IA_Sprint')
montages = [unreal.load_asset('/Game/Main/Character/Hero/Anim/Montages/AM_Attack0' + str(i) + '_Montage') for i in range(1, 6)]
notifications = [e.notify_state_class for m in montages for e in unreal.AnimationLibrary.get_animation_notify_events(m)
                 if isinstance(e.notify_state_class, unreal.HodgeAnimNotifyState_WeaponHand)]
original = [(n, n.end_behavior) for n in notifications]
BACK = unreal.HodgeWeaponHandEndBehavior.RETURN_TO_BACK
DIRECT = unreal.HodgeWeaponHandEndBehavior.FADE_AT_HAND
PHASE = unreal.HodgeWeaponPresentationPhase
cases = ['back', 'direct', 'interrupt', 'cancel']
checks = []
samples = []
state = {'wall': time.monotonic(), 'case': -1, 'started': False, 'last_progress': 0.0, 'cancel_sent': False,
         'second_sent': False, 'second_hand': False, 'completed_at': None, 'ready': False}

def check(name, passed, detail=None):
    checks.append({'check': name, 'passed': bool(passed), 'detail': detail})
    assert passed, (name, detail)

def finish(error=None):
    unreal.unregister_slate_post_tick_callback(state['callback'])
    for notify, value in original:
        notify.set_editor_property('end_behavior', value)
    result = {'mode': mode, 'passed': not error and all(c['passed'] for c in checks), 'checks': checks,
              'samples': samples, 'error': error, 'original_modes': [str(v) for _, v in original]}
    (WORK / (mode + '-e2e.json')).write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
    print(json.dumps({'mode': mode, 'passed': result['passed'], 'checks': len(checks), 'error': error}, ensure_ascii=False))

def start_case(index, now):
    state.update({'case': index, 'started': False, 'case_at': now, 'cancel_sent': False, 'second_sent': False,
                  'second_hand': False, 'completed_at': None, 'phases': [set() for _ in peers],
                  'midpoints': set(), 'end_modes': set(), 'hand_seen': set()})
    for notify, _ in original:
        notify.set_editor_property('end_behavior', BACK if index == 0 else DIRECT)
    inputs.inject_input_vector_for_action(attack, unreal.Vector(1, 0, 0), [], [])

def tick(delta):
    try:
        assert time.monotonic() - state['wall'] < 150, '武器结束方式端到端超时'
        now = unreal.GameplayStatics.get_time_seconds(server)
        if not state['ready']:
            items = []
            for peer in peers:
                manager = peer.get_component_by_class(unreal.HodgeEquipmentManagerComponent)
                instances = manager.get_equipment_instances_of_type(unreal.HodgeWeaponInstance) if manager else []
                if not instances:
                    return
                weapon = instances[0]
                actors = [a for a in weapon.get_spawned_actors() if isinstance(a, unreal.HodgeWeaponPresentationActor)]
                if not actors or actors[0].get_visual_mesh().get_num_materials() == 0:
                    return
                items.append((weapon, actors[0]))
            state['items'] = items
            state['ready'] = True
            check('原有通知默认回背', all(value == BACK for _, value in original))
            start_case(0, now)
            return
        case = cases[state['case']]
        rows = []
        for index, (weapon, actor) in enumerate(state['items']):
            pose = weapon.get_presentation_state()
            phase = actor.get_visual_phase()
            amount = actor.get_visibility_amount()
            is_server_only = mode == 'dedicated' and peers[index].get_world() == server
            if is_server_only:
                phase = pose.phase
            rows.append({'peer': index, 'phase': str(phase), 'end_behavior': str(pose.end_behavior),
                         'amount': amount, 'requests': weapon.get_hand_use_count(), 'revision': pose.revision})
            state['phases'][index].add(phase)
            if phase == PHASE.HAND:
                state['hand_seen'].add(index)
            if phase == PHASE.FADING and case != 'back':
                assert pose.end_behavior == DIRECT, '释放方式未按通知复制'
                state['end_modes'].add(index)
                if not is_server_only:
                    assert actor.get_visual_mesh().get_attach_parent() == actor.get_detection_mesh(), '直接淡出错误挂回背部'
                    if 0.05 < amount < .95:
                        state['midpoints'].add(index)
                        profile = weapon.get_presentation_profile()
                        values = [actor.get_visual_mesh().get_material(i).get_scalar_parameter_value(profile.visibility_parameter)
                                  for i in range(actor.get_visual_mesh().get_num_materials())]
                        assert all(abs(v - amount) < .001 for v in values), '实际材质透明度不同步'
        samples.append({'time': now, 'case': case, 'peers': rows})
        local_index = peers.index(local)
        local_weapon, local_actor = state['items'][local_index]
        if rows[local_index]['phase'] == str(PHASE.HAND):
            state['started'] = True
            if state['second_sent']:
                state['second_hand'] = True
        if case == 'interrupt' and local_actor.get_visual_phase() == PHASE.FADING and not state['second_sent']:
            state['second_sent'] = True
            inputs.inject_input_vector_for_action(attack, unreal.Vector(1, 0, 0), [], [])
        if case == 'cancel' and state['started'] and now - state['case_at'] > .2 and not state['cancel_sent']:
            state['cancel_sent'] = True
            inputs.inject_input_vector_for_action(dash, unreal.Vector(1, 0, 0), [], [])
        hidden = all((weapon.get_presentation_state().phase if mode == 'dedicated' and peer.get_world() == server
                      else actor.get_visual_phase()) == PHASE.HIDDEN
                     for peer, (weapon, actor) in zip(peers, state['items']))
        if state['started'] and hidden:
            state['completed_at'] = state['completed_at'] or now
            if now - state['completed_at'] < .25:
                return
            visible_peers = {i for i, p in enumerate(peers) if not (mode == 'dedicated' and p.get_world() == server)}
            check(case + '_各端经历手持', state['hand_seen'] == set(range(len(peers))))
            if case == 'back':
                check('默认回背悬浮路径保留', all(PHASE.HOVERING in phases for phases in state['phases']))
            else:
                check(case + '_直接方式各端一致', state['end_modes'] == set(range(len(peers))))
                check(case + '_跳过回背和悬浮', all(PHASE.RETURNING not in p and PHASE.HOVERING not in p for p in state['phases']))
                check(case + '_表现端材质渐变', state['midpoints'] == visible_peers)
            if case == 'interrupt':
                check('新攻击覆盖未结束的消隐', state['second_sent'] and state['second_hand'])
            if case == 'cancel':
                check('真实Dash取消执行', state['cancel_sent'])
            check(case + '_最终无手持请求', all(w.get_hand_use_count() == 0 for w, _ in state['items']))
            if state['case'] == len(cases) - 1:
                finish()
            else:
                start_case(state['case'] + 1, now)
        elif not hidden:
            state['completed_at'] = None
        if now - state['case_at'] > 35:
            raise RuntimeError('当前用例没有完成：' + case)
    except Exception:
        finish(traceback.format_exc())

state['callback'] = unreal.register_slate_post_tick_callback(tick)
print('启动武器通知结束方式测试：' + mode)
