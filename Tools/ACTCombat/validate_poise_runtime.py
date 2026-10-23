"""通过真实装备能力、Notify、GE 和 CMC 验证分段击飞、轻重削韧与原地运动，支持服务器与双客户端。"""
from pathlib import Path
import builtins
import json
import math
import os
import re
import sys
import time
import traceback
import unreal


def require_exclusive_session(command_line, expected_pid, current_pid, dirty_packages):
    """在任何编辑器写操作之前核对本任务专用进程，拒绝已有会话或未保存包。"""
    if '-HodgeCombatValidationExclusive' not in command_line.split():
        raise RuntimeError('受击验证仅允许在带专用启动标记的独占编辑器中运行')
    if type(expected_pid) is not int or expected_pid <= 0 or expected_pid != current_pid:
        raise RuntimeError('受击验证的独占编辑器进程身份不匹配')
    if dirty_packages:
        raise RuntimeError('受击验证开始前存在未保存包，拒绝重载或覆盖:' + ', '.join(dirty_packages))


# 入口保护先于地图加载、PIE设置和探针修改；独占启动器必须传入自己进程的真实PID。
require_exclusive_session(unreal.SystemLibrary.get_command_line(), globals().get('POISE_EXCLUSIVE_EDITOR_PID'), os.getpid(),
                          [package.get_path_name() for package in list(unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()) +
                           list(unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages())])

sys.path.insert(0, str(Path(unreal.Paths.project_dir()) / 'Tools/HitReactionDummy'))
import control

MODE = globals().get('POISE_MODE', 'single')
LAG = int(globals().get('POISE_LAG_MS', 0))
PLAYER_TARGET = bool(globals().get('POISE_PLAYER_TARGET', False))
assert not PLAYER_TARGET or MODE in ['listen', 'dedicated'], '真实玩家受击专项需要两个玩家'
OUT = Path(unreal.Paths.project_saved_dir()) / globals().get('POISE_REPORT_DIR', 'SegmentLaunch')
OUT.mkdir(parents=True, exist_ok=True)
editor = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
play = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.LevelEditorPlaySettings'))
perf = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.EditorPerformanceSettings'))
previous = {k: play.get_editor_property(k) for k in ['PlayNumberOfClients', 'RunUnderOneProcess']}
try:
    old_mode = int(play.get_editor_property('PlayNetMode').value)
except TypeError as error:
    old_mode = int(re.search(r"Cannot pythonize '(\d+)'", str(error)).group(1))
throttle = perf.get_editor_property('bThrottleCPUWhenNotForeground')
lib = unreal.HodgeCombatValidationLibrary
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
assert lib.configure_validation_pie(1 if MODE == 'single' else 2, MODE == 'dedicated')
perf.set_editor_property('bThrottleCPUWhenNotForeground', False)
assert unreal.EditorLoadingAndSavingUtils.load_map('/Game/Tests/Combat/L_HitReactionDummy')
cases = ['LightL', 'HeavyR', 'HeavyL', 'HugePoise', 'Knockback', 'Cancel', 'Armor', 'FlyRehitEnabled', 'FlyRehitDisabled', 'FlyLimit', 'DenseNoRefresh', 'DenseThresholds']
if PLAYER_TARGET:
    # 采用正式玩家生命值，有限正伤害覆盖主要状态，不通过修改生命冒充玩家回归。
    cases = ['LightL', 'HeavyR', 'FlyRehitEnabled', 'FlyRehitDisabled']
report = {'mode': MODE, 'lag_ms': LAG, 'cases': [], 'samples': [], 'events': [], 'error': None,
          'target_kind': 'PlayerState-owned player ASC' if PLAYER_TARGET else 'enemy dummy',
          'input': 'Equipment grant -> native ability queue -> actual melee Notify/GE; not physical keyboard'}
scope = {'phase': 'boot', 'index': 0, 'wall': time.monotonic(), 'saved_hits': {}, 'saved_notifies': {}, 'last_sample': -1.}
builtins.HODGE_POISE_RUNTIME_SCOPE = globals()


def clone(value):
    """反射包装器按值复制，测试覆盖不写回磁盘。"""
    result = type(value)()
    result.import_text(value.export_text())
    return result


def check(name, condition, detail=None):
    """每项断言附带上下文，失败仍保留此前完整轨迹。"""
    scope['checks'].append({'name': name, 'passed': bool(condition), 'detail': detail})
    assert condition, (cases[scope['index']], name, detail)


