"""创建共鸣解放原型与右下方充能条，只初始化本任务拥有的资产。"""
import json
from pathlib import Path
import unreal

ROOT = '/Game/Main/Character/Hero/Ability/ResonanceLiberation'
WING_ROOT = '/Game/Main/Character/Hero/Wing'
WORK = Path(unreal.Paths.project_saved_dir()) / 'CinematicUltimate'
MARKER = 'Hodge.ResonanceLiberation.Version'
tools = unreal.AssetToolsHelpers.get_asset_tools()
written = []
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
# 失败重试只允许本任务明确拥有的包，其他未保存内容仍阻止装配。
owned = {'/Game/Main/Data/PawnData/DA_Dafult_PawnData', '/Game/Main/Input/DA_HodgeInputConfig', '/Game/Main/Input/IMC_Default',
         '/Game/Main/Input/InputAction/IA_Resonance_Liberation', '/Game/Main/UI/HUD/WBP_HodgeHUD', '/Game/Main/UI/HUD/WBP_Resonance_Energy',
         '/Game/Main/Character/Hero/Wing/Montages/AM_Wing_Burst01', '/Game/Main/Character/Hero/Ability/ResonanceSkill/DA_Resonance_Skill',
         '/Game/Main/Character/Hero/Ability/ResonanceSkill/AM_Resonance_Skill1'}
owned.update(f'/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_{number}' for number in range(1, 6))
owned.update(f'/Game/Main/Character/Hero/Anim/Montages/AM_Attack{number:02d}_Montage' for number in range(1, 6))
assert all(package.get_path_name().startswith(ROOT) or package.get_path_name() in owned for package in unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()), '先保存本任务之外的编辑器工作'


def tag(name):
    """使用原生已注册标签，不创建重复别名。"""
    result = unreal.GameplayTag()
    assert result.import_text('(TagName="' + name + '")')
    return result


def tags(names):
    """构造正式输入和冷却的精确标签集合。"""
    result = unreal.GameplayTagContainer()
    assert result.import_text('(GameplayTags=(' + ','.join('(TagName="' + name + '")' for name in names) + '))')
    return result


def data(name, cls, folder=ROOT):
    """已有资产保留用户调参；失败的首次装配可重复执行。"""
    asset = unreal.load_asset(folder + '/' + name)
    if not asset:
        factory = unreal.DataAssetFactory()
        factory.set_editor_property('DataAssetClass', cls)
        asset = tools.create_asset(name, folder, cls, factory)
    assert asset
    written.append(asset)
    return asset


def blueprint(name, parent, source=None, folder=ROOT, widget=False):
    """新蓝图复用已有执行类或兼容GE，不覆盖旧技能。"""
    path = folder + '/' + name
    asset = unreal.load_asset(path)
    if not asset and source:
        asset = unreal.EditorAssetLibrary.duplicate_asset(source, path)
    if not asset:
        factory = unreal.WidgetBlueprintFactory() if widget else unreal.BlueprintFactory()
        factory.set_editor_property('ParentClass', parent)
        asset = tools.create_asset(name, folder, unreal.WidgetBlueprint if widget else unreal.Blueprint, factory)
    assert asset
    unreal.BlueprintEditorLibrary.compile_blueprint(asset)
    written.append(asset)
    return asset


def setup(asset):
    """已交付标记之后不重置蒙太奇切点、数值或镜头。"""
    return unreal.EditorAssetLibrary.get_metadata_tag(asset, MARKER) != '1'


pawn = unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData')
hero_cdo = unreal.get_default_object(pawn.pawn_class)
bones = [str(hero_cdo.mesh.get_bone_name(index)) for index in range(hero_cdo.mesh.get_num_bones())]
head = next((bone for bone in bones if bone.lower() in ('head', 'head_m', 'bip001head', 'bip001 head')), None)
assert head, '必须确认真实头部骨骼，不能静默使用猜测锚点：' + str(bones)

