"""Exercise natural BT -> shared request -> GAS attacks for both Guardian variants."""
import unreal,json,time,traceback
from pathlib import Path
worlds=unreal.EditorLevelLibrary.get_pie_worlds(True)
server=next(w for w in worlds if unreal.GameplayStatics.get_game_mode(w))
clients=[w for w in worlds if w!=server]
enemies=sorted(unreal.GameplayStatics.get_all_actors_of_class(server,unreal.HodgeEnemyCharacter),key=lambda e:'Equipped' in e.get_name())
assert len(enemies)==2
heroes=unreal.GameplayStatics.get_all_actors_of_class(server,unreal.HodgeHeroCharacter)
assert heroes
hero=heroes[0]
attack=unreal.load_asset('/Game/Main/Character/Enemies/Guardian/Anim/AM_Attack')
mode=globals().get('AI_REGRESSION_MODE','network' if clients else 'single')
output=Path(unreal.Paths.project_saved_dir(),'AIRequestRegression',mode+'-attacks.json')
state={'phase':0,'start':time.monotonic(),'phase_start':None,'initial_health':None,'instances':[set(),set()],'instance_times':[[],[]],'remote_seen':[False,False]}
records=[]
def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    output.write_text(json.dumps({'error':error,'mode':mode,'results':records,'montage_instances':[sorted(s) for s in state['instances']]},ensure_ascii=False,indent=2),encoding='utf-8')
def tick(delta):
    try:
        now=time.monotonic()
        if now-state['start']>60:raise AssertionError('Attack regression timed out')
        index=state['phase'];enemy=enemies[index]
        if state['phase_start'] is None:
            if not enemy.get_enemy_init_component().is_gameplay_ready() or not hero.get_component_by_class(unreal.HodgeHealthComponent):return
            player_id=hero.get_editor_property('PlayerState').get_editor_property('PlayerId')
            for world in unreal.EditorLevelLibrary.get_pie_worlds(True):
                for player in unreal.GameplayStatics.get_all_actors_of_class(world,unreal.HodgeHeroCharacter):
                    ps=player.get_editor_property('PlayerState')
                    position=enemy.get_actor_location()+unreal.Vector(-140,0,0) if ps and ps.get_editor_property('PlayerId')==player_id else unreal.Vector(-2600,-2600,100)
                    player.character_movement.stop_movement_immediately()
                    player.set_actor_location(position,False,True)
            state['phase_start']=unreal.GameplayStatics.get_time_seconds(server)
            state['initial_health']=hero.get_component_by_class(unreal.HodgeHealthComponent).get_health()
            return
        elapsed=unreal.GameplayStatics.get_time_seconds(server)-state['phase_start']
        assert elapsed<20,{'actor':enemy.get_name(),'instances':sorted(state['instances'][index])}
        info=unreal.HodgeCombatValidationLibrary.inspect_montage_state(enemy,attack)
        if info.instance_id>=0 and info.instance_id not in state['instances'][index]:
            state['instances'][index].add(info.instance_id);state['instance_times'][index].append(elapsed)
        for client in [w for w in unreal.EditorLevelLibrary.get_pie_worlds(True) if w!=server and unreal.SystemLibrary.is_valid(w)]:
            remote=next((e for e in unreal.GameplayStatics.get_all_actors_of_class(client,unreal.HodgeEnemyCharacter)
                         if e.get_editor_property('EnemyPawnData')==enemy.get_editor_property('EnemyPawnData')),None)
            if remote:state['remote_seen'][index]|=unreal.HodgeCombatValidationLibrary.inspect_montage_state(remote,attack).weight>.02
        health=hero.get_component_by_class(unreal.HodgeHealthComponent).get_health()
        if len(state['instances'][index])>=3 and health<state['initial_health'] and (not clients or state['remote_seen'][index]):
            times=state['instance_times'][index]
            assert all(b-a<8 for a,b in zip(times,times[1:])),times
            records.append({'actor':enemy.get_name(),'passed':True,'attack_instances':len(state['instances'][index]),'instance_start_times':times,'elapsed_game_s':elapsed,'initial_health':state['initial_health'],'health':health,'remote_animation':state['remote_seen'][index] if clients else None})
            if index==1:finish();return
            state['phase']=1;state['phase_start']=None
    except Exception:finish(traceback.format_exc())
output.write_text('{"error":"RUNNING"}',encoding='utf-8')
callback=unreal.register_slate_post_tick_callback(tick)
print('Attack regression started; wait for final JSON')
