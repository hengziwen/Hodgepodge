"""复用已有PIE启动/停止链路，分别运行单人、Listen与PIE Dedicated的图标测试。"""
import json
import os
import subprocess
import sys
import time
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
WORK=Path(os.environ.get('HODGE_ABILITY_HUD_WORK',str(ROOT/'Saved/AbilityHUD')))
WORK.mkdir(parents=True,exist_ok=True)
os.environ['PYTHONIOENCODING']='utf-8'


def execute(path):
    """同时检查网关进程和脚本执行结果，认证信息只由原工具读取。"""
    result=subprocess.run([sys.executable,str(ROOT/'Tools/HitReaction/mcp_execute.py'),str(path)],cwd=ROOT,capture_output=True)
    output=result.stdout.decode('utf-8',errors='replace')
    if result.returncode or not json.loads(output)['success']:
        raise RuntimeError(output+result.stderr.decode('utf-8',errors='replace'))


for mode in sys.argv[1:] or ['single','listen','dedicated']:
    report=WORK/(mode+'-e2e.json'); failure=WORK/(mode+'-startup-error.txt')
    for path in [report,failure]:
        if path.exists():path.rename(path.with_suffix('.history-'+str(time.time_ns())))
    values={'LOCK_ON_MODE':mode,'LOCK_ON_WORK':WORK.as_posix(),'LOCK_ON_RUNNER':(ROOT/'Tools/AbilityHUD/run_e2e.py').as_posix()}
    wrapper=WORK/'batch-start.py'
    wrapper.write_text('"""仅注入测试入口，不复制PIE启动实现。"""\nfrom pathlib import Path\nexec(Path('+repr((ROOT/'Tools/LockOn/start_test.py').as_posix())+').read_text(encoding="utf-8-sig"),'+repr(values)+')\n',encoding='utf-8')
    execute(wrapper); deadline=time.monotonic()+160.
    while not report.exists():
        if failure.exists():raise RuntimeError(failure.read_text(encoding='utf-8'))
        if time.monotonic()>deadline:raise TimeoutError(mode)
        time.sleep(1)
    result=json.loads(report.read_text(encoding='utf-8'))
    execute(ROOT/'Tools/LockOn/stop_test.py'); time.sleep(2)
    print(json.dumps({'mode':mode,'passed':result['passed'],'checks':len(result['checks']),'error':result['error']},ensure_ascii=False),flush=True)
    if not result['passed']:raise AssertionError(result['error'])
