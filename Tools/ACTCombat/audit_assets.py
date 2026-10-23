"""只读记录 ACT 切片的实际资产、反射布局和来源哈希，不保存正式资源。"""
from pathlib import Path
from datetime import datetime
import hashlib
import json
import unreal

PROJECT = Path(unreal.Paths.get_project_file_path()).resolve()
assert PROJECT == Path('E:/Project/Git/Hodgepodge/Hodgepodge.uproject')
OUT = PROJECT.parent / 'Saved' / 'ACTCombat' / ('Audit-' + datetime.now().strftime('%Y%m%d-%H%M%S'))
OUT.mkdir(parents=True, exist_ok=False)


def serialize(value):
    """结构和对象只保存值或路径，不把反射包装器地址当配置。"""
    if value is None or isinstance(value, (str, float, int, bool)):
        return value
    if isinstance(value, unreal.Object):
        return value.get_path_name()
    if hasattr(value, 'export_text'):
        return value.export_text()
    return [serialize(item) for item in value]


def export_object(obj):
    """保护字段由引擎导出复核，查询失败不返回伪造的空配置。"""
    task = unreal.AssetExportTask()
    task.object = obj
    task.filename = str(OUT / (obj.get_name().replace(':', '_') + '.t3d'))
    task.automated = True
    task.prompt = False
    task.replace_identical = True
    task.exporter = unreal.ObjectExporterT3D()
    assert unreal.Exporter.run_asset_export_task(task), obj.get_path_name()


paths = ['/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_' + str(i) for i in [1, 2, 3, 5, 4]]
paths += ['/Game/Main/Character/Hero/Anim/HitReactions/DA_Hero_HitReaction',
          '/Game/Main/Character/Enemies/Guardian/DA_Guardian_HitReaction',
          '/Game/CodexText/HitReactionValidation/Dummy/DA_DummyHitReaction',
          '/Game/Main/Data/Combo/DT_LightCombo', '/Game/Main/Character/Hero/Anim/ABP_Pover_Base']
paths += ['/Game/Main/Character/Hero/Ability/ResonanceSkill/DA_Resonance_Skill',
          '/Game/Main/Character/Hero/Ability/ResonanceLiberation/DA_Resonance_Liberation',
          '/Game/Main/Character/Hero/Ability/Sprint/DA_Hero_SprintAbility']
report = {'project': str(PROJECT), 'assets': [], 'errors': [], 'read_only': True}
for path in paths:
    row = {'asset': path}
    try:
        obj = unreal.load_asset(path)
        assert obj, path
        package = PROJECT.parent / 'Content' / (path.removeprefix('/Game/') + '.uasset')
        row['source_sha256'] = hashlib.sha256(package.read_bytes()).hexdigest()
        row['class'] = obj.get_class().get_path_name()
        export_object(obj)
        if isinstance(obj, unreal.HodgeAbilityDefinition):
            config = obj.get_editor_property('ExecutionConfig')
            montage = config.get_editor_property('Montage')
            export_object(montage)
            row['execution'] = serialize(config)
            row['hit'] = serialize(obj.get_editor_property('DefaultHitConfig'))
            row['notifies'] = [event.export_text() for event in unreal.AnimationLibrary.get_animation_notify_events(montage)]
        if isinstance(obj, unreal.HodgeHitReactionProfile):
            fields = ['Animations', 'DirectionalLightFeedback', 'ContactEffect', 'ContactSound',
                      'ReplacementBlendOut', 'bSustainGroundHitStun', 'KnockbackStrengthCurve',
                      'DownedDuration', 'MaxControlDuration', 'MaxAirborneDuration', 'MaxAirHits',
                      'bAllowNaturalRecoveryTail', 'bEnableGroundRecovery', 'DodgeRecoveryDelay', 'AttackRecoveryDelay', 'MovementRecoveryDelay']
            row['properties'] = {field: serialize(obj.get_editor_property(field)) for field in fields}
            for entry in obj.get_editor_property('Animations'):
                for field in ['Montage', 'AirLoopMontage', 'LandingMontage', 'GetUpMontage']:
                    montage = entry.get_editor_property(field)
                    if montage:
                        export_object(montage)
        assert hashlib.sha256(package.read_bytes()).hexdigest() == row['source_sha256'], path
    except Exception as error:
        row['error'] = str(error)
        report['errors'].append({'asset': path, 'error': str(error)})
    report['assets'].append(row)

(OUT / 'assets.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
assert not report['errors'], report['errors']
print('ACT_AUDIT_COMPLETE ' + str(OUT))
