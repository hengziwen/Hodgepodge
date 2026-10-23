"""对联机断言做失败注入，不加载 Unreal，也不将离线结果当成真实网络验证。"""
import ast
import os
from pathlib import Path
from types import SimpleNamespace
import unittest
from unittest.mock import patch


def load_predicates():
    """仅执行脚本中的纯判定函数，避免导入时启动编辑器或修改正式资源。"""
    source = Path(__file__).resolve().parents[1] / 'validate_poise_runtime.py'
    tree = ast.parse(source.read_text(encoding='utf-8-sig'))
    names = {'has_client_control_sample', 'reaction_snapshot_released', 'active_montage_instances', 'require_exclusive_session'}
    functions = [node for node in tree.body if isinstance(node, ast.FunctionDef) and node.name in names]
    assert len(functions) == len(names), '缺少需要验证的实际断言入口'
    scope = {}
    exec(compile(ast.Module(body=functions, type_ignores=[]), str(source), 'exec'), scope)
    return scope


class RuntimeAssertionsTest(unittest.TestCase):
    """服务端、拥有者、观察者和错误玩家必须产生不同的证据结果。"""

    def test_authority_and_owner_cannot_impersonate_observer(self):
        """缺少真实观察者、目标身份不符或未受控时必须拒绝观察证据。"""
        predicate = load_predicates()['has_client_control_sample']
        authority = {'server': True, 'locally_controlled': False, 'player_id': 2, 'controlled': True}
        owner = {**authority, 'server': False, 'locally_controlled': True}
        observer = {**authority, 'server': False}
        self.assertFalse(predicate([authority, owner], 2, True))
        self.assertTrue(predicate([authority, owner], 2, False))
        self.assertTrue(predicate([observer], 2, True))
        self.assertFalse(predicate([observer], 3, True))
        self.assertFalse(predicate([{**observer, 'controlled': False}], 2, True))
        self.assertFalse(predicate([], 2, False))

    def test_each_motion_or_animation_residue_fails_release(self):
        """逐项保留控制或运动残留，确保每种失败都能被现有断言发现。"""
        predicate = load_predicates()['reaction_snapshot_released']
        released = {'controlled': False, 'rms': 0., 'animation_root': 0., 'playing_root': 0.,
                    'active_reaction_montages': []}
        self.assertTrue(predicate(released))
        residues = {'controlled': True, 'rms': 1., 'animation_root': 1., 'playing_root': 1.,
                    'active_reaction_montages': [{'instance': 1, 'weight': .5}]}
        for field, value in residues.items():
            with self.subTest(field=field):
                self.assertFalse(predicate({**released, field: value}))
        self.assertFalse(predicate({**released, 'rms': -1.}))

    def test_old_montage_instance_cannot_hide_behind_latest_stopped_instance(self):
        """采集列表保留旧实例残留，即使最后一个同资源实例已经停止且权重归零。"""
        predicates = load_predicates()
        older = SimpleNamespace(instance_id=10, playing=False, weight=.4)
        latest = SimpleNamespace(instance_id=11, playing=False, weight=0.)
        active = predicates['active_montage_instances']('/Game/TestMontage', [older, latest])
        self.assertEqual([item['instance'] for item in active], [10])
        snapshot = {'controlled': False, 'rms': 0., 'animation_root': 0., 'playing_root': 0.,
                    'active_reaction_montages': active}
        self.assertFalse(predicates['reaction_snapshot_released'](snapshot))
        older.weight = 0.
        self.assertEqual(predicates['active_montage_instances']('/Game/TestMontage', [older, latest]), [])
        older.playing = True
        self.assertEqual(len(predicates['active_montage_instances']('/Game/TestMontage', [older])), 1)

    def test_exclusive_session_requires_marker_identity_and_clean_packages(self):
        """启动标记、进程身份和未保存包分别校验，不把相似标记或脏包当成可安全重载。"""
        guard = load_predicates()['require_exclusive_session']
        marker = '-unattended -HodgeCombatValidationExclusive'
        guard(marker, 123, 123, [])
        for command, expected, current, dirty in [('', 123, 123, []),
                ('-HodgeCombatValidationExclusiveExtra', 123, 123, []), (marker, None, 123, []),
                (marker, True, 1, []), (marker, 123, 456, []), (marker, 123, 123, ['/Game/Unsaved'])]:
            with self.subTest(command=command, expected=expected, dirty=dirty):
                with self.assertRaises(RuntimeError):
                    guard(command, expected, current, dirty)

    def test_rejected_editor_entry_stops_before_any_mutation(self):
        """执行实际脚本入口并注入普通编辑器，保护失败必须先于任何设置或地图修改。"""
        source = Path(__file__).resolve().parents[1] / 'validate_poise_runtime.py'
        unreal_stub = SimpleNamespace(SystemLibrary=SimpleNamespace(get_command_line=lambda: ''),
            EditorLoadingAndSavingUtils=SimpleNamespace(get_dirty_content_packages=lambda: [], get_dirty_map_packages=lambda: []))
        with patch.dict('sys.modules', {'unreal': unreal_stub}):
            with self.assertRaisesRegex(RuntimeError, '独占编辑器'):
                exec(compile(source.read_text(encoding='utf-8-sig'), str(source), 'exec'), {'POISE_EXCLUSIVE_EDITOR_PID': os.getpid()})


if __name__ == '__main__':
    unittest.main()
