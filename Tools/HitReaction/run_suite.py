"""Sample real animation-backed reactions while PIE advances; assertions run after settling."""
import unreal, builtins, json, traceback, time, math
from pathlib import Path

s = builtins.HODGE_REACTION_TEST
world = s['world']
source = s['source']
target = s['target']
lib = s['lib']
out = Path('E:/Project/Git/Hodgepodge/Saved/HitReactionPIE')
out.mkdir(parents=True, exist_ok=True)
network = s.get('network', False)
output_name = s.get('output_name', 'single')
s.pop('last_report', None)

def asc(pawn): return pawn.get_hodge_ability_system_component()
def react(pawn): return pawn.get_component_by_class(unreal.HodgeHitReactionComponent)
def hp(pawn): return pawn.get_component_by_class(unreal.HodgeHealthComponent).get_health()
def handle(pawn, definition):
    values = [h for h in asc(pawn).get_all_abilities() if asc(pawn).find_ability_definition(h) == definition]
    assert len(values) == 1, (pawn.get_path_name(), definition.get_name(), len(values))
    return values[0]
def ga(pawn, definition): return lib.call_method('GetGameplayAbilityFromSpecHandle', (asc(pawn), handle(pawn, definition)))[0]
def active(ability): return bool(ability and lib.call_method('IsGameplayAbilityActive', (ability,)))
def clock(): return unreal.GameplayStatics.get_time_seconds(world)
def tag(name):
    t = unreal.GameplayTag(); t.import_text('(TagName="' + name + '")'); return t

cases = [('EqualSkill', 'Skill', 'Skill', unreal.HodgeImpactType.HIT_STUN),
         ('LowerAgainstArmor', 'Skill', 'SuperArmor', unreal.HodgeImpactType.HIT_STUN),
         ('EqualVajra', 'Vajra', 'Vajra', unreal.HodgeImpactType.HIT_STUN),
         ('Knockback', 'Skill', 'Normal', unreal.HodgeImpactType.KNOCKBACK),
         ('Launch', 'Skill', 'Skill', unreal.HodgeImpactType.LAUNCH),
         ('Knockdown', 'Skill', 'Normal', unreal.HodgeImpactType.KNOCKDOWN),
         ('CancelLaunch', 'Skill', 'Normal', unreal.HodgeImpactType.LAUNCH)]
if s.get('extended'):
    cases = [('AirHit', 'Skill', 'Normal', unreal.HodgeImpactType.LAUNCH),
             ('Slam', 'Skill', 'Normal', unreal.HodgeImpactType.LAUNCH),
             ('WallKnockback', 'Skill', 'Normal', unreal.HodgeImpactType.KNOCKBACK),
             ('CeilingLaunch', 'Skill', 'Normal', unreal.HodgeImpactType.LAUNCH)]
state = {'index': -1, 'stage': 'idle', 'after': clock() + 0.8, 'start_wall': time.monotonic()}
rows = []; reports = []
owned_target = s.get('owned_target', target)
observer_target = s.get('observer_target', target)
source_owner = s.get('source_owner', source)
light_montage = s['profile'].get_editor_property('LightFeedbackMontage')
hold_montage = s['hold'].get_editor_property('ExecutionConfig').get_editor_property('Montage')
hold_sequence = unreal.load_asset('/Game/CodexText/HitReactionValidation/A_Hold') if s.get('use_formal_profile') else None

def snapshot(pawn):
    r = react(pawn); st = r.get_reaction_state()
    pos = pawn.get_actor_location(); vel = pawn.character_movement.velocity
    m = st.montage
    ai = pawn.mesh.get_anim_instance()
    bone_deviation = {}
    if hold_sequence and ai and ai.montage_is_playing(hold_montage) and light_montage and ai.montage_is_playing(light_montage):
        position = ai.montage_get_position(hold_montage)
        for bone in ['Bip001Spine2', 'Bip001Pelvis']:
            actual = pawn.mesh.get_socket_transform(bone, unreal.RelativeTransformSpace.RTS_PARENT_BONE_SPACE).rotation
            expected = unreal.AnimationLibrary.get_bone_pose_for_time(hold_sequence, bone, position, False).rotation
            dot = abs(actual.x*expected.x + actual.y*expected.y + actual.z*expected.z + actual.w*expected.w)
            bone_deviation[bone] = math.degrees(2*math.acos(min(1.0, dot)))
    return {'role': str(pawn.get_local_role()), 'phase': str(st.phase), 'sequence': st.sequence,
            'type': str(st.type), 'controlled': r.is_controlled(), 'health': hp(pawn),
            'position': [pos.x, pos.y, pos.z], 'velocity': [vel.x, vel.y, vel.z],
            'montage': m.get_name() if m else None,
            'montage_position': ai.montage_get_position(m) if ai and m else None,
            'montage_playing': bool(ai and m and ai.montage_is_playing(m)), 'air_hit_count': st.air_hit_count,
            'light_playing': bool(ai and light_montage and ai.montage_is_playing(light_montage)),
            'hold_playing': bool(ai and hold_montage and ai.montage_is_playing(hold_montage)), 'light_bone_deviation': bone_deviation}

