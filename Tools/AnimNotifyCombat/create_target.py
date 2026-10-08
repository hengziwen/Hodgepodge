import unreal,builtins,json
w=unreal.EditorLevelLibrary.get_pie_worlds(False)[0]
pc=unreal.GameplayStatics.get_player_controller(w,0)
target_pc=unreal.GameplayStatics.create_player(w,-1,True);assert target_pc
builtins.HODGE_SKILL_TEST={'world':w,'pc':pc,'target_pc':target_pc}
print(json.dumps({'target_pc':target_pc.get_path_name()}))
