import unreal
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
levels=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
assert levels.load_level('/Game/ThirdPerson/Maps/ThirdPersonMap')
assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(1,False)
performance=unreal.get_default_object(unreal.load_class(None,'/Script/UnrealEd.EditorPerformanceSettings'))
performance.set_editor_property('bThrottleCPUWhenNotForeground',False)
levels.editor_request_begin_play()
print('Started formal Experience with Hodge UI, standalone.')
