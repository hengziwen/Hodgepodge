import builtins
import unreal

assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
restore = getattr(builtins, 'HODGE_WING_RESTORE', None)
if restore:
    settings = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.EditorPerformanceSettings'))
    settings.set_editor_property('bThrottleCPUWhenNotForeground', restore['throttle'])
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    for name, value in [('Slate.bAllowThrottling', restore['slate']), ('t.IdleWhenNotForeground', restore['idle'])]:
        unreal.SystemLibrary.execute_console_command(world, name + ' ' + str(value))
    assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(1, False)
    assert unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).load_level(restore['map'])
    del builtins.HODGE_WING_RESTORE
print('Restored original map and editor performance preferences; PIE settings returned to initial standalone mode.')
