"""Author only the new Main Guardian v0.1 assets; source rigs and hero assets are read-only."""
import unreal
import json
from pathlib import Path

ROOT = '/Game/Main/Character/Enemies/Guardian'
SOURCE = '/Game/Assets/Enemies/Gruntling/Enemy_Gruntling_Guardian'
ANIM = SOURCE + '/Enemy_Gruntling_Guardian_Animation/'
tools = unreal.AssetToolsHelpers.get_asset_tools()
lib = unreal.HodgeAnimationAuthoringLibrary
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
dirty = unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
assert all(p.get_path_name().startswith(ROOT + '/') for p in dirty), [p.get_path_name() for p in dirty]
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
written = []


def tag(name):
    result = unreal.GameplayTag()
    result.import_text('(TagName="' + name + '")')
    return result


def asset(path, cls, factory):
    value = unreal.load_asset(path)
    if not value:
        parent, name = path.rsplit('/', 1)
        value = tools.create_asset(name, parent, cls, factory)
    assert value, path
    written.append(path)
    return value


def duplicate(path, source):
    value = unreal.load_asset(path)
    if not value:
        value = unreal.EditorAssetLibrary.duplicate_asset(source, path)
    assert value, source
    written.append(path)
    return value


def data(name, cls):
    factory = unreal.DataAssetFactory()
    factory.set_editor_property('DataAssetClass', cls)
    return asset(ROOT + '/' + name, cls, factory)


def blueprint(name, parent):
    factory = unreal.BlueprintFactory()
    factory.set_editor_property('ParentClass', parent)
    return asset(ROOT + '/' + name, unreal.Blueprint, factory)


skeleton = duplicate(ROOT + '/Model/SK_Guardian', SOURCE + '/SK_Gruntling_Guardian_Skeleton')
mesh = duplicate(ROOT + '/Model/SKM_Guardian', SOURCE + '/SK_Gruntling_Guardian')
assert lib.set_enemy_mesh_skeleton(mesh, skeleton)
assert lib.set_skeleton_slot_group(skeleton, 'FullBobySlot', 'HodgeEnemyFullBody')
assert lib.set_skeleton_slot_group(skeleton, 'UpperBobySlot', 'HodgeEnemyLightFeedback')
bones = [str(b) for b in lib.get_skeleton_bone_names(skeleton)]
spine = next((b for b in bones if 'spine' in b.lower()), None)
assert spine, bones


def sequence(name, source):
    value = duplicate(ROOT + '/Anim/A_' + name, ANIM + source)
    assert lib.configure_enemy_animation_asset(value, skeleton)
    value.set_editor_property('enable_root_motion', False)
    value.set_editor_property('bForceRootLock', True)
    value.set_editor_property('additive_anim_type', unreal.AdditiveAnimationType.AAT_NONE)
    return value


def montage(name, source, light=False):
    seq = sequence(name, source)
    factory = unreal.AnimMontageFactory()
    factory.set_editor_property('TargetSkeleton', skeleton)
    factory.set_editor_property('SourceAnimation', seq)
    value = asset(ROOT + '/Anim/AM_' + name, unreal.AnimMontage, factory)
    assert lib.configure_enemy_animation_asset(value, skeleton, 'UpperBobySlot' if light else 'FullBobySlot')
    for prop, duration in [('BlendIn', .10), ('BlendOut', .15)]:
        blend = value.get_editor_property(prop)
        blend.import_text('(BlendTime=' + str(duration) + ')')
        value.set_editor_property(prop, blend)
    return value


