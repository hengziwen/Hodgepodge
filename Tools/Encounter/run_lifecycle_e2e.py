"""验证待领取死亡后换 Pawn 领取，以及战斗死亡取消；沿用正式自动重生。"""
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
local_ps = owner.get_editor_property('PlayerState')
pid = local_ps.get_editor_property('PlayerId')
E = unreal.HodgeEncounterState
lag = 100 if '100' in WORK.name else 0
for world in worlds:
    unreal.SystemLibrary.execute_console_command(world, 'Net PktLag=' + str(lag))
switch = next(a for a in unreal.GameplayStatics.get_all_actors_of_class(server, unreal.HodgeEncounterSpawner)
              if a.get_editor_property('Definition').get_editor_property('Mode') == unreal.HodgeEncounterMode.INTERACTION)
material = unreal.load_asset('/Game/Main/Data/Inventory/ID_TestMaterial').generated_class()
checks = []
state = {'phase': 'initial', 'at': time.monotonic(), 'start': time.monotonic()}


def authority():
    """每次按持久 PlayerState 身份重新找 Pawn，不缓存已经死亡的身体。"""
    return next((p for p in unreal.GameplayStatics.get_all_actors_of_class(server, unreal.HodgeHeroCharacter)
                 if p.get_editor_property('PlayerState') and p.get_editor_property('PlayerState').get_editor_property('PlayerId') == pid), None)


original = authority()
ps = original.get_editor_property('PlayerState')
inventory = ps.get_component_by_class(unreal.HodgeInventoryManagerComponent)
baseline = inventory.get_total_item_count_by_definition(material)
original.set_actor_location(unreal.Vector(0, 0, 100), False, True)
for pawn in unreal.GameplayStatics.get_all_actors_of_class(server, unreal.HodgeHeroCharacter):
    if pawn != original:
        pawn.set_actor_location(unreal.Vector(-800, -800, 100), False, True)


def check(name, value):
    """失败保留阶段与已有断言，不跳过不匹配的网络样本。"""
    checks.append({'name': name, 'passed': bool(value)})
    assert value, name


def phase(name):
    state['phase'] = name
    state['at'] = time.monotonic()


def key(value):
    """真实 F 键输入经过正式 IMC／GA／拥有者 RPC。"""
    assert unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc, 'InteractionKey', 1 if value else 0)


def ready(wave, count):
    snapshot = switch.get_snapshot()
    return snapshot.state == E.SPAWNING and snapshot.wave_index == wave and snapshot.alive_count == count and snapshot.pending_count == 0


def kill_enemies():
    """真实 Damage GE 经既有死亡能力清场，不直接发送刷怪器死亡回调。"""
    for enemy in switch.get_living_enemies():
        assert unreal.HodgeCombatValidationLibrary.queue_number_pop_probe(authority(), enemy, 100000, False)


def kill_player():
    """与既有重生专项相同的自伤路径，不调用 FinishDeath 或 RestartPlayer。"""
    pawn = authority()
    state['old_pawn'] = pawn.get_path_name()
    assert unreal.HodgeCombatValidationLibrary.queue_number_pop_probe(pawn, pawn, 1000000, False)


def new_pawn_ready():
    pawn = authority()
    local = pc.get_controlled_pawn()
    return bool(pawn and pawn.get_path_name() != state['old_pawn'] and local and
                local.get_component_by_class(unreal.HodgeLockOnComponent).is_ready() and
                pawn.get_component_by_class(unreal.HodgeHealthComponent).get_death_state() == unreal.HodgeDeathState.NOT_DEAD)


def finish(error=None):
    """仅结束输入和写报告，PIE 由调度器统一关闭。"""
    unreal.unregister_slate_post_tick_callback(state['callback'])
    key(False)
    (WORK / (MODE + '-encounter-lifecycle.json')).write_text(json.dumps(
        {'mode': WORK.name, 'pie_mode': MODE, 'lag_ms': lag, 'passed': error is None, 'checks': checks,
         'error': error, 'phase': state['phase'], 'elapsed': time.monotonic() - state['start'],
         'respawn_settings_modified': False, 'disconnect_socket_test': False}, ensure_ascii=False, indent=2), encoding='utf-8')


def tick(delta):
    try:
        if time.monotonic() - state['start'] > 100:
            raise TimeoutError(state['phase'])
        current = state['phase']
        elapsed = time.monotonic() - state['at']
        if current == 'initial' and elapsed > 1.5:
            key(True)
            phase('wave0')
        elif current == 'wave0' and elapsed > 1 and ready(0, 1):
            key(False)
            kill_enemies()
            phase('wave1')
        elif current == 'wave1' and elapsed > 1 and ready(1, 2):
            kill_enemies()
            phase('await')
        elif current == 'await' and elapsed > 1.5 and switch.get_snapshot().state == E.AWAITING_REWARD:
            state['reward_run'] = switch.get_snapshot().run_id.to_string()
            check('领奖前未提前发奖', inventory.get_total_item_count_by_definition(material) == baseline)
            kill_player()
            phase('await_dead')
        elif current == 'await_dead' and elapsed > .5:
            check('待领取死亡仍保持本轮和资格', switch.get_snapshot().state == E.AWAITING_REWARD and
                  switch.get_snapshot().run_id.to_string() == state['reward_run'] and switch.get_snapshot().owner_player_state == ps)
            phase('await_respawn')
        elif current == 'await_respawn' and new_pawn_ready():
            check('正式自动重生生成新 Pawn', True)
            check('PlayerState 与库存沿用原身份', authority().get_editor_property('PlayerState') == ps and pc.get_controlled_pawn().get_editor_property('PlayerState') == local_ps)
            authority().set_actor_location(unreal.Vector(0, 0, 100), False, True)
            phase('new_claim')
        elif current == 'new_claim' and elapsed > 1.5:
            key(True)
            phase('claimed')
        elif current == 'claimed' and elapsed > 2:
            check('新 Pawn 成功领取旧轮奖励一次', inventory.get_total_item_count_by_definition(material) == baseline + 1 and switch.get_snapshot().state == E.INACTIVE)
            key(False)
            phase('restart')
        elif current == 'restart' and elapsed > .6:
            key(True)
            phase('active')
        elif current == 'active' and elapsed > 1 and ready(0, 1):
            key(False)
            state['active_enemies'] = list(switch.get_living_enemies())
            kill_player()
            phase('cancelled')
        elif current == 'cancelled' and elapsed > .5:
            check('战斗死亡终止且恢复开关', switch.get_snapshot().state == E.INACTIVE and bool(switch.get_snapshot().last_failure))
            check('本轮存活敌人主动清理', not switch.get_living_enemies() and all(not unreal.SystemLibrary.is_valid(e) for e in state['active_enemies']))
            check('失败不会额外发奖', inventory.get_total_item_count_by_definition(material) == baseline + 1)
            state['cancel_epoch'] = switch.get_snapshot().run_id.to_string()
            phase('cancel_respawn')
        elif current == 'cancel_respawn' and new_pawn_ready():
            phase('settled')
        elif current == 'settled' and elapsed > 1.5:
            check('重生和旧回调不会恢复已取消轮次', switch.get_snapshot().state == E.INACTIVE and
                  switch.get_snapshot().run_id.to_string() == state['cancel_epoch'] and not switch.get_living_enemies())
            check('奖励数量跨两次死亡保持一致', inventory.get_total_item_count_by_definition(material) == baseline + 1)
            finish()
    except Exception:
        finish(traceback.format_exc())


state['callback'] = unreal.register_slate_post_tick_callback(tick)
