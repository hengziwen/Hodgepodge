"""将锁定标记接到可编辑 WBP，并启用正式 Dash 的即时朝向；不覆盖已有 Designer 样式。"""
import unreal,json,shutil
from pathlib import Path
ROOT=Path('E:/Project/Git/Hodgepodge');WORK=ROOT/'Saved/LockOnDash'
lock_path='/Game/Main/Character/Hero/Targeting/DA_HeroLockOn'
dash_path='/Game/Main/Character/Hero/Ability/Sprint/DA_Hero_SprintAbility'
widget_path='/Game/Main/UI/Targeting/WBP_HeroLockOnIndicator'
for path in [lock_path,dash_path]:
 file=ROOT/'Content'/(path[6:]+'.uasset');backup=WORK/'Before'/file.name
 if not backup.exists():shutil.copy2(file,backup)
widget=unreal.load_asset(widget_path)
if not widget:
 factory=unreal.WidgetBlueprintFactory();factory.set_editor_property('ParentClass',unreal.HodgeLockOnIndicatorWidget.static_class())
 widget=unreal.AssetToolsHelpers.get_asset_tools().create_asset('WBP_HeroLockOnIndicator','/Game/Main/UI/Targeting',unreal.WidgetBlueprint,factory)
assert widget and unreal.HodgeUIAuthoringLibrary.configure_lock_on_indicator(widget)
assert unreal.EditorAssetLibrary.save_loaded_asset(widget,False)
widget_class=unreal.EditorAssetLibrary.load_blueprint_class(widget_path);assert widget_class
lock=unreal.load_asset(lock_path);dash=unreal.load_asset(dash_path)
lock.set_editor_property('IndicatorClass',widget_class)
dash.set_editor_property('bInstantFacingAtStart',True)
assert unreal.EditorAssetLibrary.save_loaded_asset(lock,False)
assert unreal.EditorAssetLibrary.save_loaded_asset(dash,False)
r={'widget':widget_path,'class':widget_class.get_path_name(),'lock_profile':lock_path,'dash_profile':dash_path,'instant_facing':dash.get_editor_property('bInstantFacingAtStart')}
(WORK/'configuration.json').write_text(json.dumps(r,indent=2),encoding='utf-8');print(json.dumps(r))
