"""校准本轮原型曲线的离散采样偏差，保留初版并保持攻击的期望距离不变。"""
from pathlib import Path
import json
import unreal

assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
root = '/Game/Tests/Combat/ACTPrototype'
curve = unreal.load_asset(root + '/CF_ACT_Knockback')
assert curve and abs(curve.get_float_value(0.0) - 1.0) < .001, '只允许校准本轮初版，不覆盖后续手调'
tools = unreal.AssetToolsHelpers.get_asset_tools()
backup_path = root + '/Backups/CF_ACT_Knockback_Initial'
assert not unreal.EditorAssetLibrary.does_asset_exist(backup_path)
backup = tools.duplicate_asset('CF_ACT_Knockback_Initial', root + '/Backups', curve)
assert backup and unreal.EditorAssetLibrary.save_loaded_asset(backup, False)
# 首尾同为零使离散采样误差更小，峰值在移动开始后的极短区间内建立。
keys = [(0, 0), (.08, 1), (.12, .9), (.4, .35), (.75, .07), (1, 0)]
assert unreal.HodgeAnimationAuthoringLibrary.configure_act_knockback_curve(curve, [unreal.Vector2D(x, y) for x, y in keys])
assert unreal.EditorAssetLibrary.save_loaded_asset(curve, False)
output = Path(unreal.Paths.project_saved_dir()) / 'ACTCombat' / 'curve-calibration.json'
output.write_text(json.dumps({'backup': backup_path, 'keys': keys, 'reason': 'first PIE 150cm request measured 161.3cm; endpoint sampling correction, distance unchanged'}, indent=2), encoding='utf-8')
