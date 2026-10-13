import unreal
import builtins

assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
assert unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).load_level('/Game/Main/Maps/L_EnemyAIValidation')
players = globals().get('ENEMY_PLAYERS', 1)
assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(players, False)
settings = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.EditorPerformanceSettings'))
if not hasattr(builtins, 'HODGE_ENEMY_RESTORE'):
    builtins.HODGE_ENEMY_RESTORE = {'throttle': settings.get_editor_property('bThrottleCPUWhenNotForeground'),
        'slate': unreal.SystemLibrary.get_console_variable_int_value('Slate.bAllowThrottling'),
        'idle': unreal.SystemLibrary.get_console_variable_int_value('t.IdleWhenNotForeground')}
settings.set_editor_property('bThrottleCPUWhenNotForeground', False)
world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
unreal.SystemLibrary.execute_console_command(world, 'Slate.bAllowThrottling 0')
unreal.SystemLibrary.execute_console_command(world, 't.IdleWhenNotForeground 0')
unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_begin_play()
print('Requested EnemyAI ' + ('Listen Server with remote client' if players > 1 else 'single') + ' PIE')
