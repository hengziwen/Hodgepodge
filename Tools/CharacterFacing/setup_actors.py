import unreal, builtins
from pathlib import Path

s = builtins.HODGE_REACTION_TEST
if s.get('network'):
    root = Path(unreal.Paths.project_dir())
    exec(compile((root / 'Tools/HitReaction/setup_actors.py').read_text(encoding='utf-8'), 'setup_actors.py', 'exec'))
else:
    pawn = s['source_pc'].get_controlled_pawn()
    assert pawn
    s['source'] = s['target'] = pawn
    s['hold'] = unreal.load_asset('/Game/CodexText/HitReactionValidation/DA_ProbeHold')
    floor = unreal.HodgeCombatValidationLibrary.spawn_hit_reaction_validation_actor(s['world'], unreal.StaticMeshActor, unreal.Vector(1000,1000,-30))
    floor.static_mesh_component.set_mobility(unreal.ComponentMobility.MOVABLE)
    floor.static_mesh_component.set_static_mesh(unreal.load_asset('/Engine/BasicShapes/Cube'))
    floor.set_actor_scale3d(unreal.Vector(30,30,.5))
    floor.static_mesh_component.set_collision_enabled(unreal.CollisionEnabled.QUERY_AND_PHYSICS)
    floor.static_mesh_component.set_collision_response_to_all_channels(unreal.CollisionResponseType.ECR_BLOCK)
    s['floor'] = floor
    manager = pawn.get_component_by_class(unreal.HodgeEquipmentManagerComponent)
    cls = unreal.load_asset(s['grant_path']).generated_class()
    grant = manager.equip_item(cls)
    assert grant
    s['grants'] = [(manager, grant)]
    print('Prepared formal Hero, one probe grant, and a collision floor')
