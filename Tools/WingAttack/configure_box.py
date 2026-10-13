"""将现有第 5 段翼命中改为跟随翼根的 Box，保存测量与原始资产备份。"""
import json
import shutil
from pathlib import Path
import unreal

ROOT = Path('E:/Project/Git/Hodgepodge')
WORK = ROOT / 'Saved/WingBox'
WORK.mkdir(exist_ok=True)
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
paths = ['Content/Main/Character/Hero/Wing/DA_PoverWing_Anzhu.uasset',
         'Content/Main/Character/Hero/Anim/Montages/AM_Attack04_Montage.uasset']
for name in paths:
    backup = WORK / 'backup' / name
    backup.parent.mkdir(parents=True, exist_ok=True)
    if not backup.exists():
        shutil.copy2(ROOT / name, backup)
montage = unreal.load_asset('/Game/Main/Character/Hero/Anim/Montages/AM_Attack04_Montage')
profile = unreal.load_asset('/Game/Main/Character/Hero/Wing/DA_PoverWing_Anzhu')
before = len(unreal.AnimationLibrary.get_animation_notify_events(montage))
measurement = unreal.HodgeAnimationAuthoringLibrary.configure_wing_box_hit(montage, profile, 'Bone_Wing000', 4.0)
assert measurement.startswith('{'), measurement
result = json.loads(measurement)
assert len(unreal.AnimationLibrary.get_animation_notify_events(montage)) == before
for asset in [montage, profile]:
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset, False)
result.update({'montage': montage.get_path_name(), 'profile': profile.get_path_name(), 'anchor': 'Bone_Wing000',
               'component': 'WingMesh', 'shape': 'Box', 'authority_sampling': profile.enable_authority_hit_sampling,
               'notify_count': before, 'display_window_changed': False, 'fade_seconds': profile.fade_out_seconds})
(WORK / 'configuration.json').write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps(result, ensure_ascii=False))
