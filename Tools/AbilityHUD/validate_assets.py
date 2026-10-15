"""只校验本轮图标资产和蓝图诊断，避免资产存在但正式HUD未接线。"""
import json
from pathlib import Path
import unreal

BASE='/Game/Main/UI'
checks=[]


def check(name,condition,detail=None):
    """任何引用、参数或原生蓝图编译失败均阻止本轮资产验收。"""
    checks.append({'case':name,'passed':bool(condition),'detail':detail})
    assert condition,(name,detail)


assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
icon=unreal.load_asset(BASE+'/Art/AbilityIcons/M_AbilityIcon_Cooldown')
ring=unreal.load_asset(BASE+'/Art/AbilityIcons/M_AbilityEnergy_Clockwise')
for material in [icon,ring]:
    check('材质使用UI域与透明混合',material and material.get_editor_property('MaterialDomain')==unreal.MaterialDomain.MD_UI and material.get_editor_property('BlendMode')==unreal.BlendMode.BLEND_TRANSLUCENT,material.get_path_name() if material else None)
    check('材质保留Progress参数','Progress' in [str(name) for name in unreal.MaterialEditingLibrary.get_scalar_parameter_names(material)])
for name,tag,key,energy in [('WBP_Resonance_Skill','InputTag.Ability.ResonanceSkill','E',False),('WBP_Resonance_Energy','InputTag.Ability.ResonanceLiberation','R',True)]:
    asset=unreal.load_asset(BASE+'/HUD/'+name)
    cdo=unreal.get_default_object(asset.generated_class())
    check(name+'保留正式输入Tag',str(cdo.get_editor_property('InputTag').get_editor_property('TagName'))==tag)
    check(name+'绑定正确材质、纹样、键帽和能量环',cdo.get_editor_property('IconMaterial')==icon and cdo.get_editor_property('EnergyRingMaterial')==ring and cdo.get_editor_property('IconTexture') is not None and str(cdo.get_editor_property('KeyLabel'))==key and cdo.get_editor_property('bShowEnergyRing')==energy)
    designer=unreal.find_object(asset,'WidgetTree')
    for child in ['IconCanvas','SkillIcon','EnergyRing','CooldownText','KeyText','AbilityButton']:
        check(name+'保存Designer控件'+child,unreal.find_object(designer,child) is not None)
    for child in ['CooldownText','KeyText']:
        check(name+'保存持久字体'+child,unreal.find_object(designer,child).get_editor_property('Font').get_editor_property('FontObject') is not None)
    diagnostic=unreal.HodgeAnimationAuthoringLibrary.compile_animation_blueprint(asset)
    check(name+'蓝图编译',diagnostic.startswith('ERRORS=0'),diagnostic)
    # 编译可能标记当前目标包，保存本次已验证的目标，绝不批量重存。
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset,False)
hud=unreal.load_asset(BASE+'/HUD/WBP_HodgeHUD')
tree=unreal.find_object(hud,'WidgetTree')
for name,position in [('ResonanceSkill',(-196.,-24.)),('ResonanceEnergy',(-48.,-24.))]:
    widget=unreal.find_object(tree,name)
    expected='UI.Slot.ResonanceSkill' if name=='ResonanceSkill' else 'UI.Slot.ResonanceLiberation'
    check(name+'是Layout扩展点',isinstance(widget,unreal.UIExtensionPointWidget) and str(widget.get_editor_property('ExtensionPointTag').get_editor_property('TagName'))==expected)
    slot=widget.get_editor_property('Slot')
    offset=slot.get_position(); size=slot.get_size()
    check(name+'正式HUD槽位',(offset.x,offset.y)==position and (size.x,size.y)==(76.,100.))
diagnostic=unreal.HodgeAnimationAuthoringLibrary.compile_animation_blueprint(hud)
check('HUD蓝图编译',diagnostic.startswith('ERRORS=0'),diagnostic)
assert unreal.EditorAssetLibrary.save_loaded_asset(hud,False)
# Experience配置是注入事实来源，不能用HUD内的硬引用子控件代替。
eas=unreal.load_asset(BASE+'/EAS_HodgeGameplayUI')
action=next(item for item in eas.get_editor_property('Actions') if item.get_class().get_name()=='GameFeatureAction_AddWidgets')
entries=list(action.get_editor_property('Widgets'))
for tag_name,class_name in [('UI.Slot.ResonanceSkill','WBP_Resonance_Skill_C'),('UI.Slot.ResonanceLiberation','WBP_Resonance_Energy_C')]:
    matches=[row for row in entries if str(row.get_editor_property('SlotID').get_editor_property('TagName'))==tag_name]
    check(tag_name+'由Experience唯一注入',len(matches)==1 and matches[0].get_editor_property('WidgetClass').get_name()==class_name)
result={'passed':all(row['passed'] for row in checks),'checks':checks}
work=Path(globals().get('ABILITY_HUD_WORK',str(Path(unreal.Paths.project_saved_dir())/'AbilityHUD')))
(work/'assets-results.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps({'passed':result['passed'],'checks':len(checks)},ensure_ascii=False))
