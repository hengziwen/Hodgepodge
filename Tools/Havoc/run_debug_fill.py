"""实际控制台回满暗流，验证远端RPC、玩家隔离和暗涌中正常消费。"""
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
config = unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData').get_editor_property('InputConfig')
attack = next(r.get_editor_property('InputAction') for r in config.get_editor_property('AbilityInputActions') if 'InputTag.Ability.Melee' in r.get_editor_property('InputTag').export_text())
e_action = unreal.load_asset('/Game/Main/Input/InputAction/IA_Resonance_Skill')
e_tag = unreal.GameplayTag()
e_tag.import_text('(TagName="InputTag.Ability.ResonanceSkill")')
state_class = unreal.load_class(None, '/Game/Main/Character/Hero/Ability/Havoc/GE_DarkSurge_State.GE_DarkSurge_State_C')
checks = []
state = {'phase': 'warmup', 'at': unreal.GameplayStatics.get_time_seconds(server), 'wall': time.monotonic(), 'holding': False}


def check(name, condition, detail=None):
    """依据真实权威资源和拥有者复制确认命令效果。"""
    checks.append({'case': name, 'passed': bool(condition), 'detail': detail})
    assert condition, (name, detail)


def command():
    """只排队执行本地玩家控制台，属性变更及RPC全部走正式命令。"""
    assert unreal.HodgeUIAuthoringLibrary.queue_ui_action(owner.get_controller(), 'DarkFlowFill')


def step(phase, now):
    """测试计时只等待复制与动作，不控制资源消费。"""
    state.update(phase=phase, at=now)


def finish(error=None):
    """撤销本次输入与回调，保存断言，不修改正式资产。"""
    unreal.unregister_slate_post_tick_callback(state['callback'])
    for action in [attack, e_action]:
        inputs.inject_input_vector_for_action(action, unreal.Vector(), [], [])
    result = {'mode': MODE, 'passed': not error and all(r['passed'] for r in checks), 'checks': checks, 'error': error}
    (WORK / (MODE + '-havoc-debug-e2e.json')).write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
    for name in ['worlds', 'server', 'clients', 'owner', 'authority', 'ui', 'inputs']:
        globals()[name] = None
    gc.collect()


def tick(delta):
    """检查回满、重复、原CD保持，以及Buff中补能后继续消费。"""
    try:
        assert time.monotonic() - state['wall'] < 40., '暗流命令验证超时：' + state['phase']
        now = unreal.GameplayStatics.get_time_seconds(server)
        age = now - state['at']
        remote = lib.inspect_dark_flow(authority)
        local = lib.inspect_dark_flow(owner)
        snapshot = ui.get_havoc_snapshot()
        e_display = ui.get_ability_display_state(e_tag)
        phase = state['phase']
        if state['holding']:
            inputs.inject_input_vector_for_action(attack, unreal.Vector(1, 0, 0), [], [])
        if phase == 'warmup' and age > 1.:
            check('正式初始化容量100且暗流0', remote.y == 100. and remote.x == 0. and local.x == 0.)
            command()
            step('fill', now)
        elif phase == 'fill' and local.x == 100. and remote.x == 100.:
            check('实际控制台使服务器与拥有者回满', True)
            check('HUD同步满条且不自动入态', snapshot.percent == 1. and not snapshot.dark_surge)
            others = [lib.inspect_dark_flow(p).x for p in unreal.GameplayStatics.get_all_actors_of_class(server, unreal.HodgeHeroCharacter) if p != authority]
            check('只补当前玩家暗流', all(value == 0. for value in others), others)
            check('不会补充R能量', lib.inspect_resonance_energy(authority).x == 0.)
            inputs.inject_input_vector_for_action(e_action, unreal.Vector(1, 0, 0), [], [])
            step('castE', now)
        elif phase == 'castE':
            inputs.inject_input_vector_for_action(e_action, unreal.Vector(), [], [])
            if e_display.cooldown_remaining > 0.:
                state['cd_end'] = now + e_display.cooldown_remaining
                command()
                step('repeat', now)
        elif phase == 'repeat' and age > .3:
            check('重复回满不溢出', local.x == 100. and remote.x == 100.)
            check('E冷却未重置或清除', e_display.cooldown_remaining > 0. and abs(now + e_display.cooldown_remaining - state['cd_end']) < .3)
            step('waitE', now)
        elif phase == 'waitE' and age > 4.5:
            state['holding'] = True
            step('enter', now)
        elif phase == 'enter' and snapshot.dark_surge:
            state['holding'] = False
            inputs.inject_input_vector_for_action(attack, unreal.Vector(), [], [])
            step('drain', now)
        elif phase == 'drain' and age > .7:
            check('正式灭音后暗流正在消耗', 0. < remote.x < 99.)
            command()
            step('refill_mode', now)
        elif phase == 'refill_mode' and remote.x > 98.5 and age > .12:
            check('暗涌中命令补满燃料', snapshot.dark_surge and local.x > 97., {'server': remote.x, 'owner': local.x})
            count = authority.get_hodge_ability_system_component().call_method('GetGameplayEffectCount', (state_class, None, True))
            check('补能不重复施加Buff', count == 1)
            state['filled_fuel'] = remote.x
            step('still_drain', now)
        elif phase == 'still_drain' and age > .5:
            check('补满后仍按正常速率继续消耗', snapshot.dark_surge and 3. < state['filled_fuel'] - remote.x < 8.)
            finish()
    except Exception:
        finish(traceback.format_exc())


state['callback'] = unreal.register_slate_post_tick_callback(tick)
