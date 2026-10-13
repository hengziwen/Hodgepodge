"""从实际构建与测试报告导出轻量证据清单，不把未运行的检查标记为成功。"""
import hashlib
import json
from datetime import datetime
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
WORK = ROOT / 'Saved/LockOn'
OUTPUT = ROOT / 'Docs/Validation/lock-on-implementation-2026-10-12-results.json'


def load(relative):
    return json.loads((WORK / relative).read_text(encoding='utf-8-sig'))


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


automation = load('Regression/index.json')
tests = [{'name': t['fullTestPath'], 'state': t['state'], 'warnings': t.get('warnings', 0),
          'errors': t.get('errors', 0)} for t in automation['tests']]
assert len(tests) == 29 and all(t['state'] == 'Success' for t in tests)
modes = {}
for mode in ['single', 'listen', 'dedicated', 'listen-lag']:
    result = load(mode + '-e2e.json')
    assert result['passed'] and len(result['checks']) == 20 and all(c['passed'] for c in result['checks'])
    modes[mode] = {'passed': result['passed'], 'checks': result['checks'],
                   'error': result.get('error'), 'raw_sha256': digest(WORK / (mode + '-e2e.json')),
                   'raw_path': 'Saved/LockOn/' + mode + '-e2e.json',
                   'mtime_local': datetime.fromtimestamp((WORK / (mode + '-e2e.json')).stat().st_mtime).isoformat(),
                   'network': 'Net PktLag=100 per PIE driver' if mode == 'listen-lag' else mode}
assets = load('asset-validation.json')
assert assets['passed'] and len(assets['checks']) == 10
changes = load('source-changes.json')
builds = {target: {'exit_code': int((WORK / (name + '-build-exit.txt')).read_text().strip()),
                  'log': 'Saved/LockOn/' + name + '-build.log',
                  'log_sha256': digest(WORK / (name + '-build.log'))}
          for target, name in [('HodgepodgeEditor Win64 Development', 'editor'), ('Hodgepodge Win64 Development', 'game')]}
assert all(b['exit_code'] == 0 for b in builds.values())
restored = load('editor-restored.json')
assert restored['pie'] == 0 and not restored['dirty_maps'] and not restored['dirty_content']
report = {
    'date': '2026-10-12', 'engine': 'UE 5.5.4', 'design': 'Docs/Design/lock-on-system.md v0.3',
    'builds': builds, 'automation': {'count': len(tests), 'passed': 29, 'tests': tests,
                                  'raw_path': 'Saved/LockOn/Regression/index.json',
                                  'raw_sha256': digest(WORK / 'Regression/index.json')},
    'asset_validation': assets, 'pie': modes, 'editor_restored': restored,
    'source_delta': changes,
    'source_sha256': {p: digest(ROOT / p) for p in changes['changed'] + changes['added']},
    'coverage': {'line_percent': None, 'branch_percent': None,
                 'reason': '本轮未执行覆盖率插桩；只报告需求到测试的追溯和实际检查数量。'},
    'not_verified': ['物理鼠标按键、DPI、主观镜头手感与全八向动作组合', '复杂遮挡恢复与临时技能镜头端到端组合',
                     '真实攻击快照切换与锁定范围外不吸附专项场景', '真实换 Pawn、热卸载、丢包及目标引用晚到联机场景',
                     '独立进程、独立 Server Target、Cook 与打包', '规模性能与长期内存测试'],
}
OUTPUT.write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
print('已导出：29 原生测试，80 PIE 检查，10 资产检查，Editor/Game 构建，恢复结果。')
