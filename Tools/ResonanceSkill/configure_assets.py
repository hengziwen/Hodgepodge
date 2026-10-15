"""创建共鸣技能模板，只增补本技能拥有的资产、授予和 E 键输入。"""
import json
from pathlib import Path
import unreal

ROOT = '/Game/Main/Character/Hero/Ability/ResonanceSkill'
WORK = Path('E:/Project/Git/Hodgepodge/Saved/ResonanceSkill')
MARKER = 'Hodge.ResonanceSkill.TemplateVersion'
tools = unreal.AssetToolsHelpers.get_asset_tools()
changed = []
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
owned_paths = {'/Game/Main/Data/PawnData/DA_Dafult_PawnData', '/Game/Main/Input/DA_HodgeInputConfig', '/Game/Main/Input/IMC_Default', '/Game/Main/Input/InputAction/IA_Resonance_Skill'}
assert all(package.get_path_name().startswith(ROOT) or package.get_path_name() in owned_paths for package in unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages())

def tag(name):
    """新标签必须已经由 Config/Tags 注册，再写入正式资产。"""
    value = unreal.GameplayTag()
    assert value.import_text('(TagName="' + name + '")')
    assert str(value.get_editor_property('TagName')) == name, name
    return value

def tag_container(names):
    """构造精确标签集合，避免技能共用冷却身份。"""
    value = unreal.GameplayTagContainer()
    assert value.import_text('(GameplayTags=(' + ','.join('(TagName="' + name + '")' for name in names) + '))')
    return value

def blueprint(name, parent):
    """已有模板保留用户修改，新资产直接继承现有原生类型。"""
    path = ROOT + '/' + name
    asset = unreal.load_asset(path)
    if not asset:
        factory = unreal.BlueprintFactory()
        factory.set_editor_property('ParentClass', parent)
        asset = tools.create_asset(name, ROOT, unreal.Blueprint, factory)
    assert asset, path
    changed.append(asset)
    return asset

def duplicate(name, source):
    """模型和通知保持兼容，同时为技能提供独立可调整副本。"""
    path = ROOT + '/' + name
    asset = unreal.load_asset(path) or unreal.EditorAssetLibrary.duplicate_asset(source, path)
    assert asset, path
    changed.append(asset)
    return asset

def needs_setup(asset):
    """只初始化尚未成功交付的模板，不重置后续调参。"""
    return unreal.EditorAssetLibrary.get_metadata_tag(asset, MARKER) != '1'

ability_tag = tag('Ability.Type.Action.Melee.ResonanceSkill')
input_tag = tag('InputTag.Ability.ResonanceSkill')
cooldown_tag = tag('Cooldown.Hero.ResonanceSkill')

ga = blueprint('GA_Resonance_Skill', unreal.HodgeGameplayAbility_Melee)
cooldown = duplicate('GE_Resonance_Skill_Cooldown', '/Game/GameplayEffects/GE_HeroDash_Cooldown')
damage = duplicate('GE_Resonance_Skill_Damage', '/Game/Main/Character/Hero/Ability/BasicAttack/GE_MeleeDamage_Instant')
montage = duplicate('AM_Resonance_Skill', '/Game/Main/Character/Hero/Anim/Montages/AM_Attack01_Montage')
definition = duplicate('DA_Resonance_Skill', '/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_1')

for asset in [ga, cooldown, damage]:
    unreal.BlueprintEditorLibrary.compile_blueprint(asset)

if needs_setup(cooldown):
    cdo = unreal.get_default_object(cooldown.generated_class())
    cdo.set_editor_property('DurationPolicy', unreal.GameplayEffectDurationType.HAS_DURATION)
    magnitude = cdo.get_editor_property('DurationMagnitude')
    assert magnitude.import_text('(MagnitudeCalculationType=ScalableFloat,ScalableFloatMagnitude=(Value=8.0))')
    cdo.set_editor_property('DurationMagnitude', magnitude)
    components = cdo.get_editor_property('GEComponents')
    target_component = next(component for component in components if component.get_class().get_name() == 'TargetTagsGameplayEffectComponent')
    granted = target_component.get_editor_property('InheritableGrantedTagsContainer')
    tags = tag_container(['Cooldown.Hero.ResonanceSkill']).export_text()
    assert granted.import_text('(CombinedTags=' + tags + ',Added=' + tags + ',Removed=(GameplayTags=))')
    target_component.set_editor_property('InheritableGrantedTagsContainer', granted)
    cooldown.set_editor_property('BlueprintDescription', '共鸣技能独立冷却：默认 8 秒，通过 Target Tags 授予冷却标签。')

if needs_setup(ga):
    cdo = unreal.get_default_object(ga.generated_class())
    cdo.set_editor_property('CooldownGameplayEffectClass', cooldown.generated_class())
    cdo.set_editor_property('AbilityTags', tag_container(['Ability.Type.Action.Melee', 'Ability.Type.Action.Melee.ResonanceSkill']))
    cdo.set_editor_property('ActivationPolicy', unreal.HodgeAbilityActivationPolicy.ON_INPUT_TRIGGERED)
    ga.set_editor_property('BlueprintDescription', 'Resonance Skill：复用原生 Melee 和 Standalone Definition，CD 发动时开始，打断后保留。')
    unreal.EditorAssetLibrary.set_metadata_tag(ga, 'DisplayName', 'Resonance Skill')

