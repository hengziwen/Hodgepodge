"""Check actual marker groups, early Pivot recovery and a second turn in the old tail."""
import unreal, builtins
from pathlib import Path

s = builtins.HODGE_REACTION_TEST
source = Path(unreal.Paths.project_dir()).joinpath('Tools/DashSprint/run_e2e.py').read_text(encoding='utf-8')
source = source.replace("cases = ['forward_tap','backward_tap','side_tap','held_sprint','release_sprint','stop_sprint','move_cancel','delayed_hold','delayed_handoff','pivot_turn']",
                       "cases = ['dash_stride','pivot_exit','pivot_tail_reversal','pivot_early_reversal','pivot_release','pivot_attack']")
source = source.replace("name = ('network-' if s.get('network') else 'single-') + str(s.get('lag_ms',0)) + 'ms'",
                       "name = 'sync-' + ('network-' if s.get('network') else 'single-') + str(s.get('lag_ms',0)) + 'ms'")
source = source.replace("        'pivot':pivots}", """        'pivot':pivots,
        'pivot_instance':rm.instance_id if rm else -1, 'pivot_playing':rm.playing if rm else False,
        'sync':[{'group':str(x.group),'sequence':str(x.sequence),'time':x.time,'weight':x.weight,'leader':x.leader,'markers':x.marker_sync} for x in lib.inspect_locomotion_sync(p)],
        'foot_cs':[[v.x,v.y,v.z] for v in [p.mesh.get_socket_transform(b,unreal.RelativeTransformSpace.RTS_COMPONENT).translation for b in ['Bip001LFoot','Bip001RFoot']]]}
""")
source = source.replace("            case=cases[ctx['index']]", "            case=cases[ctx['index']];ctx.pop('pivot_started',None);ctx.pop('second_input_at',None);ctx['attack_sent']=False")
source = source.replace("        if case=='pivot_turn' and age>=1.25:vector=unreal.Vector(0,-1,0)", """        if case.startswith('pivot_') and age>=1.25:
            vector=unreal.Vector(0,-1,0)
            state=lib.inspect_montage_state(owner,profile.get_editor_property('SprintPivotMontage'))
            if state.playing and state.weight>.05:ctx.setdefault('pivot_started',age-state.position/profile.get_editor_property('SprintTurnPlayRate'))
            if 'pivot_started' in ctx:
                delay=.2 if case=='pivot_early_reversal' else profile.get_editor_property('PivotRecoveryEndTime')+.08
                if case in ['pivot_tail_reversal','pivot_early_reversal'] and age>=ctx['pivot_started']+delay:
                    ctx.setdefault('second_input_at',age);vector=unreal.Vector(0,1,0)
                if case=='pivot_attack' and age>=ctx['pivot_started']+.4 and not ctx['attack_sent']:
                    sub.inject_input_vector_for_action(unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack'),unreal.Vector(1,0,0),[],[]);ctx['attack_sent']=True
""")
source = source.replace("        held= age<(.12 if case.endswith('tap') or case=='move_cancel' else .9 if case=='release_sprint' else 3.5)",
                       "        held=age<(ctx.get('pivot_started',100.)+.4 if case=='pivot_release' else 3.5)")
start=source.index('def validate(case,samples):');end=source.index('def tick(delta):',start)
source=source[:start]+'''def validate(case,samples):
    assert any(r['owner']['sprinting'] for r in samples),(case,'Sprint must execute')
    cycle=[x for r in samples for x in r['owner']['sync'] if x['sequence']=='A_Hero_Sprint_F']
    grouped=[x for x in cycle if x['group']=='HodgeSprint']
    assert grouped,(case,'dedicated Cycle must use its isolated group',cycle)
    assert any(x['markers'] for x in grouped),(case,'actual marker synchronization must participate')
    if case.startswith('pivot_'):
        for role in ['owner','server']:
            active=[r for r in samples if r[role]['pivot_playing'] and r[role]['pivot']]
            assert active,(case,role,'real Pivot required')
            first_id=active[0][role]['pivot_instance']
            first=[r for r in active if r[role]['pivot_instance']==first_id]
            maximum=max(r[role]['pivot'][0]['time'] for r in first)
            assert maximum<=profile.get_editor_property('PivotRecoveryEndTime')+.2,(case,role,'must not play the running tail',maximum)
            if case in ['pivot_tail_reversal','pivot_early_reversal']:
                ids=sorted(set(r[role]['pivot_instance'] for r in active))
                assert len(ids)>=2,(case,role,'second Pivot must execute before original tail ends',ids)
                second=next(r for r in active if r[role]['pivot_instance']!=first_id)
                started=first[0]['t']-first[0][role]['pivot'][0]['time']/profile.get_editor_property('SprintTurnPlayRate')
                assert .8<second['t']-started<1.55,(case,role,'second turn timing',second['t']-started)
                second_rows=[r for r in active if r[role]['pivot_instance']==second[role]['pivot_instance']]
                assert min(r[role]['pivot'][0]['time'] for r in second_rows)<.2,(case,role,'new Pivot must start at its beginning')
                assert max(r[role]['pivot'][0]['time'] for r in second_rows)>.75,(case,role,'new Pivot must complete its own turn')
                assert any(abs((r[role]['yaw']+180)%360-180)<8 for r in second_rows),(case,role,'second turn must face the new direction')
        if case in ['pivot_release','pivot_attack']:
            assert not samples[-1]['owner']['sprinting'] and not samples[-1]['server']['sprinting'],(case,'cancel must end Sprint')
            assert not samples[-1]['owner']['pivot_playing'],(case,'cancel must stop own montage')
        if case=='pivot_exit':
            assert any(r['t']>2.5 and r['owner']['sprinting'] and not r['owner']['pivot_playing'] for r in samples),(case,'return to Cycle while Shift held')
    import math
    speeds=[]
    if case=='pivot_exit':
        boundary=ctx['pivot_started']+profile.get_editor_property('PivotRecoveryEndTime')/profile.get_editor_property('SprintTurnPlayRate')
        for a,b in zip(samples,samples[1:]):
            if boundary-.1<a['t']<boundary+.3 and 0<b['t']-a['t']<.1:
                speeds.extend(math.dist(x,y)/(b['t']-a['t']) for x,y in zip(a['owner']['foot_cs'],b['owner']['foot_cs']))
        assert speeds and max(speeds)<3500,(case,'no single-frame foot teleport at recovery',speeds)
    return {'case':case,'passed':True,'second_input_at':ctx.get('second_input_at'),'maximum_recovery_foot_speed':max(speeds) if speeds else None,'final':samples[-1]}

'''+source[end:]
exec(compile(source,'animation_sync_suite.py','exec'))