def has_client_control_sample(rows, player_id, observer):
    """严格区分受击拥有者与独立观察客户端，服务器样本不能冒充客户端证据。"""
    return any(not row['server'] and row['player_id'] == player_id and
               row['locally_controlled'] != observer and row['controlled'] for row in rows)


def reaction_snapshot_released(snapshot):
    """最终释放必须同时覆盖控制、动画根运动、外部运动和实际受击动画实例。"""
    return (not snapshot['controlled'] and snapshot['rms'] == 0. and
            snapshot['animation_root'] == 0. and snapshot['playing_root'] == 0. and
            not snapshot['active_reaction_montages'])


def active_montage_instances(path, instances):
    """保留所有仍在播放或混出的实例身份，不能让最新已退出实例遮蔽旧实例。"""
    return [{'path': path, 'instance': pose.instance_id, 'weight': pose.weight} for pose in instances
            if pose.instance_id >= 0 and (pose.playing or pose.weight > .001)]


def configure_request():
    """同一共享探针分别覆盖普通削韧或明确击退，最后一组才追加密集通知。"""
    name = cases[scope['index']]
    air_chain = name in ['FlyRehitEnabled', 'FlyRehitDisabled', 'FlyLimit']
    kind = 'Knockback' if name == 'Knockback' else 'Launch' if air_chain else 'HitStun'
    definition = unreal.load_asset(control.ROOT + '/DA_Probe_' + kind)
    if kind not in scope['saved_hits']:
        scope['saved_hits'][kind] = definition.get_editor_property('DefaultHitConfig').export_text()
    hit = clone(definition.get_editor_property('DefaultHitConfig'))
    request = clone(hit.get_editor_property('Reaction'))
    explicit = name == 'Knockback' or air_chain
    request.set_editor_property('InputMode', unreal.HodgeReactionInputMode.EXPLICIT_IMPACT if explicit else unreal.HodgeReactionInputMode.POISE_DRIVEN)
    request.set_editor_property('ReservedPoiseDamage', 0. if explicit else 120. if name in ['HugePoise', 'Armor'] else 60. if name in ['HeavyL', 'HeavyR', 'Cancel'] else 20.)
    request.set_editor_property('PresentationSide', unreal.HodgeHitSide.LEFT if name.endswith('L') else unreal.HodgeHitSide.RIGHT)
    if not explicit:
        request.set_editor_property('Impacts', [])
    hit.set_editor_property('Reaction', request)
    definition.set_editor_property('DefaultHitConfig', hit)
    # 同一共享探针的每次执行都显式设置分段开关，不修改默认反应来伪造击飞资格。
    montage = definition.get_editor_property('ExecutionConfig').montage
    for event in unreal.AnimationLibrary.get_animation_notify_events(montage):
        if isinstance(event.notify, unreal.HodgeAnimNotify_Hit):
            path = event.notify.get_path_name()
            if path not in scope['saved_notifies']:
                scope['saved_notifies'][path] = (event.notify, event.notify.get_editor_property('Hit').export_text())
            value = clone(event.notify.get_editor_property('Hit'))
            value.set_editor_property('bCanLaunch', air_chain)
            if PLAYER_TARGET:
                value.set_editor_property('bAllowFriendlyFire', True)
            event.notify.set_editor_property('Hit', value)
    profile = scope['reaction'].get_profile()
    profile.set_editor_property('MaxAirHits', 0 if name == 'FlyLimit' else scope['max_air_hits'])
    if name == 'Armor':
        tags = unreal.GameplayTagContainer()
        tags.import_text('(GameplayTags=((TagName="State.Combat.Body.Vajra")))')
        assert unreal.AbilitySystemLibrary.add_loose_gameplay_tags(scope['target'], tags, True)
        scope['armor_tags'] = tags
    if name in ['DenseThresholds', 'DenseNoRefresh']:
        montage = definition.get_editor_property('ExecutionConfig').montage
        events = [e for e in unreal.AnimationLibrary.get_animation_notify_events(montage) if isinstance(e.notify, unreal.HodgeAnimNotify_Hit)]
        assert len(events) in [1, 4]
        first = events[0].notify
        notify_hit = clone(first.get_editor_property('Hit'))
        notify_hit.set_editor_property('bUseDefaultReaction', False)
        for index, amount in enumerate([15., 20., 25., 50.] if name == 'DenseThresholds' else [25., 1., 1., 1.]):
            override = clone(request)
            override.set_editor_property('ReservedPoiseDamage', amount)
            notify_hit.set_editor_property('ReactionOverride', override)
            notify_hit.set_editor_property('HitGroup', 'PoiseDense' + str(index))
            if index < len(events):
                events[index].notify.set_editor_property('Hit', notify_hit)
            else:
                assert lib.add_hit_notify(montage, .3 + index * .12, .3 + index * .12, notify_hit, True)
        scope['dirty_montage'] = montage.get_path_name()
    control.probe(kind)


