import unreal,builtins
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
restore=getattr(builtins,'HODGE_HUD_RESTORE',None)
assert restore
unreal.get_default_object(unreal.load_class(None,'/Script/UnrealEd.EditorPerformanceSettings')).set_editor_property('bThrottleCPUWhenNotForeground',restore['throttle'])
settings=unreal.get_default_object(unreal.load_class(None,'/Script/UnrealEd.LevelEditorPlaySettings'))
for key,value in restore['play'].items():settings.set_editor_property(key,value)
assert unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).load_level(restore['map'])
del builtins.HODGE_HUD_RESTORE
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
print('Original map, PIE settings and performance preference restored')
