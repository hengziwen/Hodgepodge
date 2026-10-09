import unreal, sys
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
players = 2 if globals().get('REACTION_NETWORK', False) else 1
assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(players, False)
performance = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.EditorPerformanceSettings'))
performance.set_editor_property('bThrottleCPUWhenNotForeground', False)
unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_begin_play()
print('Requested ' + ('listen-server' if players == 2 else 'standalone') + ' PIE')
