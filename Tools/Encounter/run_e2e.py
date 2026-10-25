"""正式示例的 F 键启动、真实 GAS 清波、库存领奖与区域冷却重入验证。"""
from pathlib import Path
import json
import time
import traceback
import unreal

WORK = Path(globals()['LOCK_ON_WORK'])
MODE = globals()['LOCK_ON_MODE']
worlds = unreal.EditorLevelLibrary.get_pie_worlds(True)
server = next(w for w in worlds if unreal.SystemLibrary.is_server(w))
clients = [w for w in worlds if w != server]
owner_world = clients[0] if clients else server
owner = next(p for p in unreal.GameplayStatics.get_all_actors_of_class(owner_world, unreal.HodgeHeroCharacter)
             if p.get_controller() and p.get_controller().is_local_controller())
pc = owner.get_controller()
ps = owner.get_editor_property('PlayerState')
pid = ps.get_editor_property('PlayerId')
authority = next(p for p in unreal.GameplayStatics.get_all_actors_of_class(server, unreal.HodgeHeroCharacter)
                 if p.get_editor_property('PlayerState').get_editor_property('PlayerId') == pid)
authority_ps = authority.get_editor_property('PlayerState')
inventory = authority_ps.get_component_by_class(unreal.HodgeInventoryManagerComponent)
local_inventory = ps.get_component_by_class(unreal.HodgeInventoryManagerComponent)
material = unreal.load_asset('/Game/Main/Data/Inventory/ID_TestMaterial').generated_class()
E = unreal.HodgeEncounterState
lag = 100 if '100' in WORK.name else 0
for world in worlds:
    unreal.SystemLibrary.execute_console_command(world, 'Net PktLag=' + str(lag))


def encounter(world, mode):
    """每类示例唯一，依据配置模式找实例，不依赖跨端对象包装身份。"""
    return next(a for a in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.HodgeEncounterSpawner)
                if a.get_editor_property('Definition').get_editor_property('Mode') == mode)


switch = encounter(server, unreal.HodgeEncounterMode.INTERACTION)
area = encounter(server, unreal.HodgeEncounterMode.AREA)
local_switch = encounter(owner_world, unreal.HodgeEncounterMode.INTERACTION)
checks = []
baseline = inventory.get_total_item_count_by_definition(material)
state = {'phase': 'initial', 'at': time.monotonic(), 'start': time.monotonic(), 'held': False}
other_counts = {}
for pawn in unreal.GameplayStatics.get_all_actors_of_class(server, unreal.HodgeHeroCharacter):
    other_ps = pawn.get_editor_property('PlayerState')
    if other_ps.get_editor_property('PlayerId') != pid:
        # 只调整隔离场景位置，防止另一个玩家挡住交互线，不修改其参数或库存。
        pawn.set_actor_location(unreal.Vector(-800, -800, 100), False, True)
        inv = other_ps.get_component_by_class(unreal.HodgeInventoryManagerComponent)
        other_counts[other_ps.get_editor_property('PlayerId')] = (inv, inv.get_total_item_count_by_definition(material))
authority.set_actor_location(unreal.Vector(0, 0, 100), False, True)


def check(name, value):
    """断言以实际权威状态和客户端读回为准，保留失败时的全部已有证据。"""
    checks.append({'name': name, 'passed': bool(value)})
    assert value, name


def phase(name):
    state['phase'] = name
    state['at'] = time.monotonic()


def key(pressed):
    """通过真实 F 键、正式 IMC 和 GAS，不能直接调用启动或领奖代替输入。"""
    assert unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc, 'InteractionKey', 1 if pressed else 0)
    state['held'] = pressed


def ready(actor, wave, count):
    snapshot = actor.get_snapshot()
    return snapshot.state == E.SPAWNING and snapshot.wave_index == wave and snapshot.alive_count == count and snapshot.pending_count == 0


def kill(actor, count):
    """由现有下一 Tick Damage GE 经过 Health／死亡能力清怪，不直接改清波计数。"""
    enemies = actor.get_living_enemies()
    assert len(enemies) >= count
    for enemy in enemies[:count]:
        check('敌人沿真实 EnemyInit 和 Brain 就绪', enemy.get_enemy_init_component().is_gameplay_ready() and
              enemy.get_controller().get_enemy_brain_state() == unreal.HodgeEnemyBrainState.RUNNING)
        assert unreal.HodgeCombatValidationLibrary.queue_number_pop_probe(authority, enemy, 100000.0, False)


def presentation(expected_state, visible, text=None):
    """各显示端依据收到的同轮快照更新组件；专用服务器不要求创建 UI。"""
    for world in worlds:
        instance = encounter(world, unreal.HodgeEncounterMode.INTERACTION)
        check('各端交互状态收敛', instance.get_snapshot().state == expected_state)
        check('各端开关显隐正确', instance.get_component_by_class(unreal.StaticMeshComponent).is_visible() == visible)
        if text and (world != server or MODE != 'dedicated'):
            widget = instance.get_component_by_class(unreal.WidgetComponent).get_user_widget_object()
            check('提示组件使用当前状态文字', widget and text in str(widget.get_prompt_text()))


def finish(error=None):
    """释放键状态，退出由批处理负责；不保存任何 PIE 对象或正式参数。"""
    unreal.unregister_slate_post_tick_callback(state['callback'])
    key(False)
    result = {'mode': WORK.name, 'pie_mode': MODE, 'lag_ms': lag, 'passed': error is None,
              'checks': checks, 'error': error, 'phase': state['phase'], 'elapsed': time.monotonic() - state['start'],
              'switch': str(switch.get_snapshot()), 'area': str(area.get_snapshot()), 'visual_review': False,
              'kill_path': 'authoritative Damage GE -> Health -> DeathStarted', 'formal_cooldown_seconds': 30}
    (WORK / (MODE + '-encounter-e2e.json')).write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')


