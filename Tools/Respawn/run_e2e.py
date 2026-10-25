"""真实死亡 GA → 自动调度 → 新 Pawn：检查身份、装备、资源与多端就绪，不保存测试修改。"""
from pathlib import Path
import gc
import json
import time
import traceback
import unreal

WORK = Path(globals()['LOCK_ON_WORK'])
MODE = globals()['LOCK_ON_MODE']
LAG = 100 if WORK.name.endswith('100') else 0
worlds = unreal.EditorLevelLibrary.get_pie_worlds(True)
server = next(w for w in worlds if unreal.SystemLibrary.is_server(w))
clients = [w for w in worlds if w != server]
owner = next(p for p in unreal.GameplayStatics.get_all_actors_of_class(clients[0] if clients else server, unreal.HodgeHeroCharacter)
             if p.get_controller() and p.get_controller().is_local_controller())
local_pc = owner.get_controller()
local_ps = owner.get_editor_property('PlayerState')
pid = local_ps.get_editor_property('PlayerId')
authority = next(p for p in unreal.GameplayStatics.get_all_actors_of_class(server, unreal.HodgeHeroCharacter)
                 if p.get_editor_property('PlayerState').get_editor_property('PlayerId') == pid)
pc = authority.get_controller()
ps = authority.get_editor_property('PlayerState')
asc = ps.get_hodge_ability_system_component()
inventory = ps.get_component_by_class(unreal.HodgeInventoryManagerComponent)
selection = ps.get_component_by_class(unreal.HodgeEquipmentSelectionComponent)
ui = unreal.HodgeUIManagerSubsystem.get_gameplay_data_for_controller(local_pc)
lib = unreal.HodgeCombatValidationLibrary
checks, samples = [], []
state = {'phase': 'warmup', 'at': 0.0, 'wall': time.monotonic(), 'last_sample': 0.0, 'round': 0}
e_tag = unreal.GameplayTag()
e_tag.import_text('(TagName="InputTag.Ability.ResonanceSkill")')
cooldown_query = unreal.GameplayEffectQuery()
cooldown_query.set_editor_property('EffectDefinition', unreal.load_class(None,
    '/Game/Main/Character/Hero/Ability/ResonanceSkill/GE_Resonance_Skill_Cooldown.GE_Resonance_Skill_Cooldown_C'))
for world in worlds:
    unreal.SystemLibrary.execute_console_command(world, 'Net PktLag=' + str(LAG))


def valid(obj):
    """死亡后的 Python 包装对象可能仍在，必须先核对原生有效性。"""
    return obj is not None and unreal.SystemLibrary.is_valid(obj)


def check(name, passed, detail=None):
    """只记录实际断言，失败不会被启动成功覆盖。"""
    checks.append({'name': name, 'passed': bool(passed), 'detail': detail})
    assert passed, (name, detail)


def stock(component):
    """公开查询身份、定义、等级、数量及定义中可初始化的全部 StatTags。"""
    return sorted([{'id': row.item_id.to_string(), 'definition': row.instance.get_item_def().get_path_name(),
                    'level': row.instance.get_item_level(), 'count': row.count,
                    'stats': {tag.export_text(): row.instance.get_stat_tag_stack_count(tag)
                              for fragment in unreal.get_default_object(row.instance.get_item_def()).get_editor_property('Fragments')
                              if fragment.get_class().get_name() == 'HodgeInventoryFragment_SetStats'
                              for tag in fragment.get_editor_property('InitialStats')}}
                   for row in component.get_items()], key=lambda row: row['id'])


def passive():
    """通过真实 Spec 查询持久能力实例与句柄。"""
    found = []
    for handle in asc.get_all_abilities():
        ability, is_instance = unreal.AbilitySystemLibrary.get_gameplay_ability_from_spec_handle(asc, handle)
        if ability and ability.get_class().get_name() == 'HodgeGameplayAbility_AutoRespawn':
            found.append((handle.export_text(), ability.get_path_name(), is_instance))
    return found


def avatar():
    """用公开能力 ActorInfo 查询服务器 ASC 的真实 Avatar，不读取受保护反射字段。"""
    for handle in asc.get_all_abilities():
        ability, is_instance = unreal.AbilitySystemLibrary.get_gameplay_ability_from_spec_handle(asc, handle)
        if ability and is_instance and ability.get_class().get_name() == 'HodgeGameplayAbility_AutoRespawn':
            return ability.get_avatar_actor_from_actor_info()
    return None


def core_sets():
    """比较核心属性集对象，公开 GAS 查询仍读取同一持久 ASC。"""
    return [asc.get_attribute_set(unreal.load_class(None, '/Script/Hodgepodge.' + name)).get_path_name()
            for name in ['HodgeHealthSet', 'HodgeCombatSet', 'HodgeResonanceSet', 'HodgeHavocSet']]


