"""制作独立持续／恢复动作样例，只接入百万血假人，不覆盖正式源动画。"""
from pathlib import Path
import hashlib
import json
import math
import traceback
import unreal

ROOT = '/Game/Tests/Combat/ACTPrototype'
TARGET = '/Game/CodexText/HitReactionValidation/Dummy/DA_DummyHitReaction'
BACKUP = ROOT + '/Backups/DA_DummyHitReaction_BeforeACT'
OUT = Path(unreal.Paths.project_saved_dir()) / 'ACTCombat'
OUT.mkdir(parents=True, exist_ok=True)
report = {'created': [], 'target': TARGET, 'backup': BACKUP, 'error': None,
          'scope': '可替换动作原型；尚未进行主观美术验收'}
tools = unreal.AssetToolsHelpers.get_asset_tools()
CURVE_KEYS = [unreal.Vector2D(x, y) for x, y in [(0, 0), (0.08, 1), (0.12, 0.9), (0.4, 0.35), (0.75, 0.07), (1, 0)]]


def save(obj):
    """只保存本批明确创建或指定的包，不执行全部脏包保存。"""
    assert unreal.EditorAssetLibrary.save_loaded_asset(obj, False), obj.get_path_name()


def weight(t, head=False):
    """冲击后进入闭合持续段，控制释放后从恢复段回到基准姿态。"""
    peak = 0.09 if head else 0.06
    if t < peak:
        return math.sin(math.pi * 0.5 * t / peak)
    if t < 0.12:
        return 1.0
    if t <= 0.32:
        return 0.98 + 0.02 * math.cos(2 * math.pi * (t - 0.12) / 0.2)
    return max(0.0, math.cos(math.pi * 0.5 * (t - 0.32) / 0.48)) ** 2


def make_montage(source, tracks, base, name, multiplier):
    """在当前合法骨架上制作原地强反应，源资源与其压缩数据保持原样。"""
    # 新建普通序列，避免复制 Additive 包后异步压缩与类型切换发生竞争。
    factory = unreal.AnimSequenceFactory()
    factory.set_editor_property('TargetSkeleton', source.get_editor_property('Skeleton'))
    sequence = tools.create_asset('A_ACT_' + name, ROOT, unreal.AnimSequence, factory)
    assert sequence
    controller = sequence.controller
    controller.open_bracket('Hodge ACT reaction prototype', False)
    controller.set_frame_rate(unreal.FrameRate(60, 1), False)
    controller.set_number_of_frames(unreal.FrameNumber(48), False)
    amplitudes = {'Bip001Spine': 8.0, 'Bip001Spine1': 10.0, 'Bip001Spine2': 7.0, 'Bip001Head': 12.0}
    for bone in tracks:
        assert controller.add_bone_curve(bone, False), str(bone)
        pose = base[str(bone)]
        rotations = []
        for frame in range(49):
            strength = multiplier * amplitudes.get(str(bone), 0.0) * weight(frame / 60.0, 'Head' in str(bone))
            pitch = strength * (-1.0 if name.startswith('Front') else 1.0) if name.startswith(('Front', 'Back')) else 0.0
            yaw = strength * (-1.0 if name.startswith('Left') else 1.0) if name.startswith(('Left', 'Right')) else 0.0
            rotations.append(pose.rotation * unreal.Rotator(pitch=pitch, yaw=yaw, roll=0.0).quaternion())
        assert controller.set_bone_track_keys(bone, [pose.translation] * 49, rotations, [pose.scale3d] * 49, False)
    controller.close_bracket(False)
    sequence.set_editor_property('enable_root_motion', False)
    sequence.set_editor_property('bForceRootLock', True)
    sequence.set_editor_property('additive_anim_type', unreal.AdditiveAnimationType.AAT_NONE)
    factory = unreal.AnimMontageFactory()
    factory.set_editor_property('TargetSkeleton', sequence.get_editor_property('Skeleton'))
    factory.set_editor_property('SourceAnimation', sequence)
    montage = tools.create_asset('AM_ACT_' + name, ROOT, unreal.AnimMontage, factory)
    assert montage and unreal.HodgeAnimationAuthoringLibrary.configure_act_reaction_sections(montage, 0.12, 0.32)
    for key, seconds in [('BlendIn', 0.06), ('BlendOut', 0.12)]:
        blend = montage.get_editor_property(key)
        blend.import_text('(BlendTime=' + str(seconds) + ')')
        montage.set_editor_property(key, blend)
    for asset in [sequence, montage]:
        unreal.EditorAssetLibrary.set_metadata_tag(asset, 'HodgePrototype', 'ACT sustained reaction; replace after art review')
        save(asset)
        report['created'].append(asset.get_path_name())
    return montage


