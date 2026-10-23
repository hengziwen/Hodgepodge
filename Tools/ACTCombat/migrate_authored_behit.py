"""备份后接入现有 Behit 动作，不生成骨骼关键帧，不覆盖第三方源资产。"""
from pathlib import Path
import hashlib
import json
import unreal

PROJECT = Path(unreal.Paths.get_project_file_path()).resolve()
assert PROJECT == Path('E:/Project/Git/Hodgepodge/Hodgepodge.uproject')
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
ROOT = '/Game/Main/Character/Hero/Anim/HitReactions/Behit'
BACKUPS = '/Game/Tests/Combat/BehitMigration/Backups'
OUT = PROJECT.parent / 'Saved/ACTCombat/Behit'
OUT.mkdir(parents=True, exist_ok=True)
assert not unreal.EditorAssetLibrary.does_asset_exist(ROOT + '/AM_Behit_S_L'), '已存在本批资源，停止而不覆盖手调'
tools = unreal.AssetToolsHelpers.get_asset_tools()
lib = unreal.HodgeAnimationAuthoringLibrary
# 正式 Pawn 的 CDO 是实际 Mesh 来源，不按文件名称猜测骨架。
pawn = unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData')
mesh = unreal.get_default_object(pawn.get_editor_property('PawnClass')).get_component_by_class(unreal.SkeletalMeshComponent).get_editor_property('SkeletalMesh')
assert mesh
skeleton = unreal.load_asset('/Game/Main/Character/Hero/Anim/Model/SK_Pover_LyraLab')
assert skeleton
names = ['Behit_S_L', 'Behit_S_R', 'Behit_B_L', 'Behit_B_R', 'Behit_Fly_Start', 'Behit_Fly_Loop',
         'Behit_Fly_Fall', 'Behit_Push_Start', 'Behit_Push_Loop', 'Behit_Push_Fall', 'StandUp']
sources = {name: unreal.load_asset('/Game/Wuwa/Anim/BaseAnim/' + name) for name in names}
report = {'sources': {}, 'created': [], 'backups': [], 'profiles': [], 'attacks': [], 'generated_bone_keys': False}


def save(asset):
    """只保存明确授权的包，不保存其他脏资产。"""
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset, False), asset.get_path_name()


def backup(asset):
    """保留本轮之前的完整配置，不删除前两批原型或备份。"""
    path = BACKUPS + '/' + asset.get_name() + '_BeforeBehit'
    assert not unreal.EditorAssetLibrary.does_asset_exist(path), path
    copy = unreal.EditorAssetLibrary.duplicate_asset(asset.get_path_name().split('.')[0], path)
    assert copy
    save(copy)
    report['backups'].append(path)


for name, source in sources.items():
    assert source, name
    audit = json.loads(lib.inspect_behit_animation(source, mesh))
    report['sources'][name] = audit
    (OUT / 'source-audit.json').write_text(json.dumps(report['sources'], ensure_ascii=False, indent=2), encoding='utf-8')
    assert not audit['missing_bones'] and not audit['parent_mismatch'], (name, audit)
    assert audit['max_ref_translation_error'] < .1 and audit['max_ref_rotation_degrees'] < .1, '骨架参考姿势不同，需要真实重定向，不能仅替换 Skeleton'
    file = PROJECT.parent / 'Content/Wuwa/Anim/BaseAnim' / (name + '.uasset')
    audit['sha256'] = hashlib.sha256(file.read_bytes()).hexdigest()
# 先完整记录素材审计，失败时仍可定位骨架或运动数据缺口。
(OUT / 'source-audit.json').write_text(json.dumps(report['sources'], ensure_ascii=False, indent=2), encoding='utf-8')
profiles = [unreal.load_asset('/Game/Main/Character/Hero/Anim/HitReactions/DA_Hero_HitReaction'),
            unreal.load_asset('/Game/CodexText/HitReactionValidation/Dummy/DA_DummyHitReaction')]
