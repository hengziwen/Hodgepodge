import unreal
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(2, True)
settings = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.EditorPerformanceSettings'))
settings.set_editor_property('bThrottleCPUWhenNotForeground', False)
unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_begin_play()
print('Requested Play As Client with two clients and PIE dedicated server')
