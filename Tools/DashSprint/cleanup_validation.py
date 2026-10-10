"""Discard only temporary validation edits after PIE has stopped; never save test overrides."""
import unreal, json

assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
dirty = unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages() + unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
allowed_profile = '/Game/Main/Character/Hero/Ability/Sprint/DA_Hero_SprintAbility'
assert all(p.get_path_name() == allowed_profile or p.get_path_name().startswith('/Game/CodexText/HitReactionValidation/') for p in dirty), [p.get_path_name() for p in dirty]
if dirty:
    result = unreal.EditorLoadingAndSavingUtils.reload_packages(dirty, unreal.ReloadPackagesInteractionMode.ASSUME_POSITIVE)
    assert result[0], result
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
print(json.dumps({'pie': [], 'dirty': [], 'reloaded_test_packages': [p.get_path_name() for p in dirty]}))
