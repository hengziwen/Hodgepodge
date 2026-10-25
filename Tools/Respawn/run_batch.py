"""复用既有 PIE 网关调度，依次验证单人、Listen 和 PIE Dedicated 的死亡自动重生。"""
from pathlib import Path
import json
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'Saved/PlayerRespawn_20261024'


def execute(path):
    """通过现有授权网关 search→describe→execute，不打印能力令牌。"""
    result = subprocess.run([sys.executable, str(ROOT / 'Tools/HitReaction/mcp_execute.py'), str(path)], cwd=ROOT, capture_output=True)
    if result.returncode:
        raise RuntimeError(result.stdout.decode('utf-8', errors='replace') + result.stderr.decode('utf-8', errors='replace'))


for label in sys.argv[1:] or ['single', 'listen0', 'listen100', 'dedicated0', 'dedicated100']:
    mode = 'dedicated' if label.startswith('dedicated') else 'listen' if label.startswith('listen') else 'single'
    work = OUT / label
    work.mkdir(parents=True, exist_ok=True)
    report = work / (mode + '-respawn-e2e.json')
    startup = work / (mode + '-startup-error.txt')
    for previous in [report, startup]:
        if previous.exists():
            previous.rename(previous.with_name(previous.stem + '-history-' + str(time.time_ns()) + previous.suffix))
    wrapper = work / 'start.py'
    runner = 'run_failures.py' if label == 'failures' else 'run_parallel.py' if 'parallel' in label else 'run_e2e.py'
    values = {'LOCK_ON_MODE': mode, 'LOCK_ON_WORK': work.as_posix(), 'LOCK_ON_RUNNER': (ROOT / 'Tools/Respawn' / runner).as_posix()}
    wrapper.write_text('from pathlib import Path\nexec(Path(' + repr((ROOT / 'Tools/LockOn/start_test.py').as_posix()) + ').read_text(encoding="utf-8-sig"), ' + repr(values) + ')\n', encoding='utf-8')
    execute(wrapper)
    try:
        deadline = time.monotonic() + 170
        while not report.exists():
            if startup.exists():
                raise RuntimeError(startup.read_text(encoding='utf-8'))
            if time.monotonic() > deadline:
                raise TimeoutError(label)
            time.sleep(.5)
        result = json.loads(report.read_text(encoding='utf-8'))
    finally:
        execute(ROOT / 'Tools/LockOn/stop_test.py')
    print(json.dumps({'mode': label, 'passed': result['passed'], 'checks': len(result['checks']), 'error': result['error']}, ensure_ascii=False), flush=True)
    assert result['passed'], result['error']
