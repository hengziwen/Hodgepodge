import unreal
bp=unreal.load_asset('/Game/CodexText/Survivor/BP_SurvivorMode')
unreal.BlueprintEditorLibrary.compile_blueprint(bp)
print(unreal.EditorAssetLibrary.save_loaded_asset(bp,False))
