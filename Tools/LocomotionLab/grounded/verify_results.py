import json,math
from pathlib import Path
P=Path('D:/CodexUE55Lab')
load=lambda n:json.loads((P/n).read_text())
d=load('grounded-test.json');frames=d['frames'];checks={}
checks['directions_32']=len(load('grounded-directions.json'))==32 and all(r['passed'] for r in load('grounded-directions.json'))
checks['hodge_input_8']=len(load('grounded-input-results.json'))==8 and all(r['passed'] for r in load('grounded-input-results.json'))
stop_states={s for r in d['results'][:8] for s in r['states'] if s.startswith('Stop')}
checks['four_stop_actions']=stop_states=={'StopWalkLeft','StopWalkRight','StopRunLeft','StopRunRight'}
checks['stops_finish_idle']=all([r for r in frames if r['case']==i][-1]['state']=='Idle' for i in range(8))
checks['turns_90']=all(abs(d['results'][i]['final_yaw']-yaw)<1 for i,yaw in [(8,90),(9,-90)])
checks['turn_interrupts_to_move']=all(r['state']=='Move' and max(r['alpha'])==0 for r in frames if r['case']==10 and 1.3<r['t']<1.5)
checks['fall_releases_feet']=all(max(r['alpha'])==0 for r in frames if r['case']==11 and .8<r['t']<1.2)
checks['teleport_releases_feet']=any(max(r['alpha'])==0 for r in frames if r['case']==12 and .6<r['t']<.7)
windows=[]
for i in [8,9]:
 for side in range(2):
  window=[]
  for r in [r for r in frames if r['case']==i]+[None]:
   if r and r['alpha'][side]>.999 and r['state'].startswith('Turn'):window.append(r)
   elif window:
    windows.append({'case':i,'side':side,'frames':len(window),'drift_cm':math.dist(window[0]['feet'][side],window[-1]['feet'][side])});window=[]
checks['turn_locked_drift_under_1cm']=all(w['drift_cm']<1 for w in windows)
interrupt=load('grounded-interruptions.json');mid=[r for r in interrupt if .8<r['time']<1.4]
checks['montage_releases_feet']=bool(mid) and all(r['montage'] and max(r['alpha'])==0 for r in mid)
checks['montage_exit_relocks']=min(interrupt[-1]['alpha'])==1
checks['crouch_disabled']=all(not r['can_crouch'] and not r['crouched'] for r in interrupt)
reversals=load('grounded-reversals.json')
checks['reversal_holds_opposite_hip']=all(expected in [r['state'] for r in reversals if r['step']==step] for step,expected in [(1,'RightBackward'),(2,'LeftBackward'),(3,'RightBackward')])
low=load('grounded-turn30.json');low_drift=[]
for i in [0,1]:
 for side in [0,1]:
  w=[]
  for r in [r for r in low['frames'] if r['case']==i]+[None]:
   if r and r['state'].startswith('Turn') and r['alpha'][side]>.999:w.append(r)
   elif w:low_drift.append(math.dist(w[0]['feet'][side],w[-1]['feet'][side]));w=[]
checks['30fps_turns_and_lock']=max(low_drift)<1 and all(abs(r['final_yaw']-r['case'][2])<1 for r in low['results'])
stop_interrupt=load('grounded-stop-interrupt.json')
stop_move_rows=[r for r in stop_interrupt['frames'] if 1.8<r['t']<1.95]
checks['stop_interrupts_to_move']=len(stop_move_rows)>2 and all(r['state']=='Move' and max(r['alpha'])==0 for r in stop_move_rows)
result={'checks':checks,'passed':all(checks.values()),'grounded_frames':len(frames),'reversal_frames':len(reversals),'turn_lock_windows':windows,'30fps_max_locked_drift_cm':max(low_drift),'turn_end_yaw':[d['results'][i]['final_yaw'] for i in [8,9]]}
(P/'grounded-verification.json').write_text(json.dumps(result,indent=2))
print(json.dumps(result,indent=2));assert result['passed']
