"""仅在编辑器 PIE 中观察假人并通过真实能力触发测试；不修改任何资产。"""
import json
import math
import time
from pathlib import Path

import unreal

ROOT = '/Game/CodexText/HitReactionValidation/Dummy'
KINDS = ('Light', 'HitStun', 'Knockback', 'Launch', 'AirHit', 'Slam', 'Knockdown')
_state = {}


def snapshot():
    """读取真实生命、控制阶段、动画权重和胶囊运动，不补造接触信息。"""
    dummy = _state['dummy']
    health = dummy.get_component_by_class(unreal.HodgeHealthComponent)
    reaction = dummy.get_component_by_class(unreal.HodgeHitReactionComponent)
    state = reaction.get_reaction_state()
    location = dummy.get_actor_location()
    velocity = dummy.get_velocity()
    row = {'time': unreal.GameplayStatics.get_time_seconds(_state['world']),
           'health': health.get_health(), 'max_health': health.get_max_health(),
           'phase': str(state.phase), 'type': str(state.type), 'sequence': state.sequence,
           'position': [location.x, location.y, location.z],
           'velocity': [velocity.x, velocity.y, velocity.z],
           'controlled': reaction.is_controlled(), 'air_hits': state.air_hit_count}
    # 削韧只在服务器采样，目标等级与执行阶段分别显示，避免把表现力度当玩法阈值。
    row.update(poise=reaction.get_accumulated_poise(), reaction_level=str(state.reaction_level))
    profile = reaction.get_profile()
    selected = state.montage or (profile.get_editor_property('LightFeedbackMontage') if profile else None)
    if selected:
        montage = unreal.HodgeCombatValidationLibrary.inspect_montage_state(dummy, selected)
        row.update(montage=selected.get_path_name(), montage_position=montage.position,
                   montage_weight=montage.weight, montage_playing=montage.playing)
    return row


def record(event, **values):
    """事件保留有界记录，画面与 Output Log 同时显示，不逐帧刷屏。"""
    if not _state:
        return
    row = {'event': event, **snapshot(), **values}
    _state['records'].append(row)
    if len(_state['records']) > 5000:
        del _state['records'][:1000]
    text = '[HitDummy] ' + json.dumps(row, ensure_ascii=False)
    unreal.log(text)
    if 'outcome' in values:
        _state['last_outcome'] = values['outcome']
    # 完整上下文保留在日志；画面只显示一条摘要，避免遮挡实际受击动作。
    screen = 'HP %.2f / %.0f | %s | %s | %s' % (row['health'], row['max_health'], event, row['phase'], _state.get('last_outcome', ''))
    unreal.SystemLibrary.print_string(_state['dummy'], screen, True, False, unreal.LinearColor(0.2, 1.0, 0.3, 1.0), 3.0, key='HitDummy')


@unreal.uclass()
class HodgeDummyObserver(unreal.Object):
    """使用持有的 UObject 监听器，PIE 结束后配对解绑委托。"""

    @unreal.ufunction(params=[unreal.Vector])
    def light(self, direction):
        record('light', direction=[direction.x, direction.y, direction.z])

    @unreal.ufunction(params=[unreal.HodgeHealthComponent, float, float, unreal.Actor])
    def health(self, component, old_value, new_value, instigator):
        # 生命变化可能先于 FinishHit，此时不把旧 LastResult 当作本次结算。
        record('health', old_health=old_value, new_health=new_value,
               instigator=instigator.get_name() if instigator else None)


