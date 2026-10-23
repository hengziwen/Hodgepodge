"""补齐本批既有 StandUp 根运动和 Slam 空中素材配对，不重跑或覆盖其他动作。"""
from pathlib import Path
import hashlib
import json
import unreal

assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
root = '/Game/Main/Character/Hero/Anim/HitReactions/Behit'
out = Path(unreal.Paths.project_saved_dir()) / 'ACTCombat/Behit'
assert (out / 'migration.json').exists()
assert not unreal.EditorAssetLibrary.does_asset_exist(root + '/AM_StandUp'), '已补齐，停止覆盖手调'
source = unreal.load_asset('/Game/Wuwa/Anim/BaseAnim/StandUp')
pawn = unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData')
mesh = unreal.get_default_object(pawn.get_editor_property('PawnClass')).get_component_by_class(unreal.SkeletalMeshComponent).get_editor_property('SkeletalMesh')
audit = json.loads(unreal.HodgeAnimationAuthoringLibrary.inspect_behit_animation(source, mesh))
assert not audit['missing_bones'] and not audit['parent_mismatch']
assert audit['max_ref_translation_error'] < .1 and audit['max_ref_rotation_degrees'] < .1
file = Path(unreal.Paths.project_content_dir()) / 'Wuwa/Anim/BaseAnim/StandUp.uasset'
audit['sha256'] = hashlib.sha256(file.read_bytes()).hexdigest()
sequence = unreal.EditorAssetLibrary.duplicate_asset(source.get_path_name().split('.')[0], root + '/A_StandUp')
skeleton = unreal.load_asset('/Game/Main/Character/Hero/Anim/Model/SK_Pover_LyraLab')
assert sequence and unreal.HodgeAnimationAuthoringLibrary.set_hero_reaction_skeleton(sequence, skeleton)
sequence.set_editor_property('enable_root_motion', True)
sequence.set_editor_property('bForceRootLock', False)
sequence.set_editor_property('root_motion_root_lock', unreal.RootMotionRootLock.ANIM_FIRST_FRAME)
factory = unreal.AnimMontageFactory()
factory.set_editor_property('TargetSkeleton', skeleton)
factory.set_editor_property('SourceAnimation', sequence)
montage = unreal.AssetToolsHelpers.get_asset_tools().create_asset('AM_StandUp', root, unreal.AnimMontage, factory)
assert montage and unreal.HodgeAnimationAuthoringLibrary.set_hero_reaction_montage_slot(montage, 'FullBody')
assert unreal.HodgeAnimationAuthoringLibrary.configure_behit_montage(montage, 0.0, sequence.get_play_length(), 1.0)
for asset in [sequence, montage]:
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset, False)
for path in ['/Game/Main/Character/Hero/Anim/HitReactions/DA_Hero_HitReaction',
             '/Game/CodexText/HitReactionValidation/Dummy/DA_DummyHitReaction']:
    profile = unreal.load_asset(path)
    entries = list(profile.get_editor_property('Animations'))
    for entry in entries:
        assert entry.get_editor_property('GetUpMontage').get_name() == 'AM_Hero_GetUp', '起身配置已被手调，停止替换'
        entry.set_editor_property('GetUpMontage', montage)
        entry.set_editor_property('GetUpMotionSource', unreal.HodgeReactionMotionSource.ANIMATION_ROOT_MOTION)
        if entry.type == unreal.HodgeImpactType.SLAM:
            for field, suffix in [('Montage', 'Start'), ('AirLoopMontage', 'Loop'), ('LandingMontage', 'Fall')]:
                entry.set_editor_property(field, unreal.load_asset(root + '/AM_Behit_Push_' + suffix))
    profile.set_editor_property('Animations', entries)
    assert unreal.EditorAssetLibrary.save_loaded_asset(profile, False)
source_audit = json.loads((out / 'source-audit.json').read_text(encoding='utf-8'))
source_audit['StandUp'] = audit
(out / 'source-audit.json').write_text(json.dumps(source_audit, ensure_ascii=False, indent=2), encoding='utf-8')
assert hashlib.sha256(file.read_bytes()).hexdigest() == audit['sha256']
(out / 'completion.json').write_text(json.dumps({'get_up': montage.get_path_name(), 'slam_family': 'Push', 'generated_bone_keys': False}, indent=2), encoding='utf-8')
print('AUTHORED_BEHIT_COMPLETED')
