"""一次性备份并接入目标削韧与原地 Behit 副本；不改源动画、伤害或攻击时机。"""
from pathlib import Path
import hashlib
import json
import traceback
import unreal

PROJECT = Path(unreal.Paths.get_project_file_path()).resolve()
assert PROJECT == Path('E:/Project/Git/Hodgepodge/Hodgepodge.uproject')
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
OUT = PROJECT.parent / 'Saved/PoiseReaction'
OUT.mkdir(parents=True, exist_ok=True)
ROOT = '/Game/Main/Character/Hero/Anim/HitReactions/Behit'
DEST = ROOT + '/InPlace'
BACKUP = '/Game/Tests/Combat/PoiseMigration/Backups'
report = {'error': None, 'backups': [], 'created': [], 'profiles': [], 'pawn_data': [], 'attacks': [], 'source_hashes': {}}


def save(asset):
    """只保存本轮列出的资产，不保存共享编辑器其他脏包。"""
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset, False), asset.get_path_name()


def clone(value):
    """复制反射结构，避免修改包装器时连同原配置一起变化。"""
    result = type(value)()
    result.import_text(value.export_text())
    return result


def backup(asset):
    """已存在备份时停止，不覆盖用户后来手调的配置。"""
    path = BACKUP + '/' + asset.get_name() + '_BeforePoise'
    assert not unreal.EditorAssetLibrary.does_asset_exist(path), path
    copied = unreal.EditorAssetLibrary.duplicate_asset(asset.get_path_name().split('.')[0], path)
    assert copied
    save(copied)
    report['backups'].append({'asset': asset.get_path_name(), 'backup': path})


