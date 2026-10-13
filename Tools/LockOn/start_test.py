"""在独立验证地图启动指定网络模式，保存编辑器性能设置。"""
import unreal,json
from pathlib import Path
WORK=Path(globals().get('LOCK_ON_WORK','E:/Project/Git/Hodgepodge/Saved/LockOn'))
WORK.mkdir(parents=True,exist_ok=True)
mode=globals().get('LOCK_ON_MODE','single')
runner=globals().get('LOCK_ON_RUNNER','E:/Project/Git/Hodgepodge/Tools/LockOn/run_e2e.py')
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
settings=unreal.get_default_object(unreal.load_class(None,'/Script/UnrealEd.EditorPerformanceSettings'))
path=WORK/'performance-restore.json'
if not path.exists():
    path.write_text(json.dumps({'throttle':settings.get_editor_property('bThrottleCPUWhenNotForeground'),
        'slate':unreal.SystemLibrary.get_console_variable_int_value('Slate.bAllowThrottling'),
        'idle':unreal.SystemLibrary.get_console_variable_int_value('t.IdleWhenNotForeground')},indent=2),encoding='utf-8')
import gc
gc.collect()
if unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world().get_path_name().split('.')[0]!='/Game/Main/Maps/L_EnemyAIValidation':
    assert unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).load_level('/Game/Main/Maps/L_EnemyAIValidation')
assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(1 if mode=='single' else 2,mode=='dedicated')
settings.set_editor_property('bThrottleCPUWhenNotForeground',False)
world=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
unreal.SystemLibrary.execute_console_command(world,'Slate.bAllowThrottling 0')
unreal.SystemLibrary.execute_console_command(world,'t.IdleWhenNotForeground 0')
world=None
gc.collect()
# 初始化期间持续隔离敌人，避免脚本接入前玩家已被攻击并销毁。
import time,traceback
startup={'wall':time.monotonic()}
def prepare(delta):
    try:
        for w in unreal.EditorLevelLibrary.get_pie_worlds(True):
            if not unreal.SystemLibrary.is_server(w):continue
            for enemy in unreal.GameplayStatics.get_all_actors_of_class(w,unreal.HodgeEnemyCharacter):
                controller=enemy.get_controller()
                if controller:
                    controller.stop_movement()
                    if controller.brain_component:controller.brain_component.stop_logic('LockOn validation isolation')
                enemy.set_actor_location(unreal.Vector(2300,2300,100),False,True)
        peers=[a for w in unreal.EditorLevelLibrary.get_pie_worlds(True) for a in unreal.GameplayStatics.get_all_actors_of_class(w,unreal.HodgeHeroCharacter)]
        ready=[a for a in peers if a.get_hodge_ability_system_component() and a.get_component_by_class(unreal.HodgeLockOnComponent) and a.get_component_by_class(unreal.HodgeLockOnComponent).is_ready()]
        expected=1 if mode=='single' else (6 if mode=='dedicated' else 4)
        if len(ready)>=expected:
            unreal.unregister_slate_post_tick_callback(startup['callback'])
            exec(Path(runner).read_text(encoding='utf-8-sig'),{'unreal':unreal,'LOCK_ON_MODE':mode,'LOCK_ON_WORK':str(WORK)})
        elif time.monotonic()-startup['wall']>65:
            raise RuntimeError('玩家初始化超时')
    except Exception:
        unreal.unregister_slate_post_tick_callback(startup['callback'])
        (WORK/(mode+'-startup-error.txt')).write_text(traceback.format_exc(),encoding='utf-8')
startup['callback']=unreal.register_slate_post_tick_callback(prepare)
unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_begin_play()
print('启动锁定验证模式：'+mode)