def start(target=None):
    """默认观察假人；专项联机回归可明确指定服务器上的真实玩家受击目标。"""
    if _state:
        stop()
    worlds = [w for w in unreal.EditorLevelLibrary.get_pie_worlds(True) if unreal.SystemLibrary.is_server(w)]
    assert len(worlds) == 1, '请先在假人地图启动 PIE，且只观察一个服务器世界'
    world = worlds[0]
    if target is None:
        cls = unreal.load_asset(ROOT + '/BP_HitReactionDummy').generated_class()
        targets = unreal.GameplayStatics.get_all_actors_of_class(world, cls)
        assert len(targets) == 1, '地图应预放置一个假人'
        dummy = targets[0]
        assert dummy.initialize_enemy_ability_system(), '等待假人初始化完成'
    else:
        dummy = target
        assert dummy in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.HodgeHeroCharacter), '指定目标必须是服务器真实玩家'
        assert dummy.get_hodge_ability_system_component(), '等待玩家ASC绑定完成'
    source = unreal.GameplayStatics.get_player_pawn(world, 0)
    assert isinstance(source, unreal.HodgeHeroCharacter) and source != dummy, '等待不同攻击者和受击目标初始化完成'
    health = dummy.get_component_by_class(unreal.HodgeHealthComponent)
    assert target is not None or health.get_max_health() == 1000000.0, '假人属性没有按独立配置初始化'
    reaction = dummy.get_component_by_class(unreal.HodgeHitReactionComponent)
    observer = unreal.new_object(HodgeDummyObserver)
    _state.update(world=world, dummy=dummy, source=source, reaction=reaction, health=health,
                  observer=observer, records=[], manager=None, grant=None)
    reaction.on_light_feedback.add_function(observer, 'light')
    health.on_health_changed.add_function(observer, 'health')
    _state.update(last_hit=reaction.get_last_result().hit_id.export_text(), last_state=None, last_sample=-1.0)

    def ended(delta):
        """只读公开结果和阶段；同一帧多次结算可能合并为最后一条，不能充当完整遥测。"""
        if not unreal.SystemLibrary.is_valid(dummy) or not unreal.EditorLevelLibrary.get_pie_worlds(True):
            stop()
            return
        result = reaction.get_last_result()
        if result.hit_id.export_text() != _state['last_hit']:
            _state['last_hit'] = result.hit_id.export_text()
            record('reaction', result=result.export_text(), hit_id=result.hit_id.export_text(),
                   outcome=str(result.outcome), actual_damage=result.actual_damage,
                   attack_rank=result.attack_rank, body_rank=result.target_body_rank)
        current = reaction.get_reaction_state()
        identity = (current.sequence, str(current.phase), current.started_at)
        if identity != _state['last_state']:
            _state['last_state'] = identity
            record('state')
        now = unreal.GameplayStatics.get_time_seconds(world)
        if reaction.is_controlled() and now - _state['last_sample'] >= 0.1:
            _state['last_sample'] = now
            _state['records'].append({'event': 'sample', **snapshot()})

    _state['callback'] = unreal.register_slate_post_tick_callback(ended)
    record('attached')
    return snapshot()


def probe(kind='HitStun'):
    """装备一类独立测试能力，再由游戏 Tick 执行真实 Notify 和伤害 GE。"""
    assert kind in KINDS, '可选类型：' + ', '.join(KINDS)
    if not _state:
        start()
    source, dummy = _state['source'], _state['dummy']
    if kind in ('AirHit', 'Slam'):
        assert dummy.character_movement.is_falling(), '先 Launch，待目标离地后再执行 ' + kind
    delta = dummy.get_actor_location() - source.get_actor_location()
    assert delta.x * delta.x + delta.y * delta.y < 330.0 ** 2, '超出测试攻击范围；先执行 reset()'
    yaw = math.degrees(math.atan2(delta.y, delta.x))
    source.set_actor_rotation(unreal.Rotator(yaw=yaw), False)
    manager = source.get_component_by_class(unreal.HodgeEquipmentManagerComponent)
    assert manager, '玩家缺少正式装备管理组件'
    if _state.get('grant'):
        manager.unequip_item(_state['grant'])
        _state['grant'] = None
    grant_class = unreal.load_asset(ROOT + '/BP_ProbeGrant_' + kind).generated_class()
    grant = manager.equip_item(grant_class)
    assert grant, '测试能力授予失败；查看装备管理组件诊断'
    _state.update(manager=manager, grant=grant)
    asc = source.get_hodge_ability_system_component()
    definition = unreal.load_asset(ROOT + '/DA_Probe_' + kind)
    handles = [h for h in asc.get_all_abilities() if asc.find_ability_definition(h) == definition]
    assert len(handles) == 1, '测试能力句柄应唯一'
    if source.is_locally_controlled():
        assert unreal.HodgeCombatValidationLibrary.queue_ability_action(asc, handles[0], False)
    else:
        # 专服上的 LocalPredicted 动作由真正拥有者激活，并等待本次授予复制到达。
        source_id = source.get_editor_property('PlayerState').get_editor_property('PlayerId')
        pending = {'start': time.monotonic()}

        def activate_owner(delta):
            if not _state:
                unreal.unregister_slate_post_tick_callback(pending['handle'])
                return
            for world in unreal.EditorLevelLibrary.get_pie_worlds(True):
                owner = unreal.GameplayStatics.get_player_pawn(world, 0)
                player_state = owner.get_editor_property('PlayerState') if isinstance(owner, unreal.HodgeHeroCharacter) else None
                if unreal.SystemLibrary.is_server(world) or not player_state or player_state.get_editor_property('PlayerId') != source_id:
                    continue
                client_asc = owner.get_hodge_ability_system_component()
                # 同 Definition 重授予时旧句柄可能仍在路上，必须等到本次服务器句柄抵达。
                values = [h for h in client_asc.get_all_abilities() if h.export_text() == handles[0].export_text()
                          and client_asc.find_ability_definition(h) == definition] if client_asc else []
                if len(values) == 1:
                    owner.character_movement.stop_movement_immediately()
                    owner.set_actor_location(source.get_actor_location(), False, True)
                    owner.set_actor_rotation(unreal.Rotator(yaw=yaw), False)
                    assert unreal.HodgeCombatValidationLibrary.queue_ability_action(client_asc, values[0], False)
                    unreal.unregister_slate_post_tick_callback(pending['handle'])
                    _state.get('followups', []).remove(pending['handle'])
                    return
            if time.monotonic() - pending['start'] > 10:
                record('probe_failed', reason='拥有者能力授予未在期限内复制')
                unreal.unregister_slate_post_tick_callback(pending['handle'])
                _state.get('followups', []).remove(pending['handle'])

        pending['handle'] = unreal.register_slate_post_tick_callback(activate_owner)
        _state.setdefault('followups', []).append(pending['handle'])
    record('probe_queued', kind=kind, server_handle=handles[0].export_text())


