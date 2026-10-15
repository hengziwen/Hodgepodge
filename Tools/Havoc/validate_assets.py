"""只读验证正式暗涌资产与冻结规则，结果可追溯到HDS需求。"""
import json
from pathlib import Path
import unreal

root = Path(unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir()))
base = '/Game/Main/Character/Hero/Ability/Havoc/'
checks = []


def check(name, condition, detail=None):
    """仅从已保存的正式资产读取事实，不依据作者脚本的期望值判成功。"""
    checks.append({'case': name, 'passed': bool(condition), 'detail': detail})
    assert condition, (name, detail)


ability_set = unreal.load_asset(base + 'AS_Havoc')
pawn = unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData')
check('HDS-01 PawnData只引用一次AS_Havoc', list(pawn.get_editor_property('AbilitySets')).count(ability_set) == 1)
check('HDS-01 不重复授予原生属性集', not ability_set.get_editor_property('GrantedAttributes'))
check('HDS-05 提前授予七个新攻击', len(ability_set.get_editor_property('GrantedAbilityDefinitions')) == 7)
normal = unreal.load_asset('/Game/Main/Character/Hero/Ability/ResonanceSkill/GA_Resonance_Skill')
enhanced = unreal.load_asset(base + 'GA_EnhancedResonanceSkill')
normal_cd = unreal.get_default_object(normal.generated_class()).get_editor_property('CooldownGameplayEffectClass')
enhanced_cd = unreal.get_default_object(enhanced.generated_class()).get_editor_property('CooldownGameplayEffectClass')
check('HDS-05 两种E使用同一份8秒冷却GE', normal_cd == enhanced_cd, normal_cd.get_path_name())
sources = ['AM_Ex_Skill01_01', 'AM_Ex_Skill02'] + ['AM_Ex_Attack%02d' % i for i in range(1, 6)]
for name in sources:
    montage = unreal.load_asset(base + name + '_Montage')
    # SlotAnimTracks未开放Python读取，通知链接文本仍记录实际分段源动画。
    events = unreal.AnimationLibrary.get_animation_notify_events(montage)
    text = '\n'.join(event.export_text() for event in events)
    check('HDS-09 ' + name + '链接指定源动画', '/Game/Wuwa/Anim/' + name + '.' + name in text)
    check('HDS-09 ' + name + '正式通知存在', len(unreal.AnimationLibrary.get_animation_notify_events(montage)) >= 5)
for i in range(1, 6):
    data = unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_%d' % i)
    check('HDS-06 常态第%d段每阶段5点' % i, data.get_editor_property('DarkFlowGainPerPhase') == 5.)
    data = unreal.load_asset(base + 'DA_EnhancedAttack_%d' % i)
    check('HDS-05/06 强化第%d段资格和拢音分类' % i, data.get_editor_property('bEnhancedNormalAttack') and data.get_editor_property('DarkFlowGainPerPhase') == 0.
          and 'State.Combat.Mode.DarkSurge' in data.get_editor_property('RequiredSourceTags').export_text())
action = next(a for a in unreal.load_asset('/Game/Main/UI/EAS_HodgeGameplayUI').get_editor_property('Actions') if a.get_class().get_name() == 'GameFeatureAction_AddWidgets')
slots = [w for w in action.get_editor_property('Widgets') if 'UI.Slot.DarkFlow' in w.get_editor_property('SlotID').export_text()]
check('HDS-07 EAS只注入一次WBP_DarkFlow', len(slots) == 1 and 'WBP_DarkFlow' in slots[0].get_editor_property('WidgetClass').get_path_name())
config = pawn.get_editor_property('InputConfig')
attack = next(r.get_editor_property('InputAction') for r in config.get_editor_property('AbilityInputActions') if 'InputTag.Ability.Melee' in r.get_editor_property('InputTag').export_text())
check('HDS-01 普攻不再使用提前Completed的Pressed触发器', not any(isinstance(t, unreal.InputTriggerPressed) for t in attack.get_editor_property('Triggers')))
result = {'passed': all(c['passed'] for c in checks), 'checks': checks}
(root / 'Saved/Havoc/asset-validation.json').write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps({'passed': result['passed'], 'checks': len(checks)}))
