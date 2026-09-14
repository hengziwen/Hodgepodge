import unreal
E=unreal.EditorAssetLibrary;P='/Game/CodexText/Model/'
m=E.load_asset(P+'R2T1PlayerMaleMd10011')
slots=[]
for old in m.get_editor_property('materials'):
 new=unreal.SkeletalMaterial()
 new.set_editor_property('material_slot_name',old.material_slot_name)
 local=E.load_asset(P+str(old.material_slot_name));assert local.get_path_name().startswith(P)
 new.set_editor_property('material_interface',local);slots.append(new)
 local.set_editor_property('used_with_morph_targets',True);unreal.MaterialEditingLibrary.recompile_material(local);E.save_loaded_asset(local,False)
m.set_editor_property('materials',slots)
print('AFTER', [s.material_interface.get_path_name() for s in m.get_editor_property('materials')])
assert all(s.material_interface.get_path_name().startswith(P) for s in m.get_editor_property('materials'))
E.save_loaded_asset(m,False)