camera = data('DA_Resonance_Liberation_Camera', unreal.HodgeCinematicCameraProfile)
if setup(camera):
    rows = []
    values = [('Acquire', 0., .2, (210., 100., 5.), (165., 65., 3.), (65., 55.)),
              ('FaceHands', .2, .6, (165., 65., 3.), (115., 30., 0.), (55., 43.)),
              ('WingReveal', .6, 1., (190., -95., 5.), (300., -150., 15.), (55., 65.))]
    for name, start, end, first, last, fov in values:
        row = unreal.HodgeCinematicCameraShot()
        row.set_editor_property('ShotId', name)
        row.set_editor_property('Start', start)
        row.set_editor_property('End', end)
        row.set_editor_property('Anchor', head)
        row.set_editor_property('LookAt', head)
        row.set_editor_property('StartOffset', unreal.Vector(*first))
        row.set_editor_property('EndOffset', unreal.Vector(*last))
        row.set_editor_property('FOV', unreal.Vector2D(*fov))
        row.set_editor_property('BlendFraction', .3)
        rows.append(row)
    camera.set_editor_property('Shots', rows)
    camera.set_editor_property('PostProcessWeight', .2)
    pp = camera.get_editor_property('PostProcess')
    assert pp.import_text('(bOverride_VignetteIntensity=True,VignetteIntensity=0.55,bOverride_AutoExposureBias=True,AutoExposureBias=-0.25)')
    camera.set_editor_property('PostProcess', pp)

hit_profile = data('DA_Resonance_Liberation_HitProfile', unreal.HodgeHitDetectionProfile)
if setup(hit_profile):
    hit_profile.set_editor_property('QueryMode', unreal.HodgeHitQueryMode.OVERLAP)
    hit_profile.set_editor_property('SampleMode', unreal.HodgeHitSampleMode.ONCE_ON_ENTER)
    hit_profile.set_editor_property('FilterFrame', unreal.HodgeHitFilterFrame.DETECTION_ANCHOR)
    hit_profile.set_editor_property('HalfAngleDegrees', 180.)

wing_source = unreal.load_asset('/Game/Wuwa/Wing/BaseAnim/G_Burst01')
wing_montage = unreal.load_asset(WING_ROOT + '/Montages/AM_Wing_Burst01')
if not wing_montage:
    factory = unreal.AnimMontageFactory()
    factory.set_editor_property('TargetSkeleton', wing_source.get_editor_property('Skeleton'))
    wing_montage = tools.create_asset('AM_Wing_Burst01', WING_ROOT + '/Montages', unreal.AnimMontage, factory)
    assert unreal.HodgeAnimationAuthoringLibrary.configure_wing_montage_asset(wing_montage, wing_source)
written.append(wing_montage)

source = unreal.load_asset('/Game/Wuwa/Anim/AM_Burst01')
montage = unreal.load_asset(ROOT + '/AM_Burst01') or unreal.EditorAssetLibrary.duplicate_asset('/Game/Main/Character/Hero/Anim/Montages/AM_Attack01_Montage', ROOT + '/AM_Burst01')
written.append(montage)
hit_time = source.get_play_length() * .68
if setup(montage):
    assert unreal.HodgeAnimationAuthoringLibrary.configure_liberation_montage(montage, source, wing_montage, camera, hit_profile, hit_time)

gain = blueprint('GE_ResonanceEnergy_Gain', unreal.HodgeResonanceEnergyEffect)
cost = blueprint('GE_ResonanceEnergy_Cost', unreal.HodgeResonanceEnergyEffect)
initial = blueprint('GE_ResonanceEnergy_Init', unreal.HodgeResonanceEnergyInitialization)
cooldown = blueprint('GE_Resonance_Liberation_Cooldown', unreal.GameplayEffect,
                     '/Game/Main/Character/Hero/Ability/ResonanceSkill/GE_Resonance_Skill_Cooldown')
damage = blueprint('GE_Resonance_Liberation_Damage', unreal.GameplayEffect,
                   '/Game/Main/Character/Hero/Ability/BasicAttack/GE_MeleeDamage_Instant')
ga = blueprint('GA_Resonance_Liberation', unreal.HodgeGameplayAbility_Melee)

if setup(cooldown):
    cdo = unreal.get_default_object(cooldown.generated_class())
    magnitude = cdo.get_editor_property('DurationMagnitude')
    assert magnitude.import_text('(MagnitudeCalculationType=ScalableFloat,ScalableFloatMagnitude=(Value=12.0))')
    cdo.set_editor_property('DurationMagnitude', magnitude)
    component = next(row for row in cdo.get_editor_property('GEComponents') if row.get_class().get_name() == 'TargetTagsGameplayEffectComponent')
    grant = component.get_editor_property('InheritableGrantedTagsContainer')
    own_tags = tags(['Cooldown.Hero.ResonanceLiberation']).export_text()
    assert grant.import_text('(CombinedTags=' + own_tags + ',Added=' + own_tags + ',Removed=(GameplayTags=))')
    component.set_editor_property('InheritableGrantedTagsContainer', grant)

