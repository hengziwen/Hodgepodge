"""通过真实技能输入验证发动、命中、冷却拒绝、到期和 Dash 取消后保留 CD。"""
import gc
import json
import time
import traceback
from pathlib import Path
import unreal

WORK = Path(globals()['LOCK_ON_WORK'])
mode = globals()['LOCK_ON_MODE']
worlds = unreal.EditorLevelLibrary.get_pie_worlds(True)
server = next(world for world in worlds if unreal.SystemLibrary.is_server(world))
clients = [world for world in worlds if world != server]
owner = next(pawn for pawn in unreal.GameplayStatics.get_all_actors_of_class(clients[0] if clients else server, unreal.HodgeHeroCharacter)
             if not clients or pawn.get_local_role() == unreal.NetRole.ROLE_AUTONOMOUS_PROXY)
pid = owner.get_editor_property('PlayerState').get_editor_property('PlayerId')
authority = next(pawn for pawn in unreal.GameplayStatics.get_all_actors_of_class(server, unreal.HodgeHeroCharacter) if pawn.get_editor_property('PlayerState').get_editor_property('PlayerId') == pid)
inputs = unreal.get_default_object(unreal.load_class(None, '/Script/Engine.SubsystemBlueprintLibrary')).call_method(
    'GetLocalPlayerSubSystemFromPlayerController', (owner.get_controller(), unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
bridge = unreal.get_default_object(unreal.load_class(None, '/Script/GameplayAbilities.AbilitySystemBlueprintLibrary'))
skill_input = unreal.load_asset('/Game/Main/Input/InputAction/IA_Resonance_Skill')
dash_input = unreal.load_asset('/Game/Main/Input/InputAction/IA_Sprint')
montage = unreal.load_asset('/Game/Main/Character/Hero/Ability/ResonanceSkill/AM_Resonance_Skill')
skill_class = unreal.load_class(None, '/Game/Main/Character/Hero/Ability/ResonanceSkill/GA_Resonance_Skill.GA_Resonance_Skill_C')
assert skill_class
cooldown_class = unreal.load_class(None, '/Game/Main/Character/Hero/Ability/ResonanceSkill/GE_Resonance_Skill_Cooldown.GE_Resonance_Skill_Cooldown_C')
assert cooldown_class
cooldown_tags = unreal.GameplayTagContainer()
cooldown_tags.import_text('(GameplayTags=((TagName="Cooldown.Hero.ResonanceSkill")))')
checks, samples = [], []
state = {'wall': time.monotonic(), 'stage': 0, 'at': unreal.GameplayStatics.get_time_seconds(server), 'damage': False, 'started': False}

for world in worlds:
    if mode == 'listen-lag':
        unreal.SystemLibrary.execute_console_command(world, 'Net PktLag=100')
    for pawn in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.HodgeHeroCharacter):
        if pawn.get_editor_property('PlayerState').get_editor_property('PlayerId') != pid:
            pawn.get_component_by_class(unreal.CharacterMovementComponent).disable_movement()
    for enemy in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.HodgeEnemyCharacter):
        enemy.get_component_by_class(unreal.CharacterMovementComponent).disable_movement()
        enemy.set_actor_location(unreal.Vector(2300, 2300, 100), False, True)

target = unreal.GameplayStatics.get_all_actors_of_class(server, unreal.HodgeEnemyCharacter)[0]
target.set_actor_location(unreal.Vector(1150, 980, 100), False, True)
initial_hp = target.get_component_by_class(unreal.HodgeHealthComponent).get_health()
for pawn in {owner, authority}:
    pawn.set_actor_location(unreal.Vector(1000, 1000, 100), False, True)
    pawn.set_actor_rotation(unreal.Rotator(), False)
    pawn.get_controller().set_control_rotation(unreal.Rotator())

def inject(action, pressed=False):
    """只注入实际 InputAction，保留原有授予和 Standalone 激活流程。"""
    inputs.inject_input_vector_for_action(action, unreal.Vector(1, 0, 0) if pressed else unreal.Vector(), [], [])

def check(name, condition, detail=None):
    """断言失败停止并保留完整过程数据。"""
    checks.append({'check': name, 'passed': bool(condition), 'detail': detail})
    assert condition, (name, detail)

