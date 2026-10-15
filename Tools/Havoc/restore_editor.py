"""完成验证后恢复已记录的地图、性能与调试设置，不覆盖未保存资产。"""
import gc
import json
from pathlib import Path
import unreal

root = Path(unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir()))
work = root / 'Saved/Havoc'
assert not unreal.EditorLevelLibrary.get_pie_worlds(True), '先停止PIE'
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
original = json.loads((work / 'editor-state-end.json').read_text(encoding='utf-8'))['map'].split('.')[0]
performance = json.loads((work / 'performance-restore.json').read_text(encoding='utf-8'))
settings = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.EditorPerformanceSettings'))
settings.set_editor_property('bThrottleCPUWhenNotForeground', performance['throttle'])
world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
for name, key in [('Slate.bAllowThrottling', 'slate'), ('t.IdleWhenNotForeground', 'idle')]:
    unreal.SystemLibrary.execute_console_command(world, name + ' ' + str(performance[key]))
debug = work / 'UIRegression/debug-restore.json'
if debug.exists():
    for name, value in json.loads(debug.read_text(encoding='utf-8')).items():
        unreal.SystemLibrary.execute_console_command(world, name + ' ' + str(value))
world = None
gc.collect()
assert unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).load_level(original)
# 原PIE选项没有完整快照，明确恢复项目常用单人基线而不声称逐项还原。
assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(1, False)
result = {'map': original, 'performance': performance, 'play_settings': 'single baseline',
          'dirty': [p.get_path_name() for p in unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages() + unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()]}
(work / 'editor-restored-final.json').write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps(result, ensure_ascii=False))
