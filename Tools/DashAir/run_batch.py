"""依次启动各 PIE 模式，按实际断言检查离地 Dash，不保存正式资产。"""
import json
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
WORK = ROOT / 'Saved/DashAirImplementation'

def execute(path):
    """网关只确认执行成功，玩法结果由报告逐项确认。"""
    result = subprocess.run([sys.executable, str(ROOT / 'Tools/HitReaction/mcp_execute.py'), str(path)], cwd=ROOT, capture_output=True)
    output = result.stdout.decode('utf-8', errors='replace')
    if result.returncode or not json.loads(output)['success']:
        raise RuntimeError(output + result.stderr.decode('utf-8', errors='replace'))

for mode in sys.argv[1:] or ['single', 'listen', 'dedicated', 'listen-lag']:
    report = WORK / (mode + '-dash-air-e2e.json')
    if report.exists():
        history = WORK / 'History'
        history.mkdir(exist_ok=True)
        report.rename(history / (report.stem + '-' + str(time.time_ns()) + '.json'))
    error = WORK / (mode + '-startup-error.txt')
    if error.exists():
        error.rename(error.with_suffix('.old-' + str(time.time_ns())))
    wrapper = WORK / 'batch-start.py'
    values = {'LOCK_ON_MODE': mode, 'LOCK_ON_WORK': WORK.as_posix(), 'LOCK_ON_RUNNER': (ROOT / 'Tools/DashAir/run_e2e.py').as_posix()}
    wrapper.write_text('from pathlib import Path\nexec(Path(' + repr((ROOT / 'Tools/LockOn/start_test.py').as_posix()) + ').read_text(encoding="utf-8-sig"),' + repr(values) + ')\n', encoding='utf-8')
    execute(wrapper)
    deadline = time.monotonic() + 180
    while True:
        if error.exists():
            raise RuntimeError(error.read_text(encoding='utf-8'))
        if time.monotonic() > deadline:
            raise TimeoutError(mode)
        try:
            result = json.loads(report.read_text(encoding='utf-8'))
            break
        except (OSError, json.JSONDecodeError):
            time.sleep(1)
    execute(ROOT / 'Tools/LockOn/stop_test.py')
    time.sleep(1)
    passed = result['passed'] and all(check['passed'] for check in result['checks'])
    print(json.dumps({'mode': mode, 'passed': passed, 'checks': len(result['checks']), 'error': result['error']}, ensure_ascii=False), flush=True)
    if not passed:
        raise AssertionError(result['error'])
