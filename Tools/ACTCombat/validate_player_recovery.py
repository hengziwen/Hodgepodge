"""复用真实双玩家夹具，注入 Enhanced Input 验证受击恢复与动作交接，不保存资产。"""
import builtins
import json
import time
import traceback
from pathlib import Path
import unreal

s = builtins.HODGE_REACTION_TEST
server, owner, observer = s['target'], s['owned_target'], s['observer_target']
world, lib = s['world'], s['lib']
definition = s['definition']
original = definition.get_editor_property('DefaultHitConfig').export_text()
reaction = server.get_component_by_class(unreal.HodgeHitReactionComponent)
owner_combat = owner.get_component_by_class(unreal.HodgeCombatComponentBase)
server_combat = server.get_component_by_class(unreal.HodgeCombatComponentBase)
sub = unreal.get_default_object(unreal.load_class(None, '/Script/Engine.SubsystemBlueprintLibrary')).call_method(
    'GetLocalPlayerSubSystemFromPlayerController', (owner.get_controller(), unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
attack = unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack')
dodge = unreal.load_asset('/Game/Main/Input/InputAction/IA_Sprint')
move = unreal.load_asset('/Game/Main/Input/InputAction/IA_Move')
cases = [('EarlyDodge', .08), ('AllowedDodge', .23), ('EarlyAttack', .08), ('AllowedAttack', .305),
         ('RejectedDodge', .23), ('RejectedAttack', .32), ('Movement', .04)]
# 定向复现使用独立报告名，不能覆盖完整七项回归的证据。
cases = globals().get('RECOVERY_CASES', cases)
state = {'index': -1, 'stage': 'reset', 'after': 0., 'wall': time.monotonic()}
report = {'cases': [], 'samples': [], 'error': None, 'input': 'Enhanced Input injection; not physical keyboard',
          'lag_ms': s.get('lag_ms', 0)}
builtins.HODGE_ACT_RECOVERY_SCOPE = globals()


def asc(pawn):
    """玩家来源沿用 PlayerState ASC，不创建测试 ASC。"""
    return pawn.get_hodge_ability_system_component()


def actions(pawn):
    """只读取实际活跃的闪避和正式普攻，不能用播放某个测试动画冒充。"""
    active = []
    for handle in asc(pawn).get_all_abilities():
        ga = lib.call_method('GetGameplayAbilityFromSpecHandle', (asc(pawn), handle))[0]
        if ga and lib.call_method('IsGameplayAbilityActive', (ga,)):
            data = asc(pawn).find_ability_definition(handle)
            if isinstance(ga, unreal.HodgeGameplayAbility_Dash) and ga.has_committed_movement():
                active.append('Dodge')
            elif data and data.get_editor_property('bAllowHitRecoveryAttack'):
                montage = data.get_editor_property('ExecutionConfig').montage
                observed = unreal.HodgeCombatValidationLibrary.inspect_montage_state(pawn, montage)
                if observed.instance_id >= 0 and observed.weight > .001 and not observed.stopped:
                    active.append('BasicAttack')
    return active


def cleanup():
    """仅结束夹具动作与计划，保留正式授予，下一用例再重置站位。"""
    if state.get('blocked'):
        unreal.AbilitySystemLibrary.remove_loose_gameplay_tags(server, state.pop('blocked'), False)
    for pawn in [s['source'], server, s['source_owner'], owner]:
        for handle in asc(pawn).get_all_abilities():
            ga = lib.call_method('GetGameplayAbilityFromSpecHandle', (asc(pawn), handle))[0]
            if isinstance(ga, (unreal.HodgeGameplayAbility_Definition, unreal.HodgeGameplayAbility_MovementAction,
                               unreal.HodgeGameplayAbility_HitReaction)) and lib.call_method('IsGameplayAbilityActive', (ga,)):
                unreal.HodgeCombatValidationLibrary.queue_ability_action(asc(pawn), handle, True)


def finish(error=None):
    """报告失败也恢复内存配置；不保存共享探针包。"""
    report['error'] = error
    value = definition.get_editor_property('DefaultHitConfig')
    value.import_text(original)
    definition.set_editor_property('DefaultHitConfig', value)
    cleanup()
    unreal.unregister_slate_post_tick_callback(state['callback'])
    report_name = globals().get('RECOVERY_REPORT_NAME', 'player-recovery-' + str(s.get('lag_ms', 0)) + 'ms.json')
    path = Path(unreal.Paths.project_saved_dir()) / 'ACTCombat' / report_name
    path.write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
    s['act_recovery_result'] = error or 'PASS'


def tick(delta):
    """按实际受击到达时间驱动输入，逐端记录控制、动作和加速度。"""
    try:
        assert time.monotonic() - state['wall'] < 90, '恢复用例超时'
        now = unreal.GameplayStatics.get_time_seconds(world)
        if now < state['after']:
            return
        if state['stage'] == 'reset':
            state['index'] += 1
            if state['index'] == len(cases):
                finish()
                return
            cleanup()
            state.update(stage='start', after=now + .7)
            return
        if state['stage'] == 'start':
            for pawn, x in [(s['source'], 1000), (s['source_owner'], 1000), (server, 1200), (owner, 1200)]:
                pawn.character_movement.stop_movement_immediately()
                pawn.set_actor_location(unreal.Vector(x, 1000, 95), False, True)
                pawn.set_actor_rotation(unreal.Rotator(yaw=0), False)
                # 恢复用例统一测试镜头朝向，完整攻击转向与连段窗口另有独立用例。
                if pawn.get_controller():
                    pawn.get_controller().set_control_rotation(unreal.Rotator(yaw=0))
            cfg = definition.get_editor_property('DefaultHitConfig')
            request = cfg.get_editor_property('Reaction')
            request.import_text('(AttackJudgementTag=(TagName="Combat.Attack.Judgement.Skill"),Impacts=((Type=HitStun)))')
            cfg.set_editor_property('Reaction', request)
            definition.set_editor_property('DefaultHitConfig', cfg)
            source_asc = asc(s['source_owner'])
            handle = next(h for h in source_asc.get_all_abilities() if source_asc.find_ability_definition(h) == definition)
            assert unreal.HodgeCombatValidationLibrary.queue_ability_action(source_asc, handle, False)
            state.update(stage='hit', after=now, timeout=now + 3., pressed=False, rows=[])
            return
        if state['stage'] == 'hit':
            assert now < state['timeout'], '真实攻击未建立玩家受击'
            if not reaction.is_controlled():
                return
            state.update(stage='observe', began=reaction.get_reaction_state().recovery_started_at)
        age = now - state['began']
        name, when = cases[state['index']]
        if name.startswith('Rejected') and 'blocked' not in state:
            # 只在服务器制造既有准入／费用拒绝，拥有者仍走真实预测输入，不伪造拒绝 RPC。
            tags = unreal.GameplayTagContainer()
            blocked_tag = 'State.Movement.Dashing' if name == 'RejectedDodge' else 'State.Combat.Mode.DarkSurge'
            tags.import_text('(GameplayTags=((TagName="' + blocked_tag + '")))')
            unreal.AbilitySystemLibrary.add_loose_gameplay_tags(server, tags, False)
            state['blocked'] = tags
        if name == 'Movement' and when <= age < .8:
            sub.inject_input_vector_for_action(move, unreal.Vector(0, 1, 0), [], [])
        elif name != 'Movement' and age >= when and not state['pressed']:
            # 延迟网络按拥有者实际收到的窗口发起合法用例，不能用服务器时钟提前替客户端按键。
            owner_reaction = owner.get_component_by_class(unreal.HodgeHitReactionComponent)
            kind = unreal.HodgeHitRecoveryAction.DODGE if 'Dodge' in name else unreal.HodgeHitRecoveryAction.BASIC_ATTACK
            allowed = not (s.get('lag_ms', 0) and name.startswith('Allowed')) or not owner_reaction.is_controlled() or owner_reaction.can_recover(kind)
            if allowed:
                sub.inject_input_vector_for_action(dodge if 'Dodge' in name else attack, unreal.Vector(1, 0, 0), [], [])
                state['pressed'] = True
        row = {'case': name, 'age': age, 'controlled': reaction.is_controlled(), 'server_actions': actions(server),
               'owner_actions': actions(owner), 'observer_controlled': observer.get_component_by_class(unreal.HodgeHitReactionComponent).is_controlled(),
               'acceleration': server.character_movement.get_current_acceleration().length(),
               'owner_dodge_window': owner.get_component_by_class(unreal.HodgeHitReactionComponent).can_recover(unreal.HodgeHitRecoveryAction.DODGE),
               'server_dodge_window': reaction.can_recover(unreal.HodgeHitRecoveryAction.DODGE),
               'owner_ground': owner.character_movement.is_moving_on_ground(),
               'owner_sequence': owner.get_component_by_class(unreal.HodgeHitReactionComponent).get_reaction_state().sequence,
               'server_sequence': reaction.get_reaction_state().sequence}
        row['owner_combo'] = owner_combat.get_current_combo_tag().export_text()
        row['server_combo'] = server_combat.get_current_combo_tag().export_text()
        row['server_yaw'] = server.get_actor_rotation().yaw
        row['owner_yaw'] = owner.get_actor_rotation().yaw
        state['rows'].append(row)
        report['samples'].append(row)
        if age < 1.5:
            return
        rows = state['rows']
        action = 'Dodge' if 'Dodge' in name else 'BasicAttack'
        earliest = .2 if action == 'Dodge' else .3
        assert all(action not in r['server_actions'] for r in rows if r['age'] < earliest - .015), '禁止期提前行动'
        if name.startswith('Allowed'):
            assert any(action in r['server_actions'] for r in rows), '合法恢复输入未启动行动'
            if action == 'BasicAttack':
                # 播放攻击不能冒充连段接管；拥有者必须记录当前刀，后续输入才能正确衔接。
                attack_rows = [r for r in rows if action in r['owner_actions']]
                assert attack_rows and all('Combo.Light.' in r['owner_combo'] for r in attack_rows), '拥有者播放攻击但未接管连段节点'
                assert all('Combo.Light.' in r['server_combo'] for r in rows if action in r['server_actions']), '服务器未接管连段节点'
            if not s.get('lag_ms', 0):
                # 权威提交先解除旧控制，方向 TargetData 往返后 Montage 才真正起播。
                assert any(not r['controlled'] and r['age'] < .395 for r in rows), '未在自然释放前提交恢复'
        if name.startswith('Rejected'):
            assert all(action not in r['server_actions'] for r in rows if r['age'] < .395), '服务器拒绝仍提前逃脱'
            assert all(r['controlled'] for r in rows if r['age'] < .38), '提交拒绝提前解除权威控制'
        if name == 'Movement':
            assert all(r['acceleration'] < 1. for r in rows if r['controlled']), '受控时接受普通加速度'
            assert any(r['acceleration'] > 1. for r in rows if r['age'] > .42), '移动未恢复'
        assert not rows[-1]['controlled'] and not rows[-1]['observer_controlled'], '观察端残留控制'
        report['cases'].append({'case': name, 'passed': True,
                               'authority_release': next((r['age'] for r in rows if not r['controlled']), None),
                               'earliest_action': next((r['age'] for r in rows if action in r['server_actions']), None),
                               'owner_action': next((r['age'] for r in rows if action in r['owner_actions']), None)})
        state.update(stage='reset', after=now + .2)
    except Exception:
        finish(traceback.format_exc())


state['callback'] = unreal.register_slate_post_tick_callback(tick)
print('ACT_PLAYER_RECOVERY_STARTED')
