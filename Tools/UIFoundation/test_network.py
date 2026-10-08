import unreal,json,traceback
from pathlib import Path
worlds=unreal.EditorLevelLibrary.get_pie_worlds(True);server=next(w for w in worlds if unreal.GameplayStatics.get_game_mode(w));client=next(w for w in worlds if w!=server)
host=unreal.GameplayStatics.get_player_controller(server,0);owner=unreal.GameplayStatics.get_player_controller(client,0)
pid=owner.player_state.get_editor_property('PlayerId')
remote=next(pc for pc in unreal.GameplayStatics.get_all_actors_of_class(server,unreal.PlayerController) if pc.player_state and pc.player_state.get_editor_property('PlayerId')==pid)
def view(pc):return json.loads(unreal.HodgeUIAuthoringLibrary.inspect_player_ui(pc))
def sample():return {'host':view(host),'client':view(owner)}
actions=[(remote,'Damage',30),(owner,'Escape',0),(owner,'Modal',0),(owner,'Close',0),(owner,'Escape',0),(host,'Escape',0),(host,'Escape',0),(owner,'Deactivate',0),(owner,'Activate',0),(owner,'RebuildRoot',0)]
records=[];state={'step':0,'due':0}
def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    Path('E:/Project/Git/Hodgepodge/Saved/UIFoundation/network-results.json').write_text(json.dumps({'error':error,'results':records},ensure_ascii=False,indent=2),encoding='utf-8')
def tick(delta):
    try:
        now=unreal.GameplayStatics.get_time_seconds(server)
        if now<state['due']:return
        d=sample();i=state['step'];h=d['host'];c=d['client']
        if i==0:assert h['health']==c['health']==150 and h['entries']==c['entries']==2,d
        if i>=1 and i!=8:assert c['health']==120 and h['health']==150,d
        if i==2:assert c['menu_active'] and not c['game_input'] and h['game_input'],d
        if i==3:assert c['modal_active'] and not c['game_input'] and h['game_input'],d
        if i==4:assert not c['modal_active'] and c['menu_active'] and h['game_input'],d
        if i==5:assert c['game_input'] and h['game_input'],d
        if i==6:assert h['menu_active'] and not h['game_input'] and c['game_input'],d
        if i==7:assert h['game_input'] and c['game_input'],d
        if i==8:assert c['game_count']==0 and h['game_count']==1,d
        if i in [9,10]:assert c['game_count']==h['game_count']==1 and c['entries']==h['entries']==2,d
        records.append({'case':'initial' if i==0 else actions[i-1][1],'ui':d})
        if i==len(actions):finish();return
        pc,action,value=actions[i];assert unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,action,value)
        state['step']+=1;state['due']=now+1.2
    except Exception:finish(traceback.format_exc())
Path(unreal.Paths.project_saved_dir(),'UIFoundation','network-results.json').write_text('{"error":"RUNNING"}',encoding='utf-8')
callback=unreal.register_slate_post_tick_callback(tick)
print('Started two-world own-stat and menu isolation tests with real native viewport input.')
