"""装配圆形共鸣技能与解放图标，使用UI材质承载纵向冷却和顺时针充能。"""
import json
from pathlib import Path
import unreal

ROOT=Path(unreal.Paths.project_dir())
WORK=ROOT/'Saved/AbilityHUD'
BASE='/Game/Main/UI'
tools=unreal.AssetToolsHelpers.get_asset_tools()
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
owned={BASE+'/HUD/WBP_Resonance_Skill',BASE+'/HUD/WBP_Resonance_Energy',BASE+'/HUD/WBP_HodgeHUD'}
assert all(p.get_path_name() in owned or p.get_path_name().startswith(BASE+'/Art/AbilityIcons/') for p in unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages())


def texture(name,file):
    """导入原创透明纹样，不把带场景背景的截图当作图标贴图。"""
    path=BASE+'/Art/AbilityIcons/'+name
    asset=unreal.load_asset(path)
    if not asset:
        task=unreal.AssetImportTask(); task.filename=str(ROOT/'Tools/AbilityHUD/Art'/file)
        task.destination_path=BASE+'/Art/AbilityIcons'; task.destination_name=name
        task.automated=True; task.save=False; task.replace_existing=False
        tools.import_asset_tasks([task]); asset=unreal.load_asset(path)
    assert asset,path
    asset.set_editor_property('CompressionSettings',unreal.TextureCompressionSettings.TC_EDITOR_ICON)
    asset.set_editor_property('LODGroup',unreal.TextureGroup.TEXTUREGROUP_UI)
    asset.set_editor_property('MipGenSettings',unreal.TextureMipGenSettings.TMGS_NO_MIPMAPS)
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset,False)
    return asset


def material(name,code,inputs,icon=None):
    """每种表现一个可编辑UI材质，MID参数由Widget更新，不累计材质时间。"""
    path=BASE+'/Art/AbilityIcons/'+name
    asset=unreal.load_asset(path)
    if asset and unreal.EditorAssetLibrary.get_metadata_tag(asset,'Hodge.AbilityIcons.Version')=='2':return asset
    asset=asset or tools.create_asset(name,BASE+'/Art/AbilityIcons',unreal.Material,unreal.MaterialFactoryNew())
    unreal.MaterialEditingLibrary.delete_all_material_expressions(asset)
    asset.set_editor_property('MaterialDomain',unreal.MaterialDomain.MD_UI)
    asset.set_editor_property('BlendMode',unreal.BlendMode.BLEND_TRANSLUCENT)
    custom=unreal.MaterialEditingLibrary.create_material_expression(asset,unreal.MaterialExpressionCustom,0,0)
    custom.set_editor_property('Code',code)
    custom.set_editor_property('Description','技能图标：实际GAS进度驱动，不使用独立Time')
    custom.set_editor_property('OutputType',unreal.CustomMaterialOutputType.CMOT_FLOAT4)
    # UE 5.5 的材质输入结构使用属性赋值，不能传构造参数。
    custom_inputs=[]
    for input_name,kind,value in inputs:
        item=unreal.CustomInput()
        item.set_editor_property('InputName',input_name)
        custom_inputs.append(item)
    custom.set_editor_property('Inputs',custom_inputs)
    for index,(parameter,kind,value) in enumerate(inputs):
        if kind=='uv':
            node=unreal.MaterialEditingLibrary.create_material_expression(asset,unreal.MaterialExpressionTextureCoordinate,-500,index*130)
        elif kind=='texture':
            node=unreal.MaterialEditingLibrary.create_material_expression(asset,unreal.MaterialExpressionTextureObjectParameter,-500,index*130)
            node.set_editor_property('ParameterName',parameter); node.set_editor_property('Texture',icon)
        elif kind=='vector':
            node=unreal.MaterialEditingLibrary.create_material_expression(asset,unreal.MaterialExpressionVectorParameter,-500,index*130)
            node.set_editor_property('ParameterName',parameter); node.set_editor_property('DefaultValue',unreal.LinearColor(*value))
        else:
            node=unreal.MaterialEditingLibrary.create_material_expression(asset,unreal.MaterialExpressionScalarParameter,-500,index*130)
            node.set_editor_property('ParameterName',parameter); node.set_editor_property('DefaultValue',value)
        # 向量参数默认只有RGB，拼接Alpha输出以保留颜色透明度。
        if kind=='vector':
            rgba=unreal.MaterialEditingLibrary.create_material_expression(asset,unreal.MaterialExpressionAppendVector,-220,index*130)
            assert unreal.MaterialEditingLibrary.connect_material_expressions(node,'',rgba,'A')
            assert unreal.MaterialEditingLibrary.connect_material_expressions(node,'A',rgba,'B')
            node=rgba
        assert unreal.MaterialEditingLibrary.connect_material_expressions(node,'',custom,parameter)
    assert unreal.MaterialEditingLibrary.connect_material_property(custom,'',unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    alpha=unreal.MaterialEditingLibrary.create_material_expression(asset,unreal.MaterialExpressionComponentMask,220,160)
    alpha.set_editor_property('A',True); alpha.set_editor_property('R',False); alpha.set_editor_property('G',False); alpha.set_editor_property('B',False)
    assert unreal.MaterialEditingLibrary.connect_material_expressions(custom,'',alpha,'')
    assert unreal.MaterialEditingLibrary.connect_material_property(alpha,'',unreal.MaterialProperty.MP_OPACITY)
    unreal.MaterialEditingLibrary.recompile_material(asset)
    unreal.EditorAssetLibrary.set_metadata_tag(asset,'Hodge.AbilityIcons.Version','2')
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset,False)
    return asset


