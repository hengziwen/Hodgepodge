"""Configure the formal hero's target presentation and grant; preserve all attack Impact settings."""
import unreal, json
from pathlib import Path

ROOT = '/Game/Main/Character/Hero/Anim/HitReactions'
lib = unreal.HodgeAnimationAuthoringLibrary
tools = unreal.AssetToolsHelpers.get_asset_tools()
out = Path(unreal.Paths.project_saved_dir()) / 'HeroHitReactionSetup'
out.mkdir(parents=True, exist_ok=True)
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
anim = unreal.load_asset('/Game/Main/Character/Hero/Anim/ABP_Pover_Base')
skeleton = unreal.load_asset('/Game/Main/Character/Hero/Anim/Model/SK_Pover_LyraLab')

def load_or_data(name, cls):
    path = ROOT + '/' + name
    result = unreal.load_asset(path)
    if not result:
        factory = unreal.DataAssetFactory(); factory.set_editor_property('DataAssetClass', cls)
        result = tools.create_asset(name, ROOT, cls, factory)
    assert result, path
    return result

def montage(name, source, additive=False):
    seq_path = ROOT + '/A_Hero_' + name
    seq = unreal.load_asset(seq_path)
    if not seq:
        seq = unreal.EditorAssetLibrary.duplicate_asset('/Game/Wuwa/Anim/BaseAnim/' + source, seq_path)
        assert seq, source
    assert lib.set_hero_reaction_skeleton(seq, skeleton)
    seq.set_editor_property('enable_root_motion', False)
    seq.set_editor_property('bForceRootLock', True)
    seq.set_editor_property('additive_anim_type', unreal.AdditiveAnimationType.AAT_LOCAL_SPACE_BASE if additive else unreal.AdditiveAnimationType.AAT_NONE)
    if additive:
        seq.set_editor_property('ref_pose_type', unreal.AdditiveBasePoseType.ABPT_LOCAL_ANIM_FRAME)
        seq.set_editor_property('ref_frame_index', 0)
    path = ROOT + '/AM_Hero_' + name
    result = unreal.load_asset(path)
    if not result:
        factory = unreal.AnimMontageFactory()
        factory.set_editor_property('TargetSkeleton', skeleton)
        factory.set_editor_property('SourceAnimation', seq)
        result = tools.create_asset('AM_Hero_' + name, ROOT, unreal.AnimMontage, factory)
    assert result and lib.set_hero_reaction_montage_slot(result, 'AdditiveHitReact' if additive else 'FullBody')
    for prop, duration in [('BlendIn', .08), ('BlendOut', .12)]:
        blend = result.get_editor_property(prop)
        blend.import_text('(BlendTime=' + str(duration) + ')')
        result.set_editor_property(prop, blend)
    return result

assert lib.configure_hero_hit_reaction_graph(anim)
compile_result = lib.compile_animation_blueprint(anim)
assert compile_result.startswith('ERRORS=0'), compile_result
clips = {name: montage(name, source) for name, source in [
    ('HitStun', 'Behit_B_R'), ('Knockback', 'Behit_Push_Start'), ('Launch', 'Behit_Fly_Start'),
    ('AirLoop', 'Behit_Fly_Loop'), ('Landing', 'Behit_Fly_Fall'), ('GetUp', 'StandUp'), ('AirHit', 'Behit_Hover')]}
light = montage('LightFeedback', 'Behit_S_R', True)
profile = load_or_data('DA_Hero_HitReaction', unreal.HodgeHitReactionProfile)
entries = []
for typ, name in [(unreal.HodgeImpactType.HIT_STUN, 'HitStun'), (unreal.HodgeImpactType.KNOCKBACK, 'Knockback'),
                  (unreal.HodgeImpactType.LAUNCH, 'Launch'), (unreal.HodgeImpactType.KNOCKDOWN, 'Landing'),
                  (unreal.HodgeImpactType.AIR_HIT, 'AirHit'), (unreal.HodgeImpactType.SLAM, 'Launch')]:
    entry = unreal.HodgeHitReactionAnimation()
    for prop, value in [('Type', typ), ('Montage', clips[name]), ('DefaultDuration', .4),
                        ('AirLoopMontage', clips['AirLoop']), ('LandingMontage', clips['Landing']),
                        ('GetUpMontage', clips['GetUp']), ('bAllowWithoutMontage', False)]:
        entry.set_editor_property(prop, value)
    entries.append(entry)
