"""隔离玩家主动恢复开关，复用真实高抗动作和空中链套件验证持续控制，不保存配置。"""
from pathlib import Path
import builtins
import time
import unreal

s = builtins.HODGE_REACTION_TEST
assert s.get('act_fixture_ready')
profile = s['profile']
previous = profile.get_editor_property('bEnableGroundRecovery')
# 本组检查 0.9 秒持续姿态；玩家 0.4 秒主动恢复由独立七项输入套件验证。
profile.set_editor_property('bEnableGroundRecovery', False)
s['output_name'] = ('act-control-extended-' if s.get('extended') else 'act-control-chain-') + str(s.get('lag_ms', 0)) + 'ms'
state = {'started': time.monotonic()}
try:
    exec(Path(unreal.Paths.project_dir(), 'Tools/HitReaction/run_suite.py').read_text(encoding='utf-8'), {})
except Exception:
    profile.set_editor_property('bEnableGroundRecovery', previous)
    raise


def tick(delta):
    """完成或退出 PIE 都恢复同一配置，不能让验证设置泄漏到正式资源。"""
    if s.get('last_report') is not None or not unreal.EditorLevelLibrary.get_pie_worlds(True) or time.monotonic() - state['started'] > 190:
        profile.set_editor_property('bEnableGroundRecovery', previous)
        unreal.unregister_slate_post_tick_callback(state['callback'])
        print('ACT_CONTROL_PROFILE_RESTORED')


state['callback'] = unreal.register_slate_post_tick_callback(tick)
