import unreal, builtins, json, sys
sys.path.insert(0, 'E:/Project/Git/Hodgepodge/Tools/HitReaction')
import feedback_listener
s = builtins.HODGE_REACTION_TEST
w = s['world']
for manager, grant in s.get('grants', []):
    if unreal.SystemLibrary.is_valid(grant): manager.unequip_item(grant)
if s.get('floor') and unreal.SystemLibrary.is_valid(s['floor']): s['floor'].destroy_actor()
s['source'] = s['source_pc'].get_controlled_pawn()
s['target'] = s['target_pc'].get_controlled_pawn()
assert s['source'] and s['target']
s['profile'] = unreal.load_asset('/Game/Main/Character/Hero/Anim/HitReactions/DA_Hero_HitReaction' if s.get('use_formal_profile') else '/Game/CodexText/HitReactionValidation/DA_ReactionProfile')
s['definition'] = unreal.load_asset('/Game/CodexText/HitReactionValidation/DA_ProbeAttack')
s['hold'] = unreal.load_asset('/Game/CodexText/HitReactionValidation/DA_ProbeHold')
s['grant_class'] = unreal.load_asset(s.get('grant_path', '/Game/CodexText/HitReactionValidation/BP_ReactionGrant')).generated_class()
s['grants'] = []
s['feedback'] = {}
floor = unreal.HodgeCombatValidationLibrary.spawn_hit_reaction_validation_actor(w, unreal.StaticMeshActor, unreal.Vector(1000, 1000, -30))
assert floor
floor.static_mesh_component.set_mobility(unreal.ComponentMobility.MOVABLE)
floor.static_mesh_component.set_static_mesh(unreal.load_asset('/Engine/BasicShapes/Cube'))
floor.set_actor_scale3d(unreal.Vector(30, 30, 0.5))
floor.static_mesh_component.set_collision_enabled(unreal.CollisionEnabled.QUERY_AND_PHYSICS)
floor.static_mesh_component.set_collision_response_to_all_channels(unreal.CollisionResponseType.ECR_BLOCK)
s['floor'] = floor
for i, pawn in enumerate([s['source'], s['target']]):
    pawn.character_movement.stop_movement_immediately()
    pawn.set_actor_location(unreal.Vector(1000 + i * 200, 1000, 95), False, False)
    pawn.set_actor_rotation(unreal.Rotator(0, 0, 0), False)
    pawn.capsule_component.set_collision_response_to_channel(unreal.CollisionChannel.ECC_PAWN, unreal.CollisionResponseType.ECR_OVERLAP)
    reaction = pawn.get_component_by_class(unreal.HodgeHitReactionComponent)
    if s.get('use_formal_profile'): assert reaction.get_profile() == s['profile'], pawn.get_path_name()
    else: reaction.set_editor_property('OverrideProfile', s['profile'])
    manager = pawn.get_component_by_class(unreal.HodgeEquipmentManagerComponent)
    grant = manager.equip_item(s['grant_class'])
    assert grant, pawn.get_path_name()
    s['grants'].append((manager, grant))
    key = pawn.get_path_name()
    s['feedback'][key] = 0
    feedback_listener.bind(pawn, s)
if s.get('network'):
    for p in [s['remote'],s['client_host']]:
        key=p.get_path_name();s['feedback'][key]=0
        feedback_listener.bind(p, s)
s['lib'] = unreal.get_default_object(unreal.load_class(None, '/Script/GameplayAbilities.AbilitySystemBlueprintLibrary'))
print(json.dumps({'source': s['source'].get_path_name(), 'target': s['target'].get_path_name(),
                 'floor': floor.get_path_name(), 'profile': s['profile'].get_path_name()}))
