"""记录并恢复本次测试改变的地图、PIE 与性能设置；不保存正式资产。"""
import gc
import json
from pathlib import Path
import unreal

WORK = Path(unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_saved_dir())) / 'DamageNumbers'
path = WORK / 'editor-state.json'
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
play = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.LevelEditorPlaySettings'))
performance = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.EditorPerformanceSettings'))
world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()

if not globals().get('RESTORE_NUMBERS_EDITOR', False):
    # 使用带时间戳的批次目录时可重复保留原始设置，此处拒绝覆盖本批快照。
    if not path.exists():
        snapshot = {'map': world.get_path_name().split('.')[0],
                    'clients': play.get_editor_property('PlayNumberOfClients'),
                    'single_process': play.get_editor_property('RunUnderOneProcess'),
                    'net_mode': 'unexposed_enum_restore_single_baseline',
                    'throttle': performance.get_editor_property('bThrottleCPUWhenNotForeground'),
                    'slate': unreal.SystemLibrary.get_console_variable_int_value('Slate.bAllowThrottling'),
                    'idle': unreal.SystemLibrary.get_console_variable_int_value('t.IdleWhenNotForeground')}
        path.write_text(json.dumps(snapshot, indent=2), encoding='utf-8')
    print(path.read_text(encoding='utf-8'))
else:
    snapshot = json.loads(path.read_text(encoding='utf-8'))
    play.set_editor_property('PlayNumberOfClients', snapshot['clients'])
    play.set_editor_property('RunUnderOneProcess', snapshot['single_process'])
    # UE 5.5 的 PIE 模式枚举不能由 Python 读取；明确恢复单人基线，不声称完整还原。
    assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(1, False)
    performance.set_editor_property('bThrottleCPUWhenNotForeground', snapshot['throttle'])
    for name, key in [('Slate.bAllowThrottling', 'slate'), ('t.IdleWhenNotForeground', 'idle')]:
        unreal.SystemLibrary.execute_console_command(world, name + ' ' + str(snapshot[key]))
    world = None
    gc.collect()
    assert unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).load_level(snapshot['map'])
    print('已恢复地图与本次记录的设置')