idle = sequence('Idle', 'Idle_Grunt')
run = sequence('Run', 'ExoGame_Gruntling_Run')
attack = montage('Attack', 'GuardiantMeleeAttack_1_Adjusted')
stun = montage('HitStun', 'ExoGame_Gruntling_React_Heavy_Front')
knock = montage('Knockback', 'ExoGame_Gruntling_React_Knockback_Front')
hold = montage('Hold', 'ExoGame_Gruntling_React_Stunned_Idle')
getup = montage('GetUp', 'ExoGame_Gruntling_React_Stunned_Exit')
light = montage('LightFeedback', 'ExoGame_Gruntling_React_Light_Front', True)
death = montage('Death', 'ExoGame_Gruntling_Death')
anim_factory = unreal.AnimBlueprintFactory()
anim_factory.set_editor_property('ParentClass', unreal.HodgeEnemyAnimInstance)
anim_factory.set_editor_property('TargetSkeleton', skeleton)
anim = asset(ROOT + '/Anim/ABP_Guardian', unreal.AnimBlueprint, anim_factory)
assert lib.configure_enemy_animation(anim, idle, run, spine)
compile_result = lib.compile_animation_blueprint(anim)
assert compile_result.startswith('ERRORS=0'), compile_result
unreal.get_default_object(anim.generated_class()).set_editor_property('root_motion_mode', unreal.RootMotionMode.ROOT_MOTION_FROM_MONTAGES_ONLY)

profile = data('DA_Guardian_HitReaction', unreal.HodgeHitReactionProfile)
entries = []
for impact_type, motion in [(unreal.HodgeImpactType.HIT_STUN, stun), (unreal.HodgeImpactType.KNOCKBACK, knock),
                            (unreal.HodgeImpactType.KNOCKDOWN, hold)]:
    entry = unreal.HodgeHitReactionAnimation()
    entry.set_editor_property('Type', impact_type)
    entry.set_editor_property('Montage', motion)
    entry.set_editor_property('DefaultDuration', .45)
    entry.set_editor_property('GetUpMontage', getup)
    entries.append(entry)
profile.set_editor_property('Animations', entries)
profile.set_editor_property('LightFeedbackMontage', light)
container = unreal.GameplayTagContainer()
container.import_text('(GameplayTags=((TagName="Ability.Type.Action.Melee")))')
profile.set_editor_property('InterruptibleAbilityTags', container)

