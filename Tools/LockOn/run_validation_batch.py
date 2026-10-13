"""本机 CLI 依次运行镜头专项与锁定回归；每项读取实际断言结果，失败立即停止。"""
import json,subprocess,sys,time,argparse
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
WORK=ROOT/'Saved/LockOnCamera'
EXECUTOR=ROOT/'Tools/HitReaction/mcp_execute.py'


def execute(path):
    result=subprocess.run([sys.executable,str(EXECUTOR),str(path)],cwd=ROOT,capture_output=True)
    output=result.stdout.decode('utf-8',errors='replace')
    # gateway 成功只表示脚本进入编辑器，玩法结果另读 JSON。
    if result.returncode or not json.loads(output.strip())['success']:
        raise RuntimeError(output+result.stderr.decode('utf-8',errors='replace'))


def run(kind,mode):
    filename=mode+({'camera':'-camera-e2e.json','tracking':'-tracking-e2e.json','lock':'-e2e.json'}[kind])
    report=WORK/filename
    if report.exists():
        history=WORK/'History';history.mkdir(exist_ok=True)
        report.rename(history/(report.stem+'-'+str(time.time_ns())+'.json'))
    start=WORK/'batch-start.py'
    runner=ROOT/({'camera':'Tools/LockOn/run_camera_e2e.py','tracking':'Tools/LockOn/run_tracking_e2e.py','lock':'Tools/LockOn/run_e2e.py'}[kind])
    variables={'LOCK_ON_MODE':mode,'LOCK_ON_WORK':WORK.as_posix(),'LOCK_ON_RUNNER':runner.as_posix()}
    start.write_text('from pathlib import Path\nexec(Path('+repr((ROOT/'Tools/LockOn/start_test.py').as_posix())+').read_text(encoding="utf-8-sig"),'+repr(variables)+')\n',encoding='utf-8')
    execute(start)
    deadline=time.monotonic()+150
    while not report.exists():
        error=WORK/(mode+'-startup-error.txt')
        if error.exists():raise RuntimeError(error.read_text(encoding='utf-8'))
        if time.monotonic()>deadline:raise TimeoutError(filename)
        time.sleep(1)
    result=json.loads(report.read_text(encoding='utf-8'))
    execute(ROOT/'Tools/LockOn/stop_test.py')
    time.sleep(1)
    passed=result['passed'] and all(c['passed'] for c in result['checks'])
    print(json.dumps({'kind':kind,'mode':mode,'passed':passed,'checks':len(result['checks']),'error':result.get('error')},ensure_ascii=False),flush=True)
    if not passed:raise AssertionError(result.get('error'))


if __name__=='__main__':
    WORK.mkdir(parents=True,exist_ok=True)
    parser=argparse.ArgumentParser()
    parser.add_argument('--kind',choices=['camera','tracking','lock'])
    parser.add_argument('--work',default=str(WORK))
    parser.add_argument('modes',nargs='*')
    args=parser.parse_args()
    WORK=Path(args.work);WORK.mkdir(parents=True,exist_ok=True)
    for kind in ([args.kind] if args.kind else ['camera','lock']):
        for mode in (args.modes or ['single','listen','dedicated','listen-lag']):
            if mode not in ['single','listen','dedicated','listen-lag']:raise ValueError(mode)
            run(kind,mode)