def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    result = {'error': error, 'network': network, 'packet_lag_ms': s.get('lag_ms', 0),
              'world': world.get_path_name(), 'cases': reports, 'samples': rows}
    (out / (output_name + '.json')).write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
    s['last_report'] = result

def cancel_all():
    for pawn in [source, target]:
        for d in [s['definition'], s['hold']]:
            if active(ga(pawn, d)): unreal.HodgeCombatValidationLibrary.queue_ability_action(asc(pawn), handle(pawn, d), True)
        if react(pawn).is_controlled():
            found = [h for h in asc(pawn).get_all_abilities() if lib.call_method('GetGameplayAbilityFromSpecHandle', (asc(pawn), h))[0] and
                     isinstance(lib.call_method('GetGameplayAbilityFromSpecHandle', (asc(pawn), h))[0], unreal.HodgeGameplayAbility_HitReaction)]
            assert len(found) == 1, len(found)
            unreal.HodgeCombatValidationLibrary.queue_ability_action(asc(pawn), found[0], True)

def prepare(index):
    name, judgement, body, typ = cases[index]
    cancel_all()
    if s.get('obstacle') and unreal.SystemLibrary.is_valid(s['obstacle']): s['obstacle'].destroy_actor()
    s['obstacle'] = None
    if name in ['WallKnockback', 'CeilingLaunch']:
        pos = unreal.Vector(1320, 1000, 200) if name == 'WallKnockback' else unreal.Vector(1200, 1000, 350)
        obstacle = unreal.HodgeCombatValidationLibrary.spawn_hit_reaction_validation_actor(world, unreal.StaticMeshActor, pos)
        assert obstacle
        obstacle.static_mesh_component.set_mobility(unreal.ComponentMobility.MOVABLE)
        obstacle.static_mesh_component.set_static_mesh(unreal.load_asset('/Engine/BasicShapes/Cube'))
        obstacle.set_actor_scale3d(unreal.Vector(.2, 5, 5) if name == 'WallKnockback' else unreal.Vector(5, 5, .2))
        obstacle.static_mesh_component.set_collision_enabled(unreal.CollisionEnabled.QUERY_AND_PHYSICS)
        obstacle.static_mesh_component.set_collision_response_to_all_channels(unreal.CollisionResponseType.ECR_BLOCK)
        s['obstacle'] = obstacle
    r = s['definition'].get_editor_property('DefaultHitConfig')
    cfg = r.get_editor_property('Reaction')
    cfg.set_editor_property('AttackJudgementTag', tag('Combat.Attack.Judgement.' + judgement))
    impact = unreal.HodgeImpactSpec(); impact.set_editor_property('Type', typ)
    impact.set_editor_property('ControlDuration', 0.9)
    impact.set_editor_property('HorizontalSpeed', 0.0)
    impact.set_editor_property('VerticalSpeed', 700.0)
    impact.set_editor_property('KnockbackDistance', 150.0)
    impact.set_editor_property('MoveDuration', 0.3)
    cfg.set_editor_property('Impacts', [impact]); r.set_editor_property('Reaction', cfg)
    s['definition'].set_editor_property('DefaultHitConfig', r)
    s['hold'].set_editor_property('ExecutionBodyTag', tag('State.Combat.Body.' + body))
    state.update({'stage': 'reset', 'name': name, 'type': typ, 'after': clock() + .4,
                  'cancelled': False, 'result_seen': False, 'seen_control': False, 'seen_air': False,
                  'seen_down': False, 'seen_getup': False, 'seen_montage': False, 'seen_owner_control': False,
                  'seen_observer_control': False, 'seen_owner_montage': False, 'seen_observer_montage': False,
                  'max_z': 0.0, 'max_x_delta': 0.0, 'max_owner_z': 0.0, 'max_observer_z': 0.0,
                  'max_owner_x_delta': 0.0, 'max_observer_x_delta': 0.0, 'chain_requested': False,
                  'chain_queued': False, 'seen_chain_type': False, 'max_air_hits': 0, 'first_control_at': None,
                  'seen_light_overlap': False, 'seen_owner_light_overlap': False, 'seen_observer_light_overlap': False})

