"""Listen-server latency/rejection/loss regression; restores emulation on completion."""
import json
from pathlib import Path
import unreal

sub = next(x for x in unreal.ObjectIterator(unreal.EnhancedInputLocalPlayerSubsystem)
           if 'Default__' not in x.get_name() and x.get_world() is not None
           and not unreal.GameplayStatics.get_player_controller(x.get_world(), 0).has_authority())
world = sub.get_world()
pc = unreal.GameplayStatics.get_player_controller(world, 0)
player_id = pc.player_state.get_editor_property('PlayerId')
peer = next(x for x in unreal.ObjectIterator(unreal.HodgePlayerState)
            if x.get_world() is not None and x.has_authority() and x.get_editor_property('PlayerId') == player_id)
combo = pc.player_state.get_component_by_class(unreal.HodgeComboComponent)
peer_combo = peer.get_component_by_class(unreal.HodgeComboComponent)
asc = pc.player_state.get_component_by_class(unreal.AbilitySystemComponent)
peer_asc = peer.get_component_by_class(unreal.AbilitySystemComponent)
anim = pc.get_controlled_pawn().get_component_by_class(unreal.SkeletalMeshComponent).get_anim_instance()
action = unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack')
tags = []
for name in ['Status.Attack', 'Status.Attack.Cancel.Move', 'Status.Attack.Cancel.NextAttack']:
    tag = unreal.GameplayTag(); tag.import_text('(TagName="%s")' % name); tags.append(tag)
state = {'phase': 'accepted', 'checks': [], 'changes': [], 'sent': [], 'last': None}
path = Path(unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_saved_dir())) / 'Tests/definition_combo_network.json'


def now(): return unreal.GameplayStatics.get_time_seconds(world)
def node(c): return c.get_current_combo_tag().export_text().split('"')[1]
def click(): sub.inject_input_vector_for_action(action, unreal.Vector(1, 0, 0), [], [])
def emulate(lag=0, loss=0, duplicate=0):
    for w in [world, peer.get_world()]:
        for key, value in [('PktLag', lag), ('PktLoss', loss), ('PktDup', duplicate)]:
            unreal.SystemLibrary.execute_console_command(w, 'NetEmulation.%s %d' % (key, value))


def check(name, value):
    state['checks'].append({'name': name, 'passed': bool(value)})
    assert value, name


def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    emulate()
    state['passed'] = error is None
    if error: state['error'] = str(error)
    path.write_text(json.dumps(state, indent=2), encoding='utf-8')
    unreal.log('DEFINITION_NETWORK ' + json.dumps(state))


def start(phase):
    emulate()
    state.update(phase=phase, start=now(), sent=[], last=None)
    click()


def tick(delta):
    try:
        elapsed = now() - state['start']
        phase = state['phase']
        pair = (node(combo), node(peer_combo))
        if pair != state['last']:
            state['changes'].append({'phase': phase, 'time': elapsed, 'client': pair[0], 'server': pair[1]})
            state['last'] = pair
        if elapsed > .6 and 'emulated' not in state['sent']:
            check(phase + '_source_ready', pair == ('Combo.Light.01', 'Combo.Light.01'))
            emulate(750 if phase != 'loss' else 100, 10 if phase == 'loss' else 0, 10 if phase == 'loss' else 0)
            state['sent'].append('emulated')
        if phase == 'late' and elapsed > .8 and 'paused' not in state['sent']:
            anim.montage_pause(anim.get_current_active_montage())
            state['sent'].append('paused')
        if phase == 'late' and elapsed > 2.1 and 'resumed' not in state['sent']:
            anim.montage_resume(anim.get_current_active_montage())
            state['sent'].append('resumed')
        trigger_ready = (elapsed > 2.1 and pair == ('Combo.Light.01', 'Combo.Entry')) if phase == 'late' else elapsed >= (2.15 if phase == 'accepted' else 2.4)
        if trigger_ready and 'next' not in state['sent']:
            click(); state['sent'].append('next')
        if elapsed > 8:
            rows = [x for x in state['changes'] if x['phase'] == phase]
            check(phase + '_predicted_second', any(x['client'] == 'Combo.Light.02' for x in rows))
            server_saw_second = any(x['server'] == 'Combo.Light.02' for x in rows)
            check(phase + '_authority_result', not server_saw_second if phase == 'late' else server_saw_second)
            check(phase + '_converges_entry', pair == ('Combo.Entry', 'Combo.Entry'))
            check(phase + '_no_residual_tags', all(a.get_gameplay_tag_count(t) == 0 for a in [asc, peer_asc] for t in tags))
            if phase == 'accepted': start('late')
            elif phase == 'late': start('loss')
            else: finish()
    except Exception as error:
        finish(error)


start('accepted')
callback = unreal.register_slate_post_tick_callback(tick)
print('STARTED_NETWORK', path)