def snapshot(pawn):
    """读取实际 GA Spec 和冷却 ActiveGE，不依据 UI 或动画猜测 CD。"""
    asc = pawn.get_hodge_ability_system_component()
    active = False
    granted = 0
    for handle in asc.get_all_abilities():
        ability = bridge.call_method('GetGameplayAbilityFromSpecHandle', (asc, handle))[0]
        if ability and ability.get_class().get_path_name() == skill_class.get_path_name():
            granted += 1
            active |= bool(bridge.call_method('IsGameplayAbilityActive', (ability,)))
    info = unreal.HodgeCombatValidationLibrary.inspect_montage_state(pawn, montage)
    return {'active': active, 'granted': granted, 'cooldowns': asc.call_method('GetGameplayEffectCount', (cooldown_class, None, True)),
            'instance': info.instance_id, 'position': info.position, 'stopped': info.stopped}

def transition(stage, now):
    """以游戏时间而非墙钟等待动画和冷却。"""
    state.update({'stage': stage, 'at': now})

def finish(error=None):
    """解除回调和输入，报告写入 Saved，不改正式资产。"""
    unreal.unregister_slate_post_tick_callback(state['callback'])
    inject(skill_input)
    inject(dash_input)
    result = {'mode': mode, 'passed': not error, 'checks': checks, 'samples': samples, 'error': error}
    (WORK / (mode + '-resonance-e2e.json')).write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
    print(json.dumps({'mode': mode, 'passed': result['passed'], 'checks': len(checks), 'error': error}, ensure_ascii=False))
    for name in ['worlds', 'clients', 'owner', 'authority', 'server', 'inputs', 'target', 'world', 'pawn', 'enemy']:
        globals()[name] = None
    gc.collect()

def tick(delta):
    """依次检查一次发动、CD 内拒绝、8 秒到期和真实 Dash 中断。"""
    try:
        assert time.monotonic() - state['wall'] < 100, 'Resonance timeout'
        now = unreal.GameplayStatics.get_time_seconds(server)
        age = now - state['at']
        local, remote = snapshot(owner), snapshot(authority)
        samples.append({'time': now, 'stage': state['stage'], 'owner': local, 'authority': remote})
        if state['stage'] == 0:
            if age < .7:
                return
            check('SK-03_拥有者唯一授予', local['granted'] == 1)
            check('SK-03_服务器唯一授予', remote['granted'] == 1)
            inject(skill_input, True)
            state['first_cast'] = now
            transition(1, now)
        elif state['stage'] == 1:
            inject(skill_input)
            if local['active'] and remote['active'] and local['cooldowns'] == 1 and remote['cooldowns'] == 1 and not state['started']:
                state['started'] = True
                state['first_instance'] = local['instance']
                check('SK-01_双方实际发动', local['instance'] >= 0 and remote['instance'] >= 0)
                check('SK-02_双方获得独立CD', True)
            if not state['damage'] and unreal.SystemLibrary.is_valid(target):
                state['damage'] = target.get_component_by_class(unreal.HodgeHealthComponent).get_health() < initial_hp
            if not state['started'] or local['active'] or remote['active']:
                assert age < 7, ('技能未正常发动或结束', local, remote)
                return
            check('SK-01_攻击造成实际伤害', state['damage'])
            check('SK-02_动画结束CD仍存在', local['cooldowns'] == 1 and remote['cooldowns'] == 1)
            inject(skill_input, True)
            state['recast_seen'] = False
            transition(2, now)
        elif state['stage'] == 2:
            inject(skill_input)
            state['recast_seen'] |= local['active'] or remote['active'] or (local['instance'] >= 0 and local['instance'] != state['first_instance'])
            if age < .45:
                return
            check('SK-02_CD期间不能再发动', not state['recast_seen'] and local['cooldowns'] == 1 and remote['cooldowns'] == 1)
            transition(3, now)
        elif state['stage'] == 3:
            if local['cooldowns'] or remote['cooldowns']:
                assert now - state['first_cast'] < 11, ('冷却没有到期', local, remote)
                return
            check('SK-02_冷却至少8秒', now - state['first_cast'] >= 7.9, now - state['first_cast'])
            inject(skill_input, True)
            transition(4, now)
        elif state['stage'] == 4:
            inject(skill_input)
            if not local['active'] or not remote['active'] or local['instance'] < 0 or remote['instance'] < 0:
                assert age < 2, ('到期无法再发动', local, remote)
                return
            check('SK-02_到期可再次发动', local['instance'] != state['first_instance'])
            inject(dash_input, True)
            transition(5, now)
        elif state['stage'] == 5:
            inject(dash_input)
            if age < .7:
                return
            check('SK-04_Dash真实中断技能', not local['active'] and not remote['active'])
            check('SK-04_中断后CD仍保留', local['cooldowns'] == 1 and remote['cooldowns'] == 1)
            finish()
    except Exception:
        finish(traceback.format_exc())

state['callback'] = unreal.register_slate_post_tick_callback(tick)
