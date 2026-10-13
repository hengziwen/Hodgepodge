"""用真实攻击和 Dash 输入验证 WING-01～06/09/10/12。"""
import unreal,json,time,traceback
from pathlib import Path
WORK=Path(globals().get('WING_WORK','E:/Project/Git/Hodgepodge/Saved/WingAttack'))
mode=globals().get('WING_MODE','single')
worlds=unreal.EditorLevelLibrary.get_pie_worlds(True)
server=next(w for w in worlds if unreal.SystemLibrary.is_server(w))
clients=[w for w in worlds if w!=server]
local=next(a for a in unreal.GameplayStatics.get_all_actors_of_class(clients[0] if clients else server,unreal.HodgeHeroCharacter)
           if not clients or a.get_local_role()==unreal.NetRole.ROLE_AUTONOMOUS_PROXY)
player_id=local.get_editor_property('PlayerState').get_editor_property('PlayerId')
peers=[a for w in worlds for a in unreal.GameplayStatics.get_all_actors_of_class(w,unreal.HodgeHeroCharacter)
       if a.get_editor_property('PlayerState').get_editor_property('PlayerId')==player_id]
for w in worlds:
    for actor in unreal.GameplayStatics.get_all_actors_of_class(w,unreal.HodgeHeroCharacter):
        if actor not in peers:actor.set_actor_location(unreal.Vector(-2000,-2000,100),False,True)
for enemy in unreal.GameplayStatics.get_all_actors_of_class(server,unreal.HodgeEnemyCharacter):
    if enemy.get_controller() and enemy.get_controller().brain_component:enemy.get_controller().brain_component.stop_logic('Wing presentation validation')
    enemy.set_actor_location(unreal.Vector(2300,2300,100),False,True)