def reset():
    """停止当前受击并重置站位；保留真实扣血，不伪装自动回血。"""
    if not _state:
        start()
    source, dummy = _state['source'], _state['dummy']
    _state['reaction'].cancel_reaction()
    for actor, x, yaw in [(source, 0, 0), (dummy, 180, 180)]:
        actor.character_movement.stop_movement_immediately()
        actor.set_actor_location(unreal.Vector(x, 0, 95), False, True)
        actor.set_actor_rotation(unreal.Rotator(yaw=yaw), False)
        actor.character_movement.set_movement_mode(unreal.MovementMode.MOVE_WALKING)
    # 远端拥有者有自主移动预测，测试站位需同步设置；只移动服务端会被旧客户端位置纠正。
    for actor, x, yaw in [(source, 0, 0), (dummy, 180, 180)]:
        state = actor.get_editor_property('PlayerState')
        if not state:
            continue
        player_id = state.get_editor_property('PlayerId')
        for world in unreal.EditorLevelLibrary.get_pie_worlds(True):
            owner = unreal.GameplayStatics.get_player_pawn(world, 0)
            player_state = owner.get_editor_property('PlayerState') if owner else None
            if owner and owner != actor and player_state and player_state.get_editor_property('PlayerId') == player_id:
                owner.character_movement.stop_movement_immediately()
                owner.set_actor_location(unreal.Vector(x, 0, 95), False, True)
                owner.set_actor_rotation(unreal.Rotator(yaw=yaw), False)
                owner.character_movement.set_movement_mode(unreal.MovementMode.MOVE_WALKING)
    record('reset')


def launch_then(kind='AirHit'):
    """在真实挑飞后延迟追击，避免手动切换 Python 控制台错过空中窗口。"""
    assert kind in ('AirHit', 'Slam')
    if not _state:
        start()
    reset()
    probe('Launch')
    world = _state['world']
    due = unreal.GameplayStatics.get_time_seconds(world) + 0.48
    holder = {}

    def follow(delta):
        if not _state or not unreal.SystemLibrary.is_valid(world):
            unreal.unregister_slate_post_tick_callback(holder['handle'])
            return
        if unreal.GameplayStatics.get_time_seconds(world) >= due and _state['dummy'].character_movement.is_falling():
            unreal.unregister_slate_post_tick_callback(holder['handle'])
            _state.get('followups', []).remove(holder['handle'])
            probe(kind)
        elif unreal.GameplayStatics.get_time_seconds(world) > due + 2.5:
            unreal.unregister_slate_post_tick_callback(holder['handle'])
            _state.get('followups', []).remove(holder['handle'])
            record('probe_failed', reason='未观察到实际离地，不伪造空中追击')

    holder['handle'] = unreal.register_slate_post_tick_callback(follow)
    _state.setdefault('followups', []).append(holder['handle'])


def stop():
    """撤销测试来源、解绑监听并保存报告，不影响玩家基础能力或正式装备。"""
    if not _state:
        return
    callback = _state.pop('callback', None)
    if callback:
        unreal.unregister_slate_post_tick_callback(callback)
    for handle in _state.pop('followups', []):
        unreal.unregister_slate_post_tick_callback(handle)
    observer = _state['observer']
    try:
        if unreal.SystemLibrary.is_valid(_state['dummy']):
            reaction = _state['reaction']
            reaction.on_light_feedback.remove_function(observer, 'light')
            _state['health'].on_health_changed.remove_function(observer, 'health')
        manager, grant = _state.get('manager'), _state.get('grant')
        if manager and grant and unreal.SystemLibrary.is_valid(manager) and unreal.SystemLibrary.is_valid(grant):
            manager.unequip_item(grant)
    finally:
        report = Path(unreal.Paths.project_saved_dir()) / 'HitReactionDummy' / 'observations.json'
        report.parent.mkdir(parents=True, exist_ok=True)
        report.write_text(json.dumps(_state['records'], ensure_ascii=False, indent=2), encoding='utf-8')
        _state.clear()
