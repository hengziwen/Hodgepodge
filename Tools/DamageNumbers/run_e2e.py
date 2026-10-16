"""正式普攻/E/R 与独立 GE 压力夹具；每项对比权威损失和拥有者接收，不重存正式资产。"""
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
owner_world = server if MODE == 'listen-host' or not clients else clients[0]
owner = next(p for p in unreal.GameplayStatics.get_all_actors_of_class(owner_world, unreal.HodgeHeroCharacter)
             if p.get_controller() and p.get_controller().is_local_controller())
pid = owner.get_editor_property('PlayerState').get_editor_property('PlayerId')
authority = next(p for p in unreal.GameplayStatics.get_all_actors_of_class(server, unreal.HodgeHeroCharacter)
                 if p.get_editor_property('PlayerState').get_editor_property('PlayerId') == pid)
pc, sender = owner.get_controller(), authority.get_controller()
pop = pc.get_number_pop_component()
lib = unreal.HodgeCombatValidationLibrary
inputs = unreal.get_default_object(unreal.load_class(None, '/Script/Engine.SubsystemBlueprintLibrary')).call_method(
    'GetLocalPlayerSubSystemFromPlayerController', (pc, unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
config = unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData').get_editor_property('InputConfig')
bindings = config.get_editor_property('AbilityInputActions')
attack = next(r.get_editor_property('InputAction') for r in bindings if 'InputTag.Ability.Melee' in r.get_editor_property('InputTag').export_text())
targets = unreal.GameplayStatics.get_all_actors_of_class(server, unreal.HodgeEnemyCharacter)[:2]
assert len(targets) == 2
health = [p.get_component_by_class(unreal.HodgeHealthComponent) for p in targets]
others = [c.get_number_pop_component() for w in worlds for c in unreal.GameplayStatics.get_all_actors_of_class(w, unreal.HodgePlayerController)
          if c.is_local_controller() and c != pc]
checks, samples, hits = [], [], []
debug_names = ['Hodge.Combat.DebugDraw.Weapon', 'Hodge.Combat.DebugDraw.Body', 'Hodge.Combat.DebugDraw.HitBox']
debug_values = {name: unreal.SystemLibrary.get_console_variable_int_value(name) for name in debug_names}
for name in debug_names:
    unreal.SystemLibrary.execute_console_command(owner, name + ' 0')
state = {'phase': 'warmup', 'at': unreal.GameplayStatics.get_time_seconds(server), 'wall': time.monotonic(),
         'transitions': [], 'last_montage': None, 'next_pressed': False, 'shots': 0}
CAPTURE = bool(globals().get('NUMBER_CAPTURE', False))
capture_fps = unreal.SystemLibrary.get_console_variable_float_value('t.OverrideFPS')
if CAPTURE:
    # 仅离线取证使用固定仿真步长，不把慢速逐帧渲染称为实时性能。
    unreal.SystemLibrary.execute_console_command(owner, 't.OverrideFPS 60')
    (WORK / 'Frames').mkdir(exist_ok=True)
    state['frame'] = 0


def inject(pressed=False, action=attack):
    """真实 Enhanced Input 触发正式技能，避免编辑器 RPC 调用空间差异。"""
    inputs.inject_input_vector_for_action(action, unreal.Vector(1, 0, 0) if pressed else unreal.Vector(), [], [])


def check(name, condition, detail=None):
    """断言失败保留时间线，不把网关调用成功当作玩法成功。"""
    checks.append({'case': name, 'passed': bool(condition), 'detail': detail})
    assert condition, (name, detail)


def step(phase, now):
    state.update(phase=phase, at=now)


def place():
    """只在每段开始摆放静止目标，命中窗口不追随武器伪造接触。"""
    for index, target in enumerate(targets):
        target.get_component_by_class(unreal.CharacterMovementComponent).disable_movement()
        target.set_actor_location(authority.get_actor_location() + unreal.Vector(110 + 20 * index, 50 * index, 0), False, True)


def active_definition():
    """按正式 ASC 能力句柄和实际 Montage 观测五段顺序。"""
    asc = owner.get_hodge_ability_system_component()
    for handle in asc.get_all_abilities():
        definition = asc.find_ability_definition(handle)
        if definition:
            montage = definition.get_editor_property('ExecutionConfig').get_editor_property('Montage')
            if montage and lib.inspect_montage_state(owner, montage).playing:
                return definition, montage
    return None, None


def finish(error=None):
    """配对清理输入和回调，报告明确区分真实攻击与 GE 压力夹具。"""
    unreal.unregister_slate_post_tick_callback(state['callback'])
    inject()
    for name, value in debug_values.items():
        unreal.SystemLibrary.execute_console_command(owner, name + ' ' + str(value))
    if CAPTURE:
        unreal.SystemLibrary.execute_console_command(owner, 't.OverrideFPS ' + str(capture_fps))
    result = {'mode': MODE, 'passed': not error, 'checks': checks, 'samples': samples, 'hits': hits,
              'transitions': state['transitions'], 'error': error, 'capture': {'enabled': CAPTURE, 'frames': state.get('frame', 0), 'fps': 60, 'kind': 'offline fixed simulation step'},
              'font_backend': 'Lyra Niagara 原资源，未宣称鸣潮视觉还原'}
    (WORK / (MODE + '-numbers-e2e.json')).write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
    for name in ['worlds', 'server', 'clients', 'owner', 'authority', 'pc', 'sender', 'pop', 'inputs', 'targets', 'health', 'others']:
        globals()[name] = None
    gc.collect()


def tick(delta):
    """先测正式连段与技能，再测多目标、AI 路由和高频纯表现承载。"""
    try:
        assert time.monotonic() - state['wall'] < 100., 'Timeout: ' + state['phase']
        now = unreal.GameplayStatics.get_time_seconds(server)
        age = now - state['at']
        hp = [h.get_health() for h in health]
        row = {'time': now, 'phase': state['phase'], 'hp': hp, 'sent': sender.get_damage_number_sent_count(),
               'received': pop.get_received_count(), 'rendered': pop.get_rendered_count(), 'delta': delta,
               'others': [p.get_received_count() for p in others]}
        samples.append(row)
        if row['sent'] != state.get('last_sent', 0):
            request = sender.get_last_damage_number_sent()
            hits.append({'time': now, 'event': request.event_id, 'amount': request.damage_amount,
                         'position': [request.world_location.x, request.world_location.y, request.world_location.z],
                         'phase': state['phase'], 'hp': hp, 'montage': state['last_montage'].get_name() if state['last_montage'] else None})
            state['last_sent'] = row['sent']
        phase = state['phase']
        if phase == 'warmup' and age > .5:
            for w in worlds:
                for hero in unreal.GameplayStatics.get_all_actors_of_class(w, unreal.HodgeHeroCharacter):
                    hero.set_actor_location(unreal.Vector(-2500, -2500, 100), False, True)
            for pawn in {owner, authority}:
                pawn.set_actor_location(unreal.Vector(1000, 1000, 100), False, True)
                pawn.set_actor_rotation(unreal.Rotator(), False)
            pc.set_control_rotation(unreal.Rotator(pitch=-12))
            for target in targets:
                check('PIE fixture max HP reset queued', lib.queue_number_pop_probe(authority, target, 0., True))
            place()
            step('ready', now)
        elif phase == 'ready' and age > .5:
            check('HP reset emits no damage number', row['sent'] == 0)
            state['hp_before'] = hp
            inject(True)
            step('combo', now)
        elif phase == 'combo':
            inject()
            if CAPTURE and age < 7.5:
                frame = state['frame']
                state['frame'] += 1
                unreal.SystemLibrary.execute_console_command(owner, 'HighResShot 960x540 filename="' + str(WORK / 'Frames' / ('frame-%04d.png' % frame)).replace('\\', '/') + '"')
            definition, montage = active_definition()
            if montage:
                if montage != state['last_montage']:
                    state['last_montage'] = montage
                    state['next_pressed'] = False
                    state['transitions'].append(montage.get_name())
                    place()
                starts = [unreal.AnimationLibrary.get_anim_notify_event_trigger_time(e)
                          for e in unreal.AnimationLibrary.get_animation_notify_events(montage)
                          if isinstance(e.get_editor_property('NotifyStateClass'), unreal.HodgeAnimNotifyState_GameplayTag)
                          and 'Status.Attack.Cancel.NextAttack' in e.get_editor_property('NotifyStateClass').get_editor_property('StateTag').export_text()]
                position = lib.inspect_montage_state(owner, montage).position
                if len(state['transitions']) < 5 and not state['next_pressed'] and starts and position >= min(starts) + .03:
                    inject(True)
                    state['next_pressed'] = True
            if not CAPTURE and MODE == 'single' and row['rendered'] > 0 and state['shots'] < 4 and now - state.get('last_shot', now - 1.) >= .18:
                # 实际游戏截图与报告同目录；图片检查另行进行，不以 RPC 数量代替可见性。
                state['shots'] += 1
                state['last_shot'] = now
                unreal.SystemLibrary.execute_console_command(owner, 'HighResShot 960x540 filename="' + str(WORK / ('real-combo-' + str(state['shots']) + '.png')).replace('\\', '/') + '"')
            if age > 9.:
                check('Formal five-attack sequence', state['transitions'][:5] == ['AM_Attack01_Montage', 'AM_Attack02_Montage', 'AM_Attack03_Montage', 'AM_Attack05_Montage', 'AM_Attack04_Montage'], state['transitions'])
                check('Formal notify damage creates local Niagara output', row['rendered'] > 0 and any(a > b for a, b in zip(state['hp_before'], hp)), row)
                state['combo_sent'] = row['sent']
                action = next(r.get_editor_property('InputAction') for r in bindings if 'InputTag.Ability.ResonanceSkill' in r.get_editor_property('InputTag').export_text())
                state['skill_action'] = action
                place(); inject(True, action)
                step('E', now)
        elif phase == 'E':
            inject(False, state['skill_action'])
            if age > 5.:
                check('E automatically shares accepted-damage route', row['sent'] > state['combo_sent'], row)
                state['E_sent'] = row['sent']
                check('R energy fixture queued', lib.queue_resonance_energy_delta(authority, 100.))
                step('R_fill', now)
        elif phase == 'R_fill' and age > .5:
            action = next(r.get_editor_property('InputAction') for r in bindings if 'InputTag.Ability.ResonanceLiberation' in r.get_editor_property('InputTag').export_text())
            state['skill_action'] = action
            place(); inject(True, action)
            step('R', now)
        elif phase == 'R':
            inject(False, state['skill_action'])
            if age > 7.:
                check('R automatically shares accepted-damage route', row['sent'] > state['E_sent'], row)
                check('Other local players received no attacker output', all(p.get_received_count() == 0 for p in others), row)
                state['stress_sent'], state['stress_received'] = row['sent'], row['received']
                state['stress_hp'] = hp
                for index in range(40):
                    assert lib.queue_number_pop_probe(authority, targets[index % 2], .25)
                step('stress', now)
        elif phase == 'stress' and age > 1.:
            check('Forty separate accepted GE contacts sent', row['sent'] - state['stress_sent'] == 40, row)
            check('Owner received forty separate events without duplicate host path', row['received'] - state['stress_received'] == 40, row)
            check('Fractional actual damage remains exact', sender.get_last_damage_number_sent().damage_amount == .25)
            check('Two targets each lost five HP', all(abs(a - b - 5.) < .01 for a, b in zip(state['stress_hp'], hp)), hp)
            check('Renderer exists only on owner', bool(pop.get_number_pop_niagara()) and all(not p.get_number_pop_niagara() for p in others))
            state['ai_sent'] = row['sent']
            assert lib.queue_number_pop_probe(targets[0], targets[1], 1.)
            step('ai', now)
        elif phase == 'ai' and age > .5:
            check('AI source never attributed to local player', row['sent'] == state['ai_sent'], row)
            check('Niagara reused one component', len(owner.get_controller().get_components_by_class(unreal.NiagaraComponent)) == 1)
            check('Received and submitted count agree', pop.get_received_count() == pop.get_rendered_count(), row)
            assert lib.queue_number_pop_probe(authority, targets[0], hp[0] - 10.)
            step('last_hit_prepare', now)
        elif phase == 'last_hit_prepare' and age > .5:
            check('Killing-blow fixture leaves exactly ten HP', health[0].get_health() == 10.)
            state['kill_received'] = row['received']
            assert lib.queue_number_pop_probe(authority, targets[0], 100.)
            step('last_hit', now)
        elif phase == 'last_hit' and age > .5:
            check('Overkill actual damage reaches owner as ten', pop.get_last_request().damage_amount == 10. and row['received'] == state['kill_received'] + 1, row)
            check('Killing blow target is dead without losing presentation', health[0].get_health() == 0. and bool(pop.get_number_pop_niagara()))
            if MODE == 'dedicated':
                server_pops = [c.get_number_pop_component() for c in unreal.GameplayStatics.get_all_actors_of_class(server, unreal.HodgePlayerController)]
                check('Dedicated server creates no renderer and receives no local events', all(not p.get_number_pop_niagara() and p.get_received_count() == 0 for p in server_pops))
            finish()
    except Exception:
        finish(traceback.format_exc())


if MODE == 'listen-lag':
    for world in worlds:
        unreal.SystemLibrary.execute_console_command(world, 'Net PktLag=100')
state['callback'] = unreal.register_slate_post_tick_callback(tick)
