"""在隔离地图验证真实假人生命、测试攻击和恢复；不保存运行时世界。"""
import json
import re
import sys
import time
import traceback
from pathlib import Path

import unreal

sys.path.insert(0, str(Path(unreal.Paths.project_dir()) / 'Tools' / 'HitReactionDummy'))
import control

WORK = Path(unreal.Paths.project_saved_dir()) / 'HitReactionDummy'
play = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.LevelEditorPlaySettings'))
performance = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.EditorPerformanceSettings'))
saved = {name: play.get_editor_property(name) for name in ['PlayNumberOfClients', 'RunUnderOneProcess']}
# UE5.5 未完整导出该枚举，沿用项目已核实的数值读取和原生恢复入口。
try:
    saved_mode = int(play.get_editor_property('PlayNetMode').value)
except TypeError as error:
    saved_mode = int(re.search(r"Cannot pythonize '(\d+)' \(int64\)", str(error)).group(1))
throttle = performance.get_editor_property('bThrottleCPUWhenNotForeground')
idle_settings = {name: unreal.SystemLibrary.get_console_variable_int_value(name)
                 for name in ['Slate.bAllowThrottling', 't.IdleWhenNotForeground']}
cases = ['Light', 'HitStun', 'Knockback', 'Launch', 'Knockdown', 'LaunchAirHit', 'LaunchSlam']
state = {'stage': 'boot', 'wall': time.monotonic(), 'index': -1, 'results': []}


def finish(error=None):
    """先撤销测试来源并结束 PIE，再恢复用户测试偏好和正常退出编辑器。"""
    state['error'] = error
    try:
        control.stop()
    except Exception:
        state['cleanup_error'] = traceback.format_exc()
    unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_end_play()
    state['stage'] = 'closing'
    report = {key: value for key, value in state.items() if key != 'callback'}
    (WORK / 'pie-results.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')


def tick(delta):
    """等待实际游戏 Tick 发生，收集真实动画、运动和受击事件。"""
    try:
        if state['stage'] == 'closing':
            if unreal.EditorLevelLibrary.get_pie_worlds(True):
                return
            assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(1 if saved_mode == 0 else 2, saved_mode == 2)
            for name, value in saved.items():
                play.set_editor_property(name, value)
            performance.set_editor_property('bThrottleCPUWhenNotForeground', throttle)
            world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
            for name, value in idle_settings.items():
                unreal.SystemLibrary.execute_console_command(world, name + ' ' + str(value))
            unreal.unregister_slate_post_tick_callback(state['callback'])
            unreal.SystemLibrary.quit_editor()
            return
        if time.monotonic() - state['wall'] > 140:
            raise RuntimeError('假人 PIE 验证超时')
        if state['stage'] == 'boot':
            worlds = unreal.EditorLevelLibrary.get_pie_worlds(True)
            if not worlds:
                return
            world = worlds[0]
            source = unreal.GameplayStatics.get_player_pawn(world, 0)
            if not isinstance(source, unreal.HodgeHeroCharacter) or not source.get_hodge_ability_system_component():
                return
            cls = unreal.load_asset(control.ROOT + '/BP_HitReactionDummy').generated_class()
            targets = unreal.GameplayStatics.get_all_actors_of_class(world, cls)
            if len(targets) != 1:
                return
            if not targets[0].initialize_enemy_ability_system():
                diagnostic = targets[0].get_enemy_init_component().get_initialization_diagnostic()
                assert not diagnostic.startswith('Missing required'), diagnostic
                return
            control.start()
            state['initial'] = control.snapshot()
            assert state['initial']['health'] == 1000000.0
            assert targets[0].get_controller().get_class() == unreal.AIController.static_class()
            assert targets[0].character_movement.is_moving_on_ground()
            state['stage'] = 'next'
        if state['stage'] == 'next':
            state['index'] += 1
            if state['index'] == len(cases):
                finish()
                return
            control.reset()
            name = cases[state['index']]
            state.update(stage='running', name=name,
                         start=unreal.GameplayStatics.get_time_seconds(control._state['world']),
                         initial_health=control.snapshot()['health'], start_record=len(control._state['records']),
                         chained=False, max_height=95.0, max_x=180.0, phases=[])
            control.probe('Launch' if name.startswith('Launch') else name)
            return
        if state['stage'] == 'running':
            snap = control.snapshot()
            elapsed = snap['time'] - state['start']
            state['max_height'] = max(state['max_height'], snap['position'][2])
            state['max_x'] = max(state['max_x'], snap['position'][0])
            if snap['phase'] not in state['phases']:
                state['phases'].append(snap['phase'])
            if state['name'] in ('LaunchAirHit', 'LaunchSlam') and elapsed > 0.48 and not state['chained']:
                control.probe('AirHit' if state['name'] == 'LaunchAirHit' else 'Slam')
                state['chained'] = True
            if elapsed < (7.0 if state['name'] in ('Knockdown', 'LaunchSlam') else 4.5):
                return
            events = control._state['records'][state['start_record']:]
            reactions = [e for e in events if e['event'] == 'reaction']
            assert snap['health'] < state['initial_health'], '没有发生真实伤害：' + state['name']
            assert snap['health'] > 990000.0 and snap['max_health'] == 1000000.0
            assert reactions, '没有受击结算事件：' + state['name']
            assert not snap['controlled'], '受击没有恢复：' + state['name']
            if state['name'] == 'Light':
                assert any('NO_IMPACT' in e['outcome'] for e in reactions)
                assert any(e['event'] == 'light' for e in events)
            else:
                assert all('APPLIED' in e['outcome'] for e in reactions), [e['outcome'] for e in reactions]
            if state['name'] == 'Knockback':
                assert state['max_x'] > 200.0
            if state['name'].startswith('Launch'):
                assert state['max_height'] > 125.0
            if state['chained']:
                expected = 'AirHit' if state['name'] == 'LaunchAirHit' else 'Slam'
                assert any(expected in e.get('result', '') for e in reactions)
            state['results'].append({'case': state['name'], 'health_before': state['initial_health'],
                                     'health_after': snap['health'], 'max_height': state['max_height'],
                                     'max_x': state['max_x'], 'phases': state['phases'],
                                     'reactions': reactions, 'recovered': True})
            state['stage'] = 'next'
    except Exception:
        finish(traceback.format_exc())


assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(1, False)
performance.set_editor_property('bThrottleCPUWhenNotForeground', False)
editor_world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
for variable in idle_settings:
    unreal.SystemLibrary.execute_console_command(editor_world, variable + ' 0')
state['callback'] = unreal.register_slate_post_tick_callback(tick)
unreal.EditorPythonScripting.set_keep_python_script_alive(True)
unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_begin_play()
