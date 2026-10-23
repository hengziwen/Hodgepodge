"""核对正式数值未越界、原型加法基准和旧字段兼容，输出可复查报告。"""
import json
from pathlib import Path
import unreal

OUT = Path(unreal.Paths.project_saved_dir()) / 'HitPresentation'
BACKUP = '/Game/Tests/Combat/HitPresentationBackups/B20261016'
report = {'attacks': [], 'prototypes': [], 'reserved_nonzero': [], 'errors': []}
fields = ['DamageEffect', 'DamageMultiplier', 'DamageType', 'DamageElement', 'TargetPolicy',
          'RepeatHitInterval', 'HitGroup', 'AttackPhase', 'bCanBeDodged', 'bCanTriggerPerfectDodge', 'bAllowFriendlyFire']


def value(obj):
    """将对象与结构体转为稳定值，不比较 Python 包装器地址。"""
    if hasattr(obj, 'get_path_name'):
        return obj.get_path_name()
    if hasattr(obj, 'export_text'):
        return obj.export_text()
    return str(obj)


for number in [1, 2, 3, 5, 4]:
    name = 'DA_Attack_' + str(number)
    current = unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/' + name)
    old = unreal.load_asset(BACKUP + '/' + name)
    assert current and old
    config = current.get_editor_property('DefaultHitConfig')
    previous = old.get_editor_property('DefaultHitConfig')
    differences = [key for key in fields if value(config.get_editor_property(key)) != value(previous.get_editor_property(key))]
    assert not differences, (name, differences)
    assert value(current.get_editor_property('ExecutionConfig')) == value(old.get_editor_property('ExecutionConfig'))
    assert value(current.get_editor_property('ExecutionBodyTag')) == value(old.get_editor_property('ExecutionBodyTag'))
    report['attacks'].append({'name': name, 'unchanged_damage_and_execution': True,
                              'reaction': config.get_editor_property('Reaction').export_text()})

profile = unreal.load_asset('/Game/Main/Character/Hero/Anim/HitReactions/DA_Hero_HitReaction')
for entry in profile.get_editor_property('DirectionalLightFeedback'):
    montage = entry.get_editor_property('Montage')
    sequence = unreal.load_asset(montage.get_path_name().replace('AM_Hit_', 'A_Hit_'))
    assert sequence and abs(montage.get_play_length() - 0.2) < 0.001
    assert sequence.get_editor_property('additive_anim_type') == unreal.AdditiveAnimationType.AAT_LOCAL_SPACE_BASE
    assert sequence.get_editor_property('ref_frame_index') == 0 and not sequence.get_editor_property('enable_root_motion')
    root_first = unreal.AnimationLibrary.get_bone_pose_for_time(sequence, 'Bip001Pelvis', 0, False)
    root_last = unreal.AnimationLibrary.get_bone_pose_for_time(sequence, 'Bip001Pelvis', 0.2, False)
    assert root_first.export_text() == root_last.export_text()
    report['prototypes'].append({'direction': str(entry.direction), 'montage': montage.get_path_name(),
                                  'length': montage.get_play_length(), 'additive': True, 'pelvis_static': True})

# 只审计正式定义与其通知覆盖，不把未配置和合法空 Impact 合并。
registry = unreal.AssetRegistryHelpers.get_asset_registry()
definitions = registry.get_assets_by_class(unreal.TopLevelAssetPath('/Script/Hodgepodge', 'HodgeAbilityDefinition'), True)
report['formal_definitions_checked'] = 0
for item in definitions:
    if not str(item.package_name).startswith('/Game/Main/'):
        continue
    asset = item.get_asset()
    report['formal_definitions_checked'] += 1
    reaction = asset.get_editor_property('DefaultHitConfig').get_editor_property('Reaction')
    amount = reaction.get_editor_property('ReservedPoiseDamage')
    if amount != 0:
        report['reserved_nonzero'].append({'asset': asset.get_path_name(), 'value': amount})
    montage = asset.get_editor_property('ExecutionConfig').get_editor_property('Montage')
    if montage:
        for event in unreal.AnimationLibrary.get_animation_notify_events(montage):
            notify = event.get_editor_property('Notify')
            if notify and isinstance(notify, unreal.HodgeAnimNotify_Hit):
                configs = [notify.get_editor_property('Hit')] + list(notify.get_editor_property('AdditionalHits'))
                for config in configs:
                    amount = config.get_editor_property('ReactionOverride').get_editor_property('ReservedPoiseDamage')
                    if amount != 0:
                        report['reserved_nonzero'].append({'asset': montage.get_path_name(), 'notify': notify.get_name(), 'value': amount})
            window = event.get_editor_property('NotifyStateClass')
            if window and isinstance(window, unreal.HodgeAnimNotifyState_HitCheck):
                configs = [window.get_editor_property('Hit')]
                for config in configs:
                    amount = config.get_editor_property('ReactionOverride').get_editor_property('ReservedPoiseDamage')
                    if amount != 0:
                        report['reserved_nonzero'].append({'asset': montage.get_path_name(), 'notify': window.get_name(), 'value': amount})
(OUT / 'asset-verification.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
print('ASSET_VERIFICATION ' + str(len(report['attacks'])) + ' attacks, ' + str(report['formal_definitions_checked']) + ' formal definitions')
