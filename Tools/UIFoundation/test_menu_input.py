import unreal,json,traceback
from pathlib import Path
worlds=unreal.EditorLevelLibrary.get_pie_worlds(True);server=next(w for w in worlds if unreal.GameplayStatics.get_game_mode(w));client=next((w for w in worlds if w!=server),server)
pc=unreal.GameplayStatics.get_player_controller(client,0);pawn=pc.get_controlled_pawn();asc=pc.player_state.get_hodge_ability_system_component()
sub=unreal.get_default_object(unreal.load_class(None,'/Script/Engine.SubsystemBlueprintLibrary')).call_method('GetLocalPlayerSubSystemFromPlayerController',(pc,unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
attack=unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack');move=unreal.load_asset('/Game/Main/Input/InputAction/IA_Move')
lib=unreal.get_default_object(unreal.load_class(None,'/Script/GameplayAbilities.AbilitySystemBlueprintLibrary'))
state={'step':0,'due':0};rows=[]
def active():
    return [h for h in asc.get_all_abilities() if asc.find_ability_definition(h) and lib.call_method('IsGameplayAbilityActive',(lib.call_method('GetGameplayAbilityFromSpecHandle',(asc,h))[0],))]
def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    Path(unreal.Paths.project_saved_dir(),'UIFoundation','menu-input-results.json').write_text(json.dumps({'error':error,'results':rows},indent=2),encoding='utf-8')
def tick(delta):
    try:
        now=unreal.GameplayStatics.get_time_seconds(client);d=json.loads(unreal.HodgeUIAuthoringLibrary.inspect_player_ui(pc));i=state['step']
        if i==0:
            assert not active(),active();unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'Escape');state['step']=1;state['due']=now+1
        elif i==1:
            if now<state['due']:return
            assert d['menu_active'] and not d['game_input'],d
            state['location']=pawn.get_actor_location();state['step']=2;state['due']=now+1
        elif i==2:
            sub.inject_input_vector_for_action(attack,unreal.Vector(1,0,0),[],[]);sub.inject_input_vector_for_action(move,unreal.Vector(0,1,0),[],[])
            assert not active() and not d['move_intent'],d
            assert (pawn.get_actor_location()-state['location']).length()<2,d
            if now<state['due']:return
            rows.append({'case':'menu-blocks-enhanced-attack-and-move','ui':d});unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'Escape');state['step']=3;state['due']=now+1
        elif i==3:
            if now<state['due']:return
            assert d['game_input'] and not d['menu_active'] and not active() and not d['move_intent'],d
            rows.append({'case':'close-no-buffer-replay','ui':d});finish()
    except Exception:finish(traceback.format_exc())
Path(unreal.Paths.project_saved_dir(),'UIFoundation','menu-input-results.json').write_text('{"error":"RUNNING"}',encoding='utf-8')
callback=unreal.register_slate_post_tick_callback(tick)
print('Injecting real EnhancedInput attack/movement while a menu blocks gameplay, then checking no replay.')
