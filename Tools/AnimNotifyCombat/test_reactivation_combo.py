import unreal,builtins,json,traceback
from pathlib import Path
s=builtins.HODGE_SKILL_TEST;w=s['world'];asc=s['source_asc'];target=s['target'];target_asc=s['target_asc'];source=s['source'];hc=s['health']
out=Path('E:/Project/Git/Hodgepodge/Saved/AnimNotifyMigration');lib=unreal.get_default_object(unreal.load_class(None,'/Script/GameplayAbilities.AbilitySystemBlueprintLibrary'))
heal_cls=unreal.load_class(None,'/Game/CodexText/SkillHitVolumes/GE_VolumeLabHeal.GE_VolumeLabHeal_C')
shared=unreal.load_asset('/Game/CodexText/SkillHitVolumes/DA_VolumeSharedExecution');pulse=unreal.load_asset('/Game/CodexText/SkillHitVolumes/DA_VolumeThreePulse')
combat=source.get_component_by_class(unreal.HodgeCombatComponentBase)
entry_tag=unreal.load_asset('/Game/Main/Data/Combo/DA_LightCombo').get_editor_property('EntryComboTag').export_text()
pc=s['pc'];sub=unreal.get_default_object(unreal.load_class(None,'/Script/Engine.SubsystemBlueprintLibrary')).call_method('GetLocalPlayerSubSystemFromPlayerController',(pc,unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
attack=unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack')
state={'phase':'first_prepare','grant':None};reports=[]
def heal():target_asc.apply_gameplay_effect_spec_to_self(target_asc.make_outgoing_spec(heal_cls,1,target_asc.make_effect_context()))
def install(d):
    entry=unreal.HodgeAbilitySet_Definition();entry.import_text('(Definition="/Script/Hodgepodge.HodgeAbilityDefinition\''+d.get_path_name()+'\'",AbilityLevel=1)');s['ability_set'].set_editor_property('GrantedAbilityDefinitions',[entry])
    state['grant']=s['manager'].equip_item(s['grant_class']);assert state['grant']
    handles=[h for h in asc.get_all_abilities() if asc.find_ability_definition(h)==d];assert len(handles)==1
    state['handle']=handles[0];state['ga']=lib.call_method('GetGameplayAbilityFromSpecHandle',(asc,handles[0]))[0]
def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    if state['grant'] and unreal.SystemLibrary.is_valid(state['grant']):s['manager'].unequip_item(state['grant'])
    entry=unreal.HodgeAbilitySet_Definition();entry.import_text('(Definition="/Script/Hodgepodge.HodgeAbilityDefinition\'/Game/CodexText/SkillHitVolumes/DA_VolumeThreePulse.DA_VolumeThreePulse\'",AbilityLevel=1)');s['ability_set'].set_editor_property('GrantedAbilityDefinitions',[entry])
    (out/'reactivation-combo-tests.json').write_text(json.dumps({'error':error,'results':reports},ensure_ascii=False,indent=2),encoding='utf-8')
def tick(delta):
    try:
        now=unreal.GameplayStatics.get_time_seconds(w)
        if state['phase']=='first_prepare':
            source.set_actor_location(unreal.Vector(1000,1000,300),False,False);target.set_actor_location(unreal.Vector(1200,1000,300),False,False)
            heal();install(shared);state['ga_path']=state['ga'].get_path_name();assert unreal.HodgeCombatValidationLibrary.queue_ability_action(asc,state['handle'],False)
            state.update({'phase':'first','start':now});return
        if state['phase']=='first' and now-state['start']>shared.get_duration()+.4:
            assert abs(hc.get_health()-125)<.01,hc.get_health();reports.append({'case':'same_spec_first','health':hc.get_health(),'ga':state['ga_path']})
            heal();assert unreal.HodgeCombatValidationLibrary.queue_ability_action(asc,state['handle'],False);state.update({'phase':'second','start':now});return
        if state['phase']=='second' and now-state['start']>shared.get_duration()+.4:
            assert state['ga'].get_path_name()==state['ga_path'] and abs(hc.get_health()-125)<.01
            reports.append({'case':'same_spec_second','health':hc.get_health(),'same_ga_instance':True})
            s['manager'].unequip_item(state['grant']);state['grant']=None;install(pulse)
            target.set_actor_location(unreal.Vector(6000,1000,300),False,False)
            sub.inject_input_vector_for_action(attack,unreal.Vector(1,0,0),[],[])
            state.update({'phase':'attack_wait','start':now});return
        if state['phase']=='attack_wait':
            current=combat.get_current_combo_tag().export_text()
            if current!=entry_tag:
                state['attack_tag']=current;assert unreal.HodgeCombatValidationLibrary.queue_ability_action(asc,state['handle'],False)
                state.update({'phase':'memory_check','start':now});return
            assert now-state['start']<2,'Real attack input did not enter combo';return
        if state['phase']=='memory_check' and lib.call_method('IsGameplayAbilityActive',(state['ga'],)):
            remembered=combat.get_remembered_combo_tag().export_text();remaining=combat.get_combo_memory_remaining_time()
            assert remembered==state['attack_tag'] and remaining>0,(remembered,state['attack_tag'],remaining)
            reports.append({'case':'standalone_interrupt_preserves_combo','before':state['attack_tag'],'remembered':remembered,'remaining':remaining})
            assert unreal.HodgeCombatValidationLibrary.queue_ability_action(asc,state['handle'],True)
            state.update({'phase':'cancel_wait','start':now});return
        if state['phase']=='cancel_wait' and now-state['start']>.35:
            assert not lib.call_method('IsGameplayAbilityActive',(state['ga'],))
            reports.append({'case':'standalone_cancel','active_after':False});finish();return
    except Exception:finish(traceback.format_exc())
callback=unreal.register_slate_post_tick_callback(tick)
print('Verifying same spec/GA reactivation clears hit history and real attack input retains combo memory through a standalone interruption')
