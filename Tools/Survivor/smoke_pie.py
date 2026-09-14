import unreal,time,json,traceback
from pathlib import Path

_survivor_report=Path('D:/CodexUE55Lab/survivor-smoke.json')
_survivor_report.write_text('RUNNING')
_survivor_begin=time.monotonic()
_survivor_stage=0
_survivor_evidence=[]
_survivor_t=0
_survivor_start=None

def survivor_tick(dt):
    global _survivor_stage,_survivor_t,_survivor_start,_survivor_handle
    try:
        if time.monotonic()-_survivor_begin>70:raise RuntimeError('PIE test timeout at stage '+str(_survivor_stage))
        world=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world()
        if not world:return
        mode=unreal.GameplayStatics.get_game_mode(world)
        if not isinstance(mode,unreal.HodgeSurvivorMode):return
        hero=mode.hero
        if not hero:return
        if _survivor_stage==0:
            assert isinstance(mode.interface,unreal.HodgeSurvivorHUD),'HUD missing: '+str(mode.interface)+' class='+str(mode.get_editor_property('interface_class'))
            assert mode.get_enemy_count()>0,'enemy spawn missing'
            _survivor_start=hero.get_actor_location()
            _survivor_t=mode.elapsed
            hero.set_editor_property('invulnerability',100)
            _survivor_stage=1
        elif _survivor_stage==1:
            hero.add_movement_input(unreal.Vector(0,1,0),1,False)
            if mode.elapsed-_survivor_t>1:
                assert hero.get_actor_location().distance(_survivor_start)>100,'movement failed'
                hero.dash();assert hero.dash_cooldown>0,'dash failed'
                hero.set_editor_property('invulnerability',100)
                _survivor_evidence.append('pawn, camera, HUD, movement and dash initialized')
                _survivor_stage=2
        elif _survivor_stage==2:
            if mode.kills>0:
                assert mode.get_orb_count()>0,'kill did not drop XP'
                _survivor_evidence.append('natural enemy pursuit -> automatic attack -> kill -> XP drop')
                for actor in unreal.GameplayStatics.get_all_actors_of_class(world,unreal.StaticMeshActor):
                    if actor.static_mesh_component.static_mesh and actor.static_mesh_component.static_mesh.get_name()=='Sphere' and actor.get_actor_scale3d().x<.4 and actor.get_life_span()==0:
                        hero.set_actor_location(actor.get_actor_location()+unreal.Vector(0,0,100),False,False)
                        break
                _survivor_t=mode.elapsed;_survivor_stage=3
        elif _survivor_stage==3:
            if mode.elapsed-_survivor_t>.8:
                assert mode.experience>0,'XP pickup failed'
                _survivor_evidence.append('physical XP pickup increments experience')
                mode.grant_experience(mode.next_level_xp)
                assert mode.state==unreal.HodgeSurvivorState.UPGRADE,'upgrade not shown'
                _survivor_t=time.monotonic();_survivor_start=mode.elapsed
                unreal.SystemLibrary.execute_console_command(world,'Shot SHOWUI')
                _survivor_stage=4
        elif _survivor_stage==4:
            if time.monotonic()-_survivor_t>.8:
                assert mode.elapsed==_survivor_start,'upgrade did not freeze combat'
                before=(mode.damage,mode.attack_interval,mode.max_health,mode.pickup_radius,mode.pulse_level,hero.character_movement.max_walk_speed)
                mode.interface.call_method('ChooseOne')
                after=(mode.damage,mode.attack_interval,mode.max_health,mode.pickup_radius,mode.pulse_level,hero.character_movement.max_walk_speed)
                assert before!=after and mode.state==unreal.HodgeSurvivorState.PLAYING,'choice not applied'
                _survivor_evidence.append('upgrade freezes game; UI callback applies one random upgrade')
                mode.toggle_pause();_survivor_t=time.monotonic();_survivor_start=mode.elapsed;_survivor_stage=5
        elif _survivor_stage==5:
            if time.monotonic()-_survivor_t>.5:
                assert mode.elapsed==_survivor_start,'pause failed'
                mode.toggle_pause();hero.set_editor_property('invulnerability',0)
                mode.damage_hero(10000)
                assert mode.state==unreal.HodgeSurvivorState.DEFEAT,'defeat failed'
                _survivor_evidence.append('pause/resume and defeat panel')
                mode.interface.call_method('Retry');_survivor_stage=6
        elif _survivor_stage==6:
            if mode.elapsed<2 and mode.health==100 and mode.level==1:
                _survivor_evidence.append('UI restart travels to a fresh arena and resets run state')
                mode.set_editor_property('elapsed',mode.round_duration-1)
                _survivor_stage=7
        elif _survivor_stage==7:
            if mode.state==unreal.HodgeSurvivorState.VICTORY:
                _survivor_evidence.append('round timer reaches victory (elapsed time advanced only in PIE instance)')
                _survivor_report.write_text(json.dumps({'status':'PASS','checks':_survivor_evidence},ensure_ascii=False,indent=2))
                unreal.unregister_slate_post_tick_callback(_survivor_handle)
    except Exception:
        _survivor_report.write_text(json.dumps({'status':'FAIL','stage':_survivor_stage,'checks':_survivor_evidence,'error':traceback.format_exc()},ensure_ascii=False,indent=2))
        unreal.unregister_slate_post_tick_callback(_survivor_handle)

_survivor_handle=unreal.register_slate_post_tick_callback(survivor_tick)
print('Registered PIE survival test')