definition = data('DA_Guardian_Attack', unreal.HodgeAbilityDefinition)
definition.set_editor_property('AbilityTag', tag('Ability.Type.Action.Melee'))
definition.set_editor_property('AbilityClass', unreal.HodgeGameplayAbility_Melee)
definition.set_editor_property('ExecutionBodyTag', tag('State.Combat.Body.Normal'))
definition.set_editor_property('ExecutionRoute', unreal.HodgeAbilityExecutionRoute.STANDALONE)
execution = definition.get_editor_property('ExecutionConfig')
execution.set_editor_property('Montage', attack)
execution.set_editor_property('PlayRate', 1.0)
definition.set_editor_property('ExecutionConfig', execution)
hit = definition.get_editor_property('DefaultHitConfig')
hit.set_editor_property('DamageEffect', unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/GE_MeleeDamage_Instant').generated_class())
hit.set_editor_property('DamageMultiplier', 1.0)
reaction = unreal.HodgeHitReactionConfig()
reaction.set_editor_property('AttackJudgementTag', tag('Combat.Attack.Judgement.Normal'))
impact = unreal.HodgeImpactSpec()
impact.set_editor_property('Type', unreal.HodgeImpactType.HIT_STUN)
impact.set_editor_property('ControlDuration', .35)
reaction.set_editor_property('Impacts', [impact])
hit.set_editor_property('Reaction', reaction)
definition.set_editor_property('DefaultHitConfig', hit)
notify = unreal.HodgeAnimHitConfig()
notify.set_editor_property('Source', unreal.HodgeAnimHitSource.AVATAR_ROOT)
notify.set_editor_property('Shape', unreal.HodgeHitShape.BOX)
notify.set_editor_property('BoxHalfExtent', unreal.Vector(95, 80, 80))
notify.set_editor_property('LocalTransform', unreal.Transform(location=unreal.Vector(85, 0, 0)))
for track in unreal.AnimationLibrary.get_animation_notify_track_names(attack):
    unreal.AnimationLibrary.remove_animation_notify_events_by_track(attack, track)
assert unreal.HodgeCombatValidationLibrary.add_hit_notify(attack, .35, .35, notify, True)

death_bp = blueprint('GA_Guardian_Death', unreal.HodgeGameplayAbility_EnemyDeath)
unreal.BlueprintEditorLibrary.compile_blueprint(death_bp)
unreal.get_default_object(death_bp.generated_class()).set_editor_property('DeathMontage', death)
unreal.BlueprintEditorLibrary.compile_blueprint(death_bp)


def ability_entry(cls):
    entry = unreal.HodgeAbilitySet_GameplayAbility()
    class_path = unreal.get_default_object(cls).get_class().get_path_name()
    entry.import_text('(Ability="/Script/CoreUObject.Class\'' + class_path + '\'",AbilityLevel=1)')
    return entry


action_entry = unreal.HodgeAbilitySet_Definition()
action_entry.import_text('(Definition="/Script/Hodgepodge.HodgeAbilityDefinition\'' + definition.get_path_name() + '\'",AbilityLevel=1)')
basic = data('AS_Guardian_Basic', unreal.HodgeAbilitySet)
basic.set_editor_property('GrantedGameplayAbilities', [ability_entry(unreal.HodgeGameplayAbility_HitReaction), ability_entry(death_bp.generated_class())])
basic.set_editor_property('GrantedAbilityDefinitions', [action_entry])
stats = data('DA_Guardian_Stats', unreal.HodgeCharacterStatProfile)
for prop, value in [('MaxHealth', 350.0), ('BaseDamage', 15.0)]:
    scalable = unreal.ScalableFloat()
    scalable.import_text('(Value=' + str(value) + ')')
    stats.set_editor_property(prop, scalable)

bb = asset(ROOT + '/AI/BB_Guardian', unreal.BlackboardData, unreal.BlackboardDataFactory())
bt = asset(ROOT + '/AI/BT_Guardian', unreal.BehaviorTree, unreal.BehaviorTreeFactory())
assert lib.configure_enemy_behavior_tree(bt, bb, tag('Ability.Type.Action.Melee'))
controller = blueprint('AIC_Guardian', unreal.HodgeEnemyAIController)
unreal.BlueprintEditorLibrary.compile_blueprint(controller)
unreal.get_default_object(controller.generated_class()).set_editor_property('BehaviorTree', bt)
unreal.BlueprintEditorLibrary.compile_blueprint(controller)
pawn_data = data('DA_Guardian_PawnData', unreal.HodgePawnData)
pawn_data.set_editor_property('StatProfile', stats)
pawn_data.set_editor_property('HitReactionProfile', profile)
pawn_data.set_editor_property('AbilitySets', [basic])
enemy = blueprint('BP_Enemy_Guardian', unreal.HodgeEnemyCharacter)
unreal.BlueprintEditorLibrary.compile_blueprint(enemy)
cdo = unreal.get_default_object(enemy.generated_class())
cdo.set_editor_property('EnemyPawnData', pawn_data)
cdo.set_editor_property('AIControllerClass', controller.generated_class())
cdo.mesh.set_skeletal_mesh_asset(mesh)
cdo.mesh.set_anim_instance_class(anim.generated_class())
cdo.mesh.set_relative_location(unreal.Vector(0, 0, -90), False, False)
cdo.mesh.set_relative_rotation(unreal.Rotator(0, -90, 0), False, False)
cdo.mesh.set_editor_property('visibility_based_anim_tick_option', unreal.VisibilityBasedAnimTickOption.ALWAYS_TICK_POSE_AND_REFRESH_BONES)
cdo.character_movement.set_editor_property('MaxWalkSpeed', 280.0)
pawn_data.set_editor_property('PawnClass', enemy.generated_class())
unreal.BlueprintEditorLibrary.compile_blueprint(enemy)

# Equipped variant owns only lifecycle abilities; the weapon owns its attack grant.
life = data('AS_Guardian_Lifecycle', unreal.HodgeAbilitySet)
life.set_editor_property('GrantedGameplayAbilities', [ability_entry(unreal.HodgeGameplayAbility_HitReaction), ability_entry(death_bp.generated_class())])
weapon_set = data('AS_Guardian_Weapon', unreal.HodgeAbilitySet)
weapon_set.set_editor_property('GrantedAbilityDefinitions', [action_entry])
weapon_actor = blueprint('BP_Guardian_WeaponActor', unreal.StaticMeshActor)
unreal.BlueprintEditorLibrary.compile_blueprint(weapon_actor)
weapon_cdo = unreal.get_default_object(weapon_actor.generated_class())
weapon_cdo.static_mesh_component.set_static_mesh(unreal.load_asset('/Game/Assets/Enemies/Gruntling/Weapons/Enemy_Gruntling_Weapons/Meshes/SM_Gruntling_Weapon01_Internal'))
weapon_cdo.static_mesh_component.set_mobility(unreal.ComponentMobility.MOVABLE)
weapon_cdo.static_mesh_component.set_collision_enabled(unreal.CollisionEnabled.NO_COLLISION)
weapon = blueprint('BP_Guardian_Equipment', unreal.HodgeEquipmentDefinition)
unreal.BlueprintEditorLibrary.compile_blueprint(weapon)
weapon_def = unreal.get_default_object(weapon.generated_class())
weapon_def.set_editor_property('InstanceType', unreal.HodgeWeaponInstance)
weapon_def.set_editor_property('AbilitySetsToGrant', [weapon_set])
spawn = unreal.HodgeEquipmentActorToSpawn()
spawn.set_editor_property('ActorToSpawn', weapon_actor.generated_class())
spawn.set_editor_property('AttachSocket', next((b for b in bones if 'hand' in b.lower() and (b.lower().endswith('_r') or 'rhand' in b.lower())), spine))
spawn.set_editor_property('AttachTransform', unreal.Transform())
weapon_def.set_editor_property('ActorsToSpawn', [spawn])
equipped_data = data('DA_Guardian_EquippedPawnData', unreal.HodgePawnData)
equipped_data.set_editor_property('StatProfile', stats)
equipped_data.set_editor_property('HitReactionProfile', profile)
equipped_data.set_editor_property('AbilitySets', [life])
equipped_data.set_editor_property('DefaultWeaponDefinition', weapon.generated_class())
equipped = blueprint('BP_Enemy_GuardianEquipped', unreal.HodgeEnemyCharacter)
unreal.BlueprintEditorLibrary.compile_blueprint(equipped)
equipped_cdo = unreal.get_default_object(equipped.generated_class())
equipped_cdo.set_editor_property('EnemyPawnData', equipped_data)
equipped_cdo.set_editor_property('AIControllerClass', controller.generated_class())
equipped_cdo.mesh.set_skeletal_mesh_asset(mesh)
equipped_cdo.mesh.set_anim_instance_class(anim.generated_class())
equipped_cdo.mesh.set_relative_location(unreal.Vector(0, 0, -90), False, False)
equipped_cdo.mesh.set_relative_rotation(unreal.Rotator(0, -90, 0), False, False)
equipped_cdo.mesh.set_editor_property('visibility_based_anim_tick_option', unreal.VisibilityBasedAnimTickOption.ALWAYS_TICK_POSE_AND_REFRESH_BONES)
equipped_cdo.character_movement.set_editor_property('MaxWalkSpeed', 280.0)
equipped_data.set_editor_property('PawnClass', equipped.generated_class())
unreal.BlueprintEditorLibrary.compile_blueprint(equipped)
experience = unreal.load_asset('/Game/Main/Experiences/Exp_HodgeDefaultExperience')
assert lib.configure_enemy_experience_components(experience)
unreal.BlueprintEditorLibrary.compile_blueprint(experience)
assert unreal.EditorAssetLibrary.save_asset('/Game/Main/Experiences/Exp_HodgeDefaultExperience', only_if_is_dirty=False)

for path in sorted(set(written)):
    assert unreal.EditorAssetLibrary.save_asset(path, only_if_is_dirty=False), path
assert unreal.get_default_object(unreal.load_asset(ROOT + '/BP_Enemy_Guardian').generated_class()).get_editor_property('AIControllerClass') == controller.generated_class()
summary = {'root': ROOT, 'assets': sorted(set(written)), 'spine_bone': spine, 'animation_compile': compile_result,
           'controller_class': controller.generated_class().get_path_name(), 'behavior_tree': bt.get_path_name(), 'source_assets_modified': False}
Path('E:/Project/Git/Hodgepodge/Saved/EnemyAIImplementation/assets.json').write_text(json.dumps(summary, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps(summary, ensure_ascii=False))
