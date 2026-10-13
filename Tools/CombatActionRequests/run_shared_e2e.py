import unreal,json,time,traceback
from pathlib import Path
worlds=unreal.EditorLevelLibrary.get_pie_worlds(False)
w=next(w for w in worlds if unreal.SystemLibrary.is_server(w));clients=[v for v in worlds if v!=w]
heroes=unreal.GameplayStatics.get_all_actors_of_class(w,unreal.HodgeHeroCharacter)
local=None
if clients:
 local=next(a for a in unreal.GameplayStatics.get_all_actors_of_class(clients[0],unreal.HodgeHeroCharacter) if a.get_local_role()==unreal.NetRole.ROLE_AUTONOMOUS_PROXY)
 pid=local.get_editor_property('PlayerState').get_editor_property('PlayerId')
 hero=next(a for a in heroes if a.get_editor_property('PlayerState').get_editor_property('PlayerId')==pid)
else:hero=heroes[0];local=hero
pid=hero.get_editor_property('PlayerState').get_editor_property('PlayerId')
for world in worlds:
 for a in unreal.GameplayStatics.get_all_actors_of_class(world,unreal.HodgeHeroCharacter):
  target=unreal.Vector(0,0,100) if a.get_editor_property('PlayerState').get_editor_property('PlayerId')==pid else unreal.Vector(-2400,2400,100)
  a.character_movement.stop_movement_immediately();a.set_actor_location(target,False,True);a.set_actor_rotation(unreal.Rotator(0,0,0),False)
