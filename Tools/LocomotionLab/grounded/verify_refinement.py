import json,math,statistics
from pathlib import Path
P=Path('D:/CodexUE55Lab');load=lambda n:json.loads((P/n).read_text());checks={};metrics={}
idle=load('idle-spawn-test.json');checks['fresh_spawn']=idle['passed'];metrics['idle_min_separation_cm']=idle['min_separation_cm']
g=load('grounded-test.json');f=g['frames'];checks['grounded_cases']=len(g['results'])==13
checks['four_stops']={s for x in g['results'][:8] for s in x['states'] if s.startswith('Stop')}=={'StopWalkLeft','StopWalkRight','StopRunLeft','StopRunRight'}
checks['stops_end_idle']=all([x for x in f if x['case']==i][-1]['state']=='Idle' for i in range(8))
checks['turns_90']=all(abs(g['results'][i]['final_yaw']-v)<1 for i,v in [(8,90),(9,-90)])
checks['move_interrupt']=all(x['state']=='Move' and max(x['alpha'])==0 for x in f if x['case']==10 and 1.3<x['t']<1.5)
checks['fall_release']=all(max(x['alpha'])==0 for x in f if x['case']==11 and .8<x['t']<1.2)
entry=[max(math.dist(a['feet'][i],b['feet'][i]) for i in (0,1)) for a,b in zip(f,f[1:]) if a['case']==b['case'] and a['state']=='Move' and b['state'].startswith('Stop')];metrics['max_stop_entry_delta_cm']=max(entry)
checks['stop_entry_no_large_pop']=max(entry)<5
s=load('stride-results.json');steady=[x for x in s if 1.2<x['t']<1.8];checks['low_speed_stride']=bool(steady) and all(.55<x['scale']<.75 and .75<x['rate']<.85 for x in steady)
checks['diagonal_compensation']=max(x['diagonal'] for x in s)>1.05;metrics['max_diagonal_scale']=max(x['diagonal'] for x in s)
t=load('terrain-results.json');metrics['terrain_toe_clearance_cm']={}
for name in ['flat','slope','step']:
 rows=[x for x in t if x['case']==name and x['enabled'] and x['t']>2.5]
 values=[x['feet'][i]['clearance'] for x in rows for i in (2,3)]
 checks['terrain_'+name]=bool(values) and all(v is not None and -1.5<v<4 for v in values)
 metrics['terrain_toe_clearance_cm'][name]=[min(values),max(values)]
c=load('combat-layer-results.json');checks['overlay_enabled']=all(x['overlay']>.99 for x in c if x['case'] in (1,2) and x['t']>2)
h=[x for x in c if x['case']==3 and x['t']>2];checks['hand_grip']=bool(h) and all(x['hand_alpha']==1 and x['grip_error']<3 for x in h);metrics['max_grip_error_cm']=max(x['grip_error'] for x in h)
m=[x for x in c if x['case']==4 and x['montage']];first=min(x['t'] for x in m);checks['montage_releases_hand']=all(x['hand_alpha']==0 for x in m if x['t']>first+.05)
low=load('grounded-turn30.json');drifts=[]
for case in (0,1):
 for side in (0,1):
  window=[]
  for x in [r for r in low['frames'] if r['case']==case]+[None]:
   if x and x['state'].startswith('Turn') and x['alpha'][side]>.999:window.append(x)
   elif window:
    drifts.append(max(math.dist(window[0]['feet'][side],r['feet'][side]) for r in window));window=[]
checks['30fps_turns']=all(abs(x['final_yaw']-x['case'][2])<1 for x in low['results']);checks['30fps_lock_drift']=bool(drifts) and max(drifts)<1;metrics['30fps_max_lock_drift_cm']=max(drifts)
d=load('grounded-directions.json');checks['directions_32']=len(d)==32 and all(x['passed'] for x in d)
result={'checks':checks,'metrics':metrics,'passed':all(checks.values())}
(P/'refinement-verification.json').write_text(json.dumps(result,indent=2));print(json.dumps(result,indent=2))
if not result['passed']:raise SystemExit(1)
