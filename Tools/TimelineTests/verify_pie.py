"""Run via Unreal MCP during single-player PIE; changes only the granted test ability."""
import json
from pathlib import Path
import unreal

result_path = Path(unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_saved_dir())) / 'Tests' / 'timeline_pie_result.json'
result_path.parent.mkdir(parents=True, exist_ok=True)

world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world()
assert world is not None
player = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.HodgePlayerState)[0]
asc = player.get_component_by_class(unreal.AbilitySystemComponent)
timeline = unreal.load_asset('/Game/CodexText/Timeline/DA_TimelineTest')
tags = {str(e.get_editor_property('event_id')): e.get_editor_property('window_tag')
        for e in timeline.get_editor_property('events') if str(e.get_editor_property('event_id')) != 'Test'}
ability_class = unreal.load_class(None, '/Game/CodexText/Timeline/GA_TimelineTest.GA_TimelineTest_C')
handle = asc.give_ability(ability_class, 1)
baseline = len(asc.get_active_effects(unreal.GameplayEffectQuery()))
assert asc.try_activate_ability(handle)
start = unreal.GameplayStatics.get_time_seconds(world)
state = {'stage': 0, 'rows': [], 'checks': [], 'world': world.get_path_name()}
checkpoints = [(0.10, 'Windup'), (0.30, 'Active'), (0.70, 'Recovery'), (1.15, None)]

def sample(label):
    row = {'label': label, 'time': unreal.GameplayStatics.get_time_seconds(world)-start,
           'tags': {name: asc.get_gameplay_tag_count(tag) for name, tag in tags.items()},
           'effects': len(asc.get_active_effects(unreal.GameplayEffectQuery()))-baseline}
    state['rows'].append(row)
    return row

def tick(delta):
    global start, handle
    try:
        elapsed = unreal.GameplayStatics.get_time_seconds(world)-start
        index = state['stage']
        if index < 4:
            target, phase = checkpoints[index]
            if elapsed < target:
                return
            row = sample('natural_'+str(index))
            expected = {name: int(name == phase or (index == 2 and name == 'MoveCancel')) for name in tags}
            state['checks'].append(row['tags'] == expected)
            state['checks'].append(row['effects'] == int(index == 1))
            state['stage'] += 1
            if index == 3:
                asc.clear_ability(handle)
                handle = asc.give_ability(ability_class, 1)
                assert asc.try_activate_ability(handle)
                start = unreal.GameplayStatics.get_time_seconds(world)
        elif elapsed >= 0.3:
            sample('before_owner_end')
            asc.clear_ability(handle)
            row = sample('after_owner_end')
            state['checks'].append(all(value == 0 for value in row['tags'].values()) and row['effects'] == 0)
            state['passed'] = all(state['checks'])
            result_path.write_text(json.dumps(state, indent=2), encoding='utf-8')
            unreal.unregister_slate_post_tick_callback(callback)
            unreal.log('TIMELINE_PIE_RESULT '+json.dumps(state))
    except Exception as exc:
        asc.clear_ability(handle)
        state['error'] = repr(exc)
        state['passed'] = False
        result_path.write_text(json.dumps(state, indent=2), encoding='utf-8')
        unreal.unregister_slate_post_tick_callback(callback)
        unreal.log_error('TIMELINE_PIE_ERROR '+repr(exc))

callback = unreal.register_slate_post_tick_callback(tick)
print('TIMELINE_PIE_SAMPLING_STARTED', state['world'])
