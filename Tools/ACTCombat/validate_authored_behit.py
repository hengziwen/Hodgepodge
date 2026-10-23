"""用真实探针近战检查 S/B、根运动互斥、连续受力和 Fly/Push 接地，支持多端观察。"""
from pathlib import Path
import builtins
import json
import re
import sys
import time
import traceback
import unreal

sys.path.insert(0, str(Path(unreal.Paths.project_dir()) / 'Tools/HitReactionDummy'))
import control

MODE = globals().get('BEHIT_MODE', 'single')
LAG = int(globals().get('BEHIT_LAG_MS', 0))
editor = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
play = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.LevelEditorPlaySettings'))
perf = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.EditorPerformanceSettings'))
previous = {k: play.get_editor_property(k) for k in ['PlayNumberOfClients', 'RunUnderOneProcess']}
try:
    old_mode = int(play.get_editor_property('PlayNetMode').value)
except TypeError as error:
    old_mode = int(re.search(r"Cannot pythonize '(\d+)'", str(error)).group(1))
throttle = perf.get_editor_property('bThrottleCPUWhenNotForeground')
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(1 if MODE == 'single' else 2, MODE == 'dedicated')
perf.set_editor_property('bThrottleCPUWhenNotForeground', False)
assert unreal.EditorLoadingAndSavingUtils.load_map('/Game/Tests/Combat/L_HitReactionDummy')
cases = ['SmallL', 'SmallR', 'BigL', 'BigR', 'DenseContinue', 'IndependentReimpact', 'GroundKnockback', 'GroundDowned', 'FlyPush', 'FlySlam', 'CancelRoot']
report = {'mode': MODE, 'lag_ms': LAG, 'cases': [], 'samples': [], 'error': None, 'input': 'Existing equipment-granted melee probe and native action queue'}
scope = {'wall': time.monotonic(), 'phase': 'boot', 'index': 0, 'saved_hits': {}, 'last_sample': -1.0}
builtins.HODGE_AUTHORED_BEHIT_SCOPE = globals()
lib = unreal.HodgeCombatValidationLibrary


def check(name, condition, detail=None):
    """失败不吞掉已采集的动作身份、位移和运动来源证据。"""
    scope['checks'].append({'check': name, 'passed': bool(condition), 'detail': detail})
    assert condition, (cases[scope['index']], name, detail)


def request(kind, strength=1.15, side='L'):
    """仅改探针的内存 Reaction，正式攻击资源不被测试工具保存。"""
    asset = unreal.load_asset(control.ROOT + '/DA_Probe_' + kind)
    if kind not in scope['saved_hits']:
        scope['saved_hits'][kind] = asset.get_editor_property('DefaultHitConfig').export_text()
    hit = asset.get_editor_property('DefaultHitConfig')
    reaction = hit.get_editor_property('Reaction')
    reaction.set_editor_property('PresentationStrength', strength)
    reaction.set_editor_property('PresentationSide', unreal.HodgeHitSide.LEFT if side == 'L' else unreal.HodgeHitSide.RIGHT)
    if kind == 'HitStun':
        impacts = list(reaction.get_editor_property('Impacts'))
        impacts[0].set_editor_property('ControlDuration', .9)
        reaction.set_editor_property('Impacts', impacts)
    hit.set_editor_property('Reaction', reaction)
    asset.set_editor_property('DefaultHitConfig', hit)
    if cases[scope['index']] == 'DenseContinue' and not scope.get('dense_notify'):
        # 两次独立能力本身有约 0.3 秒起手，密集案例必须在同一真实 Montage 内追加通知。
        montage = asset.get_editor_property('ExecutionConfig').montage
        events = [e for e in unreal.AnimationLibrary.get_animation_notify_events(montage) if isinstance(e.notify, unreal.HodgeAnimNotify_Hit)]
        assert len(events) == 1
        second = type(events[0].notify.get_editor_property('Hit'))()
        second.import_text(events[0].notify.get_editor_property('Hit').export_text())
        second.set_editor_property('HitGroup', 'DenseFollow')
        assert lib.add_hit_notify(montage, .42, .42, second, True)
        scope['dense_notify'] = [e.notify for e in unreal.AnimationLibrary.get_animation_notify_events(montage) if isinstance(e.notify, unreal.HodgeAnimNotify_Hit)][-1]
        scope['dense_montage_path'] = montage.get_path_name()
    control.probe(kind)


