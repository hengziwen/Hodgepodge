"""Configure only Dash recovery timing and the Hero layer's foot IK alpha."""
import unreal, json
from pathlib import Path

assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
profile = unreal.load_asset('/Game/Main/Character/Hero/Ability/Sprint/DA_Hero_SprintAbility')
layer = unreal.load_asset('/Game/Main/Character/Hero/Anim/Layer/ABP_Pover_LocomotionBase')
assert profile and layer
travel = profile.get_editor_property('Duration')
profile.set_editor_property('MoveCancelOpenTime', travel)
profile.set_editor_property('HandoffOpenTime', travel)
result = unreal.HodgeAnimationAuthoringLibrary.configure_hero_dash_foot_ik(layer)
assert result.startswith('OK'), result
diagnostic = unreal.HodgeAnimationAuthoringLibrary.compile_animation_blueprint(layer)
assert diagnostic.startswith('ERRORS=0'), diagnostic
for asset in [profile, layer]: assert unreal.EditorAssetLibrary.save_loaded_asset(asset, False)
Path(unreal.Paths.project_saved_dir()).joinpath('DashTransitions/configuration.json').write_text(json.dumps(
    {'graph': result, 'diagnostic': diagnostic, 'travel': travel, 'move_cancel_open': travel, 'handoff_open': travel}, indent=2), encoding='utf-8')
print(json.dumps({'graph': result, 'diagnostic': diagnostic, 'open_time': travel}))
