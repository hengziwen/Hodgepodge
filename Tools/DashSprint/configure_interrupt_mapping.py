"""Configure current Hero's interruption rules in the existing relationship DataAsset."""
import unreal, json, re
from pathlib import Path

assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
tools = unreal.AssetToolsHelpers.get_asset_tools()
folder = '/Game/Main/Data/TagRelationship'
path = folder + '/DA_Hero_TagRelationshipMapping'
mapping = unreal.load_asset(path)
if not mapping:
    factory = unreal.DataAssetFactory()
    factory.set_editor_property('DataAssetClass', unreal.HodgeAbilityTagRelationshipMapping)
    mapping = tools.create_asset('DA_Hero_TagRelationshipMapping', folder, unreal.HodgeAbilityTagRelationshipMapping, factory)
assert mapping

def container(tags):
    return '(GameplayTags=(' + ','.join('(TagName="' + tag + '")' for tag in tags) + '))'

attacks = ['Ability.Attack', 'Ability.Type.Action.Melee', 'Ability.Type.Action.WeaponFire', 'Ability.Type.Action.Grenade']
dash = unreal.HodgeAbilityTagRelationship()
dash.import_text('(AbilityTag=(TagName="Ability.Type.Action.Dash"),AbilityTagsToBlock=' + container(attacks) +
                 ',AbilityTagsToCancel=' + container(attacks) + ',AbilityTagsToCancelExceptions=' +
                 container(['Ability.Type.StatusChange.Death', 'Status.Death']) +
                 ',ActivationBlockedTags=' + container(['Status.Death']) + ',bForceCancel=True)')
death = unreal.HodgeAbilityTagRelationship()
death.import_text('(AbilityTag=(TagName="Ability.Type.StatusChange.Death"),AbilityTagsToBlock=' +
                  container(['Ability.Type.Action.Dash']) + ')')
existing = mapping.get_editor_property('AbilityTagRelationships')
rows = [row for row in existing if not any(name in row.get_editor_property('AbilityTag').export_text()
        for name in ['Ability.Type.Action.Dash', 'Ability.Type.StatusChange.Death'])]
mapping.set_editor_property('AbilityTagRelationships', rows + [dash, death])
modified = [mapping]
ability_tags = {}
for index in range(1, 6):
    bp = unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/GA_Attack_' + str(index))
    cdo = unreal.get_default_object(bp.generated_class())
    tags = cdo.get_editor_property('AbilityTags')
    values = set(re.findall(r'TagName="([^"]+)"', tags.export_text()))
    values.update(['Ability.Attack', 'Ability.Attack.Light.0' + str(index), 'Ability.Type.Action.Melee'])
    tags.import_text(container(sorted(values)))
    cdo.set_editor_property('AbilityTags', tags)
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    ability_tags[bp.get_path_name()] = tags.export_text()
    modified.append(bp)

pawn = unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData')
pawn.set_editor_property('TagRelationshipMapping', mapping)
modified.append(pawn)
for asset in modified:
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset, False), asset
result = {'mapping': mapping.get_path_name(), 'pawn_data': pawn.get_path_name(), 'rows': [row.export_text() for row in rows + [dash, death]], 'abilities': ability_tags}
Path(unreal.Paths.project_saved_dir()).joinpath('DashInterrupt/configuration.json').write_text(json.dumps(result, indent=2), encoding='utf-8')
print(json.dumps({'mapping': result['mapping'], 'configured_attacks': len(ability_tags), 'rule_count': len(rows) + 2}))