def finish(error=None):
    """恢复探针配置和输入夹具，退出后再释放世界引用。"""
    if scope['phase'] == 'closing':
        return
    report['error'] = error
    for kind, original in scope['saved_hits'].items():
        asset = unreal.load_asset(control.ROOT + '/DA_Probe_' + kind)
        value = type(asset.get_editor_property('DefaultHitConfig'))()
        value.import_text(original)
        asset.set_editor_property('DefaultHitConfig', value)
    control.stop()
    output = Path(unreal.Paths.project_saved_dir()) / 'ACTCombat/Behit'
    output.mkdir(parents=True, exist_ok=True)
    suffix = ('-' + str(LAG) + 'ms') if LAG else ''
    (output / ('motion-' + MODE + suffix + '.json')).write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
    for world in unreal.EditorLevelLibrary.get_pie_worlds(True):
        if LAG:
            unreal.SystemLibrary.execute_console_command(world, 'Net PktLag=0')
    scope['phase'] = 'closing'
    editor.editor_request_end_play()


def sample(now):
    """读取每端实际动画、胶囊和 RMS；配置字段本身不代表已执行根运动。"""
    rows = []
    for world in unreal.EditorLevelLibrary.get_pie_worlds(True):
        targets = unreal.GameplayStatics.get_all_actors_of_class(world, scope['target_class'])
        if not targets:
            continue
        target = targets[0]
        reaction = target.get_component_by_class(unreal.HodgeHitReactionComponent)
        state = reaction.get_reaction_state()
        location = target.get_actor_location()
        motion = lib.inspect_reaction_motion(target)
        row = {'case': cases[scope['index']], 'time': now, 'server': unreal.SystemLibrary.is_server(world),
               'world': world.get_path_name(), 'phase': str(state.phase), 'sequence': state.sequence, 'started': state.started_at,
               'source': str(state.motion_source), 'landing': state.landing, 'controlled': reaction.is_controlled(),
               'x': location.x, 'y': location.y, 'z': location.z, 'falling': target.character_movement.is_falling(),
               'anim_root': motion.x, 'rms': motion.y, 'playing_root': motion.z,
               'montage': state.montage.get_name() if state.montage else None}
        if state.montage:
            pose = lib.inspect_montage_state(target, state.montage)
            row.update(instance=pose.instance_id, weight=pose.weight, position=pose.position)
        rows.append(row)
    report['samples'].extend(rows)
    return rows