if setup(ga):
    cdo = unreal.get_default_object(ga.generated_class())
    energy_cost = unreal.new_object(unreal.HodgeAbilityCost_ResonanceEnergy, outer=cdo)
    energy_cost.set_editor_property('CostGameplayEffectClass', cost.generated_class())
    cdo.set_editor_property('AdditionalCosts', [energy_cost])
    cdo.set_editor_property('CostGameplayEffectClass', None)
    cdo.set_editor_property('CooldownGameplayEffectClass', cooldown.generated_class())
    cdo.set_editor_property('AbilityTags', tags(['Ability.Type.Action.Melee', 'Ability.Type.Action.Melee.ResonanceLiberation']))
    cdo.set_editor_property('ActivationPolicy', unreal.HodgeAbilityActivationPolicy.ON_INPUT_TRIGGERED)
    ga.set_editor_property('BlueprintDescription', '共鸣解放：R键，满充消耗，独立12秒CD；复用Standalone Melee与原生通知，不重复Commit或播放蒙太奇。')

definition = data('DA_Resonance_Liberation', unreal.HodgeAbilityDefinition)
if setup(definition):
    definition.set_editor_property('AbilityTag', tag('Ability.Type.Action.Melee.ResonanceLiberation'))
    definition.set_editor_property('InputTag', tag('InputTag.Ability.ResonanceLiberation'))
    definition.set_editor_property('AbilityClass', ga.generated_class())
    definition.set_editor_property('ExecutionRoute', unreal.HodgeAbilityExecutionRoute.STANDALONE)
    definition.set_editor_property('ExecutionBodyTag', tag('State.Combat.Body.Vajra'))
    execution = definition.get_editor_property('ExecutionConfig')
    execution.set_editor_property('Montage', montage)
    definition.set_editor_property('ExecutionConfig', execution)
    binding = definition.get_editor_property('DefaultHitConfig')
    binding.set_editor_property('DamageEffect', damage.generated_class())
    binding.set_editor_property('DamageMultiplier', 2.)
    reaction = binding.get_editor_property('Reaction')
    reaction.set_editor_property('AttackJudgementTag', tag('Combat.Attack.Judgement.Vajra'))
    reaction.set_editor_property('Impacts', [])
    binding.set_editor_property('Reaction', reaction)
    definition.set_editor_property('DefaultHitConfig', binding)

ability_set = data('AS_Resonance_Liberation', unreal.HodgeAbilitySet)
if setup(ability_set):
    entry = unreal.HodgeAbilitySet_Definition()
    assert entry.import_text('(Definition="/Script/Hodgepodge.HodgeAbilityDefinition\'' + definition.get_path_name() + '\'",AbilityLevel=1)')
    ability_set.set_editor_property('GrantedAbilityDefinitions', [entry])
    init_effect = unreal.HodgeAbilitySet_GameplayEffect()
    assert init_effect.import_text('(GameplayEffect="/Script/Engine.BlueprintGeneratedClass\'' + initial.generated_class().get_path_name() + '\'",EffectLevel=1.0)')
    ability_set.set_editor_property('GrantedGameplayEffects', [init_effect])

# 只增补充能参数和阶段身份，不改普攻／共鸣技能的动画、伤害和已有Impact。
for path, amount in [(f'/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_{number}', 5.) for number in range(1, 6)] + [('/Game/Main/Character/Hero/Ability/ResonanceSkill/DA_Resonance_Skill', 20.)]:
    attack = unreal.load_asset(path)
    assert attack, path
    if not unreal.EditorAssetLibrary.get_metadata_tag(attack, 'Hodge.ResonanceGain.Version'):
        value = attack.get_editor_property('ResonanceEnergyGainPerPhase')
        assert value.import_text('(Value=' + str(amount) + ')')
        attack.set_editor_property('ResonanceEnergyGainPerPhase', value)
        attack.set_editor_property('ResonanceEnergyGainEffect', gain.generated_class())
        for event in unreal.AnimationLibrary.get_animation_notify_events(attack.execution_config.montage):
            notify = event.notify or event.notify_state_class
            if notify and notify.get_class().get_name() in ('HodgeAnimNotify_Hit', 'HodgeAnimNotifyState_HitCheck'):
                binding = notify.get_editor_property('Hit')
                if str(binding.hit_group) == 'None':
                    binding.set_editor_property('HitGroup', attack.get_name())
                if str(binding.attack_phase) == 'None':
                    binding.set_editor_property('AttackPhase', 'Strike')
                notify.set_editor_property('Hit', binding)
        unreal.EditorAssetLibrary.set_metadata_tag(attack, 'Hodge.ResonanceGain.Version', '1')
    written.extend([attack, attack.execution_config.montage])

