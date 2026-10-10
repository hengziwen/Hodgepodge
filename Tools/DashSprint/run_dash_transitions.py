"""Sample capsule, mesh and Root continuity around Dash recovery and handoff."""
import unreal, builtins
from pathlib import Path

s = builtins.HODGE_REACTION_TEST
root = Path(unreal.Paths.project_dir())
source = root.joinpath('Tools/DashSprint/run_e2e.py').read_text(encoding='utf-8')
source = source.replace("cases = ['forward_tap','backward_tap','side_tap','held_sprint','release_sprint','stop_sprint','move_cancel','delayed_hold','delayed_handoff','pivot_turn']",
    "cases = ['idle_tap','tap_move','held_move','late_tap_move','late_held_move','held_sprint','backward_idle_tap']")
source = source.replace("name = ('network-' if s.get('network') else 'single-') + str(s.get('lag_ms',0)) + 'ms'",
    "name = s.get('transition_phase','baseline') + ('-network-' if s.get('network') else '-single-') + str(s.get('lag_ms',0)) + 'ms'")
source = source.replace("        'pivot':pivots}", """        'pivot':pivots,
        'actor_z':p.get_actor_location().z, 'mesh_relative_z':p.mesh.relative_location.z,
        'root_cs_z':p.mesh.get_socket_transform('Root',unreal.RelativeTransformSpace.RTS_COMPONENT).translation.z,
        'pelvis_cs_z':p.mesh.get_socket_transform('Bip001Pelvis',unreal.RelativeTransformSpace.RTS_COMPONENT).translation.z,
        'foot_world_z':[p.mesh.get_socket_location(b).z for b in ['Bip001LFoot','Bip001RFoot']],
        'dash_weight':max(lib.inspect_montage_state(p,m).weight for m in montages),
        'dash_playing':any(lib.inspect_montage_state(p,m).playing for m in montages),
        'dash_stopped':all(lib.inspect_montage_state(p,m).stopped for m in montages),
        'acceleration':(p.character_movement.get_current_acceleration().x**2+p.character_movement.get_current_acceleration().y**2)**.5}
""")
source = source.replace("if case=='backward_tap' else", "if case in ['backward_tap','backward_idle_tap'] else")
source = source.replace("        moving = age<(.9 if case=='stop_sprint' else .10 if case.endswith('tap') else 3.6)",
    "        moving = (age<.12 if case=='backward_idle_tap' else case!='idle_tap' and age>=(1.18 if case.startswith('late_') else .7 if case in ['tap_move','held_move'] else 0.) and age<3.6)")
source = source.replace("        held= age<(.12 if case.endswith('tap') or case=='move_cancel' else .9 if case=='release_sprint' else 3.5)",
    "        held=age<(.12 if case in ['idle_tap','tap_move','late_tap_move','backward_idle_tap'] else 3.5)")
source = source.replace("        if moving: sub.inject_input_vector_for_action(move,vector,[],[])",
    "        if moving:\n            ctx.setdefault('move_injected_at',age)\n            sub.inject_input_vector_for_action(move,vector,[],[])")
source = source.replace("            ctx.update({'stage':'run','start':now,'pressed':False})", "            ctx.pop('move_injected_at',None)\n            ctx.update({'stage':'run','start':now,'pressed':False})")
source = source.replace("            sub.inject_input_vector_for_action(move,first_move,[],[])",
    "            if case in ['held_sprint','backward_idle_tap']:\n                ctx.setdefault('move_injected_at',0.)\n                sub.inject_input_vector_for_action(move,first_move,[],[])")
start = source.index('def validate(case,samples):')
end = source.index('def tick(delta):', start)
source = source[:start] + """def validate(case,samples):
    assert samples and any(r['owner']['dash_committed'] for r in samples),(case,'Dash must execute')
    if s.get('transition_phase')=='fixed':
        for role in ['owner','server']:
            values=[r[role] for r in samples]
            assert max(v['actor_z'] for v in values)-min(v['actor_z'] for v in values)<.15,(case,role,'capsule Z continuity')
            assert max(v['mesh_relative_z'] for v in values)-min(v['mesh_relative_z'] for v in values)<.05,(case,role,'no mesh offset workaround')
            if case in ['idle_tap','backward_idle_tap'] and role=='owner':
                tail_start=min(m.get_play_length()-m.get_editor_property('blend_out').get_editor_property('blend_time')-.03 for m in montages)
                tail=[v['pelvis_cs_z'] for v in values if v['montage_position']>=tail_start and v['dash_weight']>.2]
                idle=[r[role]['pelvis_cs_z'] for r in samples if 2.<r['t']<3.4]
                assert tail and idle and abs(sum(tail)/len(tail)-sum(idle)/len(idle))<.75,(case,role,'no late pelvis drop',tail,idle)
            if case=='held_sprint':
                travel=next(r['t'] for r in samples if r[role]['dash_committed'])
                bridge=[r[role] for r in samples if travel+.2<r['t']<travel+.8]
                assert min((v['velocity'][0]**2+v['velocity'][1]**2)**.5 for v in bridge)>150,(role,'no stop between RMS and Sprint')
            if case in ['tap_move','held_move','late_tap_move','late_held_move']:
                input_at=ctx['move_injected_at']
                onset=next((r['t'] for r in samples if r['t']>=input_at and (r[role]['velocity'][0]**2+r[role]['velocity'][1]**2)**.5>10),None)
                near=[r['t'] for r in samples if input_at-.2<r['t']<input_at+.6]
                frame=max((b-a for a,b in zip(near,near[1:])),default=.033)
                allowance=.04+3*frame+(2*s.get('lag_ms',0)/1000. if role=='server' else 0.)
                assert onset is not None and onset-input_at<allowance,(case,role,'movement must respond without waiting for montage completion',onset)
                if 'held' in case: assert any(r[role]['sprinting'] for r in samples),(case,role,'held handoff must commit')
    return {'case':case,'passed':True,'move_injected_at':ctx.get('move_injected_at'),'final':samples[-1]}

""" + source[end:]
exec(compile(source, 'dash_transition_suite.py', 'exec'))
