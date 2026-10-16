"""顺序启动真实 PIE 验收；停止 PIE 后读取断言报告，不覆盖历史失败证据。"""
import json
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
WORK = ROOT / 'Saved/DamageNumbers'
WORK.mkdir(parents=True, exist_ok=True)


def execute(path):
    """复用项目现有鉴权网关，先发现 schema 再执行编辑器脚本。"""
    result = subprocess.run([sys.executable, str(ROOT / 'Tools/HitReaction/mcp_execute.py'), str(path)], cwd=ROOT, capture_output=True)
    if result.returncode:
        raise RuntimeError(result.stdout.decode('utf-8', errors='replace') + result.stderr.decode('utf-8', errors='replace'))


capture = '--capture' in sys.argv
modes = [v for v in sys.argv[1:] if not v.startswith('--')]
for mode in modes or ['single', 'listen-host', 'listen', 'dedicated', 'listen-lag']:
    report = WORK / (mode + '-numbers-e2e.json')
    startup_error = WORK / (mode + '-startup-error.txt')
    for previous in [report, startup_error]:
        if previous.exists():
            previous.rename(previous.with_name(previous.stem + '-history-' + str(time.time_ns()) + previous.suffix))
    wrapper = WORK / 'start-numbers.py'
    runner = ROOT / 'Tools/DamageNumbers/run_e2e.py'
    if capture:
        assert mode == 'single', '离线录帧只用于单人视觉取证，网络验收使用真实帧时间'
        capture_runner = WORK / 'capture-runner.py'
        capture_runner.write_text('NUMBER_CAPTURE=True\n' + runner.read_text(encoding='utf-8'), encoding='utf-8')
        runner = capture_runner
        frames = WORK / 'Frames'
        if frames.exists():
            frames.rename(WORK / ('Frames-history-' + str(time.time_ns())))
    values = {'LOCK_ON_MODE': mode, 'LOCK_ON_WORK': WORK.as_posix(), 'LOCK_ON_RUNNER': runner.as_posix()}
    wrapper.write_text('from pathlib import Path\nexec(Path(' + repr((ROOT / 'Tools/LockOn/start_test.py').as_posix()) + ').read_text(encoding="utf-8-sig"), ' + repr(values) + ')\n', encoding='utf-8')
    execute(wrapper)
    try:
        deadline = time.monotonic() + 180.
        while not report.exists():
            if startup_error.exists():
                raise RuntimeError(startup_error.read_text(encoding='utf-8'))
            if time.monotonic() > deadline:
                raise TimeoutError(mode)
            time.sleep(1.)
        result = json.loads(report.read_text(encoding='utf-8'))
    finally:
        execute(ROOT / 'Tools/LockOn/stop_test.py')
    print(json.dumps({'mode': mode, 'passed': result['passed'], 'checks': len(result['checks']), 'error': result['error']}, ensure_ascii=False), flush=True)
    assert result['passed'], result['error']
