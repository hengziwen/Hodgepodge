"""创建独立双模式示例，不修改 Guardian、原验证关卡或用户已有同名配置。"""
from pathlib import Path
import hashlib
import json
import unreal

ROOT = Path(unreal.Paths.project_dir()).resolve()
OUT = ROOT / 'Saved/Encounter_20261025'
MAP = '/Game/Main/Maps/L_EncounterValidation'
DATA_ROOT = '/Game/Main/Data/Encounters'
assert ROOT == Path('E:/Project/Git/Hodgepodge')
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
tools = unreal.AssetToolsHelpers.get_asset_tools()
levels = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
source = unreal.load_asset('/Game/Main/Character/Enemies/Guardian/DA_Guardian_EquippedPawnData')
reward = unreal.load_asset('/Game/Main/Data/Inventory/ID_TestMaterial')
assert source and reward and source.get_editor_property('PawnClass')
assert hasattr(unreal, 'HodgeEncounterSpawner') and hasattr(unreal, 'HodgeEncounterDefinition'), '先编译并重启编辑器'
source_file = ROOT / 'Content/Main/Character/Enemies/Guardian/DA_Guardian_EquippedPawnData.uasset'
source_hash = hashlib.sha256(source_file.read_bytes()).hexdigest()
report = {'created_assets': [], 'definitions': [], 'map': MAP, 'source_sha256': source_hash}


def entry(location):
    """每个成员有明确相对出生点，不随机叠在同一坐标。"""
    result = unreal.HodgeEncounterSpawn()
    result.set_editor_property('PawnData', source)
    result.set_editor_property('Level', 1)
    result.set_editor_property('RelativeTransform', unreal.Transform(location=unreal.Vector(*location)))
    return result


def wave(locations):
    """零额外等待沿用批准的最小流程，仍逐波真实清场。"""
    result = unreal.HodgeEncounterWave()
    result.set_editor_property('DelayBeforeSpawn', 0.0)
    result.set_editor_property('Enemies', [entry(x) for x in locations])
    return result


def definition(name, mode):
    """新资产写入示例；已有资产只校验，保留后续制作调整。"""
    path = DATA_ROOT + '/' + name
    value = unreal.load_asset(path)
    created = value is None
    if created:
        factory = unreal.DataAssetFactory()
        factory.set_editor_property('data_asset_class', unreal.HodgeEncounterDefinition)
        value = tools.create_asset(name, DATA_ROOT, unreal.HodgeEncounterDefinition, factory)
        assert value
        value.set_editor_property('Mode', mode)
        value.set_editor_property('Waves', [wave([(350, 0, 0)]), wave([(350, 150, 0), (350, -150, 0)])])
        value.set_editor_property('EnemyInitializationTimeout', 10.0)
        value.set_editor_property('AreaHalfExtent', unreal.Vector(300, 300, 150))
        value.set_editor_property('TriggerEdge', unreal.HodgeEncounterTriggerEdge.ENTER)
        value.set_editor_property('bRepeatable', True)
        value.set_editor_property('CooldownSeconds', 30.0)
        if mode == unreal.HodgeEncounterMode.INTERACTION:
            value.set_editor_property('RewardDefinition', reward.generated_class())
            value.set_editor_property('RewardCount', 1)
            value.set_editor_property('RewardLevel', 1)
        report['created_assets'].append(path)
    assert isinstance(value, unreal.HodgeEncounterDefinition) and value.get_editor_property('Mode') == mode
    data = unreal.EditorAssetLibrary.find_asset_data(value.get_path_name())
    result, errors, warnings = unreal.get_editor_subsystem(unreal.EditorValidatorSubsystem).is_asset_valid(data, unreal.DataValidationUsecase.SCRIPT)
    assert result == unreal.DataValidationResult.VALID, (path, errors)
    if created:
        assert unreal.EditorAssetLibrary.save_loaded_asset(value, False)
    report['definitions'].append({'path': path, 'created': created, 'warnings': [str(w) for w in warnings]})
    return value


