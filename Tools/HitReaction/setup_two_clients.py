import unreal, builtins, json
worlds = unreal.EditorLevelLibrary.get_pie_worlds(True)
assert len(worlds) == 3, len(worlds)
cls = unreal.load_asset('/Game/Main/Character/Hero/BP_Hero_Pover').generated_class()
server = next(w for w in worlds if all(p.has_authority() for p in unreal.GameplayStatics.get_all_actors_of_class(w, cls)))
clients = [(w, unreal.GameplayStatics.get_player_character(w, 0)) for w in worlds if w != server]
assert all(p and p.get_local_role() == unreal.NetRole.ROLE_AUTONOMOUS_PROXY for w, p in clients)
source_world, source_owner = clients[0]; target_world, owned_target = clients[1]
def pid(p): return p.get_editor_property('PlayerState').get_editor_property('PlayerId')
authority_pawns = [p for p in unreal.GameplayStatics.get_all_actors_of_class(server, cls) if p.get_editor_property('PlayerState')]
source = next(p for p in authority_pawns if pid(p) == pid(source_owner))
target = next(p for p in authority_pawns if pid(p) == pid(owned_target))
observer = next(p for p in unreal.GameplayStatics.get_all_actors_of_class(source_world, cls) if p.get_editor_property('PlayerState') and pid(p) == pid(target))
assert observer.get_local_role() == unreal.NetRole.ROLE_SIMULATED_PROXY
builtins.HODGE_REACTION_TEST = {'world': server, 'client_world': source_world, 'client_worlds': [source_world, target_world],
    'source_pc': source.get_controller(), 'target_pc': target.get_controller(), 'source_owner': source_owner,
    'owned_target': owned_target, 'observer_target': observer, 'host': source, 'server_remote': target,
    'remote': owned_target, 'client_host': observer, 'network': True, 'lag_ms': 0, 'use_formal_profile': True,
    'grant_path': '/Game/CodexText/HitReactionValidation/BP_HeroPresentationProbe', 'output_name': 'hero_two_clients_100ms'}
print(json.dumps({'server': server.get_path_name(), 'clients': [w.get_path_name() for w, p in clients],
    'source_role': str(source_owner.get_local_role()), 'owner_role': str(owned_target.get_local_role()),
    'observer_role': str(observer.get_local_role())}))
