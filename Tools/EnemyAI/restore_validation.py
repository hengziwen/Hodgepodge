import unreal
import builtins
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
restore = getattr(builtins, 'HODGE_ENEMY_RESTORE', None)
if restore:
    settings=unreal.get_default_object(unreal.load_class(None,'/Script/UnrealEd.EditorPerformanceSettings'))
    settings.set_editor_property('bThrottleCPUWhenNotForeground',restore['throttle'])
    w=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    for key,value in [('Slate.bAllowThrottling',restore['slate']),('t.IdleWhenNotForeground',restore['idle'])]:
        unreal.SystemLibrary.execute_console_command(w,key+' '+str(value))
    del builtins.HODGE_ENEMY_RESTORE
print('Validation preferences restored')