def tick(delta):
    """每次只运行一个动作案例，所有异步等待都有期限。"""
    try:
        worlds = unreal.EditorLevelLibrary.get_pie_worlds(True)
        if scope['phase'] == 'closing':
            if worlds:
                return
            assert lib.configure_validation_pie(1 if old_mode == 0 else 2, old_mode == 2)
            for key, value in previous.items():
                play.set_editor_property(key, value)
            perf.set_editor_property('bThrottleCPUWhenNotForeground', throttle)
            unreal.unregister_slate_post_tick_callback(scope['callback'])
            for key in ['source', 'target', 'reaction', 'server']:
                scope.pop(key, None)
            if scope.get('dense_montage_path'):
                path = scope.pop('dense_montage_path')
                scope.pop('dense_notify', None)
                asset = unreal.load_asset(path)
                changed, error = unreal.EditorLoadingAndSavingUtils.reload_packages([asset.get_outermost()], unreal.ReloadPackagesInteractionMode.ASSUME_POSITIVE)
                assert changed and not str(error), str(error)
            print('AUTHORED_BEHIT_CLEANUP ' + MODE + ' ' + str(report['error']))
            return
        assert time.monotonic() - scope['wall'] < 180, '受击动作测试超时'
        server = next((w for w in worlds if unreal.SystemLibrary.is_server(w)), None)
        if not server:
            return
        now = unreal.GameplayStatics.get_time_seconds(server)
        if scope['phase'] == 'boot':
            expected = 1 if MODE == 'single' else (3 if MODE == 'dedicated' else 2)
            if len(worlds) != expected:
                return
            source = unreal.GameplayStatics.get_player_pawn(server, 0)
            if not isinstance(source, unreal.HodgeHeroCharacter) or not source.get_hodge_ability_system_component():
                return
            cls = unreal.load_asset(control.ROOT + '/BP_HitReactionDummy').generated_class()
            targets = unreal.GameplayStatics.get_all_actors_of_class(server, cls)
            if len(targets) != 1 or not targets[0].initialize_enemy_ability_system():
                return
            reaction = targets[0].get_component_by_class(unreal.HodgeHitReactionComponent)
            if not reaction.get_profile():
                return
            control.start()
            if LAG:
                for world in worlds:
                    unreal.SystemLibrary.execute_console_command(world, 'Net PktLag=' + str(LAG))
            scope.update(source=source, target=targets[0], reaction=reaction, target_class=cls, server=server, phase='reset')
        if scope['phase'] == 'reset':
            control.reset()
            scope.update(phase='settle', at=now, checks=[], second=False, canceled=False, saw_start=False, saw_push=False,
                         saw_push_loop=False, saw_landing=False, saw_get_up=False, samples_from=len(report['samples']))
            return
        if scope['phase'] == 'settle':
            if now - scope['at'] < .4:
                return
            name = cases[scope['index']]
            kind = 'Knockback' if name == 'GroundKnockback' else 'Knockdown' if name == 'GroundDowned' else 'Launch' if name in ['FlyPush', 'FlySlam'] else 'HitStun'
            strength = .65 if name.startswith('Small') else 1.15
            scope['before_hp'] = scope['target'].get_component_by_class(unreal.HodgeHealthComponent).get_health()
            scope['origin'] = scope['target'].get_actor_location()
            request(kind, strength, 'R' if name.endswith('R') else 'L')
            scope.update(phase='observe', at=now, first_sequence=None)
        if scope['phase'] != 'observe':
            return
        name = cases[scope['index']]
        state = scope['reaction'].get_reaction_state()
        active = state.phase != unreal.HodgeHitReactionPhase.NONE
        if active and scope['first_sequence'] is None:
            scope.update(first_sequence=state.sequence, first_started=state.started_at, first_at=now,
                         first_instance=lib.inspect_montage_state(scope['target'], state.montage).instance_id)
        age = now - scope.get('first_at', now)
        if not scope['second'] and scope['first_sequence'] is not None:
            if name == 'IndependentReimpact' and age > .25:
                request('HitStun', 1.15, 'R')
                scope['second'] = True
            if name in ['FlyPush', 'FlySlam'] and scope['target'].character_movement.is_falling() and age > .2:
                request('AirHit' if name == 'FlyPush' else 'Slam')
                scope['second'] = True
        if name == 'CancelRoot' and not scope['canceled'] and active and age > .25:
            scope['reaction'].cancel_reaction()
            scope.update(canceled=True, canceled_at=now)
        if now - scope['last_sample'] > .025:
            rows = sample(now)
            scope['last_sample'] = now
            authority = next(r for r in rows if r['server'])
            if authority['montage'] == 'AM_StandUp' and authority.get('weight', 0) > .01:
                scope['saw_get_up'] = True
            if name in ['FlyPush', 'FlySlam']:
                scope['saw_start'] |= authority['montage'] == 'AM_Behit_Fly_Start'
                scope['saw_push'] |= authority['montage'] == 'AM_Behit_Push_Start'
                scope['saw_push_loop'] |= authority['montage'] == 'AM_Behit_Push_Loop'
                if authority['montage'] == 'AM_Behit_Push_Fall' and not scope['saw_landing']:
                    check('Push_Fall 在实际接地后播放', not authority['falling'] and authority['landing'])
                    scope['saw_landing'] = True
        # 完整原动作的恢复段仍有根位移；网络最终位置必须在片段结束后比较，不能截在高速退步中途。
        duration = 10.0 if name in ['FlyPush', 'FlySlam'] else 6.0 if name != 'CancelRoot' else 1.5
        if scope['first_sequence'] is None:
            assert now - scope['at'] < 5.0, '本次探针未在期限内建立真实受击，检查授予句柄和拒绝日志'
            return
        if now - scope['at'] < duration:
            return
        samples = report['samples'][scope['samples_from']:]
        authority = [r for r in samples if r['server']]
        check('独立有效命中实际扣血', scope['target'].get_component_by_class(unreal.HodgeHealthComponent).get_health() < scope['before_hp'])
        if name in ['FlyPush', 'FlySlam']:
            check('Fly 转 Push 进入对应空中和接地动作', scope['saw_start'] and scope['saw_push'] and scope['saw_landing'])
            if name == 'FlyPush':
                check('续空后进入 Push_Loop', scope['saw_push_loop'])
            else:
                check('下砸落地后完成起身', scope['saw_get_up'] and not scope['reaction'].is_controlled())
            check('空中未同时启用 RMS', not any(r['rms'] for r in authority if r['phase'].endswith('AIRBORNE: 2>')))
        else:
            check('实际动画根运动播放且胶囊移动', any(r['playing_root'] for r in authority) and
                  abs(scope['target'].get_actor_location().x - scope['origin'].x) > 1)
            check('地面根运动没有同时启用外部 RMS', not any(r['rms'] for r in authority))
            if name.startswith(('Small', 'Big')):
                expected = 'AM_Behit_' + ('S' if name.startswith('Small') else 'B') + '_' + name[-1]
                check('小/大幅与左右片选择正确', any(r['montage'] == expected and r.get('weight', 0) > .01 for r in authority), expected)
            if name in ['DenseContinue', 'IndependentReimpact']:
                updated = [r for r in authority if r['sequence'] != scope['first_sequence'] and r['controlled']]
                check('第二次命中更新反应身份', bool(updated))
                same = all(abs(r['started'] - scope['first_started']) < .001 for r in updated)
                check('密集续播或独立重受力符合策略', same if name == 'DenseContinue' else not same)
            if name == 'GroundKnockback':
                check('地面击退未误用空中 Push', any(r['montage'] in ['AM_Behit_B_L', 'AM_Behit_B_R'] for r in authority))
            if name == 'GroundDowned':
                check('倒地完整衔接已有 StandUp', scope['saw_get_up'] and not scope['reaction'].is_controlled())
            if name == 'CancelRoot':
                check('取消后没有受控或根运动残留', not scope['reaction'].is_controlled() and lib.inspect_reaction_motion(scope['target']).z == 0)
        if MODE != 'single':
            latest = [r for r in samples if r['time'] >= samples[-1]['time'] - .06]
            check('各端均有实际目标采样', len({r['world'] for r in latest}) == len(worlds))
            server_row = next(r for r in latest if r['server'])
            error = max(((r['x'] - server_row['x']) ** 2 + (r['y'] - server_row['y']) ** 2 +
                         (r['z'] - server_row['z']) ** 2) ** .5 for r in latest)
            check('多端胶囊位置接近权威轨迹', error < 25.0, error)
            check('结尾没有遗留根运动', not any(r['playing_root'] for r in latest))
            if name.startswith(('Small', 'Big')):
                rendered = {r['world'] for r in samples if r['playing_root'] and r.get('weight', 0) > .01}
                check('每端均实际播放根运动片段', len(rendered) == len(worlds), sorted(rendered))
        report['cases'].append({'name': name, 'checks': scope['checks']})
        if name == 'DenseContinue':
            hit = scope['dense_notify'].get_editor_property('Hit')
            hit.set_editor_property('DamageScale', 0.0)
            scope['dense_notify'].set_editor_property('Hit', hit)
        scope['index'] += 1
        if scope['index'] == len(cases):
            finish()
        else:
            scope['phase'] = 'reset'
    except Exception:
        finish(traceback.format_exc())


scope['callback'] = unreal.register_slate_post_tick_callback(tick)
editor.editor_request_begin_play()
print('AUTHORED_BEHIT_STARTED ' + MODE)
