"""冷加载只读核对 F → InputTag → 本地交互 GA，不代表刷怪流程验收。"""
from pathlib import Path
import json
import unreal

OUT = Path(unreal.Paths.project_dir()).resolve() / 'Saved/Encounter_20261025'
checks = []


def check(name, value):
    """保留逐项证据，失败不能仅靠进程退出码判为通过。"""
    checks.append({'name': name, 'passed': bool(value)})
    assert value, name


error = None
try:
    action = unreal.load_asset('/Game/Main/Input/InputAction/IA_Interact')
    inputs = unreal.load_asset('/Game/Main/Input/DA_HodgeInputConfig')
    context = unreal.load_asset('/Game/Main/Input/IMC_Default')
    abilities = unreal.load_asset('/Game/Main/Data/AbilitySet/DA_Pover')
    check('四份目标资产可冷加载', action and inputs and context and abilities)
    check('交互动作是 Boolean', action.get_editor_property('value_type') == unreal.InputActionValueType.BOOLEAN)
    triggers = list(action.get_editor_property('triggers'))
    check('只使用按下沿，长按不连发', len(triggers) == 1 and isinstance(triggers[0], unreal.InputTriggerPressed))
    mappings = [m for m in context.get_editor_property('mappings') if m.get_editor_property('action') == action]
    check('唯一 F 映射', len(mappings) == 1 and mappings[0].get_editor_property('key').export_text() == 'F')
    check('其他动作没有占用 F', all(m.get_editor_property('action') == action for m in context.get_editor_property('mappings')
                                   if m.get_editor_property('key').export_text() == 'F'))
    tag = 'InputTag.Ability.Interact'
    rows = [r for r in inputs.get_editor_property('AbilityInputActions')
            if str(r.get_editor_property('InputTag').get_editor_property('TagName')) == tag]
    check('输入标签唯一并指向交互动作', len(rows) == 1 and rows[0].get_editor_property('InputAction') == action)
    cls = unreal.load_class(None, '/Script/Hodgepodge.HodgeGameplayAbility_Interact')
    grants = [g for g in abilities.get_editor_property('GrantedGameplayAbilities') if g.get_editor_property('Ability') == cls]
    check('交互 GA 唯一且等级和标签正确', len(grants) == 1 and grants[0].get_editor_property('AbilityLevel') == 1 and
          str(grants[0].get_editor_property('InputTag').get_editor_property('TagName')) == tag)
    defaults = unreal.get_default_object(cls)
    check('交互 GA 使用本地命令入口', defaults.get_editor_property('NetExecutionPolicy') == unreal.GameplayAbilityNetExecutionPolicy.LOCAL_ONLY)
    check('交互 GA 使用按键触发策略', defaults.get_editor_property('ActivationPolicy') == unreal.HodgeAbilityActivationPolicy.ON_INPUT_TRIGGERED)
    pawn = unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData')
    check('正式Hero使用可保存的250cm交互参数', pawn and pawn.get_editor_property('InteractionDistance') == 250)
    check('保留旧重生能力', any(g.get_editor_property('Ability') and g.get_editor_property('Ability').get_path_name() ==
          '/Script/Hodgepodge.HodgeGameplayAbility_AutoRespawn' for g in abilities.get_editor_property('GrantedGameplayAbilities')))
except Exception as exception:
    error = str(exception)
    raise
finally:
    OUT.mkdir(parents=True, exist_ok=True)
    (OUT / 'interaction-asset-validation.json').write_text(json.dumps(
        {'passed': error is None, 'checks': checks, 'error': error, 'saves': False}, ensure_ascii=False, indent=2), encoding='utf-8')
unreal.log('ENCOUNTER_INTERACTION_ASSETS_VALID')
