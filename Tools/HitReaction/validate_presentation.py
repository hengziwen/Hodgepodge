"""跨 PIE 拓扑验证真实近战接触、方向反馈、运动及客户端去重次数。"""
import json
import re
import sys
import time
import traceback
from pathlib import Path
import unreal

MODE = globals().get('VALIDATION_MODE', 'Standalone')
OUT = Path(unreal.Paths.project_saved_dir()) / 'HitPresentation'
sys.path.insert(0, str(Path(unreal.Paths.project_dir()) / 'Tools' / 'HitReactionDummy'))
import control

editor = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
play = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.LevelEditorPlaySettings'))
perf = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.EditorPerformanceSettings'))
settings = {n: play.get_editor_property(n) for n in ['PlayNumberOfClients', 'RunUnderOneProcess']}
try:
    mode_before = int(play.get_editor_property('PlayNetMode').value)
except TypeError as error:
    mode_before = int(re.search(r"Cannot pythonize '(\d+)'", str(error)).group(1))
throttle = perf.get_editor_property('bThrottleCPUWhenNotForeground')
idle_settings = {n: unreal.SystemLibrary.get_console_variable_int_value(n) for n in ['Slate.bAllowThrottling', 't.IdleWhenNotForeground']}
cases = ['LightFront', 'LightBack', 'LightLeft', 'LightRight', 'HitStun', 'Knockback', 'Launch', 'Knockdown', 'LaunchAirHit', 'LaunchSlam', 'DownedContact', 'HighArmor']
state = {'boot': time.monotonic(), 'index': -1, 'results': [], 'samples': [], 'closing': False}
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(1 if MODE == 'Standalone' else 2, MODE == 'Dedicated')
perf.set_editor_property('bThrottleCPUWhenNotForeground', False)
editor_world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
for name in idle_settings:
    unreal.SystemLibrary.execute_console_command(editor_world, name + ' 0')
assert unreal.EditorLoadingAndSavingUtils.load_map('/Game/Tests/Combat/L_HitReactionDummy')


def tags():
    """高抗状态由既有 ASC 标签表达，不添加无敌或假扣血。"""
    result = unreal.GameplayTagContainer()
    result.import_text('(GameplayTags=((TagName="State.Combat.Body.SuperArmor")))')
    return result


def peers():
    """每个世界分别读取真实复制目标，不能把服务器结果当成客户端结果。"""
    result = []
    for world in unreal.EditorLevelLibrary.get_pie_worlds(True):
        targets = unreal.GameplayStatics.get_all_actors_of_class(world, state['class'])
        if targets:
            component = targets[0].get_component_by_class(unreal.HodgeHitReactionComponent)
            result.append({'world': world.get_path_name(), 'server': unreal.SystemLibrary.is_server(world),
                           'count': component.get_presentation_count(), 'controlled': component.is_controlled()})
    return result


def finish(error=None):
    """保存实际断言和未执行项，结束 PIE 后恢复用户偏好。"""
    if state['closing']:
        return
    state['closing'] = True
    if state.get('armor'):
        unreal.AbilitySystemLibrary.remove_loose_gameplay_tags(control._state['dummy'], tags(), True)
    if state.get('direction_handle'):
        state['rotation'].release_facing_request(state['direction_handle'])
    report = {'mode': MODE, 'error': error, 'results': state['results'], 'samples': state['samples'],
              'not_tested': ['physical input', 'high-armor active enemy ability continuity', 'subjective audio quality']}
    (OUT / ('pie-' + MODE + '.json')).write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
    control.stop()
    editor.editor_request_end_play()


