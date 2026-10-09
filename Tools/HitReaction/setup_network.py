import unreal,builtins,json
worlds=unreal.EditorLevelLibrary.get_pie_worlds(False)
assert len(worlds)==2,len(worlds)
owned=[(w,unreal.GameplayStatics.get_player_character(w,0)) for w in worlds]
assert all(p for w,p in owned)
server,host=next((w,p) for w,p in owned if p.has_authority())
client,remote=next((w,p) for w,p in owned if not p.has_authority())
def pid(p):return p.get_editor_property('PlayerState').get_editor_property('PlayerId')
server_remote=next(p for p in unreal.GameplayStatics.get_all_actors_of_class(server,host.get_class()) if p.get_editor_property('PlayerState') and pid(p)==pid(remote))
client_host=next(p for p in unreal.GameplayStatics.get_all_actors_of_class(client,host.get_class()) if p.get_editor_property('PlayerState') and pid(p)==pid(host))
builtins.HODGE_REACTION_TEST={'world':server,'client_world':client,'source_pc':unreal.GameplayStatics.get_player_controller(server,0),
    'target_pc':server_remote.get_controller(),'network':True,'lag_ms':0,'output_name':'network_owner_0ms',
    'source_owner':host,'owned_target':remote,'observer_target':server_remote,
    'host':host,'remote':remote,'server_remote':server_remote,'client_host':client_host}
print(json.dumps({'server':server.get_path_name(),'client':client.get_path_name(),'owner_role':str(remote.get_local_role()),
                 'simulated_role':str(client_host.get_local_role())}))
