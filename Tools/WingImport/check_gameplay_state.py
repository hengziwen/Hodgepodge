"""Read the previous wing opt-out and ensure this import did not activate gameplay."""
import unreal,json
from pathlib import Path
experience=unreal.load_asset('/Game/Main/Experiences/Exp_HodgeDefaultExperience')
wing_class=unreal.load_asset('/Game/Main/Character/Hero/Wing/BP_PoverWingComponent').generated_class()
entries=[]
for action in unreal.get_default_object(experience.generated_class()).get_editor_property('Actions'):
    if action.get_class().get_name()!='GameFeatureAction_AddComponents':continue
    for entry in action.get_editor_property('ComponentList'):
        if entry.get_editor_property('ComponentClass')==wing_class:
            enabled=entry.get_editor_property('bClientComponent') or entry.get_editor_property('bServerComponent')
            assert not enabled,'Existing wing gameplay must remain disabled.'
            entries.append({'client':False,'server':False,'component':wing_class.get_path_name()})
assert entries
result={'wing_gameplay_disabled':True,'entries':entries,
        'dirty_content':[p.get_path_name() for p in unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()],
        'dirty_maps':[p.get_path_name() for p in unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()]}
Path('E:/Project/Git/Hodgepodge/Saved/WingImport/gameplay-state.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps(result,ensure_ascii=False))
