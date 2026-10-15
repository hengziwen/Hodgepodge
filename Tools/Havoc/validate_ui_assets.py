"""只读核对已保存的暗流控件树与HUD扩展点，不代替PIE注入和运行时数据验证。"""
import json
from pathlib import Path
import unreal

root = Path(unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir()))
widget = unreal.load_asset("/Game/Main/UI/HUD/WBP_DarkFlow")
hud = unreal.load_asset("/Game/Main/UI/HUD/WBP_HodgeHUD")
assert widget and hud, "缺少正式Designer资产"
tree = unreal.find_object(widget, "WidgetTree")
hud_tree = unreal.find_object(hud, "WidgetTree")
assert tree and hud_tree, "缺少已保存WidgetTree"
bar = unreal.find_object(tree, "DarkFlowBar")
label = unreal.find_object(tree, "DarkFlowLabel")
point = unreal.find_object(hud_tree, "DarkFlowPoint")
vitals = unreal.find_object(hud_tree, "PlayerVitalsPoint")
assert bar and label and point and vitals, "缺少正式控件或扩展点"
assert point.get_parent().get_name() == "ReferenceHUDCanvas"
# 比较同一父Canvas的布局坐标，不能用截图像素当成跨分辨率锚点。
position = point.slot.get_position()
life_position = vitals.slot.get_position()
assert abs(position.x - life_position.x) < 0.01 and position.y < life_position.y
tag = point.get_editor_property("extension_point_tag").export_text()
assert "UI.Slot.DarkFlow" in tag
result = {
    "asset": widget.get_path_name(),
    "widget_types": {"bar": bar.get_class().get_name(), "label": label.get_class().get_name(),
                     "point": point.get_class().get_name()},
    "point_position": [position.x, position.y],
    "vitals_position": [life_position.x, life_position.y],
    "extension_point_tag": tag,
    "designer_structure_passed": True,
    "runtime_injection": "本脚本只验证Designer结构；运行时注入结果另见PIE报告",
}
(root / "Saved/Havoc/ui-designer-validation.json").write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding="utf-8")
print(json.dumps(result, ensure_ascii=False))
