import builtins
import unreal

assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
editor = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
settings = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.EditorPerformanceSettings'))
if not hasattr(builtins, 'HODGE_WING_RESTORE'):
    builtins.HODGE_WING_RESTORE = {'map': editor.get_editor_world().get_path_name().split('.')[0],
        'throttle': settings.get_editor_property('bThrottleCPUWhenNotForeground'),
        'slate': unreal.SystemLibrary.get_console_variable_int_value('Slate.bAllowThrottling'),
        'idle': unreal.SystemLibrary.get_console_variable_int_value('t.IdleWhenNotForeground')}
assert unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).load_level('/Game/Main/Maps/L_EnemyAIValidation')
assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(globals().get('WING_PLAYERS', 1), globals().get('WING_DEDICATED', False))
settings.set_editor_property('bThrottleCPUWhenNotForeground', False)
world = editor.get_editor_world()
unreal.SystemLibrary.execute_console_command(world, 'Slate.bAllowThrottling 0')
unreal.SystemLibrary.execute_console_command(world, 't.IdleWhenNotForeground 0')
unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_begin_play()
print('Requested wing validation PIE')