lib=unreal.get_default_object(unreal.load_class(None,'/Script/Engine.SubsystemBlueprintLibrary'))
inputs=lib.call_method('GetLocalPlayerSubSystemFromPlayerController',(local.get_controller(),unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
attack=unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack')
dash=unreal.load_asset('/Game/Main/Input/InputAction/IA_Sprint')
profile=unreal.load_asset('/Game/Main/Character/Hero/Wing/DA_PoverWing_Anzhu')
montage=profile.attack_montage
checks=[];samples=[]
state={'wall':time.monotonic(),'phase':'natural','last_input':-100.,'seen':set(),'moved':set(),'points':{},'seen_attack':False,'cancel_sent':False,'end_at':None}
state['natural_fade']=set();state['cancel_fade']=set()

def check(req,name,condition,detail=None):
    checks.append({'requirement':req,'check':name,'passed':bool(condition),'detail':detail})
    assert condition,(name,detail)

def finish(error=None):
    unreal.unregister_slate_post_tick_callback(state['callback'])
    result={'mode':mode,'passed':not error and all(c['passed'] for c in checks),'checks':checks,'samples':samples,'error':error}
    (WORK/(mode+'-e2e.json')).write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
    print(json.dumps({'mode':mode,'passed':result['passed'],'checks':len(checks),'error':error},ensure_ascii=False))

for index,peer in enumerate(peers):
    wings=peer.get_components_by_class(unreal.HodgeWingComponent)
    check('WING-12','组件唯一_'+str(index),len(wings)==1)
    mesh=wings[0].get_wing_mesh()
    if mode=='dedicated' and peer.get_world()==server:
        if profile.enable_authority_hit_sampling:
            check('WING-HIT-02','专服采样网格隐藏',mesh and not mesh.visible and mesh.hidden_in_game)
            check('WING-HIT-02','专服翼不自动Tick',not mesh.is_component_tick_enabled())
        else:
            check('WING-10','专服没有翼网格',not mesh)
    else:
        check('WING-01','初始隐藏_'+str(index),mesh and not mesh.visible and mesh.hidden_in_game)
        check('WING-10','翼自动Tick关闭_'+str(index),not mesh.is_component_tick_enabled())
        check('WING-07','校准骨骼_'+str(index),str(mesh.get_attach_socket_name())=='WingCase')
        check('WING-07','校准缩放_'+str(index),abs(mesh.relative_scale3d.x-.8)<.0001)

def tick(delta):
    try:
        assert time.monotonic()-state['wall']<110,'端到端测试超时'
        now=unreal.GameplayStatics.get_time_seconds(server)
        node=str(local.get_component_by_class(unreal.HodgeCombatComponentBase).get_current_combo_tag().get_editor_property('TagName'))
        state['seen'].add(node)
        primary=unreal.HodgeCombatValidationLibrary.inspect_montage_state(local,montage)
        rows=[];shown=[]
        for index,peer in enumerate(peers):
            wing=peer.get_component_by_class(unreal.HodgeWingComponent)
            pose=wing.get_presentation_state()
            visible=pose.get_editor_property('bVisible')
            mesh=wing.get_wing_mesh()
            row={'peer':index,'role':str(peer.get_local_role()),'visible':visible,
                 'source':pose.get_editor_property('SourceTime'),'wing':pose.get_editor_property('WingTime'),
                 'evaluations':pose.get_editor_property('PoseEvaluations'),'instance':pose.get_editor_property('SourceInstanceId')}
            row['amount']=wing.get_visibility_amount();row['fading']=wing.is_fading_out()
            if mesh and 0<row['amount']<1 and row['fading']:
                state['cancel_fade' if state['phase']=='cancel_wait' else 'natural_fade'].add(index)
                row['materials']=[mesh.get_material(i).get_scalar_parameter_value('WingVisibility') for i in range(mesh.get_num_materials())]
                assert all(abs(value-row['amount'])<.0001 for value in row['materials']),'材质槽未同步淡出'
            rows.append(row)
            if visible:
                shown.append(index);state['moved'].add(index)
                assert abs(row['source']-row['wing'])<.0001,'翼时钟与源时钟不一致'
                point=mesh.get_socket_transform('Bone_Wing006',unreal.RelativeTransformSpace.RTS_COMPONENT).translation
                state['points'].setdefault(index,set()).add((round(point.x,1),round(point.y,1),round(point.z,1)))
            if mode=='dedicated' and peer.get_world()==server:
                if profile.enable_authority_hit_sampling:
                    assert mesh and not visible and not mesh.visible
                else:assert not mesh
        samples.append({'time':now,'phase':state['phase'],'node':node,'montage_position':primary.position,'peers':rows})
        if state['phase'] in ['natural','cancel_combo']:
            if primary.instance_id<0 and now-state['last_input']>.12:
                inputs.inject_input_vector_for_action(attack,unreal.Vector(1,0,0),[],[])
                state['last_input']=now
            if primary.instance_id>=0:
                state['seen_attack']=True
                if state['phase']=='cancel_combo' and primary.position>.3 and not state['cancel_sent']:
                    assert dash,'Dash 输入资产缺失'
                    inputs.inject_input_vector_for_action(dash,unreal.Vector(1,0,0),[],[])
                    state['cancel_sent']=True;state['cancel_at']=now;state['phase']='cancel_wait'
            if state['phase']=='natural' and state['seen_attack'] and primary.instance_id<0:
                check('WING-02','五段连段已执行',all('Combo.Light.'+n in state['seen'] for n in ['01','02','03','05','04']))
                expected={i for i,p in enumerate(peers) if not(mode=='dedicated' and p.get_world()==server)}
                check('WING-09','所有表现端显现',state['moved']==expected,sorted(state['moved']))
                check('WING-05','实际翼骨运动',all(len(state['points'].get(i,set()))>3 for i in expected))
                state['phase']='idle_wait';state['end_at']=now;state['idle_counts']=[r['evaluations'] for r in rows]
        elif state['phase']=='idle_wait' and now-state['end_at']>.6:
            check('WING-06','正常收招各端隐藏',not shown)
            if profile.fade_out_seconds>0:
                expected={i for i,p in enumerate(peers) if not(mode=='dedicated' and p.get_world()==server)}
                check('WING-13','正常收招各端有材质淡出',state['natural_fade']==expected,sorted(state['natural_fade']))
            check('WING-10','空闲没有额外翼求值',[r['evaluations'] for r in rows]==state['idle_counts'])
            state['phase']='cancel_combo';state['seen_attack']=False;state['last_input']=now
        elif state['phase']=='cancel_wait' and now-state['cancel_at']>.8:
            check('WING-06','Dash 取消主攻击',primary.instance_id<0)
            check('WING-06','Dash 取消各端隐藏',not shown)
            if profile.fade_out_seconds>0:
                expected={i for i,p in enumerate(peers) if not(mode=='dedicated' and p.get_world()==server)}
                check('WING-13','Dash 取消各端有材质淡出',state['cancel_fade']==expected,sorted(state['cancel_fade']))
            state['phase']='final_idle';state['end_at']=now;state['idle_counts']=[r['evaluations'] for r in rows]
        elif state['phase']=='final_idle' and now-state['end_at']>.6:
            check('WING-10','取消后空闲不求值',[r['evaluations'] for r in rows]==state['idle_counts'])
            finish()
    except Exception:finish(traceback.format_exc())
state['callback']=unreal.register_slate_post_tick_callback(tick)
print('开始真实输入翅膀端到端测试：'+mode)
