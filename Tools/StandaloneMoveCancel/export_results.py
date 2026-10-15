"""导出需求断言、原生测试、源码身份和构建结果，不将过程退出码当作玩法通过。"""
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
WORK = ROOT / 'Saved/StandaloneMoveCancel'
OUTPUT = ROOT / 'Docs/Validation/definition-move-cancel-2026-10-14-results.json'


def load(path):
    """UE 报告可能有 UTF-8 BOM，统一解析后再核对失败项。"""
    return json.loads(path.read_text(encoding='utf-8-sig'))


native = []
for group in ['ActionAutomation-accepted', 'ComboAutomation-delivery']:
    report = load(WORK / group / 'index.json')
    assert report['failed'] == 0 and all(t['state'] == 'Success' for t in report['tests'])
    native.extend({'name': t['fullTestPath'], 'state': t['state'], 'warnings': t['warnings'], 'duration': t['duration']} for t in report['tests'])
assets = load(WORK / 'assets.json')
assert assets['passed'] and not assets['dirty']
blueprints = [r['data'] for r in load(WORK / 'blueprint-status.json')]
assert all(b['compileStatus'] == 'BS_UpToDate' and not b['hasCompileErrors'] for b in blueprints)
pie = []
for mode in ['single', 'listen', 'dedicated', 'listen-lag']:
    report = load(WORK / (mode + '-e2e.json'))
    assert report['passed'] and all(c['passed'] for c in report['checks'])
    moving = [s['owner'] for s in report['samples'] if s['phase'] == 'after_cancel']
    prediction = [s for s in report['samples'] if s['owner']['active'] and s['owner']['stopped'] and s['owner']['acceleration'] > 100]
    # 移动状态是原动画状态机的实际输出，额外核实恢复阶段最后的有效采样。
    final_moving = [s for s in report['samples'] if s['phase'] == 'cooldown_check']
    assert final_moving and all(s['owner']['locomotion'] == 'Cycle' for s in final_moving)
    pie.append({'mode': mode, 'passed': True, 'checks': report['checks'], 'samples_count': len(report['samples']),
                'moving_states': sorted({r['locomotion'] for r in moving}), 'predicted_moving_samples': len(prediction),
                'raw_report': (WORK / (mode + '-e2e.json')).relative_to(ROOT).as_posix()})
builds = []
for target, filename in [('HodgepodgeEditor', 'EditorBuild-delivery.log'), ('Hodgepodge', 'GameBuild-delivery.log')]:
    output = (WORK / filename).read_text(encoding='utf-8-sig', errors='replace')
    assert 'Total execution time:' in output and 'error C' not in output and 'error :' not in output
    builds.append({'target': target, 'platform': 'Win64', 'configuration': 'Development', 'exit_code': 0, 'log': 'Saved/StandaloneMoveCancel/' + filename})
sources = ['Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp', 'Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h',
           'Source/Hodgepodge/Private/Data/HodgeAbilityDefinition.cpp', 'Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h',
           'Source/Hodgepodge/Private/Tests/HodgeCombatActionRequestTests.cpp', 'Tools/StandaloneMoveCancel/run_e2e.py']
result = {'date': '2026-10-14', 'design': 'Docs/Design/definition-move-cancel.md', 'builds': builds, 'native_tests': native,
          'asset_checks': assets['checks'], 'blueprints': [{'asset': b['assetPath'], 'compileStatus': b['compileStatus']} for b in blueprints],
          'pie': pie, 'requirement_coverage': {'covered': ['MC00','MC01','MC02','MC03','MC04','MC05','MC06','MC07','MC08'],
          'note': '需求覆盖不是代码行／分支覆盖率；MC08 由原生测试覆盖，其他编号见 PIE 断言。'},
          'source_sha256': {s: hashlib.sha256((ROOT / s).read_bytes()).hexdigest() for s in sources},
          'not_verified': ['Cook／打包', '独立 Server Target', '手柄输入', '人工手感评审', '代码行／分支覆盖率采集']}
OUTPUT.write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps({'native': len(native), 'assets': len(assets['checks']), 'blueprints': len(blueprints),
                  'pie_checks': {r['mode']: len(r['checks']) for r in pie}, 'output': OUTPUT.as_posix()}, ensure_ascii=False))
