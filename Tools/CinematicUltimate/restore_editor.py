"""测试结束后恢复实际原地图与性能设置，不覆盖用户未保存内容。"""
import gc
import json
from pathlib import Path
import unreal

WORK=Path(globals().get('LOCK_ON_WORK',str(Path(unreal.Paths.project_saved_dir())/'CinematicUltimate')))
assert not unreal.EditorLevelLibrary.get_pie_worlds(False),'先停止PIE并等待世界销毁'
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages(),'存在未保存地图'
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages(),'存在未保存资产'
performance=json.loads((WORK/'performance-restore.json').read_text(encoding='utf-8'))
settings=unreal.get_default_object(unreal.load_class(None,'/Script/UnrealEd.EditorPerformanceSettings'))
settings.set_editor_property('bThrottleCPUWhenNotForeground',performance['throttle'])
world=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
if (WORK/'debug-restore.json').exists():
    for name,value in json.loads((WORK/'debug-restore.json').read_text(encoding='utf-8')).items():
        unreal.SystemLibrary.execute_console_command(world,name+' '+str(value))
for name,key in [('Slate.bAllowThrottling','slate'),('t.IdleWhenNotForeground','idle')]:
    unreal.SystemLibrary.execute_console_command(world,name+' '+str(performance[key]))
world=None
gc.collect()
original=json.loads((WORK/'editor-state.json').read_text(encoding='utf-8'))['map'].split('.')[0]
assert unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).load_level(original)
# 初始Play设置没有完整快照，恢复明确的单人基线，不声称还原用户所有PIE选项。
assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(1,False)
result={'map':original,'performance':performance,'pie':len(unreal.EditorLevelLibrary.get_pie_worlds(False)),
        'dirty':[package.get_path_name() for package in unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()+unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()],
        'play_settings':'single baseline; original play settings unavailable'}
(WORK/'editor-restored.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps(result,ensure_ascii=False))
