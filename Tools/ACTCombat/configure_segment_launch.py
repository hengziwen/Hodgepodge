"""只迁移正式终结刀与 R 的分段击飞开关，保存前备份 Montage，不改伤害或攻击时机。"""
from pathlib import Path
import hashlib
import json
import traceback
import unreal

ROOT = Path(unreal.Paths.project_dir()).resolve()
assert ROOT == Path('E:/Project/Git/Hodgepodge')
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
OUT = ROOT / 'Saved/SegmentLaunch'
OUT.mkdir(parents=True, exist_ok=True)
BACKUP = '/Game/Tests/Combat/SegmentLaunch/Backups'
report = {'error': None, 'assets': [], 'checks': []}


def hit_records(montage):
    """单次与持续通知都属于分段作者入口，附加命中也必须单独覆盖。"""
    for event in unreal.AnimationLibrary.get_animation_notify_events(montage):
        notify = event.notify or event.notify_state_class
        if isinstance(notify, (unreal.HodgeAnimNotify_Hit, unreal.HodgeAnimNotifyState_HitCheck)):
            yield notify, 'Hit', False
            if isinstance(notify, unreal.HodgeAnimNotify_Hit):
                yield notify, 'AdditionalHits', True


try:
    paths = ['/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_' + str(i) for i in [1, 2, 3, 5, 4]]
    paths.append('/Game/Main/Character/Hero/Ability/ResonanceLiberation/DA_Resonance_Liberation')
    for path in paths:
        definition = unreal.load_asset(path)
        assert definition
        definition_file = ROOT / 'Content' / (path.removeprefix('/Game/') + '.uasset')
        before_hash = hashlib.sha256(definition_file.read_bytes()).hexdigest()
        montage = definition.get_editor_property('ExecutionConfig').montage
        assert montage
        enabled = definition.get_name() in ['DA_Attack_4', 'DA_Resonance_Liberation']
        row = {'definition': path, 'montage': montage.get_path_name(), 'enabled': enabled, 'records': []}
        records = list(hit_records(montage))
        assert records, path + ': no authored hit segment'
        if enabled:
            backup = BACKUP + '/' + montage.get_name() + '_BeforeSegmentLaunch'
            assert not unreal.EditorAssetLibrary.does_asset_exist(backup), '迁移已存在，停止以保留后续手调：' + backup
            copied = unreal.EditorAssetLibrary.duplicate_asset(montage.get_path_name().split('.')[0], backup)
            assert copied and unreal.EditorAssetLibrary.save_loaded_asset(copied, False)
            row['backup'] = backup
        for notify, field, is_array in records:
            values = list(notify.get_editor_property(field)) if is_array else [notify.get_editor_property(field)]
            updated = []
            for value in values:
                old = value.export_text()
                value.set_editor_property('bCanLaunch', enabled)
                row['records'].append({'notify': notify.get_name(), 'field': field, 'before': old, 'after': value.export_text()})
                updated.append(value)
            if values:
                notify.set_editor_property(field, updated if is_array else updated[0])
        if enabled:
            assert unreal.EditorAssetLibrary.save_loaded_asset(montage, False)
        assert hashlib.sha256(definition_file.read_bytes()).hexdigest() == before_hash, 'Definition was changed'
        report['checks'].append({'asset': path, 'definition_unchanged': True, 'segment_flag': enabled})
        report['assets'].append(row)
except Exception:
    report['error'] = traceback.format_exc()
finally:
    (OUT / 'asset-migration.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
    print('SEGMENT_LAUNCH_ASSETS ' + str(report['error']))
if report['error']:
    raise RuntimeError(report['error'])