for a in unreal.GameplayStatics.get_all_actors_of_class(w,unreal.HodgeEnemyCharacter):a.set_actor_location(unreal.Vector(2400,2400,100),False,True)
ROOT='/Game/Main/Character/Enemies/Guardian/Validation'
enemy=unreal.HodgeCombatValidationLibrary.spawn_hit_reaction_validation_actor(w,unreal.load_asset(ROOT+'/BP_EnemyCombo').generated_class(),unreal.Vector(850,0,100))
assert enemy
enemy.set_actor_rotation(unreal.Rotator(0,180,0),False)
attack=unreal.load_asset(ROOT+'/DA_ComboAttack').get_editor_property('ExecutionConfig').get_editor_property('Montage')
player_defs=[unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_'+str(n)) for n in [1,2]]
player_montages=[a.get_editor_property('ExecutionConfig').get_editor_property('Montage') for a in player_defs]
sub_lib=unreal.get_default_object(unreal.load_class(None,'/Script/Engine.SubsystemBlueprintLibrary'))
sub=sub_lib.call_method('GetLocalPlayerSubSystemFromPlayerController',(local.get_controller(),unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
ia=unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack')
bridge=unreal.get_default_object(unreal.load_class(None,'/Script/GameplayAbilities.AbilitySystemBlueprintLibrary'))
results=[];samples=[];state={'start':unreal.GameplayStatics.get_time_seconds(w),'wall':time.monotonic(),'phase':'ai','instances':set(),'ai_node':False,'player_local':set(),'player_server':set(),'remote_ai':False,'second':False}
name='shared-network' if clients else 'shared-single'
health=lambda a:a.get_component_by_class(unreal.HodgeHealthComponent).get_health()
state['hero_hp']=health(hero)
def check(case,passed,**detail):
 results.append({'case':case,'passed':bool(passed),**detail});assert passed,(case,detail)
def finish(error=None):
 unreal.unregister_slate_post_tick_callback(state['callback'])
 if unreal.SystemLibrary.is_valid(enemy):enemy.destroy_actor()
 r={'name':name,'error':error,'results':results,'samples':samples,'elapsed_s':time.monotonic()-state['wall']}
 Path('E:/Project/Git/Hodgepodge/Saved/CombatActionRequests/'+name+'.json').write_text(json.dumps(r,ensure_ascii=False,indent=2),encoding='utf-8')
 print(json.dumps({'name':name,'passed':sum(r['passed'] for r in results),'error':error}))
def tick(delta):
 try:
  now=unreal.GameplayStatics.get_time_seconds(w);elapsed=now-state['start']
  if time.monotonic()-state['wall']>75:raise RuntimeError('timeout '+state['phase'])
  if state['phase']=='ai':
   info=unreal.HodgeCombatValidationLibrary.inspect_montage_state(enemy,attack)
   if info.instance_id>=0 and info.weight>.02:state['instances'].add(info.instance_id)
   combat=enemy.get_component_by_class(unreal.HodgeCombatComponentBase)
   state['ai_node'] |= str(combat.get_current_combo_tag().get_editor_property('TagName'))=='Combo.Light.01'
   if clients:
    remotes=[a for a in unreal.GameplayStatics.get_all_actors_of_class(clients[0],unreal.HodgeEnemyCharacter) if a.get_class()==enemy.get_class()]
    if remotes:state['remote_ai'] |= unreal.HodgeCombatValidationLibrary.inspect_montage_state(remotes[0],attack).weight>.1
   samples.append({'t':now,'phase':'ai','hero_hp':health(hero),'montage':info.instance_id,'weight':info.weight})
   if len(state['instances'])>=2 and health(hero)<state['hero_hp']:
    check('ai_combo_ready',enemy.get_enemy_init_component().is_gameplay_ready())
    check('ai_intent_without_input_mapping',len(enemy.get_editor_property('EnemyPawnData').get_editor_property('ComboDefinition').get_editor_property('InputBindings'))==0)
    check('ai_coordinator_authorization',state['ai_node'])
    check('ai_task_completes_and_repeats',True,instances=sorted(state['instances']))
    check('ai_combo_damage',True,before=state['hero_hp'],after=health(hero))
    if clients:check('remote_ai_combo_animation',state['remote_ai'])
    tags=unreal.GameplayTagContainer();tags.import_text('(GameplayTags=((TagName="State.Combat.HitReaction.Controlled")))')
    assert bridge.call_method('AddLooseGameplayTags',(enemy,tags,False))
    pos=enemy.get_actor_location()+unreal.Vector(-120,0,0)
    for world in worlds:
     for p in unreal.GameplayStatics.get_all_actors_of_class(world,unreal.HodgeHeroCharacter):
      if p.get_editor_property('PlayerState').get_editor_property('PlayerId')==pid:p.character_movement.stop_movement_immediately();p.set_actor_location(pos,False,True);p.set_actor_rotation(unreal.Rotator(0,0,0),False)
    state.update({'phase':'prepare_player','start':now});return
   assert elapsed<18,('ai did not complete',info,health(hero));return
  if state['phase']=='prepare_player':
   if elapsed<.8:return
   state['enemy_hp']=health(enemy);sub.inject_input_vector_for_action(ia,unreal.Vector(1,0,0),[],[])
   state.update({'phase':'player','start':now});return
  if state['phase']=='player':
   if elapsed>.4 and not state['second']:
    sub.inject_input_vector_for_action(ia,unreal.Vector(1,0,0),[],[]);state['second']=True
   for index,montage in enumerate(player_montages):
    li=unreal.HodgeCombatValidationLibrary.inspect_montage_state(local,montage)
    si=unreal.HodgeCombatValidationLibrary.inspect_montage_state(hero,montage)
    if li.weight>.02:state['player_local'].add(index+1)
    if si.weight>.02:state['player_server'].add(index+1)
   samples.append({'t':now,'phase':'player','hp':health(enemy),'local':sorted(state['player_local']),'server':sorted(state['player_server']),'node':local.get_component_by_class(unreal.HodgeCombatComponentBase).get_current_combo_tag().export_text(),'first_position':unreal.HodgeCombatValidationLibrary.inspect_montage_state(local,player_montages[0]).position})
   if elapsed<3:return
   check('real_player_input_enters_shared_combo',1 in state['player_local'])
   check('player_queued_followup_starts',2 in state['player_local'])
   check('server_authorizes_player_combo',state['player_server']=={1,2})
   check('player_combo_server_damage',health(enemy)<state['enemy_hp'],before=state['enemy_hp'],after=health(enemy))
   if clients:check('remote_owner_local_prediction_path',local.get_local_role()==unreal.NetRole.ROLE_AUTONOMOUS_PROXY)
   finish()
 except Exception:finish(traceback.format_exc())
state['callback']=unreal.register_slate_post_tick_callback(tick)
print('Started '+name)
