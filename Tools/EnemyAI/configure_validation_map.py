"""Create an isolated, saved map with preplaced enemies and actual navigation bounds."""
import unreal
import json
from pathlib import Path

MAP = '/Game/Main/Maps/L_EnemyAIValidation'
ROOT = '/Game/Main/Character/Enemies/Guardian'
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
levels = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
if unreal.EditorAssetLibrary.does_asset_exist(MAP):
    assert levels.load_level(MAP)
else:
    assert levels.new_level(MAP)
actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)


def spawn(cls, label, location, rotation=unreal.Rotator()):
    found = next((a for a in actors.get_all_level_actors() if a.get_actor_label() == label), None)
    if not found:
        found = actors.spawn_actor_from_class(cls, location, rotation)
    assert found, label
    found.set_actor_label(label)
    found.set_actor_location(location, False, False)
    found.set_actor_rotation(rotation, False)
    return found


floor = spawn(unreal.StaticMeshActor, 'HodgeEnemyFloor', unreal.Vector(0, 0, -50))
floor.static_mesh_component.set_static_mesh(unreal.load_asset('/Engine/BasicShapes/Cube'))
floor.set_actor_scale3d(unreal.Vector(60, 60, 1))
floor.static_mesh_component.set_collision_enabled(unreal.CollisionEnabled.QUERY_AND_PHYSICS)
spawn(unreal.PlayerStart, 'HodgeEnemyPlayerStart1', unreal.Vector(0, 0, 100))
spawn(unreal.PlayerStart, 'HodgeEnemyPlayerStart2', unreal.Vector(0, 1200, 100))
spawn(unreal.DirectionalLight, 'HodgeEnemyLight', unreal.Vector(0, 0, 900), unreal.Rotator(-40, -30, 0))
spawn(unreal.SkyLight, 'HodgeEnemySky', unreal.Vector(0, 0, 900))
nav = spawn(unreal.NavMeshBoundsVolume, 'HodgeEnemyNavigation', unreal.Vector(0, 0, 200))
assert unreal.HodgeAnimationAuthoringLibrary.configure_enemy_navigation_bounds(nav, unreal.Vector(5700, 5700, 600))
guardian = spawn(unreal.load_asset(ROOT + '/BP_Enemy_Guardian').generated_class(), 'HodgeGuardian', unreal.Vector(650, 0, 100), unreal.Rotator(0, 180, 0))
equipped = spawn(unreal.load_asset(ROOT + '/BP_Enemy_GuardianEquipped').generated_class(), 'HodgeGuardianEquipped', unreal.Vector(2300, 1500, 100), unreal.Rotator(0, 180, 0))
world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
world.get_world_settings().set_editor_property('DefaultGameMode', unreal.load_asset('/Game/ThirdPerson/Blueprints/BP_ThirdPersonGameMode').generated_class())
unreal.SystemLibrary.execute_console_command(world, 'RebuildNavigation')
assert unreal.EditorLoadingAndSavingUtils.save_map(world, MAP)
summary = {'map': MAP, 'preplaced': [guardian.get_path_name(), equipped.get_path_name()], 'navigation_bounds': nav.get_actor_bounds(False)}
Path('E:/Project/Git/Hodgepodge/Saved/EnemyAIImplementation/map.json').write_text(json.dumps(summary, default=str, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps(summary, default=str))
