import unreal,builtins,json
from pathlib import Path
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
world=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
performance=unreal.get_default_object(unreal.load_class(None,'/Script/UnrealEd.EditorPerformanceSettings'))
settings=unreal.get_default_object(unreal.load_class(None,'/Script/UnrealEd.LevelEditorPlaySettings'))
keys=['PlayNumberOfClients','PlayNetMode','RunUnderOneProcess','NewWindowWidth','NewWindowHeight','ClientWindowWidth','ClientWindowHeight','LastExecutedPlayModeType']
restore={'map':world.get_path_name().split('.')[0],'throttle':performance.get_editor_property('bThrottleCPUWhenNotForeground'),'play':{}}
for key in keys:
    try:restore['play'][key]=settings.get_editor_property(key)
    except Exception:pass
builtins.HODGE_HUD_RESTORE=restore
Path(unreal.Paths.project_saved_dir(),'HUDReference','restore-state.txt').write_text(str(restore),encoding='utf-8')
performance.set_editor_property('bThrottleCPUWhenNotForeground',False)
assert unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).load_level('/Game/ThirdPerson/Maps/ThirdPersonMap')
assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(1,False)
unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_begin_play()
print('Requested single-player HUD validation')
