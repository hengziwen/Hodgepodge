"""通过真实 E／移动输入验证独立技能取消、冷却、资源清理和联机拒绝恢复。"""
import gc
import json
import time
import traceback
from pathlib import Path
import unreal

WORK = Path(globals()['LOCK_ON_WORK'])
MODE = globals()['LOCK_ON_MODE']
worlds = unreal.EditorLevelLibrary.get_pie_worlds(True)
server = next(w for w in worlds if unreal.SystemLibrary.is_server(w))
clients = [w for w in worlds if w != server]
owner = next(p for p in unreal.GameplayStatics.get_all_actors_of_class(clients[0] if clients else server, unreal.HodgeHeroCharacter)
             if not clients or p.get_local_role() == unreal.NetRole.ROLE_AUTONOMOUS_PROXY)
pid = owner.get_editor_property('PlayerState').get_editor_property('PlayerId')
peers = [p for w in worlds for p in unreal.GameplayStatics.get_all_actors_of_class(w, unreal.HodgeHeroCharacter)
         if p.get_editor_property('PlayerState').get_editor_property('PlayerId') == pid]
authority = next(p for p in peers if p.get_world() == server)
bridge = unreal.get_default_object(unreal.load_class(None, '/Script/GameplayAbilities.AbilitySystemBlueprintLibrary'))
inputs = unreal.get_default_object(unreal.load_class(None, '/Script/Engine.SubsystemBlueprintLibrary')).call_method(
    'GetLocalPlayerSubSystemFromPlayerController', (owner.get_controller(), unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
skill_input = unreal.load_asset('/Game/Main/Input/InputAction/IA_Resonance_Skill')
move_input = unreal.load_asset('/Game/Main/Input/InputAction/IA_Move')
attack_input = unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack')
definition = unreal.load_asset('/Game/Main/Character/Hero/Ability/ResonanceSkill/DA_Resonance_Skill')
montage = definition.execution_config.montage
cooldown = unreal.load_class(None, '/Game/Main/Character/Hero/Ability/ResonanceSkill/GE_Resonance_Skill_Cooldown.GE_Resonance_Skill_Cooldown_C')


def tag(name):
    """只构造已有标签，不向项目写入临时配置。"""
    value = unreal.GameplayTag()
    value.import_text('(TagName="' + name + '")')
    return value


move_tag = tag('Status.Attack.Cancel.Move')
attack_tag = tag('Status.Attack')
body_tag = tag('State.Combat.Body.Normal')
next_tag = tag('Status.Attack.Cancel.NextAttack')
blocked_tags = unreal.GameplayTagContainer()
blocked_tags.import_text('(GameplayTags=((TagName="Gameplay.AbilityInputBlocked")))')
for w in worlds:
    if MODE == 'listen-lag':
        unreal.SystemLibrary.execute_console_command(w, 'Net PktLag=100')
    for enemy in unreal.GameplayStatics.get_all_actors_of_class(w, unreal.HodgeEnemyCharacter):
        enemy.character_movement.disable_movement()
        enemy.set_actor_location(unreal.Vector(2300, 2300, 100), False, True)
for p in {owner, authority}:
    p.set_actor_location(unreal.Vector(1000, 1000, 100), False, True)
    p.set_actor_rotation(unreal.Rotator(), False)
    p.get_controller().set_control_rotation(unreal.Rotator())

# 各轮都等待真实八秒冷却到期，不删除或缩短生产 GE。
cases = ['pre_release', 'threshold', 'held', 'late'] + (['reject'] if clients else [])
state = {'wall': time.monotonic(), 'phase': 'ready', 'case_index': 0, 'at': unreal.GameplayStatics.get_time_seconds(server),
         'requested_instances': set(), 'seen_combo': set(), 'samples_at': -1.}
checks = []
samples = []


def read(pawn):
    """只读实际能力、蒙太奇、通知资源、冷却和移动动画。"""
    asc = pawn.get_hodge_ability_system_component()
    # 模拟代理不会复制完整 AbilitySpec，只验证其动画与观察者标签。
    handle = next((h for h in asc.get_all_abilities() if asc.find_ability_definition(h) == definition), None)
    ability = bridge.call_method('GetGameplayAbilityFromSpecHandle', (asc, handle))[0] if handle else None
    active = bridge.call_method('IsGameplayAbilityActive', (ability,)) if ability else False
    info = unreal.HodgeCombatValidationLibrary.inspect_montage_state(pawn, montage)
    wing = pawn.get_component_by_class(unreal.HodgeWingComponent)
    mesh = wing.get_wing_mesh() if wing else None
    return {'active': active, 'position': info.position, 'instance': info.instance_id, 'playing': info.playing,
            'stopped': info.stopped, 'weight': info.weight, 'locomotion': str(info.locomotion_state),
            'acceleration': pawn.character_movement.get_current_acceleration().length(),
            'speed': pawn.get_velocity().length(), 'cooldown': asc.call_method('GetGameplayEffectCount', (cooldown, None, True)),
            'input_blocked': asc.has_matching_gameplay_tag(tag('Gameplay.AbilityInputBlocked')), 'attack': asc.has_matching_gameplay_tag(attack_tag), 'cancel_window': asc.has_matching_gameplay_tag(move_tag),
            'body': asc.has_matching_gameplay_tag(body_tag), 'hit_sessions': unreal.HodgeCombatValidationLibrary.inspect_hit_sessions(pawn),
            'pose_leases': unreal.HodgeCombatValidationLibrary.inspect_pose_leases(pawn),
            'wing_visible': bool(mesh and mesh.visible and not mesh.hidden_in_game)}


def inject(action, value=unreal.Vector()):
    """输入走 EnhancedInput，避免 Slate 回调中直接激活网络 GAS。"""
    inputs.inject_input_vector_for_action(action, value, [], [])


def check(requirement, name, condition, detail=None):
    """每项断言关联设计需求，失败保留完整采样。"""
    checks.append({'requirement': requirement, 'check': name, 'passed': bool(condition), 'detail': detail})
    assert condition, (name, detail)


def finish(error=None):
    """清理测试输入和世界引用，不修改生产资产。"""
    unreal.unregister_slate_post_tick_callback(state['callback'])
    for action in [skill_input, move_input, attack_input]:
        inject(action)
    if state.get('authority_blocked'):
        bridge.call_method('RemoveLooseGameplayTags', (authority, blocked_tags, False))
    result = {'mode': MODE, 'passed': not error and all(c['passed'] for c in checks), 'checks': checks,
              'samples': samples, 'error': error}
    (WORK / (MODE + '-e2e.json')).write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
    print(json.dumps({'mode': MODE, 'passed': result['passed'], 'checks': len(checks), 'error': error}, ensure_ascii=False))
    for name in ['worlds', 'clients', 'peers', 'owner', 'authority', 'server', 'inputs', 'p', 'w', 'enemy']:
        globals()[name] = None
    gc.collect()


def after_cancel(now):
    """移动持续生效后检查各端退出、冷却和资源，再进入下一轮。"""
    if now - state['at'] < .8:
        return
    rows = [read(p) for p in peers]
    check('MC03', state['case'] + '_拥有者与服务器结束', not read(owner)['active'] and not read(authority)['active'], rows)
    check('MC04', state['case'] + '_原冷却保留', read(authority)['cooldown'] == 1, rows)
    check('MC05', state['case'] + '_所有端蒙太奇退出', all(r['weight'] < .05 for r in rows), rows)
    check('MC05', state['case'] + '_通知及姿势资源清理', all(not r['attack'] and not r['cancel_window'] and not r['body']
          and r['hit_sessions'] == 0 and r['pose_leases'] == 0 and not r['wing_visible'] for r in rows), rows)
    check('MC03', state['case'] + '_移动状态恢复', read(owner)['acceleration'] > 100 and read(owner)['speed'] > 50, rows)
    # 冷却内再次真实点按，不能重新播放技能。
    inject(skill_input, unreal.Vector(1, 0, 0))
    state['phase'] = 'cooldown_check'
    state['at'] = now


def tick(delta):
    """用源动画位置控制输入时机，拒绝轮只限制服务器 PIE 输入权限。"""
    try:
        assert time.monotonic() - state['wall'] < 210, '测试超时'
        now = unreal.GameplayStatics.get_time_seconds(server)
        local, remote = read(owner), read(authority)
        if now - state['samples_at'] > .025:
            samples.append({'time': now, 'phase': state['phase'], 'case': state.get('case'), 'owner': local, 'authority': remote})
            state['samples_at'] = now
        phase = state['phase']
        if phase == 'ready':
            for action in [skill_input, move_input, attack_input]:
                inject(action)
            if now - state['at'] < .8 or remote['cooldown'] or local['cooldown'] or local['active']:
                return
            if state['case_index'] >= len(cases):
                state['phase'] = 'combo'
                state['at'] = now
                return
            state['case'] = cases[state['case_index']]
            for p in {owner, authority}:
                p.set_actor_location(unreal.Vector(1000, 1000, 100), False, True)
                p.set_actor_rotation(unreal.Rotator(), False)
                p.get_controller().set_control_rotation(unreal.Rotator())
            inject(skill_input, unreal.Vector(1, 0, 0))
            state['phase'] = 'start'
            state['at'] = now
        elif phase == 'start':
            inject(skill_input)
            if not local['active'] or not remote['active']:
                assert now - state['at'] < 3, ('技能未发动', local, remote)
                return
            state['first_instance'] = local['instance']
            check('MC00', state['case'] + '_真实 E 发动独立技能', local['instance'] >= 0 and remote['instance'] >= 0)
            state['phase'] = 'exercise'
            state['before_asserted'] = False
            state['move_started'] = False
            state['window_asserted'] = False
            state['authority_blocked'] = False
        elif phase == 'exercise':
            case = state['case']
            position = local['position']
            if case == 'pre_release':
                inject(move_input, unreal.Vector(0, 1, 0) if .25 < position < .65 else unreal.Vector())
            elif case == 'threshold':
                inject(move_input, unreal.Vector(0, .05, 0))
            elif case in ['held', 'reject']:
                # 预测退出后读不到蒙太奇位置，仍须持续按住，不能让测试意外松键后重按。
                state['move_started'] = state['move_started'] or position > .25
                inject(move_input, unreal.Vector(0, 1, 0) if state['move_started'] else unreal.Vector())
            else:
                inject(move_input, unreal.Vector(0, 1, 0) if position > 1.55 else unreal.Vector())
            if .45 < position < 1.0 and not state['before_asserted']:
                check('MC01', case + '_前摇不提前取消', local['active'] and remote['active'], (local, remote))
                state['before_asserted'] = True
            if case == 'reject' and remote['position'] > .8 and not state['authority_blocked']:
                check('MC06', '服务器临时禁止输入', bridge.call_method('AddLooseGameplayTags', (authority, blocked_tags, False)))
                state['authority_blocked'] = True
            if case in ['pre_release', 'threshold']:
                if position > 1.6 and not state['window_asserted']:
                    check('MC01', case + '_无有效输入不取消', local['active'] and remote['active'] and local['cancel_window'], (local, remote))
                    state['window_asserted'] = True
                if not local['active'] and not remote['active']:
                    check('MC01', case + '_自然结束', state['before_asserted'] and state['window_asserted'])
                    state['case_index'] += 1
                    state['phase'] = 'ready'
                    state['at'] = now
            elif case == 'reject':
                if state['authority_blocked'] and local['instance'] != state['first_instance'] and local['active'] and local['playing'] and not local['stopped']:
                    check('MC06', '服务器拒绝后恢复蒙太奇实例', remote['active'] and remote['input_blocked'] and not local['input_blocked'], (local, remote))
                    state['phase'] = 'reject_wait'
                    state['at'] = now
                else:
                    assert now - state['at'] < 8, ('未恢复拒绝预测', local, remote)
            elif not local['active'] and not remote['active']:
                check('MC02', case + '_窗口允许移动取消', state['before_asserted'])
                state['phase'] = 'after_cancel'
                state['at'] = now
            else:
                assert position < 2.4, ('窗口内没有取消', local, remote)
        elif phase == 'reject_wait':
            inject(move_input, unreal.Vector(0, 1, 0))
            if now - state['at'] > .15:
                check('MC06', '拒绝后持续输入不重复预测退出', local['active'] and local['playing'] and not local['stopped'], local)
                inject(move_input)
                check('MC06', '解除服务器输入限制', bridge.call_method('RemoveLooseGameplayTags', (authority, blocked_tags, False)))
                state['authority_blocked'] = False
                state['phase'] = 'retry_wait'
                state['at'] = now
        elif phase == 'retry_wait':
            inject(move_input)
            assert local['active'] and remote['active'], ('重试前技能意外结束', local, remote)
            if remote['position'] > 1.2 and local['position'] > 1.2:
                state['phase'] = 'retry'
                state['at'] = now
        elif phase == 'retry':
            inject(move_input, unreal.Vector(0, 1, 0))
            if not local['active'] and not remote['active']:
                check('MC06', '松键重新移动后可再次确认取消', True)
                state['phase'] = 'after_cancel'
                state['at'] = now
            else:
                assert now - state['at'] < 1.5, ('重试取消失败', local, remote)
        elif phase == 'after_cancel':
            inject(move_input, unreal.Vector(0, 1, 0))
            after_cancel(now)
        elif phase == 'cooldown_check':
            inject(skill_input)
            inject(move_input, unreal.Vector(0, 1, 0))
            if now - state['at'] > .4:
                check('MC04', state['case'] + '_冷却中再按 E 被拒绝', not local['active'] and not remote['active'], (local, remote))
                state['case_index'] += 1
                state['phase'] = 'ready'
                state['at'] = now
        elif phase == 'combo':
            inject(move_input)
            inject(attack_input)
            current = owner.get_component_by_class(unreal.SkeletalMeshComponent).get_anim_instance().get_current_active_montage()
            info = unreal.HodgeCombatValidationLibrary.inspect_montage_state(owner, current) if current else None
            idle = not info or info.instance_id < 0 or info.stopped
            if current and not idle:
                state['seen_combo'].add(current.get_name())
            if len(state['seen_combo']) >= 5:
                check('MC07', '五段真实输入连段保持正常', len(state['seen_combo']) == 5, sorted(state['seen_combo']))
                state['phase'] = 'combo_move'
                state['at'] = now
                return
            ready = not idle and owner.get_hodge_ability_system_component().has_matching_gameplay_tag(next_tag) and info.instance_id not in state['requested_instances']
            if (idle and now - state['at'] > .5) or ready:
                inject(attack_input, unreal.Vector(1, 0, 0))
                if ready:
                    state['requested_instances'].add(info.instance_id)
            assert now - state['at'] < 17, ('连段回归失败', sorted(state['seen_combo']))
        elif phase == 'combo_move':
            inject(move_input, unreal.Vector(0, 1, 0))
            inject(attack_input)
            if now - state['at'] > 1.4:
                rows = [read(p) for p in peers]
                check('MC07', '普攻窗口移动取消仍可恢复移动', not owner.get_hodge_ability_system_component().has_matching_gameplay_tag(attack_tag)
                      and local['acceleration'] > 100, rows)
                finish()
    except Exception:
        finish(traceback.format_exc())

state['callback'] = unreal.register_slate_post_tick_callback(tick)
