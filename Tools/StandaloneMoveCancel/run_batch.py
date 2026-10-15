"""按网络模式依次执行独立技能移动取消回归，保留旧报告。"""
import json
import os
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
WORK = ROOT / 'Saved/StandaloneMoveCancel'
WORK.mkdir(parents=True, exist_ok=True)
os.environ['PYTHONIOENCODING'] = 'utf-8'


def execute(path):
    """网关接入成功和游戏验收通过分别判断，不输出认证凭据。"""
    completed = subprocess.run([sys.executable, str(ROOT / 'Tools/HitReaction/mcp_execute.py'), str(path)], cwd=ROOT, capture_output=True)
    output = completed.stdout.decode('utf-8', errors='replace')
    if completed.returncode or not json.loads(output)['success']:
        raise RuntimeError(output + completed.stderr.decode('utf-8', errors='replace'))


for mode in sys.argv[1:] or ['single', 'listen', 'dedicated', 'listen-lag']:
    report = WORK / (mode + '-e2e.json')
    startup_error = WORK / (mode + '-startup-error.txt')
    for path in [report, startup_error]:
        if path.exists():
            path.rename(path.with_suffix('.history-' + str(time.time_ns())))
    values = {'LOCK_ON_MODE': mode, 'LOCK_ON_WORK': WORK.as_posix(),
              'LOCK_ON_RUNNER': (ROOT / 'Tools/StandaloneMoveCancel/run_e2e.py').as_posix()}
    wrapper = WORK / 'batch-start.py'
    wrapper.write_text('"""复用现有 PIE 生命周期工具，原地图记录独立保存。"""\nfrom pathlib import Path\nexec(Path(' +
                       repr((ROOT / 'Tools/LockOn/start_test.py').as_posix()) + ').read_text(encoding="utf-8-sig"), ' + repr(values) + ')\n', encoding='utf-8')
    execute(wrapper)
    deadline = time.monotonic() + 250
    while True:
        if startup_error.exists():
            raise RuntimeError(startup_error.read_text(encoding='utf-8'))
        if time.monotonic() > deadline:
            raise TimeoutError(mode)
        try:
            result = json.loads(report.read_text(encoding='utf-8'))
            break
        except (OSError, json.JSONDecodeError):
            time.sleep(1)
    execute(ROOT / 'Tools/LockOn/stop_test.py')
    time.sleep(2)
    print(json.dumps({'mode': mode, 'passed': result['passed'], 'checks': len(result['checks']), 'error': result['error']}, ensure_ascii=False), flush=True)
    if not result['passed']:
        raise AssertionError(result['error'])
