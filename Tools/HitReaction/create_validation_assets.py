"""Create isolated animation-backed fixtures; never save the project's formal assets."""
import unreal, json
from pathlib import Path

ROOT = '/Game/CodexText/HitReactionValidation'
tools = unreal.AssetToolsHelpers.get_asset_tools()
lib = unreal.HodgeCombatValidationLibrary
created = []

def tag(name):
    value = unreal.GameplayTag()
    value.import_text('(TagName="' + name + '")')
    return value

def data(name, cls):
    path = ROOT + '/' + name
    old = unreal.load_asset(path)
    if old:
        return old
    factory = unreal.DataAssetFactory()
    factory.set_editor_property('DataAssetClass', cls)
    result = tools.create_asset(name, ROOT, cls, factory)
    assert result, path
    created.append(path)
    return result

def sequence(name, source):
    path = ROOT + '/A_' + name
    result = unreal.load_asset(path)
    if not result:
        result = unreal.EditorAssetLibrary.duplicate_asset(source, path)
        assert result, source
        created.append(path)
    result.set_editor_property('enable_root_motion', False)
    result.set_editor_property('bForceRootLock', True)
    return result

def montage(name, source):
    seq = sequence(name, source)
    path = ROOT + '/AM_' + name
    result = unreal.load_asset(path)
    if not result:
        factory = unreal.AnimMontageFactory()
        factory.set_editor_property('TargetSkeleton', seq.get_editor_property('Skeleton'))
        factory.set_editor_property('SourceAnimation', seq)
        result = tools.create_asset('AM_' + name, ROOT, unreal.AnimMontage, factory)
        assert result, path
        created.append(path)
    assert lib.configure_hit_reaction_validation_montage(result, 'FullBody')
    return result

def blueprint(name, parent):
    path = ROOT + '/' + name
    result = unreal.load_asset(path)
    if not result:
        factory = unreal.BlueprintFactory()
        factory.set_editor_property('ParentClass', parent)
        result = tools.create_asset(name, ROOT, unreal.Blueprint, factory)
        assert result, path
        created.append(path)
    return result

base = '/Game/Wuwa/Anim/BaseAnim/'
ms = {name: montage(name, base + source) for name, source in [
    ('HitStun', 'Behit_B_R'), ('Knockback', 'Behit_Push_Start'), ('Launch', 'Behit_Fly_Start'),
    ('AirLoop', 'Behit_Fly_Loop'), ('Landing', 'Behit_Fly_Fall'), ('GetUp', 'StandUp'),
    ('AirHit', 'Behit_Hover'), ('Hold', 'Behit_S_R')]}
# This is a real current-rig action clip, stripped only in the fixture copy.
attack = unreal.load_asset(ROOT + '/AM_ProbeAttack')
if not attack:
    attack = unreal.EditorAssetLibrary.duplicate_asset('/Game/Main/Character/Hero/Anim/Montages/AM_Attack01_Montage', ROOT + '/AM_ProbeAttack')
    assert attack
    created.append(ROOT + '/AM_ProbeAttack')
assert lib.configure_hit_reaction_validation_montage(attack, 'FullBody')

profile = data('DA_ReactionProfile', unreal.HodgeHitReactionProfile)
entries = []
for typ, name in [(unreal.HodgeImpactType.HIT_STUN, 'HitStun'), (unreal.HodgeImpactType.KNOCKBACK, 'Knockback'),
                  (unreal.HodgeImpactType.LAUNCH, 'Launch'), (unreal.HodgeImpactType.KNOCKDOWN, 'Landing'),
                  (unreal.HodgeImpactType.AIR_HIT, 'AirHit'), (unreal.HodgeImpactType.SLAM, 'Launch')]:
    entry = unreal.HodgeHitReactionAnimation()
    entry.set_editor_property('Type', typ)
    entry.set_editor_property('Montage', ms[name])
    entry.set_editor_property('DefaultDuration', 0.6)
    entry.set_editor_property('AirLoopMontage', ms['AirLoop'])
    entry.set_editor_property('LandingMontage', ms['Landing'])
    entry.set_editor_property('GetUpMontage', ms['GetUp'])
    entry.set_editor_property('bAllowWithoutMontage', False)
    entries.append(entry)
profile.set_editor_property('Animations', entries)
profile.set_editor_property('DownedDuration', 1.0)
profile.set_editor_property('MaxControlDuration', 8.0)
profile.set_editor_property('MaxAirborneDuration', 4.0)
profile.set_editor_property('MaxLaunchHeight', 650.0)

