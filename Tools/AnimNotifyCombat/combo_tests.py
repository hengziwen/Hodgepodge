import unreal,builtins,json,traceback
from pathlib import Path
s=builtins.HODGE_SKILL_TEST;w=s['world'];source=s['source'];asc=s['source_asc'];combat=source.get_component_by_class(unreal.HodgeCombatComponentBase)
lib=unreal.get_default_object(unreal.load_class(None,'/Script/GameplayAbilities.AbilitySystemBlueprintLibrary'))
sub=unreal.get_default_object(unreal.load_class(None,'/Script/Engine.SubsystemBlueprintLibrary')).call_method('GetLocalPlayerSubSystemFromPlayerController',(s['pc'],unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
attack=unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack');target=s['target'];target.set_actor_location(unreal.Vector(6000,1000,300),False,False)
expected=['AM_Attack01_Montage','AM_Attack02_Montage','AM_Attack03_Montage','AM_Attack05_Montage','AM_Attack04_Montage','AM_Attack01_Montage']
state={'started':False,'current':None,'pressed':False,'records':[],'begin':0,'start':unreal.GameplayStatics.get_time_seconds(w)}
def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    Path('E:/Project/Git/Hodgepodge/Saved/AnimNotifyMigration/combo-tests.json').write_text(json.dumps({'error':error,'sequence':state['records'],'expected':expected},ensure_ascii=False,indent=2),encoding='utf-8')
def tick(delta):
    try:
        now=unreal.GameplayStatics.get_time_seconds(w)
        if not state['started']:
            sub.inject_input_vector_for_action(attack,unreal.Vector(1,0,0),[],[]);state['started']=True;return
        active=[]
        for h in asc.get_all_abilities():
            d=asc.find_ability_definition(h)
            if not d or d.get_editor_property('ExecutionRoute')!=unreal.HodgeAbilityExecutionRoute.COMBO_COORDINATED:continue
            ga=lib.call_method('GetGameplayAbilityFromSpecHandle',(asc,h))[0]
            if lib.call_method('IsGameplayAbilityActive',(ga,)):active.append((ga,d,h))
        assert len(active)<=1,[(ga.get_name(),d.get_name()) for ga,d,h in active]
        if active:
            ga,d,handle=active[0];name=d.get_editor_property('ExecutionConfig').get_editor_property('Montage').get_name()
            if name!=state['current']:
                state['current']=name;state['pressed']=False;state['begin']=now
                state['records'].append({'montage':name,'time':now-state['start'],'combo':combat.get_current_combo_tag().export_text()})
                assert [x['montage'] for x in state['records']]==expected[:len(state['records'])],state['records']
            if len(state['records'])==len(expected):
                assert unreal.HodgeCombatValidationLibrary.queue_ability_action(asc,handle,True)
                finish();return
            m=d.get_editor_property('ExecutionConfig').get_editor_property('Montage');start=None
            for event in unreal.AnimationLibrary.get_animation_notify_events(m):
                n=event.get_editor_property('NotifyStateClass')
                if isinstance(n,unreal.HodgeAnimNotifyState_GameplayTag) and 'Status.Attack.Cancel.NextAttack' in n.get_editor_property('StateTag').export_text():start=unreal.AnimationLibrary.get_anim_notify_event_trigger_time(event);break
            assert start is not None,name
            # 窗口前 0.15 秒输入，验证缓存由通知进入消耗；末段同样直接回首段。
            if not state['pressed'] and now-state['begin']>=max(0,start-.15):
                sub.inject_input_vector_for_action(attack,unreal.Vector(1,0,0),[],[]);state['pressed']=True
        assert now-state['start']<25,('Combo timed out',state['records'])
    except Exception:finish(traceback.format_exc())
callback=unreal.register_slate_post_tick_callback(tick)
print('Started real input combo sequence and buffered final-recovery restart test.')
