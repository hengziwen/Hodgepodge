"""Create explicit Main movement resources and wire the existing locomotion graphs."""
import unreal, json
from pathlib import Path

assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
tools = unreal.AssetToolsHelpers.get_asset_tools()
lib = unreal.HodgeAnimationAuthoringLibrary
root = '/Game/Main/Character/Hero'
anim_root = root + '/Anim/Movement'
skeleton = unreal.load_asset(root + '/Anim/Model/SK_Pover_LyraLab')
assert skeleton
modified = []

def sequence(name, source):
    path = anim_root + '/' + name
    seq = unreal.load_asset(path)
    if not seq:
        seq = unreal.EditorAssetLibrary.duplicate_asset(source, path)
    assert seq and isinstance(seq, unreal.AnimSequence), source
    assert lib.set_hero_reaction_skeleton(seq, skeleton)
    seq.set_editor_property('enable_root_motion', False)
    seq.set_editor_property('bForceRootLock', True)
    modified.append(seq)
    return seq

def montage(name, seq):
    result = unreal.load_asset(anim_root + '/' + name)
    if not result:
        factory = unreal.AnimMontageFactory()
        factory.set_editor_property('TargetSkeleton', skeleton)
        factory.set_editor_property('SourceAnimation', seq)
        result = tools.create_asset(name, anim_root, unreal.AnimMontage, factory)
    assert result and lib.set_hero_reaction_montage_slot(result, 'FullBody')
    for prop, duration in [('BlendIn', .06), ('BlendOut', .12)]:
        value = result.get_editor_property(prop)
        value.import_text('(BlendTime=' + str(duration) + ')')
        result.set_editor_property(prop, value)
    modified.append(result)
    return result

forward = montage('AM_Hero_Dash_F', sequence('A_Hero_Dash_F', '/Game/Wuwa/Anim/AM_Move_F'))
backward = montage('AM_Hero_Dash_B', sequence('A_Hero_Dash_B', '/Game/Wuwa/Anim/AM_Move_B'))
cycle = sequence('A_Hero_Sprint_F', '/Game/Wuwa/Anim/BaseAnim/Sprint_F')
turn = sequence('A_Hero_Sprint_Turn', '/Game/Wuwa/Anim/BaseAnim/Run_Turnback')
profile_path = root + '/Ability/Sprint/DA_Hero_SprintAbility'
profile = unreal.load_asset(profile_path)
if not profile:
    factory = unreal.DataAssetFactory()
    factory.set_editor_property('DataAssetClass', unreal.HodgeSprintAbilityProfile)
    profile = tools.create_asset('DA_Hero_SprintAbility', root + '/Ability/Sprint', unreal.HodgeSprintAbilityProfile, factory)
assert profile
for prop, value in [('ForwardMontage', forward), ('BackwardMontage', backward), ('bEnableBackwardVariant', True),
                    ('BackwardConeAngle', 45.), ('SprintCycle', cycle), ('SprintTurn', turn)]:
    profile.set_editor_property(prop, value)
modified.append(profile)

def ability(name, parent):
    asset = unreal.load_asset(root + '/Ability/' + name)
    if not asset:
        factory = unreal.BlueprintFactory(); factory.set_editor_property('ParentClass', parent)
        asset = tools.create_asset(name, root + '/Ability', unreal.Blueprint, factory)
    assert asset
    default = unreal.get_default_object(asset.generated_class())
    default.set_editor_property('Profile', profile)
    modified.append(asset)
    return asset.generated_class()

dash_class = ability('GA_Hero_Dash', unreal.HodgeGameplayAbility_Dash)
sprint_class = ability('GA_Hero_Sprint', unreal.HodgeGameplayAbility_Sprint)
for name, tag_name, effect_path in [
    ('GCN_Hero_Dash', 'GameplayCue.Character.Dash', '/Game/Assets/Niagara/Refraction/NS_RefractionBurst'),
    ('GCN_Hero_Sprint', 'GameplayCue.Character.Sprint', '/Game/Assets/Niagara/Trails/NS_Hero_Trail'),
    ('GCN_Hero_PerfectDodge', 'GameplayCue.Character.PerfectDodge', '/Game/Assets/Niagara/Refraction/NS_RefractionBurst')]:
    folder = '/Game/Main/GameplayCues/Movement'
    asset = unreal.load_asset(folder + '/' + name)
    if not asset:
        factory = unreal.BlueprintFactory(); factory.set_editor_property('ParentClass', unreal.HodgeGameplayCue_Movement)
        asset = tools.create_asset(name, folder, unreal.Blueprint, factory)
    assert asset
    default = unreal.get_default_object(asset.generated_class())
    tag = unreal.GameplayTag(); tag.import_text('(TagName="' + tag_name + '")')
    default.set_editor_property('GameplayCueTag',tag)
    default.set_editor_property('Effect',unreal.load_asset(effect_path))
    unreal.BlueprintEditorLibrary.compile_blueprint(asset)
    modified.append(asset)
ability_set = unreal.load_asset('/Game/Main/Data/AbilitySet/DA_Pover')
assert ability_set
entries = list(ability_set.get_editor_property('GrantedGameplayAbilities'))
for cls in [dash_class, sprint_class]:
    if not any(entry.get_editor_property('Ability') == cls for entry in entries):
        entry = unreal.HodgeAbilitySet_GameplayAbility()
        entry.import_text('(Ability=BlueprintGeneratedClass\'' + cls.get_path_name() + '\',AbilityLevel=1)')
        entries.append(entry)
ability_set.set_editor_property('GrantedGameplayAbilities', entries); modified.append(ability_set)

pawn_data = unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData')
assert pawn_data
pawn_data.set_editor_property('SprintAbilityProfile', profile)
combo = pawn_data.get_editor_property('ComboDefinition')
if combo: profile.set_editor_property('AttackCancelWindow', combo.get_editor_property('MoveCancelWindowTag'))
modified.append(pawn_data)
input_config = pawn_data.get_editor_property('InputConfig')
action = unreal.load_asset('/Game/Main/Input/InputAction/IA_Sprint')
assert input_config and action
action.set_editor_property('triggers', [])
entries = list(input_config.get_editor_property('AbilityInputActions'))
if not any(entry.get_editor_property('InputAction') == action for entry in entries):
    entry = unreal.HodgeInputAction()
    entry.import_text('(InputAction=InputAction\'' + action.get_path_name() + '\',InputTag=(TagName="InputTag.Sprint"))')
    entries.append(entry)
input_config.set_editor_property('AbilityInputActions', entries); modified.extend([input_config, action])

main = unreal.load_asset(root + '/Anim/ABP_Pover_Base')
layer = unreal.load_asset(root + '/Anim/Layer/ABP_Pover_LocomotionBase')
result = lib.configure_hero_sprint_animations(main, layer)
assert result.startswith('OK'), result
diagnostics = {key: lib.compile_animation_blueprint(asset) for key, asset in [('main',main),('layer',layer)]}
assert all(value.startswith('ERRORS=0') for value in diagnostics.values()), diagnostics
modified.extend([main,layer])
for asset in modified:
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset, False), asset.get_path_name()
out = Path(unreal.Paths.project_saved_dir()) / 'DashSprintImplementation'
out.joinpath('hero-configuration.json').write_text(json.dumps({'result':result,'diagnostics':diagnostics,'assets':[asset.get_path_name() for asset in modified]}, indent=2),encoding='utf-8')
print(json.dumps({'result':result,'diagnostics':diagnostics,'asset_count':len(modified)}))
