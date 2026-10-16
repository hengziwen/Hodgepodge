"""增量接入共鸣回路资产；保留源动画、正常连段次序及用户既有E/R配置。"""
import json
import re
import shutil
from pathlib import Path
import unreal
# 共用新规则配置入口，避免作者工具重新建立旧字段事实来源。
exec(Path(unreal.Paths.project_dir(), 'Tools/AttackRules/rule_config.py').read_text(encoding='utf-8-sig'))


root = Path(unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir()))
work = root / 'Saved/Havoc'
base = '/Game/Main/Character/Hero/Ability/Havoc'
mode = 'State.Combat.Mode.DarkSurge'
marker = 'Hodge.Havoc.Version'
tools = unreal.AssetToolsHelpers.get_asset_tools()
changed, compiled, configured = [], [], []
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
dirty = unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
# 失败重跑只接管本工具已有备份或新建目录中的包，拒绝覆盖无关编辑。
assert all(p.get_path_name().startswith(base + '/') or
           (work / 'BeforeAssets' / (p.get_path_name().removeprefix('/Game/') + '.uasset')).exists() for p in dirty), [p.get_path_name() for p in dirty]
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()


def edit(asset):
    """先备份既有二进制资产，再登记本工具明确要保存的包。"""
    if asset in changed:
        return asset
    path = asset.get_path_name().split('.')[0]
    source = root / 'Content' / (path.removeprefix('/Game/') + '.uasset')
    target = work / 'BeforeAssets' / (path.removeprefix('/Game/') + '.uasset')
    if source.exists() and not target.exists():
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, target)
    changed.append(asset)
    return asset


def tag(name):
    """所有玩法标签必须已在原生或配置中注册。"""
    value = unreal.GameplayTag()
    assert value.import_text('(TagName="' + name + '")'), name
    return value


def tags(names):
    """精确保存资格集合，不用冷却失败作为形态选择依据。"""
    value = unreal.GameplayTagContainer()
    assert value.import_text('(GameplayTags=(' + ','.join('(TagName="' + n + '")' for n in names) + '))')
    return value


def blueprint(name, parent):
    """专属蓝图继承已有原生执行链，不创建重复的播放或伤害逻辑。"""
    asset = unreal.load_asset(base + '/' + name)
    if not asset:
        factory = unreal.BlueprintFactory()
        factory.set_editor_property('ParentClass', parent)
        asset = tools.create_asset(name, base, unreal.Blueprint, factory)
    assert asset
    edit(asset)
    return asset


def compile_bp(asset):
    """读取原生编译错误计数，不将编译调用成功等同于蓝图通过。"""
    result = unreal.HodgeAnimationAuthoringLibrary.compile_animation_blueprint(asset)
    compiled.append({'asset': asset.get_path_name(), 'result': result})
    assert result.startswith('ERRORS=0'), result


def duplicate(name, source):
    """只创建任务自己的配置副本，重复执行保留后续调参。"""
    asset = unreal.load_asset(base + '/' + name)
    if not asset:
        asset = unreal.EditorAssetLibrary.duplicate_asset(source, base + '/' + name)
    assert asset
    return edit(asset)


def fresh(asset):
    """已成功配置的资产不会因重跑脚本被重置窗口。"""
    return unreal.EditorAssetLibrary.get_metadata_tag(asset, marker) != '1'


def variant(asset, enhanced):
    """原有其他资格限制保留，仅增补暗涌互斥条件。"""
    property_name = 'RequiredSourceTags' if enhanced else 'BlockedSourceTags'
    container = asset.get_editor_property(property_name)
    if mode not in container.export_text():
        names = re.findall(r'TagName="([^"]+)"', container.export_text())
        asset.set_editor_property(property_name, tags(names + [mode]))


# 三种短攻击和独立规则各自持有职责；R继续使用现有大招子类和成本配置。
rules = blueprint('GA_HavocRules', unreal.HodgeGameplayAbility_HavocRules)
circuit = blueprint('GA_Resonance_Circuit', unreal.HodgeGameplayAbility_ResonanceCircuit)
normal_template = blueprint('GA_EnhancedNormalAttack', unreal.HodgeGameplayAbility_Melee)
old_e = unreal.load_asset('/Game/Main/Character/Hero/Ability/ResonanceSkill/GA_Resonance_Skill')
enhanced_e = blueprint('GA_EnhancedResonanceSkill', old_e.generated_class())
for bp in [rules, circuit, normal_template, enhanced_e]:
    compile_bp(bp)

