"""Reload the saved Hero assets and verify tag rules and Blueprint defaults."""
import unreal, json
from pathlib import Path

assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
mapping = unreal.load_asset('/Game/Main/Data/TagRelationship/DA_Hero_TagRelationshipMapping')
pawn = unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData')
assets = [mapping, pawn] + [unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/GA_Attack_' + str(i)) for i in range(1, 6)]
result = unreal.EditorLoadingAndSavingUtils.reload_packages([a.get_outermost() for a in assets], unreal.ReloadPackagesInteractionMode.ASSUME_POSITIVE)
assert result[0], result
mapping = unreal.load_asset('/Game/Main/Data/TagRelationship/DA_Hero_TagRelationshipMapping')
assert unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData').get_editor_property('TagRelationshipMapping') == mapping
def tags(*values):
    value = unreal.GameplayTagContainer()
    value.import_text('(GameplayTags=(' + ','.join('(TagName="' + t + '")' for t in values) + '))')
    return value
source = tags('Ability.Type.Action.Dash')
checks = []
for i in range(1, 6):
    bp = unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/GA_Attack_' + str(i))
    target = unreal.get_default_object(bp.generated_class()).get_editor_property('AbilityTags')
    assert 'Ability.Attack.Light.0' + str(i) in target.export_text(), target
    assert mapping.is_ability_cancelled_by_tags(source, target), bp
    checks.append({'ability': bp.get_path_name(), 'tags': target.export_text(), 'cancelled_by_dash': True})
assert not mapping.is_ability_cancelled_by_tags(source, tags('Ability.Attack', 'Ability.Type.StatusChange.Death'))
assert not mapping.is_ability_cancelled_by_tags(source, tags('Ability.Type.Action.Emote'))
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
Path(unreal.Paths.project_saved_dir()).joinpath('DashInterrupt/asset-verification.json').write_text(json.dumps({'checks': checks, 'death_protected': True, 'utility_untargeted': True}, indent=2), encoding='utf-8')
print('Verified saved PawnData mapping, all 5 attack Blueprints, Death protection and unrelated utility')