try:
    assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
    assert hasattr(unreal.HodgeAnimationAuthoringLibrary, 'configure_act_reaction_sections'), '需要先构建并冷启动最新 Editor 模块'
    # 写包前先验证本机曲线反射和工厂接口，失败时不留下半批动画资产。
    assert hasattr(unreal, 'CurveFloatFactory')
    probe_curve = unreal.new_object(unreal.CurveFloat)
    assert unreal.HodgeAnimationAuthoringLibrary.configure_act_knockback_curve(probe_curve, CURVE_KEYS)
    assert abs(probe_curve.get_float_value(0.08) - 1.0) < 0.001 and abs(probe_curve.get_float_value(1.0)) < 0.001
    # 原生压缩失败可能只留下备份；已制作任何正式样例时禁止自动覆盖。
    assert not unreal.EditorAssetLibrary.does_asset_exist(ROOT + '/AM_ACT_Front_Medium'), '已有本批资源，停止而不覆盖用户修改'
    source = unreal.load_asset('/Game/Main/Character/Hero/Anim/HitReactions/A_Hero_LightFeedback')
    profile = unreal.load_asset(TARGET)
    assert source and profile
    source_file = Path(unreal.Paths.project_content_dir()) / 'Main/Character/Hero/Anim/HitReactions/A_Hero_LightFeedback.uasset'
    source_sha = hashlib.sha256(source_file.read_bytes()).hexdigest()
    backup = unreal.load_asset(BACKUP) if unreal.EditorAssetLibrary.does_asset_exist(BACKUP) else tools.duplicate_asset(BACKUP.rsplit('/', 1)[1], BACKUP.rsplit('/', 1)[0], profile)
    assert backup
    save(backup)
    report['created'].append(backup.get_path_name())
    tracks = unreal.AnimationLibrary.get_animation_track_names(source)
    base = {str(b): unreal.AnimationLibrary.get_bone_pose_for_time(source, b, 0.0, False) for b in tracks}
    variants = []
    default = None
    for direction_name, direction in [('Front', unreal.HodgeHitPresentationDirection.FRONT),
                                      ('Back', unreal.HodgeHitPresentationDirection.BACK),
                                      ('Left', unreal.HodgeHitPresentationDirection.LEFT),
                                      ('Right', unreal.HodgeHitPresentationDirection.RIGHT)]:
        for suffix, threshold, multiplier in [('Medium', 0.1, 1.0), ('Heavy', 1.35, 1.45)]:
            montage = make_montage(source, tracks, base, direction_name + '_' + suffix, multiplier)
            if default is None:
                default = montage
            entry = unreal.HodgeHitReactionVariant()
            entry.set_editor_property('Direction', direction)
            entry.set_editor_property('MinimumStrength', threshold)
            entry.set_editor_property('Montage', montage)
            variants.append(entry)
    curve = tools.create_asset('CF_ACT_Knockback', ROOT, unreal.CurveFloat, unreal.CurveFloatFactory())
    assert curve
    assert unreal.HodgeAnimationAuthoringLibrary.configure_act_knockback_curve(curve, CURVE_KEYS)
    assert abs(curve.get_float_value(0.08) - 1.0) < 0.001 and abs(curve.get_float_value(1.0)) < 0.001
    save(curve)
    report['created'].append(curve.get_path_name())
    animations = list(profile.get_editor_property('Animations'))
    for entry in animations:
        if entry.get_editor_property('Type') in [unreal.HodgeImpactType.HIT_STUN, unreal.HodgeImpactType.KNOCKBACK]:
            entry.set_editor_property('Montage', default)
            entry.set_editor_property('Variants', variants)
            entry.set_editor_property('SustainSection', 'Sustain')
            entry.set_editor_property('RecoverySection', 'Recover')
    profile.set_editor_property('Animations', animations)
    profile.set_editor_property('bAllowNaturalRecoveryTail', True)
    profile.set_editor_property('KnockbackStrengthCurve', curve)
    assert hashlib.sha256(source_file.read_bytes()).hexdigest() == source_sha, '源资源被意外修改'
    save(profile)
    report['source_unchanged'] = True
except Exception:
    report['error'] = traceback.format_exc()
    raise
finally:
    (OUT / 'reaction-slice-assets.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
