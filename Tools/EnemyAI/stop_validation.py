import unreal
import builtins
unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_end_play()
print('Requested PIE end')
