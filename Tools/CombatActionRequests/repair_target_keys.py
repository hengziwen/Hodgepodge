"""Repair only generated Guardian target key contracts; preserve tree layout and weapon sockets."""
import unreal,json
from pathlib import Path
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
root='/Game/Main/Character/Enemies/Guardian'
pairs=[(root+'/AI/BT_Guardian',root+'/AI/BB_Guardian'),
       (root+'/Validation/BT_EnemyCombo',root+'/Validation/BB_EnemyCombo')]
results=[]
for tree_path,bb_path in pairs:
    if not unreal.EditorAssetLibrary.does_asset_exist(tree_path):continue
    tree=unreal.load_asset(tree_path);bb=unreal.load_asset(bb_path)
    key=next(e.get_editor_property('KeyType') for e in bb.get_editor_property('Keys') if str(e.get_editor_property('EntryName'))=='TargetActor')
    assert key.get_class().get_name()=='BlackboardKeyType_Object'
    key.set_editor_property('BaseClass',unreal.Actor.static_class())
    targets=[]
    sample=next((t for n in range(16) if (t:=unreal.find_object(tree,'HodgeBTTask_ExecuteAbility_'+str(n)))),None)
    assert sample,tree_path
    for task in unreal.ObjectIterator(type(sample)):
        if task.get_class().get_name()!='HodgeBTTask_ExecuteAbility':continue
        if not task.get_path_name().startswith(tree.get_path_name()+':'):continue
        selector=task.get_editor_property('TargetActor')
        old=str(selector.get_editor_property('SelectedKeyName'))
        # 本脚本仅修复明确的自动回退，不覆盖用户另选的合法目标键。
        if old in ['SelfActor','None','TargetActor']:
            selector.set_editor_property('SelectedKeyName','TargetActor')
            task.set_editor_property('TargetActor',selector)
            targets.append({'task':task.get_path_name(),'old':old,'new':'TargetActor'})
        else:
            targets.append({'task':task.get_path_name(),'old':old,'new':old,'preserved':True})
    assert targets,tree_path
    assert unreal.EditorAssetLibrary.save_loaded_asset(bb,False)
    assert unreal.EditorAssetLibrary.save_loaded_asset(tree,False)
    results.append({'tree':tree_path,'blackboard':bb_path,'tasks':targets})
Path(unreal.Paths.project_saved_dir(),'AIRequestRegression','asset-repair.json').write_text(json.dumps(results,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps(results,ensure_ascii=False))
