"""从实际玩家控制台验证回满命令、远端权限转发和冷却保持。"""
import gc
import json
import time
import traceback
from pathlib import Path
import unreal

WORK=Path(globals()['LOCK_ON_WORK'])
MODE=globals()['LOCK_ON_MODE']
worlds=unreal.EditorLevelLibrary.get_pie_worlds(True)
server=next(world for world in worlds if unreal.SystemLibrary.is_server(world))
clients=[world for world in worlds if world!=server]
owner=next(pawn for pawn in unreal.GameplayStatics.get_all_actors_of_class(clients[0] if clients else server,unreal.HodgeHeroCharacter)
           if not clients or pawn.get_local_role()==unreal.NetRole.ROLE_AUTONOMOUS_PROXY)
pid=owner.get_editor_property('PlayerState').get_editor_property('PlayerId')
authority=next(pawn for pawn in unreal.GameplayStatics.get_all_actors_of_class(server,unreal.HodgeHeroCharacter)
               if pawn.get_editor_property('PlayerState').get_editor_property('PlayerId')==pid)
lib=unreal.HodgeCombatValidationLibrary
for enemy in unreal.GameplayStatics.get_all_actors_of_class(server,unreal.HodgeEnemyCharacter):
    controller=enemy.get_controller()
    if controller and controller.brain_component:controller.brain_component.stop_logic('Debug fill validation')
    enemy.set_actor_location(unreal.Vector(5000,5000,100),False,True)
input_library=unreal.get_default_object(unreal.load_class(None,'/Script/Engine.SubsystemBlueprintLibrary'))
inputs=input_library.call_method('GetLocalPlayerSubSystemFromPlayerController',(owner.get_controller(),unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
action=unreal.load_asset('/Game/Main/Input/InputAction/IA_Resonance_Liberation')
cooldown=unreal.load_class(None,'/Game/Main/Character/Hero/Ability/ResonanceLiberation/GE_Resonance_Liberation_Cooldown.GE_Resonance_Liberation_Cooldown_C')
state={'phase':'warmup','at':unreal.GameplayStatics.get_time_seconds(server),'wall':time.monotonic()}
checks=[]


def check(name,condition,detail=None):
    """记录实际结果，失败保留诊断，不只检查命令有没有调用。"""
    checks.append({'case':name,'passed':bool(condition),'detail':detail})
    assert condition,(name,detail)


def command():
    """夹具仅排队控制台执行，客户端RPC和GE由正式Runtime实现。"""
    assert unreal.HodgeUIAuthoringLibrary.queue_ui_action(owner.get_controller(),'ResonanceFill')


def finish(error=None):
    """清理输入及编辑器回调，PIE生命周期交给已有停止工具。"""
    unreal.unregister_slate_post_tick_callback(state['callback'])
    inputs.inject_input_vector_for_action(action,unreal.Vector(),[],[])
    report={'mode':MODE,'passed':not error and all(row['passed'] for row in checks),'checks':checks,'error':error}
    (WORK/(MODE+'-e2e.json')).write_text(json.dumps(report,ensure_ascii=False,indent=2),encoding='utf-8')
    for name in ['worlds','server','clients','owner','authority','inputs','enemy','controller']:
        globals()[name]=None
    gc.collect()


def tick(delta):
    """等待真实复制与激活完成，不通过直接修改属性证明命令通过。"""
    try:
        now=unreal.GameplayStatics.get_time_seconds(server)
        assert time.monotonic()-state['wall']<35.,'调试命令PIE超时'
        local=lib.inspect_resonance_energy(owner)
        remote=lib.inspect_resonance_energy(authority)
        count=authority.get_hodge_ability_system_component().call_method('GetGameplayEffectCount',(cooldown,None,True))
        phase=state['phase']
        if phase=='warmup' and now-state['at']>2.:
            check('初始化容量100且能量0',local.y==100. and remote.y==100. and local.x==0. and remote.x==0.)
            command(); state.update(phase='fill',at=now)
        elif phase=='fill' and local.x==100. and remote.x==100.:
            check('实际控制台命令使服务器和拥有者回满',True)
            others=[lib.inspect_resonance_energy(pawn).x for pawn in unreal.GameplayStatics.get_all_actors_of_class(server,unreal.HodgeHeroCharacter) if pawn!=authority]
            check('其他玩家能量保持0',all(value==0. for value in others),others)
            check('回满不产生额外冷却',count==0)
            inputs.inject_input_vector_for_action(action,unreal.Vector(1,0,0),[],[])
            state.update(phase='cast',at=now)
        elif phase=='cast':
            inputs.inject_input_vector_for_action(action,unreal.Vector(),[],[])
            if local.x==0. and remote.x==0. and count==1:
                check('回满后R键正常消耗100并进入CD',True)
                command(); state.update(phase='cooldown_fill',at=now)
        elif phase=='cooldown_fill' and local.x==100. and remote.x==100.:
            check('冷却中仍可回满且原CD保留',count==1)
            command(); state.update(phase='repeat',at=now)
        elif phase=='repeat' and now-state['at']>.5:
            ui=json.loads(unreal.HodgeUIAuthoringLibrary.inspect_player_ui(owner.get_controller()))
            check('重复命令保持100且不移除冷却',local.x==100. and remote.x==100. and count==1)
            check('HUD满条显示独立CD且按钮仍不可释放',ui.get('resonance_percent')==1. and '100/100' in ui.get('resonance_text','') and 's' in ui.get('resonance_text','') and ui.get('resonance_enabled') is False,ui)
            finish()
    except Exception:
        finish(traceback.format_exc())


state['callback']=unreal.register_slate_post_tick_callback(tick)
