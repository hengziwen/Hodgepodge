"""Exercise Main attack chain interruption and Death veto through actual Enhanced Input."""
import unreal, builtins, json, time, traceback, re
from pathlib import Path

s = builtins.HODGE_REACTION_TEST
world = s['world']; owner = s.get('source_owner', s['source']); server = s['source']
bridge = s.get('lib') or unreal.get_default_object(unreal.load_class(None, '/Script/GameplayAbilities.AbilitySystemBlueprintLibrary'))
lib = unreal.HodgeCombatValidationLibrary
sub = unreal.get_default_object(unreal.load_class(None, '/Script/Engine.SubsystemBlueprintLibrary')).call_method(
    'GetLocalPlayerSubSystemFromPlayerController', (owner.get_controller(), unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
attack_input = unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack')
dash_input = unreal.load_asset('/Game/Main/Input/InputAction/IA_Sprint')
roles = {'owner': owner, 'server': server}
if s.get('network'):
    other = next(w for w in s['client_worlds'] if w != owner.get_world())
    player_id = server.get_editor_property('PlayerState').get_editor_property('PlayerId')
    roles['observer'] = next(p for p in unreal.GameplayStatics.get_all_actors_of_class(other, owner.get_class())
        if p.get_editor_property('PlayerState') and p.get_editor_property('PlayerState').get_editor_property('PlayerId') == player_id)
attacks = [unreal.load_asset('/Game/Main/Character/Hero/Anim/Montages/AM_Attack0' + str(i) + '_Montage') for i in range(1, 6)]
dash_montage = unreal.load_asset('/Game/Main/Character/Hero/Anim/Movement/AM_Hero_Dash_F')
combo = unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData').get_editor_property('ComboDefinition')
combo_rows = json.loads(unreal.DataTableFunctionLibrary.export_data_table_to_json_string(combo.get_editor_property('ComboTable')))
advance_windows = {}
for row in combo_rows:
    match = re.search(r'Ability.Attack.Light.0(\d)', row['AbilityTag'])
    if match and row['Transitions']:
        text = row['Transitions'][0].split('RequiredWindowTags=', 1)[1].split(',bAllowAfterExecutionEnded=', 1)[0]
        advance_windows[int(match.group(1))] = re.findall(r'TagName="([^"]+)"', text)
cases = ['early_attack_' + str(i) for i in range(1, 6)] + ['late_attack', 'death_state', 'death_dying', 'death_dead', 'death_ability']
ctx = {'index': -1, 'stage': 'next', 'after': unreal.GameplayStatics.get_time_seconds(world) + .5, 'wall': time.monotonic()}
rows = []; reports = []; death_tags = None
def asc(p): return p.get_hodge_ability_system_component()
def active(p):
    result = []
    if not asc(p): return result
    for handle in asc(p).get_all_abilities():
        ability = bridge.call_method('GetGameplayAbilityFromSpecHandle', (asc(p), handle))[0]
        if ability and bridge.call_method('IsGameplayAbilityActive', (ability,)): result.append(ability)
    return result
def attack_index(p):
    for a in active(p):
        if isinstance(a, unreal.HodgeGameplayAbility_Definition) and a.get_definition():
            name = a.get_definition().get_name()
            if name.startswith('DA_Attack_'): return int(name.rsplit('_', 1)[1])
    return 0
def snapshot(p):
    if not asc(p): return {'attack': 0, 'dash': False, 'death': False, 'attack_weight': 0, 'dash_weight': 0, 'poses': 0, 'hits': 0, 'pawn_removed': True}
    abilities = active(p)
    dash_state = lib.inspect_montage_state(p, dash_montage)
    return {'attack': attack_index(p), 'dash': any(isinstance(a, unreal.HodgeGameplayAbility_Dash) and a.has_committed_movement() for a in abilities),
        'death': any(isinstance(a, unreal.HodgeGameplayAbility_Death) for a in abilities),
        'attack_weight': max(lib.inspect_montage_state(p, m).weight for m in attacks), 'dash_weight': dash_state.weight,
        'poses': lib.inspect_pose_leases(p), 'hits': lib.inspect_hit_sessions(p)}
def set_death(value):
    if death_tags:
        for p in set([owner, server]):
            bridge.call_method('AddLooseGameplayTags' if value else 'RemoveLooseGameplayTags', (p, death_tags, False))
def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    set_death(False)
    result = {'error': error, 'cases': reports, 'samples': rows, 'network': s.get('network', False), 'lag_ms': s.get('lag_ms', 0)}
    name = ('network-' if s.get('network') else 'single-') + str(s.get('lag_ms', 0)) + 'ms-interrupt.json'
    Path(unreal.Paths.project_saved_dir()).joinpath('DashInterrupt', name).write_text(json.dumps(result, indent=2), encoding='utf-8')
    s['interrupt_result'] = result
    hero = owner.get_component_by_class(unreal.HodgeHeroComponent)
    if hero: hero.end_sprint_input()
def inject(action): sub.inject_input_vector_for_action(action, unreal.Vector(1, 0, 0), [], [])
def tick(delta):
    global death_tags
    try:
        now = unreal.GameplayStatics.get_time_seconds(world)
        if time.monotonic() - ctx['wall'] > 180: raise RuntimeError('interrupt integration timeout')
        if ctx['stage'] == 'next':
            if now < ctx['after']: return
            set_death(False); death_tags = None
            ctx['index'] += 1
            if ctx['index'] == len(cases): finish(); return
            case = cases[ctx['index']]
            owner.get_component_by_class(unreal.HodgeHeroComponent).end_sprint_input()
            for p in set([owner, server]):
                p.character_movement.stop_movement_immediately(); p.set_actor_location(unreal.Vector(1000, 1000, 95), False, False)
            if s['target'] != server: s['target'].set_actor_location(unreal.Vector(2400, 1000, 95), False, False)
            ctx.update({'case': case, 'stage': 'prepare', 'after': now + 1.3, 'samples': [], 'pressed': False,
                'last': -1, 'last_attack_press': -1, 'advanced_instances': set(), 'target_seen': False, 'outside_window': False})
            return
        if ctx['stage'] == 'prepare':
            if now < ctx['after']: return
            case = ctx['case']; ctx.update({'stage': 'run', 'start': now})
            if case.startswith('death_') and case != 'death_ability':
                tag = {'death_state': 'Status.Death', 'death_dying': 'Status.Death.Dying', 'death_dead': 'Status.Death.Dead'}[case]
                death_tags = unreal.GameplayTagContainer(); death_tags.import_text('(GameplayTags=((TagName="' + tag + '")))'); set_death(True)
            elif case == 'death_ability':
                tag = unreal.GameplayTag(); tag.import_text('(TagName="GameplayEvent.Death")')
                assert lib.queue_gameplay_event(asc(server), tag)
            else: inject(attack_input); ctx['last_attack_press'] = now
            return
        case = ctx['case']; age = now - ctx['start']
        if case.startswith('early_attack_') or case == 'late_attack':
            wanted = int(case.rsplit('_', 1)[1]) if case.startswith('early_') else 1
            oi = attack_index(owner); si = attack_index(server)
            if oi == wanted and si == wanted and not ctx['pressed']:
                a = next(a for a in active(owner) if isinstance(a, unreal.HodgeGameplayAbility_Definition))
                window = owner.get_component_by_class(unreal.HodgeLocomotionPolicyComponent).get_profile().get_editor_property('AttackCancelWindow').export_text()
                tag = window.split('"')[1] if '"' in window else ''
                in_window = bool(tag and tag in lib.inspect_ability_windows(a).export_text())
                if case != 'late_attack' or in_window:
                    assert case == 'late_attack' or not in_window, 'early cancellation must precede old window'
                    ctx['target_seen'] = True; ctx['outside_window'] = not in_window
                    inject(dash_input); ctx['pressed'] = True; ctx['dash_at'] = now
            elif oi != wanted and not ctx['pressed']:
                if oi:
                    a = next(a for a in active(owner) if isinstance(a, unreal.HodgeGameplayAbility_Definition))
                    window_text = lib.inspect_ability_windows(a).export_text()
                    instance = lib.inspect_montage_state(owner, attacks[oi - 1]).instance_id
                    if instance not in ctx['advanced_instances'] and all(tag in window_text for tag in advance_windows.get(oi, [])):
                        inject(attack_input); ctx['advanced_instances'].add(instance); ctx['last_attack_press'] = now
                elif now - ctx['last_attack_press'] > .4:
                    inject(attack_input); ctx['last_attack_press'] = now
            if not ctx['pressed'] and age > 8: raise AssertionError(('failed to reach intended attack', wanted, oi, si))
        elif ((.025 < age < .125 or .3 < age < .4) if case == 'death_ability' else .25 < age < .35): inject(dash_input)
        if age - ctx['last'] >= .025:
            row = {'case': case, 't': age, **{role: snapshot(p) for role, p in roles.items()}}
            rows.append(row); ctx['samples'].append(row); ctx['last'] = age
        done = (ctx['pressed'] and now - ctx['dash_at'] > 2) if not case.startswith('death_') else age > 1
        if done:
            samples = ctx['samples']
            if case.startswith('death_'):
                assert not any(r['server']['dash'] for r in samples), 'authority Death must veto Dash'
                if case == 'death_ability':
                    assert any(r['server']['death'] for r in samples), 'actual Death ability must remain active'
                    assert any(r['owner']['death'] for r in samples), 'owner must receive actual Death activation'
                    assert not any(r['owner']['dash'] and r['owner']['death'] for r in samples), 'known Death must cancel and veto owner prediction'
                    assert not samples[-1]['owner']['dash'], 'early unknown-state prediction must settle to death'
                else:
                    assert not any(r['owner']['dash'] for r in samples), 'known owner Death state must veto Dash'
            else:
                assert ctx['target_seen'] and any(r['server']['dash'] for r in samples), 'mapped cancellation must commit on server'
                assert any(r['owner']['dash'] for r in samples), 'owner prediction must commit'
                assert samples[-1]['server']['attack'] == 0 and samples[-1]['server']['poses'] == 0 and samples[-1]['server']['hits'] == 0, 'attack must release resources'
                if 'observer' in roles: assert any(r['observer']['dash_weight'] > .1 for r in samples), 'observer must see Dash montage'
            reports.append({'case': case, 'passed': True, 'outside_old_window': ctx['outside_window'],
                'prediction_before_death_received': any(r['owner']['dash'] and not r['owner']['death'] for r in samples) if case == 'death_ability' else False})
            ctx.update({'stage': 'next', 'after': now + 1.2})
    except Exception: finish(traceback.format_exc())
callback = unreal.register_slate_post_tick_callback(tick); s['suite_callback'] = callback
s['interrupt_progress'] = {'context': ctx, 'reports': reports}
print('Started Main Hero tag relationship interruption integration')
