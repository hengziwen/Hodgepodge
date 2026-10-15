"""只读导出暗涌依赖的正式资产、通知和源动画采样，不修改共享资产。"""
import json
from pathlib import Path
import unreal

root = Path(unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir()))
work = root / 'Saved/Havoc'
paths = [
    '/Game/Main/Data/Combo/DA_LightCombo', '/Game/Main/Data/AbilitySet/DA_Pover',
    '/Game/Main/UI/EAS_HodgeGameplayUI',
    '/Game/Main/Character/Hero/Ability/ResonanceSkill/DA_Resonance_Skill',
    '/Game/Main/Character/Hero/Ability/ResonanceLiberation/DA_Resonance_Liberation',
    '/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_1',
]
result = {'assets': {}, 'animations': [], 'dirty': []}
for path in paths:
    asset = unreal.load_asset(path)
    assert asset, path
    # 文本导出保留结构字段，供后续工具做增量修改而不是整包覆盖。
    task = unreal.AssetExportTask()
    task.object = asset
    task.filename = str(work / (asset.get_name() + '.t3d'))
    task.automated = True
    task.prompt = False
    unreal.Exporter.run_asset_export_task(task)
    result['assets'][path] = asset.get_class().get_name()
    if isinstance(asset, unreal.HodgeComboDefinition):
        table = asset.get_editor_property('ComboTable')
        (work / 'combo-before.json').write_text(unreal.DataTableFunctionLibrary.export_data_table_to_json_string(table), encoding='utf-8')
    if isinstance(asset, unreal.HodgeAbilityDefinition):
        montage = asset.get_editor_property('ExecutionConfig').montage
        result['assets'][path] = {'execution': asset.get_editor_property('ExecutionConfig').export_text(),
                                 'hit': asset.get_editor_property('DefaultHitConfig').export_text(),
                                 'tags': asset.get_editor_property('AbilityTag').export_text()}
        result['assets'][path]['notifies'] = [n.export_text() for n in unreal.AnimationLibrary.get_animation_notify_events(montage)]
names = ['AM_Ex_Skill01_01', 'AM_Ex_Skill02'] + ['AM_Ex_Attack%02d' % i for i in range(1, 6)]
for name in names:
    asset = unreal.load_asset('/Game/Wuwa/Anim/' + name)
    options = unreal.AnimPoseEvaluationOptions()
    first = unreal.AnimPoseExtensions.get_anim_pose_at_time(asset, 0., options)
    bones = [str(b) for b in unreal.AnimPoseExtensions.get_bone_names(first)]
    selected = [b for b in bones if b.lower() in ['root', 'hand_r', 'hand_l', 'pelvis'] or b.lower().endswith('hand_r')]
    samples = []
    for i in range(int(asset.get_play_length() * 30) + 1):
        time = i / 30.
        pose = unreal.AnimPoseExtensions.get_anim_pose_at_time(asset, time, options)
        samples.append({'time': time, 'bones': {b: unreal.AnimPoseExtensions.get_bone_pose(pose, b, unreal.AnimPoseSpaces.WORLD).export_text() for b in selected}})
    result['animations'].append({'name': name, 'bones': bones, 'samples': samples})
result['dirty'] = [p.get_path_name() for p in unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()]
(work / 'asset-audit.json').write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps({'animations': len(result['animations']), 'dirty': result['dirty']}))
