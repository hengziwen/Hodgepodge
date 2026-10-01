"""Validate server-origin point and natural-end transitions on a predicted owning client."""
import json
from pathlib import Path
import unreal

sub = next(x for x in unreal.ObjectIterator(unreal.EnhancedInputLocalPlayerSubsystem)
           if 'Default__' not in x.get_name() and x.get_world() is not None
           and not unreal.GameplayStatics.get_player_controller(x.get_world(), 0).has_authority())
world = sub.get_world()
pc = unreal.GameplayStatics.get_player_controller(world, 0)
peer = next(x for x in unreal.ObjectIterator(unreal.HodgePlayerState) if x.get_world() is not None
            and x.has_authority() and x.get_editor_property('PlayerId') == pc.player_state.get_editor_property('PlayerId'))
combo = pc.get_controlled_pawn().get_component_by_class(unreal.HodgeCombatComponentBase)
server_combo = peer.get_pawn().get_component_by_class(unreal.HodgeCombatComponentBase)
action = unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack')
state = {'start': unreal.GameplayStatics.get_time_seconds(world), 'checked': [], 'changes': [], 'last': None}
path = Path(unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_saved_dir())) / 'Tests/definition_combo_events.json'


def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    state['passed'] = error is None
    if error: state['error'] = str(error)
    path.write_text(json.dumps(state, indent=2), encoding='utf-8')
    unreal.log('DEFINITION_EVENT_TEST ' + json.dumps(state))


def tick(delta):
    try:
        elapsed = unreal.GameplayStatics.get_time_seconds(world) - state['start']
        pair = [c.get_current_combo_tag().export_text().split('"')[1] for c in [combo, server_combo]]
        if pair != state['last']:
            state['changes'].append({'time': elapsed, 'nodes': pair})
            state['last'] = pair
        for time, expected, label in [(1.3, 'Combo.Light.03', 'authority_point'), (4.8, 'Combo.Light.04', 'natural_end_event'), (10., 'Combo.Entry', 'final_cleanup')]:
            if elapsed >= time and label not in state['checked']:
                assert pair == [expected, expected], label + ': ' + str(pair)
                state['checked'].append(label)
        if elapsed >= 10: finish()
    except Exception as error: finish(error)


sub.inject_input_vector_for_action(action, unreal.Vector(1, 0, 0), [], [])
callback = unreal.register_slate_post_tick_callback(tick)
print('STARTED_EVENT_TEST')
