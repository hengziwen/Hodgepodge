"""Configure only the two requested HUD elements and their local menu entry."""
import unreal, json
from pathlib import Path

assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
base = '/Game/Main/UI'
menu_path = base + '/Menu/WBP_HodgeEndMenu'
if not unreal.EditorAssetLibrary.does_asset_exist(menu_path):
    assert unreal.EditorAssetLibrary.duplicate_asset(base+'/Menu/WBP_HodgePauseMenu',menu_path)
menu = unreal.load_asset(menu_path)
tree = unreal.find_object(menu,'WidgetTree')
def widget(name):
    result=unreal.find_object(tree,name)
    assert result, name
    return result
widget('MenuTitle').set_text('终焉')
widget('BackHint').set_text('敬请期待\nEsc 返回')
widget('ResumeButton').set_editor_property('LabelText','返回')
widget('ConfirmButton').set_visibility(unreal.SlateVisibility.COLLAPSED)
widget('PanelBackground').set_brush_color(unreal.LinearColor(.035,.045,.04,.94))
unreal.BlueprintEditorLibrary.compile_blueprint(menu)
assert unreal.EditorAssetLibrary.save_loaded_asset(menu, False)

task=unreal.AssetImportTask()
task.filename=str(Path(unreal.Paths.project_dir())/'Tools/HUDReference/Art/end-icon.png')
task.destination_path=base+'/Art'
task.destination_name='T_EndIcon'
task.automated=True
task.replace_existing=False
task.save=True
if not unreal.EditorAssetLibrary.does_asset_exist(base+'/Art/T_EndIcon'):
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
icon=unreal.load_asset(base+'/Art/T_EndIcon')
assert icon
icon.set_editor_property('CompressionSettings',unreal.TextureCompressionSettings.TC_EDITOR_ICON)
icon.set_editor_property('LODGroup',unreal.TextureGroup.TEXTUREGROUP_UI)
icon.set_editor_property('MipGenSettings',unreal.TextureMipGenSettings.TMGS_NO_MIPMAPS)
assert unreal.EditorAssetLibrary.save_loaded_asset(icon, False)

result=json.loads(unreal.HodgeUIAuthoringLibrary.configure_reference_hud())
assert result.get('success') or result.get('alreadyConfigured'),result
# 修复早期制作版本的 Slot 对齐，并明确保存 setter 更新的控件树。
for path,name in [(base+'/HUD/WBP_HodgeHUD','ReferenceHUDCanvas'),(base+'/HUD/WBP_PlayerVitals','HealthBar')]:
    bp=unreal.load_asset(path)
    slot=unreal.find_object(unreal.find_object(bp,'WidgetTree'),name).get_editor_property('slot')
    slot.set_horizontal_alignment(unreal.HorizontalAlignment.H_ALIGN_FILL)
    slot.set_vertical_alignment(unreal.VerticalAlignment.V_ALIGN_FILL)
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    assert unreal.EditorAssetLibrary.save_loaded_asset(bp, False)
eas=unreal.load_asset(base+'/EAS_HodgeGameplayUI')
action=next(a for a in eas.get_editor_property('Actions') if a.get_class().get_name()=='GameFeatureAction_AddWidgets')
entries=[row for row in action.get_editor_property('Widgets') if str(row.get_editor_property('SlotID').get_editor_property('TagName'))=='UI.Slot.PlayerVitals']
assert len(entries)==1
action.set_editor_property('Widgets',entries)
assert unreal.EditorAssetLibrary.save_loaded_asset(eas, False)
Path(unreal.Paths.project_saved_dir(),'HUDReference','configuration.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps(result,ensure_ascii=False))