def finish(error=None):
    """恢复探针并结束 PIE；临时通知在世界销毁后重载，不保存临时资产状态。"""
    if scope['phase'] == 'closing':
        return
    report['error'] = error
    if scope.get('reaction') and 'max_air_hits' in scope:
        scope['reaction'].get_profile().set_editor_property('MaxAirHits', scope['max_air_hits'])
    if scope.get('ignore_source_collision'):
        scope['target'].capsule_component.ignore_actor_when_moving(scope['source'], False)
        scope['source'].capsule_component.ignore_actor_when_moving(scope['target'], False)
        scope['ignore_source_collision'] = False
    if scope.get('armor_tags') and scope.get('target'):
        unreal.AbilitySystemLibrary.remove_loose_gameplay_tags(scope['target'], scope.pop('armor_tags'), True)
    if scope.get('checks'):
        report['unfinished_checks'] = scope['checks']
    for kind, original in scope['saved_hits'].items():
        asset = unreal.load_asset(control.ROOT + '/DA_Probe_' + kind)
        value = type(asset.get_editor_property('DefaultHitConfig'))()
        value.import_text(original)
        asset.set_editor_property('DefaultHitConfig', value)
    for notify, original in scope['saved_notifies'].values():
        value = clone(notify.get_editor_property('Hit'))
        value.import_text(original)
        notify.set_editor_property('Hit', value)
    control.stop()
    for world in unreal.EditorLevelLibrary.get_pie_worlds(True):
        if LAG:
            unreal.SystemLibrary.execute_console_command(world, 'Net PktLag=0')
    (OUT / ('runtime-' + MODE + '-' + str(LAG) + 'ms.json')).write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
    scope['phase'] = 'closing'
    editor.editor_request_end_play()


def sample(worlds, now):
    """逐端采样胶囊、骨盆、主动作和外部 RMS，观察整段轨迹而非只看终点。"""
    for world in worlds:
        targets = find_targets(world)
        if not targets:
            continue
        target = targets[0]
        reaction = target.get_component_by_class(unreal.HodgeHitReactionComponent)
        state = reaction.get_reaction_state()
        location = target.get_actor_location()
        mesh = target.mesh
        motion = lib.inspect_reaction_motion(target)
        row = {'case': cases[scope['index']], 'time': now, 'server': unreal.SystemLibrary.is_server(world), 'world': world.get_path_name(),
               'locally_controlled': target.is_locally_controlled(),
               'avatar': target.get_path_name(),
               'player_id': target.get_editor_property('PlayerState').get_editor_property('PlayerId') if PLAYER_TARGET else None,
               'actor': [location.x, location.y, location.z], 'velocity': list_vector(target.get_velocity()),
               'mesh_relative': list_vector(mesh.get_relative_transform().translation),
               'root': list_vector(mesh.get_socket_transform('Root', unreal.RelativeTransformSpace.RTS_COMPONENT).translation),
               'pelvis': list_vector(mesh.get_socket_transform('Bip001Pelvis', unreal.RelativeTransformSpace.RTS_COMPONENT).translation),
               'chain_started': state.chain_started_at, 'air_started': state.air_started_at, 'air_hits': state.air_hit_count,
               'phase': str(state.phase), 'level': str(state.reaction_level), 'sequence': state.sequence, 'controlled': reaction.is_controlled(),
               'montage': state.montage.get_name() if state.montage else None,
               'montage_path': state.montage.get_path_name() if state.montage else None,
               'playing_root': motion.z, 'rms': motion.y,
               'hp': target.get_component_by_class(unreal.HodgeHealthComponent).get_health()}
        row['source_server'] = list_vector(scope['source'].get_actor_location())
        if state.montage:
            pose = lib.inspect_montage_state(target, state.montage)
            row.update(instance=pose.instance_id, position=pose.position, weight=pose.weight)
        report['samples'].append(row)


def list_vector(value):
    """转换采样向量为纯 JSON 数组，不保留引擎对象引用。"""
    return [value.x, value.y, value.z]


