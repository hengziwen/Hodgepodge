"""等待 PIE 退出后恢复本任务记录的性能与播放设置，可选择恢复原地图。"""
from pathlib import Path
import gc
import json
import time
import traceback
import unreal

BASE = Path(unreal.Paths.project_dir()).resolve() / 'Saved/Encounter_20261025'
WORK = Path(globals().get('LOCK_ON_WORK', str(BASE)))
RESTORE_MAP = bool(globals().get('RESTORE_MAP', False))
state = {'start': time.monotonic()}


def restore():
    """仅恢复已记录的本次测试设置，拒绝覆盖任何用户未保存工作。"""
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages(), '存在未保存关卡，停止恢复地图'
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages(), '存在未保存内容，停止恢复地图'
    candidates = [BASE / 'single/performance-restore.json', WORK / 'performance-restore.json']
    source = next((p for p in candidates if p.exists()), None)
    assert source, '缺少测试前的性能设置记录'
    performance = json.loads(source.read_text(encoding='utf-8'))
    settings = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.EditorPerformanceSettings'))
    settings.set_editor_property('bThrottleCPUWhenNotForeground', performance['throttle'])
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    unreal.SystemLibrary.execute_console_command(world, 'Slate.bAllowThrottling ' + str(performance['slate']))
    unreal.SystemLibrary.execute_console_command(world, 't.IdleWhenNotForeground ' + str(performance['idle']))
    original = json.loads((BASE / 'pie-before.json').read_text(encoding='utf-8'))
    # PlayNetMode 的原生枚举未暴露给 Python，复用现有精确设置入口再恢复人数与进程选项。
    mode = original['PlayNetMode']
    assert mode in ['PIE_Standalone', 'PIE_ListenServer', 'PIE_Client'], mode
    assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(2 if mode == 'PIE_ListenServer' else 1, mode == 'PIE_Client')
    play = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.LevelEditorPlaySettings'))
    play.set_editor_property('PlayNumberOfClients', original['PlayNumberOfClients'])
    play.set_editor_property('RunUnderOneProcess', original['RunUnderOneProcess'])
    if RESTORE_MAP:
        world = None
        gc.collect()
        previous = json.loads((BASE / 'editor-before-build.json').read_text(encoding='utf-8'))
        assert unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).load_level(previous['map'])
    return {'performance': performance, 'play': original,
            'map': unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world().get_path_name().split('.')[0]}


def tick(delta):
    """异步结束 PIE 期间只等待，不抢先修改仍在使用的播放配置。"""
    try:
        if unreal.EditorLevelLibrary.get_pie_worlds(True):
            if time.monotonic() - state['start'] > 20:
                raise TimeoutError('等待 PIE 结束超时')
            return
        result = {'passed': True, **restore()}
    except Exception:
        result = {'passed': False, 'error': traceback.format_exc()}
    unreal.unregister_slate_post_tick_callback(state['callback'])
    WORK.mkdir(parents=True, exist_ok=True)
    (WORK / 'editor-restored.json').write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')


state['callback'] = unreal.register_slate_post_tick_callback(tick)
