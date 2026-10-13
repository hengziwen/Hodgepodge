"""从实际报告生成仓库摘要和代码指纹，不复制逐帧采样、日志或本机凭据。"""
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
WORK = ROOT / 'Saved/AttackWarp'
automation = json.loads((WORK / 'Automation/index.json').read_text(encoding='utf-8-sig'))
assert automation['failed'] == 0 and automation['notRun'] == 0

def summarize(filename):
    """报告存在不等于通过，必须验证全部逐项断言。"""
    data = json.loads((WORK / filename).read_text(encoding='utf-8'))
    assert data['passed'] and all(item['passed'] for item in data['checks']), filename
    return {'report': 'Saved/AttackWarp/' + filename, 'checks': len(data['checks']), 'passed': True}

modes = ['single', 'listen', 'dedicated', 'listen-lag']
result = {
    'date': '2026-10-13',
    'engine': '5.5.4',
    'builds': {
        'Editor': {'exit_code': 0, 'log': 'Saved/AttackWarp/editor-build.log'},
        'Game': {'exit_code': 0, 'log': 'Saved/AttackWarp/game-build.log'},
    },
    'automation': {
        'report': 'Saved/AttackWarp/Automation/index.json',
        'succeeded': automation['succeeded'],
        'succeeded_with_warnings': automation['succeededWithWarnings'],
        'failed': automation['failed'],
        'tests': [{'name': test['fullTestPath'], 'state': test['state'], 'warnings': test['warnings']} for test in automation['tests']],
    },
    'attack_pie': {mode: summarize(mode + '-attack-warp-e2e.json') for mode in modes},
    'combo_pie': {mode: summarize(mode + '-combo-regression.json') for mode in modes},
    'lock_pie': {mode: summarize(mode + '-e2e.json') for mode in modes},
    'configuration': json.loads((WORK / 'configuration-final.json').read_text(encoding='utf-8')),
    'editor_restore': json.loads((WORK / 'editor-restored.json').read_text(encoding='utf-8')),
    'coverage': '需求 AW-01～07 与实际用例对应；未运行编译器行覆盖率工具。',
    'limitations': ['PIE Dedicated，不是独立 Server 构建', '拒绝恢复结果由 Editor 探针注入', '未做跨机器、Cook、打包或严重丢包测试'],
}
# 仅收录本轮备份后确实变更的目标，以及新建攻击 Warp 文件。
files = []
before = WORK / 'Before'
for original in before.rglob('*'):
    if original.is_file():
        current = ROOT / original.relative_to(before)
        if current.is_file() and current.read_bytes() != original.read_bytes():
            files.append(current)
for folder in ['Source/Hodgepodge', 'Tools/AttackWarp']:
    for current in (ROOT / folder).rglob('*'):
        if current.is_file() and (current.suffix in ['.h', '.cpp'] and ('AttackFacing' in current.name or 'AttackMotionWarping' in current.name or 'AttackWarp' in current.name) or folder == 'Tools/AttackWarp' and current.suffix in ['.py', '.md']):
            files.append(current)
result['files'] = [{'path': current.relative_to(ROOT).as_posix(), 'sha256': hashlib.sha256(current.read_bytes()).hexdigest()} for current in sorted(set(files))]
output = ROOT / 'Docs/Validation/attack-facing-motion-warp-2026-10-13-results.json'
output.write_text(json.dumps(result, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'automation': result['automation']['succeeded'] + result['automation']['succeeded_with_warnings'], 'attack_assertions': sum(item['checks'] for item in result['attack_pie'].values()), 'combo_assertions': sum(item['checks'] for item in result['combo_pie'].values()), 'lock_assertions': sum(item['checks'] for item in result['lock_pie'].values()), 'fingerprints': len(result['files'])}))
