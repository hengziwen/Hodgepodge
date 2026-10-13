"""恢复本轮开始前的地图、网络选项与性能选项。"""
import unreal,json
from pathlib import Path
WORK=Path('E:/Project/Git/Hodgepodge/Saved/WingAttack')
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
saved=json.loads((WORK/'editor-restore.json').read_text(encoding='utf-8'))
settings=unreal.get_default_object(unreal.load_class(None,'/Script/UnrealEd.LevelEditorPlaySettings'))
if not saved['play_settings']:
    # 引擎的非反射网络枚举不能由 Python 导出，使用未改动的持久配置恢复。
    import re
    ini=(Path('E:/Project/Git/Hodgepodge/Saved/Config/WindowsEditor/EditorPerProjectUserSettings.ini')).read_text(encoding='utf-8-sig')
    def setting(name,default):
        match=re.search(r'^'+name+r'=(.*)$',ini,re.M)
        return match.group(1).strip() if match else default
    players=int(setting('PlayNumberOfClients','1'))
    net=setting('PlayNetMode','PIE_Standalone')
    assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(players,net=='PIE_Client')
    settings.set_editor_property('RunUnderOneProcess',setting('RunUnderOneProcess','True')=='True')
    settings.set_editor_property('bLaunchSeparateServer',setting('bLaunchSeparateServer','False')=='True')
for prop,value in saved['play_settings'].items():
    current=settings.get_editor_property(prop)
    if hasattr(current,'value'):
        value=next(getattr(type(current),name) for name in dir(type(current)) if name.startswith('PIE_') and getattr(type(current),name).value==value)
    settings.set_editor_property(prop,value)
perf=json.loads((WORK/'performance-restore.json').read_text(encoding='utf-8'))
performance=unreal.get_default_object(unreal.load_class(None,'/Script/UnrealEd.EditorPerformanceSettings'))
performance.set_editor_property('bThrottleCPUWhenNotForeground',perf['throttle'])
world=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
for name,value in [('Slate.bAllowThrottling',perf['slate']),('t.IdleWhenNotForeground',perf['idle'])]:
    unreal.SystemLibrary.execute_console_command(world,name+' '+str(value))
assert unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).load_level(saved['map'])
print('已恢复原始编辑器状态')
