"""Real-world HUD data, button delegate/Blueprint and lifecycle regression."""
import unreal, json, traceback
from pathlib import Path

worlds=unreal.EditorLevelLibrary.get_pie_worlds(True)
server=next(w for w in worlds if unreal.GameplayStatics.get_game_mode(w))
client=next((w for w in worlds if w!=server),server)
owner=unreal.GameplayStatics.get_player_controller(client,0)
host=unreal.GameplayStatics.get_player_controller(server,0)
player_id=owner.player_state.get_editor_property('PlayerId')
authority=next(p for p in unreal.GameplayStatics.get_all_actors_of_class(server,unreal.PlayerController)
               if p.player_state and p.player_state.get_editor_property('PlayerId')==player_id)
network=client!=server
output=Path(unreal.Paths.project_saved_dir(),'HUDReference','network.json' if network else 'single.json')
actions=[(authority,'Damage',25),(authority,'Level',2),(owner,'EndButton',0),(owner,'MenuReturn',0),
         (owner,'EndButton',0),(owner,'RoutedEscape',0),(owner,'EndButton',0),(owner,'Deactivate',0),
         (owner,'Activate',0),(owner,'RebuildRoot',0),(owner,'Capture',0)]
records=[]
state={'step':0,'due':0,'start':unreal.GameplayStatics.get_time_seconds(server)}
def view(pc):return json.loads(unreal.HodgeUIAuthoringLibrary.inspect_player_ui(pc))
def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    output.write_text(json.dumps({'error':error,'network':network,'checks':len(records),'results':records},ensure_ascii=False,indent=2),encoding='utf-8')
def tick(delta):
    try:
        now=unreal.GameplayStatics.get_time_seconds(server)
        if now-state['start']>45:raise AssertionError('HUD verification timed out')
        if now<state['due']:return
        d=view(owner);h=view(host);i=state['step']
        if not d.get('vitals_ready') and i==0:return
        if i!=8:
            assert d['entries']==1 and d['game_count']==1,d
            assert abs(d['display_percent']-d['health']/d['max_health'])<.0001,d
            assert d['display_text'].replace(',','')==f"{d['health']:.0f}/{d['max_health']:.0f}",d
            assert d['display_level']=='Lv. '+str(owner.player_state.get_character_level()),d
            if i>0:
                assert abs(d['vitals_center_x']-.5)<.015 and d['vitals_center_y']>.92,d
                assert d['end_center_x']>.85 and d['end_center_y']<.15,d
        if i==0:
            state['initial_health']=d['health'];state['initial_max']=d['max_health'];state['host_health']=h['health']
            assert d['game_input'] and not d['menu_active'],d
        if i==1:assert abs(d['health']-(state['initial_health']-25))<.01,d
        if i==2:assert d['max_health']>state['initial_max'] and d['display_level']=='Lv. 2',d
        if i in [3,5,7]:assert d['menu_active'] and not d['game_input'],d
        if i in [4,6,9,10,11]:assert not d['menu_active'] and d['game_input'],d
        if i==8:assert d['game_count']==0 and d['entries']==0 and not d['menu_active'],d
        if network:
            assert h['health']==state['host_health'] and h['game_input'] and not h['menu_active'],h
        records.append({'case':'initial' if i==0 else actions[i-1][1],'owner':d,'host':h if network else None})
        if i==len(actions):finish();return
        pc,action,value=actions[i]
        assert unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,action,value)
        state['step']+=1;state['due']=now+1.1
    except Exception:finish(traceback.format_exc())
output.write_text('{"error":"RUNNING"}',encoding='utf-8')
callback=unreal.register_slate_post_tick_callback(tick)
print('HUD verification started; wait for final JSON, not this message.')
