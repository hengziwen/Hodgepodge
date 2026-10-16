"""复用项目已验证的 PIE 调度，执行无视觉数据层集成测试并保留历史报告。"""
import json
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
WORK = ROOT / 'Saved/Inventory'
WORK.mkdir(parents=True, exist_ok=True)


def execute(path):
    """复用现有 search→describe→execute 网关，不绕过工具 schema。"""
    result = subprocess.run([sys.executable, str(ROOT / 'Tools/HitReaction/mcp_execute.py'), str(path)], cwd=ROOT, capture_output=True)
    if result.returncode:
        raise RuntimeError(result.stdout.decode('utf-8', errors='replace') + result.stderr.decode('utf-8', errors='replace'))


for mode in sys.argv[1:] or ['single', 'listen']:
    report = WORK / (mode + '-inventory-e2e.json')
    startup_error = WORK / (mode + '-startup-error.txt')
    for previous in [report, startup_error]:
        if previous.exists():
            previous.rename(previous.with_name(previous.stem + '-history-' + str(time.time_ns()) + previous.suffix))
    wrapper = WORK / 'start-inventory.py'
    values = {'LOCK_ON_MODE': mode, 'LOCK_ON_WORK': WORK.as_posix(), 'LOCK_ON_RUNNER': (ROOT / 'Tools/Inventory/run_e2e.py').as_posix()}
    wrapper.write_text('from pathlib import Path\nexec(Path(' + repr((ROOT / 'Tools/LockOn/start_test.py').as_posix()) + ').read_text(encoding="utf-8-sig"), ' + repr(values) + ')\n', encoding='utf-8')
    execute(wrapper)
    try:
        deadline = time.monotonic() + 190
        while not report.exists():
            if startup_error.exists():
                raise RuntimeError(startup_error.read_text(encoding='utf-8'))
            if time.monotonic() > deadline:
                raise TimeoutError(mode)
            time.sleep(1)
        result = json.loads(report.read_text(encoding='utf-8'))
    finally:
        execute(ROOT / 'Tools/LockOn/stop_test.py')
    print(json.dumps({'mode': mode, 'passed': result['passed'], 'checks': len(result['checks']), 'error': result['error']}, ensure_ascii=False), flush=True)
    assert result['passed'], result['error']