# GE蓝图暴露容量、20%加成和周期；默认值沿用原生契约，方便后续资产调参。
effects = {}
for name, parent in [('GE_DarkFlow_Init', unreal.HodgeDarkFlowInitialization), ('GE_DarkFlow_Gain', unreal.HodgeDarkFlowEffect),
                     ('GE_DarkSurge_State', unreal.HodgeDarkSurgeEffect), ('GE_DarkFlow_Drain', unreal.HodgeDarkFlowDrainEffect)]:
    effects[name] = blueprint(name, parent)
    compile_bp(effects[name])
rules_cdo = unreal.get_default_object(rules.generated_class())
for field, name, parent in [('StateEffect', 'GE_DarkSurge_State', unreal.HodgeDarkSurgeEffect),
                            ('DrainEffect', 'GE_DarkFlow_Drain', unreal.HodgeDarkFlowDrainEffect),
                            ('GainEffect', 'GE_DarkFlow_Gain', unreal.HodgeDarkFlowEffect)]:
    if rules_cdo.get_editor_property(field) == parent.static_class():
        rules_cdo.set_editor_property(field, effects[name].generated_class())
compile_bp(rules)
if fresh(circuit):
    unreal.get_default_object(circuit.generated_class()).set_editor_property('EntryCancelWindow', tag('Status.Attack.Cancel.NextAttack'))
    circuit.set_editor_property('BlueprintDescription', '共鸣回路灭音：地面起手、可打断，满暗流资格；通过一次Box提交入态，无CD和全量资源消费。')
if fresh(rules):
    rules.set_editor_property('BlueprintDescription', '暗涌角色规则：只按实际燃料维持；死亡、驱散、换Pawn清空；普通受击保留Buff。')
if fresh(normal_template):
    normal_template.set_editor_property('BlueprintDescription', '强化五段普攻共用模板；差异由Definition和原生蒙太奇通知配置，不复制命中实现。')
if fresh(enhanced_e):
    enhanced_e.set_editor_property('BlueprintDescription', '破命继承普通共鸣技能的同一8秒Cooldown GE；状态资格在Definition选择，后续E图标资产另行提供。')
for bp in [rules, circuit, normal_template, enhanced_e]:
    compile_bp(bp)

normal_path = '/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_1'
e_data = edit(unreal.load_asset('/Game/Main/Character/Hero/Ability/ResonanceSkill/DA_Resonance_Skill'))
specs = [('DA_MieYin', 'AM_Ex_Skill01_01', circuit, .4, .4, .75, True, False)]
specs += [('DA_EnhancedAttack_%d' % i, 'AM_Ex_Attack%02d' % i, normal_template, *timing, False, False)
          for i, timing in enumerate([(.17,.58,.65),(.12,.42,.55),(.17,.6,.7),(.17,1.,1.12),(.15,.7,.82)], 1)]
specs += [('DA_PoMing', 'AM_Ex_Skill02', enhanced_e, .2, 1.15, 1.4, False, True)]
definitions = []
for index, (name, source_name, bp, hit_start, hit_end, recovery, is_circuit, is_skill) in enumerate(specs):
    source = unreal.load_asset('/Game/Wuwa/Anim/' + source_name)
    montage = duplicate(source_name + '_Montage', '/Game/Main/Character/Hero/Anim/Montages/AM_Attack01_Montage')
    definition = duplicate(name, e_data.get_path_name() if is_skill else normal_path)
    if fresh(montage):
        assert unreal.HodgeAnimationAuthoringLibrary.configure_havoc_montage(montage, source, hit_start, hit_end, recovery, is_circuit, is_skill)
    if fresh(definition):
        definition.set_editor_property('AbilityClass', bp.generated_class())
        definition.set_editor_property('AbilityTag', tag('Ability.Type.Action.Melee.ResonanceCircuit' if is_circuit else ('Ability.Type.Action.Melee.ResonanceSkill.Enhanced' if is_skill else 'Ability.Attack.Enhanced.%02d' % index)))
        definition.set_editor_property('ExecutionRoute', unreal.HodgeAbilityExecutionRoute.STANDALONE if is_circuit or is_skill else unreal.HodgeAbilityExecutionRoute.COMBO_COORDINATED)
        definition.set_editor_property('InputTag', tag('InputTag.Ability.ResonanceCircuit' if is_circuit else 'InputTag.Ability.ResonanceSkill') if is_circuit or is_skill else unreal.GameplayTag())
        execution = definition.get_editor_property('ExecutionConfig')
        execution.set_editor_property('Montage', montage)
        definition.set_editor_property('ExecutionConfig', execution)
        facing = definition.get_editor_property('FacingWarp')
        facing.set_editor_property('bEnabled', False)
        definition.set_editor_property('FacingWarp', facing)
        havoc_rule = ensure_attack_rule(definition, unreal.HodgeHavocAttackRuleConfig)
        havoc_rule.set_editor_property('DarkFlowGainPerPhase', 0.)
        havoc_rule.set_editor_property('DarkFlowGainOnCast', 20. if is_skill else 0.)
        havoc_rule.set_editor_property('bEnhancedNormalAttack', not is_circuit and not is_skill)
        variant(definition, not is_circuit)
        binding = definition.get_editor_property('DefaultHitConfig')
        binding.set_editor_property('DamageElement', tag('GameplayEffect.DamageElement.Havoc'))
        definition.set_editor_property('DefaultHitConfig', binding)
    definitions.append(definition)
    configured.append({'definition': definition.get_path_name(), 'montage': montage.get_path_name(), 'source': source.get_path_name(),
                       'hit_start': hit_start, 'hit_end': hit_end, 'recovery': recovery, 'timing_status': '首版可调窗口，待动作逐帧美术校准'})

