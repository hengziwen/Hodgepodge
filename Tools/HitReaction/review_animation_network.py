"""Measure real owner/server/observer montage weights and movement across formal transitions."""
import unreal, builtins, json, traceback, time, math
from pathlib import Path
s = builtins.HODGE_REACTION_TEST
w = s['world']; owner = s['source_owner']; server = s['source']
pc = owner.get_controller()
other_world = next(world for world in s['client_worlds'] if world != owner.get_world())
pid = server.get_editor_property('PlayerState').get_editor_property('PlayerId')
observer = next(p for p in unreal.GameplayStatics.get_all_actors_of_class(other_world, owner.get_class())
    if p.get_editor_property('PlayerState') and p.get_editor_property('PlayerState').get_editor_property('PlayerId') == pid)
lib = s['lib']; inspect = unreal.HodgeCombatValidationLibrary.inspect_montage_state
sub = unreal.get_default_object(unreal.load_class(None, '/Script/Engine.SubsystemBlueprintLibrary')).call_method(
    'GetLocalPlayerSubSystemFromPlayerController', (pc, unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
attack = unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack')
move = unreal.load_asset('/Game/Main/Input/InputAction/IA_Move')
montages = [unreal.load_asset('/Game/Main/Character/Hero/Anim/Montages/AM_Attack0'+str(i)+'_Montage') for i in range(1,6)]
hit_montage = unreal.load_asset('/Game/Main/Character/Hero/Anim/HitReactions/AM_Hero_HitStun')
out = Path(unreal.Paths.project_saved_dir())/'AnimationNetworkReview';out.mkdir(parents=True,exist_ok=True)
lag = s.get('lag_ms', 0)
name = s.get('review_name', 'baseline') + '-' + str(lag) + 'ms'
cases = s.get('review_cases', ['move_cancel', 'combo', 'reaction'])
state = {'case_index':-1,'stage':'next','after':unreal.GameplayStatics.get_time_seconds(w)+.5,
    'wall':time.monotonic()};rows=[]; reports=[]

def asc(p):return p.get_hodge_ability_system_component()
def active(p):
    values=[]
    for h in asc(p).get_all_abilities():
        d=asc(p).find_ability_definition(h)
        if not d or d.get_editor_property('ExecutionRoute')!=unreal.HodgeAbilityExecutionRoute.COMBO_COORDINATED:continue
        ga=lib.call_method('GetGameplayAbilityFromSpecHandle',(asc(p),h))[0]
        if ga and lib.call_method('IsGameplayAbilityActive',(ga,)):values.append((ga,d,h))
    return values
def snap(p, clips):
    velocity=p.character_movement.velocity;accel=p.character_movement.get_current_acceleration()
    a=p.mesh.get_anim_instance();q=p.mesh.get_socket_transform('Bip001Spine2',unreal.RelativeTransformSpace.RTS_PARENT_BONE_SPACE).rotation
    states=[]
    for m in clips:
        r=inspect(p,m)
        states.append({'montage':m.get_name(),'instance':r.instance_id,'position':r.position,'weight':r.weight,
            'desired':r.desired_weight,'blend':r.blend_time,'stopped':r.stopped,'playing':r.playing,'locomotion':str(r.locomotion_state)})
    return {'role':str(p.get_local_role()),'velocity':[velocity.x,velocity.y,velocity.z],
        'acceleration':[accel.x,accel.y,accel.z],'montages':states,'spine':[q.x,q.y,q.z,q.w],
        'controlled':p.get_component_by_class(unreal.HodgeHitReactionComponent).is_controlled()}
def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    result={'error':error,'lag_ms':lag,'cases':reports,'samples':rows}
    (out/(name+'.json')).write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
    s['animation_review_result']=result
def cleanup():
    for p in [server,s['target']]:
        if not asc(p): continue
        for h in asc(p).get_all_abilities():
            ga=lib.call_method('GetGameplayAbilityFromSpecHandle',(asc(p),h))[0]
            if ga and lib.call_method('IsGameplayAbilityActive',(ga,)):
                unreal.HodgeCombatValidationLibrary.queue_ability_action(asc(p),h,True)

def tick(delta):
    try:
        now=unreal.GameplayStatics.get_time_seconds(w)
        if time.monotonic()-state['wall']>90:raise RuntimeError('probe timeout')
        if state['stage']=='next':
            if now<state['after']:return
            state['case_index']+=1
            if state['case_index']==len(cases):finish();return
            cleanup();state.update({'stage':'reset','after':now+.6});return
        if state['stage']=='reset':
            if now<state['after']:return
            for i,p in enumerate([server,s['target']]):
                p.character_movement.stop_movement_immediately()
                distance = 250 if cases[state['case_index']]=='reaction' else 1300
                p.set_actor_location(unreal.Vector(1000+i*distance,1000,95),False,False)
                p.set_actor_rotation(unreal.Rotator(0,0 if i==0 else 180,0),False)
            pc.set_control_rotation(unreal.Rotator(0,0,0))
            s['owned_target'].get_controller().set_control_rotation(unreal.Rotator(0,180,0))
            state.update({'stage':'start','after':now+.5});return
        if state['stage']=='start':
            if now<state['after']:return
            case=cases[state['case_index']]
            state.update({'stage':'running','case':case,'start':now,'last_sample':-1.,'last_montage':None,
                'last_begin':now,'next_pressed':False,'transitions':[],'move_at':None})
            if case=='reaction':
                # The reaction is applied to the remote player with the actual default 0.4 second profile duration.
                r=s['definition'].get_editor_property('DefaultHitConfig');cfg=r.get_editor_property('Reaction')
                t=unreal.GameplayTag();t.import_text('(TagName="Combat.Attack.Judgement.Skill")');cfg.set_editor_property('AttackJudgementTag',t)
                impact=unreal.HodgeImpactSpec();impact.set_editor_property('Type',unreal.HodgeImpactType.HIT_STUN)
                cfg.set_editor_property('Impacts',[impact]);r.set_editor_property('Reaction',cfg);s['definition'].set_editor_property('DefaultHitConfig',r)
                h=next(x for x in asc(owner).get_all_abilities() if asc(owner).find_ability_definition(x)==s['definition'])
                assert unreal.HodgeCombatValidationLibrary.queue_ability_action(asc(owner),h,False)
            else:sub.inject_input_vector_for_action(attack,unreal.Vector(1,0,0),[],[])
            return
        t=now-state['start'];case=state['case']
        oa=active(owner) if case!='reaction' else []
        if case=='combo' and oa:
            m=oa[0][1].get_editor_property('ExecutionConfig').get_editor_property('Montage')
            if state['last_montage']!=m:
                state['last_montage']=m;state['last_begin']=now;state['next_pressed']=False
                state['transitions'].append({'montage':m.get_name(),'t':t})
            starts=[unreal.AnimationLibrary.get_anim_notify_event_trigger_time(e)
                for e in unreal.AnimationLibrary.get_animation_notify_events(m)
                if isinstance(e.get_editor_property('NotifyStateClass'),unreal.HodgeAnimNotifyState_GameplayTag)
                and 'Status.Attack.Cancel.NextAttack' in e.get_editor_property('NotifyStateClass').get_editor_property('StateTag').export_text()]
            if len(state['transitions'])<5 and not state['next_pressed'] and starts and now-state['last_begin']>=min(starts)-.1:
                sub.inject_input_vector_for_action(attack,unreal.Vector(1,0,0),[],[]);state['next_pressed']=True
        if case=='move_cancel' and t>=.45 and t<2.5:
            sub.inject_input_vector_for_action(move,unreal.Vector(0,1,0),[],[])
            if state['move_at'] is None:state['move_at']=t
        if t-state['last_sample']<.008:return
        state['last_sample']=t
        if case=='reaction':
            targets={'owner':s['owned_target'],'server':s['target'],'observer':s['observer_target']};clips=[hit_montage]
        else:targets={'owner':owner,'server':server,'observer':observer};clips=montages
        row={'case':case,'t':t,**{role:snap(p,clips) for role,p in targets.items()}}
        row['owner_abilities']=[x[1].get_name() for x in oa]
        row['server_abilities']=[x[1].get_name() for x in active(server)] if case!='reaction' else []
        row['frame_seconds'] = delta
        rows.append(row)
        duration=7.0 if case=='combo' else 3.2
        if t>=duration:
            reports.append({'case':case,'input_move_at':state['move_at'],'owner_transitions':state['transitions']})
            cleanup();state.update({'stage':'next','after':now+.6})
    except Exception:finish(traceback.format_exc())
callback=unreal.register_slate_post_tick_callback(tick)
s['suite_callback']=callback
print('Started animation network timeline: '+name)
