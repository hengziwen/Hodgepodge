import unreal,json
from pathlib import Path

def diag_tick(dt):
 w=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world()
 if not w:return
 m=unreal.GameplayStatics.get_game_mode(w)
 if not m:return
 c=unreal.get_default_object(m.get_class())
 Path('D:/CodexUE55Lab/survivor-diag.txt').write_text(str([(n,str(m.get_editor_property(n)),str(c.get_editor_property(n))) for n in ['interface_class','enemy_material','default_pawn_class','hero','interface']])+str(m.get_class()),encoding='utf-8')
 unreal.SystemLibrary.execute_console_command(w,'Shot SHOWUI')
 unreal.unregister_slate_post_tick_callback(diag_handle)
diag_handle=unreal.register_slate_post_tick_callback(diag_tick)
