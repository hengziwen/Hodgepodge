"""备份后仅接入已批准的 F 键交互；保留现有能力、按键及手工配置。"""
from pathlib import Path
import hashlib
import json
import shutil
import unreal

ROOT = Path(unreal.Paths.project_dir()).resolve()
OUT = ROOT / 'Saved/Encounter_20261025'
assert ROOT == Path('E:/Project/Git/Hodgepodge')
assert not unreal.EditorLevelLibrary.get_pie_worlds(True), '请先结束 PIE'
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages(), '存在未保存关卡'
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages(), '存在未保存内容，不能覆盖'
OUT.mkdir(parents=True, exist_ok=True)
ACTION = '/Game/Main/Input/InputAction/IA_Interact'
INPUT = '/Game/Main/Input/DA_HodgeInputConfig'
CONTEXT = '/Game/Main/Input/IMC_Default'
SET = '/Game/Main/Data/AbilitySet/DA_Pover'
TAG = 'InputTag.Ability.Interact'
ABILITY = '/Script/Hodgepodge.HodgeGameplayAbility_Interact'
report = {'assets': [], 'references': {}, 'error': None}


def backup(asset):
    """首次文件备份不覆盖，避免重跑丢失用户的原始配置。"""
    package = asset.get_path_name().split('.')[0]
    source = ROOT / 'Content' / (package.removeprefix('/Game/') + '.uasset')
    target = OUT / 'BeforeInteraction' / source.relative_to(ROOT)
    target.parent.mkdir(parents=True, exist_ok=True)
    if not target.exists():
        shutil.copy2(source, target)
    report['assets'].append({'asset': package, 'backup': str(target),
                             'before_sha256': hashlib.sha256(source.read_bytes()).hexdigest()})
    report['references'][package] = list(unreal.EditorAssetLibrary.find_package_referencers_for_asset(package, False))


try:
    inputs, context, abilities = [unreal.load_asset(p) for p in [INPUT, CONTEXT, SET]]
    ability_class = unreal.load_class(None, ABILITY)
    assert inputs and context and abilities and ability_class, '先完成常规编译并重启编辑器'
    pawn = unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData')
    assert pawn and pawn.get_editor_property('InputConfig') == inputs and abilities in pawn.get_editor_property('AbilitySets')
    assert pawn.get_editor_property('InteractionDistance') > 0, '正式 Hero 交互配置无效'
    action = unreal.load_asset(ACTION)
    for asset in [inputs, context, abilities] + ([action] if action else []):
        backup(asset)

    # 修改前拒绝 F 冲突和同标签多来源，不覆盖已有手工布局。
    mappings = list(context.get_editor_property('mappings'))
    assert all(m.get_editor_property('action') == action for m in mappings
               if m.get_editor_property('key').export_text() == 'F'), 'F 已绑定其他动作'
    rows = list(inputs.get_editor_property('AbilityInputActions'))
    tagged = [r for r in rows if str(r.get_editor_property('InputTag').get_editor_property('TagName')) == TAG]
    assert len(tagged) <= 1, '交互输入标签重复'
    assert not tagged or tagged[0].get_editor_property('InputAction') == action, '交互标签已有其他动作'
    grants = list(abilities.get_editor_property('GrantedGameplayAbilities'))
    existing = [g for g in grants if g.get_editor_property('Ability') == ability_class]
    assert len(existing) <= 1, '交互能力重复授予'
    assert not existing or (existing[0].get_editor_property('AbilityLevel') == 1 and
                            str(existing[0].get_editor_property('InputTag').get_editor_property('TagName')) == TAG), '停止以保留手工能力配置'

    if action is None:
        action = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
            'IA_Interact', '/Game/Main/Input/InputAction', unreal.InputAction, unreal.InputAction_Factory())
        assert action
        action.set_editor_property('value_type', unreal.InputActionValueType.BOOLEAN)
        # 按下沿只发一次请求，避免持续按键在完成、领奖、重置间连续执行。
        pressed = unreal.new_object(unreal.InputTriggerPressed, outer=action)
        action.set_editor_property('triggers', [pressed])
    else:
        assert action.get_editor_property('value_type') == unreal.InputActionValueType.BOOLEAN
        triggers = list(action.get_editor_property('triggers'))
        assert len(triggers) == 1 and isinstance(triggers[0], unreal.InputTriggerPressed), '停止以保留手工触发器配置'

    if not tagged:
        row = unreal.HodgeInputAction()
        assert row.import_text('(InputAction="' + action.get_path_name() + '",InputTag=(TagName="' + TAG + '"))')
        rows.append(row)
        inputs.set_editor_property('AbilityInputActions', rows)
    action_mappings = [m for m in mappings if m.get_editor_property('action') == action]
    assert len(action_mappings) <= 1 and all(m.get_editor_property('key').export_text() == 'F' for m in action_mappings)
    if not action_mappings:
        key = unreal.Key()
        assert key.import_text('F')
        context.map_key(action, key)
    if not existing:
        grant = unreal.HodgeAbilitySet_GameplayAbility()
        assert grant.import_text('(Ability="' + ABILITY + '",AbilityLevel=1,InputTag=(TagName="' + TAG + '"))')
        grants.append(grant)
        abilities.set_editor_property('GrantedGameplayAbilities', grants)

    # 先核对整条接线，再定向保存，不能批量保存编辑器工作区。
    assert len(inputs.get_editor_property('AbilityInputActions')) == len(rows)
    assert len(context.get_editor_property('mappings')) == len(mappings) + (0 if action_mappings else 1)
    assert len(abilities.get_editor_property('GrantedGameplayAbilities')) == len(grants)
    changed = [action, inputs, context, abilities]
    validator = unreal.get_editor_subsystem(unreal.EditorValidatorSubsystem)
    for asset in changed:
        data = unreal.EditorAssetLibrary.find_asset_data(asset.get_path_name())
        result, errors, warnings = validator.is_asset_valid(data, unreal.DataValidationUsecase.SCRIPT)
        assert result != unreal.DataValidationResult.INVALID and not errors, (asset.get_path_name(), errors)
    for asset in changed:
        assert unreal.EditorAssetLibrary.save_loaded_asset(asset, False), asset.get_path_name()
    report['saved'] = [a.get_path_name() for a in changed]
except Exception as error:
    report['error'] = str(error)
    raise
finally:
    (OUT / 'interaction-setup.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
unreal.log('ENCOUNTER_INTERACTION_CONFIGURED')
