import unreal
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(2, False)
performance=unreal.get_default_object(unreal.load_class(None,'/Script/UnrealEd.EditorPerformanceSettings'))
performance.set_editor_property('bThrottleCPUWhenNotForeground',False)
unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_begin_play()
print('Requested two-player Listen Server PIE')
