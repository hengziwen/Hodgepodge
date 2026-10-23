"""接入已授权的玩家恢复参数与强动作样例，先备份且不触碰攻击数值或通知。"""
from pathlib import Path
import json
import unreal

ROOT = '/Game/Tests/Combat/ACTPrototype'
tools = unreal.AssetToolsHelpers.get_asset_tools()
report = {'modified': [], 'backups': [], 'error': None}


def backup(asset):
    """存在备份时禁止重跑覆盖；手工调整由用户保留。"""
    name = asset.get_name() + '_BeforeACT'
    path = ROOT + '/Backups/' + name
    assert not unreal.EditorAssetLibrary.does_asset_exist(path), '已有备份，停止覆盖 ' + path
    copy = tools.duplicate_asset(name, ROOT + '/Backups', asset)
    assert copy and unreal.EditorAssetLibrary.save_loaded_asset(copy, False)
    report['backups'].append(copy.get_path_name())


try:
    assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
    profile = unreal.load_asset('/Game/Main/Character/Hero/Anim/HitReactions/DA_Hero_HitReaction')
    dummy = unreal.load_asset('/Game/CodexText/HitReactionValidation/Dummy/DA_DummyHitReaction')
    assert profile and dummy
    # 预先核实动作样例完整，失败时不修改正式 Profile。
    source = next(e for e in dummy.get_editor_property('Animations') if e.type == unreal.HodgeImpactType.HIT_STUN)
    assert str(source.sustain_section) == 'Sustain' and len(source.variants) == 8
    backup(profile)
    entries = list(profile.get_editor_property('Animations'))
    for entry in entries:
        if entry.type in [unreal.HodgeImpactType.HIT_STUN, unreal.HodgeImpactType.KNOCKBACK]:
            entry.set_editor_property('Montage', source.montage)
            entry.set_editor_property('Variants', source.variants)
            entry.set_editor_property('SustainSection', 'Sustain')
            entry.set_editor_property('RecoverySection', 'Recover')
    profile.set_editor_property('Animations', entries)
    for name, value in [('bEnableGroundRecovery', True), ('DodgeRecoveryDelay', .2), ('AttackRecoveryDelay', .3),
                        ('MovementRecoveryDelay', .4), ('bAllowNaturalRecoveryTail', True),
                        ('KnockbackStrengthCurve', dummy.get_editor_property('KnockbackStrengthCurve'))]:
        profile.set_editor_property(name, value)
    assert unreal.EditorAssetLibrary.save_loaded_asset(profile, False)
    report['modified'].append(profile.get_path_name())
    for number in [1, 2, 3, 5, 4]:
        asset = unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_' + str(number))
        assert asset
        before = asset.get_editor_property('DefaultHitConfig').export_text()
        execution = asset.get_editor_property('ExecutionConfig').export_text()
        backup(asset)
        asset.set_editor_property('bAllowHitRecoveryAttack', True)
        assert before == asset.get_editor_property('DefaultHitConfig').export_text()
        assert execution == asset.get_editor_property('ExecutionConfig').export_text()
        assert unreal.EditorAssetLibrary.save_loaded_asset(asset, False)
        report['modified'].append(asset.get_path_name())
except Exception as error:
    report['error'] = str(error)
    raise
finally:
    output = Path(unreal.Paths.project_saved_dir()) / 'ACTCombat' / 'player-recovery-assets.json'
    output.write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