# 常态五段只增补收益与元素，不改用户已有的3→5→4连段次序。
for i in range(1, 6):
    asset = edit(unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_%d' % i))
    ensure_attack_rule(asset, unreal.HodgeHavocAttackRuleConfig).set_editor_property('DarkFlowGainPerPhase', 5.)
    variant(asset, False)
    hit = asset.get_editor_property('DefaultHitConfig')
    hit.set_editor_property('DamageElement', tag('GameplayEffect.DamageElement.Havoc'))
    asset.set_editor_property('DefaultHitConfig', hit)
variant(e_data, False)
ensure_attack_rule(e_data, unreal.HodgeHavocAttackRuleConfig).set_editor_property('DarkFlowGainOnCast', 20.)
normal_montage = edit(e_data.get_editor_property('ExecutionConfig').montage)
if fresh(e_data):
    assert unreal.HodgeAnimationAuthoringLibrary.configure_havoc_cast_commit(normal_montage, .17)
for asset in [e_data, edit(unreal.load_asset('/Game/Main/Character/Hero/Ability/ResonanceLiberation/DA_Resonance_Liberation'))]:
    hit = asset.get_editor_property('DefaultHitConfig')
    hit.set_editor_property('DamageElement', tag('GameplayEffect.DamageElement.Havoc'))
    asset.set_editor_property('DefaultHitConfig', hit)

# 同一图内增加互斥入口和强化分支，不在运行时重新Configure组件。
combo = unreal.load_asset('/Game/Main/Data/Combo/DA_LightCombo')
table = edit(combo.get_editor_property('ComboTable'))
if fresh(table) and 'Combo.Enhanced.01' not in unreal.DataTableFunctionLibrary.export_data_table_to_json_string(table):
    rows = json.loads(unreal.DataTableFunctionLibrary.export_data_table_to_json_string(table))
    entry = next(row for row in rows if row['Name'] == 'Combo.Entry')
    for row in rows:
        row['Transitions'] = [edge.replace('BlockedSourceTags=(GameplayTags=)', 'BlockedSourceTags=(GameplayTags=((TagName="' + mode + '")))') for edge in row['Transitions']]
    def edge(target, active, entry_edge=False):
        return '(TriggerInputIntentTag=(TagName="InputIntent.Attack.Light"),TargetComboTag=(TagName="' + target + '"),RequiredWindowTags=' + ('(GameplayTags=)' if entry_edge else '(GameplayTags=((TagName="Status.Attack.Cancel.NextAttack")))') + ',bAllowAfterExecutionEnded=' + ('False' if entry_edge else 'True') + ',RequiredSourceTags=' + ('(GameplayTags=((TagName="' + mode + '")))' if active else '(GameplayTags=)') + ',TransitionPriority=20)'
    entry['Transitions'].append(edge('Combo.Enhanced.01', True, True))
    for i in range(1, 6):
        rows.append({'Name': 'Combo.Enhanced.%02d' % i, 'ComboTag': '(TagName="Combo.Enhanced.%02d")' % i,
                     'AbilityTag': '(TagName="Ability.Attack.Enhanced.%02d")' % i, 'GrantedTags': '(GameplayTags=((TagName="Status.Attack")))',
                     'Transitions': [edge('Combo.Enhanced.%02d' % (i % 5 + 1), True)]})
    assert unreal.DataTableFunctionLibrary.fill_data_table_from_json_string(table, json.dumps(rows))

ability_set = unreal.load_asset(base + '/AS_Havoc')
if not ability_set:
    factory = unreal.DataAssetFactory()
    factory.set_editor_property('DataAssetClass', unreal.HodgeAbilitySet)
    ability_set = tools.create_asset('AS_Havoc', base, unreal.HodgeAbilitySet, factory)
edit(ability_set)
if fresh(ability_set):
    entries = []
    for asset in definitions:
        item = unreal.HodgeAbilitySet_Definition()
        assert item.import_text('(Definition="/Script/Hodgepodge.HodgeAbilityDefinition\'' + asset.get_path_name() + '\'",AbilityLevel=1)')
        entries.append(item)
    ability_set.set_editor_property('GrantedAbilityDefinitions', entries)
    ability = unreal.HodgeAbilitySet_GameplayAbility()
    assert ability.import_text('(Ability="' + rules.generated_class().get_path_name() + '",AbilityLevel=1)')
    ability_set.set_editor_property('GrantedGameplayAbilities', [ability])
    effect = unreal.HodgeAbilitySet_GameplayEffect()
    assert effect.import_text('(GameplayEffect="' + effects['GE_DarkFlow_Init'].generated_class().get_path_name() + '",EffectLevel=1)')
    ability_set.set_editor_property('GrantedGameplayEffects', [effect])
    assert not ability_set.get_editor_property('GrantedAttributes'), '禁止重复创建原生属性集'
effect_entries = list(ability_set.get_editor_property('GrantedGameplayEffects'))
for index, item in enumerate(effect_entries):
    if item.get_editor_property('GameplayEffect') == unreal.HodgeDarkFlowInitialization.static_class():
        replacement = unreal.HodgeAbilitySet_GameplayEffect()
        assert replacement.import_text('(GameplayEffect="' + effects['GE_DarkFlow_Init'].generated_class().get_path_name() + '",EffectLevel=1)')
        effect_entries[index] = replacement
ability_set.set_editor_property('GrantedGameplayEffects', effect_entries)
pawn = edit(unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData'))
sets = list(pawn.get_editor_property('AbilitySets'))
if ability_set not in sets:
    pawn.set_editor_property('AbilitySets', sets + [ability_set])

# Pressed触发器会在下一帧完成，必须移除才能保留真实长按生命周期。
config = pawn.get_editor_property('InputConfig')
attack = next(row.get_editor_property('InputAction') for row in config.get_editor_property('AbilityInputActions')
              if 'InputTag.Ability.Melee' in row.get_editor_property('InputTag').export_text())
pressed = [trigger for trigger in attack.get_editor_property('Triggers') if isinstance(trigger, unreal.InputTriggerPressed)]
if pressed:
    edit(attack)
    attack.set_editor_property('Triggers', [t for t in attack.get_editor_property('Triggers') if t not in pressed])

# 沿用Experience→AddWidgets→UIExtension注入新条，不在PlayerController造UI。
eas = edit(unreal.load_asset('/Game/Main/UI/EAS_HodgeGameplayUI'))
action = next(a for a in eas.get_editor_property('Actions') if a.get_class().get_name() == 'GameFeatureAction_AddWidgets')
widgets = list(action.get_editor_property('Widgets'))
if not any('UI.Slot.DarkFlow' in row.get_editor_property('SlotID').export_text() for row in widgets):
    row = widgets[0].copy()
    row.set_editor_property('SlotID', tag('UI.Slot.DarkFlow'))
    row.set_editor_property('WidgetClass', unreal.load_class(None, '/Game/Main/UI/HUD/WBP_DarkFlow.WBP_DarkFlow_C'))
    action.set_editor_property('Widgets', widgets + [row])

for asset in changed:
    unreal.EditorAssetLibrary.set_metadata_tag(asset, marker, '1')
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset, False), asset.get_path_name()
report = {'assets': [a.get_path_name() for a in changed], 'compiled': compiled, 'configuration': configured,
          'limitations': ['强化动作使用首版AvatarRoot Box，未接角色专属剑/翼多点判定', '新动作通知帧需逐帧美术校准', 'E新图标、重击和变奏暂缓']}
(work / 'asset-configuration.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps({'assets': len(changed), 'compiled': len(compiled), 'saved': True}))
