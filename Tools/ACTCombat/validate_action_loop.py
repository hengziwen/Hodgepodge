"""用正式 Enhanced Input 验证五段、移动取消、闪避和闪避后普攻闭环，不保存资产。"""
from pathlib import Path
import builtins
import json
import math
import re
import sys
import time
import traceback
import unreal

sys.path.insert(0, str(Path(unreal.Paths.project_dir(), 'Tools/HitReactionDummy')))
import control

editor = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
play = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.LevelEditorPlaySettings'))
perf = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.EditorPerformanceSettings'))
previous = {n: play.get_editor_property(n) for n in ['PlayNumberOfClients', 'RunUnderOneProcess']}
try:
    mode = int(play.get_editor_property('PlayNetMode').value)
except TypeError as error:
    mode = int(re.search(r"Cannot pythonize '(\d+)'", str(error)).group(1))
throttle = perf.get_editor_property('bThrottleCPUWhenNotForeground')
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(1, False)
perf.set_editor_property('bThrottleCPUWhenNotForeground', False)
assert unreal.EditorLoadingAndSavingUtils.load_map('/Game/Tests/Combat/L_HitReactionDummy')
state = {'boot': time.monotonic(), 'stage': 'boot', 'sequence': [], 'last_press': -1., 'samples': [], 'hit_ids': set(), 'damage_events': []}
report = {'error': None, 'checks': [], 'input': 'Enhanced Input injection; not physical keyboard'}
builtins.HODGE_ACT_ACTION_SCOPE = globals()


def validate_combo_poise_window(ordinary, heavy_threshold):
    """首次Heavy前不能漏报达到阈值的普通刀；消费Heavy后仍保留同轮不重复触发语义。"""
    assert ordinary and math.isfinite(heavy_threshold) and heavy_threshold > 0, '削韧验收缺少有效普通命中或阈值'
    assert all(math.isfinite(event['poise']) and event['poise'] >= 0 for event in ordinary), ordinary
    heavy_events = [event for event in ordinary if 'HEAVY' in event['level']]
    first_heavy = next((i for i, event in enumerate(ordinary) if 'HEAVY' in event['level']), len(ordinary))
    assert all(event['poise'] < heavy_threshold for event in ordinary[:first_heavy]), ordinary
    assert all(event['poise'] >= heavy_threshold and any('KNOCKBACK' in impact for impact in event['impacts']) for event in heavy_events), heavy_events
    return bool(heavy_events)


def actions():
    """读取实际能力实例，不用输入已发送替代动作已发生。"""
    active = []
    for handle in state['asc'].get_all_abilities():
        ga = state['lib'].call_method('GetGameplayAbilityFromSpecHandle', (state['asc'], handle))[0]
        if ga and state['lib'].call_method('IsGameplayAbilityActive', (ga,)):
            data = state['asc'].find_ability_definition(handle)
            if isinstance(ga, unreal.HodgeGameplayAbility_Dash) and ga.has_committed_movement():
                active.append('Dodge')
            elif data and data.get_editor_property('bAllowHitRecoveryAttack'):
                observed = unreal.HodgeCombatValidationLibrary.inspect_montage_state(state['source'], data.get_editor_property('ExecutionConfig').montage)
                if observed.instance_id >= 0 and observed.weight > .001 and not observed.stopped:
                    active.append(data.get_name())
    return active


def press(action):
    state['input'].inject_input_vector_for_action(action, unreal.Vector(1, 0, 0), [], [])