def tick(delta):
    try:
        if time.monotonic() - state['start'] > 150:
            raise TimeoutError(state['phase'] + ' switch=' + str(switch.get_snapshot()) + ' area=' + str(area.get_snapshot()))
        current = state['phase']
        elapsed = time.monotonic() - state['at']
        if current == 'initial' and elapsed > 1.5:
            check('交互和区域初始均未激活', switch.get_snapshot().state == E.INACTIVE and area.get_snapshot().state == E.INACTIVE)
            check('库存已沿正式配置初始化', inventory.is_initialized() and local_inventory.is_initialized())
            presentation(E.INACTIVE, True, '启动挑战')
            key(True)
            phase('wave0')
        elif current == 'wave0' and elapsed > 1.5 and ready(switch, 0, 1):
            key(False)
            presentation(E.SPAWNING, False)
            state['first_run'] = switch.get_snapshot().run_id.to_string()
            check('首位玩家 PlayerState 持有本轮', switch.get_snapshot().owner_player_state == authority_ps)
            kill(switch, 1)
            phase('wave1')
        elif current == 'wave1' and elapsed > 1 and ready(switch, 1, 2):
            check('第二波确实包含两只真实敌人', len(switch.get_living_enemies()) == 2)
            kill(switch, 1)
            phase('one_left')
        elif current == 'one_left' and elapsed > 1 and ready(switch, 1, 1):
            check('最后一只未死亡时不结算', switch.get_snapshot().state != E.AWAITING_REWARD)
            kill(switch, 1)
            phase('reward')
        elif current == 'reward' and elapsed > 1.5 and switch.get_snapshot().state == E.AWAITING_REWARD:
            presentation(E.AWAITING_REWARD, True, '领取奖励')
            check('清怪不提前入库', inventory.get_total_item_count_by_definition(material) == baseline)
            key(True)
            phase('claim')
        elif current == 'claim' and elapsed > 2:
            check('领奖真实增加一件物品', inventory.get_total_item_count_by_definition(material) == baseline + 1)
            check('拥有者私有库存复制收敛', local_inventory.get_total_item_count_by_definition(material) == baseline + 1)
            check('其他玩家没有被误发奖励', all(inv.get_total_item_count_by_definition(material) == old for inv, old in other_counts.values()))
            check('长按不重复领奖或重启新挑战', switch.get_snapshot().state == E.INACTIVE and not switch.get_living_enemies())
            check('领奖后初始选项使用新轮次身份', switch.get_snapshot().run_id.to_string() != state['first_run'])
            presentation(E.INACTIVE, True, '启动挑战')
            key(False)
            authority.set_actor_location(area.get_actor_location(), False, True)
            phase('area0')
        elif current == 'area0' and elapsed > 1 and ready(area, 0, 1):
            check('进入实际 Box 自动启动区域', area.get_snapshot().owner_player_state == authority_ps)
            kill(area, 1)
            phase('area1')
        elif current == 'area1' and elapsed > 1 and ready(area, 1, 2):
            kill(area, 2)
            phase('cooldown')
        elif current == 'cooldown' and elapsed > 1.5 and area.get_snapshot().state == E.COMPLETED:
            check('区域清场自动完成且不发奖励', inventory.get_total_item_count_by_definition(material) == baseline + 1)
            check('区域实例始终隐藏交互开关', not area.get_component_by_class(unreal.StaticMeshComponent).is_visible())
            state['area_run'] = area.get_snapshot().run_id.to_string()
            state['deadline'] = area.get_snapshot().cooldown_end_server_time
            check('正式区域采用30秒冷却', 27 < state['deadline'] - unreal.GameplayStatics.get_time_seconds(server) <= 30.1)
            phase('stay_inside')
        elif current == 'stay_inside' and unreal.GameplayStatics.get_time_seconds(server) > state['deadline'] + .3:
            check('冷却结束仍在区内不自动重刷', area.get_snapshot().state == E.COMPLETED and not area.get_living_enemies())
            check('结束后仍要求全员离开', area.get_snapshot().get_editor_property('bNeedsExit'))
            authority.set_actor_location(unreal.Vector(0, 0, 100), False, True)
            phase('leave')
        elif current == 'leave' and elapsed > 1.5:
            check('冷却且全员离开只重新武装', area.get_snapshot().state == E.INACTIVE and not area.get_living_enemies())
            authority.set_actor_location(area.get_actor_location(), False, True)
            phase('repeat0')
        elif current == 'repeat0' and elapsed > 1 and ready(area, 0, 1):
            check('新进入后才建立新轮次', area.get_snapshot().run_id.to_string() != state['area_run'])
            kill(area, 1)
            phase('repeat1')
        elif current == 'repeat1' and elapsed > 1 and ready(area, 1, 2):
            kill(area, 2)
            phase('complete')
        elif current == 'complete' and elapsed > 1.5 and area.get_snapshot().state == E.COMPLETED:
            check('第二次区域完成仍不额外发奖', inventory.get_total_item_count_by_definition(material) == baseline + 1)
            check('所有本轮存活敌人均由死亡清空', not area.get_living_enemies() and not switch.get_living_enemies())
            for world in worlds:
                replica = encounter(world, unreal.HodgeEncounterMode.AREA)
                check('区域完成状态与计数复制收敛', replica.get_snapshot().state == E.COMPLETED and replica.get_snapshot().alive_count == 0)
            finish()
    except Exception:
        finish(traceback.format_exc())


state['callback'] = unreal.register_slate_post_tick_callback(tick)
