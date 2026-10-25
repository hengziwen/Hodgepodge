"""备份后接入正式 Hero 自动重生；只修改基础 AbilitySet 与现有 PawnData。"""
from pathlib import Path
import hashlib
import json
import shutil
import unreal

ROOT = Path(unreal.Paths.project_dir()).resolve()
assert ROOT == Path('E:/Project/Git/Hodgepodge')
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
OUT = ROOT / 'Saved/PlayerRespawn_20261024'
OUT.mkdir(parents=True, exist_ok=True)
PAWN = '/Game/Main/Data/PawnData/DA_Dafult_PawnData'
SET = '/Game/Main/Data/AbilitySet/DA_Pover'
GA = '/Script/Hodgepodge.HodgeGameplayAbility_AutoRespawn'
report = {'assets': [], 'references': {}, 'error': None}


def backup(asset):
    """保存首次基线，不覆盖历史备份；已有制作改动由读回断言保护。"""
    package = asset.get_path_name().split('.')[0]
    file = ROOT / 'Content' / (package.removeprefix('/Game/') + '.uasset')
    dest = OUT / 'BeforeAutoRespawn' / file.relative_to(ROOT)
    dest.parent.mkdir(parents=True, exist_ok=True)
    if not dest.exists():
        shutil.copy2(file, dest)
    return {'asset': package, 'backup': str(dest), 'before_sha256': hashlib.sha256(file.read_bytes()).hexdigest()}


def validate(asset):
    """执行正式数据校验，失败时不保存该资产。"""
    data = unreal.EditorAssetLibrary.find_asset_data(asset.get_path_name())
    result, errors, warnings = unreal.get_editor_subsystem(unreal.EditorValidatorSubsystem).is_asset_valid(
        data, unreal.DataValidationUsecase.SCRIPT)
    assert result == unreal.DataValidationResult.VALID, (asset.get_path_name(), result, errors)
    return [str(x) for x in warnings]


try:
    pawn, ability_set = unreal.load_asset(PAWN), unreal.load_asset(SET)
    ability_class = unreal.load_class(None, GA)
    assert pawn and ability_set and ability_class
    assert pawn.get_editor_property('bUseInventory') and pawn.get_editor_property('Poise').get_editor_property('bEnabled')
    assert pawn.get_editor_property('PawnClass').get_path_name() == '/Game/Main/Character/Hero/BP_Hero_Pover.BP_Hero_Pover_C'
    assert ability_set in pawn.get_editor_property('AbilitySets')
    report['references'][SET] = list(unreal.EditorAssetLibrary.find_package_referencers_for_asset(SET, False))
    records = [backup(pawn), backup(ability_set)]
    entries = list(ability_set.get_editor_property('GrantedGameplayAbilities'))
    matches = [entry for entry in entries if entry.get_editor_property('Ability') == ability_class]
    assert len(matches) <= 1, '基础来源重复授予 AutoRespawn'
    if not matches:
        entry = unreal.HodgeAbilitySet_GameplayAbility()
        # EditDefaultsOnly 的结构体成员沿用已有 Havoc 配置工具的整条文本导入方式。
        assert entry.import_text('(Ability="' + GA + '",AbilityLevel=1)')
        entries.append(entry)
        ability_set.set_editor_property('GrantedGameplayAbilities', entries)
    settings = {'AutoRespawnDelay': 0.0, 'AutoRespawnMaxAttempts': 3, 'AutoRespawnRetryInterval': 0.5}
    if pawn.get_editor_property('bEnableAutoRespawn'):
        for name, value in settings.items():
            assert pawn.get_editor_property(name) == value, '停止以保留后续手调：' + name
    else:
        for name, value in settings.items():
            pawn.set_editor_property(name, value)
        pawn.set_editor_property('bEnableAutoRespawn', True)
    # 两份都通过校验后才开始保存，失败明确留证，不批量重存其他资产。
    for record, asset in zip(records, [pawn, ability_set]):
        record['warnings'] = validate(asset)
    for record, asset in zip(records, [pawn, ability_set]):
        assert unreal.EditorAssetLibrary.save_loaded_asset(asset, False)
        report['assets'].append(record)
    report['settings'] = settings
    report['grants'] = len(entries)
except Exception as error:
    report['error'] = str(error)
    raise
finally:
    (OUT / 'asset-setup.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
unreal.log('HERO_AUTO_RESPAWN_ASSETS_CONFIGURED')