def tick(delta):
    """每个用例通过真实装备与 Melee 流程触发，不直接伪造裁决。"""
    try:
        worlds = unreal.EditorLevelLibrary.get_pie_worlds(True)
        if state['closing']:
            if worlds:
                return
            assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(1 if mode_before == 0 else 2, mode_before == 2)
            for name, value in settings.items():
                play.set_editor_property(name, value)
            perf.set_editor_property('bThrottleCPUWhenNotForeground', throttle)
            editor_world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
            for name, value in idle_settings.items():
                unreal.SystemLibrary.execute_console_command(editor_world, name + ' ' + str(value))
            unreal.unregister_slate_post_tick_callback(state['callback'])
            return
        if time.monotonic() - state['boot'] > 180:
            raise RuntimeError('拓扑验证超时')
        if 'world' not in state:
            servers = [w for w in worlds if unreal.SystemLibrary.is_server(w)]
            if len(servers) != 1:
                return
            world = servers[0]
            source = unreal.GameplayStatics.get_player_pawn(world, 0)
            if not isinstance(source, unreal.HodgeHeroCharacter) or not source.get_hodge_ability_system_component():
                return
            cls = unreal.load_asset(control.ROOT + '/BP_HitReactionDummy').generated_class()
            targets = unreal.GameplayStatics.get_all_actors_of_class(world, cls)
            if len(targets) != 1 or not targets[0].initialize_enemy_ability_system():
                return
            # 第一刀等待所有接收端完成真实初始化，不用服务器就绪推定客户端已绑定。
            expected_worlds = 1 if MODE == 'Standalone' else (3 if MODE == 'Dedicated' else 2)
            if len(worlds) != expected_worlds:
                return
            for peer_world in worlds:
                peer_targets = unreal.GameplayStatics.get_all_actors_of_class(peer_world, cls)
                if len(peer_targets) != 1 or not peer_targets[0].get_enemy_init_component().is_gameplay_ready():
                    return
                if not peer_targets[0].get_component_by_class(unreal.HodgeHitReactionComponent).get_profile():
                    return
            control.start()
            state.update(world=world, **{'class': cls})
            state['next'] = unreal.GameplayStatics.get_time_seconds(world) + 0.5
        now = unreal.GameplayStatics.get_time_seconds(state['world'])
        if not state.get('active') and now < state['next']:
            return
        if state.get('active'):
            row = control.snapshot()
            component = control._state['reaction']
            montage = control._state['dummy'].mesh.get_anim_instance().get_current_active_montage()
            row.update(case=state['active'], selected=montage.get_name() if montage else None, presentations=component.get_presentation_count())
            row['target_yaw'] = control._state['dummy'].get_actor_rotation().yaw
            direction = component.get_last_presentation().direction
            row['contact_direction'] = [direction.x, direction.y, direction.z]
            state['samples'].append(row)
            # 同时观察各客户端实际播放的资产，计数一致不代表动画选择必然一致。
            for peer_world in worlds:
                targets = unreal.GameplayStatics.get_all_actors_of_class(peer_world, state['class'])
                if targets:
                    peer_montage = targets[0].mesh.get_anim_instance().get_current_active_montage()
                    if peer_montage:
                        state['peer_animations'].setdefault(peer_world.get_path_name(), set()).add(peer_montage.get_name())
            if state['active'] == 'DownedContact' and not state.get('followed') and now - state['started'] >= 0.5:
                control.probe('HitStun')
                state['followed'] = True
            if now < state['next']:
                return
            after = control.snapshot()
            # 起身使用实际 Montage 长度；固定三秒不能当作所有阶段的结束期限。
            if after['controlled'] and now - state['started'] < 6.3:
                return
            outcome = str(component.get_last_result().outcome)
            counts = peers()
            expected = 2 if state['active'] in ['LaunchAirHit', 'LaunchSlam', 'DownedContact'] else 1
            added = component.get_presentation_count() - state['before_count']
            assert added == expected, (state['active'], 'contact count', added, expected)
            assert after['health'] < state['before_health'], '没有真实扣血'
            assert all(p['count'] == component.get_presentation_count() for p in counts), counts
            if state['active'] == 'HighArmor':
                assert 'LOW_JUDGEMENT' in outcome and not after['controlled'], outcome
                unreal.AbilitySystemLibrary.remove_loose_gameplay_tags(control._state['dummy'], tags(), True)
                state['armor'] = False
            elif state['active'] == 'DownedContact':
                assert 'NO_IMPACT' in outcome, outcome
            else:
                assert not after['controlled'], (state['active'], '未恢复控制')
            seen = sorted({r['selected'] for r in state['samples'] if r['case'] == state['active'] and r['selected']})
            if state['active'].startswith('Light'):
                if MODE != 'Dedicated':
                    assert 'AM_Hit_' + state['active'][5:] in seen, seen
                assert all('AM_Hit_' + state['active'][5:] in values for values in state['peer_animations'].values()), state['peer_animations']
            state['results'].append({'case': state['active'], 'passed': True, 'outcome': outcome, 'contacts': added, 'peers': counts,
                                     'elapsed': now - state['started'], 'animations': seen,
                                     'peer_animations': {k: sorted(v) for k, v in state['peer_animations'].items()}})
            state['active'] = None
        state['index'] += 1
        if state['index'] >= len(cases):
            finish()
            return
        case = cases[state['index']]
        if state.get('direction_handle'):
            state['rotation'].release_facing_request(state.pop('direction_handle'))
        control.reset()
        state.update(active=case, started=now, next=now + 3.2, followed=False, peer_animations={},
                     before_health=control.snapshot()['health'], before_count=control._state['reaction'].get_presentation_count())
        if case.startswith('Light'):
            yaw = {'Front': 180, 'Back': 0, 'Left': -90, 'Right': 90}[case[5:]]
            control._state['dummy'].set_actor_rotation(unreal.Rotator(yaw=yaw), False)
            control._state['dummy'].get_controller().set_control_rotation(unreal.Rotator(yaw=yaw))
            # 使用现有朝向仲裁持有测试方向，不能让基础朝向在命中前恢复到旧 Yaw。
            rotation = control._state['dummy'].get_component_by_class(unreal.HodgeCharacterRotationComponent)
            direction = unreal.MathLibrary.get_forward_vector(unreal.Rotator(yaw=yaw))
            identity = unreal.Guid()
            identity.import_text('%032x' % (state['index'] + 1))
            handle = rotation.request_action_facing(direction, control._state['dummy'], identity, True)
            state.update(rotation=rotation, direction_handle=handle)
            control.probe('Light')
        elif case in ['LaunchAirHit', 'LaunchSlam']:
            control.launch_then('AirHit' if case == 'LaunchAirHit' else 'Slam')
        elif case == 'HighArmor':
            assert unreal.AbilitySystemLibrary.add_loose_gameplay_tags(control._state['dummy'], tags(), True)
            state['armor'] = True
            control.probe('HitStun')
        else:
            control.probe('Knockdown' if case == 'DownedContact' else case)
    except Exception:
        finish(traceback.format_exc())


state['callback'] = unreal.register_slate_post_tick_callback(tick)
editor.editor_request_begin_play()
print('PRESENTATION_VALIDATION_STARTED ' + MODE)
