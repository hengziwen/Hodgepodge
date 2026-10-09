"""Pause the server before its cancel window and prove the owning client's prediction restores."""
import unreal, builtins, json, traceback
from pathlib import Path
s=builtins.HODGE_REACTION_TEST;w=s['world'];owner=s['source_owner'];server=s['source']
pc=owner.get_controller()
sub=unreal.get_default_object(unreal.load_class(None,'/Script/Engine.SubsystemBlueprintLibrary')).call_method(
 'GetLocalPlayerSubSystemFromPlayerController',(pc,unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
attack=unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack');move=unreal.load_asset('/Game/Main/Input/InputAction/IA_Move')
definition=unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_1')
montage=definition.get_editor_property('ExecutionConfig').get_editor_property('Montage')
lib=s['lib'];inspect=unreal.HodgeCombatValidationLibrary.inspect_montage_state
def ability(p):
 a=p.get_hodge_ability_system_component();h=next(x for x in a.get_all_abilities() if a.find_ability_definition(x)==definition)
 return a,h,lib.call_method('GetGameplayAbilityFromSpecHandle',(a,h))[0]
oa,oh,og=ability(owner);sa,sh,sg=ability(server)
blocked=unreal.GameplayTagContainer();blocked.import_text('(GameplayTags=((TagName="Gameplay.AbilityInputBlocked")))')
state={'start':unreal.GameplayStatics.get_time_seconds(w),'pressed':False,'blocked':False,'initial_id':None,
 'seen_prediction':False,'seen_restore':False};rows=[]
def finish(error=None):
 unreal.unregister_slate_post_tick_callback(callback)
 lib.call_method('RemoveLooseGameplayTags',(server,blocked,False))
 unreal.HodgeCombatValidationLibrary.queue_ability_action(sa,sh,True)
 out=Path(unreal.Paths.project_saved_dir())/'AnimationNetworkReview'/'rejected-move-cancel.json'
 out.write_text(json.dumps({'error':error,'state':state,'samples':rows},ensure_ascii=False,indent=2),encoding='utf-8')
 s['rejection_review_result']=error or 'PASS'
def tick(delta):
 try:
  t=unreal.GameplayStatics.get_time_seconds(w)-state['start']
  o=inspect(owner,montage);a=inspect(server,montage)
  if not state['pressed']:sub.inject_input_vector_for_action(attack,unreal.Vector(1,0,0),[],[]);state['pressed']=True
  if o.instance_id>=0 and state['initial_id'] is None:state['initial_id']=o.instance_id
  if not state['blocked'] and a.position>.18:
   lib.call_method('AddLooseGameplayTags',(server,blocked,False));state['blocked']=True
  if t>.45 and t<2.3:sub.inject_input_vector_for_action(move,unreal.Vector(0,1,0),[],[])
  state['seen_prediction'] |= o.stopped and state['initial_id'] is not None and o.instance_id==state['initial_id']
  state['seen_restore'] |= state['initial_id'] is not None and o.instance_id>state['initial_id'] and not o.stopped
  v=server.character_movement.get_current_acceleration()
  rows.append({'t':t,'owner_position':o.position,'owner_weight':o.weight,'owner_stopped':o.stopped,'owner_id':o.instance_id,
   'server_position':a.position,'server_stopped':a.stopped,'server_acceleration':(v.x*v.x+v.y*v.y)**.5,
   'owner_active':bool(lib.call_method('IsGameplayAbilityActive',(og,))),
   'server_active':bool(lib.call_method('IsGameplayAbilityActive',(sg,)))})
  if t>1.25:
   assert state['blocked'] and state['seen_prediction'] and state['seen_restore'],state
   assert rows[-1]['owner_active'] and rows[-1]['server_active'],rows[-1]
   assert max(x['server_acceleration'] for x in rows)<1.,'Closed server window accepted movement'
   finish()
 except Exception:finish(traceback.format_exc())
callback=unreal.register_slate_post_tick_callback(tick);s['suite_callback']=callback
print('Started rejected predicted cancellation / restoration probe')