def find_targets(world):
    """使用PlayerState身份定位同一受击玩家，禁止把每个客户端的玩家0当成同一对象。"""
    values = unreal.GameplayStatics.get_all_actors_of_class(world, scope['target_class'])
    if not PLAYER_TARGET:
        return values
    return [actor for actor in values if actor.get_editor_property('PlayerState') and
            actor.get_editor_property('PlayerState').get_editor_property('PlayerId') == scope['target_player_id']]


def tick(delta):
    """按实际结算身份推进，所有等待有界，客户端状态不能替代服务器裁决。"""
    try:
        worlds = unreal.EditorLevelLibrary.get_pie_worlds(True)
        if scope['phase'] == 'closing':
            if worlds:
                return
            assert lib.configure_validation_pie(1 if old_mode == 0 else 2, old_mode == 2)
            for key, value in previous.items():
                play.set_editor_property(key, value)
            perf.set_editor_property('bThrottleCPUWhenNotForeground', throttle)
            for kind in scope['saved_hits']:
                asset = unreal.load_asset(control.ROOT + '/DA_Probe_' + kind)
                unreal.EditorLoadingAndSavingUtils.reload_packages([asset.get_outermost()], unreal.ReloadPackagesInteractionMode.ASSUME_POSITIVE)
            if scope.get('dirty_montage'):
                asset = unreal.load_asset(scope['dirty_montage'])
                unreal.EditorLoadingAndSavingUtils.reload_packages([asset.get_outermost()], unreal.ReloadPackagesInteractionMode.ASSUME_POSITIVE)
            unreal.unregister_slate_post_tick_callback(scope['callback'])
            print('POISE_RUNTIME_CLEANUP ' + MODE + ' ' + str(LAG) + ' ' + str(report['error']))
            return
        assert time.monotonic() - scope['wall'] < 170, '削韧真实命中验证超时'
        server = next((w for w in worlds if unreal.SystemLibrary.is_server(w)), None)
        if not server:
            return
        now = unreal.GameplayStatics.get_time_seconds(server)
        if scope['phase'] == 'boot':
            expected = 1 if MODE == 'single' else 3 if MODE == 'dedicated' else 2
            if len(worlds) != expected:
                return
            source = unreal.GameplayStatics.get_player_pawn(server, 0)
            if not isinstance(source, unreal.HodgeHeroCharacter) or not source.get_hodge_ability_system_component():
                return
            if PLAYER_TARGET:
                target = unreal.GameplayStatics.get_player_pawn(server, 1)
                if not isinstance(target, unreal.HodgeHeroCharacter) or not target.get_hodge_ability_system_component():
                    return
                reaction = target.get_component_by_class(unreal.HodgeHitReactionComponent)
                if not reaction.get_profile():
                    return
                assert target.get_hodge_ability_system_component().get_owner() == target.get_editor_property('PlayerState'), '受击玩家ASC必须由PlayerState持有'
                cls = unreal.HodgeHeroCharacter
                targets = [target]
                scope['target_player_id'] = target.get_editor_property('PlayerState').get_editor_property('PlayerId')
                dummy_class = unreal.load_asset(control.ROOT + '/BP_HitReactionDummy').generated_class()
                for dummy in unreal.GameplayStatics.get_all_actors_of_class(server, dummy_class):
                    dummy.set_actor_location(unreal.Vector(5000, 5000, 95), False, True)
                control.start(target)
            else:
                cls = unreal.load_asset(control.ROOT + '/BP_HitReactionDummy').generated_class()
                targets = unreal.GameplayStatics.get_all_actors_of_class(server, cls)
                if len(targets) != 1 or not targets[0].initialize_enemy_ability_system():
                    return
                control.start()
            for world in worlds:
                if LAG:
                    unreal.SystemLibrary.execute_console_command(world, 'Net PktLag=' + str(LAG))
            scope['max_air_hits'] = targets[0].get_component_by_class(unreal.HodgeHitReactionComponent).get_profile().get_editor_property('MaxAirHits')
            scope.update(source=source, target=targets[0], reaction=targets[0].get_component_by_class(unreal.HodgeHitReactionComponent), target_class=cls, phase='reset')
        if scope['phase'] == 'reset':
            control.reset()
            scope.update(phase='settle', at=now, checks=[], event_ids={scope['reaction'].get_last_result().hit_id.export_text()}, canceled=False, followup=False, first_hit=None, rows_from=len(report['samples']), events_from=len(report['events']))
            return
        if scope['phase'] == 'settle':
            if now - scope['at'] < 1.8:
                return
            scope['hp'] = scope['target'].get_component_by_class(unreal.HodgeHealthComponent).get_health()
            configure_request()
            scope.update(phase='observe', at=now)
        if scope['phase'] != 'observe':
            return
        result = scope['reaction'].get_last_result()
        identity = result.hit_id.export_text()
        if identity != unreal.Guid().export_text() and identity not in scope['event_ids']:
            scope['event_ids'].add(identity)
            report['events'].append({'case': cases[scope['index']], 'time': now, 'id': identity, 'level': str(result.reaction_level),
                                     'outcome': str(result.outcome), 'poise': result.accumulated_poise, 'damage': result.actual_damage})
            if result.actual_damage > 0. and scope['target'].get_component_by_class(unreal.HodgeHealthComponent).get_health() < scope['hp']:
                scope['first_hit'] = scope['first_hit'] or now
        if cases[scope['index']] == 'Cancel' and scope['first_hit'] and now - scope['first_hit'] > .1 and not scope['canceled']:
            scope['reaction'].cancel_reaction()
            scope['canceled'] = True
        if cases[scope['index']] in ['FlyRehitEnabled', 'FlyRehitDisabled', 'FlyLimit'] and scope['first_hit'] and now - scope['first_hit'] > .2 and not scope['followup']:
            if scope['target'].character_movement.is_falling():
                # 追击仍由真实新能力触发，不能直接写目标的 Phase 或胶囊速度。
                montage = unreal.load_asset(control.ROOT + '/DA_Probe_Launch').get_editor_property('ExecutionConfig').montage
                for event in unreal.AnimationLibrary.get_animation_notify_events(montage):
                    if isinstance(event.notify, unreal.HodgeAnimNotify_Hit):
                        value = clone(event.notify.get_editor_property('Hit'))
                        value.set_editor_property('bCanLaunch', cases[scope['index']] != 'FlyRehitDisabled')
                        event.notify.set_editor_property('Hit', value)
                control.probe('Launch' if cases[scope['index']] != 'FlyRehitDisabled' else 'HitStun')
                scope['followup'] = True
        if now - scope['last_sample'] >= .02:
            sample(worlds, now)
            scope['last_sample'] = now
        if now - scope['at'] < (10. if cases[scope['index']] in ['FlyRehitEnabled', 'FlyRehitDisabled', 'FlyLimit'] else 5.5):
            return
        name = cases[scope['index']]
        rows = report['samples'][scope['rows_from']:]
        authority = [r for r in rows if r['server']]
        events = [e for e in report['events'][scope['events_from']:] if e['damage'] > 0.]
        check('真实伤害进入结算', scope['target'].get_component_by_class(unreal.HodgeHealthComponent).get_health() < scope['hp'])
        check('全程受击不施加动画根运动', not any(r['playing_root'] for r in rows))
        # 空中与角色碰撞时 CMC 合法滑动可能改变水平速度，不能套用地面退步的单调性断言。
        if name not in ['FlyRehitEnabled', 'FlyRehitDisabled', 'FlyLimit']:
            check('隔离轨迹没有权威胶囊反向回弹', min([b['actor'][0] - a['actor'][0] for a, b in zip(authority, authority[1:])] or [0]) >= -.2)
        if name == 'LightL':
            check('轻硬直选择 S_L', any(r['montage'] == 'AM_IP_Behit_S_L' for r in authority))
            check('轻硬直不移动胶囊', max(r['actor'][0] for r in authority) - min(r['actor'][0] for r in authority) < .2)
        if name in ['HeavyR', 'HeavyL']:
            check('重硬直选择左右 B 动作并由 RMS 移动', any(r['montage'] == 'AM_IP_Behit_B_' + name[-1] and r['rms'] for r in authority))
            check('目标重击距离约65cm', abs(scope['target'].get_actor_location().x - 245.) < 3.)
        if name == 'Knockback':
            check('相同重受击动作可执行150cm', abs(scope['target'].get_actor_location().x - 330.) < 3.)
        if name == 'Armor':
            check('高抗目标只接触不受控且不积累', not any(r['controlled'] for r in authority) and scope['reaction'].get_accumulated_poise() == 0.)
            check('高抗命中保留低判定结果', any('LOW_JUDGEMENT' in e['outcome'] for e in events))
            unreal.AbilitySystemLibrary.remove_loose_gameplay_tags(scope['target'], scope.pop('armor_tags'), True)
        if name in ['HugePoise', 'DenseThresholds']:
            check('未开击飞时任意累计削韧最多重僵直', any('HEAVY' in e['level'] for e in events) and not any('AIRBORNE' in r['phase'] for r in authority))
        if name in ['FlyRehitEnabled', 'FlyRehitDisabled', 'FlyLimit']:
            check('初始分段击飞真实进入 Fly 三段', all(any(r['montage'] == 'AM_IP_Behit_Fly_' + suffix for r in authority) for suffix in ['Start', 'Loop', 'Fall']))
            starts = [r for r in authority if r['montage'] == 'AM_IP_Behit_Fly_Start' and r.get('instance', -1) >= 0]
            instances = {r['instance'] for r in starts}
            check('空中再次命中经过真实伤害', scope['followup'] and len(events) == 2, events)
            check('Fly 重播只取决于开关及次数上限', len(instances) == (2 if name == 'FlyRehitEnabled' else 1), sorted(instances))
            check('未改播 Push 充当 Fly', not any(r['montage'] and 'Push' in r['montage'] for r in authority))
            check('真实落地后播放起身', any(r['montage'] == 'AM_IP_StandUp' for r in authority))
            check('续飞沿用原控制链起点', len({round(r['chain_started'], 4) for r in authority if r['controlled']}) == 1)
            scope['reaction'].get_profile().set_editor_property('MaxAirHits', scope['max_air_hits'])
        if name == 'DenseThresholds':
            observed = [e['level'] for e in events]
            check('15/20/25/50只消费轻重且无破韧击飞', len(events) == 4 and all(any(level in value for value in observed) for level in ['NONE', 'LIGHT', 'HEAVY']) and not any('BREAK' in value for value in observed), observed)
        if name == 'DenseNoRefresh':
            check('密集同级四次真实命中仅一次强受击', len(events) == 4 and sum('APPLIED' in e['outcome'] for e in events) == 1)
            check('密集同级不重播主动作', len({r['instance'] for r in authority if r.get('montage') and r.get('instance', -1) >= 0}) == 1)
        for world in worlds:
            targets = find_targets(world)
            check('各端最终位置收敛:' + world.get_path_name(), bool(targets) and math.dist(list_vector(targets[0].get_actor_location()), list_vector(scope['target'].get_actor_location())) < 5.)
            target = targets[0]
            motion = lib.inspect_reaction_motion(target)
            used_montages = {row['montage_path'] for row in rows if row['world'] == world.get_path_name() and row['montage_path']}
            active = []
            for path in sorted(used_montages):
                # 原逐帧采样保留最新实例，最终释放必须检查同一资源的全部实例。
                active.extend(active_montage_instances(path, lib.inspect_montage_states(target, unreal.load_asset(path))))
            snapshot = {'world': world.get_path_name(), 'avatar': target.get_path_name(),
                        'server': unreal.SystemLibrary.is_server(world), 'locally_controlled': target.is_locally_controlled(),
                        'controlled': target.get_component_by_class(unreal.HodgeHitReactionComponent).is_controlled(),
                        'rms': motion.y, 'animation_root': motion.x, 'playing_root': motion.z,
                        'active_reaction_montages': active}
            report.setdefault('release_snapshots', []).append({'case': name, **snapshot})
            check('本端控制、动画和外部运动已释放:' + world.get_path_name(), reaction_snapshot_released(snapshot), snapshot)
        if PLAYER_TARGET:
            check('真实受击拥有者客户端参与观察', has_client_control_sample(rows, scope['target_player_id'], False))
            if MODE == 'dedicated':
                check('独立观察客户端收到受击阶段', has_client_control_sample(rows, scope['target_player_id'], True))
            else:
                # 双人 Listen 的攻击者即主机，不能虚构第三个远端观察者。
                check('Listen主机观察远端受击玩家', any(r['server'] and not r['locally_controlled'] and
                      r['player_id'] == scope['target_player_id'] and r['controlled'] for r in rows))
        report['cases'].append({'name': name, 'checks': scope['checks'], 'events': len(events)})
        scope['checks'] = []
        scope['index'] += 1
        if scope['index'] == len(cases):
            finish()
        else:
            scope['phase'] = 'reset'
    except Exception:
        finish(traceback.format_exc())


scope['callback'] = unreal.register_slate_post_tick_callback(tick)
editor.editor_request_begin_play()
print('POISE_RUNTIME_STARTED ' + MODE + ' ' + str(LAG))
