import unreal
import json
from pathlib import Path
root='/Game/Main/Character/Enemies/Guardian'
results=[]
for path in unreal.EditorAssetLibrary.list_assets(root,recursive=True,include_folder=False):
 a=unreal.load_asset(path)
 if isinstance(a,unreal.Blueprint):
  diagnostic=unreal.HodgeAnimationAuthoringLibrary.compile_animation_blueprint(a)
  results.append({'path':path,'diagnostic':diagnostic})
  assert diagnostic.startswith('ERRORS=0'),(path,diagnostic)
  assert unreal.EditorAssetLibrary.save_asset(path,only_if_is_dirty=False)
print(json.dumps(results,ensure_ascii=False))
Path('E:/Project/Git/Hodgepodge/Saved/EnemyAIImplementation/blueprint-results.json').write_text(json.dumps(results,ensure_ascii=False,indent=2),encoding='utf-8')
