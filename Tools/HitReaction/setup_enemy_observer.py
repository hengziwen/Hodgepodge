import unreal, builtins, json, sys
sys.path.insert(0, 'E:/Project/Git/Hodgepodge/Tools/HitReaction')
import feedback_listener
s = builtins.HODGE_REACTION_TEST
enemy = s['enemy']
client_enemy = next(p for p in unreal.GameplayStatics.get_all_actors_of_class(s['client_world'], enemy.get_class()))
s['observer_target'] = client_enemy
for pawn in [enemy, client_enemy]:
    assert pawn.get_hodge_ability_system_component(), pawn.get_path_name()
    pawn.capsule_component.set_collision_response_to_channel(unreal.CollisionChannel.ECC_PAWN, unreal.CollisionResponseType.ECR_OVERLAP)
    key = pawn.get_path_name(); s['feedback'][key] = 0
    feedback_listener.bind(pawn, s)
print(json.dumps({'authority_health': enemy.get_component_by_class(unreal.HodgeHealthComponent).get_health(),
                 'client_health': client_enemy.get_component_by_class(unreal.HodgeHealthComponent).get_health(),
                 'role': str(client_enemy.get_local_role()), 'controller': str(enemy.get_controller()),
                 'combat': str(enemy.get_component_by_class(unreal.HodgeCombatComponentBase))}))
