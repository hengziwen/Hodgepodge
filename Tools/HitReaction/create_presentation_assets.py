"""制作独立短方向原型并接入正式五段，只修改已备份的目标。"""
import json
import math
from pathlib import Path
import unreal

ROOT = '/Game/Main/Character/Hero/Anim/HitReactions/PresentationPrototype'
OUT = Path(unreal.Paths.project_saved_dir()) / 'HitPresentation'
tools = unreal.AssetToolsHelpers.get_asset_tools()
source = unreal.load_asset('/Game/Main/Character/Hero/Anim/HitReactions/A_Hero_LightFeedback')
assert source and not unreal.EditorLevelLibrary.get_pie_worlds(True)
assert unreal.load_asset('/Game/Tests/Combat/HitPresentationBackups/B20261016/DA_Hero_HitReaction')
tracks = unreal.AnimationLibrary.get_animation_track_names(source)
base = {str(b): unreal.AnimationLibrary.get_bone_pose_for_time(source, b, 0.0, False) for b in tracks}
created = []


def envelope(time, peak):
    """快速受力、缓慢恢复，头部峰值迟于躯干。"""
    if time <= peak:
        return math.sin(math.pi * 0.5 * time / peak)
    return max(0.0, math.cos(math.pi * 0.5 * (time - peak) / (0.2 - peak))) ** 2


def make_direction(name, direction):
    """下半身保持基准，独立制作躯干和头部的短局部动作。"""
    path = ROOT + '/A_Hit_' + name
    assert not unreal.EditorAssetLibrary.does_asset_exist(path), '不覆盖已有原型：' + path
    sequence = tools.duplicate_asset('A_Hit_' + name, ROOT, source)
    assert sequence
    controller = sequence.controller
    controller.open_bracket('Hodge short directional hit prototype', False)
    controller.set_frame_rate(unreal.FrameRate(60, 1), False)
    controller.set_number_of_frames(unreal.FrameNumber(12), False)
    for bone in tracks:
        pose = base[str(bone)]
        rotations = []
        for frame in range(13):
            weight = envelope(frame / 60.0, 0.067 if 'Head' in str(bone) else 0.033)
            amplitude = {'Bip001Spine': 8.0, 'Bip001Spine1': 10.0, 'Bip001Spine2': 8.0, 'Bip001Head': 14.0}.get(str(bone), 0.0)
            pitch = amplitude * weight * (-1.0 if name == 'Front' else 1.0) if name in ['Front', 'Back'] else 0.0
            yaw = amplitude * weight * (-1.0 if name == 'Left' else 1.0) if name in ['Left', 'Right'] else 0.0
            rotations.append(pose.rotation * unreal.Rotator(pitch=pitch, yaw=yaw, roll=0).quaternion())
        assert controller.set_bone_track_keys(bone, [pose.translation] * 13, rotations, [pose.scale3d] * 13, False), str(bone)
    controller.close_bracket(False)
    sequence.set_editor_property('enable_root_motion', False)
    sequence.set_editor_property('bForceRootLock', True)
    sequence.set_editor_property('additive_anim_type', unreal.AdditiveAnimationType.AAT_LOCAL_SPACE_BASE)
    sequence.set_editor_property('ref_pose_type', unreal.AdditiveBasePoseType.ABPT_LOCAL_ANIM_FRAME)
    sequence.set_editor_property('ref_frame_index', 0)
    factory = unreal.AnimMontageFactory()
    factory.set_editor_property('TargetSkeleton', sequence.get_editor_property('Skeleton'))
    factory.set_editor_property('SourceAnimation', sequence)
    montage = tools.create_asset('AM_Hit_' + name, ROOT, unreal.AnimMontage, factory)
    assert montage and unreal.HodgeAnimationAuthoringLibrary.set_hero_reaction_montage_slot(montage, 'AdditiveHitReact')
    for key, value in [('BlendIn', 0.02), ('BlendOut', 0.08)]:
        blend = montage.get_editor_property(key)
        blend.import_text('(BlendTime=' + str(value) + ')')
        montage.set_editor_property(key, blend)
    for asset in [sequence, montage]:
        assert unreal.EditorAssetLibrary.save_loaded_asset(asset, False)
        created.append(asset.get_path_name())
    entry = unreal.HodgeDirectionalHitFeedback()
    entry.set_editor_property('Direction', direction)
    entry.set_editor_property('Montage', montage)
    return entry


directions = [make_direction(name, direction) for name, direction in [
    ('Front', unreal.HodgeHitPresentationDirection.FRONT), ('Back', unreal.HodgeHitPresentationDirection.BACK),
    ('Left', unreal.HodgeHitPresentationDirection.LEFT), ('Right', unreal.HodgeHitPresentationDirection.RIGHT)]]
for path in ['/Game/Main/Character/Hero/Anim/HitReactions/DA_Hero_HitReaction',
             '/Game/CodexText/HitReactionValidation/Dummy/DA_DummyHitReaction']:
    profile = unreal.load_asset(path)
    profile.set_editor_property('DirectionalLightFeedback', directions)
    # 复用项目现有接触素材作为可替换原型，不修改第三方音效或 Niagara 本体。
    profile.set_editor_property('ContactEffect', unreal.load_asset('/Game/Assets/Niagara/Impact/NS_BlockImpact'))
    profile.set_editor_property('ContactSound', unreal.load_asset('/Game/Assets/Sounds/Weapons/Axe/SW_HitBody1'))
    assert profile.get_editor_property('ContactEffect') and profile.get_editor_property('ContactSound')
    assert unreal.EditorAssetLibrary.save_loaded_asset(profile, False)

# 原伤害、倍率、攻击 Body 和窗口不改；逐刀控制差异保存在报告中。
changes = []
for number, strength in [(1, 0.65), (2, 0.8), (3, 1.15), (5, 1.0), (4, 1.6)]:
    attack = unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_' + str(number))
    config = attack.get_editor_property('DefaultHitConfig')
    before = config.export_text()
    reaction = unreal.HodgeHitReactionConfig()
    reaction.import_text('(AttackJudgementTag=(TagName="Combat.Attack.Judgement.Normal"))')
    reaction.set_editor_property('PresentationStrength', strength)
    if number in [3, 5, 4]:
        impact = unreal.HodgeImpactSpec()
        impact.set_editor_property('Type', unreal.HodgeImpactType.KNOCKBACK if number == 4 else unreal.HodgeImpactType.HIT_STUN)
        reaction.set_editor_property('Impacts', [impact])
    config.set_editor_property('Reaction', reaction)
    attack.set_editor_property('DefaultHitConfig', config)
    assert unreal.EditorAssetLibrary.save_loaded_asset(attack, False)
    changes.append({'asset': attack.get_path_name(), 'before': before, 'after': config.export_text()})

anim = unreal.load_asset('/Game/Main/Character/Hero/Anim/ABP_Pover_Base')
assert unreal.HodgeAnimationAuthoringLibrary.configure_hero_hit_reaction_graph(anim)
result = unreal.HodgeAnimationAuthoringLibrary.compile_animation_blueprint(anim)
assert result.startswith('ERRORS=0'), result
assert unreal.EditorAssetLibrary.save_loaded_asset(anim, False)
(OUT / 'asset-changes.json').write_text(json.dumps({'created': created, 'attacks': changes, 'animation_compile': result}, ensure_ascii=False, indent=2), encoding='utf-8')
print('PRESENTATION_ASSETS_CREATED ' + str(len(created)))
