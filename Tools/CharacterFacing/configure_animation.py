"""Configure only the two formal locomotion blueprints; preserve all directional assets."""
import unreal, json
from pathlib import Path

assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
main = unreal.load_asset('/Game/Main/Character/Hero/Anim/ABP_Pover_Base')
layer = unreal.load_asset('/Game/Main/Character/Hero/Anim/Layer/ABP_Pover_LocomotionBase')
lib = unreal.HodgeAnimationAuthoringLibrary
result = lib.configure_hero_facing_modes(main, layer)
assert result.startswith('OK'), result
diagnostics = {name: lib.compile_animation_blueprint(asset) for name, asset in [('main', main), ('layer', layer)]}
assert all(value.startswith('ERRORS=0') for value in diagnostics.values()), diagnostics
for asset in [main, layer]:
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset, False)
out = Path(unreal.Paths.project_saved_dir()) / 'FacingImplementation'
out.joinpath('animation-configuration.json').write_text(json.dumps({'result': result, 'diagnostics': diagnostics}, indent=2), encoding='utf-8')
print(json.dumps({'result': result, 'diagnostics': diagnostics}))
