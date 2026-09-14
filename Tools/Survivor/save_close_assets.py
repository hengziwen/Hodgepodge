import unreal
print('DIRTY',[p.get_name() for p in unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()+unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()])
for p in ['/Game/CodexText/WBP_MainMenu','/Game/CodexText/Survivor/WBP_SurvivorHUD','/Game/CodexText/Survivor/BP_SurvivorHero','/Game/CodexText/Survivor/BP_SurvivorMode']:
 a=unreal.load_asset(p)
 unreal.BlueprintEditorLibrary.compile_blueprint(a)
 unreal.EditorAssetLibrary.save_loaded_asset(a,False)
 unreal.get_editor_subsystem(unreal.AssetEditorSubsystem).close_all_editors_for_asset(a)
