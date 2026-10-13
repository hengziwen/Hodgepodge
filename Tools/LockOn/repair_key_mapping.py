"""只修正本功能的中键映射，不修改其他输入条目。"""
import unreal
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
imc=unreal.load_asset('/Game/Main/Input/IMC_Default');action=unreal.load_asset('/Game/Main/Input/InputAction/IA_LockOn')
rows=[m for m in imc.get_editor_property('Mappings') if m.get_editor_property('Action')==action]
if len(rows)!=1 or rows[0].get_editor_property('Key').export_text()!='MiddleMouseButton':
 imc.unmap_all_keys_from_action(action)
 key=unreal.Key();assert key.import_text('MiddleMouseButton') and key.export_text()=='MiddleMouseButton'
 imc.map_key(action,key);assert unreal.EditorAssetLibrary.save_loaded_asset(imc,False)
print('IMC 中键已确认')
