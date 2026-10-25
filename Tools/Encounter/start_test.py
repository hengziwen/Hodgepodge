"""在独立刷怪示例关卡启动指定 PIE 模式，等待正式 Hero 初始化后接入验证。"""
from pathlib import Path
import json
import time
import traceback
import unreal

WORK = Path(globals()['LOCK_ON_WORK'])
MODE = globals()['LOCK_ON_MODE']
RUNNER = globals()['LOCK_ON_RUNNER']
WORK.mkdir(parents=True, exist_ok=True)
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
settings = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.EditorPerformanceSettings'))
performance = WORK / 'performance-restore.json'
if not performance.exists():
    performance.write_text(json.dumps({'throttle': settings.get_editor_property('bThrottleCPUWhenNotForeground'),
                                      'slate': unreal.SystemLibrary.get_console_variable_int_value('Slate.bAllowThrottling'),
                                      'idle': unreal.SystemLibrary.get_console_variable_int_value('t.IdleWhenNotForeground')}), encoding='utf-8')
assert unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).load_level('/Game/Main/Maps/L_EncounterValidation')
assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(1 if MODE == 'single' else 2, MODE == 'dedicated')
settings.set_editor_property('bThrottleCPUWhenNotForeground', False)
world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
unreal.SystemLibrary.execute_console_command(world, 'Slate.bAllowThrottling 0')
unreal.SystemLibrary.execute_console_command(world, 't.IdleWhenNotForeground 0')
world = None
state = {'start': time.monotonic()}


def prepare(delta):
    """仅等待既有真实初始化，不移动敌人、不重写生命或战斗参数。"""
    try:
        worlds = unreal.EditorLevelLibrary.get_pie_worlds(True)
        peers = [p for w in worlds for p in unreal.GameplayStatics.get_all_actors_of_class(w, unreal.HodgeHeroCharacter)]
        ready = [p for p in peers if p.get_hodge_ability_system_component() and
                 p.get_component_by_class(unreal.HodgeLockOnComponent) and p.get_component_by_class(unreal.HodgeLockOnComponent).is_ready()]
        expected = 1 if MODE == 'single' else (6 if MODE == 'dedicated' else 4)
        if len(ready) >= expected:
            unreal.unregister_slate_post_tick_callback(state['callback'])
            exec(Path(RUNNER).read_text(encoding='utf-8-sig'), {'LOCK_ON_WORK': str(WORK), 'LOCK_ON_MODE': MODE, 'unreal': unreal})
        elif time.monotonic() - state['start'] > 70:
            raise TimeoutError('正式 Hero 初始化超时')
    except Exception:
        unreal.unregister_slate_post_tick_callback(state['callback'])
        (WORK / (MODE + '-startup-error.txt')).write_text(traceback.format_exc(), encoding='utf-8')


state['callback'] = unreal.register_slate_post_tick_callback(prepare)
unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_begin_play()
