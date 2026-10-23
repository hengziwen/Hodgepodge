"""只读核对精简配置、目标削韧接线及攻击非 Reaction 字段，编译受影响蓝图。"""
from pathlib import Path
import hashlib
import json
import traceback
import unreal

OUT = Path(unreal.Paths.project_saved_dir()) / globals().get('POISE_ASSET_REPORT_DIR', 'PoiseReaction')
OUT.mkdir(parents=True, exist_ok=True)
report = {'error': None, 'checks': [], 'blueprints': [], 'attack_inputs': []}


def check(name, condition, detail=None):
    """保留失败证据，不用资产已加载代替实际校验。"""
    report['checks'].append({'name': name, 'passed': bool(condition), 'detail': detail})
    assert condition, (name, detail)


try:
    # 迁移前后素材来源沿用上一轮记录，新一轮报告使用独立目录。
    migration = json.loads((Path(unreal.Paths.project_saved_dir()) / 'PoiseReaction/migration.json').read_text(encoding='utf-8'))
    validator = unreal.get_editor_subsystem(unreal.EditorValidatorSubsystem)
    for row in migration['profiles'] + migration['pawn_data'] + migration['attacks']:
        path = row['asset'].split('.')[0]
        result, errors, warnings = validator.is_asset_valid(unreal.EditorAssetLibrary.find_asset_data(path), unreal.DataValidationUsecase.SCRIPT)
        check('AssetValid:' + path, result == unreal.DataValidationResult.VALID, [str(e) for e in errors])
    for row in migration['backups']:
        current = unreal.load_asset(row['asset'])
        old = unreal.load_asset(row['backup'])
        if isinstance(current, unreal.HodgeAbilityDefinition):
            # 记录当前作者数值而非沿用迁移初值，便于复核后续节奏校准。
            reaction = current.get_editor_property('DefaultHitConfig').get_editor_property('Reaction')
            report['attack_inputs'].append({'asset': current.get_path_name(), 'mode': str(reaction.get_editor_property('InputMode')),
                                           'poise': reaction.get_editor_property('ReservedPoiseDamage')})
            for field in ['ExecutionConfig', 'ExecutionBodyTag']:
                check(current.get_name() + ':' + field, current.get_editor_property(field).export_text() == old.get_editor_property(field).export_text())
            hit = current.get_editor_property('DefaultHitConfig')
            copy = type(hit)()
            copy.import_text(hit.export_text())
            copy.set_editor_property('Reaction', old.get_editor_property('DefaultHitConfig').get_editor_property('Reaction'))
            check(current.get_name() + ':DamageUnchanged', copy.export_text() == old.get_editor_property('DefaultHitConfig').export_text())
            montage = current.get_editor_property('ExecutionConfig').montage
            for event in unreal.AnimationLibrary.get_animation_notify_events(montage):
                notify = event.notify or event.notify_state_class
                if isinstance(notify, (unreal.HodgeAnimNotify_Hit, unreal.HodgeAnimNotifyState_HitCheck)):
                    hit_notify = notify.get_editor_property('Hit')
                    check(current.get_name() + ':NotifyUsesDefault', hit_notify.get_editor_property('bUseDefaultReaction'))
                    check(current.get_name() + ':SegmentLaunchFlag', hit_notify.get_editor_property('bCanLaunch') ==
                          (current.get_name() in ['DA_Attack_4', 'DA_Resonance_Liberation']))
        if isinstance(current, unreal.HodgeHitReactionProfile):
            check(current.get_name() + ':LegacyRetained', current.get_editor_property('Animations').export_text() == old.get_editor_property('Animations').export_text()
                  if hasattr(current.get_editor_property('Animations'), 'export_text') else
                  [e.export_text() for e in current.get_editor_property('Animations')] == [e.export_text() for e in old.get_editor_property('Animations')])
            check(current.get_name() + ':CompactSelected', current.get_editor_property('bUseCompactAnimations'))
    root = '/Game/Main/Character/Hero/Anim/HitReactions/Behit/InPlace/'
    for path in migration['created']:
        asset = unreal.load_asset(path)
        if isinstance(asset, unreal.AnimSequence):
            check(asset.get_name() + ':RootLocked', not asset.get_editor_property('enable_root_motion') and asset.get_editor_property('bForceRootLock'))
        else:
            check(asset.get_name() + ':NoAttackNotify', not unreal.AnimationLibrary.get_animation_notify_events(asset))
    check('B_LExcludesCorruptInterval', unreal.load_asset(root + 'AM_IP_Behit_B_L').get_play_length() < 1.15)
    for name, sha in migration['source_hashes'].items():
        file = Path(unreal.Paths.project_content_dir()) / ('Wuwa/Anim/BaseAnim/' + name + '.uasset')
        check('SourcePreserved:' + name, hashlib.sha256(file.read_bytes()).hexdigest() == sha)
    anim = unreal.load_asset('/Game/Main/Character/Hero/Anim/ABP_Pover_Base')
    check('AttackRootMotionRetained', unreal.get_default_object(anim.generated_class()).get_editor_property('root_motion_mode') == unreal.RootMotionMode.ROOT_MOTION_FROM_MONTAGES_ONLY)
    compiled = unreal.HodgeAnimationAuthoringLibrary.compile_animation_blueprint(anim)
    check('HeroAnimBPCompiled', compiled.startswith('ERRORS=0'), compiled)
    report['blueprints'].append({'asset': anim.get_path_name(), 'result': compiled})
except Exception:
    report['error'] = traceback.format_exc()
    raise
finally:
    (OUT / 'assets-validated.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
print('POISE_ASSETS_VALIDATED')
