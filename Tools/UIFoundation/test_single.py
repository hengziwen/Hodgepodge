import unreal,json,traceback
from pathlib import Path
w=unreal.EditorLevelLibrary.get_pie_worlds(True)[0];pc=unreal.GameplayStatics.get_player_controller(w,0)
results=[];state={'step':0,'start':unreal.GameplayStatics.get_time_seconds(w),'due':0,'initial_max':0}
commands=['CancelAsync','Damage','Level','Escape','Modal','Close','Escape','Deactivate','Activate','RebuildRoot','FailAsync']
values=[0,25,2,0,0,0,0,0,0,0,0]
def ui():return json.loads(unreal.HodgeUIAuthoringLibrary.inspect_player_ui(pc))
def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    Path('E:/Project/Git/Hodgepodge/Saved/UIFoundation/single-results.json').write_text(json.dumps({'error':error,'results':results},ensure_ascii=False,indent=2),encoding='utf-8')
def tick(delta):
    try:
        now=unreal.GameplayStatics.get_time_seconds(w)
        if now<state['due']:return
        d=ui();i=state['step']
        if d.get('entries')==2 and d.get('vitals_ready'):
            assert abs(d['display_percent']-d['health']/d['max_health'])<.0001,d
            assert 'HP' in d['display_text'],d
        if i==0:
            assert d['game_count']==1 and d['entries']==2 and d['health']==150 and d['abilities']==[True,True],d
            state['initial_max']=d['max_health']
        elif i==1:assert d['pending']==0 and d['game_input'] and d['menu_count']==0,d
        elif i==2:assert abs(d['health']-125)<.01,d
        elif i==3:assert d['max_health']>state['initial_max'] and d['health']<d['max_health'],d
        elif i==4:assert d['menu_active'] and not d['game_input'] and d['abilities']==[False,False],d
        elif i==5:assert d['modal_active'] and d['menu_active'] and not d['game_input'],d
        elif i==6:assert not d['modal_active'] and d['menu_active'] and not d['game_input'],d
        elif i==7:assert not d['menu_active'] and d['game_input'],d
        elif i==8:assert d['game_count']==0 and d['entries']==0,d
        elif i in [9,10]:assert d['game_count']==1 and d['entries']==2 and d['vitals_ready'],d
        elif i==11:
            assert d['pending']==0 and d['game_input'] and not d['menu_active'],d
            results.append({'case':'FailAsync','ui':d});finish();return
        results.append({'case':'initial' if i==0 else commands[i-1],'ui':d})
        assert unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,commands[i],values[i])
        state['step']+=1;state['due']=now+1
    except Exception:finish(traceback.format_exc())
Path(unreal.Paths.project_saved_dir(),'UIFoundation','single-results.json').write_text('{"error":"RUNNING"}',encoding='utf-8')
callback=unreal.register_slate_post_tick_callback(tick)
print('Started standalone UI lifecycle, real attribute, native Escape routing and modal tests.')
