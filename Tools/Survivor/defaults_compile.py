import unreal
bp=unreal.load_asset('/Game/CodexText/Survivor/BP_SurvivorMode')
unreal.BlueprintEditorLibrary.compile_blueprint(bp)
cdo=unreal.get_default_object(bp.generated_class())
print('AFTER COMPILE',cdo.get_editor_property('interface_class'))
print('FIND ACTOR',unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world().get_world_settings().get_editor_property('default_game_mode'))
