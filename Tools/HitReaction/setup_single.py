import unreal, builtins, json
worlds = unreal.EditorLevelLibrary.get_pie_worlds(False)
assert len(worlds) == 1, len(worlds)
w = worlds[0]
pc = unreal.GameplayStatics.get_player_controller(w, 0)
target_pc = unreal.GameplayStatics.create_player(w, -1, True)
assert target_pc
builtins.HODGE_REACTION_TEST = {'world': w, 'source_pc': pc, 'target_pc': target_pc}
print(json.dumps({'world': w.get_path_name(), 'source': str(pc.get_controlled_pawn()), 'target': str(target_pc.get_controlled_pawn())}))
