import unreal,json,traceback
from pathlib import Path
worlds=unreal.EditorLevelLibrary.get_pie_worlds(True)
server=next(w for w in worlds if unreal.GameplayStatics.get_game_mode(w))
clients=[w for w in worlds if w!=server];assert len(clients)==2,len(worlds)
local=[unreal.GameplayStatics.get_player_character(w,0) for w in clients];assert all(local)
def pid(h):return h.get_editor_property('player_state').get_editor_property('PlayerId')
spawns=[h for h in unreal.GameplayStatics.get_all_actors_of_class(server,local[0].get_class()) if h.get_editor_property('player_state')]
source=next(h for h in spawns if pid(h)==pid(local[0]));target=next(h for h in spawns if pid(h)==pid(local[1]))
def asc(h):return h.get_editor_property('player_state').get_hodge_ability_system_component()
def health(h):return h.get_component_by_class(unreal.HodgeHealthComponent).get_health()
lib=unreal.get_default_object(unreal.load_class(None,'/Script/GameplayAbilities.AbilitySystemBlueprintLibrary'))
for w in worlds:unreal.SystemLibrary.execute_console_command(w,'NetEmulation.PktLag 100')
for h in [source,target]:
    h.character_movement.set_movement_mode(unreal.MovementMode.MOVE_NONE)
    h.capsule_component.set_collision_response_to_channel(unreal.CollisionChannel.ECC_PAWN,unreal.CollisionResponseType.ECR_OVERLAP)
    h.set_actor_location(unreal.Vector(1000,1000,300),False,False)
manager=source.get_component_by_class(unreal.HodgeEquipmentManagerComponent)
aset=unreal.load_asset('/Game/CodexText/SkillHitVolumes/AS_VolumeLab');grant=unreal.load_asset('/Game/CodexText/SkillHitVolumes/BP_VolumeLabGrant').generated_class()
heal=unreal.load_class(None,'/Game/CodexText/SkillHitVolumes/GE_VolumeLabHeal.GE_VolumeLabHeal_C')
cases=[('UnrenderedBody','/Game/CodexText/AnimNotifyCombat/DA_Body',1),('ClientThreeHits','/Game/CodexText/SkillHitVolumes/DA_VolumeThreeWindows',3)]
state={'index':-1,'phase':'idle','grant':None,'after':unreal.GameplayStatics.get_time_seconds(server)+1};reports=[];samples=[]
def active(ga):return bool(ga and lib.call_method('IsGameplayAbilityActive',(ga,)))
def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    if state['grant'] and unreal.SystemLibrary.is_valid(state['grant']):manager.unequip_item(state['grant'])
    entry=unreal.HodgeAbilitySet_Definition();entry.import_text('(Definition="/Script/Hodgepodge.HodgeAbilityDefinition\'/Game/CodexText/SkillHitVolumes/DA_VolumeThreePulse.DA_VolumeThreePulse\'",AbilityLevel=1)');aset.set_editor_property('GrantedAbilityDefinitions',[entry])
    Path('E:/Project/Git/Hodgepodge/Saved/AnimNotifyMigration/dedicated-tests.json').write_text(json.dumps({'error':error,'worlds':[w.get_name() for w in worlds],'cases':reports,'samples':samples},ensure_ascii=False,indent=2),encoding='utf-8')
def begin(index):
    name,path,count=cases[index];d=unreal.load_asset(path)
    asc(target).apply_gameplay_effect_spec_to_self(asc(target).make_outgoing_spec(heal,1,asc(target).make_effect_context()))
    source.set_actor_location(unreal.Vector(1000,1000,300),False,False)
    pos=source.mesh.get_socket_location('Bip001LHand') if name=='UnrenderedBody' else unreal.Vector(1200,1000,300)
    target.set_actor_location(pos,False,False)
    entry=unreal.HodgeAbilitySet_Definition();entry.import_text('(Definition="/Script/Hodgepodge.HodgeAbilityDefinition\''+d.get_path_name()+'\'",AbilityLevel=1)');aset.set_editor_property('GrantedAbilityDefinitions',[entry])
    state['grant']=manager.equip_item(grant);assert state['grant']
    state.update({'phase':'grant','d':d,'name':name,'count':count,'start':unreal.GameplayStatics.get_time_seconds(server),'bones':[],'pose_seen':False,'server_active':False})
def tick(delta):
    try:
        now=unreal.GameplayStatics.get_time_seconds(server)
        if state['phase']=='idle':
            if now<state['after']:return
            state['index']+=1
            if state['index']>=len(cases):finish();return
            begin(state['index']);return
        if state['phase']=='grant':
            handles=[h for h in asc(local[0]).get_all_abilities() if asc(local[0]).find_ability_definition(h)==state['d']]
            if not handles:assert now-state['start']<5,'Owner grant replication timed out';return
            assert unreal.HodgeCombatValidationLibrary.queue_ability_action(asc(local[0]),handles[0],False)
            state['client_ga']=lib.call_method('GetGameplayAbilityFromSpecHandle',(asc(local[0]),handles[0]))[0]
            sh=next(h for h in asc(source).get_all_abilities() if asc(source).find_ability_definition(h)==state['d'])
            state['server_ga']=lib.call_method('GetGameplayAbilityFromSpecHandle',(asc(source),sh))[0]
            state.update({'phase':'run','start':now});return
        t=now-state['start'];poses=unreal.HodgeCombatValidationLibrary.inspect_pose_leases(source)
        state['pose_seen']|=poses>0;state['server_active']|=active(state['server_ga'])
        if active(state['server_ga']):
            v=source.mesh.get_socket_location('Bip001LHand');state['bones'].append([v.x,v.y,v.z])
        samples.append({'case':state['name'],'t':t,'server_health':health(target),'owner_health':health(local[1]),'poses':poses,'sessions':unreal.HodgeCombatValidationLibrary.inspect_hit_sessions(source)})
        if t>state['d'].get_duration()+1.3:
            expected=150-25*state['count'];b=state['bones'];motion=max((sum((a[i]-b[0][i])**2 for i in range(3))**.5 for a in b),default=0)
            report={'case':state['name'],'expected':expected,'server_health':health(target),'client_health':health(local[1]),'server_active_after':active(state['server_ga']),'client_active_after':active(state['client_ga']),'pose_seen':state['pose_seen'],'server_executed':state['server_active'],'bone_motion_cm':motion,'sessions_after':unreal.HodgeCombatValidationLibrary.inspect_hit_sessions(source),'poses_after':poses}
            assert abs(report['server_health']-expected)<.01 and abs(report['client_health']-expected)<.01,report
            assert not report['server_active_after'] and not report['client_active_after'] and report['pose_seen'] and report['server_executed'] and not poses and not report['sessions_after'],report
            if state['name']=='UnrenderedBody':assert motion>1,report
            reports.append(report);manager.unequip_item(state['grant']);state['grant']=None;state['phase']='idle';state['after']=now+.7
    except Exception:finish(traceback.format_exc())
callback=unreal.register_slate_post_tick_callback(tick)
print('Started dedicated-server PIE: unrendered hand bones and client-predicted multi-hit damage at 100ms delay.')
