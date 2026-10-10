"""Apply actual probe Melee damage and zero-damage control to a sprinting Main Hero."""
import unreal,builtins,json,traceback,time
from pathlib import Path
s=builtins.HODGE_REACTION_TEST;world=s['world'];owner=s['source_owner'];server=s['source'];caster=s['target'];caster_owner=s['owned_target']
lib=unreal.HodgeCombatValidationLibrary;bridge=s['lib'];pc=owner.get_controller()
sub=unreal.get_default_object(unreal.load_class(None,'/Script/Engine.SubsystemBlueprintLibrary')).call_method(
    'GetLocalPlayerSubSystemFromPlayerController',(pc,unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
move=unreal.load_asset('/Game/Main/Input/InputAction/IA_Move');dash=unreal.load_asset('/Game/Main/Input/InputAction/IA_Sprint')
definition=s['definition'];original=definition.get_editor_property('DefaultHitConfig').export_text()
cases=['accepted_light_damage','zero_damage_control'];reports=[];rows=[]
ctx={'index':-1,'stage':'next','after':unreal.GameplayStatics.get_time_seconds(world)+.5,'wall':time.monotonic()}
out=Path(unreal.Paths.project_saved_dir())/'DashTimingRefinement'
def asc(p):return p.get_hodge_ability_system_component()
def policy(p):return p.get_component_by_class(unreal.HodgeLocomotionPolicyComponent)
def health(p):
    tag=unreal.GameplayAttribute();tag.import_text('(Attribute="/Script/Hodgepodge.HodgeHealthSet:Health")')
    return bridge.call_method('GetFloatAttributeFromAbilitySystemComponent',(asc(p),tag))[0]
def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    cfg=definition.get_editor_property('DefaultHitConfig');cfg.import_text(original);definition.set_editor_property('DefaultHitConfig',cfg)
    owner.get_component_by_class(unreal.HodgeHeroComponent).end_sprint_input()
    result={'error':error,'cases':reports,'samples':rows,'lag_ms':s.get('lag_ms',0)}
    out.joinpath('hit-exit-'+str(s.get('lag_ms',0))+'ms.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
    s['movement_hit_result']=result
def tick(delta):
    try:
        now=unreal.GameplayStatics.get_time_seconds(world)
        if time.monotonic()-ctx['wall']>90:raise RuntimeError('hit exit timeout')
        if ctx['stage']=='next':
            if now<ctx['after']:return
            ctx['index']+=1
            if ctx['index']==len(cases):finish();return
            case=cases[ctx['index']]
            owner.get_component_by_class(unreal.HodgeHeroComponent).end_sprint_input()
            for p in [owner,server]:p.character_movement.stop_movement_immediately();p.set_actor_location(unreal.Vector(1000,1000,95),False,False)
            pc.set_control_rotation(unreal.Rotator(0,0,0));caster_owner.get_controller().set_control_rotation(unreal.Rotator(0,180,0))
            lib.queue_facing_mode(owner,unreal.HodgeCharacterFacingDriver.CONTROLLER);lib.queue_facing_mode(caster_owner,unreal.HodgeCharacterFacingDriver.CONTROLLER)
            cfg=definition.get_editor_property('DefaultHitConfig');reaction=cfg.get_editor_property('Reaction')
            tag=unreal.GameplayTag();tag.import_text('(TagName="Combat.Attack.Judgement.Skill")');reaction.set_editor_property('AttackJudgementTag',tag)
            reaction.set_editor_property('HitAcceptancePolicy',unreal.HodgeHitAcceptancePolicy.EXPLICIT_CONTROL if case=='zero_damage_control' else unreal.HodgeHitAcceptancePolicy.ACCEPTED_DAMAGE)
            if case=='zero_damage_control':
                impact=unreal.HodgeImpactSpec();impact.set_editor_property('Type',unreal.HodgeImpactType.HIT_STUN);reaction.set_editor_property('Impacts',[impact])
            else:reaction.set_editor_property('Impacts',[])
            cfg.set_editor_property('DamageMultiplier',0. if case=='zero_damage_control' else 1.)
            cfg.set_editor_property('Reaction',reaction);definition.set_editor_property('DefaultHitConfig',cfg)
            ctx.update({'case':case,'stage':'prepare','after':now+1.2,'saw_sprint':False,'cast':False,'saw_control':False,'samples':[],'last':-1})
            return
        if ctx['stage']=='prepare':
            if now<ctx['after']:return
            lib.queue_facing_mode(owner,unreal.HodgeCharacterFacingDriver.MOVEMENT,True)
            ctx.update({'stage':'run','start':now,'health_before':health(server)})
            return
        age=now-ctx['start']
        if age<1.8:
            sub.inject_input_vector_for_action(move,unreal.Vector(0,1,0),[],[]);sub.inject_input_vector_for_action(dash,unreal.Vector(1,0,0),[],[])
        sprinting=policy(server).get_resolved_policy().sprinting;ctx['saw_sprint']|=sprinting
        if ctx['saw_sprint'] and age>.8:
            location=server.get_actor_location()+unreal.Vector(100,0,0)
            caster.set_actor_location(location,False,False);caster_owner.set_actor_location(location,False,False)
            if not ctx['cast']:
                handle=next(h for h in asc(caster_owner).get_all_abilities() if asc(caster_owner).find_ability_definition(h)==definition)
                assert lib.queue_ability_action(asc(caster_owner),handle,False);ctx['cast']=True
        controlled=server.get_component_by_class(unreal.HodgeHitReactionComponent).is_controlled();ctx['saw_control']|=controlled
        if age-ctx['last']>.025:
            row={'case':ctx['case'],'t':age,'sprinting':sprinting,'health':health(server),'controlled':controlled,
                'reaction':server.get_component_by_class(unreal.HodgeHitReactionComponent).get_last_result().export_text()}
            rows.append(row);ctx['samples'].append(row);ctx['last']=age
        if age>2.5:
            assert ctx['saw_sprint'] and ctx['cast'],'Sprint and actual hit ability must execute'
            minimum=min(r['health'] for r in ctx['samples'])
            if ctx['case']=='accepted_light_damage':assert minimum<ctx['health_before'] and not ctx['saw_control'],'positive damage must remain light feedback'
            else:assert abs(minimum-ctx['health_before'])<.05 and ctx['saw_control'],'pure control must have zero damage'
            assert not ctx['samples'][-1]['sprinting'],'hit must end Sprint'
            reports.append({'case':ctx['case'],'passed':True});ctx.update({'stage':'next','after':now+.8})
    except Exception:finish(traceback.format_exc())
callback=unreal.register_slate_post_tick_callback(tick);s['suite_callback']=callback
print('Started actual hit Sprint exit')
