import unreal,json,traceback
from pathlib import Path
worlds=unreal.EditorLevelLibrary.get_pie_worlds(True);server=next(w for w in worlds if unreal.GameplayStatics.get_game_mode(w));w=next((w for w in worlds if w!=server),server)
pc=unreal.GameplayStatics.get_player_controller(w,0);state={'step':0,'due':0};rows=[]
def finish(error=None):
 unreal.unregister_slate_post_tick_callback(callback)
 Path(unreal.Paths.project_saved_dir(),'UIFoundation','owned-modal-results.json').write_text(json.dumps({'error':error,'results':rows},indent=2),encoding='utf-8')
def tick(delta):
 try:
  now=unreal.GameplayStatics.get_time_seconds(w)
  if now<state['due']:return
  d=json.loads(unreal.HodgeUIAuthoringLibrary.inspect_player_ui(pc));i=state['step']
  if i==0:unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'Escape')
  elif i==1:
   assert d['menu_active'],d
   unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'ConfirmMenu')
  elif i==2:
   assert d['modal_active'] and d['menu_active'],d
   rows.append({'case':'production-modal','ui':d});unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'Deactivate')
  elif i==3:
   assert not d['modal_active'] and not d['menu_active'] and d['game_count']==0,d
   rows.append({'case':'owned-modal-cleared-with-HUD','ui':d});unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'Activate')
  elif i==4:
   assert d['game_input'] and d['entries']==2,d
   rows.append({'case':'reactivate-no-input-lock','ui':d});unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'Escape')
  elif i in [5,8]:
   assert d['menu_active'],d
   unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'ConfirmMenu')
  elif i==6:
   assert d['modal_active'],d
   unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'AcceptDialog')
  elif i==7:
   assert not d['modal_active'] and not d['menu_active'] and d['game_input'],d
   rows.append({'case':'confirmation-accept-returns-game','ui':d});unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'Escape')
  elif i==9:
   assert d['modal_active'],d
   unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'Escape')
  elif i==10:
   assert not d['modal_active'] and d['menu_active'] and not d['game_input'],d
   rows.append({'case':'escape-modal-returns-menu','ui':d});unreal.HodgeUIAuthoringLibrary.queue_ui_action(pc,'Escape')
  elif i==11:
   assert not d['menu_active'] and d['game_input'],d
   rows.append({'case':'escape-menu-returns-game','ui':d});finish();return
  state['step']+=1;state['due']=now+.8
 except Exception:finish(traceback.format_exc())
Path(unreal.Paths.project_saved_dir(),'UIFoundation','owned-modal-results.json').write_text('{"error":"RUNNING"}',encoding='utf-8')
callback=unreal.register_slate_post_tick_callback(tick)
print('Testing production modal ownership during HUD unload and reactivation.')
