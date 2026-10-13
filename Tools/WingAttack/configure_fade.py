"""为正式翅膀复制淡出材质，仅配置本功能的 Profile，不改导入模型和原材质。"""
import json
from pathlib import Path
import unreal

ROOT = '/Game/Main/Character/Hero/Wing/Fade'
WORK = Path('E:/Project/Git/Hodgepodge/Saved/WingFade')
LIB = unreal.MaterialEditingLibrary
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
profile = unreal.load_asset('/Game/Main/Character/Hero/Wing/DA_PoverWing_Anzhu')
dither_function = unreal.load_asset('/Engine/Functions/Engine_MaterialFunctions02/Utility/DitherTemporalAA')
assert profile and dither_function
materials = []
records = []

for index, slot in enumerate(profile.mesh.materials):
    source_instance = slot.material_interface
    source_master = source_instance.parent
    master_path = ROOT + '/' + source_master.get_name() + '_Fade'
    instance_path = ROOT + '/' + source_instance.get_name() + '_Fade'
    master = unreal.load_asset(master_path)
    if not master:
        master = unreal.EditorAssetLibrary.duplicate_asset(source_master.get_path_name(), master_path)
        assert master
        visibility = LIB.create_material_expression(master, unreal.MaterialExpressionScalarParameter)
        visibility.set_editor_property('parameter_name', 'WingVisibility')
        visibility.set_editor_property('default_value', 1.0)
        transparent = master.get_editor_property('blend_mode') == unreal.BlendMode.BLEND_TRANSLUCENT
        property_ = unreal.MaterialProperty.MP_OPACITY if transparent else unreal.MaterialProperty.MP_OPACITY_MASK
        original = LIB.get_material_property_input_node(master, property_)
        original_output = LIB.get_material_property_input_node_output_name(master, property_)
        if transparent:
            output = visibility
        else:
            master.set_editor_property('blend_mode', unreal.BlendMode.BLEND_MASKED)
            output = LIB.create_material_expression(master, unreal.MaterialExpressionMaterialFunctionCall)
            assert output.set_material_function(dither_function)
            pins = LIB.get_material_expression_input_names(output)
            alpha_pin = next(name for name in pins if 'Alpha' in name)
            assert LIB.connect_material_expressions(visibility, '', output, alpha_pin)
        if original:
            combined = LIB.create_material_expression(master, unreal.MaterialExpressionMultiply)
            assert LIB.connect_material_expressions(original, original_output, combined, 'A')
            assert LIB.connect_material_expressions(output, '', combined, 'B')
            output = combined
        assert LIB.connect_material_property(output, '', property_)
        LIB.layout_material_expressions(master)
        LIB.recompile_material(master)
    assert 'WingVisibility' in [str(name) for name in LIB.get_scalar_parameter_names(master)]
    instance = unreal.load_asset(instance_path)
    if not instance:
        instance = unreal.EditorAssetLibrary.duplicate_asset(source_instance.get_path_name(), instance_path)
    assert instance
    LIB.set_material_instance_parent(instance, master)
    LIB.set_material_instance_scalar_parameter_value(instance, 'WingVisibility', 1.0)
    LIB.update_material_instance(instance)
    for asset in [master, instance]:
        assert unreal.EditorAssetLibrary.save_loaded_asset(asset, False)
    materials.append(instance)
    records.append({'slot': index, 'source': source_instance.get_path_name(), 'master': master.get_path_name(),
                    'material': instance.get_path_name(), 'blend': str(master.get_editor_property('blend_mode'))})

profile.set_editor_property('materials', materials)
profile.set_editor_property('fade_out_seconds', .35)
profile.set_editor_property('visibility_parameter', 'WingVisibility')
assert unreal.EditorAssetLibrary.save_loaded_asset(profile, False)
result = {'fade_seconds': .35, 'parameter': 'WingVisibility', 'materials': records}
(WORK / 'materials.json').write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps(result, ensure_ascii=False))