action_path = '/Game/Main/Input/InputAction/IA_Resonance_Liberation'
action = unreal.load_asset(action_path) or tools.create_asset('IA_Resonance_Liberation', '/Game/Main/Input/InputAction', unreal.InputAction, unreal.InputAction_Factory())
action.set_editor_property('ValueType', unreal.InputActionValueType.BOOLEAN)
config = pawn.input_config
mapping = unreal.load_asset('/Game/Main/Input/IMC_Default')
rows = list(config.get_editor_property('AbilityInputActions'))
own_rows=[row for row in rows if str(row.input_tag.get_editor_property('TagName'))=='InputTag.Ability.ResonanceLiberation']
if not own_rows:
    row = unreal.HodgeInputAction()
    assert row.import_text('(InputAction="/Script/EnhancedInput.InputAction\'' + action.get_path_name() + '\'",InputTag=(TagName="InputTag.Ability.ResonanceLiberation"))')
    config.set_editor_property('AbilityInputActions', rows + [row])
elif len(own_rows)>1:
    # GameplayTag反射值按名称比较，清理本任务重复入口，不改其他技能映射。
    kept=False
    unique=[]
    for row in rows:
        if str(row.input_tag.get_editor_property('TagName'))=='InputTag.Ability.ResonanceLiberation':
            if kept:continue
            kept=True
        unique.append(row)
    config.set_editor_property('AbilityInputActions',unique)
own = [row for row in mapping.get_editor_property('Mappings') if row.action == action]
if not own:
    key = unreal.Key()
    assert key.import_text('R')
    assert not any(row.key.export_text() == 'R' for row in mapping.get_editor_property('Mappings')), 'R键冲突，不能覆盖原输入'
    mapping.map_key(action, key)
sets = list(pawn.get_editor_property('AbilitySets'))
if ability_set not in sets:
    pawn.set_editor_property('AbilitySets', sets + [ability_set])
written.extend([action, config, mapping, pawn])

widget = blueprint('WBP_Resonance_Energy', unreal.HodgeResonanceEnergyWidget, folder='/Game/Main/UI/HUD', widget=True)
hud = unreal.load_asset('/Game/Main/UI/HUD/WBP_HodgeHUD')
assert unreal.HodgeUIAuthoringLibrary.configure_resonance_hud(hud, widget.generated_class())
written.append(hud)

# 装配成功之后才写交付标记，后续运行不覆盖用户调参。
compiled = {}
for asset in written:
    if isinstance(asset, unreal.Blueprint):
        diagnostic=unreal.HodgeAnimationAuthoringLibrary.compile_animation_blueprint(asset)
        assert diagnostic.startswith('ERRORS=0'),diagnostic
        compiled[asset.get_path_name()] = diagnostic
    unreal.EditorAssetLibrary.set_metadata_tag(asset, MARKER, '1')
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset, False), asset.get_path_name()
report = {'assets': sorted({asset.get_path_name() for asset in written}), 'compiled': compiled, 'head': head,
          'source': source.get_path_name(), 'length': montage.get_play_length(), 'camera_end': montage.get_play_length() * .5,
          'hit_time': hit_time, 'hit_time_status': '原型初值，需要结合实机动作继续调帧', 'gain_values': {'normal': 5, 'skill': 20, 'liberation': 0},
          'impacts': '留空，用户后续配置', 'hud': 'ReferenceHUDCanvas/ResonanceEnergy，右下(-40,-70)，250x48'}
(WORK / 'configuration.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps(report, ensure_ascii=False))
