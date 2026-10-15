"""检查实际大招资产、通知窗口、金刚体、输入及共享资源配置。"""
import json
import re
from pathlib import Path
import unreal

ROOT = '/Game/Main/Character/Hero/Ability/ResonanceLiberation'
WORK = Path(unreal.Paths.project_saved_dir()) / 'CinematicUltimate'
checks = []


def check(requirement, name, value, detail=None):
    """每项结果关联设计需求，失败保留具体配置。"""
    checks.append({'requirement': requirement, 'case': name, 'passed': bool(value), 'detail': detail})
    assert value, (name, detail)


def path(name):
    """只加载已装配的正式资产，不创建占位对象。"""
    result = unreal.load_asset(ROOT + '/' + name)
    assert result, name
    return result


definition = path('DA_Resonance_Liberation')
montage = path('AM_Burst01')
camera = path('DA_Resonance_Liberation_Camera')
ga = path('GA_Resonance_Liberation')
cdo = unreal.get_default_object(ga.generated_class())
check('CIN-01', '已有序列包装为正式蒙太奇', isinstance(montage, unreal.AnimMontage) and abs(montage.get_play_length() - 4.5333333) < .001)
check('CIN-01', 'Definition引用指定蒙太奇', definition.execution_config.montage == montage)
check('CIN-01', 'Standalone复用Melee', definition.execution_route == unreal.HodgeAbilityExecutionRoute.STANDALONE and isinstance(cdo, unreal.HodgeGameplayAbility_Melee))
check('CIN-01', '三个不同构图', len(camera.shots) == 3 and len({(row.start_offset.x, row.start_offset.y, row.start_offset.z) for row in camera.shots}) == 3)
events = unreal.AnimationLibrary.get_animation_notify_events(montage)
movie = [event for event in events if isinstance(event.notify_state_class, unreal.HodgeAnimNotifyState_CinematicCamera)]
wing = [event for event in events if isinstance(event.notify_state_class, unreal.HodgeAnimNotifyState_WingAction)]
hit = [event for event in events if isinstance(event.notify, unreal.HodgeAnimNotify_Hit)]
check('CIN-04', '单一电影窗口', len(movie) == 1 and movie[0].notify_state_class.profile == camera)
def duration(event):
    """UE5.5未公开结构体GetDuration，通过原生序列化值只读验证。"""
    return float(re.search(r'Duration=([\d.eE+-]+)', event.export_text()).group(1))


check('CIN-04', '窗口仅覆盖前半段', abs(duration(movie[0]) - montage.get_play_length() * .5) < .001)
check('CIN-04', '翼窗口覆盖全程', len(wing) == 1 and abs(duration(wing[0]) - montage.get_play_length()) < .001)
check('CIN-04', '正确翼蒙太奇', wing[0].notify_state_class.wing_montage.get_name() == 'AM_Wing_Burst01')
check('CIN-05', '仅一个正式伤害点', len(hit) == 1)
binding = hit[0].notify.hit
check('CIN-05', 'Box取角色根位置快照', binding.shape == unreal.HodgeHitShape.BOX and binding.source == unreal.HodgeAnimHitSource.AVATAR_ROOT and binding.transform_policy == unreal.HodgeHitTransformPolicy.SNAPSHOT_ON_EVENT_ENTER)
check('CIN-05', '查询仅执行一次', binding.profile.sample_mode == unreal.HodgeHitSampleMode.ONCE_ON_ENTER and binding.profile.query_mode == unreal.HodgeHitQueryMode.OVERLAP)
check('CIN-05', '每目标去重且继承金刚判定', binding.repeat_hit_interval == 0 and binding.use_default_reaction)
check('CIN-05', '自身金刚体', str(definition.execution_body_tag.get_editor_property('TagName')) == 'State.Combat.Body.Vajra')
check('CIN-05', '输出金刚体', str(definition.default_hit_config.reaction.attack_judgement_tag.get_editor_property('TagName')) == 'Combat.Attack.Judgement.Vajra')
check('RES-01', '大招不回能', definition.resonance_energy_gain_per_phase.value == 0.)
for asset_name in ['/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_'+str(index) for index in range(1,6)]+['/Game/Main/Character/Hero/Ability/ResonanceSkill/DA_Resonance_Skill']:
    attack=unreal.load_asset(asset_name)
    expected=20. if asset_name.endswith('DA_Resonance_Skill') else 5.
    check('RES-01',attack.get_name()+'_阶段回能资产配置',attack.resonance_energy_gain_per_phase.value==expected and bool(attack.resonance_energy_gain_effect))
costs = cdo.get_editor_property('AdditionalCosts')
check('RES-02', '只有一份额外资源成本', len(costs) == 1 and isinstance(costs[0], unreal.HodgeAbilityCost_ResonanceEnergy))
check('RES-02', '没有第二条原生成本路径', not cdo.get_editor_property('CostGameplayEffectClass'))
cd = unreal.get_default_object(path('GE_Resonance_Liberation_Cooldown').generated_class())
duration = cd.get_editor_property('DurationMagnitude').get_editor_property('ScalableFloatMagnitude').get_editor_property('Value')
check('RES-02', '独立12秒冷却', duration == 12.)
pawn = unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData')
ability_set = path('AS_Resonance_Liberation')
check('RES-01', 'PawnData授予且不重复资源Set', list(pawn.ability_sets).count(ability_set) == 1 and not ability_set.get_editor_property('GrantedAttributes'))
mapping = unreal.load_asset('/Game/Main/Input/IMC_Default')
own = [row for row in mapping.get_editor_property('Mappings') if row.action and row.action.get_name() == 'IA_Resonance_Liberation']
check('RES-03', 'R键输入唯一', len(own) == 1 and own[0].key.export_text() == 'R')
input_config=unreal.load_asset('/Game/Main/Input/DA_HodgeInputConfig')
tags=[row for row in input_config.get_editor_property('AbilityInputActions') if str(row.input_tag.get_editor_property('TagName'))=='InputTag.Ability.ResonanceLiberation']
check('RES-03','大招输入Tag只绑定一次',len(tags)==1 and tags[0].input_action==own[0].action)
for name in ['GE_ResonanceEnergy_Gain', 'GE_ResonanceEnergy_Cost', 'GE_ResonanceEnergy_Init', 'GE_Resonance_Liberation_Cooldown', 'GE_Resonance_Liberation_Damage', 'GA_Resonance_Liberation']:
    diagnostic = unreal.HodgeAnimationAuthoringLibrary.compile_animation_blueprint(path(name))
    check('CIN-07', name + '_原生编译诊断', diagnostic.startswith('ERRORS=0'), diagnostic)
    assert unreal.EditorAssetLibrary.save_loaded_asset(path(name), False)
for name in ['WBP_Resonance_Energy', 'WBP_HodgeHUD']:
    asset = unreal.load_asset('/Game/Main/UI/HUD/' + name)
    diagnostic = unreal.HodgeAnimationAuthoringLibrary.compile_animation_blueprint(asset)
    check('RES-04', name + '_原生编译诊断', diagnostic.startswith('ERRORS=0'), diagnostic)
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset, False)
report = {'passed': all(row['passed'] for row in checks), 'checks': checks}
(WORK / 'assets-results.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps({'passed': report['passed'], 'checks': len(checks)}, ensure_ascii=False))
