"""只读采样普通客户端连段基线，用于区分既有窗口问题与受击恢复接管，不保存资产。"""
from pathlib import Path
import builtins
import json
import unreal

s = builtins.HODGE_REACTION_TEST
scope = builtins.HODGE_ACT_RECOVERY_SCOPE
scope['cleanup']()
owner, server = s['owned_target'], s['target']
for pawn in [owner, server]:
    pawn.character_movement.stop_movement_immediately()
    pawn.get_controller().set_control_rotation(unreal.Rotator(yaw=0))
    pawn.set_actor_rotation(unreal.Rotator(yaw=0), False)
world = s['world']
state = {'began': unreal.GameplayStatics.get_time_seconds(world), 'pressed': -1., 'rows': []}


def tick(delta):
    """重复正式攻击输入，不施加受击，不把诊断节点差异静默当成通过。"""
    now = unreal.GameplayStatics.get_time_seconds(world)
    age = now - state['began']
    if age > 4:
        unreal.unregister_slate_post_tick_callback(state['callback'])
        report = {'rows': state['rows'], 'with_hit_recovery': False, 'lag_ms': s.get('lag_ms', 0)}
        Path(unreal.Paths.project_saved_dir(), 'ACTCombat/plain-client-combo.json').write_text(json.dumps(report, indent=2), encoding='utf-8')
        scope['cleanup']()
        return
    if age > .6 and age < 3.3 and now - state['pressed'] > .16:
        scope['sub'].inject_input_vector_for_action(scope['attack'], unreal.Vector(1, 0, 0), [], [])
        state['pressed'] = now
    state['rows'].append({'age': age,
                          'owner': owner.get_component_by_class(unreal.HodgeCombatComponentBase).get_current_combo_tag().export_text(),
                          'server': server.get_component_by_class(unreal.HodgeCombatComponentBase).get_current_combo_tag().export_text()})


state['callback'] = unreal.register_slate_post_tick_callback(tick)
