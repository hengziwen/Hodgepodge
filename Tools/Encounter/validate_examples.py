"""只读检查双模式示例接线、批准参数、波次和源资产完整性。"""
from pathlib import Path
import hashlib
import json
import unreal

ROOT = Path(unreal.Paths.project_dir()).resolve()
OUT = ROOT / 'Saved/Encounter_20261025'
checks = []


def check(name, value):
    """校验失败不保存资产，结果逐项留证。"""
    checks.append({'name': name, 'passed': bool(value)})
    assert value, name


error = None
try:
    for name, mode in [('DA_Encounter_Interaction', unreal.HodgeEncounterMode.INTERACTION),
                       ('DA_Encounter_Area', unreal.HodgeEncounterMode.AREA)]:
        asset = unreal.load_asset('/Game/Main/Data/Encounters/' + name)
        check(name + ' 可加载且模式正确', asset and asset.get_editor_property('Mode') == mode)
        data = unreal.EditorAssetLibrary.find_asset_data(asset.get_path_name())
        result, errors, warnings = unreal.get_editor_subsystem(unreal.EditorValidatorSubsystem).is_asset_valid(data, unreal.DataValidationUsecase.SCRIPT)
        check(name + ' 原生配置校验通过', result == unreal.DataValidationResult.VALID and not errors)
        waves = list(asset.get_editor_property('Waves'))
        check(name + ' 两波分别1和2只', len(waves) == 2 and [len(w.get_editor_property('Enemies')) for w in waves] == [1, 2])
        check(name + ' 初始化等待上限10秒', asset.get_editor_property('EnemyInitializationTimeout') == 10)
        for wave in waves:
            check(name + ' 不增加额外等待', wave.get_editor_property('DelayBeforeSpawn') == 0)
            for enemy in wave.get_editor_property('Enemies'):
                check(name + ' 使用正式Guardian等级1', enemy.get_editor_property('Level') == 1 and
                      enemy.get_editor_property('PawnData').get_path_name().split('.')[0] ==
                      '/Game/Main/Character/Enemies/Guardian/DA_Guardian_EquippedPawnData')
        if mode == unreal.HodgeEncounterMode.INTERACTION:
            check('交互奖励为测试材料一件一级', asset.get_editor_property('RewardCount') == 1 and asset.get_editor_property('RewardLevel') == 1 and
                  asset.get_editor_property('RewardDefinition').get_path_name() == '/Game/Main/Data/Inventory/ID_TestMaterial.ID_TestMaterial_C')
        else:
            check('区域重复与30秒冷却', asset.get_editor_property('bRepeatable') and asset.get_editor_property('CooldownSeconds') == 30)
            check('区域使用进入边沿', asset.get_editor_property('TriggerEdge') == unreal.HodgeEncounterTriggerEdge.ENTER)
            extent = asset.get_editor_property('AreaHalfExtent')
            check('区域Box半尺寸300/300/150厘米', extent == unreal.Vector(300, 300, 150))
            check('区域不配置领奖', not asset.get_editor_property('RewardDefinition'))
    setup = json.loads((OUT / 'examples-setup.json').read_text(encoding='utf-8'))
    source = ROOT / 'Content/Main/Character/Enemies/Guardian/DA_Guardian_EquippedPawnData.uasset'
    check('正式Guardian源资产未修改', hashlib.sha256(source.read_bytes()).hexdigest() == setup['source_sha256'])
    check('独立示例关卡已保存', unreal.EditorAssetLibrary.does_asset_exist('/Game/Main/Maps/L_EncounterValidation'))
    check('没有遗留未保存内容', not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages() and
          not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages())
except Exception as exception:
    error = str(exception)
    raise
finally:
    OUT.mkdir(parents=True, exist_ok=True)
    (OUT / 'examples-validation.json').write_text(json.dumps({'passed': error is None, 'checks': checks, 'error': error},
                                                         ensure_ascii=False, indent=2), encoding='utf-8')
unreal.log('ENCOUNTER_EXAMPLES_VALID')
