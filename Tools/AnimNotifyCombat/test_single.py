import unreal,builtins,json,traceback
from pathlib import Path
out=Path('E:/Project/Git/Hodgepodge/Saved/AnimNotifyMigration')
s=builtins.HODGE_SKILL_TEST
w=s['world'];asc=s['source_asc'];target_asc=s['target_asc'];source=s['source'];target=s['target'];hc=s['health']
lib=unreal.get_default_object(unreal.load_class(None,'/Script/GameplayAbilities.AbilitySystemBlueprintLibrary'))
heal_cls=unreal.load_class(None,'/Game/CodexText/SkillHitVolumes/GE_VolumeLabHeal.GE_VolumeLabHeal_C');assert heal_cls
heal_tag=unreal.GameplayTag();heal_tag.import_text('(TagName="SetByCaller.Heal")')
defs={name:unreal.load_asset('/Game/CodexText/SkillHitVolumes/DA_Volume'+name) for name in ['ThreePulse','ThreeWindows','OnceWindow','FixedCenter','Confirmed','SharedExecution','SharedTrigger']}
cases=[('Point3','ThreePulse',3),('Window3','ThreeWindows',3),('OnceLate','OnceWindow',0),('FixedCenter','FixedCenter',3),('Confirmed3','Confirmed',3),('SharedExecution','SharedExecution',1),('SharedTrigger','SharedTrigger',3),('CancelBefore','ThreePulse',0),('CancelAfterFirst','ThreePulse',1),('LateEntry','ThreePulse',2)]
rows=[];reports=[];state={'index':-1,'waiting_until':0,'running':False,'grant':None,'error':None};origin=unreal.Vector(1000,1000,300)
def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    if state['grant'] and unreal.SystemLibrary.is_valid(state['grant']):s['manager'].unequip_item(state['grant'])
    entry=unreal.HodgeAbilitySet_Definition();entry.import_text('(Definition="/Script/Hodgepodge.HodgeAbilityDefinition\'/Game/CodexText/SkillHitVolumes/DA_VolumeThreePulse.DA_VolumeThreePulse\'",AbilityLevel=1)')
    s['ability_set'].set_editor_property('GrantedAbilityDefinitions',[entry])
    (out/'single-tests.json').write_text(json.dumps({'error':error,'cases':reports,'samples':rows},ensure_ascii=False,indent=2),encoding='utf-8')
def begin(index):
    name,dname,count=cases[index];d=defs[dname]
    spec=target_asc.make_outgoing_spec(heal_cls,1,target_asc.make_effect_context());spec=lib.call_method('AssignTagSetByCallerMagnitude',(spec,heal_tag,10000.0));target_asc.apply_gameplay_effect_spec_to_self(spec)
    assert abs(hc.get_health()-150)<.01,hc.get_health()
    source.set_actor_location(origin,False,False);source.set_actor_rotation(unreal.Rotator(0,0,0),False)
    target.capsule_component.set_collision_enabled(unreal.CollisionEnabled.QUERY_AND_PHYSICS)
    pos=origin+unreal.Vector(200,0,0)
    if name in ['OnceLate','LateEntry']:pos=origin+unreal.Vector(900,0,0)
    if name=='Confirmed3':pos=origin+unreal.Vector(800,0,0);target.capsule_component.set_collision_enabled(unreal.CollisionEnabled.NO_COLLISION)
    target.set_actor_location(pos,False,False)
    entry=unreal.HodgeAbilitySet_Definition();entry.import_text('(Definition="/Script/Hodgepodge.HodgeAbilityDefinition\''+d.get_path_name()+'\'",AbilityLevel=1)')
    s['ability_set'].set_editor_property('GrantedAbilityDefinitions',[entry])
    state['grant']=s['manager'].equip_item(s['grant_class']);assert state['grant']
    handles=[h for h in asc.get_all_abilities() if asc.find_ability_definition(h)==d];assert len(handles)==1
    handle=handles[0]
    assert unreal.HodgeCombatValidationLibrary.queue_ability_action(asc,handle,False),(name,'Queue failed')
    ga,instance=lib.call_method('GetGameplayAbilityFromSpecHandle',(asc,handle));assert ga and instance
    state.update({'running':True,'name':name,'def':d,'ga':ga,'handle':handle,'count':count,'start':unreal.GameplayStatics.get_time_seconds(w),'length':d.get_duration(),'changed':False,'cancelled':False,'prepared':False,'seen_active':False,'health_steps':[hc.get_health()]})
def tick(delta):
    try:
        now=unreal.GameplayStatics.get_time_seconds(w)
        if not state['running']:
            if now<state['waiting_until']:return
            state['index']+=1
            if state['index']>=len(cases):finish();return
            begin(state['index']);return
        t=now-state['start'];norm=t/state['length'];name=state['name'];ga=state['ga']
        if lib.call_method('IsGameplayAbilityActive',(ga,)):
            state['seen_active']=True
            if not state['prepared']:
                if name=='FixedCenter':assert ga.set_hit_anchor('Center',unreal.Transform(location=origin))
                if name=='Confirmed3':assert ga.set_hit_target('Enemy',target)
                state['prepared']=True
        health=hc.get_health()
        if abs(health-state['health_steps'][-1])>.01:state['health_steps'].append(health)
        rows.append({'case':name,'t':t,'health':health,'active':lib.call_method('IsGameplayAbilityActive',(ga,))})
        if not state['changed']:
            if name=='OnceLate' and norm>.4:target.set_actor_location(origin+unreal.Vector(200,0,0),False,False);state['changed']=True
            if name=='LateEntry' and norm>.3:target.set_actor_location(origin+unreal.Vector(200,0,0),False,False);state['changed']=True
            if name=='FixedCenter' and norm>.3:source.set_actor_location(origin+unreal.Vector(2000,0,0),False,False);state['changed']=True
            if name=='Confirmed3' and norm>.3:target.set_actor_location(origin+unreal.Vector(1400,0,0),False,False);state['changed']=True
        if not state['cancelled'] and ((name=='CancelBefore' and norm>.08) or (name=='CancelAfterFirst' and norm>.3)):
            assert unreal.HodgeCombatValidationLibrary.queue_ability_action(asc,state['handle'],True);state['cancelled']=True
        if t>state['length']+.45:
            expected=150-25*state['count']
            report={'case':name,'health_steps':state['health_steps'],'expected':expected,'actual':health,'active_after':lib.call_method('IsGameplayAbilityActive',(ga,))}
            reports.append(report)
            assert abs(health-expected)<.01,report
            assert not report['active_after'],report
            assert state['seen_active'],report
            assert len(state['health_steps'])-1==state['count'],report
            s['manager'].unequip_item(state['grant']);state['grant']=None
            state['running']=False;state['waiting_until']=now+.25
    except Exception:finish(traceback.format_exc())
callback=unreal.register_slate_post_tick_callback(tick)
print('Started 10 real GA/GE cases: point/window hits, once-only late entry, fixed center, confirmed target, groups, cancellation and reactivation')