damage = unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/GE_MeleeDamage_Instant').generated_class()
definition = data('DA_ProbeAttack', unreal.HodgeAbilityDefinition)
definition.set_editor_property('AbilityTag', tag('Status.Attack'))
definition.set_editor_property('AbilityClass', unreal.HodgeGameplayAbility_Melee)
definition.set_editor_property('ExecutionBodyTag', tag('State.Combat.Body.Normal'))
definition.set_editor_property('ExecutionRoute', unreal.HodgeAbilityExecutionRoute.STANDALONE)
execution = definition.get_editor_property('ExecutionConfig')
execution.set_editor_property('Montage', attack)
execution.set_editor_property('PlayRate', 1.0)
definition.set_editor_property('ExecutionConfig', execution)
hit = definition.get_editor_property('DefaultHitConfig')
hit.set_editor_property('DamageEffect', damage)
hit.set_editor_property('DamageMultiplier', 0.1)
reaction = unreal.HodgeHitReactionConfig()
reaction.set_editor_property('AttackJudgementTag', tag('Combat.Attack.Judgement.Skill'))
impact = unreal.HodgeImpactSpec()
impact.set_editor_property('Type', unreal.HodgeImpactType.HIT_STUN)
impact.set_editor_property('ControlDuration', 0.8)
reaction.set_editor_property('Impacts', [impact])
hit.set_editor_property('Reaction', reaction)
definition.set_editor_property('DefaultHitConfig', hit)
notify = unreal.HodgeAnimHitConfig()
notify.set_editor_property('Source', unreal.HodgeAnimHitSource.AVATAR_ROOT)
notify.set_editor_property('Radius', 350.0)
notify.set_editor_property('bAllowFriendlyFire', True)
assert lib.add_hit_notify(attack, 0.3, 0.3, notify, True)

hold = data('DA_ProbeHold', unreal.HodgeAbilityDefinition)
hold.set_editor_property('AbilityTag', tag('Status.Rotation.Locked'))
hold.set_editor_property('AbilityClass', unreal.HodgeGameplayAbility_Definition)
hold.set_editor_property('ExecutionBodyTag', tag('State.Combat.Body.Skill'))
hold.set_editor_property('ExecutionRoute', unreal.HodgeAbilityExecutionRoute.STANDALONE)
ex = hold.get_editor_property('ExecutionConfig')
ex.set_editor_property('Montage', ms['Hold'])
ex.set_editor_property('PlayRate', 0.1)
hold.set_editor_property('ExecutionConfig', ex)

aset = data('AS_ReactionProbe', unreal.HodgeAbilitySet)
ga = unreal.HodgeAbilitySet_GameplayAbility()
ga.import_text('(Ability="/Script/CoreUObject.Class\'/Script/Hodgepodge.HodgeGameplayAbility_HitReaction\'",AbilityLevel=1)')
aset.set_editor_property('GrantedGameplayAbilities', [ga])
defs = []
for d in [definition, hold]:
    entry = unreal.HodgeAbilitySet_Definition()
    entry.import_text('(Definition="/Script/Hodgepodge.HodgeAbilityDefinition\'' + d.get_path_name() + '\'",AbilityLevel=1)')
    defs.append(entry)
aset.set_editor_property('GrantedAbilityDefinitions', defs)
grant = blueprint('BP_ReactionGrant', unreal.HodgeEquipmentDefinition)
unreal.BlueprintEditorLibrary.compile_blueprint(grant)
unreal.get_default_object(grant.generated_class()).set_editor_property('AbilitySetsToGrant', [aset])
unreal.BlueprintEditorLibrary.compile_blueprint(grant)

for path in unreal.EditorAssetLibrary.list_assets(ROOT, recursive=True, include_folder=False):
    assert unreal.EditorAssetLibrary.save_asset(path, only_if_is_dirty=False), path
summary = {'root': ROOT, 'created': created, 'profile': profile.get_path_name(), 'attack': definition.get_path_name(),
           'hold': hold.get_path_name(), 'grant': grant.get_path_name(), 'target_skeleton': ms['Launch'].get_editor_property('Skeleton').get_path_name()}
Path('E:/Project/Git/Hodgepodge/Saved/HitReactionPIE/assets.json').write_text(json.dumps(summary, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps(summary, ensure_ascii=False))
