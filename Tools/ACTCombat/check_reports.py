"""聚合本轮真实报告并检查关键证据；不把日志存在或进程启动当成通过。"""
from pathlib import Path
import json

root = Path(__file__).resolve().parents[2] / 'Saved' / 'ACTCombat'
summary = {'checks': [], 'error': None}


def read(name):
    """自动化报告可能带 BOM，统一读取但不修改原始证据。"""
    return json.loads((root / name).read_text(encoding='utf-8-sig'))


try:
    automation = read('Automation-Final/index.json')
    assert automation['failed'] == automation['notRun'] == 0
    summary['automation'] = {k: automation[k] for k in ['succeeded', 'succeededWithWarnings', 'failed', 'notRun']}
    for name, count in [('player-recovery-0ms.json', 7), ('player-recovery-100ms.json', 7),
                        ('dense-100ms.json', 3), ('control-chain-100ms.json', 7), ('control-extended-100ms.json', 4)]:
        data = read(name)
        assert data['error'] is None and len(data['cases']) == count, (name, data.get('error'))
        if name.startswith('player-recovery') or name.startswith('dense'):
            assert all(case['passed'] for case in data['cases'])
        if name.startswith('player-recovery'):
            attacks = [row for row in data['samples'] if row['case'] == 'AllowedAttack']
            assert any('BasicAttack' in row['owner_actions'] for row in attacks)
            assert all('Combo.Light.' in row.get('owner_combo', '') for row in attacks if 'BasicAttack' in row['owner_actions'])
            assert all('Combo.Light.' in row.get('server_combo', '') for row in attacks if 'BasicAttack' in row['server_actions'])
        summary['checks'].append({'report': name, 'passed_cases': count})
    data = read('control-chain-100ms.json')
    knockback = next(case for case in data['cases'] if case['name'] == 'Knockback')
    assert abs(knockback['max_x_delta'] - 150) < 2
    assert all(case['contact_events'] == [1, 1, 1] for case in data['cases'])
    sustained = [row for row in data['samples'] if row['case'] == 'EqualSkill' and row['t'] > 1.1 and row['authority']['controlled']]
    assert sustained and all(row[peer]['montage_playing'] for row in sustained for peer in ['authority', 'owner', 'observer'])
    summary['knockback_cm'] = knockback['max_x_delta']
    summary['sustained_pose_samples'] = len(sustained)
    assets = read('asset-validation.json')
    assert assets['error'] is None and len(assets['attacks']) == 5 and len(assets['animations']) == 8
    summary['asset_checks'] = {'attacks': 5, 'profiles': len(assets['profiles']), 'strong_montages': 8}
    loop = read('action-loop.json')
    assert loop['error'] is None and len(loop['checks']) == 4
    assert loop['sequence'] == ['DA_Attack_1', 'DA_Attack_2', 'DA_Attack_3', 'DA_Attack_5', 'DA_Attack_4']
    summary['action_loop'] = loop['checks']
    summary['known_unresolved'] = ['Plain client combo: owner advances beyond 01 while authority does not; user-deferred window issue; see plain-client-combo.json']
except Exception as error:
    summary['error'] = str(error)
    raise
finally:
    (root / 'final-checks.json').write_text(json.dumps(summary, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps(summary, ensure_ascii=False))
