import sys,time,json
from pathlib import Path
sys.path.insert(0,'D:/Hodgepodge/Tools/UnrealMCP')
import server
server.identity()
base=Path('D:/Hodgepodge/Tools/LocomotionLab/grounded');out=Path('D:/CodexUE55Lab');results=[]
for script,result in [('test_grounded.py','grounded-test.json'),('test_stride.py','stride-results.json'),('test_terrain.py','terrain-results.json'),('test_combat_layer.py','combat-layer-results.json'),('test_turn30.py','grounded-turn30.json'),('test_directions.py','grounded-directions.json')]:
 start=time.time();reply=server.raw('run_python_in_unreal',{'code':(base/script).read_text(encoding='utf-8-sig')})
 if reply.get('exception') or reply.get('status')=='error':raise RuntimeError(str(reply))
 print('START',script,flush=True)
 target=out/result
 while not (target.exists() and target.stat().st_mtime>=start):
  if time.time()-start>150:raise TimeoutError(script)
  time.sleep(.5)
 results.append({'script':script,'output':str(target),'completed':time.time(),'elapsed':time.time()-start})
 (out/'refined-suite-execution.json').write_text(json.dumps(results,indent=2))
 print('DONE',script,round(time.time()-start,1),flush=True)
print('SUITE_COMPLETE',flush=True)
