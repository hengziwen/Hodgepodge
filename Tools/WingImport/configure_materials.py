"""Build editable wing surface and procedural effect materials; preserve FBX slot identities."""
import json
from pathlib import Path
import unreal

ROOT='/Game/Wuwa/Wing'
REPORT=Path('E:/Project/Git/Hodgepodge/Saved/WingImport')
TOOLS=unreal.AssetToolsHelpers.get_asset_tools()
LIB=unreal.MaterialEditingLibrary
records=[]

def make_master(name,blend,unlit=False):
    path=ROOT+'/Materials/'+name
    material=unreal.load_asset(path) or TOOLS.create_asset(name,ROOT+'/Materials',unreal.Material,unreal.MaterialFactoryNew())
    assert material
    LIB.delete_all_material_expressions(material)
    material.set_editor_property('blend_mode',blend)
    material.set_editor_property('shading_model',unreal.MaterialShadingModel.MSM_UNLIT if unlit else unreal.MaterialShadingModel.MSM_DEFAULT_LIT)
    material.set_editor_property('two_sided',True)
    material.set_editor_property('used_with_skeletal_mesh',True)
    def node(cls,**values):
        expr=LIB.create_material_expression(material,cls)
        for key,value in values.items():expr.set_editor_property(key,value)
        return expr
    def connect(a,out,b,pin):
        if pin=='Input':pin=''
        assert LIB.connect_material_expressions(a,out,b,pin),(name,b.get_class().get_name(),pin)
    return material,node,connect

