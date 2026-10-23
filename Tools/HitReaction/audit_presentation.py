"""只读采样正式五段和受击素材，保存本轮独立基线，不改资产。"""
import json
from pathlib import Path

import unreal

OUT = Path(unreal.Paths.project_saved_dir()) / 'HitPresentation' / 'Before'
OUT.mkdir(parents=True, exist_ok=True)
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
report = {'project': unreal.Paths.get_project_file_path(), 'attacks': [], 'profiles': {}, 'poses': {}, 'errors': []}


def export(asset):
    """保护字段用引擎文本导出，不将反射访问失败当成空配置。"""
    task = unreal.AssetExportTask()
    task.object = asset
    task.filename = str(OUT / (asset.get_name() + '.copy'))
    task.automated = True
    task.prompt = False
    task.replace_identical = True
    task.exporter = unreal.ObjectExporterT3D()
    assert unreal.Exporter.run_asset_export_task(task), asset.get_path_name()


for number in [1, 2, 3, 5, 4]:
    attack = unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_' + str(number))
    assert attack
    config = attack.get_editor_property('DefaultHitConfig')
    montage = attack.get_editor_property('ExecutionConfig').get_editor_property('Montage')
    export(attack)
    export(montage)
    events = unreal.AnimationLibrary.get_animation_notify_events(montage)
    report['attacks'].append({'number': number, 'asset': attack.get_path_name(), 'hit_config': config.export_text(),
                              'montage': montage.get_path_name(), 'length': montage.get_play_length(),
                              'notifies': [event.export_text() for event in events]})

for path in ['/Game/Main/Character/Hero/Anim/HitReactions/DA_Hero_HitReaction',
             '/Game/Main/Character/Enemies/Guardian/DA_Guardian_HitReaction']:
    profile = unreal.load_asset(path)
    assert profile
    export(profile)
    report['profiles'][path] = str(profile.get_editor_property('Animations'))
    for entry in profile.get_editor_property('Animations'):
        montage = entry.get_editor_property('Montage')
        if montage:
            export(montage)

sequence = unreal.load_asset('/Game/Main/Character/Hero/Anim/HitReactions/A_Hero_LightFeedback')
assert sequence
export(sequence)
tracks = unreal.AnimationLibrary.get_animation_track_names(sequence)
report['tracks'] = [str(name) for name in tracks]
for bone in tracks:
    if not any(key in str(bone).lower() for key in ['spine', 'head', 'clavicle', 'pelvis']):
        continue
    samples = []
    for frame in range(round(sequence.get_play_length() * 60) + 1):
        time = min(frame / 60.0, sequence.get_play_length())
        pose = unreal.AnimationLibrary.get_bone_pose_for_time(sequence, bone, time, False)
        rotation = pose.rotation.rotator()
        samples.append({'time': time, 'rotation': [rotation.pitch, rotation.yaw, rotation.roll],
                        'translation': [pose.translation.x, pose.translation.y, pose.translation.z]})
    report['poses'][str(bone)] = samples

anim = unreal.load_asset('/Game/Main/Character/Hero/Anim/ABP_Pover_Base')
export(anim)
export(unreal.get_default_object(anim.generated_class()))
table = unreal.load_asset('/Game/Main/Data/Combo/DT_LightCombo')
report['combo_rows'] = unreal.DataTableFunctionLibrary.export_data_table_to_json_string(table)
(OUT / 'audit.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
print('PRESENTATION_AUDIT ' + str(OUT) + ' attacks=' + str(len(report['attacks'])) + ' bones=' + str(len(report['poses'])))
