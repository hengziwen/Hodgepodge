"""复用已准备好的联机夹具，以相隔 0.12 秒的真实 Notify 验证连续受击。"""
import builtins
import json
import time
import traceback
from pathlib import Path
import unreal

s = builtins.HODGE_REACTION_TEST
assert not s.get('last_report', {}).get('error'), '先处理上一套件失败，不能重叠启动'
source, target = s['source'], s['target']
world, definition = s['world'], s['definition']
peers = [target, s['owned_target'], s['observer_target']]
montage = definition.get_editor_property('ExecutionConfig').get_editor_property('Montage')
events = [e for e in unreal.AnimationLibrary.get_animation_notify_events(montage) if isinstance(e.get_editor_property('Notify'), unreal.HodgeAnimNotify_Hit)]
assert len(events) in [1, 2], '只允许原始通知与本次临时通知'
if len(events) == 1:
    second = events[0].get_editor_property('Notify').get_editor_property('Hit')
    assert unreal.HodgeCombatValidationLibrary.add_hit_notify(montage, .42, .42, second, True)
notify = [e.get_editor_property('Notify') for e in unreal.AnimationLibrary.get_animation_notify_events(montage) if isinstance(e.get_editor_property('Notify'), unreal.HodgeAnimNotify_Hit)][-1]
report = {'cases': [], 'samples': [], 'notify_gap': .12, 'error': None}
# 上一用例会保留高抗动作，这里先通过原能力入口取消，不能留下高抗标签影响强受击。
hold_handles = [h for h in asc_target.get_all_abilities() if asc_target.find_ability_definition(h) == s['hold']] if (asc_target := target.get_hodge_ability_system_component()) else []
for h in hold_handles:
    unreal.HodgeCombatValidationLibrary.queue_ability_action(asc_target, h, True)
state = {'index': globals().get('DENSE_START_INDEX', 0) - 1, 'stage': 'prepare', 'next': unreal.GameplayStatics.get_time_seconds(world) + .3, 'boot': time.monotonic()}


def asc(pawn):
    return pawn.get_hodge_ability_system_component()


def reaction(pawn):
    return pawn.get_component_by_class(unreal.HodgeHitReactionComponent)


def handle(pawn):
    values = [h for h in asc(pawn).get_all_abilities() if asc(pawn).find_ability_definition(h) == definition]
    assert len(values) == 1
    return values[0]


def finish(error=None):
    """不保存测试资产，调用者结束 PIE 后重载四个已知夹具包。"""
    report['error'] = error
    unreal.unregister_slate_post_tick_callback(state['callback'])
    output_name = globals().get('DENSE_REPORT_NAME', 'dense-contacts.json')
    (Path(unreal.Paths.project_saved_dir()) / 'HitPresentation' / output_name).write_text(json.dumps(report, indent=2), encoding='utf-8')


