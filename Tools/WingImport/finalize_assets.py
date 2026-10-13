"""Persist scoped imports and set animation preview meshes without changing gameplay."""
import unreal,json
from pathlib import Path
ROOT='/Game/Wuwa/Wing'
REPORT=Path('E:/Project/Git/Hodgepodge/Saved/WingImport')
results=json.loads((REPORT/'import-results.json').read_text(encoding='utf-8'))
wing=unreal.load_asset(ROOT+'/EDarkNanzhuWingMd10011/Model/EDarkNanzhuWingMd10011')
weapon=unreal.load_asset(ROOT+'/MB1ZhenwuguanzheMd601_Weapon1/Model/MB1ZhenwuguanzheMd601_Weapon1')
for item in results['imported']+results['materials']:
    asset=unreal.load_asset(item['path']);assert asset
    if isinstance(asset,unreal.AnimSequence):
        asset.set_preview_skeletal_mesh(weapon if item['source'].startswith('MB1') else wing)
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset,False),item['path']
print('Saved all 66 imported/authored assets and animation preview mesh assignments.')
