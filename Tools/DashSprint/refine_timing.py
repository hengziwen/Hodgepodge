"""Migrate only Main Dash timing and the Sprint Pivot callback, preserving other assets."""
import unreal, json
from pathlib import Path

assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
profile = unreal.load_asset('/Game/Main/Character/Hero/Ability/Sprint/DA_Hero_SprintAbility')
for name, value in {'MontagePlayRate': 1., 'MoveCancelOpenTime': .5, 'bAllowMoveCancel': True,
                    'HandoffOpenTime': .5, 'HandoffCloseTime': 1.2, 'HandoffNetworkGrace': .5,
                    'MoveIntentThreshold': .1, 'SprintTurnPlayRate': 1., 'SprintTurnBlendOutTime': .12, 'SprintCyclePlayRate': 1.}.items():
    profile.set_editor_property(name, value)
layer = unreal.load_asset('/Game/Main/Character/Hero/Anim/Layer/ABP_Pover_LocomotionBase')
main = unreal.load_asset('/Game/Main/Character/Hero/Anim/ABP_Pover_Base')
result = unreal.HodgeAnimationAuthoringLibrary.configure_hero_sprint_pivot_timing(main, layer)
assert result.startswith('OK'), result
diagnostics = {}
for asset in [layer, main]:
    value = unreal.HodgeAnimationAuthoringLibrary.compile_animation_blueprint(asset)
    diagnostics[asset.get_path_name()] = value
    assert value.startswith('ERRORS=0'), value
for asset in [profile, layer, main]:
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset, False), asset
out = Path(unreal.Paths.project_saved_dir()) / 'DashTimingRefinement'
out.joinpath('asset-migration.json').write_text(json.dumps({'result': result, 'diagnostics': diagnostics}, indent=2), encoding='utf-8')
print(json.dumps({'result': result, 'diagnostics': diagnostics}))
