"""Use test configuration in memory, preserving default assets on disk."""
import unreal

pawn_data = unreal.load_asset('/Game/Main/Data/DA_Dafult_PawnData')
ability_set = unreal.load_asset('/Game/Main/Data/DA_Pover')
assert unreal.EditorAssetLibrary.does_asset_exist('/Game/CodexText/Backups/DefinitionCombo_20260928/DA_Dafult_PawnData')
assert unreal.EditorAssetLibrary.does_asset_exist('/Game/CodexText/Backups/DefinitionCombo_20260928/DA_Pover')
old = ability_set.get_editor_property('granted_gameplay_abilities')
ability_set.set_editor_property('granted_gameplay_abilities', [entry for entry in old
    if entry.get_editor_property('ability').get_path_name() != '/Game/CodexText/BasicAttack/GA_BasicAttack.GA_BasicAttack_C'])
sets = list(pawn_data.get_editor_property('ability_sets'))
new_set = unreal.load_asset('/Game/Main/Data/AbilitySet/AS_LightCombo')
if new_set not in sets: sets.append(new_set)
pawn_data.set_editor_property('ability_sets', sets)
pawn_data.set_editor_property('combo_definition', unreal.load_asset('/Game/Main/Data/Combo/DA_LightCombo'))
unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_begin_play()
print('PREPARED_DEFINITION_PIE_WITHOUT_SAVING_DEFAULTS')
