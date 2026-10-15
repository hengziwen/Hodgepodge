"""真实普攻长按与E输入验收；补满/驱散仅作为明确标记的权威GE测试夹具。"""
import gc
import json
import time
import traceback
from pathlib import Path
import unreal

WORK = Path(globals()['LOCK_ON_WORK'])
MODE = globals()['LOCK_ON_MODE']
worlds = unreal.EditorLevelLibrary.get_pie_worlds(True)
server = next(w for w in worlds if unreal.SystemLibrary.is_server(w))
clients = [w for w in worlds if w != server]
owner = next(p for p in unreal.GameplayStatics.get_all_actors_of_class(clients[0] if clients else server, unreal.HodgeHeroCharacter)
             if not clients or p.get_local_role() == unreal.NetRole.ROLE_AUTONOMOUS_PROXY)
pid = owner.get_editor_property('PlayerState').get_editor_property('PlayerId')
authority = next(p for p in unreal.GameplayStatics.get_all_actors_of_class(server, unreal.HodgeHeroCharacter)
                 if p.get_editor_property('PlayerState').get_editor_property('PlayerId') == pid)
lib = unreal.HodgeCombatValidationLibrary
ui = unreal.HodgeUIManagerSubsystem.get_gameplay_data_for_controller(owner.get_controller())
inputs = unreal.get_default_object(unreal.load_class(None, '/Script/Engine.SubsystemBlueprintLibrary')).call_method(
    'GetLocalPlayerSubSystemFromPlayerController', (owner.get_controller(), unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
actions = {'E': unreal.load_asset('/Game/Main/Input/InputAction/IA_Resonance_Skill')}
# 普攻使用当前InputConfig的真实映射，避免猜测InputAction资产名。
config = unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData').get_editor_property('InputConfig')
actions['Attack'] = next(row.get_editor_property('InputAction') for row in config.get_editor_property('AbilityInputActions')
                         if 'InputTag.Ability.Melee' in row.get_editor_property('InputTag').export_text())
base = '/Game/Main/Character/Hero/Ability/Havoc/'
montages = [unreal.load_asset(base + 'AM_Ex_Attack%02d_Montage' % i) for i in range(1, 6)]
circuit = unreal.load_asset(base + 'AM_Ex_Skill01_01_Montage')
po_ming = unreal.load_asset(base + 'AM_Ex_Skill02_Montage')
e_tag = unreal.GameplayTag()
e_tag.import_text('(TagName="InputTag.Ability.ResonanceSkill")')
checks, samples = [], []
state = {'phase': 'warmup', 'at': unreal.GameplayStatics.get_time_seconds(server), 'wall': time.monotonic(), 'holding': False}


def inject(key, held=False):
    """Enhanced Input注入保留真实Started/Completed及GAS预测入口。"""
    inputs.inject_input_vector_for_action(actions[key], unreal.Vector(1, 0, 0) if held else unreal.Vector(), [], [])


def check(name, condition, detail=None):
    """失败保留现场采样，不将编辑器调用成功当成玩法成功。"""
    checks.append({'case': name, 'passed': bool(condition), 'detail': detail})
    assert condition, (name, detail)


def step(phase, now):
    """测试时钟只用于等待与断言，不驱动生产Buff持续时间。"""
    state.update(phase=phase, at=now)
    (WORK / (MODE + '-progress.json')).write_text(json.dumps({'phase': phase, 'time': now}), encoding='utf-8')


def finish(error=None):
    """解除本次输入和Slate回调，保存每个断言与原始资源采样。"""
    unreal.unregister_slate_post_tick_callback(state['callback'])
    for key in actions:
        inject(key)
    result = {'mode': MODE, 'passed': not error and all(c['passed'] for c in checks), 'checks': checks,
              'samples': samples, 'error': error, 'fixture': 'QueueDarkFlowDelta补能、QueueDarkSurgeDispel驱散；技能激活均为真实输入'}
    (WORK / (MODE + '-havoc-e2e.json')).write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
    for name in ['worlds', 'server', 'clients', 'owner', 'authority', 'inputs', 'ui']:
        globals()[name] = None
    gc.collect()


def tick(delta):
    """验证施放收益、共享CD、两套动作选择、燃料延长及驱散清空。"""
    try:
        assert time.monotonic() - state['wall'] < 115., '暗涌验收超时：' + state['phase']
        now = unreal.GameplayStatics.get_time_seconds(server)
        elapsed = now - state['at']
        resource = lib.inspect_dark_flow(authority)
        local = ui.get_havoc_snapshot()
        e_display = ui.get_ability_display_state(e_tag)
        current = [lib.inspect_montage_state(owner, montage) for montage in montages]
        sample = {'time': now, 'phase': state['phase'], 'authority_fuel': resource.x, 'local_fuel': local.current_dark_flow,
                  'mode': local.dark_surge, 'e_cd': e_display.cooldown_remaining}
        samples.append(sample)
        if state['holding']:
            inject('Attack', True)
        phase = state['phase']
        if phase == 'warmup' and elapsed > 1.:
            check('HDS-01 正式能力集初始化100容量且初始0', resource.y == 100. and resource.x == 0., sample)
            runtime = json.loads(unreal.HodgeUIAuthoringLibrary.inspect_player_ui(owner.get_controller()))
            check('HDS-07 Experience注入四个扩展，暗流数据源就绪', runtime.get('entries') == 4 and local.ready, runtime)
            inject('E', True)
            step('normalE', now)
        elif phase == 'normalE':
            inject('E')
            if resource.x == 20. and e_display.cooldown_remaining > 0.:
                check('HDS-06 普通E空挥成功施放回20点', True, sample)
                state['normal_cd_end'] = now + e_display.cooldown_remaining
                step('normalE_end', now)
        elif phase == 'normalE_end' and elapsed > 4.5:
            assert lib.queue_dark_flow_delta(authority, 100.)
            step('fill', now)
        elif phase == 'fill' and local.current_dark_flow == 100.:
            state['holding'] = True
            step('hold', now)
        elif phase == 'hold':
            montage_state = lib.inspect_montage_state(owner, circuit)
            if montage_state.playing:
                check('HDS-01 真实长按普攻切入灭音Montage', True, sample)
                state['holding'] = False
                inject('Attack')
                step('enter', now)
        elif phase == 'enter' and local.dark_surge:
            check('HDS-02 空挥提交可入态且不一次扣空100', 90. < resource.x <= 100., sample)
            state['entered'] = now
            inject('E', True)
            step('shared_cd', now)
        elif phase == 'shared_cd':
            inject('E')
            if elapsed > .2:
                check('HDS-05 入态继承普通E未结束的8秒CD', e_display.cooldown_remaining > 0. and not lib.inspect_montage_state(owner, po_ming).playing, sample)
                step('circuit_end', now)
        elif phase == 'circuit_end' and elapsed > 2.7:
            state['attack_index'] = 0
            inject('Attack', True)
            step('enhanced', now)
        elif phase == 'enhanced':
            inject('Attack')
            i = state['attack_index']
            if current[i].playing and elapsed > .12:
                check('HDS-05 强化普攻第%d段正确' % (i + 1), True, sample)
                step('next_attack', now)
        elif phase == 'next_attack':
            i = state['attack_index']
            if elapsed > [1., .85, 1.0, 1.45, 1.1][i]:
                if i < 4:
                    state['attack_index'] += 1
                    inject('Attack', True)
                    step('enhanced', now)
                else:
                    inject('E', True)
                    state['before_po_ming'] = resource.x
                    step('po_ming', now)
        elif phase == 'po_ming':
            inject('E')
            if lib.inspect_montage_state(owner, po_ming).playing and resource.x > state['before_po_ming'] + 12.:
                check('HDS-05/06 破命使用新动作并成功施放回能', True, sample)
                check('HDS-05 破命重新建立共享8秒CD', 5. < e_display.cooldown_remaining <= 8., sample)
                assert lib.queue_dark_flow_delta(authority, 100.)
                step('extended', now)
        elif phase == 'extended' and now - state['entered'] > 11.:
            check('HDS-02 超过基础10秒仍由燃料维持，无额外时长上限', local.dark_surge and resource.x > 0., sample)
            assert lib.queue_dark_surge_dispel(authority)
            step('dispel', now)
        elif phase == 'dispel' and elapsed > .5:
            check('生命周期 驱散清空暗流并退态', not local.dark_surge and resource.x == 0. and local.current_dark_flow == 0., sample)
            check('HDS-05 退态不刷新E冷却', e_display.cooldown_remaining > 0., sample)
            inject('Attack', True)
            step('normal_again', now)
        elif phase == 'normal_again':
            inject('Attack')
            normal = unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_1').get_editor_property('ExecutionConfig').montage
            if lib.inspect_montage_state(owner, normal).playing:
                check('HDS-05 退态后下一次输入从普通第一段开始', True, sample)
                finish()
    except Exception:
        finish(traceback.format_exc())


if MODE == 'listen-lag':
    for world in worlds:
        unreal.SystemLibrary.execute_console_command(world, 'Net PktLag=100')
state['callback'] = unreal.register_slate_post_tick_callback(tick)
