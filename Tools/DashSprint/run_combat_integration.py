"""Verify tag-mapped early/late attack cancellation and Sprint exit using Main assets."""
import unreal,builtins,json,traceback,time
from pathlib import Path
s=builtins.HODGE_REACTION_TEST; world=s['world']; owner=s.get('source_owner',s['source']); server=s['source']
lib=unreal.HodgeCombatValidationLibrary; bridge=s.get('lib') or unreal.get_default_object(unreal.load_class(None,'/Script/GameplayAbilities.AbilitySystemBlueprintLibrary')); pc=owner.get_controller()
sub=unreal.get_default_object(unreal.load_class(None,'/Script/Engine.SubsystemBlueprintLibrary')).call_method(
    'GetLocalPlayerSubSystemFromPlayerController',(pc,unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
attack=unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack'); dash=unreal.load_asset('/Game/Main/Input/InputAction/IA_Sprint')
move=unreal.load_asset('/Game/Main/Input/InputAction/IA_Move')
cases=['attack_cancel_early','attack_cancel_allowed','sprint_attack_exit'];reports=[];rows=[]
ctx={'index':-1,'stage':'next','after':unreal.GameplayStatics.get_time_seconds(world)+.5,'wall':time.monotonic()}
out=Path(unreal.Paths.project_saved_dir())/s.get('movement_output_dir','DashTimingRefinement')

def asc(p):return p.get_hodge_ability_system_component()
def policy(p):return p.get_component_by_class(unreal.HodgeLocomotionPolicyComponent)
def active(p):
    result=[]
    for handle in asc(p).get_all_abilities():
        ga=bridge.call_method('GetGameplayAbilityFromSpecHandle',(asc(p),handle))[0]
        if ga and bridge.call_method('IsGameplayAbilityActive',(ga,)):result.append(ga)
    return result
def attacks(p):return [g for g in active(p) if isinstance(g,unreal.HodgeGameplayAbility_Definition)]
def snapshot(p):
    return {'sprinting':policy(p).get_resolved_policy().sprinting,
        'attack':bool(attacks(p)),'dash':any(isinstance(g,unreal.HodgeGameplayAbility_Dash) and g.has_committed_movement() for g in active(p)),
        'locked':p.get_component_by_class(unreal.HodgeCharacterRotationComponent).is_yaw_locked()}
def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    owner.get_component_by_class(unreal.HodgeHeroComponent).end_sprint_input()
    result={'error':error,'cases':reports,'samples':rows,'lag_ms':s.get('lag_ms',0)}
    out.joinpath(('' if s.get('network') else 'single-')+'combat-'+str(s.get('lag_ms',0))+'ms.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
    s['movement_combat_result']=result
def tick(delta):
    try:
        now=unreal.GameplayStatics.get_time_seconds(world)
        if time.monotonic()-ctx['wall']>100:raise RuntimeError('combat movement timeout')
        if ctx['stage']=='next':
            if now<ctx['after']:return
            ctx['index']+=1
            if ctx['index']==len(cases):finish();return
            case=cases[ctx['index']]
            owner.get_component_by_class(unreal.HodgeHeroComponent).end_sprint_input()
            for p in [owner,server]:
                p.character_movement.stop_movement_immediately();p.set_actor_location(unreal.Vector(1000,1000,95),False,False)
            s['target'].set_actor_location(unreal.Vector(2400,1000,95),False,False)
            pc.set_control_rotation(unreal.Rotator(0,0,0));lib.queue_facing_mode(owner,unreal.HodgeCharacterFacingDriver.CONTROLLER)
            ctx.update({'case':case,'stage':'prepare','after':now+1.2,'pressed':False,'saw_sprint':False,'saw_window':False,'samples':[],'last':-1})
            return
        if ctx['stage']=='prepare':
            if now<ctx['after']:return
            lib.queue_facing_mode(owner,unreal.HodgeCharacterFacingDriver.MOVEMENT,True)
            ctx.update({'stage':'start','after':now+.25});return
        if ctx['stage']=='start':
            if now<ctx['after']:return
            ctx.update({'stage':'run','start':now,'attack_started':False})
            if ctx['case']!='sprint_attack_exit':
                sub.inject_input_vector_for_action(attack,unreal.Vector(1,0,0),[],[]);ctx['attack_started']=True
            return
        age=now-ctx['start'];case=ctx['case']
        if case=='attack_cancel_early' and .05<=age<.15:
            sub.inject_input_vector_for_action(dash,unreal.Vector(1,0,0),[],[])
        if case=='attack_cancel_allowed' and not ctx['pressed']:
            tag=policy(owner).get_profile().get_editor_property('AttackCancelWindow').export_text()
            tag_name=tag.split('"')[1] if '"' in tag else ''
            allowed=[g for g in attacks(owner) if tag_name and tag_name in lib.inspect_ability_windows(g).export_text()]
            if allowed:
                assert not owner.get_component_by_class(unreal.HodgeCharacterRotationComponent).is_yaw_locked()
                sub.inject_input_vector_for_action(dash,unreal.Vector(1,0,0),[],[]);ctx['pressed']=True;ctx['saw_window']=True
        if case=='sprint_attack_exit':
            if age<1.1:
                sub.inject_input_vector_for_action(move,unreal.Vector(0,1,0),[],[])
                sub.inject_input_vector_for_action(dash,unreal.Vector(1,0,0),[],[])
            ctx['saw_sprint']|=policy(server).get_resolved_policy().sprinting
            if ctx['saw_sprint'] and age>.8 and not ctx['attack_started']:
                sub.inject_input_vector_for_action(attack,unreal.Vector(1,0,0),[],[]);ctx['attack_started']=True
        if age-ctx['last']>=.025:
            row={'case':case,'t':age,'owner':snapshot(owner),'server':snapshot(server)}
            rows.append(row);ctx['samples'].append(row);ctx['last']=age
        if age>2.6:
            values=[r['server'] for r in ctx['samples']]
            if case=='attack_cancel_early':
                assert any(r['attack'] for r in values),'actual attack must execute'
                assert any(r['dash'] for r in values),'tag-mapped Dash must cancel before the old window'
            elif case=='attack_cancel_allowed':
                assert ctx['saw_window'],'actual cancel window must be observed'
                assert any(r['dash'] for r in values),'window Dash must commit'
            else:
                assert ctx['saw_sprint'] and ctx['attack_started'],'Sprint and attack must both execute'
                assert not values[-1]['sprinting'],'attack must end Sprint'
            reports.append({'case':case,'passed':True});ctx.update({'stage':'next','after':now+.5})
    except Exception:finish(traceback.format_exc())
callback=unreal.register_slate_post_tick_callback(tick);s['suite_callback']=callback
print('Started movement combat integration')
