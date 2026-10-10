"""Exercise actual Main Hero input, Dash variants, Sprint handoff and replication."""
import unreal, builtins, json, traceback, time, sys
from pathlib import Path

s = builtins.HODGE_REACTION_TEST
owner = s.get('source_owner', s['source']); server = s['source']; world = s['world']
lib = unreal.HodgeCombatValidationLibrary; pc = owner.get_controller()
bridge=s.get('lib') or unreal.get_default_object(unreal.load_class(None,'/Script/GameplayAbilities.AbilitySystemBlueprintLibrary'))
sub = unreal.get_default_object(unreal.load_class(None, '/Script/Engine.SubsystemBlueprintLibrary')).call_method(
    'GetLocalPlayerSubSystemFromPlayerController', (pc, unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
move = unreal.load_asset('/Game/Main/Input/InputAction/IA_Move')
sprint = unreal.load_asset('/Game/Main/Input/InputAction/IA_Sprint')
roles = {'owner':owner,'server':server}
if s.get('network'):
    other = next(w for w in s['client_worlds'] if w != owner.get_world())
    player_id = server.get_editor_property('PlayerState').get_editor_property('PlayerId')
    roles['observer'] = next(p for p in unreal.GameplayStatics.get_all_actors_of_class(other,owner.get_class())
        if p.get_editor_property('PlayerState') and p.get_editor_property('PlayerState').get_editor_property('PlayerId') == player_id)
sys.path.insert(0,str(Path(unreal.Paths.project_dir())/'Tools/DashSprint'))
import montage_listener
montage_listener.bind(list(roles.values()),s)

out = Path(unreal.Paths.project_saved_dir())/s.get('movement_output_dir','DashTimingRefinement')
name = ('network-' if s.get('network') else 'single-') + str(s.get('lag_ms',0)) + 'ms'
cases = ['forward_tap','backward_tap','side_tap','held_sprint','release_sprint','stop_sprint','move_cancel','delayed_hold','delayed_handoff','pivot_turn']
montages=[unreal.load_asset('/Game/Main/Character/Hero/Anim/Movement/AM_Hero_Dash_'+v) for v in ['F','B']]
profile=policy_profile=server.get_component_by_class(unreal.HodgeLocomotionPolicyComponent).get_profile()
original={k:profile.get_editor_property(k) for k in ['HoldThreshold','HandoffOpenTime','HandoffCloseTime']}
reports=[]; rows=[]
ctx={'index':-1,'stage':'next','after':unreal.GameplayStatics.get_time_seconds(world)+.6,'wall':time.monotonic()}

def policy(p): return p.get_component_by_class(unreal.HodgeLocomotionPolicyComponent)
def hero(p): return p.get_component_by_class(unreal.HodgeHeroComponent)
def snapshot(p):
    a=p.get_hodge_ability_system_component(); st=policy(p).get_resolved_policy()
    dash_committed=False; variant=None
    for handle in a.get_all_abilities():
        ability=bridge.call_method('GetGameplayAbilityFromSpecHandle',(a,handle))[0]
        if ability and isinstance(ability,unreal.HodgeGameplayAbility_Dash) and bridge.call_method('IsGameplayAbilityActive',(ability,)):
            dash_committed=ability.has_committed_movement(); variant=str(ability.get_dash_directions().variant)
    root=profile.get_editor_property('SprintPivotMontage')
    rm=lib.inspect_montage_state(p,root) if root else None
    pivots=([{'sequence':'A_Hero_Sprint_Pivot_RootMotion','time':rm.position,'weight':rm.weight}] if rm and rm.weight>.05 and rm.playing else []) if root else ([{'sequence':str(x.sequence),'time':x.time,'weight':x.weight} for x in lib.inspect_pivot_times(p)] if str(lib.inspect_montage_state(p,montages[0]).locomotion_state)=='Pivot' else [])
    return {'position':[p.get_actor_location().x,p.get_actor_location().y], 'yaw':p.get_actor_rotation().yaw,
        'velocity':[p.character_movement.velocity.x,p.character_movement.velocity.y],
        'sprinting':st.sprinting,'driver':str(p.get_component_by_class(unreal.HodgeCharacterRotationComponent).get_resolved_state().driver),
        'sequences':[str(x) for x in lib.inspect_facing_sequences(p)],
        'session':str(hero(p).get_sprint_input_session().state) if hero(p) else None,
        'held_age':unreal.GameplayStatics.get_time_seconds(p.get_world())-hero(p).get_sprint_input_session().pressed_at if hero(p) else 0.,
        'dash_committed':dash_committed,'dash_variant':variant,
        'montage_position':max(lib.inspect_montage_state(p,m).position for m in montages),
        'locomotion':str(lib.inspect_montage_state(p,montages[0]).locomotion_state),
        'pivot':pivots}

def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    montage_listener.unbind(s)
    for k,v in original.items(): profile.set_editor_property(k,v)
    for p in [owner]:
        try: hero(p).end_sprint_input()
        except Exception: pass
    result={'error':error,'cases':reports,'samples':rows,'lag_ms':s.get('lag_ms',0),'montage_end_events':s.get('montage_end_events',{})}
    out.joinpath(name+'.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
    s['movement_result']=result

def validate(case,samples):
    own=[r['owner'] for r in samples]; auth=[r['server'] for r in samples]
    assert samples,case
    if case=='forward_tap':
        assert max(r['position'][0] for r in auth)-auth[0]['position'][0]>150,(case,auth)
        assert any(r['dash_committed'] for r in own),(case,own)
    if case=='backward_tap':
        assert min(r['position'][0] for r in auth)<auth[0]['position'][0]-150,(case,auth)
        active=[r for r in own if r['dash_committed']]
        assert active and all(abs((r['yaw']+180)%360-180)<5 for r in active),(case,active)
    if case=='side_tap':
        assert max(r['position'][1] for r in auth)>auth[0]['position'][1]+150,(case,auth)
        assert any(abs(r['yaw']-90)<5 for r in own),(case,own)
    if case.endswith('tap'):
        expected_name='AM_Hero_Dash_B' if case=='backward_tap' else 'AM_Hero_Dash_F'
        for role,pawn in roles.items():
            events=[e for e in s['montage_end_events'].get(pawn.get_path_name(),[]) if e['time']>=ctx['start'] and e['montage']==expected_name]
            assert any(not e['interrupted'] for e in events),(case,role,'authored montage must end naturally',events)
        assert not any(r['sprinting'] for r in own),(case,'release cannot enter Sprint')
        assert any(r['dash_committed'] for r in auth),(case,'server must commit Dash')
        expected='BACKWARD' if case=='backward_tap' else 'FORWARD'
        assert any(r['dash_variant'] and expected in r['dash_variant'] for r in auth),(case,'authority variant')
    if case in ['held_sprint','release_sprint','stop_sprint']:
        assert any(r['sprinting'] for r in auth),(case,auth)
        assert any(any('A_Hero_Sprint_F' in seq for seq in r['sequences']) for r in own),(case,own)
        if case in ['release_sprint','stop_sprint']: assert not auth[-1]['sprinting'],(case,auth[-1])
    if case=='move_cancel':
        assert any(r['dash_committed'] for r in auth),(case,'must commit')
        last=max(r['t'] for r in samples if r['owner']['dash_committed'])
        assert profile.get_editor_property('MoveCancelOpenTime')<=last<=profile.get_editor_property('MoveCancelOpenTime')+.35,(case,'only configured recovery window can cancel',last)
        assert not any(r['sprinting'] for r in own),(case,'tap cannot hand off')
    if case in ['delayed_hold','delayed_handoff']:
        first=next((r for r in samples if r['owner']['sprinting']),None)
        assert first is not None,(case,'configured handoff must execute')
        frame=max((b['t']-a['t'] for a,b in zip(samples,samples[1:])),default=.033)
        if case=='delayed_hold':
            assert .83<=first['owner']['held_age']<=profile.get_editor_property('HoldThreshold')+3*frame+.05,(case,'actual input hold threshold',first)
        else:
            started=next(r['t']-r['owner']['montage_position'] for r in samples if r['owner']['dash_committed'])
            assert profile.get_editor_property('HandoffOpenTime')-.04<=first['t']-started<=profile.get_editor_property('HandoffOpenTime')+3*frame+.05,(case,'actual montage handoff threshold',first)
        assert any(r['sprinting'] for r in auth),(case,'authority must approve configured handoff')
    if case=='pivot_turn':
        pivot=[(r['t'], x['time']) for r in samples for x in r['owner']['pivot'] if x['sequence'] in ['A_Hero_Sprint_Turn','A_Hero_Sprint_Pivot_RootMotion']]
        assert len(pivot)>=3,(case,'must exercise actual Turn evaluator',pivot)
        progress=[(b[1]-a[1])/(b[0]-a[0]) for a,b in zip(pivot,pivot[1:]) if b[0]>a[0] and b[1]>=a[1] and b[1]<1.59]
        assert progress and any(.65<speed<1.35 for speed in progress),(case,'Turn must advance at normal speed',progress)
        expected=profile.get_editor_property('PivotRecoveryEndTime') if profile.get_editor_property('SprintPivotMontage') else profile.get_editor_property('SprintTurn').get_play_length()
        assert max(t for _,t in pivot)>expected-.15,(case,'Turn must reach its configured recovery point',pivot)
        assert not any(x['sequence'] in ['A_Hero_Sprint_Turn','A_Hero_Sprint_Pivot_RootMotion'] for r in samples if 3.2<r['t']<3.45 for x in r['owner']['pivot']),(case,'completed Turn must exit into Cycle before Shift release')
    if 'observer' in roles and case=='held_sprint': assert any(r['observer']['sprinting'] for r in samples),(case,samples)
    return {'case':case,'passed':True,'final':samples[-1]}

def tick(delta):
    try:
        now=unreal.GameplayStatics.get_time_seconds(world)
        if time.monotonic()-ctx['wall']>180: raise RuntimeError('movement suite timeout')
        if ctx['stage']=='next':
            if now<ctx['after']:return
            ctx['index']+=1
            if ctx['index']==len(cases):finish();return
            case=cases[ctx['index']]
            for k,v in original.items():profile.set_editor_property(k,v)
            if case=='delayed_hold':profile.set_editor_property('HoldThreshold',.85)
            if case=='delayed_handoff':profile.set_editor_property('HandoffOpenTime',.85)
            for p in [owner,server]:
                p.character_movement.stop_movement_immediately()
                p.set_actor_location(unreal.Vector(1000,1000,95),False,False)
                p.set_actor_rotation(unreal.Rotator(0,0,0),False)
            pc.set_control_rotation(unreal.Rotator(0,0,0))
            assert lib.queue_facing_mode(owner,unreal.HodgeCharacterFacingDriver.CONTROLLER)
            ctx.update({'case':case,'stage':'prepare','after':now+1.2,'samples':[],'last':-1})
            return
        if ctx['stage']=='prepare':
            if now<ctx['after']:return
            assert abs((owner.get_actor_rotation().yaw+180)%360-180)<5,'fixture owner facing must settle'
            assert abs((server.get_actor_rotation().yaw+180)%360-180)<5,'fixture server facing must settle'
            assert lib.queue_facing_mode(owner,unreal.HodgeCharacterFacingDriver.MOVEMENT,True)
            ctx.update({'stage':'free','after':now+.25})
            return
        if ctx['stage']=='free':
            if now<ctx['after']:return
            ctx.update({'stage':'run','start':now,'pressed':False})
            case=ctx['case']
            first_move=unreal.Vector(0,-1,0) if case=='backward_tap' else unreal.Vector(1,0,0) if case=='side_tap' else unreal.Vector(0,1,0)
            sub.inject_input_vector_for_action(move,first_move,[],[])
            sub.inject_input_vector_for_action(sprint,unreal.Vector(1,0,0),[],[])
            return
        case=ctx['case']; age=now-ctx['start']
        vector=unreal.Vector(0,-1,0) if case=='backward_tap' else unreal.Vector(1,0,0) if case=='side_tap' else unreal.Vector(0,1,0)
        moving = age<(.9 if case=='stop_sprint' else .10 if case.endswith('tap') else 3.6)
        if case=='pivot_turn' and age>=1.25:vector=unreal.Vector(0,-1,0)
        if moving: sub.inject_input_vector_for_action(move,vector,[],[])
        held= age<(.12 if case.endswith('tap') or case=='move_cancel' else .9 if case=='release_sprint' else 3.5)
        if held: sub.inject_input_vector_for_action(sprint,unreal.Vector(1,0,0),[],[])
        if age-ctx['last']>=.025:
            row={'case':case,'t':age,**{role:snapshot(p) for role,p in roles.items()}}
            rows.append(row);ctx['samples'].append(row);ctx['last']=age
        if age>=4.0:
            reports.append(validate(case,ctx['samples']));ctx.update({'stage':'next','after':now+.4})
    except Exception:finish(traceback.format_exc())
callback=unreal.register_slate_post_tick_callback(tick)
s['suite_callback']=callback
print('Started movement '+name)
