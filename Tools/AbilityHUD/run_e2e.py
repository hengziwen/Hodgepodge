"""通过实际E/R输入和服务器能量GE验收图标，截图与断言按网络模式归档。"""
import gc
import json
import shutil
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
ui_lib=unreal.HodgeUIAuthoringLibrary
# 截图临时关闭现有命中调试框，结束后由统一恢复脚本还原原值。
debug_names=['Hodge.Combat.DebugDraw.Weapon','Hodge.Combat.DebugDraw.Body','Hodge.Combat.DebugDraw.HitBox']
debug_restore=WORK/'debug-restore.json'
if not debug_restore.exists():
    debug_restore.write_text(json.dumps({name:unreal.SystemLibrary.get_console_variable_int_value(name) for name in debug_names}),encoding='utf-8')
for name in debug_names:unreal.SystemLibrary.execute_console_command(server,name+' 0')
input_library=unreal.get_default_object(unreal.load_class(None,'/Script/Engine.SubsystemBlueprintLibrary'))
inputs=input_library.call_method('GetLocalPlayerSubSystemFromPlayerController',(owner.get_controller(),unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
actions={key:unreal.load_asset('/Game/Main/Input/InputAction/IA_Resonance_'+value) for key,value in [('E','Skill'),('R','Liberation')]}
state={'phase':'warmup','at':unreal.GameplayStatics.get_time_seconds(server),'wall':time.monotonic(),'captures':[]}
checks=[]
# 随Experience实际条目验收，暗流新增后不把旧的三个条目数量写死。
eas=unreal.load_asset('/Game/Main/UI/EAS_HodgeGameplayUI')
expected_entries=sum(len(a.get_editor_property('Widgets')) for a in eas.get_editor_property('Actions') if a.get_class().get_name()=='GameFeatureAction_AddWidgets')


def check(name,condition,detail=None):
    """只记录实际玩法与Widget数据，不用测试脚本替代生产显示计算。"""
    checks.append({'case':name,'passed':bool(condition),'detail':detail})
    assert condition,(name,detail)


def step(phase,now):
    """阶段时钟只用于等待验收时点，不驱动游戏冷却或充能。"""
    state.update(phase=phase,at=now)


def capture(name):
    """截图由游戏Tick中的Slate执行，下一帧复制为本用例的独立文件。"""
    if MODE=='single':
        assert ui_lib.queue_ui_action(owner.get_controller(),'Capture')
        state['captures'].append((name,time.monotonic()+.3))


def add_energy(value):
    """正式服务器GE注入已知能量，仅作为UI测试夹具，不修改生产充能规则。"""
    assert lib.queue_resonance_energy_delta(authority,value)


def finish(error=None):
    """只清理本次测试持有的输入及回调，原PIE停止工具销毁世界。"""
    unreal.unregister_slate_post_tick_callback(state['callback'])
    for action in actions.values():inputs.inject_input_vector_for_action(action,unreal.Vector(),[],[])
    report={'mode':MODE,'passed':not error and all(row['passed'] for row in checks),'checks':checks,'error':error}
    (WORK/(MODE+'-e2e.json')).write_text(json.dumps(report,ensure_ascii=False,indent=2),encoding='utf-8')
    for name in ['worlds','server','clients','owner','authority','inputs']:globals()[name]=None
    gc.collect()


def tick(delta):
    """检查零点、四分之一圈、满圈，以及两个技能的真实独立冷却。"""
    try:
        now=unreal.GameplayStatics.get_time_seconds(server)
        assert time.monotonic()-state['wall']<85.,'技能图标验收超时：'+state['phase']
        for name,due in list(state['captures']):
            if time.monotonic()>=due:
                shutil.copy2(Path(unreal.Paths.project_saved_dir())/'UIFoundation/UI-Slate.png',WORK/(name+'.png'))
                state['captures'].remove((name,due))
        ui=json.loads(ui_lib.inspect_player_ui(owner.get_controller()))
        local=lib.inspect_resonance_energy(owner)
        phase=state['phase']; elapsed=now-state['at']
        if phase=='warmup' and elapsed>2.:
            check('E/R由Experience注入Layout扩展点',ui.get('ability_extension_slots') and ui.get('entries')==expected_entries and ui.get('skill_icon_ready') and ui.get('resonance_widget'),ui)
            check('初始E可用，R零能量不可用',ui.get('skill_enabled') and not ui.get('resonance_enabled') and ui.get('resonance_percent')==0.,ui)
            check('R位于E右侧且二者接近底边',ui['resonance_center_x']>ui['skill_center_x'] and abs(ui['resonance_center_y']-ui['skill_center_y'])<2.,ui)
            capture('01-empty'); step('energy25',now); add_energy(25.)
        elif phase.startswith('energy'):
            expected=float(phase[6:])
            if abs(local.x-expected)<.1 and elapsed>.5:
                check('真实能量环'+str(int(expected))+'%',abs(ui.get('resonance_percent',-1.)-expected/100.)<.005,ui)
                capture('energy-'+str(int(expected)))
                if expected<100.:
                    step('energy'+str(int(expected+25)),now); add_energy(25.)
                else:
                    check('满充能且无CD时R可用',ui.get('resonance_enabled'),ui)
                    inputs.inject_input_vector_for_action(actions['E'],unreal.Vector(1,0,0),[],[])
                    step('castE',now)
        elif phase=='castE':
            inputs.inject_input_vector_for_action(actions['E'],unreal.Vector(),[],[])
            if ui.get('skill_cooldown',0.)>0.:
                check('E真实释放进入8秒CD',6.5<ui['skill_cooldown']<=8.05 and not ui['skill_enabled'],ui)
                check('E的CD不影响R满能量环',ui['resonance_percent']==1. and ui['resonance_cooldown']==0.,ui)
                check('CD开始亮色接近底部',ui['skill_cooldown_progress']<.2,ui)
                capture('02-skill-start'); step('halfE',now)
        elif phase=='halfE' and elapsed>3.5:
            check('E冷却亮色自下向上连续恢复',.4<ui['skill_cooldown_progress']<.65 and 2.<ui['skill_cooldown']<5.,ui)
            capture('03-skill-half'); step('endE',now)
        elif phase=='endE' and ui.get('skill_cooldown',1.)==0.:
            check('E冷却完成恢复白色并允许释放',ui['skill_cooldown_progress']==1. and ui['skill_enabled'],ui)
            inputs.inject_input_vector_for_action(actions['R'],unreal.Vector(1,0,0),[],[])
            step('castR',now)
        elif phase=='castR':
            inputs.inject_input_vector_for_action(actions['R'],unreal.Vector(),[],[])
            if ui.get('resonance_cooldown',0.)>0. and local.x==0.:
                check('R真实释放消耗100并进入12秒CD',10.5<ui['resonance_cooldown']<=12.05 and ui['resonance_percent']==0. and not ui['resonance_enabled'],ui)
                check('R不进入E的CD',ui['skill_cooldown']==0.,ui)
                capture('04-liberation-start'); step('refillR',now); add_energy(100.)
        elif phase=='refillR' and local.x==100. and elapsed>.5:
            check('冷却中满能量环与CD独立显示',ui['resonance_percent']==1. and ui['resonance_cooldown']>0. and not ui['resonance_enabled'],ui)
            others=[lib.inspect_resonance_energy(pawn).x for pawn in unreal.GameplayStatics.get_all_actors_of_class(server,unreal.HodgeHeroCharacter) if pawn!=authority]
            check('其他玩家未被回能',all(value==0. for value in others),others)
            capture('05-full-energy-with-cd'); step('halfR',now)
        elif phase=='halfR' and .4<ui.get('resonance_cooldown_progress',0.)<.65:
            check('R同样自下向上显示CD恢复',ui['resonance_cooldown']>0. and ui['resonance_percent']==1.,ui)
            capture('06-liberation-half'); step('endR',now)
        elif phase=='endR' and ui.get('resonance_cooldown',1.)==0.:
            check('R满能量且CD结束时可释放',ui['resonance_enabled'] and ui['resonance_cooldown_progress']==1.,ui)
            assert ui_lib.queue_ui_action(owner.get_controller(),'RebuildRoot')
            step('rebuild',now)
        elif phase=='rebuild' and elapsed>1.:
            check('重建HUD后扩展重新注入且没有重复层',ui.get('game_count')==1 and ui.get('entries')==expected_entries and ui.get('skill_icon_ready') and ui.get('resonance_enabled'),ui)
            check('生命条和终焉入口保留原有布局',ui.get('display_percent')==1. and abs(ui.get('vitals_center_x',0.)-.5)<.02 and ui.get('end_center_x',0.)>.85 and ui.get('end_center_y',1.)<.15,ui)
            assert ui_lib.queue_ui_action(owner.get_controller(),'Deactivate')
            step('deactivate',now)
        elif phase=='deactivate' and ui.get('game_count')==0:
            check('停用Experience UI时所属扩展全部撤销',ui.get('entries')==0 and not ui.get('resonance_widget'),ui)
            assert ui_lib.queue_ui_action(owner.get_controller(),'Activate')
            step('reactivate',now)
        elif phase=='reactivate' and ui.get('skill_icon_ready') and elapsed>.5:
            check('恢复Experience重新注入已配置扩展且资源不重置',ui.get('ability_extension_slots') and ui.get('entries')==expected_entries and ui.get('resonance_enabled') and ui.get('resonance_percent')==1.,ui)
            capture('07-final'); step('final_capture',now)
        elif phase=='final_capture' and elapsed>.5:
            finish()
    except Exception:
        finish(traceback.format_exc())


state['callback']=unreal.register_slate_post_tick_callback(tick)