def current_peer(world):
    """只选择同玩家、同 Ready Avatar 的新身体，排除仍在销毁的旧 Pawn。"""
    for pawn in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.HodgeHeroCharacter):
        player = pawn.get_editor_property('PlayerState')
        if player and player.get_editor_property('PlayerId') == pid:
            ready = player.get_attribute_ready_state()
            if ready.ready and ready.avatar == pawn:
                return pawn
    return None


def step(name, now):
    state['phase'], state['at'] = name, now


def die(now):
    """走权威 Damage 元属性和现有死亡能力，不手动 FinishDeath 或 RestartPlayer。"""
    pawn = pc.get_controlled_pawn()
    state.update(old=pawn, old_name=pawn.get_path_name(), death_finished=False,
                 old_generation=ps.get_attribute_ready_state().life_generation)
    assert lib.queue_number_pop_probe(pawn, pawn, 1000000.0, False)
    step('dying', now)


def finish(error=None):
    """每轮释放 PIE 引用，不让脚本持有旧世界；完整样本和错误都写入报告。"""
    unreal.unregister_slate_post_tick_callback(state['callback'])
    result = {'mode': MODE, 'lag_ms': LAG, 'passed': error is None, 'checks': checks,
              'samples': samples, 'error': error, 'elapsed': time.monotonic() - state['wall'], 'visual_validation': False}
    (WORK / (MODE + '-respawn-e2e.json')).write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
    state.clear()
    for name in ['worlds', 'server', 'clients', 'owner', 'authority', 'local_pc', 'local_ps', 'pc', 'ps', 'asc', 'inventory', 'selection', 'ui']:
        globals()[name] = None
    gc.collect()


