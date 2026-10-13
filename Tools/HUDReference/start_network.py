import unreal
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(2,False)
settings=unreal.get_default_object(unreal.load_class(None,'/Script/UnrealEd.LevelEditorPlaySettings'))
settings.set_editor_property('ClientWindowWidth',1280)
settings.set_editor_property('ClientWindowHeight',720)
unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_begin_play()
print('Requested listen server and a 1280x720 remote client')
