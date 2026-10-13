"""只更新本次软镜头参数，保存修改前正式 Profile；不重复制作其他资产。"""
import unreal,json,shutil
from pathlib import Path
work=Path(globals().get('LOCK_ON_WORK','E:/Project/Git/Hodgepodge/Saved/LockOnTracking'))
(work/'Before').mkdir(parents=True,exist_ok=True)
asset_path='/Game/Main/Character/Hero/Targeting/DA_HeroLockOn'
file=Path('E:/Project/Git/Hodgepodge/Content/Main/Character/Hero/Targeting/DA_HeroLockOn.uasset')
backup=work/'Before/DA_HeroLockOn.uasset'
if not backup.exists():shutil.copy2(file,backup)
profile=unreal.load_asset(asset_path)
assert profile
settings={'bEnableSoftCameraAssist':True,'bEnableMouseFlickSwitch':True,'CameraManualHold':.15,
          'CameraAcquireDuration':.35,'CameraAcquireSpeed':720.,
          'CameraScreenDeadZone':unreal.Vector2D(.12,.16),'CameraScreenSoftZone':unreal.Vector2D(.6,.65),
          'CameraYawStrength':5.,'CameraPitchStrength':2.,'CameraMouseSuppression':1.,
          'CameraAssistRecoveryTime':.35,'CameraMinTargetDistance':100.,
          'CameraYawSpeed':120.,'CameraPitchSpeed':60.}
for key,value in settings.items():profile.set_editor_property(key,value)
assert unreal.EditorAssetLibrary.save_loaded_asset(profile,False)
settings={k:([v.x,v.y] if isinstance(v,unreal.Vector2D) else v) for k,v in settings.items()}
(work/'soft-camera-config.json').write_text(json.dumps({'asset':asset_path,'settings':settings},indent=2),encoding='utf-8')
print(json.dumps(settings))
