"""只给正式共鸣解放增加一次 Launch 请求，保留伤害、通知、镜头和消耗。"""
from pathlib import Path
import hashlib
import json
import unreal

PROJECT = Path(unreal.Paths.get_project_file_path()).resolve()
assert PROJECT == Path('E:/Project/Git/Hodgepodge/Hodgepodge.uproject')
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
ROOT = '/Game/Main/Character/Hero/Ability/ResonanceLiberation'
BACKUP = '/Game/Tests/Combat/ResonanceLaunch/Backups/DA_Resonance_Liberation_BeforeLaunch'
OUT = PROJECT.parent / 'Saved/ACTCombat/ResonanceLaunch'
OUT.mkdir(parents=True, exist_ok=True)
definition = unreal.load_asset(ROOT + '/DA_Resonance_Liberation')
assert definition
resume = bool(globals().get('HODGE_RESUME_LAUNCH_SETUP', False))
assert resume or not unreal.EditorAssetLibrary.does_asset_exist(BACKUP), '已有备份，停止而不覆盖后续手工调整'
hit = definition.get_editor_property('DefaultHitConfig')
reaction = hit.get_editor_property('Reaction')
assert not list(reaction.get_editor_property('Impacts')), '已有 Impact 配置，不能自动替换'
before = hit.export_text()
execution = definition.get_editor_property('ExecutionConfig').export_text()
montage = definition.get_editor_property('ExecutionConfig').montage
notifies = [e.export_text() for e in unreal.AnimationLibrary.get_animation_notify_events(montage)]
hits = [e.notify for e in unreal.AnimationLibrary.get_animation_notify_events(montage)
        if isinstance(e.notify, unreal.HodgeAnimNotify_Hit)]
assert len(hits) == 1 and hits[0].get_editor_property('Hit').get_editor_property('bUseDefaultReaction')
montage_file = PROJECT.parent / 'Content' / (montage.get_path_name().split('.')[0][6:] + '.uasset')
montage_hash = hashlib.sha256(montage_file.read_bytes()).hexdigest()
# 测试目标继续复用已有 Fly 素材；缺少目标配置时先报错，不生成骨骼动作。
targets = []
for path in ['/Game/Main/Character/Hero/Anim/HitReactions/DA_Hero_HitReaction',
             '/Game/CodexText/HitReactionValidation/Dummy/DA_DummyHitReaction']:
    profile = unreal.load_asset(path)
    launch = next(e for e in profile.get_editor_property('Animations') if e.type == unreal.HodgeImpactType.LAUNCH)
    expected = ['AM_Hero_Launch', 'AM_Hero_AirLoop', 'AM_Hero_Landing']
    actual = [launch.get_editor_property(p).get_name() for p in ['Montage', 'AirLoopMontage', 'LandingMontage']]
    assert actual == expected, (path, actual)
    targets.append({'profile': path, 'launch_chain': actual})
if resume:
    # 仅恢复本轮保存前失败的制作，现配置与备份不同则仍然停止。
    backup = unreal.load_asset(BACKUP)
    assert backup and backup.get_editor_property('DefaultHitConfig').export_text() == before
    assert backup.get_editor_property('ExecutionConfig').export_text() == execution
else:
    backup = unreal.EditorAssetLibrary.duplicate_asset(definition.get_path_name().split('.')[0], BACKUP)
    assert backup and unreal.EditorAssetLibrary.save_loaded_asset(backup, False)
launch = unreal.HodgeImpactSpec()
launch.set_editor_property('Type', unreal.HodgeImpactType.LAUNCH)
launch.set_editor_property('HorizontalSpeed', 180.0)
launch.set_editor_property('VerticalSpeed', 800.0)
launch.set_editor_property('bKnockdownOnLanding', False)
reaction.set_editor_property('Impacts', [launch])
hit.set_editor_property('Reaction', reaction)
definition.set_editor_property('DefaultHitConfig', hit)
# 清除副本中新加的 Impact 后必须与原命中配置完全一致。
probe = type(hit)()
probe.import_text(definition.get_editor_property('DefaultHitConfig').export_text())
probe_reaction = probe.get_editor_property('Reaction')
probe_reaction.set_editor_property('Impacts', [])
probe.set_editor_property('Reaction', probe_reaction)
assert probe.export_text() == before, 'Launch 之外的命中配置被改变'
assert len(definition.get_editor_property('DefaultHitConfig').get_editor_property('Reaction').get_editor_property('Impacts')) == 1
assert definition.get_editor_property('ExecutionConfig').export_text() == execution
assert [e.export_text() for e in unreal.AnimationLibrary.get_animation_notify_events(montage)] == notifies
assert hashlib.sha256(montage_file.read_bytes()).hexdigest() == montage_hash
assert unreal.EditorAssetLibrary.save_loaded_asset(definition, False)
report = {'backup': BACKUP, 'definition': definition.get_path_name(), 'before': before,
          'after': definition.get_editor_property('DefaultHitConfig').export_text(),
          'montage_sha256': montage_hash, 'montage_unchanged': True, 'targets': targets,
          'root_motion_migration': False, 'generated_animation': False}
(OUT / 'configured.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
print('RESONANCE_LAUNCH_CONFIGURED ' + json.dumps(report, ensure_ascii=False))