def finish(error=None):
    """报告中保留失败采样，世界销毁后才恢复偏好与回调。"""
    if state['stage'] == 'closing':
        return
    report.update(error=error, sequence=state['sequence'], samples=state['samples'], damage_events=state['damage_events'])
    # 后续素材批次可独立归档，不覆盖之前动作闭环的历史报告。
    output = Path(unreal.Paths.project_saved_dir()) / globals().get('ACTION_REPORT_DIR', 'ACTCombat')
    output.mkdir(parents=True, exist_ok=True)
    (output / 'action-loop.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
    state['stage'] = 'closing'
    control.stop()
    editor.editor_request_end_play()


def tick(delta):
    """按实际动作与加速度推进用例，所有等待均有全局期限。"""
    try:
        worlds = unreal.EditorLevelLibrary.get_pie_worlds(True)
        if state['stage'] == 'closing':
            if worlds:
                return
            assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(1 if mode == 0 else 2, mode == 2)
            for key, value in previous.items():
                play.set_editor_property(key, value)
            perf.set_editor_property('bThrottleCPUWhenNotForeground', throttle)
            unreal.unregister_slate_post_tick_callback(state['callback'])
            print('ACT_ACTION_LOOP_CLEANUP_OK')
            return
        assert time.monotonic() - state['boot'] < 45, '动作闭环超时'
        if len(worlds) != 1:
            return
        world = worlds[0]
        if state['stage'] == 'boot':
            source = unreal.GameplayStatics.get_player_pawn(world, 0)
            if not isinstance(source, unreal.HodgeHeroCharacter) or not source.get_hodge_ability_system_component():
                return
            target_cls = unreal.load_asset(control.ROOT + '/BP_HitReactionDummy').generated_class()
            targets = unreal.GameplayStatics.get_all_actors_of_class(world, target_cls)
            if len(targets) != 1 or not targets[0].initialize_enemy_ability_system():
                return
            control.start()
            control.reset()
            pc = source.get_controller()
            input_sub = unreal.get_default_object(unreal.load_class(None, '/Script/Engine.SubsystemBlueprintLibrary')).call_method(
                'GetLocalPlayerSubSystemFromPlayerController', (pc, unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
            state.update(source=source, asc=source.get_hodge_ability_system_component(), input=input_sub,
                         lib=unreal.get_default_object(unreal.AbilitySystemLibrary),
                         attack=unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack'),
                         dodge=unreal.load_asset('/Game/Main/Input/InputAction/IA_Sprint'),
                         lock_action=unreal.load_asset('/Game/Main/Input/InputAction/IA_LockOn'),
                         lock_component=source.get_component_by_class(unreal.HodgeLockOnComponent),
                         move=unreal.load_asset('/Game/Main/Input/InputAction/IA_Move'),
                         stage='lock', lock_pressed=False, began=unreal.GameplayStatics.get_time_seconds(world))
        now = unreal.GameplayStatics.get_time_seconds(world)
        active = actions()
        # 将每个真实结算与当时正在执行的正式动作关联，不把能力已起播当作每刀已命中。
        result = control._state['reaction'].get_last_result()
        hit_id = result.hit_id.export_text()
        if result.actual_damage > 0 and hit_id not in state['hit_ids']:
            state['hit_ids'].add(hit_id)
            state['damage_events'].append({'time': now, 'id': hit_id, 'actions': list(active), 'damage': result.actual_damage,
                                           'level': str(result.reaction_level), 'poise': result.accumulated_poise, 'outcome': str(result.outcome),
                                           'impacts': [str(impact) for impact in result.applied_impacts]})
        state['samples'].append({'time': now, 'stage': state['stage'], 'actions': active,
                                 'acceleration': state['source'].character_movement.get_current_acceleration().length(),
                                 'source': list(state['source'].get_actor_location().to_tuple()),
                                 'target': list(control._state['dummy'].get_actor_location().to_tuple()),
                                 'source_yaw': state['source'].get_actor_rotation().yaw})
        if state['stage'] == 'lock':
            # 正式锁定维持每刀起手朝向；不瞬移目标或伪造每段命中来补偿自由攻击的越位。
            assert now - state['began'] < 5, '正式锁定未确认目标'
            if state['lock_component'].is_ready() and not state['lock_pressed']:
                press(state['lock_action'])
                state['lock_pressed'] = True
            if state['lock_component'].get_confirmed_target() == control._state['dummy']:
                report['checks'].append('Real lock-on confirmed before combo')
                state.update(stage='combo', began=now)
        elif state['stage'] == 'combo':
            for name in active:
                if name.startswith('DA_Attack_') and (not state['sequence'] or state['sequence'][-1] != name):
                    state['sequence'].append(name)
            if len(state['sequence']) == 5:
                assert state['sequence'] == ['DA_Attack_1', 'DA_Attack_2', 'DA_Attack_3', 'DA_Attack_5', 'DA_Attack_4'], state['sequence']
                report['checks'].append('Formal five-hit order')
                state.update(stage='combo_finish', began=now)
            elif now - state['began'] > .5 and now - state['last_press'] > .16:
                press(state['attack'])
                state['last_press'] = now
        elif state['stage'] == 'combo_finish':
            assert now - state['began'] < 3, '终结刀没有在期限内产生真实伤害'
            hit_actions = {name for event in state['damage_events'] for name in event['actions']}
            if 'DA_Attack_4' in hit_actions:
                expected = {'DA_Attack_' + str(index) for index in [1, 2, 3, 5, 4]}
                assert expected.issubset(hit_actions), ('五段动作中存在未命中段', sorted(hit_actions))
                report['checks'].append('Every formal combo step caused real damage')
                levels = [event['level'] for event in state['damage_events']]
                assert any('LIGHT' in value for value in levels) and not any('BREAK' in value for value in levels), levels
                # 五段按目标当前滑动窗口验收；到期削韧不能为展示Heavy而延长或补回。
                pawn_data = control._state['dummy'].get_editor_property('EnemyPawnData')
                poise = pawn_data.get_editor_property('Poise')
                heavy_threshold = poise.get_editor_property('HeavyThreshold')
                ordinary = [event for event in state['damage_events'] if 'DA_Attack_4' not in event['actions']]
                heavy_observed = validate_combo_poise_window(ordinary, heavy_threshold)
                report['poise_window_acceptance'] = {'pawn_data': pawn_data.get_path_name(), 'window': poise.get_editor_property('Window'),
                    'heavy_threshold': heavy_threshold, 'heavy_observed': heavy_observed,
                    'heavy_coverage': 'Separate deterministic HeavyR/HeavyL runtime cases; not guaranteed by every five-hit window',
                    'ordinary_events': ordinary}
                # 终结刀按自己的分段开关击飞，前四刀仍只消费轻重阈值。
                finishers = [event for event in state['damage_events'] if 'DA_Attack_4' in event['actions']]
                assert any(any('LAUNCH' in impact for impact in event['impacts']) for event in finishers), finishers
                assert not any('LAUNCH' in impact for event in state['damage_events'] if 'DA_Attack_4' not in event['actions'] for impact in event['impacts'])
                report['checks'].append('Poise window levels valid and only flagged finisher launches')
                state.update(stage='move', moved_at=None)
        elif state['stage'] == 'move':
            state['input'].inject_input_vector_for_action(state['move'], unreal.Vector(0, 1, 0), [], [])
            if not active and state['source'].character_movement.get_current_acceleration().length() > 1:
                report['checks'].append('Movement exits attack')
                press(state['dodge'])
                state.update(stage='dodge', began=now)
        elif state['stage'] == 'dodge':
            assert now - state['began'] < 3, '闪避未真正提交'
            if 'Dodge' in active:
                report['checks'].append('Directional dodge committed')
                state.update(stage='after_dodge', began=now)
        elif state['stage'] == 'after_dodge':
            if 'Dodge' not in active and now - state['began'] > .25:
                press(state['attack'])
                state.update(stage='attack_return', began=now)
        elif state['stage'] == 'attack_return':
            assert now - state['began'] < 3, '闪避后普攻未恢复'
            if any(name.startswith('DA_Attack_') for name in active):
                assert control._state['health'].get_health() < 1000000, '五段未造成真实伤害'
                report['checks'].append('Basic attack returns after dodge')
                finish()
    except Exception:
        finish(traceback.format_exc())


state['callback'] = unreal.register_slate_post_tick_callback(tick)
editor.editor_request_begin_play()
print('ACT_ACTION_LOOP_STARTED')
