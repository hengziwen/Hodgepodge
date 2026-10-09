import unreal,builtins,sys
sys.path.insert(0, 'E:/Project/Git/Hodgepodge/Tools/HitReaction')
import feedback_listener
context = getattr(builtins,'HODGE_REACTION_TEST',{})
try: feedback_listener.unbind(context)
except TypeError: context['feedback_observers'] = []
try:
    h=getattr(builtins,'HODGE_REACTION_TEST',{}).get('suite_callback')
    if h: unreal.unregister_slate_post_tick_callback(h)
except Exception: pass
for world in unreal.EditorLevelLibrary.get_pie_worlds(True):
    unreal.SystemLibrary.execute_console_command(world, 'NetEmulation.Off')
unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_end_play()
print('Requested validation PIE stop')
