"""创建隔离的高血量假人、七类测试攻击和测试地图，不改正式资源。"""
import json
from pathlib import Path

import unreal

ROOT = '/Game/CodexText/HitReactionValidation/Dummy'
MAP = '/Game/Tests/Combat/L_HitReactionDummy'
WORK = Path(unreal.Paths.project_saved_dir()) / 'HitReactionDummy'
WORK.mkdir(parents=True, exist_ok=True)
tools = unreal.AssetToolsHelpers.get_asset_tools()
created = []


def require(path):
    """缺少源资源时停止，不能生成貌似完整的占位资产。"""
    asset = unreal.load_asset(path)
    assert asset, '缺少源资源：' + path
    return asset


def fresh(name):
    """已有目标必须先人工检查，重复运行不覆盖用户对假人的调整。"""
    path = ROOT + '/' + name
    assert not unreal.EditorAssetLibrary.does_asset_exist(path), '目标已存在，不覆盖：' + path
    return path


def duplicate(name, source):
    """只修改新副本；源对象保留原样。"""
    fresh(name)
    result = tools.duplicate_asset(name, ROOT, source)
    assert result, name
    created.append(result)
    return result


def data(name, cls):
    """新配置使用现有反射类型，不引入测试专属 Runtime 类型。"""
    fresh(name)
    factory = unreal.DataAssetFactory()
    factory.set_editor_property('DataAssetClass', cls)
    result = tools.create_asset(name, ROOT, cls, factory)
    assert result, name
    created.append(result)
    return result


def blueprint(name, parent):
    """原生父类蓝图只作为装备配置容器。"""
    fresh(name)
    factory = unreal.BlueprintFactory()
    factory.set_editor_property('ParentClass', parent)
    result = tools.create_asset(name, ROOT, unreal.Blueprint, factory)
    assert result, name
    unreal.BlueprintEditorLibrary.compile_blueprint(result)
    created.append(result)
    return result


assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
assert not unreal.EditorAssetLibrary.does_asset_exist(MAP), '不覆盖已有测试地图'
source_stats = require('/Game/Main/Data/CharacterStats/DA_Pover_Stats')
source_profile = require('/Game/Main/Character/Hero/Anim/HitReactions/DA_Hero_HitReaction')
source_attack = require('/Game/CodexText/HitReactionValidation/DA_ProbeAttack')
source_montage = require('/Game/CodexText/HitReactionValidation/AM_ProbeAttack')
assert any(e.get_editor_property('Notify') and isinstance(e.get_editor_property('Notify'), unreal.HodgeAnimNotify_Hit)
           for e in unreal.AnimationLibrary.get_animation_notify_events(source_montage))

# 独立属性配置通过 EnemyInit 设置 MaxHealth 后再填充 Health；不使用无敌。
stats = duplicate('DA_DummyStats', source_stats)
health = unreal.ScalableFloat()
health.set_editor_property('Value', 1000000.0)
stats.set_editor_property('MaxHealth', health)
stats.set_editor_property('MinLevel', 1)
stats.set_editor_property('MaxLevel', 1)
profile = duplicate('DA_DummyHitReaction', source_profile)
ability_set = data('AS_Dummy', unreal.HodgeAbilitySet)
reaction = unreal.HodgeAbilitySet_GameplayAbility()
reaction.import_text('(Ability="/Script/CoreUObject.Class\'/Script/Hodgepodge.HodgeGameplayAbility_HitReaction\'",AbilityLevel=1)')
ability_set.set_editor_property('GrantedGameplayAbilities', [reaction])
pawn_data = data('DA_DummyPawnData', unreal.HodgePawnData)
# 直接建原生子类，避免复制旧蓝图时把非组件属性集引用置空。
dummy = blueprint('BP_HitReactionDummy', unreal.HodgeEnemyCharacter)
subsystem = unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
handles = subsystem.k2_gather_subobject_data_for_blueprint(dummy)
params = unreal.AddNewSubobjectParams(parent_handle=handles[0], new_class=unreal.HodgeCombatComponentBase, blueprint_context=dummy)
component, reason = subsystem.add_new_subobject(params)
assert not str(reason), str(reason)
unreal.BlueprintEditorLibrary.compile_blueprint(dummy)
cdo = unreal.get_default_object(dummy.generated_class())
cdo.mesh.set_skeletal_mesh_asset(require('/Game/Main/Character/Hero/Anim/Model/SKM_Pover_LyraLab'))
cdo.mesh.set_anim_instance_class(require('/Game/Main/Character/Hero/Anim/ABP_Pover_Base').generated_class())
cdo.mesh.set_relative_location(unreal.Vector(0, 0, -90), False, False)
cdo.mesh.set_relative_rotation(unreal.Rotator(yaw=-90), False, False)
cdo.set_editor_property('EnemyPawnData', pawn_data)
cdo.set_editor_property('InitialLevel', 1)
# 保留控制器以满足怪物初始化；普通 AIController 不启动追击或行为树。
cdo.set_editor_property('AIControllerClass', unreal.AIController.static_class())
cdo.set_editor_property('AutoPossessAI', unreal.AutoPossessAI.PLACED_IN_WORLD_OR_SPAWNED)
cdo.set_editor_property('bAllowPlayerLockOn', True)
pawn_data.set_editor_property('PawnClass', dummy.generated_class())
pawn_data.set_editor_property('StatProfile', stats)
pawn_data.set_editor_property('HitReactionProfile', profile)
pawn_data.set_editor_property('AbilitySets', [ability_set])
unreal.BlueprintEditorLibrary.compile_blueprint(dummy)

