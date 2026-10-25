"""冷启动只读核对正式接线、原参数与蓝图加载状态；不重存无关资产。"""
from pathlib import Path
import hashlib
import json
import unreal

ROOT = Path(unreal.Paths.project_dir()).resolve()
OUT = ROOT / 'Saved/PlayerRespawn_20261024'
pawn = unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData')
ability_set = unreal.load_asset('/Game/Main/Data/AbilitySet/DA_Pover')
assert pawn and ability_set
assert pawn.get_editor_property('bEnableAutoRespawn')
assert pawn.get_editor_property('AutoRespawnDelay') == 0.
assert pawn.get_editor_property('AutoRespawnMaxAttempts') == 3
assert pawn.get_editor_property('AutoRespawnRetryInterval') == .5
assert pawn.get_editor_property('bUseInventory') and pawn.get_editor_property('Poise').get_editor_property('bEnabled')
entries = list(ability_set.get_editor_property('GrantedGameplayAbilities'))
matches = [x for x in entries if x.get_editor_property('Ability').get_path_name() == '/Script/Hodgepodge.HodgeGameplayAbility_AutoRespawn']
assert len(matches) == 1 and matches[0].get_editor_property('AbilityLevel') == 1
records = []
for asset in [pawn, ability_set]:
    data = unreal.EditorAssetLibrary.find_asset_data(asset.get_path_name())
    result, errors, warnings = unreal.get_editor_subsystem(unreal.EditorValidatorSubsystem).is_asset_valid(data, unreal.DataValidationUsecase.SCRIPT)
    assert result == unreal.DataValidationResult.VALID, (asset.get_path_name(), errors)
    records.append({'asset': asset.get_path_name(), 'validation': str(result), 'warnings': [str(x) for x in warnings]})
# 本次没有更改这些蓝图的父类或图表，只检查冷加载状态和死亡时长。
for path in ['/Game/Main/Character/Hero/BP_Hero_Pover', '/Game/Main/Character/Hero/Ability/GA_Hero_Death']:
    blueprint = unreal.load_asset(path)
    assert blueprint and blueprint.generated_class()
    records.append({'blueprint': path, 'generated_class': blueprint.generated_class().get_path_name(), 'compiled_by_this_script': False})
death = unreal.get_default_object(unreal.load_asset('/Game/Main/Character/Hero/Ability/GA_Hero_Death').generated_class())
assert death.get_editor_property('Duration') == 2.5
guardian = ROOT / 'Content/Main/Character/Enemies/Guardian/DA_Guardian_EquippedPawnData.uasset'
assert hashlib.sha256(guardian.read_bytes()).hexdigest() == '73a268436808145d5ac0ee8e2973f0a25216b1ac5320bf720336a861c823c0f1'
(OUT / 'asset-validation.json').write_text(json.dumps({'passed': True, 'records': records, 'death_duration': 2.5, 'guardian_unchanged': True}, ensure_ascii=False, indent=2), encoding='utf-8')
unreal.log('HERO_AUTO_RESPAWN_COLD_ASSETS_VALID')
