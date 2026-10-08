import unreal,json,traceback
from pathlib import Path
worlds=unreal.EditorLevelLibrary.get_pie_worlds(True);server=next(w for w in worlds if unreal.GameplayStatics.get_game_mode(w));client=next(w for w in worlds if w!=server)
pc=unreal.GameplayStatics.get_player_controller(client,0);owner=pc.get_controlled_pawn();pid=owner.get_editor_property('player_state').get_editor_property('PlayerId')
remote=next(h for h in unreal.GameplayStatics.get_all_actors_of_class(server,owner.get_class()) if h.get_editor_property('player_state') and h.get_editor_property('player_state').get_editor_property('PlayerId')==pid)
sub=unreal.get_default_object(unreal.load_class(None,'/Script/Engine.SubsystemBlueprintLibrary')).call_method('GetLocalPlayerSubSystemFromPlayerController',(pc,unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
attack=unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack');move=unreal.load_asset('/Game/Main/Input/InputAction/IA_Move')
lib=unreal.get_default_object(unreal.load_class(None,'/Script/GameplayAbilities.AbilitySystemBlueprintLibrary'))
d=unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_1');m=d.get_editor_property('ExecutionConfig').get_editor_property('Montage')
def active(h):
    a=h.get_editor_property('player_state').get_hodge_ability_system_component();handle=next(x for x in a.get_all_abilities() if a.find_ability_definition(x)==d);ga=lib.call_method('GetGameplayAbilityFromSpecHandle',(a,handle))[0]
    return bool(lib.call_method('IsGameplayAbilityActive',(ga,))),ga
state={'start':unreal.GameplayStatics.get_time_seconds(server),'pressed':False,'seen_owner':False,'seen_server':False,'moving':False,'server_end':None,'owner_end':None};rows=[]
def tick(delta):
    try:
        now=unreal.GameplayStatics.get_time_seconds(server);t=now-state['start'];oa,og=active(owner);sa,sg=active(remote)
        op=owner.mesh.get_anim_instance().montage_get_position(m) if oa else 0;sp=remote.mesh.get_anim_instance().montage_get_position(m) if sa else 0
        if not state['pressed']:sub.inject_input_vector_for_action(attack,unreal.Vector(1,0,0),[],[]);state['pressed']=True
        if op>=.45:state['moving']=True
        if state['moving'] and (oa or sa):sub.inject_input_vector_for_action(move,unreal.Vector(0,1,0),[],[])
        if oa:state['seen_owner']=True
        if sa:state['seen_server']=True
        if state['seen_owner'] and not oa and state['owner_end'] is None:state['owner_end']=t
        if state['seen_server'] and not sa and state['server_end'] is None:state['server_end']=t
        rows.append({'t':t,'owner_active':oa,'server_active':sa,'owner_position':op,'server_position':sp,'owner_windows':unreal.HodgeCombatValidationLibrary.inspect_ability_windows(og).export_text(),'server_windows':unreal.HodgeCombatValidationLibrary.inspect_ability_windows(sg).export_text()})
        if t>2:
            assert state['seen_owner'] and state['seen_server'] and state['moving'],state
            assert state['server_end'] is not None and state['server_end']<1.5,state
            assert state['owner_end'] is not None and state['owner_end']<1.8,state
            assert all(x['server_active'] for x in rows if x['server_position']>.15 and x['server_position']<.59),'Canceled before authorized window'
            finish(None)
    except Exception:finish(traceback.format_exc())
def finish(error):
    unreal.unregister_slate_post_tick_callback(callback)
    (Path(unreal.Paths.project_saved_dir())/'ClientComboFix'/'move-cancel.json').write_text(json.dumps({'error':error,'state':state,'samples':rows},ensure_ascii=False,indent=2),encoding='utf-8')
output=Path(unreal.Paths.project_saved_dir())/'ClientComboFix'/'move-cancel.json'
output.parent.mkdir(parents=True,exist_ok=True)
output.write_text('{"error":"RUNNING"}',encoding='utf-8')
callback=unreal.register_slate_post_tick_callback(tick)
print('Testing real client movement intent before the recovery-cancel window.')