profile.set_editor_property('Animations', entries)
profile.set_editor_property('LightFeedbackMontage', light)
profile.set_editor_property('DownedDuration', 1.0)
profile.set_editor_property('MaxControlDuration', 6.0)
profile.set_editor_property('MaxAirborneDuration', 4.0)
profile.set_editor_property('MaxLaunchHeight', 600.0)
profile.set_editor_property('MaxAirHits', 6)
profile.set_editor_property('bAutoGetUp', True)

ga_path = '/Game/Main/Character/Hero/Ability/GA_Hero_HitReaction'
bp = unreal.load_asset(ga_path)
if not bp:
    factory = unreal.BlueprintFactory(); factory.set_editor_property('ParentClass', unreal.HodgeGameplayAbility_HitReaction)
    bp = tools.create_asset('GA_Hero_HitReaction', '/Game/Main/Character/Hero/Ability', unreal.Blueprint, factory)
assert bp
unreal.BlueprintEditorLibrary.compile_blueprint(bp)
aset = unreal.load_asset('/Game/Main/Data/AbilitySet/DA_Pover')
abilities = list(aset.get_editor_property('GrantedGameplayAbilities'))
native_entries = [e for e in abilities if e.get_editor_property('Ability') and
                  unreal.MathLibrary.class_is_child_of(e.get_editor_property('Ability'), unreal.HodgeGameplayAbility_HitReaction)]
assert len(native_entries) <= 1, 'Multiple existing reaction grants require resolving before writing'
if native_entries:
    index = abilities.index(native_entries[0])
else:
    index = len(abilities)
entry = unreal.HodgeAbilitySet_GameplayAbility()
entry.import_text('(Ability="/Script/Engine.BlueprintGeneratedClass\'' + bp.generated_class().get_path_name() + '\'",AbilityLevel=1,InputTag=(TagName=""))')
if index == len(abilities): abilities.append(entry)
else: abilities[index] = entry
aset.set_editor_property('GrantedGameplayAbilities', abilities)
pawn_data = unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData')
assert aset in list(pawn_data.get_editor_property('AbilitySets'))
pawn_data.set_editor_property('HitReactionProfile', profile)

normal = unreal.GameplayTag(); normal.import_text('(TagName="State.Combat.Body.Normal")')
attacks = []
for i in range(1, 6):
    definition = unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_' + str(i))
    before = definition.get_editor_property('DefaultHitConfig').export_text()
    definition.set_editor_property('ExecutionBodyTag', normal)
    assert before == definition.get_editor_property('DefaultHitConfig').export_text(), 'Hit/Impact configuration changed'
    attacks.append(definition)
for asset in [anim, skeleton, bp, aset, pawn_data] + attacks:
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=False), asset.get_path_name()
for path in unreal.EditorAssetLibrary.list_assets(ROOT, recursive=True, include_folder=False):
    assert unreal.EditorAssetLibrary.save_asset(path, only_if_is_dirty=False), path
report = {'compile': compile_result, 'anim': anim.get_path_name(), 'profile': profile.get_path_name(),
          'ability': bp.get_path_name(), 'ability_set': aset.get_path_name(), 'pawn_data': pawn_data.get_path_name(),
          'full_body_group': str(lib.get_skeleton_slot_group(skeleton, 'FullBody')),
          'light_group': str(lib.get_skeleton_slot_group(skeleton, 'AdditiveHitReact')),
          'attack_impacts_preserved': True}
assert report['full_body_group'] != report['light_group']
(out / 'configured.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps(report, ensure_ascii=False))