def tick(delta):
    """阶段只等待真实状态，不修改正式死亡时长或网络容差。"""
    try:
        assert time.monotonic() - state['wall'] < 90, '重生阶段超时：' + state['phase']
        now = unreal.GameplayStatics.get_time_seconds(server)
        age = now - state['at']
        phase = state['phase']
        if now - state['last_sample'] >= .03:
            pawn = pc.get_controlled_pawn()
            ready = ps.get_attribute_ready_state()
            samples.append({'time': now, 'phase': phase, 'pawn': pawn.get_path_name() if pawn else None,
                            'avatar': avatar().get_path_name() if avatar() else None,
                            'ready': ready.ready, 'life_generation': ready.life_generation,
                            'selection': selection.get_selected_item_id().to_string(), 'applied': selection.is_selection_applied()})
            state['last_sample'] = now
        if phase == 'warmup':
            if state['at'] == 0.0:
                state['at'] = now
            if age < 1.0:
                return
            check('正式库存与所选装备就绪', inventory and inventory.is_initialized() and selection.is_selection_applied())
            check('基础被动仅有一份实例', len(passive()) == 1 and passive()[0][2], passive())
            state.update(stock=stock(inventory), selected=selection.get_selected_item_id().to_string(),
                         passive=passive(), level=ps.get_character_level(), asc_name=asc.get_path_name(),
                         attribute_sets=core_sets())
            assert lib.queue_resonance_energy_delta(authority, 70.0)
            assert lib.queue_dark_flow_delta(authority, 70.0)
            step('resources', now)
        elif phase == 'resources' and age > .3:
            check('死亡前有可观测共鸣与暗流', lib.inspect_resonance_energy(authority).x > 0 and lib.inspect_dark_flow(authority).x > 0)
            assert ui.submit_input(e_tag), '正式 E 输入被拒绝'
            step('cooldown', now)
        elif phase == 'cooldown':
            remaining = ui.get_ability_display_state(e_tag).cooldown_remaining
            handles = asc.get_active_effects(cooldown_query)
            if remaining > 0 and len(handles) == 1:
                # 预测冷却可在确认前更换，死亡契约以服务器已提交的同一 GE 为准。
                state['cooldown_handle'] = handles[0].export_text()
                state['cooldown_end'] = unreal.AbilitySystemLibrary.get_active_gameplay_effect_expected_end_time(handles[0])
                die(now)
            elif age > 4:
                raise AssertionError('正式 E 未建立冷却')
        elif phase == 'dying':
            old = state['old']
            if valid(old):
                health = old.get_component_by_class(unreal.HodgeHealthComponent)
                if health.get_death_state() == unreal.HodgeDeathState.DEATH_FINISHED:
                    state['death_finished'] = True
            pawn = pc.get_controlled_pawn()
            if not pawn or pawn.get_path_name() == state['old_name']:
                return
            check('新身体生成前已观测完整死亡结束', state['death_finished'])
            step('ready', now)
        elif phase == 'ready':
            pawn = pc.get_controlled_pawn()
            peers = [current_peer(w) for w in worlds]
            if not pawn or not all(peers) or not selection.is_selection_applied() or age < .5:
                return
            health = pawn.get_component_by_class(unreal.HodgeHealthComponent)
            check('Controller 与 PlayerState 保留且 Pawn 替换', pawn.get_editor_property('PlayerState') == ps and pawn.get_path_name() != state['old_name'])
            check('ASC Owner Avatar 与核心属性集正确', pawn.get_hodge_ability_system_component() == asc and
                  asc.get_owner() == ps and avatar() == pawn and core_sets() == state['attribute_sets'])
            check('新生命代只增加一次', ps.get_attribute_ready_state().life_generation == state['old_generation'] + 1)
            check('最终满血且死亡状态解除', health.get_health() == health.get_max_health() and not health.is_dead_or_dying())
            check('库存全快照与装备身份保持', stock(inventory) == state['stock'] and selection.get_selected_item_id().to_string() == state['selected'])
            check('基础被动实例与句柄持续且无重复授予', passive() == state['passive'])
            check('角色等级不重置', ps.get_character_level() == state['level'])
            check('共鸣与暗流沿用既有死亡清零', lib.inspect_resonance_energy(pawn).x == 0 and lib.inspect_dark_flow(pawn).x == 0)
            equipment = pawn.get_component_by_class(unreal.HodgeEquipmentManagerComponent)
            instances = equipment.get_equipment_instances_of_type(unreal.HodgeEquipmentInstance)
            check('所选实际装备恰好重建一份', len(instances) == (1 if state['selected'] != unreal.Guid().to_string() else 0))
            if instances:
                check('实际装备继续指向持久库存物品', instances[0].get_equipment_id().to_string() == state['selected'] and
                      instances[0].get_instigator() == inventory.find_item_by_id(selection.get_selected_item_id()))
            for peer in peers:
                peer_ps = peer.get_editor_property('PlayerState')
                peer_asc = peer.get_hodge_ability_system_component()
                check('各端只就绪到当前新 Avatar', peer_asc and peer_asc == peer_ps.get_hodge_ability_system_component() and not peer.get_component_by_class(unreal.HodgeHealthComponent).is_dead_or_dying())
                if peer.get_controller() and peer.get_controller().is_local_controller():
                    check('拥有者库存复制完整', stock(peer_ps.get_component_by_class(unreal.HodgeInventoryManagerComponent)) == state['stock'])
            if state['round'] == 0:
                remaining = ui.get_ability_display_state(e_tag).cooldown_remaining
                handles = asc.get_active_effects(cooldown_query)
                current_end = unreal.AbilitySystemLibrary.get_active_gameplay_effect_expected_end_time(handles[0]) if handles else -1.
                check('跨死亡冷却保留同一GE句柄和服务器原截止时间', len(handles) == 1 and
                      handles[0].export_text() == state['cooldown_handle'] and abs(current_end - state['cooldown_end']) <= .001,
                      {'before_end': state['cooldown_end'], 'after_end': current_end, 'owner_ui_remaining': remaining, 'server_now': now})
                check('拥有者HUD仍显示未到期冷却', remaining > 0)
            state['round'] += 1
            if state['round'] < 2:
                die(now)
            elif state['round'] == 2:
                state['unequip_request'] = unreal.GuidLibrary.new_guid()
                assert unreal.HodgeInventoryValidationLibrary.dispatch_inventory_request(local_pc, unreal.Guid(), state['unequip_request'], True)
                step('unequip', now)
            else:
                step('observe', now)
        elif phase == 'unequip' and age > .4:
            if local_pc.get_last_inventory_request_id().to_string() != state['unequip_request'].to_string():
                return
            check('拥有者正式卸装 RPC 成功', local_pc.get_last_inventory_result().code == unreal.HodgeInventoryResult.SUCCESS)
            state['selected'] = unreal.Guid().to_string()
            die(now)
        elif phase == 'observe' and age > 1.5:
            check('完成后没有额外生成', len([p for p in unreal.GameplayStatics.get_all_actors_of_class(server, unreal.HodgeHeroCharacter)
                                       if p.get_editor_property('PlayerState') == ps]) == 1)
            check('显式卸装跨死亡仍为空手', selection.get_selected_item_id().to_string() == unreal.Guid().to_string())
            finish()
    except Exception:
        finish(traceback.format_exc())


state['callback'] = unreal.register_slate_post_tick_callback(tick)
