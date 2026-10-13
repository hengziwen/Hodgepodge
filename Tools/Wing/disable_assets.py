"""Disable formal wing injection and remove only wing hit notifications; retain all resources."""
import json
import shutil
from pathlib import Path

import unreal

PROJECT = Path('E:/Project/Git/Hodgepodge')
WORK = PROJECT / 'Saved/WingDisable'
WORK.mkdir(exist_ok=True)
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
experience = unreal.load_asset('/Game/Main/Experiences/Exp_HodgeDefaultExperience')
montage = unreal.load_asset('/Game/Main/Character/Hero/Anim/Montages/AM_Attack04_Montage')
wing_class = unreal.load_asset('/Game/Main/Character/Hero/Wing/BP_PoverWingComponent').generated_class()
for asset in [experience, montage]:
    source = PROJECT / 'Content' / (asset.get_path_name().split('.')[0].removeprefix('/Game/') + '.uasset')
    backup = WORK / 'BeforeAssets' / source.relative_to(PROJECT)
    if not backup.exists():
        backup.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, backup)

changed = []
cdo = unreal.get_default_object(experience.generated_class())
for action in cdo.get_editor_property('Actions'):
    if action.get_class().get_name() != 'GameFeatureAction_AddComponents':
        continue
    entries = list(action.get_editor_property('ComponentList'))
    matches = [entry for entry in entries if entry.get_editor_property('ComponentClass') == wing_class]
    if not matches:
        continue
    experience.modify()
    cdo.modify()
    action.modify()
    for entry in matches:
        entry.set_editor_property('bClientComponent', False)
        entry.set_editor_property('bServerComponent', False)
        changed.append({'action': action.get_path_name(), 'component': wing_class.get_path_name(),
                        'client': False, 'server': False})
    action.set_editor_property('ComponentList', entries)
assert changed, 'No formal wing injection entry found; do not guess another component.'

before = list(unreal.AnimationLibrary.get_animation_notify_events(montage))
removed = []
for event in before:
    notify = event.notify_state_class or event.notify
    if not isinstance(notify, (unreal.HodgeAnimNotifyState_HitCheck, unreal.HodgeAnimNotify_Hit)):
        continue
    hit = notify.get_editor_property('Hit')
    if hit.source != unreal.HodgeAnimHitSource.NAMED_COMPONENT or str(hit.component_name) != 'WingMesh':
        continue
    name = event.notify_name
    assert str(name) != 'None' and sum(e.notify_name == name for e in before) == 1, 'Notify name is ambiguous.'
    count = unreal.AnimationLibrary.remove_animation_notify_events_by_name(montage, name)
    assert count == 1, count
    removed.append({'notify': notify.get_path_name(), 'name': str(name)})
after = list(unreal.AnimationLibrary.get_animation_notify_events(montage))
assert len(after) == len(before) - len(removed)
assert [e.notify_state_class or e.notify for e in after] == [e.notify_state_class or e.notify for e in before
    if (e.notify_state_class or e.notify).get_path_name() not in {r['notify'] for r in removed}]
unreal.BlueprintEditorLibrary.compile_blueprint(experience)
for asset in [experience, montage]:
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset)
for action in unreal.get_default_object(experience.generated_class()).get_editor_property('Actions'):
    if action.get_class().get_name() == 'GameFeatureAction_AddComponents':
        for entry in action.get_editor_property('ComponentList'):
            if entry.get_editor_property('ComponentClass') == wing_class:
                assert not entry.get_editor_property('bClientComponent') and not entry.get_editor_property('bServerComponent')
result = {'injection': changed, 'removed_wing_notifies': removed, 'remaining_notify_count': len(after),
          'retained_assets': list(unreal.EditorAssetLibrary.list_assets('/Game/Main/Character/Hero/Wing', True, False))}
(WORK / 'disabled.json').write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps(result, ensure_ascii=False))
