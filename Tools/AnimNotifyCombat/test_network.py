import unreal,builtins,json,traceback
from pathlib import Path
out=Path('E:/Project/Git/Hodgepodge/Saved/AnimNotifyMigration')
worlds=unreal.EditorLevelLibrary.get_pie_worlds(False);assert len(worlds)==2
owned=[(w,unreal.GameplayStatics.get_player_character(w,0)) for w in worlds];assert all(h for w,h in owned)
server,host=next((w,h) for w,h in owned if h.has_authority());client,remote=next((w,h) for w,h in owned if not h.has_authority())
def player_id(h):return h.get_editor_property('player_state').get_editor_property('PlayerId')
server_remote=next(h for h in unreal.GameplayStatics.get_all_actors_of_class(server,host.get_class()) if h.get_editor_property('player_state') and player_id(h)==player_id(remote))
client_host=next(h for h in unreal.GameplayStatics.get_all_actors_of_class(client,host.get_class()) if h.get_editor_property('player_state') and player_id(h)==player_id(host))
def asc(h):return h.get_editor_property('player_state').get_hodge_ability_system_component()
def health(h):return h.get_component_by_class(unreal.HodgeHealthComponent).get_health()
for w in worlds:unreal.SystemLibrary.execute_console_command(w,'NetEmulation.PktLag 100')
for i,h in enumerate([host,server_remote]):
    h.character_movement.set_movement_mode(unreal.MovementMode.MOVE_NONE)
    h.capsule_component.set_collision_response_to_channel(unreal.CollisionChannel.ECC_PAWN,unreal.CollisionResponseType.ECR_OVERLAP)
    h.set_actor_location(unreal.Vector(1000+i*200,1000,300),False,False)
lib=unreal.get_default_object(unreal.load_class(None,'/Script/GameplayAbilities.AbilitySystemBlueprintLibrary'))
reject_tags=unreal.GameplayTagContainer();reject_tags.import_text('(GameplayTags=((TagName="Status.Death.Dying")))')
heal_cls=unreal.load_class(None,'/Game/CodexText/SkillHitVolumes/GE_VolumeLabHeal.GE_VolumeLabHeal_C')
grant_class=unreal.load_asset('/Game/CodexText/SkillHitVolumes/BP_VolumeLabGrant').generated_class()
aset=unreal.load_asset('/Game/CodexText/SkillHitVolumes/AS_VolumeLab')
defs={n:unreal.load_asset('/Game/CodexText/SkillHitVolumes/DA_Volume'+n) for n in ['ThreePulse','ThreeWindows','FixedCenter','Confirmed']}
cases=[('HostPoint3',False,'ThreePulse',3),('ClientPoint3',True,'ThreePulse',3),('ClientWindow3',True,'ThreeWindows',3),('ClientCancelBefore',True,'ThreePulse',0),('ClientCancelAfterFirst',True,'ThreePulse',1),('ClientFixedCenter',True,'FixedCenter',3),('ClientConfirmed3',True,'Confirmed',3),('ClientRejected',True,'ThreePulse',0)]
state={'index':-1,'phase':'idle','after':unreal.GameplayStatics.get_time_seconds(server)+.8,'grant':None};rows=[];reports=[]
def get_ga(a,handle):return lib.call_method('GetGameplayAbilityFromSpecHandle',(a,handle))[0]
def active(ga):return bool(ga and lib.call_method('IsGameplayAbilityActive',(ga,)))
def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    if state['grant'] and unreal.SystemLibrary.is_valid(state['grant']):state['manager'].unequip_item(state['grant'])
    for h in [host,server_remote]:lib.call_method('RemoveLooseGameplayTags',(h,reject_tags,False))
    entry=unreal.HodgeAbilitySet_Definition();entry.import_text('(Definition="/Script/Hodgepodge.HodgeAbilityDefinition\'/Game/CodexText/SkillHitVolumes/DA_VolumeThreePulse.DA_VolumeThreePulse\'",AbilityLevel=1)');aset.set_editor_property('GrantedAbilityDefinitions',[entry])
    (out/'network-tests.json').write_text(json.dumps({'error':error,'packet_lag_ms':100,'cases':reports,'samples':rows},ensure_ascii=False,indent=2),encoding='utf-8')
