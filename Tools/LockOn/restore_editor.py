"""结束锁定验证后恢复地图、性能和截图；不覆盖任何未保存的编辑器包。"""
import gc
import json
import shutil
import unreal
from pathlib import Path

WORK = Path(globals().get('LOCK_ON_WORK', 'E:/Project/Git/Hodgepodge/Saved/LockOn'))


def restore():
    # PIE 停止是异步过程，必须先单独执行 stop_test.py 并等待编辑器完成。
    assert not unreal.EditorLevelLibrary.get_pie_worlds(False), '请先停止 PIE'
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages(), '存在未保存地图'
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages(), '存在未保存资产'
    performance = json.loads((WORK / 'performance-restore.json').read_text(encoding='utf-8'))
    settings = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.EditorPerformanceSettings'))
    settings.set_editor_property('bThrottleCPUWhenNotForeground', performance['throttle'])
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    unreal.SystemLibrary.execute_console_command(world, 'Slate.bAllowThrottling ' + str(performance['slate']))
    unreal.SystemLibrary.execute_console_command(world, 't.IdleWhenNotForeground ' + str(performance['idle']))
    world = None
    gc.collect()
    original = json.loads((WORK / 'editor-before.json').read_text(encoding='utf-8'))
    assert unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).load_level(original['map'])
    # 最初没有记录 PIE 选项，恢复单人独立模式作为明确的测试基线。
    assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(1, False)
    screenshot = WORK / 'UI-Slate-before.png'
    if screenshot.exists():
        shutil.copy2(screenshot, WORK.parent / 'UIFoundation/UI-Slate.png')
    result = {
        'map': unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world().get_path_name().split('.')[0],
        'dirty_maps': [p.get_path_name() for p in unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()],
        'dirty_content': [p.get_path_name() for p in unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()],
        'pie': len(unreal.EditorLevelLibrary.get_pie_worlds(False)),
        'performance': performance,
        'play_settings': 'single standalone baseline; original PIE settings were not recorded',
    }
    (WORK / 'editor-restored.json').write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
    print(json.dumps(result, ensure_ascii=False))


restore()
