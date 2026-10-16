"""真实 PIE 数据层集成断言：RPC、私有复制、装备切换和重生；不截屏或录制。"""
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
owner_world = server if MODE == 'listen-host' or not clients else clients[0]
owner = next(p for p in unreal.GameplayStatics.get_all_actors_of_class(owner_world, unreal.HodgeHeroCharacter)
             if p.get_controller() and p.get_controller().is_local_controller())
pc = owner.get_controller()
pid = owner.get_editor_property('PlayerState').get_editor_property('PlayerId')
authority = next(p for p in unreal.GameplayStatics.get_all_actors_of_class(server, unreal.HodgeHeroCharacter)
                 if p.get_editor_property('PlayerState').get_editor_property('PlayerId') == pid)
source_pc = authority.get_controller()
ps = authority.get_editor_property('PlayerState')
inventory = ps.get_component_by_class(unreal.HodgeInventoryManagerComponent)
selection = ps.get_component_by_class(unreal.HodgeEquipmentSelectionComponent)
local_ps = owner.get_editor_property('PlayerState')
local_inventory = local_ps.get_component_by_class(unreal.HodgeInventoryManagerComponent)
local_selection = local_ps.get_component_by_class(unreal.HodgeEquipmentSelectionComponent)
checks = []
state = {'phase': 'initial', 'at': time.monotonic(), 'start': time.monotonic()}


def same_guid(left, right):
    """UE Python 的 Guid 包装对象按值比较，避免包装身份造成误报。"""
    return left.to_string() == right.to_string()


def check(name, value):
    """每条实际断言记录名称，失败保留到报告中。"""
    checks.append({'name': name, 'passed': bool(value)})
    assert value, name


def finish(error=None):
    """只输出数据证据，不把运行日志当视觉验收。"""
    unreal.unregister_slate_post_tick_callback(state['callback'])
    result = {'mode': MODE, 'passed': error is None, 'checks': checks, 'error': error,
              'visual_validation': False, 'elapsed': time.monotonic() - state['start']}
    (WORK / (MODE + '-inventory-e2e.json')).write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')


def next_phase(name):
    state['phase'] = name
    state['at'] = time.monotonic()


def tick(delta):
    try:
        if time.monotonic() - state['start'] > 100:
            raise TimeoutError('Inventory phase ' + state['phase'])
        phase = state['phase']
        if time.monotonic() - state['at'] < 1.5:
            return
        if phase == 'initial':
            check('official Experience injected inventory', bool(inventory and selection and local_inventory and local_selection))
            check('initial default exists once on server', inventory.is_initialized() and len(inventory.get_items()) == 1)
            check('initial default replicated to owner', local_inventory.is_initialized() and len(local_inventory.get_items()) == 1)
            check('initial selected weapon applied', selection.is_selection_applied() and local_selection.is_selection_applied())
            state['original'] = selection.get_selected_item_id()
            state['sword'] = inventory.get_items()[0].instance.get_item_def()
            check('second weapon granted by server', inventory.add_item_definition(state['sword'], 1, 2).code == unreal.HodgeInventoryResult.SUCCESS)
            state['second'] = inventory.get_items()[1].item_id
            next_phase('replicate')
        elif phase == 'replicate':
            check('independent item replicated', len(local_inventory.get_items()) == 2)
            if clients:
                remote_world = clients[0]
                other_ps = [p for p in unreal.GameplayStatics.get_all_actors_of_class(remote_world, unreal.HodgePlayerState)
                            if p.get_editor_property('PlayerId') == pid] if MODE == 'listen-host' else [
                                p for p in unreal.GameplayStatics.get_all_actors_of_class(remote_world, unreal.HodgePlayerState)
                                if p.get_editor_property('PlayerId') != pid]
                check('other PlayerState is visible for privacy assertion', bool(other_ps))
                for remote_ps in other_ps:
                    component = remote_ps.get_component_by_class(unreal.HodgeInventoryManagerComponent)
                    check('non-owner receives no private items', component is None or len(component.get_items()) == 0)
            # 由真正的拥有者客户端发起 Server RPC，不直接调用服务器选择函数代替。
            state['request'] = unreal.GuidLibrary.new_guid()
            assert unreal.HodgeInventoryValidationLibrary.dispatch_inventory_request(pc, state['second'], state['request'], False)
            next_phase('equip')
        elif phase == 'equip':
            check('owner RPC result correlated: actual=' + pc.get_last_inventory_request_id().to_string() + ', expected=' + state['request'].to_string(), same_guid(pc.get_last_inventory_request_id(), state['request']))
            result = pc.get_last_inventory_result()
            check('owner RPC succeeded: ' + str(result.code) + ' ' + result.reason, result.code == unreal.HodgeInventoryResult.SUCCESS)
            check('server applied requested identity', same_guid(selection.get_selected_item_id(), state['second']) and selection.is_selection_applied())
            check('owner selection replicated', same_guid(local_selection.get_selected_item_id(), state['second']))
            check('switch did not delete old item', len(inventory.get_items()) == 2)
            state['request'] = unreal.GuidLibrary.new_guid()
            assert unreal.HodgeInventoryValidationLibrary.dispatch_inventory_request(pc, unreal.GuidLibrary.new_guid(), state['request'], False)
            next_phase('invalid')
        elif phase == 'invalid':
            check('foreign or nonexistent identity rejected', same_guid(pc.get_last_inventory_request_id(), state['request']) and pc.get_last_inventory_result().code == unreal.HodgeInventoryResult.NOT_OWNED)
            check('rejected RPC preserves selection', same_guid(selection.get_selected_item_id(), state['second']))
            state['request'] = unreal.GuidLibrary.new_guid()
            assert unreal.HodgeInventoryValidationLibrary.dispatch_inventory_request(pc, unreal.Guid(), state['request'], True)
            next_phase('unequip')
        elif phase == 'unequip':
            check('unequip RPC acknowledged', same_guid(pc.get_last_inventory_request_id(), state['request']) and pc.get_last_inventory_result().code == unreal.HodgeInventoryResult.SUCCESS)
            check('unequip clears selected identity', same_guid(selection.get_selected_item_id(), unreal.Guid()))
            check('unequip leaves both held items', len(inventory.get_items()) == 2)
            state['request'] = unreal.GuidLibrary.new_guid()
            assert unreal.HodgeInventoryValidationLibrary.dispatch_inventory_request(pc, state['second'], state['request'], False)
            next_phase('before_respawn')
        elif phase == 'before_respawn':
            check('re-equip before respawn succeeds', selection.is_selection_applied())
            state['pawn_name'] = authority.get_path_name()
            # 使用真实服务器重启入口，保留同一个 PlayerState，不手工复制库存。
            source_pc.un_possess()
            authority.destroy_actor()
            game_mode = unreal.GameplayStatics.get_game_mode(server)
            game_mode.restart_player(source_pc)
            next_phase('respawn')
        elif phase == 'respawn':
            new_pawn = source_pc.get_controlled_pawn()
            if not new_pawn or not selection.is_selection_applied():
                return
            check('respawn creates new pawn', new_pawn.get_path_name() != state['pawn_name'])
            check('respawn retains same PlayerState inventory', new_pawn.get_editor_property('PlayerState') == ps and len(inventory.get_items()) == 2)
            check('respawn restores selected identity', same_guid(selection.get_selected_item_id(), state['second']))
            check('owning client still has exact two items', len(local_inventory.get_items()) == 2)
            finish()
    except Exception:
        finish(traceback.format_exc())


state['callback'] = unreal.register_slate_post_tick_callback(tick)