# 七类攻击只供主动测试，单次只装备一类，不改变正式五段攻击。
montage = duplicate('AM_DummyProbe', source_montage)
cases = ['Light', 'HitStun', 'Knockback', 'Launch', 'AirHit', 'Slam', 'Knockdown']
types = [None, unreal.HodgeImpactType.HIT_STUN, unreal.HodgeImpactType.KNOCKBACK,
         unreal.HodgeImpactType.LAUNCH, unreal.HodgeImpactType.AIR_HIT,
         unreal.HodgeImpactType.SLAM, unreal.HodgeImpactType.KNOCKDOWN]
for name, kind in zip(cases, types):
    definition = duplicate('DA_Probe_' + name, source_attack)
    execution = definition.get_editor_property('ExecutionConfig')
    execution.set_editor_property('Montage', montage)
    definition.set_editor_property('ExecutionConfig', execution)
    config = definition.get_editor_property('DefaultHitConfig')
    request = config.get_editor_property('Reaction')
    impact = unreal.HodgeImpactSpec()
    if kind is not None:
        impact.set_editor_property('Type', kind)
    impact.set_editor_property('ControlDuration', 0.0)
    impact.set_editor_property('KnockbackDistance', 150.0)
    impact.set_editor_property('MoveDuration', 0.3)
    impact.set_editor_property('HorizontalSpeed', 0.0)
    impact.set_editor_property('VerticalSpeed', 350.0 if name == 'AirHit' else 650.0)
    request.set_editor_property('Impacts', [impact] if kind is not None else [])
    config.set_editor_property('Reaction', request)
    definition.set_editor_property('DefaultHitConfig', config)
    grant_set = data('AS_Probe_' + name, unreal.HodgeAbilitySet)
    entry = unreal.HodgeAbilitySet_Definition()
    entry.import_text('(Definition="/Script/Hodgepodge.HodgeAbilityDefinition\'' + definition.get_path_name() + '\'",AbilityLevel=1)')
    grant_set.set_editor_property('GrantedAbilityDefinitions', [entry])
    grant = blueprint('BP_ProbeGrant_' + name, unreal.HodgeEquipmentDefinition)
    unreal.get_default_object(grant.generated_class()).set_editor_property('AbilitySetsToGrant', [grant_set])
    unreal.BlueprintEditorLibrary.compile_blueprint(grant)

# 明确保存本轮新资源，禁止 SaveAll 或重存正式蓝图。
for asset in created:
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=False), asset.get_path_name()

levels = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
assert levels.new_level(MAP)
actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)


def spawn(cls, label, location, rotation=unreal.Rotator()):
    """测试场景只生成地面、光源、出生点和一个假人。"""
    actor = actors.spawn_actor_from_class(cls, location, rotation)
    assert actor, label
    actor.set_actor_label(label)
    return actor


floor = spawn(unreal.StaticMeshActor, 'DummyTestFloor', unreal.Vector(0, 0, -50))
floor.static_mesh_component.set_static_mesh(require('/Engine/BasicShapes/Cube'))
floor.set_actor_scale3d(unreal.Vector(40, 40, 1))
floor.static_mesh_component.set_collision_enabled(unreal.CollisionEnabled.QUERY_AND_PHYSICS)
spawn(unreal.PlayerStart, 'DummyPlayerStart', unreal.Vector(0, 0, 100))
spawn(unreal.DirectionalLight, 'DummySun', unreal.Vector(0, 0, 800), unreal.Rotator(pitch=-45, yaw=-30))
spawn(unreal.SkyLight, 'DummySky', unreal.Vector(0, 0, 800))
target = spawn(dummy.generated_class(), 'HitReactionDummy_1000000HP', unreal.Vector(250, 0, 95), unreal.Rotator(yaw=180))
# 预放置实例显式绑定同一配置，避免蓝图重建保留旧实例覆盖值。
target.set_editor_property('EnemyPawnData', pawn_data)
target.set_editor_property('AIControllerClass', unreal.AIController.static_class())
world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
world.get_world_settings().set_editor_property('DefaultGameMode', require('/Game/ThirdPerson/Blueprints/BP_ThirdPersonGameMode').generated_class())
assert unreal.EditorLoadingAndSavingUtils.save_map(world, MAP)
result = {'map': MAP, 'dummy': dummy.get_path_name(), 'max_health': 1000000,
          'controller': '/Script/AIModule.AIController', 'profile': profile.get_path_name(),
          'probe_cases': cases, 'created': [a.get_path_name() for a in created]}
(WORK / 'created.json').write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
print('HODGE_DUMMY_CREATED ' + json.dumps(result, ensure_ascii=False))
