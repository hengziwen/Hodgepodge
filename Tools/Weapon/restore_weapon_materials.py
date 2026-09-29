"""Rebuild imported weapon materials without requiring the original custom Toon shader."""
import unreal

ROOT='/Game/Wuwa/Weapon'
DEST=ROOT+'/Materials'
tools=unreal.AssetToolsHelpers.get_asset_tools()
lib=unreal.MaterialEditingLibrary
unreal.EditorAssetLibrary.make_directory(DEST)

def asset(name, cls, factory):
    return unreal.load_asset(DEST+'/'+name) if unreal.EditorAssetLibrary.does_asset_exist(DEST+'/'+name) else tools.create_asset(name,DEST,cls,factory)

# Preserve the original packed source and its other users; decode XY from a linear copy.
packed_path=DEST+'/T_Weapon_PackedLinear'
packed=unreal.load_asset(packed_path) if unreal.EditorAssetLibrary.does_asset_exist(packed_path) else unreal.EditorAssetLibrary.duplicate_asset(ROOT+'/T_R5Knife505Md20001_N',packed_path)
packed.set_editor_property('srgb',False)
packed.set_editor_property('compression_settings',unreal.TextureCompressionSettings.TC_MASKS)
material=asset('M_Weapon_Restored',unreal.Material,unreal.MaterialFactoryNew())
lib.delete_all_material_expressions(material)
material.set_editor_property('two_sided',True)
material.set_editor_property('used_with_skeletal_mesh',True)
material.set_editor_property('blend_mode',unreal.BlendMode.BLEND_OPAQUE)

def node(cls,x,y,**props):
    obj=lib.create_material_expression(material,cls,x,y)
    for key,value in props.items():obj.set_editor_property(key,value)
    return obj

def link(a,out,b,inp):assert lib.connect_material_expressions(a,out,b,inp)
def output(a,out,p):assert lib.connect_material_property(a,out,p)
base=node(unreal.MaterialExpressionTextureSampleParameter2D,-650,-300,parameter_name='BaseColorTexture',texture=unreal.load_asset(ROOT+'/T_R5Sword506Md20001_D'))
output(base,'RGB',unreal.MaterialProperty.MP_BASE_COLOR)
normal=node(unreal.MaterialExpressionTextureSampleParameter2D,-1000,150,parameter_name='PackedNormalTexture',texture=packed,sampler_type=unreal.MaterialSamplerType.SAMPLERTYPE_MASKS)
mask=node(unreal.MaterialExpressionComponentMask,-780,150,r=True,g=True,b=False,a=False)
link(normal,'RGB',mask,'')
scale=node(unreal.MaterialExpressionMultiply,-580,150,const_b=2.0)
link(mask,'',scale,'A')
bias=node(unreal.MaterialExpressionSubtract,-400,150,const_b=1.0)
link(scale,'',bias,'A')
derive=node(unreal.MaterialExpressionDeriveNormalZ,-210,150)
link(bias,'',derive,'')
output(derive,'',unreal.MaterialProperty.MP_NORMAL)
for i,(name,value,prop) in enumerate([('Roughness',0.45,unreal.MaterialProperty.MP_ROUGHNESS),('Metallic',0.65,unreal.MaterialProperty.MP_METALLIC),('Specular',0.35,unreal.MaterialProperty.MP_SPECULAR)]):
    p=node(unreal.MaterialExpressionScalarParameter,-400,400+i*100,parameter_name=name,default_value=value)
    output(p,'',prop)
lib.recompile_material(material)
for mesh_name,instance_name in [('Sword_Qiuyuan','MI_R5Sword506Md20001Effect'),('Sword_Qiuyuan1','MI_R5Sword506Md20001')]:
    instance=asset(instance_name,unreal.MaterialInstanceConstant,unreal.MaterialInstanceConstantFactoryNew())
    lib.set_material_instance_parent(instance,material)
    lib.update_material_instance(instance)
    mesh=unreal.load_asset(ROOT+'/'+mesh_name)
    slots=list(mesh.get_editor_property('materials'))
    assert len(slots)==1 and str(slots[0].material_slot_name)==instance_name
    slots[0].material_interface=instance
    mesh.set_editor_property('materials',slots)
    assert unreal.EditorAssetLibrary.save_loaded_asset(instance)
    assert unreal.EditorAssetLibrary.save_loaded_asset(mesh)
    print('ASSIGNED',mesh.get_path_name(),instance.get_path_name())
assert unreal.EditorAssetLibrary.save_loaded_asset(packed)
assert unreal.EditorAssetLibrary.save_loaded_asset(material)
print('STATS',lib.get_statistics(material))
unreal.get_editor_subsystem(unreal.AssetEditorSubsystem).open_editor_for_assets([unreal.load_asset(ROOT+'/Sword_Qiuyuan1')])

