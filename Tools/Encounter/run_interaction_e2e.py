"""用隔离目标验证真实 F 键映射、GAS、拥有者 RPC 与复制；不冒充波次验收。"""
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
pid = owner.get_editor_property('PlayerState').get_editor_property('PlayerId')
authority = next(p for p in unreal.GameplayStatics.get_all_actors_of_class(server, unreal.HodgeHeroCharacter)
                 if p.get_editor_property('PlayerState').get_editor_property('PlayerId') == pid)
component = pc.get_component_by_class(unreal.HodgeInteractionComponentBase)
action = unreal.load_asset('/Game/Main/Input/InputAction/IA_Interact')
target_class = unreal.load_class(None, '/Script/Hodgepodge.HodgeInteractionTestTarget')
target = unreal.HodgeCombatValidationLibrary.spawn_hit_reaction_validation_actor(
    server, target_class, authority.get_actor_location() + unreal.Vector(150, 0, 0))
assert component, 'PlayerController 缺少默认交互组件'
assert action, '交互输入动作未配置'
assert target, '验证桥未生成授权的隔离目标'
lag = 100 if '100' in WORK.name else 0
for world in worlds:
    unreal.SystemLibrary.execute_console_command(world, 'Net PktLag=' + str(lag))
checks = []
state = {'phase': 'replicate', 'at': time.monotonic(), 'start': time.monotonic(), 'held': False}


def check(name, value):
    """检查实际服务器计数及拥有者结果，不能将发出请求等同执行成功。"""
    checks.append({'name': name, 'passed': bool(value)})
    assert value, name


def phase(name):
    state['phase'] = name
    state['at'] = time.monotonic()


def held(value):
    """只在边沿提交真实按键，按住期间由 PlayerInput 自身维护状态。"""
    assert unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc, 'InteractionKey', 1 if value else 0)
    state['held'] = value


def finish(error=None):
    """异常同样释放注入输入并保留报告，不保存运行中的资产。"""
    unreal.unregister_slate_post_tick_callback(state['callback'])
    held(False)
    result = {'mode': WORK.name, 'pie_mode': MODE, 'lag_ms': lag, 'passed': error is None, 'checks': checks, 'error': error,
              'scope': 'F-key/IMC/input-action/GAS/interaction-RPC only', 'physical_keyboard_test': False,
              'encounter_e2e': False, 'elapsed': time.monotonic() - state['start']}
    (WORK / (MODE + '-interaction-e2e.json')).write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')


def tick(delta):
    try:
        if time.monotonic() - state['start'] > 55:
            raise TimeoutError(state['phase'])
        current = state['phase']
        elapsed = time.monotonic() - state['at']
        replicas = unreal.GameplayStatics.get_all_actors_of_class(owner_world, target_class)
        if current == 'replicate' and elapsed > 2 and replicas:
            local = replicas[0]
            check('服务器目标轮次复制给拥有者', local.get_option_snapshot().run_id.to_string() == target.get_option_snapshot().run_id.to_string())
            check('默认交互距离250cm内', owner.get_distance_to(local) < 250)
            held(True)
            phase('first')
        elif current == 'first' and elapsed > 2:
            check('输入动作经GAS及真实RPC执行一次', target.get_successful_executions() == 1)
            check('拥有者收到成功结果', component.get_last_result().get_editor_property('bSucceeded'))
            check('长按不会重复执行', target.get_execute_attempts() == 1)
            check('提交后的修订复制收敛', replicas[0].get_option_snapshot().revision == target.get_option_snapshot().revision)
            held(False)
            phase('release')
        elif current == 'release' and elapsed > .5:
            held(True)
            phase('second')
        elif current == 'second' and elapsed > 2:
            check('松开再按使用新修订执行第二次', target.get_successful_executions() == 2)
            check('新的长按仍只执行一次', target.get_execute_attempts() == 2)
            check('实际执行计数复制给拥有者', replicas[0].get_successful_executions() == 2)
            held(False)
            # 通过 Actor 正常生命周期移除测试目标，不越过只读属性写入边界。
            target.destroy_actor()
            phase('unavailable')
        elif current == 'unavailable' and elapsed > 2:
            held(True)
            phase('reject')
        elif current == 'reject' and elapsed > 2:
            check('已销毁目标从拥有者候选集合移除', not unreal.SystemLibrary.is_valid(target) and not replicas)
            result = component.get_last_result()
            check('拥有者收到不可用诊断', not result.get_editor_property('bSucceeded') and bool(result.reason))
            finish()
    except Exception:
        finish(traceback.format_exc())


state['callback'] = unreal.register_slate_post_tick_callback(tick)