wing_tex=unreal.load_asset(ROOT+'/EDarkNanzhuWingMd10011/Model/T_EDarkNanzhuWingMd10011_D')
weapon_tex=unreal.load_asset(ROOT+'/MB1ZhenwuguanzheMd601_Weapon1/Model/T_MB1ZhenwuguanzheMd00601_Weapon_D')
assert wing_tex and weapon_tex
masters={}
for name,masked,texture in [('M_Wing_Surface',False,wing_tex),('M_Wing_Mask',True,wing_tex),('M_Wing_Weapon',False,weapon_tex)]:
    material,node,link=make_master(name,unreal.BlendMode.BLEND_MASKED if masked else unreal.BlendMode.BLEND_OPAQUE)
    base=node(unreal.MaterialExpressionTextureSampleParameter2D,parameter_name='BaseTexture',texture=texture,sampler_type=unreal.MaterialSamplerType.SAMPLERTYPE_COLOR)
    tint=node(unreal.MaterialExpressionVectorParameter,parameter_name='Tint',default_value=unreal.LinearColor(1,1,1,1))
    color=node(unreal.MaterialExpressionMultiply);link(base,'RGB',color,'A');link(tint,'',color,'B')
    assert LIB.connect_material_property(color,'',unreal.MaterialProperty.MP_BASE_COLOR)
    roughness=node(unreal.MaterialExpressionScalarParameter,parameter_name='Roughness',default_value=.75)
    assert LIB.connect_material_property(roughness,'',unreal.MaterialProperty.MP_ROUGHNESS)
    spec=node(unreal.MaterialExpressionConstant,r=.2)
    assert LIB.connect_material_property(spec,'',unreal.MaterialProperty.MP_SPECULAR)
    emissive=node(unreal.MaterialExpressionScalarParameter,parameter_name='EmissiveStrength',default_value=.12)
    mul=node(unreal.MaterialExpressionMultiply);link(color,'',mul,'A');link(emissive,'',mul,'B')
    assert LIB.connect_material_property(mul,'',unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    if masked:
        # The supplied wing alpha is almost constant; black atlas padding is the available mask.
        mask=node(unreal.MaterialExpressionDesaturation)
        fraction=node(unreal.MaterialExpressionConstant,r=1.0)
        link(fraction,'',mask,'Fraction')
        link(base,'RGB',mask,'Input')
        assert LIB.connect_material_property(mask,'',unreal.MaterialProperty.MP_OPACITY_MASK)
        material.set_editor_property('opacity_mask_clip_value',.01)
    masters[name]=material

material,node,link=make_master('M_Wing_FX',unreal.BlendMode.BLEND_TRANSLUCENT,True)
uv=node(unreal.MaterialExpressionTextureCoordinate)
u=node(unreal.MaterialExpressionComponentMask,r=True);v=node(unreal.MaterialExpressionComponentMask,g=True)
link(uv,'',u,'Input');link(uv,'',v,'Input')
one_minus=node(unreal.MaterialExpressionOneMinus);link(u,'',one_minus,'Input')
edge=node(unreal.MaterialExpressionMultiply);link(u,'',edge,'A');link(one_minus,'',edge,'B')
edge_scale=node(unreal.MaterialExpressionMultiply,const_b=4.0);link(edge,'',edge_scale,'A')
v_inverse=node(unreal.MaterialExpressionOneMinus);link(v,'',v_inverse,'Input')
v_edge=node(unreal.MaterialExpressionMultiply);link(v,'',v_edge,'A');link(v_inverse,'',v_edge,'B')
v_scale=node(unreal.MaterialExpressionMultiply,const_b=4.0);link(v_edge,'',v_scale,'A')
border=node(unreal.MaterialExpressionMultiply);link(edge_scale,'',border,'A');link(v_scale,'',border,'B')
clamp=node(unreal.MaterialExpressionSaturate);link(border,'',clamp,'Input')
power=node(unreal.MaterialExpressionPower)
falloff=node(unreal.MaterialExpressionScalarParameter,parameter_name='EdgeFalloff',default_value=1.5)
link(clamp,'',power,'Base');link(falloff,'',power,'Exp')
gradient=node(unreal.MaterialExpressionMultiply,const_b=.8);link(v,'',gradient,'A')
grad_bias=node(unreal.MaterialExpressionAdd,const_b=.2);link(gradient,'',grad_bias,'A')
soft_mask=node(unreal.MaterialExpressionMultiply);link(power,'',soft_mask,'A');link(grad_bias,'',soft_mask,'B')
time=node(unreal.MaterialExpressionTime)
freq=node(unreal.MaterialExpressionScalarParameter,parameter_name='PulseFrequency',default_value=.5)
speed=node(unreal.MaterialExpressionMultiply);link(time,'',speed,'A');link(freq,'',speed,'B')
sin=node(unreal.MaterialExpressionSine,period=1.0);link(speed,'',sin,'Input')
amplitude=node(unreal.MaterialExpressionScalarParameter,parameter_name='PulseAmplitude',default_value=.1)
pulse=node(unreal.MaterialExpressionMultiply);link(sin,'',pulse,'A');link(amplitude,'',pulse,'B')
pulse_bias=node(unreal.MaterialExpressionAdd,const_b=1.0);link(pulse,'',pulse_bias,'A')
opacity=node(unreal.MaterialExpressionScalarParameter,parameter_name='EffectOpacity',default_value=.16)
opacity_mul=node(unreal.MaterialExpressionMultiply);link(soft_mask,'',opacity_mul,'A');link(opacity,'',opacity_mul,'B')
opacity_pulse=node(unreal.MaterialExpressionMultiply);link(opacity_mul,'',opacity_pulse,'A');link(pulse_bias,'',opacity_pulse,'B')
opacity_clamp=node(unreal.MaterialExpressionSaturate);link(opacity_pulse,'',opacity_clamp,'Input')
assert LIB.connect_material_property(opacity_clamp,'',unreal.MaterialProperty.MP_OPACITY)
tint=node(unreal.MaterialExpressionVectorParameter,parameter_name='EffectColor',default_value=unreal.LinearColor(.12,.035,.65,1))
strength=node(unreal.MaterialExpressionScalarParameter,parameter_name='EmissiveStrength',default_value=2.5)
emissive=node(unreal.MaterialExpressionMultiply);link(tint,'',emissive,'A');link(strength,'',emissive,'B')
glow=node(unreal.MaterialExpressionMultiply);link(emissive,'',glow,'A');link(pulse_bias,'',glow,'B')
assert LIB.connect_material_property(glow,'',unreal.MaterialProperty.MP_EMISSIVE_COLOR)
masters['M_Wing_FX']=material
unreal.EditorAssetLibrary.set_metadata_tag(material,'Hodge.Import.Note','Procedural approximation: original effect shader and textures are absent. UV0 supplies the soft mask.')
for master in masters.values():
    LIB.layout_material_expressions(master);LIB.recompile_material(master)
    assert unreal.EditorAssetLibrary.save_loaded_asset(master,False)
    records.append({'path':master.get_path_name(),'type':'Material','source':'Authored UE material'})

configs=[
 ('EDarkNanzhuWingMd10011/Model/EDarkNanzhuWingMd10011',wing_tex,False),
 ('EDarkNanzhuWingMd10011/Model/test/EDarkNanzhuWingMd10011',unreal.load_asset(ROOT+'/EDarkNanzhuWingMd10011/Model/test/T_EDarkNanzhuWingMd10011_D'),True),
 ('MB1ZhenwuguanzheMd601_Weapon1/Model/MB1ZhenwuguanzheMd601_Weapon1',weapon_tex,False)]
repairs=[]
for relative,texture,is_variant in configs:
    mesh=unreal.load_asset(ROOT+'/'+relative);assert mesh and texture
    slots=list(mesh.materials)
    for index,slot in enumerate(slots):
        name=str(slot.material_slot_name)
        effect=name.startswith('MI_Trans_')
        master=masters['M_Wing_FX' if effect else 'M_Wing_Mask' if name.startswith('MI_Mask_') else 'M_Wing_Weapon' if relative.startswith('MB1') else 'M_Wing_Surface']
        directory=ROOT+'/'+Path(relative).parent.as_posix()+'/Materials'
        instance=unreal.load_asset(directory+'/'+name) or TOOLS.create_asset(name,directory,unreal.MaterialInstanceConstant,unreal.MaterialInstanceConstantFactoryNew())
        LIB.set_material_instance_parent(instance,master)
        LIB.clear_all_material_instance_parameters(instance)
        if not effect:LIB.set_material_instance_texture_parameter_value(instance,'BaseTexture',texture)
        slot.material_interface=instance
        assert unreal.EditorAssetLibrary.save_loaded_asset(instance,False)
        records.append({'path':instance.get_path_name(),'type':'MaterialInstanceConstant','source':relative+'::'+name})
        repairs.append({'mesh':mesh.get_path_name(),'slot_index':index,'slot':name,'material':instance.get_path_name(),
            'parent':master.get_path_name(),'base_texture':None if effect else texture.get_path_name(),
            'effect':effect,'blend':str(master.get_editor_property('blend_mode'))})
    mesh.materials=slots
    assert unreal.EditorAssetLibrary.save_loaded_asset(mesh,False)

results=json.loads((REPORT/'import-results.json').read_text(encoding='utf-8'))
results['materials']=records;results['repairs']=repairs
(REPORT/'import-results.json').write_text(json.dumps(results,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps({'materials':len(records),'slots_configured':len(repairs),'effect_slots':[r['mesh'] for r in repairs if r['effect']]},ensure_ascii=False))