try:
    assert not unreal.EditorAssetLibrary.does_directory_exist(DEST), '本批已存在，请审查而非覆盖重跑'
    profiles = [unreal.load_asset(p) for p in [
        '/Game/Main/Character/Hero/Anim/HitReactions/DA_Hero_HitReaction',
        '/Game/CodexText/HitReactionValidation/Dummy/DA_DummyHitReaction',
        '/Game/Main/Character/Enemies/Guardian/DA_Guardian_HitReaction']]
    assert all(profiles)
    registry = unreal.AssetRegistryHelpers.get_asset_registry()
    pawn_data = [a.get_asset() for a in registry.get_assets_by_class(unreal.TopLevelAssetPath('/Script/Hodgepodge', 'HodgePawnData'))
                 if str(a.package_name).startswith(('/Game/Main/', '/Game/CodexText/HitReactionValidation/Dummy/'))]
    targets = [p for p in pawn_data if p.get_editor_property('HitReactionProfile') in profiles]
    assert len(targets) >= 3, [p.get_path_name() for p in targets]
    attacks = [unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_' + str(i)) for i in [1, 2, 3, 5, 4]]
    liberation = unreal.load_asset('/Game/Main/Character/Hero/Ability/ResonanceLiberation/DA_Resonance_Liberation')
    for asset in profiles + targets + attacks + [liberation]:
        assert asset
        backup(asset)
    names = ['Behit_S_L', 'Behit_S_R', 'Behit_B_L', 'Behit_B_R', 'Behit_Fly_Start', 'Behit_Fly_Loop',
             'Behit_Fly_Fall', 'Behit_Push_Start', 'Behit_Push_Loop', 'Behit_Push_Fall', 'StandUp']
    montages = {}
    for name in names:
        source_file = PROJECT.parent / ('Content/Wuwa/Anim/BaseAnim/' + name + '.uasset')
        report['source_hashes'][name] = hashlib.sha256(source_file.read_bytes()).hexdigest()
        source = unreal.load_asset(ROOT + '/A_' + name)
        seq = unreal.EditorAssetLibrary.duplicate_asset(source.get_path_name().split('.')[0], DEST + '/A_IP_' + name)
        assert seq
        # 强制锁定实际根轨道，不能只关闭提取而让 Root 在 Mesh 中继续走位。
        seq.set_editor_property('enable_root_motion', False)
        seq.set_editor_property('bForceRootLock', True)
        seq.set_editor_property('root_motion_root_lock', unreal.RootMotionRootLock.ANIM_FIRST_FRAME)
        factory = unreal.AnimMontageFactory()
        factory.set_editor_property('TargetSkeleton', seq.get_editor_property('Skeleton'))
        factory.set_editor_property('SourceAnimation', seq)
        montage = unreal.AssetToolsHelpers.get_asset_tools().create_asset('AM_IP_' + name, DEST, unreal.AnimMontage, factory)
        assert montage and unreal.HodgeAnimationAuthoringLibrary.set_hero_reaction_montage_slot(montage, 'FullBody')
        # B_L 不播放已证实异常的 1.18~1.27s 区间；保留原片及完整备份供动画师修复。
        end = min(seq.get_play_length(), 1.10 if name == 'Behit_B_L' else
                  .70 if name.startswith('Behit_S_') else 1.0 if name == 'Behit_B_R' else seq.get_play_length())
        assert unreal.HodgeAnimationAuthoringLibrary.configure_behit_montage(montage, 0.0, end, 1.0)
        for field, duration in [('BlendIn', .05), ('BlendOut', .15)]:
            blend = clone(montage.get_editor_property(field))
            blend.import_text('(BlendTime=' + str(duration) + ')')
            montage.set_editor_property(field, blend)
        for asset in [seq, montage]:
            unreal.EditorAssetLibrary.set_metadata_tag(asset, 'Hodge.PoiseSource', source.get_path_name())
            save(asset)
            report['created'].append(asset.get_path_name())
        montages[name] = montage
    for profile in profiles:
        old = list(profile.get_editor_property('Animations'))
        compact = unreal.HodgeCompactReactionAnimations()
        if profile != profiles[2]:
            for field, prefix, duration in [('Light', 'Behit_S_', .4), ('Heavy', 'Behit_B_', .55)]:
                pair = unreal.HodgeGroundReactionPair()
                pair.set_editor_property('Left', montages[prefix + 'L'])
                pair.set_editor_property('Right', montages[prefix + 'R'])
                pair.set_editor_property('ControlDuration', duration)
                compact.set_editor_property(field, pair)
            for field, prefix in [('Fly', 'Behit_Fly'), ('Push', 'Behit_Push')]:
                family = unreal.HodgeAirReactionFamily()
                for key, suffix in [('Start', '_Start'), ('Loop', '_Loop'), ('Landing', '_Fall')]:
                    family.set_editor_property(key, montages[prefix + suffix])
                compact.set_editor_property(field, family)
            compact.set_editor_property('Downed', montages['Behit_Fly_Fall'])
            compact.set_editor_property('GetUp', montages['StandUp'])
        else:
            # Guardian 继续使用自己的骨架与三种现有动作，不强套 Hero 的击飞素材。
            for field, kind in [('Light', unreal.HodgeImpactType.HIT_STUN), ('Heavy', unreal.HodgeImpactType.KNOCKBACK)]:
                entry = next(e for e in old if e.type == kind)
                pair = unreal.HodgeGroundReactionPair()
                pair.set_editor_property('Left', entry.montage)
                pair.set_editor_property('Right', entry.montage)
                pair.set_editor_property('ControlDuration', entry.get_editor_property('DefaultDuration'))
                compact.set_editor_property(field, pair)
            down = next(e for e in old if e.type == unreal.HodgeImpactType.KNOCKDOWN)
            compact.set_editor_property('Downed', down.montage)
            compact.set_editor_property('GetUp', down.get_editor_property('GetUpMontage'))
        profile.set_editor_property('Compact', compact)
        profile.set_editor_property('bUseCompactAnimations', True)
        profile.set_editor_property('bAllowRootMotionRecoveryTail', False)
        profile.set_editor_property('bAllowNaturalRecoveryTail', True)
        profile.set_editor_property('bSustainGroundHitStun', True)
        save(profile)
        report['profiles'].append({'asset': profile.get_path_name(), 'compact': compact.export_text(), 'legacy_retained': len(old)})
    for data in targets:
        settings = unreal.HodgePoiseSettings()
        settings.set_editor_property('bEnabled', True)
        settings.set_editor_property('BreakImpact', unreal.HodgeImpactType.KNOCKDOWN if data.get_editor_property('HitReactionProfile') == profiles[2] else unreal.HodgeImpactType.LAUNCH)
        data.set_editor_property('Poise', settings)
        save(data)
        report['pawn_data'].append({'asset': data.get_path_name(), 'poise': settings.export_text()})
    # 演示削韧值按正式连段顺序设置；真实命中次数由既有 HitGroup/Phase 去重负责。
    for attack, amount in zip(attacks, [15., 20., 25., 25., 80.]):
        hit = clone(attack.get_editor_property('DefaultHitConfig'))
        before = hit.export_text()
        reaction = clone(hit.get_editor_property('Reaction'))
        reaction.set_editor_property('InputMode', unreal.HodgeReactionInputMode.POISE_DRIVEN)
        reaction.set_editor_property('ReservedPoiseDamage', amount)
        reaction.set_editor_property('Impacts', [])
        hit.set_editor_property('Reaction', reaction)
        attack.set_editor_property('DefaultHitConfig', hit)
        save(attack)
        report['attacks'].append({'asset': attack.get_path_name(), 'before': before, 'after': hit.export_text()})
    hit = clone(liberation.get_editor_property('DefaultHitConfig'))
    reaction = clone(hit.get_editor_property('Reaction'))
    reaction.set_editor_property('InputMode', unreal.HodgeReactionInputMode.EXPLICIT_IMPACT)
    reaction.set_editor_property('ReservedPoiseDamage', 0.)
    hit.set_editor_property('Reaction', reaction)
    liberation.set_editor_property('DefaultHitConfig', hit)
    save(liberation)
    for name, sha in report['source_hashes'].items():
        assert hashlib.sha256((PROJECT.parent / ('Content/Wuwa/Anim/BaseAnim/' + name + '.uasset')).read_bytes()).hexdigest() == sha
    print('POISE_MIGRATION_COMPLETE')
except Exception:
    report['error'] = traceback.format_exc()
    raise
finally:
    (OUT / 'migration.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
