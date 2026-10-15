"""读取真实报告生成需求追溯摘要，不复制日志、逐帧数据或本机凭据。"""
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
WORK = ROOT / 'Saved/DashAirImplementation'
automation = json.loads((WORK / 'Automation/index.json').read_text(encoding='utf-8-sig'))
assert automation['failed'] == 0 and automation['notRun'] == 0

def summarize(filename):
    """逐项验证断言，不能只相信脚本返回成功。"""
    data = json.loads((WORK / filename).read_text(encoding='utf-8'))
    assert data['passed'] and all(check['passed'] for check in data['checks']), filename
    return {'file': 'Saved/DashAirImplementation/' + filename, 'checks': len(data['checks']), 'passed': True}

modes = ['single', 'listen', 'dedicated', 'listen-lag']
files = [
    'Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.h',
    'Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_MovementAction.cpp',
    'Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Dash.h',
    'Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Dash.cpp',
    'Source/Hodgepodge/Public/Component/HodgeLocomotionPolicyComponent.h',
    'Source/Hodgepodge/Private/Component/HodgeLocomotionPolicyComponent.cpp',
    'Source/Hodgepodge/Private/Component/HodgeCharacterMovementComponent.cpp',
    'Source/Hodgepodge/Private/Tests/HodgeDashAirTests.cpp',
]
files.extend(path.relative_to(ROOT).as_posix() for path in (ROOT / 'Tools/DashAir').glob('*.py'))
result = {
    'date': '2026-10-13', 'engine': '5.5.4',
    'confirmed_behavior': '地面起手；已提交后离地续冲并保留重力；有效窗口内落地可衔接；空中或过期不补 Sprint',
    'builds': {'Editor': {'exit_code': 0, 'log': 'Saved/DashAirImplementation/editor-build.log'},
               'Game': {'exit_code': 0, 'log': 'Saved/DashAirImplementation/game-build.log'}},
    'automation': {'report': 'Saved/DashAirImplementation/Automation/index.json',
                   'succeeded': automation['succeeded'], 'succeeded_with_warnings': automation['succeededWithWarnings'],
                   'failed': automation['failed'], 'not_run': automation['notRun'],
                   'tests': [{'name': test['fullTestPath'], 'state': test['state'], 'warnings': test['warnings']} for test in automation['tests']]},
    'dash_air_pie': {mode: summarize(mode + '-dash-air-e2e.json') for mode in modes},
    'lock_regression': {mode: summarize(mode + '-e2e.json') for mode in modes},
    'restore': json.loads((WORK / 'editor-restored.json').read_text(encoding='utf-8')),
    'limitations': ['本机多世界 PIE，未测试独立 Server、跨机器、丢包、Cook 或打包', '空中实际受击／死亡联机场景未专项复跑，强约束由原生测试覆盖'],
    'files': [{'path': filename, 'sha256': hashlib.sha256((ROOT / filename).read_bytes()).hexdigest()} for filename in files],
}
output = ROOT / 'Docs/Validation/dash-air-continuation-2026-10-13-results.json'
output.write_text(json.dumps(result, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'native_tests': automation['succeeded'] + automation['succeededWithWarnings'],
                  'dash_air_checks': sum(row['checks'] for row in result['dash_air_pie'].values()),
                  'regression_checks': sum(row['checks'] for row in result['lock_regression'].values())}))
