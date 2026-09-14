import unreal,json
E=unreal.EditorAssetLibrary;P='/Game/CodexText/Model/'
names=['MI_R2T1PlayerMaleMd10011'+p for p in ['Hair','Face','Up_2','Down_2','Eyes']]+['MI_3XingStar']
packages=[E.load_asset('/Game/Wuwa/Model/'+n).get_outer() for n in names]
print('SOURCE MEMORY RELOAD',unreal.EditorLoadingAndSavingUtils.reload_packages(packages))
mesh=E.load_asset(P+'R2T1PlayerMaleMd10011')
assert all(s.material_interface.get_path_name().startswith(P) for s in mesh.get_editor_property('materials'))
for n in names:
 mat=E.load_asset(P+n)
 for t in unreal.MaterialEditingLibrary.get_used_textures(mat):assert t.get_path_name().startswith(P),t.get_path_name()
 print('VERIFIED',n)
print('COPY REFERENCES PASS')
