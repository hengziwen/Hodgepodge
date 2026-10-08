import unreal,builtins,json,traceback
from pathlib import Path
s=builtins.HODGE_SKILL_TEST;w=s['world'];asc=s['source_asc'];target=s['target'];source=s['source'];hc=s['health']
lib=unreal.get_default_object(unreal.load_class(None,'/Script/GameplayAbilities.AbilitySystemBlueprintLibrary'))
heal=unreal.load_class(None,'/Game/CodexText/SkillHitVolumes/GE_VolumeLabHeal.GE_VolumeLabHeal_C')
tag=unreal.GameplayTag();tag.import_text('(TagName="Status.Rotation.Locked")')
tags=unreal.GameplayTagContainer();tags.import_text('(GameplayTags=((TagName="Status.Rotation.Locked")))')
cases=[('BodyHidden','Body',1),('TagOverlap','OverlapTag',1),('TagCancel','OverlapTag',1),('ShortLowFPS','ShortWindow',1),('PhaseGroups','Phase',2),('SameFrame','DoubleSource',2)]
state={'index':-1,'running':False,'grant':None,'after':0};reports=[];samples=[]
def active(ga):return lib.call_method('IsGameplayAbilityActive',(ga,))
def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    unreal.SystemLibrary.execute_console_command(w,'t.MaxFPS 0')
    source.mesh.set_visibility(True,True)
    lib.call_method('RemoveLooseGameplayTags',(source,tags,False))
    if state['grant'] and unreal.SystemLibrary.is_valid(state['grant']):s['manager'].unequip_item(state['grant'])
    entry=unreal.HodgeAbilitySet_Definition();entry.import_text('(Definition="/Script/Hodgepodge.HodgeAbilityDefinition\'/Game/CodexText/SkillHitVolumes/DA_VolumeThreePulse.DA_VolumeThreePulse\'",AbilityLevel=1)');s['ability_set'].set_editor_property('GrantedAbilityDefinitions',[entry])
    Path('E:/Project/Git/Hodgepodge/Saved/AnimNotifyMigration/extra-tests.json').write_text(json.dumps({'error':error,'cases':reports,'samples':samples},ensure_ascii=False,indent=2),encoding='utf-8')
def begin(index):
    name,dname,count=cases[index];d=unreal.load_asset('/Game/CodexText/AnimNotifyCombat/DA_'+dname)
    s['target_asc'].apply_gameplay_effect_spec_to_self(s['target_asc'].make_outgoing_spec(heal,1,s['target_asc'].make_effect_context()))
    source.set_actor_location(unreal.Vector(1000,1000,300),False,False);source.set_actor_rotation(unreal.Rotator(0,0,0),False)
    target.set_actor_location(unreal.Vector(1200,1000,300),False,False)
    target.capsule_component.set_collision_enabled(unreal.CollisionEnabled.QUERY_AND_PHYSICS)
    if name=='BodyHidden':
        # 对准真实左手骨骼位置，无 Body 来源注册、无手持请求。
        pos=source.mesh.get_socket_location('Bip001LHand')
        target.set_actor_location(pos,False,False);source.mesh.set_visibility(False,True)
    if name.startswith('Tag'):lib.call_method('AddLooseGameplayTags',(source,tags,False))
    if name=='ShortLowFPS':unreal.SystemLibrary.execute_console_command(w,'t.MaxFPS 10')
    entry=unreal.HodgeAbilitySet_Definition();entry.import_text('(Definition="/Script/Hodgepodge.HodgeAbilityDefinition\''+d.get_path_name()+'\'",AbilityLevel=1)');s['ability_set'].set_editor_property('GrantedAbilityDefinitions',[entry])
    state['grant']=s['manager'].equip_item(s['grant_class']);assert state['grant']
    handle=next(h for h in asc.get_all_abilities() if asc.find_ability_definition(h)==d)
    ga=lib.call_method('GetGameplayAbilityFromSpecHandle',(asc,handle))[0]
    assert unreal.HodgeCombatValidationLibrary.queue_ability_action(asc,handle,False)
    state.update({'running':True,'name':name,'count':count,'start':unreal.GameplayStatics.get_time_seconds(w),'ga':ga,'handle':handle,'duration':d.get_duration(),'tag_mid':False,'seen':False,'cancelled':False,'max_sessions':0,'pose_seen':False})
def tick(delta):
    try:
        now=unreal.GameplayStatics.get_time_seconds(w)
        if not state['running']:
            if now<state['after']:return
            state['index']+=1
            if state['index']>=len(cases):finish();return
            begin(state['index']);return
        t=now-state['start'];ga=state['ga'];name=state['name'];windows=unreal.HodgeCombatValidationLibrary.inspect_ability_windows(ga)
        d=ga.get_definition();montage=d.get_editor_property('ExecutionConfig').get_editor_property('Montage')
        animation_time=source.mesh.get_anim_instance().montage_get_position(montage) if active(ga) else 0
        owns=tag.export_text() in windows.export_text()
        sessions=unreal.HodgeCombatValidationLibrary.inspect_hit_sessions(source)
        poses=unreal.HodgeCombatValidationLibrary.inspect_pose_leases(source)
        state['max_sessions']=max(sessions,state['max_sessions']);state['pose_seen']|=poses>0;state['seen']|=bool(active(ga))
        samples.append({'case':name,'t':t,'health':hc.get_health(),'sessions':sessions,'poses':poses,'windows':windows.export_text()})
        if name=='TagOverlap' and .31<animation_time<.59:
            assert owns,'Second overlapping state vanished after the first exited'
            assert asc.get_gameplay_tag_count(tag)==2,'Scoped tag contributions did not preserve foreign count'
            state['tag_mid']=True
        if name=='TagCancel' and hc.get_health()<150 and not state['cancelled']:
            assert unreal.HodgeCombatValidationLibrary.queue_ability_action(asc,state['handle'],True);state['cancelled']=True
        assert t<30,'Notify test montage timed out'
        if t>state['duration']+.5 and not active(ga):
            expected=150-25*state['count']
            report={'case':name,'expected':expected,'health':hc.get_health(),'active':active(ga),'sessions':sessions,'poses':poses,'owned_windows':windows.export_text(),'seen':state['seen'],'pose_seen':state['pose_seen'],'max_sessions':state['max_sessions'],'overlap_preserved':state['tag_mid']}
            assert abs(hc.get_health()-expected)<.01,report
            assert not report['active'] and not sessions and not poses and state['seen'] and state['pose_seen'],report
            if name=='TagOverlap':assert state['tag_mid'],report
            # 外部 loose tag 的贡献仍应在，随后由它自己的调用者移除。
            if name.startswith('Tag'):
                assert asc.get_gameplay_tag_count(tag)==1,report
                lib.call_method('RemoveLooseGameplayTags',(source,tags,False))
                assert asc.get_gameplay_tag_count(tag)==0,report
            reports.append(report)
            source.mesh.set_visibility(True,True);unreal.SystemLibrary.execute_console_command(w,'t.MaxFPS 0')
            s['manager'].unequip_item(state['grant']);state['grant']=None
            state['running']=False;state['after']=now+.3
    except Exception:finish(traceback.format_exc())
callback=unreal.register_slate_post_tick_callback(tick)
print('Started six notify-specific tests: hidden body, overlapping states, cancellation, short low-FPS window and explicit phases.')
