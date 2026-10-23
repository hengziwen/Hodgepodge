"""冷启动核对已修改配置、动作样例与相关动画蓝图，不保存或重编无关资源。"""
from pathlib import Path
import json
import unreal

ROOT = '/Game/Tests/Combat/ACTPrototype'
report = {'attacks': [], 'profiles': [], 'animations': [], 'blueprints': [], 'error': None}


def validate_asset(asset):
    """通过正式 IsDataValid 运行配置规则，区分未校验与通过。"""
    subsystem = unreal.get_editor_subsystem(unreal.EditorValidatorSubsystem)
    data = unreal.EditorAssetLibrary.find_asset_data(asset.get_path_name())
    result, errors, warnings = subsystem.is_asset_valid(data, unreal.DataValidationUsecase.SCRIPT)
    assert result == unreal.DataValidationResult.VALID, (asset.get_path_name(), result, errors)
    return [str(item) for item in warnings]


try:
    for number in [1, 2, 3, 5, 4]:
        name = 'DA_Attack_' + str(number)
        asset = unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/' + name)
        old = unreal.load_asset(ROOT + '/Backups/' + name + '_BeforeACT')
        for field in ['DefaultHitConfig', 'ExecutionConfig', 'ExecutionBodyTag']:
            current, previous = asset.get_editor_property(field), old.get_editor_property(field)
            assert current.export_text() == previous.export_text(), (name, field)
        assert asset.get_editor_property('bAllowHitRecoveryAttack')
        report['attacks'].append({'asset': asset.get_path_name(), 'warnings': validate_asset(asset), 'damage_execution_unchanged': True})
    for path in ['/Game/Main/Character/Hero/Anim/HitReactions/DA_Hero_HitReaction',
                 '/Game/CodexText/HitReactionValidation/Dummy/DA_DummyHitReaction',
                 '/Game/Main/Character/Enemies/Guardian/DA_Guardian_HitReaction']:
        asset = unreal.load_asset(path)
        report['profiles'].append({'asset': path, 'warnings': validate_asset(asset),
                                   'recovery': asset.get_editor_property('bEnableGroundRecovery')})
    for direction in ['Front', 'Back', 'Left', 'Right']:
        for strength in ['Medium', 'Heavy']:
            name = direction + '_' + strength
            sequence = unreal.load_asset(ROOT + '/A_ACT_' + name)
            montage = unreal.load_asset(ROOT + '/AM_ACT_' + name)
            assert sequence.get_editor_property('additive_anim_type') == unreal.AdditiveAnimationType.AAT_NONE
            assert not sequence.get_editor_property('enable_root_motion')
            assert abs(montage.get_play_length() - .8) < .001
            assert not unreal.AnimationLibrary.get_animation_notify_events(montage)
            for bone in ['Bip001Spine2', 'Bip001Head', 'Bip001Pelvis']:
                left = unreal.AnimationLibrary.get_bone_pose_for_time(sequence, bone, .12, False)
                right = unreal.AnimationLibrary.get_bone_pose_for_time(sequence, bone, .32, False)
                dot = abs(left.rotation.x * right.rotation.x + left.rotation.y * right.rotation.y + left.rotation.z * right.rotation.z + left.rotation.w * right.rotation.w)
                assert dot > .9999, (name, bone, '持续段边界不闭合')
            report['animations'].append({'asset': montage.get_path_name(), 'length': .8, 'closed_sustain': True})
    # 当前作者接口只支持主图；未改动的 Linked Layer 不冒充编译已通过。
    report['not_compiled'] = ['/Game/Main/Character/Hero/Anim/Layer/ABP_ItemAnimLayers_Pover_Base (existing helper path scope)']
    for path in ['/Game/Main/Character/Hero/Anim/ABP_Pover_Base']:
        asset = unreal.load_asset(path)
        result = unreal.HodgeAnimationAuthoringLibrary.compile_animation_blueprint(asset)
        assert result.startswith('ERRORS=0'), (path, result)
        report['blueprints'].append({'asset': path, 'compile': result, 'saved': False})
except Exception as error:
    report['error'] = str(error)
    raise
finally:
    output = Path(unreal.Paths.project_saved_dir()) / 'ACTCombat' / 'asset-validation.json'
    output.write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