def tick(delta):
    try:
        assert time.monotonic() - state['boot'] < 60, '连续命中验证超时'
        now = unreal.GameplayStatics.get_time_seconds(world)
        if now < state['next']:
            return
        if state['stage'] == 'prepare':
            state['index'] += 1
            if state['index'] == 3:
                finish()
                return
            # 同一真实攻击执行中的两次通知具有独立结算身份，不修改正式攻击。
            cfg = definition.get_editor_property('DefaultHitConfig')
            request = cfg.get_editor_property('Reaction')
            request.set_editor_property('PresentationStrength', 1.0)
            request.set_editor_property('Impacts', [unreal.HodgeImpactSpec()] if state['index'] == 2 else [])
            cfg.set_editor_property('Reaction', request)
            definition.set_editor_property('DefaultHitConfig', cfg)
            hit = notify.get_editor_property('Hit')
            override = unreal.HodgeHitReactionConfig()
            if state['index'] == 1:
                override.import_text('(AttackJudgementTag=(TagName="Combat.Attack.Judgement.Skill"),PresentationStrength=1.8)')
            hit.set_editor_property('ReactionOverride', override)
            hit.set_editor_property('bUseDefaultReaction', state['index'] != 1)
            notify.set_editor_property('Hit', hit)
            unreal.HodgeCombatValidationLibrary.queue_ability_action(asc(s['source_owner']), handle(s['source_owner']), True)
            # 正式攻击有根位移，下一用例必须重置拥有者与服务器站位并等待取消完成。
            for pawn, x in [(source, 1000), (s['source_owner'], 1000), (target, 1200), (s['owned_target'], 1200)]:
                pawn.character_movement.stop_movement_immediately()
                pawn.set_actor_location(unreal.Vector(x, 1000, 95), False, True)
                pawn.set_actor_rotation(unreal.Rotator(yaw=0), False)
            state.update(before=[reaction(p).get_presentation_count() for p in peers], first=None,
                         second=None, peer_first=[None] * 3, peer_second=[None] * 3, stage='launch', next=now + .25)
            return
        if state['stage'] == 'launch':
            state['stage'] = 'observe'
            state['next'] = now
            assert unreal.HodgeCombatValidationLibrary.queue_ability_action(asc(s['source_owner']), handle(s['source_owner']), False)
            state['end'] = now + 1.5
        values = []
        for pawn, before in zip(peers, state['before']):
            component = reaction(pawn)
            st = component.get_reaction_state()
            selected = st.montage if state['index'] == 2 else s['profile'].get_editor_property('DirectionalLightFeedback')[0].montage
            anim = pawn.mesh.get_anim_instance()
            # 使用实际播放中的方向动作，不能假设命中始终来自正面。
            if state['index'] != 2:
                selected = max((e.montage for e in s['profile'].get_editor_property('DirectionalLightFeedback')),
                               key=lambda m: unreal.HodgeCombatValidationLibrary.inspect_montage_state(pawn, m).instance_id)
            # 普通 Montage 查询会忽略混出实例，实际实例诊断才能验证恢复阶段未被重启。
            observed = unreal.HodgeCombatValidationLibrary.inspect_montage_state(pawn, selected)
            values.append({'contacts': component.get_presentation_count() - before, 'started': st.started_at,
                           'sequence': st.sequence, 'position': observed.position, 'instance_id': observed.instance_id,
                           'health': pawn.get_component_by_class(unreal.HodgeHealthComponent).get_health(),
                           'weight': observed.weight, 'playing': observed.playing or observed.weight > .001})
        for index, value in enumerate(values):
            if value['contacts'] == 1 and state['peer_first'][index] is None:
                state['peer_first'][index] = value
            if value['contacts'] == 2 and state['peer_second'][index] is None:
                state['peer_second'][index] = value
        report['samples'].append({'case': state['index'], 'time': now, 'peers': values})
        if values[0]['contacts'] == 1 and state['first'] is None:
            state['first'] = values
        if values[0]['contacts'] == 2 and state['second'] is None:
            state['second'] = values
        if now < state['end']:
            return
        assert all(v['contacts'] == 2 for v in values), values
        assert state['first'] and state['second'], '采样未观察到两次独立接触'
        first, second = state['first'][0], state['second'][0]
        if state['index'] == 0:
            assert second['playing'] and second['position'] > first['position'] + .05, (first, second)
            assert second['instance_id'] == first['instance_id'], (first, second)
        elif state['index'] == 1:
            assert second['playing'] and second['position'] < .08, second
            assert second['instance_id'] != first['instance_id'], (first, second)
        else:
            assert second['sequence'] != first['sequence'] and abs(second['started'] - first['started']) < .001, (first, second)
            assert second['position'] > first['position'] + .05, (first, second)
        # 每端分别按自己的事件到达时刻比较，避免用服务端同一帧冒充客户端观察。
        for first_peer, second_peer in zip(state['peer_first'], state['peer_second']):
            assert first_peer and second_peer, '某端未观察到两个独立事件'
            if state['index'] == 0:
                assert first_peer['instance_id'] == second_peer['instance_id'], (first_peer, second_peer)
            elif state['index'] == 1:
                assert first_peer['instance_id'] != second_peer['instance_id'], (first_peer, second_peer)
            else:
                assert first_peer['sequence'] != second_peer['sequence'] and abs(first_peer['started'] - second_peer['started']) < .001
        assert state['second'][0]['health'] < state['first'][0]['health'], '第二次接触未产生独立伤害'
        report['cases'].append({'name': ['LightSameStrength', 'LightEscalation', 'StrongContinue'][state['index']],
                                'passed': True, 'first': state['peer_first'], 'second': state['peer_second'], 'final': values})
        state.update(stage='prepare', next=now + .3)
    except Exception:
        finish(traceback.format_exc())


state['callback'] = unreal.register_slate_post_tick_callback(tick)
print('DENSE_CONTACTS_STARTED')