interaction = definition('DA_Encounter_Interaction', unreal.HodgeEncounterMode.INTERACTION)
area = definition('DA_Encounter_Area', unreal.HodgeEncounterMode.AREA)
existing_map = unreal.EditorAssetLibrary.does_asset_exist(MAP)
if existing_map:
    assert levels.load_level(MAP)
else:
    assert levels.new_level(MAP)


def spawn(cls, label, location, rotation=unreal.Rotator()):
    """示例关卡首次制作；已有同名实例不重新定位或覆盖。"""
    matches = [a for a in actors.get_all_level_actors() if a.get_actor_label() == label]
    assert len(matches) <= 1, label
    if matches:
        return matches[0]
    assert not existing_map, '已有关卡缺少实例，停止以保留手工布局：' + label
    value = actors.spawn_actor_from_class(cls, location, rotation)
    assert value
    value.set_actor_label(label)
    return value


floor = spawn(unreal.StaticMeshActor, 'EncounterFloor', unreal.Vector(0, 0, -50))
if not existing_map:
    floor.static_mesh_component.set_static_mesh(unreal.load_asset('/Engine/BasicShapes/Cube'))
    floor.set_actor_scale3d(unreal.Vector(60, 60, 1))
    floor.static_mesh_component.set_collision_enabled(unreal.CollisionEnabled.QUERY_AND_PHYSICS)
spawn(unreal.PlayerStart, 'EncounterPlayerStart1', unreal.Vector(0, 0, 100))
spawn(unreal.PlayerStart, 'EncounterPlayerStart2', unreal.Vector(0, -600, 100))
spawn(unreal.DirectionalLight, 'EncounterLight', unreal.Vector(0, 0, 900), unreal.Rotator(-40, -30, 0))
spawn(unreal.SkyLight, 'EncounterSky', unreal.Vector(0, 0, 900))
nav = spawn(unreal.NavMeshBoundsVolume, 'EncounterNavigation', unreal.Vector(0, 0, 200))
switch = spawn(unreal.HodgeEncounterSpawner, 'EncounterSwitch', unreal.Vector(150, 0, 100))
volume = spawn(unreal.HodgeEncounterSpawner, 'EncounterArea', unreal.Vector(1500, 0, 100))
if not existing_map:
    switch.set_editor_property('Definition', interaction)
    volume.set_editor_property('Definition', area)
    mesh = switch.get_component_by_class(unreal.StaticMeshComponent)
    assert mesh
    mesh.set_static_mesh(unreal.load_asset('/Engine/BasicShapes/Cube'))
    mesh.set_relative_scale3d(unreal.Vector(.5, .5, 1))
    assert unreal.HodgeAnimationAuthoringLibrary.configure_enemy_navigation_bounds(nav, unreal.Vector(5700, 5700, 600))
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    world.get_world_settings().set_editor_property('DefaultGameMode', unreal.load_asset('/Game/ThirdPerson/Blueprints/BP_ThirdPersonGameMode').generated_class())
    unreal.SystemLibrary.execute_console_command(world, 'RebuildNavigation')
    assert unreal.EditorLoadingAndSavingUtils.save_map(world, MAP)
    report['created_assets'].append(MAP)
assert switch.get_editor_property('Definition') == interaction
assert volume.get_editor_property('Definition') == area
assert hashlib.sha256(source_file.read_bytes()).hexdigest() == source_hash, '不应修改源 Guardian 配置'
report['instances'] = [{'label': a.get_actor_label(), 'location': str(a.get_actor_location()),
                        'definition': a.get_editor_property('Definition').get_path_name()} for a in [switch, volume]]
OUT.mkdir(parents=True, exist_ok=True)
(OUT / 'examples-setup.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
unreal.log('ENCOUNTER_EXAMPLES_CONFIGURED')
