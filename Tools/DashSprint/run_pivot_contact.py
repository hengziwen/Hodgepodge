"""Compare old in-place Pivot with root-motion Pivot and exercise its cancellation."""
import unreal, builtins, json, math
from pathlib import Path

root = Path(unreal.Paths.project_dir())
source = root.joinpath('Tools/DashSprint/run_e2e.py').read_text(encoding='utf-8')
source = source.replace("cases = ['forward_tap','backward_tap','side_tap','held_sprint','release_sprint','stop_sprint','move_cancel','delayed_hold','delayed_handoff','pivot_turn']",
                        "cases = ['baseline_pivot','root_pivot','root_release','root_attack']")
source = source.replace("name = ('network-' if s.get('network') else 'single-') + str(s.get('lag_ms',0)) + 'ms'",
                        "name = 'contacts-' + str(s.get('lag_ms',0)) + 'ms'")
source = source.replace("['HoldThreshold','HandoffOpenTime','HandoffCloseTime']", "['HoldThreshold','HandoffOpenTime','HandoffCloseTime','SprintPivotMontage']")
source = source.replace("            if case=='delayed_hold':", "            if case=='baseline_pivot':profile.set_editor_property('SprintPivotMontage',None)\n            if case=='delayed_hold':")
source = source.replace("if case=='pivot_turn' and age>=1.25:", "if age>=1.25:")
source = source.replace("held= age<(.12 if case.endswith('tap') or case=='move_cancel' else .9 if case=='release_sprint' else 3.5)",
                        "held=age<(1.65 if case=='root_release' else 3.5)")
source = source.replace("        if age-ctx['last']>=.025:",
                        "        if case=='root_attack' and 1.65<age<1.70 and not ctx.get('attack_sent'):\n            sub.inject_input_vector_for_action(unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack'),unreal.Vector(1,0,0),[],[])\n            ctx['attack_sent']=True\n        if age-ctx['last']>=.025:")
source = source.replace("        'pivot':pivots}",
                        "        'pivot':pivots,'feet':[[v.x,v.y,v.z] for v in [p.mesh.get_socket_location('Bip001LFoot'),p.mesh.get_socket_location('Bip001RFoot')]]}")
source = source.replace("            case=cases[ctx['index']]", "            case=cases[ctx['index']];ctx['attack_sent']=False")
start = source.index('def validate(case,samples):')
end = source.index('def tick(delta):', start)
source = source[:start] + '''def validate(case,samples):
    assert samples and any(row['owner']['sprinting'] for row in samples),(case,'Sprint required')
    active=[row for row in samples if row['owner']['pivot']]
    assert active,(case,'Pivot must actually execute')
    if case in ['root_release','root_attack']:
        assert not any(row['owner']['pivot'] for row in samples if row['t']>2.),(case,'cancel must stop own Pivot montage')
        assert not samples[-1]['owner']['sprinting'],(case,'cancellation must end Sprint')
    if case=='root_pivot':
        assert max(x['time'] for row in active for x in row['owner']['pivot'])>1.4,(case,'authored recovery must play')
    return {'case':case,'passed':True,'final':samples[-1]}

''' + source[end:]
exec(compile(source, 'pivot_contact_suite.py', 'exec'))
