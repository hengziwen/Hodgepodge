"""只读核对技能资产、标签、冷却和授予输入，避免遗漏接线。"""
import json
from pathlib import Path
import unreal

ROOT = '/Game/Main/Character/Hero/Ability/ResonanceSkill'
checks = []

def check(name, condition):
    """逐项断言失败即停止，输出到 Saved 保留证据。"""
    checks.append({'check': name, 'passed': bool(condition)})
    assert condition, name

ga = unreal.load_asset(ROOT + '/GA_Resonance_Skill')
cooldown = unreal.load_asset(ROOT + '/GE_Resonance_Skill_Cooldown')
damage = unreal.load_asset(ROOT + '/GE_Resonance_Skill_Damage')
definition = unreal.load_asset(ROOT + '/DA_Resonance_Skill')
ability_set = unreal.load_asset(ROOT + '/AS_Resonance_Skill')
action = unreal.load_asset('/Game/Main/Input/InputAction/IA_Resonance_Skill')
check('SK-01_完整七个资产', all([ga, cooldown, damage, definition, ability_set, action, unreal.load_asset(ROOT + '/AM_Resonance_Skill')]))
check('SK-01_原生Melee父类', isinstance(unreal.get_default_object(ga.generated_class()), unreal.HodgeGameplayAbility_Melee))
check('SK-01_独立路由', definition.get_editor_property('ExecutionRoute') == unreal.HodgeAbilityExecutionRoute.STANDALONE)
check('SK-01_Definition关联GA', definition.get_editor_property('AbilityClass') == ga.generated_class())
check('SK-01_独立动画副本', definition.get_editor_property('ExecutionConfig').get_editor_property('Montage').get_path_name().startswith(ROOT + '/AM_Resonance_Skill.'))
check('SK-01_独立伤害GE', definition.get_editor_property('DefaultHitConfig').get_editor_property('DamageEffect') == damage.generated_class())
check('SK-02_冷却GE绑定', unreal.get_default_object(ga.generated_class()).get_editor_property('CooldownGameplayEffectClass') == cooldown.generated_class())
cdo = unreal.get_default_object(cooldown.generated_class())
check('SK-02_持续冷却效果', cdo.get_editor_property('DurationPolicy') == unreal.GameplayEffectDurationType.HAS_DURATION)
check('SK-02_冷却8秒', cdo.get_editor_property('DurationMagnitude').get_editor_property('ScalableFloatMagnitude').get_editor_property('Value') == 8)
bridge = unreal.get_default_object(unreal.load_class(None, '/Script/GameplayAbilities.AbilitySystemBlueprintLibrary'))
tags = bridge.call_method('GetGameplayEffectGrantedTags', (cooldown.generated_class(),)).export_text()
check('SK-02_独立冷却标签', 'Cooldown.Hero.ResonanceSkill' in tags and 'Dash' not in tags)
entries = ability_set.get_editor_property('GrantedAbilityDefinitions')
check('SK-03_Definition唯一授予', len(entries) == 1 and entries[0].get_editor_property('Definition') == definition)
pawn = unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData')
check('SK-03_PawnData授予一次', list(pawn.get_editor_property('AbilitySets')).count(ability_set) == 1)
config = pawn.get_editor_property('InputConfig')
rows = [row for row in config.get_editor_property('AbilityInputActions') if str(row.get_editor_property('InputTag').get_editor_property('TagName')) == 'InputTag.Ability.ResonanceSkill']
check('SK-03_输入标签唯一接线', len(rows) == 1 and rows[0].get_editor_property('InputAction') == action)
check('SK-03_Boolean输入', action.get_editor_property('ValueType') == unreal.InputActionValueType.BOOLEAN)
mapping = unreal.load_asset('/Game/Main/Input/IMC_Default')
key = unreal.Key()
key.import_text('E')
check('SK-03_E键唯一绑定', len([row for row in mapping.get_editor_property('Mappings') if row.get_editor_property('Key').export_text() == key.export_text() and row.get_editor_property('Action') == action]) == 1)
check('SK-04_无未保存内容', not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages())
report = {'passed': all(row['passed'] for row in checks), 'checks': checks, 'cold_loaded': True}
Path('E:/Project/Git/Hodgepodge/Saved/ResonanceSkill/assets-validation.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps({'passed': report['passed'], 'checks': len(checks)}))