if needs_setup(definition):
    definition.set_editor_property('AbilityTag', ability_tag)
    definition.set_editor_property('InputTag', input_tag)
    definition.set_editor_property('ExecutionRoute', unreal.HodgeAbilityExecutionRoute.STANDALONE)
    definition.set_editor_property('AbilityClass', ga.generated_class())
    execution = definition.get_editor_property('ExecutionConfig')
    execution.set_editor_property('Montage', montage)
    definition.set_editor_property('ExecutionConfig', execution)
    hit = definition.get_editor_property('DefaultHitConfig')
    hit.set_editor_property('DamageEffect', damage.generated_class())
    definition.set_editor_property('DefaultHitConfig', hit)

if needs_setup(montage):
    # 默认与通知覆盖均指向独立伤害副本，保留窗口和默认伤害开关。
    for notify in unreal.ObjectIterator():
        if isinstance(notify, type) or not unreal.Object.get_path_name(notify).startswith(montage.get_path_name() + ':'):
            continue
        if notify.get_class().get_name() not in ['HodgeAnimNotifyState_HitCheck', 'HodgeAnimNotify_Hit']:
            continue
        hit = notify.get_editor_property('Hit')
        binding = hit.get_editor_property('Damage')
        binding.set_editor_property('DamageEffect', damage.generated_class())
        hit.set_editor_property('Damage', binding)
        notify.set_editor_property('Hit', hit)

path = ROOT + '/AS_Resonance_Skill'
ability_set = unreal.load_asset(path)
if not ability_set:
    factory = unreal.DataAssetFactory()
    factory.set_editor_property('DataAssetClass', unreal.HodgeAbilitySet)
    ability_set = tools.create_asset('AS_Resonance_Skill', ROOT, unreal.HodgeAbilitySet, factory)
changed.append(ability_set)
if needs_setup(ability_set):
    entry = unreal.HodgeAbilitySet_Definition()
    assert entry.import_text('(Definition="/Script/Hodgepodge.HodgeAbilityDefinition\'' + definition.get_path_name() + '\'",AbilityLevel=1)')
    ability_set.set_editor_property('GrantedAbilityDefinitions', [entry])

path = '/Game/Main/Input/InputAction/IA_Resonance_Skill'
action = unreal.load_asset(path) or tools.create_asset('IA_Resonance_Skill', '/Game/Main/Input/InputAction', unreal.InputAction, unreal.InputAction_Factory())
changed.append(action)
if needs_setup(action):
    action.set_editor_property('ValueType', unreal.InputActionValueType.BOOLEAN)

pawn = unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData')
config = pawn.get_editor_property('InputConfig')
mapping = unreal.load_asset('/Game/Main/Input/IMC_Default')
sets = list(pawn.get_editor_property('AbilitySets'))
if ability_set not in sets:
    pawn.set_editor_property('AbilitySets', sets + [ability_set])
rows = list(config.get_editor_property('AbilityInputActions'))
own_rows = [row for row in rows if str(row.get_editor_property('InputTag').get_editor_property('TagName')) == 'InputTag.Ability.ResonanceSkill']
if len(own_rows) > 1:
    # 只去重本工具创建的同标签条目，其他玩家输入保持原顺序。
    rows = [row for row in rows if str(row.get_editor_property('InputTag').get_editor_property('TagName')) != 'InputTag.Ability.ResonanceSkill'] + own_rows[:1]
    config.set_editor_property('AbilityInputActions', rows)
if not own_rows:
    row = unreal.HodgeInputAction()
    assert row.import_text('(InputAction="/Script/EnhancedInput.InputAction\'' + action.get_path_name() + '\'",InputTag=(TagName="InputTag.Ability.ResonanceSkill"))')
    config.set_editor_property('AbilityInputActions', rows + [row])
key = unreal.Key()
assert key.import_text('E') and key.export_text() == 'E'
own_mappings = [row for row in mapping.get_editor_property('Mappings') if row.get_editor_property('Action') == action]
if not own_mappings:
    conflicts = [row for row in mapping.get_editor_property('Mappings') if row.get_editor_property('Key').export_text() == 'E']
    assert not conflicts, 'E 键已有映射，禁止覆盖'
    mapping.map_key(action, key)
elif any(row.get_editor_property('Key').export_text() == '(' for row in own_mappings):
    # 修复本工具早期错误的 Key 文本导入，不覆盖用户后来选择的正常键位。
    assert not any(row.get_editor_property('Key').export_text() == 'E' and row.get_editor_property('Action') != action for row in mapping.get_editor_property('Mappings'))
    mapping.unmap_all_keys_from_action(action)
    mapping.map_key(action, key)
changed.extend([pawn, config, mapping])

# 编译只操作指定蓝图；原生 MCP 诊断在外部制作流程另行确认。
compiled = []
for asset in [ga, cooldown, damage]:
    unreal.BlueprintEditorLibrary.compile_blueprint(asset)
    result = 'BlueprintEditorLibrary.compile_blueprint completed; native diagnostics required'
    compiled.append({'asset': asset.get_path_name(), 'result': result})
for asset in changed:
    if asset not in [pawn, config, mapping]:
        unreal.EditorAssetLibrary.set_metadata_tag(asset, MARKER, '1')
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset, False), asset.get_path_name()
summary = {'assets': [asset.get_path_name() for asset in changed], 'tags': [ability_tag.export_text(), input_tag.export_text(), cooldown_tag.export_text()],
           'keys': [row.get_editor_property('Key').export_text() for row in mapping.get_editor_property('Mappings') if row.get_editor_property('Action') == action],
           'cooldown': unreal.get_default_object(cooldown.generated_class()).get_editor_property('DurationMagnitude').get_editor_property('ScalableFloatMagnitude').get_editor_property('Value'), 'compiled': compiled,
           'template': '首段普攻动画和命中配置副本；伤害 GE 独立副本，后续可替换'}
(WORK / 'configuration.json').write_text(json.dumps(summary, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps(summary, ensure_ascii=False))
