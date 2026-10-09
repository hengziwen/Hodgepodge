"""Finish coverage after PIE stop; discard only isolated probe configuration changes."""
import unreal, json

assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
dirty = unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages() + unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
if dirty:
    assert all(p.get_path_name().startswith('/Game/CodexText/HitReactionValidation/') for p in dirty), [p.get_path_name() for p in dirty]
    result = unreal.EditorLoadingAndSavingUtils.reload_packages(dirty, unreal.ReloadPackagesInteractionMode.ASSUME_POSITIVE)
    assert result[0], result
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
print(json.dumps({'pie': [], 'dirty': [], 'network_simulation': 'Off was sent before PIE shutdown'}))
unreal.SystemLibrary.execute_console_command(unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world(), 'QUIT_EDITOR')
