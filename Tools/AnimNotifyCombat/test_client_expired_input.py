import unreal,json,traceback
from pathlib import Path
worlds=unreal.EditorLevelLibrary.get_pie_worlds(True);server=next(w for w in worlds if unreal.GameplayStatics.get_game_mode(w));client=next(w for w in worlds if w!=server)
pc=unreal.GameplayStatics.get_player_controller(client,0);owner=pc.get_controlled_pawn();pid=owner.get_editor_property('player_state').get_editor_property('PlayerId')
remote=next(h for h in unreal.GameplayStatics.get_all_actors_of_class(server,owner.get_class()) if h.get_editor_property('player_state') and h.get_editor_property('player_state').get_editor_property('PlayerId')==pid)
sub=unreal.get_default_object(unreal.load_class(None,'/Script/Engine.SubsystemBlueprintLibrary')).call_method('GetLocalPlayerSubSystemFromPlayerController',(pc,unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
lib=unreal.get_default_object(unreal.load_class(None,'/Script/GameplayAbilities.AbilitySystemBlueprintLibrary'));attack=unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack')
state={'start':unreal.GameplayStatics.get_time_seconds(client),'first':False,'early':False,'owner':[],'server':[]}
def tick(delta):
    try:
        t=unreal.GameplayStatics.get_time_seconds(client)-state['start']
        for key,pawn in [('owner',owner),('server',remote)]:
            asc=pawn.get_editor_property('player_state').get_hodge_ability_system_component()
            for h in asc.get_all_abilities():
                d=asc.find_ability_definition(h)
                if not d or d.get_editor_property('ExecutionRoute')!=unreal.HodgeAbilityExecutionRoute.COMBO_COORDINATED:continue
                ga=lib.call_method('GetGameplayAbilityFromSpecHandle',(asc,h))[0]
                if lib.call_method('IsGameplayAbilityActive',(ga,)):
                    m=d.get_editor_property('ExecutionConfig').get_editor_property('Montage')
                    assert m.get_name()=='AM_Attack01_Montage',(key,m.get_name())
                    if not state[key]:state[key].append(m.get_name())
                    if key=='owner' and not state['early'] and pawn.mesh.get_anim_instance().montage_get_position(m)>=.1:
                        sub.inject_input_vector_for_action(attack,unreal.Vector(1,0,0),[],[]);state['early']=True
        if not state['first']:sub.inject_input_vector_for_action(attack,unreal.Vector(1,0,0),[],[]);state['first']=True
        if t>2:
            assert state['early'] and state['owner'] and state['server'],state
            finish(None)
    except Exception:finish(traceback.format_exc())
def finish(error):
    unreal.unregister_slate_post_tick_callback(callback)
    (Path(unreal.Paths.project_saved_dir())/'ClientComboFix'/'early-input.json').write_text(json.dumps({'error':error,'state':state},ensure_ascii=False,indent=2),encoding='utf-8')
output=Path(unreal.Paths.project_saved_dir())/'ClientComboFix'/'early-input.json'
output.parent.mkdir(parents=True,exist_ok=True)
output.write_text('{"error":"RUNNING"}',encoding='utf-8')
callback=unreal.register_slate_post_tick_callback(tick)
print('Testing that an input expiring before the first window cannot advance the combo.')
