"""将已有技能UI迁入Designer与Experience扩展点，保留已保存样式和HUD槽位。"""
import json
from pathlib import Path
import unreal

ROOT=Path(unreal.Paths.project_dir())
WORK=Path(globals().get('ABILITY_HUD_WORK',str(ROOT/'Saved/AbilityHUDLayout')))
BASE='/Game/Main/UI'
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
owned={BASE+'/HUD/WBP_HodgeHUD',BASE+'/HUD/WBP_Resonance_Skill',BASE+'/HUD/WBP_Resonance_Energy',BASE+'/EAS_HodgeGameplayUI'}
assert all(p.get_path_name() in owned for p in unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages())
compiled={}
assets=[unreal.load_asset(BASE+'/HUD/'+name) for name in ['WBP_Resonance_Skill','WBP_Resonance_Energy']]
for asset in assets:
    assert unreal.HodgeUIAuthoringLibrary.configure_ability_icon_designer(asset),asset.get_path_name()
    # 修复首轮原型缺失持久字体引用，已有有效用户字体保持不变。
    tree=unreal.find_object(asset,'WidgetTree')
    for name in ['CooldownText','KeyText']:
        text=unreal.find_object(tree,name); font=text.get_editor_property('Font')
        if font.get_editor_property('FontObject') is None:
            font.set_editor_property('FontObject',unreal.load_asset('/Engine/EngineFonts/Roboto'))
            text.set_editor_property('Font',font)
    diagnostic=unreal.HodgeAnimationAuthoringLibrary.compile_animation_blueprint(asset)
    assert diagnostic.startswith('ERRORS=0'),diagnostic
    compiled[asset.get_path_name()]=diagnostic
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset,False)

hud=unreal.load_asset(BASE+'/HUD/WBP_HodgeHUD')
assert unreal.HodgeUIAuthoringLibrary.configure_ability_icon_hud(hud,assets[0].generated_class())
diagnostic=unreal.HodgeAnimationAuthoringLibrary.compile_animation_blueprint(hud)
assert diagnostic.startswith('ERRORS=0'),diagnostic
compiled[hud.get_path_name()]=diagnostic
assert unreal.EditorAssetLibrary.save_loaded_asset(hud,False)

# 复用已有AddWidgets的Client资源加载及LocalPlayer上下文，不再直接构造HUD子实例。
eas=unreal.load_asset(BASE+'/EAS_HodgeGameplayUI')
action=next(item for item in eas.get_editor_property('Actions') if item.get_class().get_name()=='GameFeatureAction_AddWidgets')
entries=list(action.get_editor_property('Widgets'))
for name,asset in zip(['UI.Slot.ResonanceSkill','UI.Slot.ResonanceLiberation'],assets):
    matches=[row for row in entries if str(row.get_editor_property('SlotID').get_editor_property('TagName'))==name]
    assert len(matches)<=1,'重复UI扩展配置：'+name
    if matches:
        assert matches[0].get_editor_property('WidgetClass').get_path_name()==asset.generated_class().get_path_name(),'已有扩展指向其他资产，不覆盖用户配置'
    else:
        tag=unreal.GameplayTag(); assert tag.import_text('(TagName="'+name+'")')
        # 未导出命名Python构造器的原生结构使用已有条目的值复制，不改原条目。
        assert entries,'现有UI Action缺少基础扩展条目，先完成基础HUD装配'
        row=entries[0].copy(); row.set_editor_property('SlotID',tag); row.set_editor_property('WidgetClass',asset.generated_class())
        entries.append(row)
action.set_editor_property('Widgets',entries)
assert unreal.EditorAssetLibrary.save_loaded_asset(eas,False)
result={'compiled':compiled,'extension_count':len(entries),'slots':['UI.Slot.ResonanceSkill','UI.Slot.ResonanceLiberation'],'preserved':'原材质、输入Tag、CDO样式和HUD Canvas偏移'}
(WORK/'migration.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps(result,ensure_ascii=False))
