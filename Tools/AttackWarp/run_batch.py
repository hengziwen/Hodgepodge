"""依次启动独立 PIE 模式，读取真实断言并在失败时停止。"""
import json,subprocess,sys,time
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];WORK=ROOT/'Saved/AttackWarp';EXEC=ROOT/'Tools/HitReaction/mcp_execute.py'
import os
KIND=os.environ.get('HODGE_WARP_RUNNER','attack')
SUFFIX='-combo-regression.json' if KIND=='combo' else '-attack-warp-e2e.json'
RUNNER='run_combo_regression.py' if KIND=='combo' else 'run_e2e.py'
def execute(path):
 p=subprocess.run([sys.executable,str(EXEC),str(path)],cwd=ROOT,capture_output=True)
 out=p.stdout.decode('utf-8',errors='replace')
 if p.returncode or not json.loads(out.strip())['success']:raise RuntimeError(out+p.stderr.decode('utf-8',errors='replace'))
for mode in (sys.argv[1:] or ['single','listen','dedicated','listen-lag']):
 report=WORK/(mode+SUFFIX)
 if report.exists():
  history=WORK/'History';history.mkdir(exist_ok=True);report.rename(history/(report.stem+'-'+str(time.time_ns())+'.json'))
 error=WORK/(mode+'-startup-error.txt')
 if error.exists():error.rename(error.with_suffix('.old'))
 wrapper=WORK/'batch-start.py'
 values={'LOCK_ON_MODE':mode,'LOCK_ON_WORK':WORK.as_posix(),'LOCK_ON_RUNNER':(ROOT/('Tools/AttackWarp/'+RUNNER)).as_posix()}
 wrapper.write_text('from pathlib import Path\nexec(Path('+repr((ROOT/'Tools/LockOn/start_test.py').as_posix())+').read_text(encoding="utf-8-sig"),'+repr(values)+')\n',encoding='utf-8')
 execute(wrapper);deadline=time.monotonic()+180
 # 写入报告可能跨帧完成，存在文件不等于 JSON 已写完。
 while True:
  if error.exists():raise RuntimeError(error.read_text(encoding='utf-8'))
  if time.monotonic()>deadline:raise TimeoutError(mode)
  if report.exists():
   try:
    r=json.loads(report.read_text(encoding='utf-8'));break
   except (json.JSONDecodeError,OSError):pass
  time.sleep(1)
 execute(ROOT/'Tools/LockOn/stop_test.py');time.sleep(1)
 print(json.dumps({'mode':mode,'passed':r['passed'],'checks':len(r['checks']),'error':r['error']},ensure_ascii=False),flush=True)
 if not r['passed'] or not all(c['passed'] for c in r['checks']):raise AssertionError(r['error'])
