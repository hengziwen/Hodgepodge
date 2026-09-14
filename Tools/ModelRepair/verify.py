import unreal
P='/Game/CodexText/Model/'
m=unreal.EditorAssetLibrary.load_asset(P+'R2T1PlayerMaleMd10011')
for s in m.get_editor_property('materials'):
 assert s.material_interface.get_path_name().startswith(P)
 print(s.material_slot_name,s.material_interface.get_path_name())
