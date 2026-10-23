"""核对现有动作接入、逐阶段根运动契约和正式攻击非 Reaction 字段，不保存资产。"""
from pathlib import Path
import hashlib
import json
import unreal

OUT = Path(unreal.Paths.project_saved_dir()) / 'ACTCombat/Behit'
ROOT = '/Game/Main/Character/Hero/Anim/HitReactions/Behit'
report = {'profiles': [], 'attacks': [], 'animations': [], 'error': None}
try:
    validator = unreal.get_editor_subsystem(unreal.EditorValidatorSubsystem)
    for path in ['/Game/Main/Character/Hero/Anim/HitReactions/DA_Hero_HitReaction',
                 '/Game/CodexText/HitReactionValidation/Dummy/DA_DummyHitReaction']:
        profile = unreal.load_asset(path)
        result, errors, warnings = validator.is_asset_valid(unreal.EditorAssetLibrary.find_asset_data(path), unreal.DataValidationUsecase.SCRIPT)
        assert result == unreal.DataValidationResult.VALID, (path, errors)
        for entry in profile.get_editor_property('Animations'):
            for field in ['Montage', 'AirLoopMontage', 'LandingMontage', 'GetUpMontage']:
                asset = entry.get_editor_property(field)
                assert not asset or '/ACTPrototype/' not in asset.get_path_name()
            assert str(entry.get_editor_property('SustainSection')) in ['', 'None'] and str(entry.get_editor_property('RecoverySection')) in ['', 'None']
            if entry.type == unreal.HodgeImpactType.HIT_STUN:
                names = {v.montage.get_name() for v in entry.variants}
                assert names == {'AM_Behit_S_L', 'AM_Behit_S_R', 'AM_Behit_B_L', 'AM_Behit_B_R'}
                assert entry.motion_source == unreal.HodgeReactionMotionSource.ANIMATION_ROOT_MOTION
            if entry.type in [unreal.HodgeImpactType.LAUNCH, unreal.HodgeImpactType.AIR_HIT]:
                family = 'Fly' if entry.type == unreal.HodgeImpactType.LAUNCH else 'Push'
                assert entry.montage.get_name() == 'AM_Behit_' + family + '_Start'
                assert entry.air_loop_montage.get_name() == 'AM_Behit_' + family + '_Loop'
                assert entry.landing_montage.get_name() == 'AM_Behit_' + family + '_Fall'
                assert entry.motion_source == unreal.HodgeReactionMotionSource.IMPACT
                assert entry.landing_motion_source == unreal.HodgeReactionMotionSource.ANIMATION_ROOT_MOTION
        report['profiles'].append({'asset': path, 'valid': True, 'warnings': [str(w) for w in warnings]})
    for number in [1, 2, 3, 5, 4]:
        name = 'DA_Attack_' + str(number)
        asset = unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/' + name)
        old = unreal.load_asset('/Game/Tests/Combat/BehitMigration/Backups/' + name + '_BeforeBehit')
        for field in ['ExecutionConfig', 'ExecutionBodyTag']:
            assert asset.get_editor_property(field).export_text() == old.get_editor_property(field).export_text(), (name, field)
        current = asset.get_editor_property('DefaultHitConfig')
        probe = type(current)()
        probe.import_text(current.export_text())
        probe.set_editor_property('Reaction', old.get_editor_property('DefaultHitConfig').get_editor_property('Reaction'))
        assert probe.export_text() == old.get_editor_property('DefaultHitConfig').export_text(), (name, '伤害字段变化')
        if number in [1, 2]:
            assert len(current.reaction.impacts) == 1 and current.reaction.impacts[0].type == unreal.HodgeImpactType.HIT_STUN
        report['attacks'].append({'asset': asset.get_path_name(), 'damage_execution_body_unchanged': True})
    source_audit = json.loads((OUT / 'source-audit.json').read_text(encoding='utf-8'))
    for name, info in source_audit.items():
        path = Path(unreal.Paths.project_content_dir()) / 'Wuwa/Anim/BaseAnim' / (name + '.uasset')
        assert hashlib.sha256(path.read_bytes()).hexdigest() == info['sha256'], name
        sequence = unreal.load_asset(ROOT + '/A_' + name)
        montage = unreal.load_asset(ROOT + '/AM_' + name)
        assert abs(sequence.get_play_length() - info['length']) < .001
        assert abs(montage.get_play_length() - info['length']) < .001
        ground = name.startswith(('Behit_S_', 'Behit_B_')) or name.endswith('_Fall') or name == 'StandUp'
        assert sequence.get_editor_property('enable_root_motion') == ground
        assert not unreal.AnimationLibrary.get_animation_notify_events(montage)
        report['animations'].append({'asset': montage.get_path_name(), 'source_unchanged': True, 'root_motion': ground})
    anim = unreal.load_asset('/Game/Main/Character/Hero/Anim/ABP_Pover_Base')
    cdo = unreal.get_default_object(anim.generated_class())
    assert cdo.get_editor_property('root_motion_mode') == unreal.RootMotionMode.ROOT_MOTION_FROM_MONTAGES_ONLY
    report['root_motion_mode'] = str(cdo.get_editor_property('root_motion_mode'))
    compiled = unreal.HodgeAnimationAuthoringLibrary.compile_animation_blueprint(anim)
    assert compiled.startswith('ERRORS=0'), compiled
    report['anim_compile'] = compiled
except Exception as error:
    report['error'] = str(error)
    raise
finally:
    (OUT / 'assets-validated.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
print('AUTHORED_BEHIT_ASSETS_VALIDATED')
