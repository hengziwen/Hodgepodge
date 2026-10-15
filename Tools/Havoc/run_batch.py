"""顺序执行暗涌各PIE模式并检查真实报告，保留历史失败证据。"""
import json
import os
import subprocess
import sys
import time
from pathlib import Path

root = Path(__file__).resolve().parents[2]
suite = os.environ.get('HODGE_HAVOC_SUITE', 'core')
assert suite in ['core', 'hits', 'debug']
work = root / ('Saved/HavocDebug' if suite == 'debug' else 'Saved/Havoc')
work.mkdir(parents=True, exist_ok=True)


def execute(script):
    """网关确认执行，玩法是否通过另读断言报告。"""
    process = subprocess.run([sys.executable, str(root / 'Tools/HitReaction/mcp_execute.py'), str(script)], cwd=root, capture_output=True)
    if process.returncode:
        raise RuntimeError(process.stdout.decode('utf-8', errors='replace') + process.stderr.decode('utf-8', errors='replace'))


for mode in sys.argv[1:] or (['single', 'listen', 'dedicated'] if suite == 'debug' else ['single', 'listen', 'dedicated', 'listen-lag']):
    suffix = {'core': '-havoc-e2e.json', 'hits': '-havoc-hits-e2e.json', 'debug': '-havoc-debug-e2e.json'}[suite]
    report = work / (mode + suffix)
    if report.exists():
        report.rename(work / (report.stem + '-history-' + str(time.time_ns()) + '.json'))
    error = work / (mode + '-startup-error.txt')
    if error.exists():
        error.rename(work / (error.stem + '-history-' + str(time.time_ns()) + '.txt'))
    runner = {'core': 'run_e2e.py', 'hits': 'run_hit_e2e.py', 'debug': 'run_debug_fill.py'}[suite]
    values = {'LOCK_ON_MODE': mode, 'LOCK_ON_WORK': work.as_posix(), 'LOCK_ON_RUNNER': (root / 'Tools/Havoc' / runner).as_posix()}
    wrapper = work / 'start-havoc.py'
    wrapper.write_text('from pathlib import Path\nexec(Path(' + repr((root / 'Tools/LockOn/start_test.py').as_posix()) + ').read_text(encoding="utf-8-sig"),' + repr(values) + ')\n', encoding='utf-8')
    execute(wrapper)
    deadline = time.monotonic() + 180.
    try:
        while not report.exists():
            if error.exists():
                raise RuntimeError(error.read_text(encoding='utf-8'))
            if time.monotonic() > deadline:
                raise TimeoutError(mode)
            time.sleep(1.)
        result = json.loads(report.read_text(encoding='utf-8'))
    finally:
        execute(root / 'Tools/LockOn/stop_test.py')
    passed = result['passed'] and all(c['passed'] for c in result['checks'])
    print(json.dumps({'mode': mode, 'passed': passed, 'checks': len(result['checks']), 'error': result['error']}, ensure_ascii=False), flush=True)
    if not passed:
        raise AssertionError(result['error'])
