"""Keep the second player and floor edges outside movement transition probes."""
import unreal, builtins

s = builtins.HODGE_REACTION_TEST
assert s['floor'] and unreal.SystemLibrary.is_valid(s['floor'])
s['floor'].set_actor_scale3d(unreal.Vector(100, 100, .5))
if s.get('network'):
    player_id = s['target'].get_editor_property('PlayerState').get_editor_property('PlayerId')
    for world in [s['world']] + s['client_worlds']:
        for pawn in unreal.GameplayStatics.get_all_actors_of_class(world, s['source'].get_class()):
            state = pawn.get_editor_property('PlayerState')
            if state and state.get_editor_property('PlayerId') == player_id:
                pawn.character_movement.stop_movement_immediately()
                pawn.set_actor_location(unreal.Vector(1000, 3500, 95), False, False)
print('Prepared a 100m floor; second player is 25m away from the movement lane')
