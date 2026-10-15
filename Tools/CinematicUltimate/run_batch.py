"""按网络模式运行大招E2E，复用既有PIE生命周期工具并保留历史失败报告。"""
import json
import os
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DEBUG_FILL='--debug' in sys.argv[1:]
WORK = ROOT / ('Saved/ResonanceDebug' if DEBUG_FILL else 'Saved/CinematicUltimate')
WORK.mkdir(parents=True, exist_ok=True)
os.environ['PYTHONIOENCODING'] = 'utf-8'


def execute(path):
    """网关执行成功和测试断言通过分别判断，不输出认证信息。"""
    completed = subprocess.run([sys.executable, str(ROOT / 'Tools/HitReaction/mcp_execute.py'), str(path)], cwd=ROOT, capture_output=True)
    output = completed.stdout.decode('utf-8', errors='replace')
    if completed.returncode or not json.loads(output)['success']:
        raise RuntimeError(output + completed.stderr.decode('utf-8', errors='replace'))


modes=[value for value in sys.argv[1:] if value!='--debug'] or (['single','listen','dedicated'] if DEBUG_FILL else ['single', 'listen', 'dedicated', 'listen-lag'])
for mode in modes:
    report, failure = WORK / (mode + '-e2e.json'), WORK / (mode + '-startup-error.txt')
    for path in [report, failure]:
        if path.exists():
            path.rename(path.with_suffix('.history-' + str(time.time_ns())))
    runner_name='run_debug_fill.py' if DEBUG_FILL else 'run_e2e.py'
    values = {'LOCK_ON_MODE': mode, 'LOCK_ON_WORK': WORK.as_posix(), 'LOCK_ON_RUNNER': (ROOT / 'Tools/CinematicUltimate' / runner_name).as_posix()}
    if mode=='capture':
        # 截图只运行单次自由镜头，不覆盖完整网络验收报告。
        runner=WORK/'capture-runner.py'
        runner.write_text('"""只运行原型截图场景，正式网络报告独立保留。"""\nfrom pathlib import Path\nexec(Path('+repr(values['LOCK_ON_RUNNER'])+').read_text(encoding="utf-8-sig"),dict(globals(),CINEMATIC_CAPTURE_ONLY=True))\n',encoding='utf-8')
        values.update(LOCK_ON_MODE='single',LOCK_ON_RUNNER=runner.as_posix())
    wrapper = WORK / 'batch-start.py'
    wrapper.write_text('"""复用正式验证地图与初始化等待，只注入本任务验证入口。"""\nfrom pathlib import Path\nexec(Path(' +
                       repr((ROOT / 'Tools/LockOn/start_test.py').as_posix()) + ').read_text(encoding="utf-8-sig"), ' + repr(values) + ')\n', encoding='utf-8')
    execute(wrapper)
    deadline = time.monotonic() + 270
    while True:
        if failure.exists():
            raise RuntimeError(failure.read_text(encoding='utf-8'))
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
