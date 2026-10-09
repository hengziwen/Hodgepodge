import unreal, builtins, json
s = builtins.HODGE_REACTION_TEST
enemy_class = unreal.load_asset('/Game/CodexText/HitReactionValidation/BP_EnemyProbe').generated_class()
if s.get('enemy') and unreal.SystemLibrary.is_valid(s['enemy']): s['enemy'].destroy_actor()
enemy = unreal.HodgeCombatValidationLibrary.spawn_hit_reaction_validation_actor(s['world'], enemy_class, unreal.Vector(1200, 1000, 95))
assert enemy
s['enemy'] = enemy
for pawn in [s['server_remote'], s['host']]:
    pawn.character_movement.stop_movement_immediately()
    pawn.set_actor_location(unreal.Vector(1800, 1800, 95), False, False)
s.update({'source': s['server_remote'], 'source_owner': s['remote'], 'target': enemy,
          'owned_target': enemy, 'output_name': 'network_enemy_100ms'})
print('Spawned Pawn-owned ASC enemy: ' + enemy.get_path_name())
