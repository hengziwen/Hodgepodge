"""Saved, network-addressable player probe; leave formal hero definitions unchanged."""
import unreal

ROOT = '/Game/CodexText/EnemyAIValidation'
tools = unreal.AssetToolsHelpers.get_asset_tools()
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)


def data(name, cls):
    found = unreal.load_asset(ROOT + '/' + name)
    if not found:
        factory = unreal.DataAssetFactory()
        factory.set_editor_property('DataAssetClass', cls)
        found = tools.create_asset(name, ROOT, cls, factory)
    assert found
    return found


def tag(name):
    value = unreal.GameplayTag()
    value.import_text('(TagName="' + name + '")')
    return value


source = unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_1')
probe = data('DA_PlayerAttack', unreal.HodgeAbilityDefinition)
probe.set_editor_property('AbilityTag', tag('Status.Attack'))
probe.set_editor_property('AbilityClass', unreal.HodgeGameplayAbility_Melee)
probe.set_editor_property('ExecutionRoute', unreal.HodgeAbilityExecutionRoute.STANDALONE)
probe.set_editor_property('ExecutionBodyTag', tag('State.Combat.Body.Normal'))
probe.set_editor_property('ExecutionConfig', source.get_editor_property('ExecutionConfig'))
hit = unreal.HodgeHitEffectConfig()
hit.import_text(source.get_editor_property('DefaultHitConfig').export_text())
hit.set_editor_property('DamageMultiplier', .3)
reaction = unreal.HodgeHitReactionConfig()
reaction.set_editor_property('AttackJudgementTag', tag('Combat.Attack.Judgement.Skill'))
impact = unreal.HodgeImpactSpec()
impact.set_editor_property('Type', unreal.HodgeImpactType.HIT_STUN)
impact.set_editor_property('ControlDuration', .7)
reaction.set_editor_property('Impacts', [impact])
hit.set_editor_property('Reaction', reaction)
probe.set_editor_property('DefaultHitConfig', hit)
aset = data('AS_PlayerAttack', unreal.HodgeAbilitySet)
entry = unreal.HodgeAbilitySet_Definition()
entry.import_text('(Definition="/Script/Hodgepodge.HodgeAbilityDefinition\'' + probe.get_path_name() + '\'",AbilityLevel=1)')
aset.set_editor_property('GrantedAbilityDefinitions', [entry])
bp = unreal.load_asset(ROOT + '/BP_PlayerAttackGrant')
if not bp:
    factory = unreal.BlueprintFactory()
    factory.set_editor_property('ParentClass', unreal.HodgeEquipmentDefinition)
    bp = tools.create_asset('BP_PlayerAttackGrant', ROOT, unreal.Blueprint, factory)
unreal.BlueprintEditorLibrary.compile_blueprint(bp)
unreal.get_default_object(bp.generated_class()).set_editor_property('AbilitySetsToGrant', [aset])
unreal.BlueprintEditorLibrary.compile_blueprint(bp)
for path in [ROOT + '/DA_PlayerAttack', ROOT + '/AS_PlayerAttack', ROOT + '/BP_PlayerAttackGrant']:
    assert unreal.EditorAssetLibrary.save_asset(path, only_if_is_dirty=False), path
print('Created isolated player attack grant')
