import unreal,json,traceback
from pathlib import Path
worlds=unreal.EditorLevelLibrary.get_pie_worlds(True)
server=next(w for w in worlds if unreal.GameplayStatics.get_game_mode(w))
client=next((w for w in worlds if w!=server),server)
owner=unreal.GameplayStatics.get_player_controller(client,0)
pid=owner.player_state.get_editor_property('PlayerId')
remote=next(pc for pc in unreal.GameplayStatics.get_all_actors_of_class(server,unreal.PlayerController) if pc.player_state and pc.player_state.get_editor_property('PlayerId')==pid)
old=owner.get_controlled_pawn().get_path_name();state={'step':0,'due':0,'start':unreal.GameplayStatics.get_time_seconds(server)};rows=[]
def view():return json.loads(unreal.HodgeUIAuthoringLibrary.inspect_player_ui(owner))
def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    Path(unreal.Paths.project_saved_dir(),'UIFoundation','death-respawn-results.json').write_text(json.dumps({'error':error,'results':rows},indent=2),encoding='utf-8')
def tick(delta):
    try:
        now=unreal.GameplayStatics.get_time_seconds(server)
        assert now-state['start']<25,state
        if now<state['due']:return
        d=view();i=state['step']
        if i==0:
            assert d['health']>0 and d['entries']==2,d
            rows.append({'case':'initial','ui':d});unreal.HodgeUIAuthoringLibrary.queue_ui_action(remote,'Damage',10000);state['step']=1
        elif i==1:
            if d.get('health',1)>0 or d.get('abilities')!=[False,False]:return
            assert d['abilities']==[False,False] and d['root'],d
            rows.append({'case':'death-zero-health','ui':d});state['step']=2
        elif i==2:
            if d['pawn']:return
            assert not d['vitals_ready'] and d['entries']==2 and d['abilities']==[False,False],d
            rows.append({'case':'pawn-destroyed','ui':d});unreal.HodgeUIAuthoringLibrary.queue_ui_action(remote,'Respawn');state['step']=3
        elif i==3:
            if not d['vitals_ready']:return
            identity=(d['pawn'],d['health'],d['max_health'])
            if state.get('stable_identity')!=identity:
                state['stable_identity']=identity;state['stable_since']=now
                rows.append({'case':'rebind-observation','ui':d});return
            if now-state['stable_since']<2:return
            assert d['pawn']!=old and d['health']==d['max_health'] and d['entries']==2,d
            rows.append({'case':'new-pawn-bound','ui':d});unreal.HodgeUIAuthoringLibrary.queue_ui_action(remote,'Damage',25);state['step']=4;state['due']=now+1
        elif i==4:
            assert d['health']==d['max_health']-25 and d['abilities']==[True,True],d
            rows.append({'case':'new-pawn-damage','ui':d});finish()
    except Exception:finish(traceback.format_exc())
Path(unreal.Paths.project_saved_dir(),'UIFoundation','death-respawn-results.json').write_text('{"error":"RUNNING"}',encoding='utf-8')
callback=unreal.register_slate_post_tick_callback(tick)
print('Testing lethal server damage, UI during death, Pawn destruction, server restart and new-Pawn damage.')
