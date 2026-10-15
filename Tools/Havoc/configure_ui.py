"""在编辑器制作暗流Designer树及血条上方扩展点，不自动授予技能或修改EAS。"""
import json
from pathlib import Path
import unreal

root = Path(unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir()))
work = root / "Saved/Havoc"
work.mkdir(parents=True, exist_ok=True)
# 原生工具保留既有控件树和布局，报告包含蓝图编译错误。
result = json.loads(unreal.HodgeUIAuthoringLibrary.configure_dark_flow_hud())
(work / "ui-authoring-result.json").write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding="utf-8")
print(json.dumps(result, ensure_ascii=False))
assert result.get("success"), result
