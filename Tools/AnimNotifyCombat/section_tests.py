import unreal,builtins,json,traceback
from pathlib import Path
s=builtins.HODGE_SKILL_TEST;w=s['world'];asc=s['source_asc'];source=s['source'];target=s['target'];hc=s['health']
lib=unreal.get_default_object(unreal.load_class(None,'/Script/GameplayAbilities.AbilitySystemBlueprintLibrary'))
heal=unreal.load_class(None,'/Game/CodexText/SkillHitVolumes/GE_VolumeLabHeal.GE_VolumeLabHeal_C')
cases=[('LoopRepeat','Loop',2),('JumpBeforeHit','Sections',0),('PlayRate2','ThreePulse',3),('ComponentUnregister','OverlapTag',1)]
state={'index':-1,'running':False,'grant':None,'after':0};reports=[];samples=[]
def active(ga):return lib.call_method('IsGameplayAbilityActive',(ga,))
def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    if state.get('saved_config'):state['d'].set_editor_property('ExecutionConfig',state['saved_config'])
    if state['grant'] and unreal.SystemLibrary.is_valid(state['grant']):s['manager'].unequip_item(state['grant'])
    entry=unreal.HodgeAbilitySet_Definition();entry.import_text('(Definition="/Script/Hodgepodge.HodgeAbilityDefinition\'/Game/CodexText/SkillHitVolumes/DA_VolumeThreePulse.DA_VolumeThreePulse\'",AbilityLevel=1)');s['ability_set'].set_editor_property('GrantedAbilityDefinitions',[entry])
    Path('E:/Project/Git/Hodgepodge/Saved/AnimNotifyMigration/section-tests.json').write_text(json.dumps({'error':error,'cases':reports,'samples':samples},ensure_ascii=False,indent=2),encoding='utf-8')
def begin(index):
    name,dname,count=cases[index]
    d=unreal.load_asset(('/Game/CodexText/SkillHitVolumes/DA_Volume' if dname=='ThreePulse' else '/Game/CodexText/AnimNotifyCombat/DA_')+dname)
    cfg=d.get_editor_property('ExecutionConfig');saved=unreal.HodgeAbilityExecutionConfig();saved.import_text(cfg.export_text())
    if name=='PlayRate2':cfg.set_editor_property('PlayRate',2);d.set_editor_property('ExecutionConfig',cfg)
    s['target_asc'].apply_gameplay_effect_spec_to_self(s['target_asc'].make_outgoing_spec(heal,1,s['target_asc'].make_effect_context()))
    source.set_actor_location(unreal.Vector(1000,1000,300),False,False);target.set_actor_location(unreal.Vector(1200,1000,300),False,False)
    entry=unreal.HodgeAbilitySet_Definition();entry.import_text('(Definition="/Script/Hodgepodge.HodgeAbilityDefinition\''+d.get_path_name()+'\'",AbilityLevel=1)');s['ability_set'].set_editor_property('GrantedAbilityDefinitions',[entry])
    state['grant']=s['manager'].equip_item(s['grant_class']);assert state['grant']
    handle=next(h for h in asc.get_all_abilities() if asc.find_ability_definition(h)==d);ga=lib.call_method('GetGameplayAbilityFromSpecHandle',(asc,handle))[0]
    assert unreal.HodgeCombatValidationLibrary.queue_ability_action(asc,handle,False)
    state.update({'name':name,'count':count,'ga':ga,'handle':handle,'d':d,'saved_config':saved,'start':unreal.GameplayStatics.get_time_seconds(w),'running':True,'acted':False,'cancelled':False,'seen':False})
def tick(delta):
    try:
        now=unreal.GameplayStatics.get_time_seconds(w)
        if not state['running']:
            if now<state['after']:return
            state['index']+=1
            if state['index']>=len(cases):finish();return
            begin(state['index']);return
        t=now-state['start'];name=state['name'];ga=state['ga'];a=active(ga);state['seen']|=bool(a)
        if a and not state['acted']:
            if name=='LoopRepeat':ga.montage_set_next_section_name('SecondHalf','Default');state['acted']=True
            elif name=='JumpBeforeHit':ga.montage_jump_to_section('SecondHalf');state['acted']=True
            elif name=='ComponentUnregister' and t>.25 and hc.get_health()<150:
                source.get_component_by_class(unreal.HodgeCombatComponentBase).call_method('K2_DestroyComponent',(source,))
                state['acted']=True
        if name=='LoopRepeat' and t>state['d'].get_duration()+.4 and not state['cancelled']:
            assert a,'Loop montage unexpectedly auto-ended';assert unreal.HodgeCombatValidationLibrary.queue_ability_action(asc,state['handle'],True);state['cancelled']=True
        samples.append({'case':name,'t':t,'health':hc.get_health(),'active':bool(a),'sessions':unreal.HodgeCombatValidationLibrary.inspect_hit_sessions(source),'poses':unreal.HodgeCombatValidationLibrary.inspect_pose_leases(source)})
        finish_time=state['d'].get_duration()+.8
        if name=='PlayRate2':finish_time=state['d'].get_duration()/2+.6
        if name=='ComponentUnregister':finish_time=.8
        if t>finish_time:
            report={'case':name,'health':hc.get_health(),'expected':150-25*state['count'],'active':bool(a),'sessions':unreal.HodgeCombatValidationLibrary.inspect_hit_sessions(source),'poses':unreal.HodgeCombatValidationLibrary.inspect_pose_leases(source),'seen':state['seen']}
            assert abs(report['health']-report['expected'])<.01 and not a and not report['sessions'] and not report['poses'] and state['seen'],report
            reports.append(report)
            d=state['d'];d.set_editor_property('ExecutionConfig',state['saved_config']);state['saved_config']=None
            s['manager'].unequip_item(state['grant']);state['grant']=None
            state['running']=False;state['after']=now+.3
    except Exception:finish(traceback.format_exc())
callback=unreal.register_slate_post_tick_callback(tick)
print('Started section jump, loop reentry, doubled play-rate and component-unregister tests.')