def tick(delta):
    try:
        if time.monotonic() - state['start_wall'] > 180: raise RuntimeError('PIE suite wall-clock timeout')
        now = clock()
        if state['stage'] == 'idle':
            if now < state['after']: return
            state['index'] += 1
            if state['index'] == len(cases): finish(); return
            prepare(state['index']); return
        if state['stage'] == 'reset':
            if now < state['after']: return
            for i, pawn in enumerate([source, target]):
                pawn.character_movement.stop_movement_immediately()
                pawn.set_actor_location(unreal.Vector(1000 + i*200, 1000, 95), False, False)
                pawn.set_actor_rotation(unreal.Rotator(0, 0, 0), False)
                pawn.character_movement.set_movement_mode(unreal.MovementMode.MOVE_WALKING)
            state.update({'stage': 'hold_wait', 'start': now, 'initial_health': hp(target),
                          'initial_id': str(react(target).get_last_result().hit_id),
                          'feedback_before': s['feedback'].get(target.get_path_name(), 0),
                          'owner_feedback_before': s['feedback'].get(owned_target.get_path_name(), 0),
                          'observer_feedback_before': s['feedback'].get(observer_target.get_path_name(), 0)})
            assert unreal.HodgeCombatValidationLibrary.queue_ability_action(asc(target), handle(target, s['hold']), False)
            return
        if state['stage'] == 'hold_wait':
            assert now - state['start'] < 4, 'Target hold did not activate'
            if not active(ga(target, s['hold'])): return
            state.update({'stage': 'running', 'start': now, 'start_pos': target.get_actor_location(), 'last_sample': -1.0})
            assert unreal.HodgeCombatValidationLibrary.queue_ability_action(asc(source_owner), handle(source_owner, s['definition']), False)
            return
        t = now - state['start']
        if t - state['last_sample'] < .025: return
        state['last_sample'] = t
        a = snapshot(target); o = snapshot(owned_target); v = snapshot(observer_target)
        r = react(target).get_last_result()
        state['result_seen'] |= str(r.hit_id) != state['initial_id']
        state['seen_control'] |= a['controlled']; state['seen_air'] |= 'AIRBORNE' in a['phase']
        state['seen_down'] |= 'DOWNED' in a['phase']; state['seen_getup'] |= 'GETTING_UP' in a['phase']
        state['seen_montage'] |= a['montage_playing']
        for key, data in [('seen_light_overlap', a), ('seen_owner_light_overlap', o), ('seen_observer_light_overlap', v)]:
            state[key] |= data['light_playing'] and data['hold_playing']
        if a['controlled'] and state['first_control_at'] is None: state['first_control_at'] = now
        state['seen_owner_control'] |= o['controlled']; state['seen_observer_control'] |= v['controlled']
        state['seen_owner_montage'] |= o['montage_playing']; state['seen_observer_montage'] |= v['montage_playing']
        state['max_z'] = max(state['max_z'], a['position'][2] - state['start_pos'].z)
        state['max_x_delta'] = max(state['max_x_delta'], abs(a['position'][0] - state['start_pos'].x))
        state['max_air_hits'] = max(state['max_air_hits'], a['air_hit_count'])
        if state['name'] in ['AirHit', 'Slam']:
            chain_type = unreal.HodgeImpactType.AIR_HIT if state['name'] == 'AirHit' else unreal.HodgeImpactType.SLAM
            state['seen_chain_type'] |= a['type'] == str(chain_type)
            if a['controlled'] and t > .5 and not state['chain_requested']:
                unreal.HodgeCombatValidationLibrary.queue_ability_action(asc(source), handle(source, s['definition']), True)
                config = s['definition'].get_editor_property('DefaultHitConfig')
                request = config.get_editor_property('Reaction')
                impact = request.get_editor_property('Impacts')[0]
                impact.set_editor_property('Type', chain_type)
                request.set_editor_property('Impacts', [impact]); config.set_editor_property('Reaction', request)
                s['definition'].set_editor_property('DefaultHitConfig', config)
                state['chain_requested'] = True
                state['chain_after'] = now + .06
            elif state['chain_requested'] and not state['chain_queued'] and now >= state['chain_after']:
                assert not active(ga(source, s['definition']))
                assert unreal.HodgeCombatValidationLibrary.queue_ability_action(asc(source), handle(source, s['definition']), False)
                state['chain_queued'] = True
        for label, data in [('owner', o), ('observer', v)]:
            state['max_' + label + '_z'] = max(state['max_' + label + '_z'], data['position'][2] - state['start_pos'].z)
            state['max_' + label + '_x_delta'] = max(state['max_' + label + '_x_delta'], abs(data['position'][0] - state['start_pos'].x))
        rows.append({'case': state['name'], 't': t, 'authority': a, 'owner': o, 'observer': v, 'outcome': str(r.outcome)})
        if state['name'] == 'CancelLaunch' and a['controlled'] and now - state['first_control_at'] > .35 and not state['cancelled']:
            cancel_all(); state['cancelled'] = True
        expected_low = state['name'] == 'LowerAgainstArmor'
        finish_time = 2.4 if expected_low or state['type'] == unreal.HodgeImpactType.HIT_STUN else 4.6
        if state['name'] in ['AirHit', 'Slam']: finish_time = 6.0
        if t < finish_time: return
        report = {k: state[k] for k in ['name', 'result_seen', 'seen_control', 'seen_air', 'seen_down', 'seen_getup',
                   'seen_montage', 'seen_owner_control', 'seen_observer_control', 'seen_owner_montage', 'seen_observer_montage',
                   'max_z', 'max_x_delta', 'max_owner_z', 'max_observer_z', 'max_owner_x_delta', 'max_observer_x_delta',
                   'cancelled', 'chain_queued', 'seen_chain_type', 'max_air_hits', 'seen_light_overlap',
                   'seen_owner_light_overlap', 'seen_observer_light_overlap']}
        report.update({'outcome': str(r.outcome), 'health_before': state['initial_health'], 'health_after': hp(target),
                       'owner_health': hp(owned_target), 'observer_health': hp(observer_target),
                       'target_hold_active': active(ga(target, s['hold'])), 'controlled_after': react(target).is_controlled(),
                       'owner_controlled_after': react(owned_target).is_controlled(), 'observer_controlled_after': react(observer_target).is_controlled(),
                       'light_feedback_events': s['feedback'].get(target.get_path_name(), 0) - state['feedback_before'],
                       'owner_light_feedback_events': s['feedback'].get(owned_target.get_path_name(), 0) - state['owner_feedback_before'],
                       'observer_light_feedback_events': s['feedback'].get(observer_target.get_path_name(), 0) - state['observer_feedback_before'],
                       'owner_final_position_error': sum((a['position'][i]-o['position'][i])**2 for i in range(3))**.5,
                       'observer_final_position_error': sum((a['position'][i]-v['position'][i])**2 for i in range(3))**.5})
        reports.append(report)
        assert state['result_seen'] and hp(target) < state['initial_health'], report
        if expected_low:
            assert r.outcome == unreal.HodgeHitReactionOutcome.LOW_JUDGEMENT and not state['seen_control'], report
            assert report['target_hold_active'] and report['light_feedback_events'] > 0, report
            if light_montage:
                assert state['seen_light_overlap'] and a['hold_playing'], report
            if network:
                assert report['owner_light_feedback_events'] > 0 and report['observer_light_feedback_events'] > 0, report
                if light_montage:
                    assert state['seen_owner_light_overlap'] and state['seen_observer_light_overlap'], report
        else:
            assert r.outcome == unreal.HodgeHitReactionOutcome.APPLIED and state['seen_control'] and state['seen_montage'], report
            assert not report['target_hold_active'] and not report['controlled_after'], report
            if state['type'] == unreal.HodgeImpactType.KNOCKBACK and state['name'] != 'WallKnockback': assert state['max_x_delta'] > 70, report
            if state['type'] == unreal.HodgeImpactType.LAUNCH and state['name'] != 'CancelLaunch': assert state['max_z'] > 70 and state['seen_air'], report
            if state['type'] == unreal.HodgeImpactType.KNOCKDOWN: assert state['seen_down'] and state['seen_getup'], report
            if state['name'] == 'WallKnockback': assert 10 < state['max_x_delta'] < 85, report
            if state['name'] == 'CeilingLaunch': assert 70 < state['max_z'] < 165, report
            if state['name'] in ['AirHit', 'Slam']:
                assert state['chain_queued'] and state['seen_chain_type'] and hp(target) <= state['initial_health'] - 10, report
                if state['name'] == 'AirHit': assert state['max_air_hits'] == 1, report
                else: assert state['seen_down'] and state['seen_getup'], report
            if network:
                assert state['seen_owner_control'] and state['seen_observer_control'], report
                assert state['seen_owner_montage'] and state['seen_observer_montage'], report
                assert not report['owner_controlled_after'] and not report['observer_controlled_after'], report
                assert abs(hp(target)-hp(owned_target)) < .01 and abs(hp(target)-hp(observer_target)) < .01, report
                assert report['owner_final_position_error'] < 5 and report['observer_final_position_error'] < 5, report
                if state['type'] == unreal.HodgeImpactType.KNOCKBACK and state['name'] != 'WallKnockback':
                    assert state['max_owner_x_delta'] > 70 and state['max_observer_x_delta'] > 70, report
                if state['type'] == unreal.HodgeImpactType.LAUNCH and state['name'] != 'CancelLaunch':
                    assert state['max_owner_z'] > 70 and state['max_observer_z'] > 70, report
        state.update({'stage': 'idle', 'after': now + .35})
    except Exception:
        finish(traceback.format_exc())

callback = unreal.register_slate_post_tick_callback(tick)
s['suite_callback'] = callback
print('Started ' + str(len(cases)) + ' animation-backed reaction cases: ' + output_name)
