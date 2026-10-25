"""依次验证正式双模式示例；每个用例不论成功失败均结束 PIE。"""
from pathlib import Path
import argparse
import json
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'Saved/Encounter_20261025'
parser = argparse.ArgumentParser()
parser.add_argument('--runner', default='run_e2e.py')
parser.add_argument('modes', nargs='*', default=['single', 'listen0', 'listen100', 'dedicated0', 'dedicated100'])
arguments = parser.parse_args()
runner = ROOT / 'Tools/Encounter' / arguments.runner
assert runner.parent == ROOT / 'Tools/Encounter' and runner.is_file(), runner
suffix = {'run_e2e.py': 'encounter-e2e', 'run_lifecycle_e2e.py': 'encounter-lifecycle'}[runner.name]


def execute(path):
    """始终复用当前项目认证网关，不输出令牌或静默吞掉错误。"""
    result = subprocess.run([sys.executable, str(ROOT / 'Tools/HitReaction/mcp_execute.py'), str(path)], cwd=ROOT, capture_output=True)
    if result.returncode:
        raise RuntimeError(result.stdout.decode('utf-8', errors='replace') + result.stderr.decode('utf-8', errors='replace'))


for label in arguments.modes:
    assert label in ['single', 'listen0', 'listen100', 'dedicated0', 'dedicated100'], label
    mode = 'dedicated' if label.startswith('dedicated') else 'listen' if label.startswith('listen') else 'single'
    work = OUT / runner.stem / label
    work.mkdir(parents=True, exist_ok=True)
    report = work / (mode + '-' + suffix + '.json')
    startup = work / (mode + '-startup-error.txt')
    restored = work / 'editor-restored.json'
    for previous in [report, startup, restored]:
        if previous.exists():
            previous.rename(previous.with_name(previous.stem + '-history-' + str(time.time_ns()) + previous.suffix))
    wrapper = work / 'start.py'
    values = {'LOCK_ON_MODE': mode, 'LOCK_ON_WORK': work.as_posix(), 'LOCK_ON_RUNNER': runner.as_posix()}
    wrapper.write_text('"""启动独立刷怪示例验证，不保存 PIE 对象。"""\nfrom pathlib import Path\nexec(Path(' +
                       repr((ROOT / 'Tools/Encounter/start_test.py').as_posix()) + ').read_text(encoding="utf-8-sig"), ' + repr(values) + ')\n', encoding='utf-8')
    execute(wrapper)
    try:
        deadline = time.monotonic() + 240
        while not report.exists():
            if startup.exists():
                raise RuntimeError(startup.read_text(encoding='utf-8'))
            if time.monotonic() > deadline:
                raise TimeoutError(label)
            time.sleep(.5)
        result = json.loads(report.read_text(encoding='utf-8'))
    finally:
        execute(ROOT / 'Tools/LockOn/stop_test.py')
        restore_wrapper = work / 'restore.py'
        restore_wrapper.write_text('"""恢复本次测试设置。"""\nfrom pathlib import Path\nexec(Path(' +
                                   repr((ROOT / 'Tools/Encounter/restore_editor.py').as_posix()) +
                                   ').read_text(encoding="utf-8-sig"), ' + repr({'LOCK_ON_WORK': work.as_posix()}) + ')\n', encoding='utf-8')
        execute(restore_wrapper)
        restore_deadline = time.monotonic() + 30
        while not restored.exists():
            if time.monotonic() > restore_deadline:
                raise TimeoutError('恢复编辑器设置：' + label)
            time.sleep(.2)
        restoration = json.loads(restored.read_text(encoding='utf-8'))
        assert restoration['passed'], restoration.get('error')
    print(json.dumps({'runner': runner.name, 'mode': label, 'passed': result['passed'],
                      'checks': len(result['checks']), 'error': result['error']}, ensure_ascii=False), flush=True)
    assert result['passed'], result['error']