skill_icon=texture('T_ResonanceSkill','resonance-skill.png')
burst_icon=texture('T_ResonanceLiberation','resonance-liberation.png')
icon_code='''// 亮色区域从底部向上恢复，图标与圆底共同受当前CD比例控制。
float r=length(UV-0.5);
float disk=1-smoothstep(0.465,0.485,r);
float4 glyph=Texture2DSample(IconTexture,IconTextureSampler,UV);
float fill=smoothstep(1-saturate(Progress)-0.008,1-saturate(Progress)+0.008,UV.y);
float recover=lerp(1,fill,saturate(CoolingDown));
float bright=lerp(Darkness,1,recover)*lerp(1,0.5,saturate(Unavailable));
float3 bg=lerp(float3(0.02,0.035,0.02),float3(0.15,0.19,0.12),recover);
float border=smoothstep(0.445,0.455,r)*disk;
float3 color=lerp(bg,float3(bright,bright,bright),glyph.a);
color=lerp(color,float3(0.65,0.7,0.58),border*0.16);
return float4(color,max(0.3,glyph.a)*disk);'''
ring_code='''// UV纵轴向下，atan2(x,-y)使12点为零并沿顺时针增长。
float2 p=UV-0.5;
float r=length(p);
float ring=smoothstep(0.415,0.43,r)*(1-smoothstep(0.465,0.485,r));
float angle=frac(atan2(p.x,-p.y)/6.28318530718+1);
float arc=Progress<=0 ? 0 : (Progress>=1 ? 1 : 1-smoothstep(Progress-0.002,Progress+0.002,angle));
return float4(lerp(float3(0.12,0.14,0.09),ColorA.rgb,arc),ring*lerp(0.18,ColorA.a,arc));'''
icon_mat=material('M_AbilityIcon_Cooldown',icon_code,[('UV','uv',None),('IconTexture','texture',None),('Progress','scalar',1.),('CoolingDown','scalar',0.),('Darkness','scalar',.25),('Unavailable','scalar',0.)],skill_icon)
ring_mat=material('M_AbilityEnergy_Clockwise',ring_code,[('UV','uv',None),('Progress','scalar',0.),('ColorA','vector',(1.,.85,.35,1.))])

factory=unreal.WidgetBlueprintFactory(); factory.set_editor_property('ParentClass',unreal.HodgeAbilityIconWidget)
skill=unreal.load_asset(BASE+'/HUD/WBP_Resonance_Skill') or tools.create_asset('WBP_Resonance_Skill',BASE+'/HUD',unreal.WidgetBlueprint,factory)
burst=unreal.load_asset(BASE+'/HUD/WBP_Resonance_Energy')
assert burst
for asset,image,label,input_name,energy in [(skill,skill_icon,'E','InputTag.Ability.ResonanceSkill',False),(burst,burst_icon,'R','InputTag.Ability.ResonanceLiberation',True)]:
    cdo=unreal.get_default_object(asset.generated_class())
    tag=unreal.GameplayTag(); assert tag.import_text('(TagName="'+input_name+'")')
    for key,value in [('InputTag',tag),('bShowEnergyRing',energy),('IconTexture',image),('IconMaterial',icon_mat),('EnergyRingMaterial',ring_mat),('KeyLabel',label),('Caption','共鸣解放' if energy else '共鸣技能'),('IconSize',76.),('FontSize',24),('FillColor',unreal.LinearColor(1.,.85,.35,1.))]:
        cdo.set_editor_property(key,value)
    asset.set_editor_property('BlueprintDescription','共鸣图标：冷却亮色从底到顶恢复，中央倒计时；R额外显示12点顺时针真实能量环。')
    assert unreal.HodgeUIAuthoringLibrary.configure_ability_icon_designer(asset)
    diagnostic=unreal.HodgeAnimationAuthoringLibrary.compile_animation_blueprint(asset)
    assert diagnostic.startswith('ERRORS=0'),diagnostic
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset,False)

hud=unreal.load_asset(BASE+'/HUD/WBP_HodgeHUD')
assert unreal.HodgeUIAuthoringLibrary.configure_ability_icon_hud(hud,skill.generated_class())
diagnostic=unreal.HodgeAnimationAuthoringLibrary.compile_animation_blueprint(hud)
assert diagnostic.startswith('ERRORS=0'),diagnostic
assert unreal.EditorAssetLibrary.save_loaded_asset(hud,False)
# 统一由迁移入口装配Experience，避免重复维护一套直接挂载逻辑。
exec((ROOT/'Tools/AbilityHUD/migrate_layout.py').read_text(encoding='utf-8'),dict(globals(),ABILITY_HUD_WORK=str(WORK)))
report={'widgets':[skill.get_path_name(),burst.get_path_name()], 'materials':[icon_mat.get_path_name(),ring_mat.get_path_name()],
        'textures':[skill_icon.get_path_name(),burst_icon.get_path_name()],'hud':hud.get_path_name(),
        'cooldown':'1 - Remaining/Duration，亮色从底到顶恢复','energy':'0从12点开始，顺时针到100%完整一圈'}
(WORK/'configuration.json').write_text(json.dumps(report,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps(report,ensure_ascii=False))
