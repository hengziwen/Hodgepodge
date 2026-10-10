"""Validate final sync/recovery reports and preservation of unrelated Main assets."""
import json,hashlib,math
from pathlib import Path

root=Path(__file__).resolve().parents[2];saved=root/'Saved/AnimationSync'
native=json.loads((saved/'Native3/index.json').read_text(encoding='utf-8-sig'))
assert native['failed']==0 and native['notRun']==0
assert native['succeeded']+native['succeededWithWarnings']==36
reports={};metrics={}
for prefix,lag in [('single',0),('network',0),('network',100)]:
    names=[(f'sync-{prefix}-{lag}ms.json',6),(f'{prefix}-{lag}ms.json',10),
           (f'fixed-{prefix}-{lag}ms.json',7),(f'{"single-" if prefix=="single" else ""}combat-{lag}ms.json',3)]
    for filename,count in names:
        r=json.loads((saved/filename).read_text(encoding='utf-8'))
        assert r['error'] is None and len(r['cases'])==count and all(c['passed'] for c in r['cases']),(filename,r['error'])
        reports[filename]={'passed':count,'cases':[c['case'] for c in r['cases']]}
        if not filename.startswith('sync-'):continue
        metrics[filename]={'maximum_recovery_foot_speed_cm_s':next(c['maximum_recovery_foot_speed'] for c in r['cases'] if c['case']=='pivot_exit'),'second_pivot':{}}
        for role in ['owner','server']:
            active=[x for x in r['samples'] if x['case']=='pivot_tail_reversal' and x[role]['pivot']]
            first_id=active[0][role]['pivot_instance']
            first_start=active[0]['t']-active[0][role]['pivot'][0]['time']
            second=next(x for x in active if x[role]['pivot_instance']!=first_id)
            delay=second['t']-first_start
            assert .8<delay<1.55
            metrics[filename]['second_pivot'][role]={'delay_from_first_start_s':delay,'instance_count':len(set(x[role]['pivot_instance'] for x in active))}
movement='Content/Main/Character/Hero/Anim/Movement/'
expected={movement+n+'.uasset' for n in ['A_Hero_Sprint_F','A_Hero_Dash_F','A_Hero_Dash_B','A_Hero_Sprint_Pivot_RootMotion',
                                       'AM_Hero_Dash_F','AM_Hero_Dash_B','AM_Hero_Sprint_Pivot']}
expected|={'Content/Main/Character/Hero/Ability/Sprint/DA_Hero_SprintAbility.uasset','Content/Main/Character/Hero/Anim/Layer/ABP_Pover_LocomotionBase.uasset'}
baseline=json.loads((saved/'baseline.json').read_text(encoding='utf-8'));preserved=[];changed=[]
for e in baseline:
    if not e['path'].startswith('Content/Main/'):continue
    same=hashlib.sha256((root/e['path']).read_bytes()).hexdigest()==e['sha256']
    if e['path'] not in expected:assert same,e['path'];preserved.append(e['path'])
    elif not same:changed.append(e['path'])
assert set(changed)==expected
config=json.loads((saved/'configuration.json').read_text(encoding='utf-8'))
assert config['diagnostic'].startswith('ERRORS=0 WARNINGS=0')
state=json.loads((saved/'final-state.json').read_text(encoding='utf-8'))
assert not state['pie_worlds'] and not state['dirty_packages']
result={'date':'2026-10-10','engine':'5.5.4','native_passed':36,'native_with_warnings':native['succeededWithWarnings'],
        'pie_passed':sum(x['passed'] for x in reports.values()),'reports':reports,'metrics':metrics,
        'builds':{'editor':{'exit_code':0,'log':'Saved/AnimationSync/EditorBuild9.log'},'game':{'exit_code':0,'log':'Saved/AnimationSync/GameBuild3.log'}},
        'blueprint_diagnostic':config['diagnostic'],'changed_main_assets':sorted(changed),'preserved_main_assets':preserved,
        'final_editor_state':state,'not_verified':['Cook/package','separate-process networking','packet loss and delay above 100ms','full-terrain visual review','instrumented code and Blueprint coverage']}
(root/'Docs/Validation/animation-sync-pivot-results-2026-10-10.json').write_text(json.dumps(result,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
print(json.dumps({'native_passed':36,'pie_passed':result['pie_passed'],'metrics':metrics,'preserved_main_assets':len(preserved)}))
