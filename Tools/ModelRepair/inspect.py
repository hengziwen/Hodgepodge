import unreal,json
p='/Game/CodexText/Model/'
e=unreal.EditorAssetLibrary
m=e.load_asset(p+'R2T1PlayerMaleMd10011')
print('MESH',m.get_path_name())
for s in m.get_editor_property('materials'):
 print('SLOT',str(s.material_slot_name),s.material_interface.get_path_name() if s.material_interface else None)
for path in e.list_assets(p,recursive=True,include_folder=False):
 a=e.load_asset(path)
 if isinstance(a,unreal.MaterialInstanceConstant):
  print('MI',a.get_name(),'PARENT',str(a.get_editor_property('parent')))
  for t in a.get_editor_property('texture_parameter_values'):print('TEX',str(t.parameter_info.name),str(t.parameter_value))
 elif isinstance(a,unreal.Texture2D):
  print('TEXTURE',a.get_name(),'srgb',a.get_editor_property('srgb'),'compression',str(a.get_editor_property('compression_settings')))
