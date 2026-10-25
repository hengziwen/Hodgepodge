"""PIE Dedicated 双玩家同时死亡：验证请求隔离、新 Avatar 收敛、私有库存与新身体输入。"""
from pathlib import Path
import gc
import json
import time
import traceback
import unreal

WORK = Path(globals()['LOCK_ON_WORK'])
MODE = globals()['LOCK_ON_MODE']
worlds = unreal.EditorLevelLibrary.get_pie_worlds(True)
server = next(w for w in worlds if unreal.SystemLibrary.is_server(w))
clients = [w for w in worlds if w != server]
for world in worlds:
    unreal.SystemLibrary.execute_console_command(world, 'Net PktLag=100')
targets = []
for pawn in unreal.GameplayStatics.get_all_actors_of_class(server, unreal.HodgeHeroCharacter):
    player = pawn.get_editor_property('PlayerState')
    selection = player.get_component_by_class(unreal.HodgeEquipmentSelectionComponent)
    targets.append({'pc': pawn.get_controller(), 'ps': player, 'asc': pawn.get_hodge_ability_system_component(),
                    'old': pawn.get_path_name(), 'pid': player.get_editor_property('PlayerId'),
                    'generation': player.get_attribute_ready_state().life_generation,
                    'selected': selection.get_selected_item_id().to_string(), 'finished': False})
assert len(targets) == 2
checks = []
state = {'phase': 'warmup', 'at': 0., 'wall': time.monotonic()}
e_tag = unreal.GameplayTag()
e_tag.import_text('(TagName="InputTag.Ability.ResonanceSkill")')
death_tag = unreal.GameplayTag()
death_tag.import_text('(TagName="Status.Death")')
query = unreal.GameplayEffectQuery()
query.set_editor_property('EffectDefinition', unreal.load_class(None,
    '/Game/Main/Character/Hero/Ability/ResonanceSkill/GE_Resonance_Skill_Cooldown.GE_Resonance_Skill_Cooldown_C'))


def check(name, passed, detail=None):
    """每名玩家独立断言，不能用总 Pawn 数掩盖身份错配。"""
    checks.append({'name': name, 'passed': bool(passed), 'detail': detail})
    assert passed, (name, detail)


def finish(error=None):
    """结果与引用清理配对，不保存 PIE 内的任何角色。"""
    unreal.unregister_slate_post_tick_callback(state['callback'])
    (WORK / (MODE + '-respawn-e2e.json')).write_text(json.dumps(
        {'mode': MODE, 'lag_ms': 100, 'simultaneous_players': 2, 'passed': error is None, 'checks': checks, 'error': error},
        ensure_ascii=False, indent=2), encoding='utf-8')
    targets.clear()
    for name in ['worlds', 'server', 'clients', 'pawn', 'player', 'selection']:
        globals()[name] = None
    gc.collect()


def tick(delta):
    """同一回调排入两份真实致死 GE，后续仅观察生产链的状态。"""
    try:
        assert time.monotonic() - state['wall'] < 40., '双玩家重生超时：' + state['phase']
        now = unreal.GameplayStatics.get_time_seconds(server)
        if state['phase'] == 'warmup':
            if state['at'] == 0.: state['at'] = now
            if now - state['at'] < 1.: return
            for row in targets:
                current = row['pc'].get_controlled_pawn()
                assert unreal.HodgeCombatValidationLibrary.queue_number_pop_probe(current, current, 1000000., False)
            state['phase'] = 'dying'
        elif state['phase'] == 'dying':
            for row in targets:
                current = row['pc'].get_controlled_pawn()
                if current and current.get_path_name() == row['old']:
                    if current.get_component_by_class(unreal.HodgeHealthComponent).get_death_state() == unreal.HodgeDeathState.DEATH_FINISHED:
                        row['finished'] = True
            if not all(row['pc'].get_controlled_pawn() and row['pc'].get_controlled_pawn().get_path_name() != row['old'] and
                       row['ps'].get_attribute_ready_state().ready for row in targets): return
            for row in targets:
                current = row['pc'].get_controlled_pawn()
                health = current.get_component_by_class(unreal.HodgeHealthComponent)
                check('双玩家完整死亡后分别生成', row['finished'])
                check('双玩家Controller与PlayerState身份独立保留', current.get_editor_property('PlayerState') == row['ps'])
                check('双玩家ASC与生命代正确', current.get_hodge_ability_system_component() == row['asc'] and row['ps'].get_attribute_ready_state().life_generation == row['generation'] + 1)
                check('双玩家最终满血与装备身份保留', health.get_health() == health.get_max_health() and
                      row['ps'].get_component_by_class(unreal.HodgeEquipmentSelectionComponent).get_selected_item_id().to_string() == row['selected'])
            state['phase'], state['at'] = 'peers', now
        elif state['phase'] == 'peers' and now - state['at'] > 1.:
            for world in clients:
                peers = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.HodgeHeroCharacter)
                if len(peers) != 2 or not all(p.get_editor_property('PlayerState').get_attribute_ready_state().ready and
                        p.get_editor_property('PlayerState').get_attribute_ready_state().avatar == p for p in peers): return
            for world in clients:
                for peer in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.HodgeHeroCharacter):
                    player = peer.get_editor_property('PlayerState')
                    if peer.get_controller() and peer.get_controller().is_local_controller():
                        ui = unreal.HodgeUIManagerSubsystem.get_gameplay_data_for_controller(peer.get_controller())
                        details = {'display': ui.get_ability_display_state(e_tag).export_text(), 'vitals': ui.get_vitals().export_text(),
                                   'asc_death_tag': peer.has_matching_gameplay_tag(death_tag)}
                        check('新身体拥有者能提交正式E输入', ui.submit_input(e_tag), details)
                    else:
                        inventory = player.get_component_by_class(unreal.HodgeInventoryManagerComponent)
                        check('模拟代理仍不可读取其他玩家私有库存', not inventory or len(inventory.get_items()) == 0)
            state['phase'], state['at'] = 'inputs', now
        elif state['phase'] == 'inputs' and now - state['at'] > 1.:
            check('两个拥有者新输入均通过服务器实际建立冷却', all(len(row['asc'].get_active_effects(query)) == 1 for row in targets))
            finish()
    except Exception:
        finish(traceback.format_exc())


state['callback'] = unreal.register_slate_post_tick_callback(tick)
