"""依次验证通知蒙太奇表现与翼 Box 伤害，报告写入独立本轮目录。"""
import json
import os
import shutil
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
WORK = ROOT / 'Saved/WingMontage'

def execute(path):
    """网关执行结果与玩法报告分别校验，不把脚本接入当成通过。"""
    result = subprocess.run([sys.executable, str(ROOT / 'Tools/HitReaction/mcp_execute.py'), str(path)], cwd=ROOT, capture_output=True)
    output = result.stdout.decode('utf-8', errors='replace')
    if result.returncode or not json.loads(output)['success']:
        raise RuntimeError(output + result.stderr.decode('utf-8', errors='replace'))

for scenario in os.environ.get('WING_SCENARIOS','presentation,damage').split(','):
    # Box 测试复用之前已校准的尺寸，不改正式攻击通知。
    if scenario == 'damage':
        shutil.copy2(ROOT / 'Saved/WingBox/configuration.json', WORK / 'configuration.json.box')
    for mode in sys.argv[1:] or ['single', 'listen', 'dedicated']:
        folder = WORK / scenario
        folder.mkdir(exist_ok=True)
        if scenario == 'damage': shutil.copy2(WORK / 'configuration.json.box', folder / 'configuration.json')
        report = folder / (mode + ('-e2e.json' if scenario == 'presentation' else '-damage.json'))
        if report.exists(): report.rename(report.with_suffix('.history-' + str(time.time_ns())))
        startup_error = folder / (mode + '-startup-error.txt')
        if startup_error.exists(): startup_error.rename(startup_error.with_suffix('.history-' + str(time.time_ns())))
        runner = ROOT / 'Tools/WingAttack' / ('run_e2e.py' if scenario == 'presentation' else 'run_box_damage.py')
        wrapper = WORK / 'batch-start.py'
        values = {'WING_MODE': mode, 'WING_WORK': folder.as_posix(), 'WING_TEST_SCRIPT': runner.as_posix()}
        wrapper.write_text('from pathlib import Path\nexec(Path(' + repr((ROOT / 'Tools/WingAttack/start_test.py').as_posix()) + ').read_text(encoding="utf-8-sig"),' + repr(values) + ')\n', encoding='utf-8')
        execute(wrapper)
        deadline = time.monotonic() + 200
        while True:
            if startup_error.exists(): raise RuntimeError(startup_error.read_text(encoding='utf-8'))
            if time.monotonic() > deadline: raise TimeoutError((scenario, mode))
            try:
                result = json.loads(report.read_text(encoding='utf-8'))
                break
            except (OSError, json.JSONDecodeError): time.sleep(1)
        execute(ROOT / 'Tools/WingAttack/stop_test.py')
        time.sleep(1)
        passed = result['passed'] and all(row['passed'] for row in result['checks'])
        print(json.dumps({'scenario': scenario, 'mode': mode, 'passed': passed, 'checks': len(result['checks']), 'error': result['error']}, ensure_ascii=False), flush=True)
        if not passed: raise AssertionError(result['error'])
