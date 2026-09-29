"""Validate and persist the authorized default PawnData migration after PIE regressions."""
from pathlib import Path
import json
import unreal

assert not any(x.get_world() for x in unreal.ObjectIterator(unreal.EnhancedInputLocalPlayerSubsystem)
               if 'Default__' not in x.get_name()), 'Stop PIE before migration'
root = '/Game/CodexText/DefinitionCombo/'
pawn = unreal.load_asset('/Game/Main/Data/DA_Dafult_PawnData')
legacy = unreal.load_asset('/Game/Main/Data/DA_Pover')
before = {
    'ability_sets': [x.get_path_name() for x in pawn.get_editor_property('ability_sets')],
    'legacy_abilities': [x.export_text() for x in legacy.get_editor_property('granted_gameplay_abilities')],
}
prepare = Path(unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir())) / 'Tools/BasicAttack/prepare_definition_pie.py'
exec(compile(prepare.read_text(encoding='utf-8').replace(
    'unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_begin_play()', ''), str(prepare), 'exec'))
validator = unreal.get_editor_subsystem(unreal.EditorValidatorSubsystem)
assets = [pawn, unreal.load_asset(root + 'DA_LightCombo')]
assets += [unreal.load_asset(root + 'DA_Attack_%d' % i) for i in range(1, 6)]
validation = {}
for asset in assets:
    result, errors, warnings = validator.is_object_valid(asset, unreal.DataValidationUsecase.SCRIPT)
    validation[asset.get_path_name()] = {'result': str(result), 'errors': [str(x) for x in errors], 'warnings': [str(x) for x in warnings]}
    assert result == unreal.DataValidationResult.VALID, validation[asset.get_path_name()]
for asset in [pawn, legacy]:
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=False)
output = Path(unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_saved_dir())) / 'Tests/definition_combo_migration.json'
output.parent.mkdir(parents=True, exist_ok=True)
output.write_text(json.dumps({'before': before, 'validation': validation, 'saved': True}, indent=2), encoding='utf-8')
print('SAVED_DEFAULT_DEFINITION_COMBO', str(output))
