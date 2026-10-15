"""用真实敌人生命扣除验证暗流按阶段收益、元素增伤及拢音全局限流。"""
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
inputs = unreal.get_default_object(unreal.load_class(None, '/Script/Engine.SubsystemBlueprintLibrary')).call_method(
    'GetLocalPlayerSubSystemFromPlayerController', (owner.get_controller(), unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
config = unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData').get_editor_property('InputConfig')
attack = next(r.get_editor_property('InputAction') for r in config.get_editor_property('AbilityInputActions') if 'InputTag.Ability.Melee' in r.get_editor_property('InputTag').export_text())
targets = unreal.GameplayStatics.get_all_actors_of_class(server, unreal.HodgeEnemyCharacter)[:2]
assert len(targets) == 2, '多目标收益用例需要验证地图中两只敌人'
health = [p.get_component_by_class(unreal.HodgeHealthComponent) for p in targets]
ui = unreal.HodgeUIManagerSubsystem.get_gameplay_data_for_controller(owner.get_controller())
state = {'phase': 'warmup', 'at': unreal.GameplayStatics.get_time_seconds(server), 'wall': time.monotonic(), 'holding': False}
checks, samples = [], []


def inject(held=False):
    """保持真实普攻输入，不直接调用GA或伪造命中。"""
    inputs.inject_input_vector_for_action(attack, unreal.Vector(1, 0, 0) if held else unreal.Vector(), [], [])


def check(name, condition, detail=None):
    """每项断言都从真实ASC与目标HP取得证据。"""
    checks.append({'case': name, 'passed': bool(condition), 'detail': detail})
    assert condition, (name, detail)


def step(phase, now):
    """阶段等待只用于测试，不修改正式技能时间轴。"""
    state.update(phase=phase, at=now)


def place_targets():
    """每刀前摆放固定目标，命中窗口中不追随角色移动。"""
    position = authority.get_actor_location()
    for index, target in enumerate(targets):
        target.get_component_by_class(unreal.CharacterMovementComponent).disable_movement()
        target.set_actor_location(position + unreal.Vector(100 + index * 40, index * 80, 0), False, True)


def finish(error=None):
    """清理本次输入与回调，保留命中前后HP、暗流与R能量。"""
    unreal.unregister_slate_post_tick_callback(state['callback'])
    inject()
    result = {'mode': MODE, 'passed': not error, 'checks': checks, 'samples': samples, 'error': error,
              'fixture': '只在入态前通过正式GE补满资源；伤害和收益均由真实蒙太奇通知执行'}
    (WORK / (MODE + '-havoc-hits-e2e.json')).write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
    for name in ['worlds', 'server', 'clients', 'owner', 'authority', 'inputs', 'targets', 'health', 'ui']:
        globals()[name] = None
    gc.collect()


def tick(delta):
    """先验证常态命中，再比较强化伤害及两刀间不足一秒的拢音去重。"""
    try:
        assert time.monotonic() - state['wall'] < 55., '命中验收超时：' + state['phase']
        now = unreal.GameplayStatics.get_time_seconds(server)
        age = now - state['at']
        flow = lib.inspect_dark_flow(authority).x
        energy = lib.inspect_resonance_energy(authority).x
        hp = [h.get_health() for h in health]
        samples.append({'time': now, 'phase': state['phase'], 'flow': flow, 'energy': energy, 'hp': hp})
        if state['holding']:
            inject(True)
        phase = state['phase']
        if phase == 'warmup' and age > 1.:
            for pawn in {owner, authority}:
                pawn.set_actor_location(unreal.Vector(1000, 1000, 100), False, True)
                pawn.set_actor_rotation(unreal.Rotator(), False)
                pawn.get_controller().set_control_rotation(unreal.Rotator())
            place_targets()
            state['initial_hp'] = hp
            inject(True)
            step('normal_hit', now)
        elif phase == 'normal_hit':
            inject()
            if flow > 0. and age > .5:
                check('HDS-06 常态普攻受检生命伤害只回5点暗流', flow == 5. and any(a > b for a, b in zip(state['initial_hp'], hp)), samples[-1])
                state['normal_damage'] = state['initial_hp'][0] - hp[0]
                step('wait_normal', now)
        elif phase == 'wait_normal' and age > 4.7:
            # 入态空挥隔离，避免灭音伤害混入强化倍率比较。
            for target in targets:
                target.set_actor_location(unreal.Vector(2300, 2300, 100), False, True)
            assert lib.queue_dark_flow_delta(authority, 100.)
            assert lib.queue_resonance_energy_delta(authority, -100.)
            step('filled', now)
        elif phase == 'filled' and ui.get_havoc_snapshot().current_dark_flow == 100.:
            state['holding'] = True
            step('enter', now)
        elif phase == 'enter' and ui.get_havoc_snapshot().dark_surge:
            state['holding'] = False
            inject()
            step('wait_circuit', now)
        elif phase == 'wait_circuit' and age > 2.7:
            state['hp_before'] = hp
            place_targets()
            inject(True)
            step('enhanced_1', now)
        elif phase == 'enhanced_1':
            inject()
            if energy > 0. and age > .3:
                check('HDS-06 强化普攻多目标共享阶段回能和拢音，R只增加6', energy == 6., samples[-1])
                check('HDS-04 两个目标都收到实际强化攻击伤害', all(a > b for a, b in zip(state['hp_before'], hp)), samples[-1])
                damage = state['hp_before'][0] - hp[0]
                check('HDS-04 相同伤害配置湮灭增伤20%', abs(damage - 1.2 * state['normal_damage']) < .1, {'normal': state['normal_damage'], 'enhanced': damage})
                state['first_hit_time'] = now
                step('next', now)
        elif phase == 'next' and age > .35:
            place_targets()
            inject(True)
            step('enhanced_2', now)
        elif phase == 'enhanced_2':
            inject()
            if energy > 6.:
                check('HDS-06 换段未满一秒不重复拢音，R累计11', energy == 11. and now - state['first_hit_time'] < 1., samples[-1])
                others = [lib.inspect_dark_flow(p).x for p in unreal.GameplayStatics.get_all_actors_of_class(server, unreal.HodgeHeroCharacter) if p != authority]
                check('网络收益只作用于施法者ASC', all(value == 0. for value in others), others)
                finish()
    except Exception:
        finish(traceback.format_exc())


state['callback'] = unreal.register_slate_post_tick_callback(tick)
