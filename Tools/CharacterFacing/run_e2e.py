"""Real PIE owner/server/observer movement, animation resources, and request lifecycle."""
import unreal, builtins, json, traceback, time, math
from pathlib import Path

s = builtins.HODGE_REACTION_TEST
owner = s.get('source_owner', s['source']); server = s['source']
world = s['world']; lib = unreal.HodgeCombatValidationLibrary
pc = owner.get_controller()
sub = unreal.get_default_object(unreal.load_class(None, '/Script/Engine.SubsystemBlueprintLibrary')).call_method(
    'GetLocalPlayerSubSystemFromPlayerController', (pc, unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
move = unreal.load_asset('/Game/Main/Input/InputAction/IA_Move')
assert move
out = Path(unreal.Paths.project_saved_dir()) / 'FacingImplementation'
lag = s.get('lag_ms', 0); name = ('facing-network-' if s.get('network') else 'facing-single-') + str(lag) + 'ms'
roles = {'owner': owner, 'server': server}
if s.get('network'):
    other_world = next(w for w in s['client_worlds'] if w != owner.get_world())
    player_id = server.get_editor_property('PlayerState').get_editor_property('PlayerId')
    observer = next(p for p in unreal.GameplayStatics.get_all_actors_of_class(other_world, owner.get_class())
                    if p.get_editor_property('PlayerState') and p.get_editor_property('PlayerState').get_editor_property('PlayerId') == player_id)
    roles['observer'] = observer

cases = ['free_camera', 'free_side', 'strafe_side', 'strafe_back', 'release_mode', 'action_override', 'action_release', 'server_reject']
rows = []; reports = []; ctx = {'index': -1, 'stage': 'next', 'after': unreal.GameplayStatics.get_time_seconds(world) + .5, 'wall': time.monotonic()}
s['facing_context'] = ctx; s['facing_rows'] = rows; s['facing_reports'] = reports

def rotation(p): return p.get_component_by_class(unreal.HodgeCharacterRotationComponent)
def asc(p): return p.get_hodge_ability_system_component()
def yaw_error(a, b): return abs((a - b + 180) % 360 - 180)

def snapshot(p):
    state = rotation(p).get_resolved_state(); anim = p.mesh.get_anim_instance()
    return {'yaw': p.get_actor_rotation().yaw, 'driver': str(state.driver), 'style': str(state.style),
            'locked': state.yaw_locked, 'recovering': state.recovering_facing, 'version': state.state_version,
            'velocity': [p.character_movement.velocity.x, p.character_movement.velocity.y],
            'root_yaw': anim.get_editor_property('RootYawOffset'), 'strafe': anim.get_editor_property('bUseStrafeLocomotion'),
            'sequences': [str(x) for x in lib.inspect_facing_sequences(p)]}

def activate_hold():
    handle = next(h for h in asc(owner).get_all_abilities() if asc(owner).find_ability_definition(h) == s['hold'])
    assert lib.queue_ability_action(asc(owner), handle, False)
    return handle

def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    cleanup_error = None
    try:
        if ctx.get('hold_handle'): lib.queue_ability_action(asc(owner), ctx['hold_handle'], True)
        lib.queue_facing_mode(owner, unreal.HodgeCharacterFacingDriver.MOVEMENT, True)
        lib.set_facing_validation_controller_permission(server, True)
    except Exception: cleanup_error = traceback.format_exc()
    result = {'error': error or cleanup_error, 'lag_ms': lag, 'cases': reports, 'samples': rows}
    out.joinpath(name + '.json').write_text(json.dumps(result, indent=2), encoding='utf-8')
    s['facing_result'] = result

def validate(case, samples):
    final = samples[-1]
    for role, p in roles.items():
        record = final[role]
        if case in ['free_camera','free_side','release_mode','server_reject']:
            assert 'MOVEMENT' in record['driver'] and not record['strafe'], (case, role, record)
        if case in ['strafe_side','strafe_back','action_release']:
            assert 'CONTROLLER' in record['driver'] and record['strafe'], (case, role, record)
        if case == 'action_override':
            assert 'ACTION_DIRECTION' in record['driver'] and yaw_error(record['yaw'],90)<3, (case,role,record)
        if case == 'free_camera': assert yaw_error(record['yaw'],0)<2, (case,role,record)
        if case == 'free_side': assert any('_Jog_F' in x or '_Walk_F' in x for x in record['sequences']), (case,role,record)
        if case == 'strafe_side': assert any('_Jog_R' in x or '_Walk_R' in x for x in record['sequences']), (case,role,record)
        if case == 'strafe_back': assert any('_Jog_B' in x or '_Walk_B' in x for x in record['sequences']), (case,role,record)
    if len(roles)>2: assert yaw_error(final['server']['yaw'],final['observer']['yaw'])<5, final
    return {'case':case,'passed':True,'final':final}

def tick(delta):
    try:
        now = unreal.GameplayStatics.get_time_seconds(world)
        if time.monotonic()-ctx['wall']>180: raise RuntimeError('facing suite timeout')
        if ctx['stage']=='next':
            if now<ctx['after']:return
            ctx['index']+=1
            if ctx['index']==len(cases):finish();return
            case=cases[ctx['index']]
            if case not in ['action_override','action_release']:
                lib.queue_facing_mode(owner,unreal.HodgeCharacterFacingDriver.MOVEMENT,True)
                if ctx.get('hold_handle'):lib.queue_ability_action(asc(owner),ctx['hold_handle'],True);ctx['hold_handle']=None
            for p in [server,s['target']]:p.character_movement.stop_movement_immediately()
            server.set_actor_location(unreal.Vector(1000,1000,95),False,False)
            if s['target'] != server:s['target'].set_actor_location(unreal.Vector(2400,1000,95),False,False)
            server.set_actor_rotation(unreal.Rotator(0,0,0),False)
            owner.character_movement.stop_movement_immediately()
            owner.set_actor_location(unreal.Vector(1000,1000,95),False,False)
            owner.set_actor_rotation(unreal.Rotator(0,0,0),False)
            pc.set_control_rotation(unreal.Rotator(0,0,0))
            ctx.update({'stage':'prepare','after':now+.8,'case':case,'samples':[]})
            return
        if ctx['stage']=='prepare':
            if now<ctx['after']:return
            case=ctx['case']
            if case in ['strafe_side','strafe_back','action_override']:
                assert lib.queue_facing_mode(owner,unreal.HodgeCharacterFacingDriver.CONTROLLER)
            if case=='server_reject':
                assert lib.set_facing_validation_controller_permission(server,False)
                assert lib.queue_facing_mode(owner,unreal.HodgeCharacterFacingDriver.CONTROLLER)
            if case=='action_override':ctx['hold_handle']=activate_hold()
            if case=='action_release':assert lib.queue_action_facing(owner,unreal.Vector(0,1,0),True)
            ctx.update({'stage':'running','start':now,'action_requested':False,'last_sample':-1})
            return
        case=ctx['case'];t=now-ctx['start']
        if case=='free_camera':pc.set_control_rotation(unreal.Rotator(0,90,0))
        if case in ['free_side','strafe_side'] and .6<t<2.8:sub.inject_input_vector_for_action(move,unreal.Vector(1,0,0),[],[])
        if case=='strafe_back' and .6<t<2.8:sub.inject_input_vector_for_action(move,unreal.Vector(0,-1,0),[],[])
        if case=='action_override' and t>.7 and not ctx['action_requested']:
            assert lib.queue_action_facing(owner,unreal.Vector(0,1,0),False,False);ctx['action_requested']=True
        if t-ctx['last_sample']>=.04:
            row={'case':case,'t':t,**{role:snapshot(p) for role,p in roles.items()}}
            rows.append(row);ctx['samples'].append(row);ctx['last_sample']=t
        duration=2.4
        if t>=duration:
            reports.append(validate(case,ctx['samples']))
            ctx.update({'stage':'next','after':now+.3})
    except Exception:finish(traceback.format_exc())

callback=unreal.register_slate_post_tick_callback(tick)
s['suite_callback']=callback
print('Started '+name)
