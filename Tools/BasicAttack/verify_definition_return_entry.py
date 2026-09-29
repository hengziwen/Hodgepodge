"""Client PIE with node 01's input edge temporarily targeting Combo.Entry."""
import json
from pathlib import Path
import unreal

sub = next(x for x in unreal.ObjectIterator(unreal.EnhancedInputLocalPlayerSubsystem)
           if 'Default__' not in x.get_name() and x.get_world() is not None
           and not unreal.GameplayStatics.get_player_controller(x.get_world(), 0).has_authority())
world = sub.get_world()
pc = unreal.GameplayStatics.get_player_controller(world, 0)
peer = next(x for x in unreal.ObjectIterator(unreal.HodgePlayerState) if x.get_world() and x.has_authority()
            and x.get_editor_property('PlayerId') == pc.player_state.get_editor_property('PlayerId'))
combos = [p.get_component_by_class(unreal.HodgeComboComponent) for p in [pc.player_state, peer]]
systems = [p.get_component_by_class(unreal.HodgeAbilitySystemComponent) for p in [pc.player_state, peer]]
tag = unreal.GameplayTag()
tag.import_text('(TagName="Status.Attack")')
action = unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack')
state = {'start': unreal.GameplayStatics.get_time_seconds(world), 'checks': [], 'clicked': False}
path = Path(unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_saved_dir())) / 'Tests/definition_combo_return_entry.json'


def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    state['passed'] = error is None
    if error: state['error'] = str(error)
    path.write_text(json.dumps(state, indent=2), encoding='utf-8')
    unreal.log('DEFINITION_RETURN_ENTRY_TEST ' + json.dumps(state))


def tick(delta):
    try:
        elapsed = unreal.GameplayStatics.get_time_seconds(world) - state['start']
        nodes = [c.get_current_combo_tag().export_text().split('"')[1] for c in combos]
        if elapsed > .4 and not state['checks']:
            assert nodes == ['Combo.Light.01'] * 2, nodes
            state['checks'].append('source_active_both_ends')
        if elapsed > 2.1 and not state['clicked']:
            sub.inject_input_vector_for_action(action, unreal.Vector(1, 0, 0), [], [])
            state['clicked'] = True
        if elapsed > 2.6:
            assert nodes == ['Combo.Entry'] * 2, nodes
            assert all(a.get_gameplay_tag_count(tag) == 0 for a in systems)
            state['checks'].extend(['input_edge_returns_entry_both_ends', 'node_tags_cleared_both_ends'])
            finish()
    except Exception as error: finish(error)


sub.inject_input_vector_for_action(action, unreal.Vector(1, 0, 0), [], [])
callback = unreal.register_slate_post_tick_callback(tick)
print('STARTED_RETURN_ENTRY_TEST')
