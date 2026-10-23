"""结束本轮玩家夹具并恢复偏好与共享探针内存，不保存测试期间的资产覆盖。"""
from pathlib import Path
import builtins
import sys
import unreal

root = Path(unreal.Paths.project_dir())
state = {'ended': False}
s = builtins.HODGE_REACTION_TEST
sys.path.insert(0, str(root / 'Tools/HitReaction'))
import feedback_listener
feedback_listener.unbind(s)
for world in unreal.EditorLevelLibrary.get_pie_worlds(True):
    unreal.SystemLibrary.execute_console_command(world, 'NetEmulation.PktLag 0')
    unreal.SystemLibrary.execute_console_command(world, 'NetEmulation.PktLoss 0')
unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_end_play()


def tick(delta):
    """等待 World 销毁后重载明确的探针包，包含密集命中临时通知，避免污染后续运行。"""
    if unreal.EditorLevelLibrary.get_pie_worlds(True):
        return
    try:
        assets = [unreal.load_asset('/Game/CodexText/HitReactionValidation/' + name)
                  for name in ['AS_ReactionProbe', 'DA_ProbeAttack', 'DA_ProbeHold', 'AM_ProbeAttack']]
        changed, error = unreal.EditorLoadingAndSavingUtils.reload_packages(
            [asset.get_outermost() for asset in assets], unreal.ReloadPackagesInteractionMode.ASSUME_POSITIVE)
        assert changed and not str(error), str(error)
        previous = builtins.HODGE_ACT_PREFERENCES
        assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(1 if previous['mode'] == 0 else 2, previous['mode'] == 2)
        settings = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.LevelEditorPlaySettings'))
        for name, value in previous['values'].items():
            settings.set_editor_property(name, value)
        performance = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.EditorPerformanceSettings'))
        performance.set_editor_property('bThrottleCPUWhenNotForeground', previous['throttle'])
        print('ACT_FIXTURE_CLEANUP_OK')
    finally:
        unreal.unregister_slate_post_tick_callback(state['callback'])


state['callback'] = unreal.register_slate_post_tick_callback(tick)
