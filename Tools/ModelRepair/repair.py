import unreal,json
E=unreal.EditorAssetLibrary; L=unreal.MaterialEditingLibrary
P='/Game/CodexText/Model/'
parts=[('Hair',.72),('Face',.7),('Up_2',.62),('Down_2',.68),('Eyes',.26)]
for part,rough in parts:
 m=E.load_asset(P+'MI_R2T1PlayerMaleMd10011'+part)
 assert isinstance(m,unreal.Material)
 L.delete_all_material_expressions(m)
 m.set_editor_property('blend_mode',unreal.BlendMode.BLEND_OPAQUE)
 m.set_editor_property('two_sided',part=='Hair')
 m.set_editor_property('shading_model',unreal.MaterialShadingModel.MSM_DEFAULT_LIT)
 m.set_editor_property('used_with_skeletal_mesh',True)
 t=E.load_asset(P+'T_R2T1PlayerMaleMd10011'+part.replace('_2','')+'_D')
 n=L.create_material_expression(m,unreal.MaterialExpressionTextureSample,-600,0);n.set_editor_property('texture',t)
 L.connect_material_property(n,'RGB',unreal.MaterialProperty.MP_BASE_COLOR)
 r=L.create_material_expression(m,unreal.MaterialExpressionConstant,-250,180);r.set_editor_property('r',rough)
 L.connect_material_property(r,'',unreal.MaterialProperty.MP_ROUGHNESS)
 if part in ('Up_2','Down_2'):
  nt=E.load_asset(P+'T_R2T1PlayerMaleMd10011'+part.replace('_2','')+'_N')
  nt.set_editor_property('srgb',False)
  nt.set_editor_property('compression_settings',unreal.TextureCompressionSettings.TC_NORMALMAP)
  assert E.save_loaded_asset(nt,False)
  nn=L.create_material_expression(m,unreal.MaterialExpressionTextureSample,-600,350);nn.set_editor_property('texture',nt);nn.set_editor_property('sampler_type',unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL)
  L.connect_material_property(nn,'RGB',unreal.MaterialProperty.MP_NORMAL)
 L.recompile_material(m)
 assert E.save_loaded_asset(m,False)
 print('REPAIRED',m.get_path_name())
# No corresponding star texture exists in the supplied copy: use an explicit gold surface.
m=E.load_asset(P+'MI_3XingStar');L.delete_all_material_expressions(m)
m.set_editor_property('used_with_skeletal_mesh',True)
c=L.create_material_expression(m,unreal.MaterialExpressionConstant3Vector,-300,0);c.set_editor_property('constant',unreal.LinearColor(.65,.4,.1,1));L.connect_material_property(c,'',unreal.MaterialProperty.MP_BASE_COLOR)
r=L.create_material_expression(m,unreal.MaterialExpressionConstant,-300,150);r.set_editor_property('r',.5);L.connect_material_property(r,'',unreal.MaterialProperty.MP_ROUGHNESS)
L.recompile_material(m);assert E.save_loaded_asset(m,False)
mesh=E.load_asset(P+'R2T1PlayerMaleMd10011')
slots=[]
for old in mesh.get_editor_property('materials'):
 slot=old.copy()
 local=E.load_asset(P+str(slot.material_slot_name))
 assert local and local.get_path_name().startswith(P)
 slot.set_editor_property('material_interface',local)
 slots.append(slot)
mesh.set_editor_property('materials',slots)
assert all(s.material_interface.get_path_name().startswith(P) for s in mesh.get_editor_property('materials'))
assert E.save_loaded_asset(mesh,False)
print('ALL SIX SLOTS NOW LOCAL')
