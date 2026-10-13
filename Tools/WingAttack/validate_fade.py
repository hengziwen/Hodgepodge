"""只读核对 WING-13 材质接线、参数与原始模型引用。"""
import json
from pathlib import Path
import unreal

WORK = Path('E:/Project/Git/Hodgepodge/Saved/WingFade')
profile = unreal.load_asset('/Game/Main/Character/Hero/Wing/DA_PoverWing_Anzhu')
LIB = unreal.MaterialEditingLibrary
checks = []

def check(name, condition):
    checks.append({'requirement': 'WING-13', 'check': name, 'passed': bool(condition)})
    assert condition, name

check('淡出时长与参数', abs(profile.fade_out_seconds - .35) < .0001 and str(profile.visibility_parameter) == 'WingVisibility')
check('覆盖所有三个材质槽', len(profile.materials) == len(profile.mesh.materials) == 3)
for index, material in enumerate(profile.materials):
    source = profile.mesh.materials[index].material_interface
    master = material.parent
    check('保留原始引用_' + str(index), source.get_path_name().startswith('/Game/Wuwa/Wing/') and source != material)
    check('独立父材质_' + str(index), master != source.parent and master.get_path_name().startswith('/Game/Main/Character/Hero/Wing/Fade/'))
    check('可见度参数_' + str(index), 'WingVisibility' in [str(name) for name in LIB.get_scalar_parameter_names(master)])
    check('默认完整可见_' + str(index), LIB.get_material_instance_scalar_parameter_value(material, 'WingVisibility') == 1.0)
    property_ = unreal.MaterialProperty.MP_OPACITY if index == 0 else unreal.MaterialProperty.MP_OPACITY_MASK
    output = LIB.get_material_property_input_node(master, property_)
    check('透明度或遮罩输出_' + str(index), bool(output))
    check('混合模式_' + str(index), master.get_editor_property('blend_mode') == (unreal.BlendMode.BLEND_TRANSLUCENT if index == 0 else unreal.BlendMode.BLEND_MASKED))
    if index:
        check('保留贴图_' + str(index), LIB.get_material_instance_texture_parameter_value(material, 'BaseTexture') == LIB.get_material_instance_texture_parameter_value(source, 'BaseTexture'))
    else:
        check('保留特效强度', LIB.get_material_instance_scalar_parameter_value(material, 'EffectOpacity') == LIB.get_material_instance_scalar_parameter_value(source, 'EffectOpacity'))

result = {'passed': all(row['passed'] for row in checks), 'checks': checks}
(WORK / 'asset-validation.json').write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps(result, ensure_ascii=False))