attacks = [unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_' + str(i)) for i in [1, 2, 3, 5, 4]]
for asset in profiles + attacks:
    assert asset
    backup(asset)
montages = {}
for name, source in sources.items():
    sequence = unreal.EditorAssetLibrary.duplicate_asset(source.get_path_name().split('.')[0], ROOT + '/A_' + name)
    assert sequence and lib.set_hero_reaction_skeleton(sequence, skeleton)
    ground = name.startswith(('Behit_S_', 'Behit_B_')) or name.endswith('_Fall') or name == 'StandUp'
    sequence.set_editor_property('enable_root_motion', ground)
    sequence.set_editor_property('bForceRootLock', not ground)
    sequence.set_editor_property('root_motion_root_lock', unreal.RootMotionRootLock.ANIM_FIRST_FRAME)
    sequence.set_editor_property('additive_anim_type', unreal.AdditiveAnimationType.AAT_NONE)
    factory = unreal.AnimMontageFactory()
    factory.set_editor_property('TargetSkeleton', skeleton)
    factory.set_editor_property('SourceAnimation', sequence)
    montage = tools.create_asset('AM_' + name, ROOT, unreal.AnimMontage, factory)
    assert montage and lib.set_hero_reaction_montage_slot(montage, 'FullBody')
    assert lib.configure_behit_montage(montage, 0.0, sequence.get_play_length(), 1.0)
    for key, seconds in [('BlendIn', .05), ('BlendOut', .12)]:
        blend = montage.get_editor_property(key)
        blend.import_text('(BlendTime=' + str(seconds) + ')')
        montage.set_editor_property(key, blend)
    for asset in [sequence, montage]:
        unreal.EditorAssetLibrary.set_metadata_tag(asset, 'Hodge.SourceAnimation', source.get_path_name())
        save(asset)
        report['created'].append(asset.get_path_name())
    montages[name] = montage

# 不打断接触也复用现有 S 动作的短片段，独立 Additive Group 不抢主动作。
lights = {}
for side in ['L', 'R']:
    source = sources['Behit_S_' + side]
    sequence = unreal.EditorAssetLibrary.duplicate_asset(source.get_path_name().split('.')[0], ROOT + '/A_Local_S_' + side)
    assert sequence and lib.set_hero_reaction_skeleton(sequence, skeleton)
    sequence.set_editor_property('enable_root_motion', False)
    sequence.set_editor_property('bForceRootLock', True)
    sequence.set_editor_property('additive_anim_type', unreal.AdditiveAnimationType.AAT_LOCAL_SPACE_BASE)
    sequence.set_editor_property('ref_pose_type', unreal.AdditiveBasePoseType.ABPT_LOCAL_ANIM_FRAME)
    sequence.set_editor_property('ref_frame_index', 0)
    factory = unreal.AnimMontageFactory()
    factory.set_editor_property('TargetSkeleton', skeleton)
    factory.set_editor_property('SourceAnimation', sequence)
    montage = tools.create_asset('AM_Local_S_' + side, ROOT, unreal.AnimMontage, factory)
    assert montage and lib.set_hero_reaction_montage_slot(montage, 'AdditiveHitReact')
    assert lib.configure_behit_montage(montage, 0.0, .3, 1.0)
    save(sequence)
    save(montage)
    lights[side] = montage
    report['created'].extend([sequence.get_path_name(), montage.get_path_name()])

motion = unreal.HodgeReactionMotionSource
for profile in profiles:
    entries = list(profile.get_editor_property('Animations'))
    for entry in entries:
        entry.set_editor_property('Variants', [])
        entry.set_editor_property('SustainSection', '')
        entry.set_editor_property('RecoverySection', '')
        entry.set_editor_property('MotionSource', motion.IMPACT)
        entry.set_editor_property('LandingMotionSource', motion.ANIMATION_ROOT_MOTION)
        entry.set_editor_property('GetUpMontage', montages['StandUp'])
        entry.set_editor_property('GetUpMotionSource', motion.ANIMATION_ROOT_MOTION)
        if entry.type in [unreal.HodgeImpactType.HIT_STUN, unreal.HodgeImpactType.KNOCKBACK]:
            variants = []
            for side, direction in [('L', unreal.HodgeHitPresentationDirection.LEFT), ('R', unreal.HodgeHitPresentationDirection.RIGHT)]:
                strengths = [('S', .1), ('B', 1.0)] if entry.type == unreal.HodgeImpactType.HIT_STUN else [('B', .1)]
                for size, threshold in strengths:
                    variant = unreal.HodgeHitReactionVariant()
                    variant.set_editor_property('Direction', direction)
                    variant.set_editor_property('MinimumStrength', threshold)
                    variant.set_editor_property('Montage', montages['Behit_' + size + '_' + side])
                    variants.append(variant)
            entry.set_editor_property('Variants', variants)
            entry.set_editor_property('Montage', montages['Behit_S_R' if entry.type == unreal.HodgeImpactType.HIT_STUN else 'Behit_B_R'])
            entry.set_editor_property('MotionSource', motion.ANIMATION_ROOT_MOTION)
            entry.set_editor_property('bUseLateralVariants', True)
        elif entry.type in [unreal.HodgeImpactType.LAUNCH, unreal.HodgeImpactType.AIR_HIT, unreal.HodgeImpactType.SLAM]:
            # Slam 暂复用已有空中受力动作，不伪造一条新的专用下砸动画。
            family = 'Behit_Fly' if entry.type == unreal.HodgeImpactType.LAUNCH else 'Behit_Push'
            for field, suffix in [('Montage', '_Start'), ('AirLoopMontage', '_Loop'), ('LandingMontage', '_Fall')]:
                entry.set_editor_property(field, montages[family + suffix])
            entry.set_editor_property('AirLoopTransitionTime', .18)
        elif entry.type == unreal.HodgeImpactType.KNOCKDOWN:
            entry.set_editor_property('Montage', montages['Behit_Fly_Fall'])
            entry.set_editor_property('MotionSource', motion.ANIMATION_ROOT_MOTION)
        # 地面条目不再携带没有用途的空中循环引用，避免运动契约混淆。
        if entry.type in [unreal.HodgeImpactType.HIT_STUN, unreal.HodgeImpactType.KNOCKBACK, unreal.HodgeImpactType.KNOCKDOWN]:
            entry.set_editor_property('AirLoopMontage', None)
            entry.set_editor_property('LandingMontage', montages['Behit_Fly_Fall'])
    profile.set_editor_property('Animations', entries)
    profile.set_editor_property('bSustainGroundHitStun', False)
    profile.set_editor_property('bAllowNaturalRecoveryTail', True)
    profile.set_editor_property('bAllowRootMotionRecoveryTail', True)
    profile.set_editor_property('GroundReplayInterval', .16)
    profile.set_editor_property('LightFeedbackMontage', lights['R'])
    light_entries = []
    for direction, side in [(unreal.HodgeHitPresentationDirection.FRONT, 'R'), (unreal.HodgeHitPresentationDirection.BACK, 'L'),
                            (unreal.HodgeHitPresentationDirection.LEFT, 'L'), (unreal.HodgeHitPresentationDirection.RIGHT, 'R')]:
        value = unreal.HodgeDirectionalHitFeedback()
        value.set_editor_property('Direction', direction)
        value.set_editor_property('Montage', lights[side])
        light_entries.append(value)
    profile.set_editor_property('DirectionalLightFeedback', light_entries)
    save(profile)
    report['profiles'].append(profile.get_path_name())

for index, attack in enumerate(attacks):
    original = attack.get_editor_property('DefaultHitConfig').export_text()
    execution = attack.get_editor_property('ExecutionConfig').export_text()
    hit = attack.get_editor_property('DefaultHitConfig')
    reaction = hit.get_editor_property('Reaction')
    reaction.set_editor_property('PresentationSide', unreal.HodgeHitSide.LEFT if index % 2 == 0 else unreal.HodgeHitSide.RIGHT)
    if index < 2:
        impact = unreal.HodgeImpactSpec()
        impact.set_editor_property('ControlDuration', .16 if index == 0 else .18)
        reaction.set_editor_property('Impacts', [impact])
    hit.set_editor_property('Reaction', reaction)
    attack.set_editor_property('DefaultHitConfig', hit)
    assert attack.get_editor_property('ExecutionConfig').export_text() == execution
    save(attack)
    report['attacks'].append({'asset': attack.get_path_name(), 'before': original, 'after': hit.export_text()})
for name, source in sources.items():
    file = PROJECT.parent / 'Content/Wuwa/Anim/BaseAnim' / (name + '.uasset')
    assert hashlib.sha256(file.read_bytes()).hexdigest() == report['sources'][name]['sha256']
(OUT / 'migration.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
print('AUTHORED_BEHIT_MIGRATED ' + str(len(report['created'])))
