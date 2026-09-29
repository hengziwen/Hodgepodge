"""PIE check: all five stages run with ignored manual durations set to zero in memory."""
import json
import re
from pathlib import Path
import unreal

sub = next(x for x in unreal.ObjectIterator(unreal.EnhancedInputLocalPlayerSubsystem)
           if 'Default__' not in x.get_name() and x.get_world() is not None)
world = sub.get_world()
pc = unreal.GameplayStatics.get_player_controller(world, 0)
pawn = pc.get_controlled_pawn()
asc = unreal.AbilitySystemLibrary.get_ability_system_component(pawn)
combo = pc.player_state.get_component_by_class(unreal.HodgeComboComponent)
anim = pawn.get_component_by_class(unreal.SkeletalMeshComponent).get_anim_instance()
action = unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack')
rows = json.loads(unreal.DataTableFunctionLibrary.export_data_table_to_json_string(
    unreal.load_asset('/Game/CodexText/DefinitionCombo/DT_LightCombo')))
next_nodes = {}
for row in rows:
    for transition in row['Transitions']:
        if 'InputIntent.Attack.Light' in transition:
            next_nodes[row['Name']] = re.search(r'TargetComboTag=\(TagName="([^"]+)"\)', transition).group(1)
expected = []
node = next_nodes['Combo.Entry']
while node:
    index = int(node.rsplit('.', 1)[1])
    assert index not in expected, 'This test requires a finite input chain'
    expected.append(index)
    node = next_nodes.get(node)
assert sorted(expected) == [1, 2, 3, 4, 5], expected
assets = []
for index in range(1, 6):
    definition = unreal.load_asset('/Game/CodexText/DefinitionCombo/DA_Attack_%d' % index)
    config = definition.get_editor_property('execution_config')
    timeline = config.timeline_task_config.timeline
    assert timeline.get_editor_property('use_montage_duration')
    assets.append((timeline, timeline.get_editor_property('duration'), config.montage))
for timeline, duration, montage in assets:
    timeline.set_editor_property('duration', 0.0)
state = {'visited': [], 'checks': [], 'pressed': [], 'started': unreal.GameplayStatics.get_time_seconds(world)}
output = Path(unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_saved_dir())) / 'Tests/timeline_montage_duration_pie.json'

def click(): sub.inject_input_vector_for_action(action, unreal.Vector(1, 0, 0), [], [])
def check(label, value):
    assert value, label
    state['checks'].append(label)
def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    for timeline, duration, montage in assets:
        timeline.set_editor_property('duration', duration)
    state['passed'] = error is None
    if error: state['error'] = str(error)
    output.write_text(json.dumps(state, indent=2), encoding='utf-8')
    unreal.log('MONTAGE_DURATION_PIE ' + json.dumps(state))
def tick(delta):
    try:
        node = combo.get_current_combo_tag().export_text().split('"')[1]
        if node.startswith('Combo.Light.'):
            index = int(node.rsplit('.', 1)[1])
            timeline, duration, montage = assets[index - 1]
            if index not in state['visited']:
                check('stage_%d_ignores_zero_duration' % index, anim.get_current_active_montage() == montage)
                state['visited'].append(index)
            position = anim.montage_get_position(montage)
            windows = [e for e in timeline.get_editor_property('events')
                       if 'Status.Attack.Cancel.NextAttack' in e.window_tag.export_text()]
            if node in next_nodes and windows and position > windows[0].start_time + .08 and index not in state['pressed']:
                click()
                state['pressed'].append(index)
        elif node == 'Combo.Entry' and expected[-1] in state['visited']:
            check('all_five_stages', state['visited'] == expected)
            for name in ['Status.Attack', 'Status.Attack.Recovery', 'Status.Attack.Cancel.Move', 'Status.Attack.Cancel.NextAttack']:
                tag = unreal.GameplayTag()
                tag.import_text('(TagName="%s")' % name)
                check('clean_' + name, asc.get_gameplay_tag_count(tag) == 0)
            finish()
            return
        if unreal.GameplayStatics.get_time_seconds(world) - state['started'] > 25:
            raise RuntimeError('Five-stage montage duration test timed out')
    except Exception as error:
        finish(error)
click()
callback = unreal.register_slate_post_tick_callback(tick)
print('STARTED_MONTAGE_DURATION_PIE')