def prepare(index):
    name,owned_client,dname,count=cases[index]
    source_server=server_remote if owned_client else host;source_owner=remote if owned_client else host
    target_server=host if owned_client else server_remote;target_observer=client_host if owned_client else remote
    for h in [host,server_remote]:
        h.set_actor_location(unreal.Vector(1000 if h==source_server else 1200,1000,300),False,False)
        heal=asc(h).make_outgoing_spec(heal_cls,1,asc(h).make_effect_context());asc(h).apply_gameplay_effect_spec_to_self(heal)
    d=defs[dname];entry=unreal.HodgeAbilitySet_Definition();entry.import_text('(Definition="/Script/Hodgepodge.HodgeAbilityDefinition\''+d.get_path_name()+'\'",AbilityLevel=1)');aset.set_editor_property('GrantedAbilityDefinitions',[entry])
    manager=source_server.get_component_by_class(unreal.HodgeEquipmentManagerComponent);grant=manager.equip_item(grant_class);assert grant
    handles=[h for h in asc(source_server).get_all_abilities() if asc(source_server).find_ability_definition(h)==d];assert len(handles)==1
    if name=='ClientRejected':assert lib.call_method('AddLooseGameplayTags',(source_server,reject_tags,False))
    state.update({'phase':'grant_wait','name':name,'d':d,'count':count,'manager':manager,'grant':grant,'server_handle':handles[0],'source_server':source_server,'source_owner':source_owner,'target_server':target_server,'target_observer':target_observer,'after':unreal.GameplayStatics.get_time_seconds(server)+.65,'deadline':unreal.GameplayStatics.get_time_seconds(server)+5})
def tick(delta):
    try:
        now=unreal.GameplayStatics.get_time_seconds(server)
        if state['phase']=='idle':
            if now<state['after']:return
            state['index']+=1
            if state['index']>=len(cases):finish();return
            prepare(state['index']);return
        if state['phase']=='grant_wait':
            if now<state['after']:return
            a=asc(state['source_owner']);handles=[h for h in a.get_all_abilities() if h.export_text()==state['server_handle'].export_text() and a.find_ability_definition(h)==state['d']]
            if not handles:
                assert now<state['deadline'],'Owner did not receive definition/spec';return
            assert unreal.HodgeCombatValidationLibrary.queue_ability_action(a,handles[0],False),(state['name'],'queue failed')
            state.update({'phase':'running','start':now,'owner_handle':handles[0],'owner_ga':get_ga(a,handles[0]),'server_ga':get_ga(asc(state['source_server']),state['server_handle']),'prepared':False,'cancelled':False,'changed':False,'server_steps':[health(state['target_server'])],'server_was_active':False,'owner_was_active':False})
            return
        t=now-state['start'];name=state['name'];sg=state['server_ga'];og=state['owner_ga'];length=state['d'].get_duration()
        sa=active(sg);oa=active(og);state['server_was_active']|=sa;state['owner_was_active']|=oa
        if sa and not state['prepared']:
            if name=='ClientFixedCenter':assert sg.set_hit_anchor('Center',state['source_server'].get_actor_transform())
            if name=='ClientConfirmed3':assert sg.set_hit_target('Enemy',state['target_server'])
            state['prepared']=True
        montage=state['d'].get_editor_property('ExecutionConfig').get_editor_property('Montage')
        norm=state['source_owner'].mesh.get_anim_instance().montage_get_position(montage)/length if oa else 0
        if not state['cancelled'] and oa and ((name=='ClientCancelBefore' and norm>.06) or (name=='ClientCancelAfterFirst' and norm>.32)):
            assert unreal.HodgeCombatValidationLibrary.queue_ability_action(asc(state['source_owner']),state['owner_handle'],True);state['cancelled']=True
        server_norm=state['source_server'].mesh.get_anim_instance().montage_get_position(montage)/length if sa else 0
        if name=='ClientFixedCenter' and not state['changed'] and server_norm>.33:
            state['source_server'].set_actor_location(unreal.Vector(3000,1000,300),False,False);state['changed']=True
        sh=health(state['target_server']);ch=health(state['target_observer'])
        if abs(sh-state['server_steps'][-1])>.01:state['server_steps'].append(sh)
        rows.append({'case':name,'t':t,'server_health':sh,'replicated_health':ch,'server_active':sa,'owner_active':oa})
        assert t<max(30,length*5),'Montage failed to finish within the safety timeout'
        if t>length+1.0 and not sa and not oa:
            expected=150-25*state['count'];r={'case':name,'expected':expected,'server_health':sh,'replicated_health':ch,'server_steps':state['server_steps'],'server_was_active':state['server_was_active'],'owner_was_active':state['owner_was_active'],'server_active_after':sa,'owner_active_after':oa};reports.append(r)
            assert abs(sh-expected)<.01 and abs(ch-expected)<.01,r
            assert len(state['server_steps'])-1==state['count'],r
            assert not sa and not oa,r
            assert state['server_was_active'] == (name!='ClientRejected'),r
            if name=='ClientRejected':lib.call_method('RemoveLooseGameplayTags',(state['source_server'],reject_tags,False))
            state['manager'].unequip_item(state['grant']);state['grant']=None;state['phase']='idle';state['after']=now+.6
    except Exception:finish(traceback.format_exc())
callback=unreal.register_slate_post_tick_callback(tick)
print('Started 8 real listen-server cases with 100ms packet lag, including client prediction, remote cancellation and rejection')
