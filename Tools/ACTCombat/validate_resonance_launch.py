"""在百万血假人场景用真实 R 输入验证伤害、Fly 阶段和真实落地，不保存资产。"""
from pathlib import Path
import builtins
import json
import re
import time
import traceback
import unreal

editor = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
play = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.LevelEditorPlaySettings'))
perf = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.EditorPerformanceSettings'))
previous = {key: play.get_editor_property(key) for key in ['PlayNumberOfClients', 'RunUnderOneProcess']}
try:
    old_mode = int(play.get_editor_property('PlayNetMode').value)
except TypeError as error:
    old_mode = int(re.search(r"Cannot pythonize '(\d+)'", str(error)).group(1))
old_throttle = perf.get_editor_property('bThrottleCPUWhenNotForeground')
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(1, False)
perf.set_editor_property('bThrottleCPUWhenNotForeground', False)
assert unreal.EditorLoadingAndSavingUtils.load_map('/Game/Tests/Combat/L_HitReactionDummy')
definition = unreal.load_asset('/Game/Main/Character/Hero/Ability/ResonanceLiberation/DA_Resonance_Liberation')
action = unreal.load_asset('/Game/Main/Input/InputAction/IA_Resonance_Liberation')
report = {'checks': [], 'samples': [], 'error': None, 'input': 'Enhanced Input injection; not physical keyboard',
          'root_motion_migration': None, 'old_mode': old_mode}
scope = {'phase': 'boot', 'wall': time.monotonic(), 'last_sample': -1.0, 'hit_ids': set(),
         'seen_start': False, 'seen_loop': False, 'seen_landing': False, 'seen_falling': False}
builtins.HODGE_RESONANCE_LAUNCH_SCOPE = globals()


def check(name, condition, detail=None):
    """保留失败前的完整采样，不用已发送输入代替实际游戏结果。"""
    report['checks'].append({'case': name, 'passed': bool(condition), 'detail': detail})
    assert condition, (name, detail)


def finish(error=None):
    """等待 PIE 真正退出后恢复偏好，避免脚本回调泄漏到下一次会话。"""
    if scope['phase'] == 'closing':
        return
    report['error'] = error
    output = Path(unreal.Paths.project_saved_dir()) / globals().get('R_REPORT_DIR', 'ACTCombat/ResonanceLaunch')
    output.mkdir(parents=True, exist_ok=True)
    (output / 'r-input-smoke.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
    scope['phase'] = 'closing'
    editor.editor_request_end_play()


def tick(delta):
    """按实际世界、能量、动画和移动阶段推进；超时明确失败。"""
    try:
        worlds = unreal.EditorLevelLibrary.get_pie_worlds(True)
        if scope['phase'] == 'closing':
            if worlds:
                return
            assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(1 if old_mode == 0 else 2, old_mode == 2)
            for key, value in previous.items():
                play.set_editor_property(key, value)
            perf.set_editor_property('bThrottleCPUWhenNotForeground', old_throttle)
            unreal.unregister_slate_post_tick_callback(scope['callback'])
            for key in ['source', 'asc', 'target', 'reaction', 'input', 'health']:
                scope.pop(key, None)
            print('RESONANCE_LAUNCH_SMOKE_CLEANUP ' + str(report['error']))
            return
        assert time.monotonic() - scope['wall'] < 70, '共鸣解放 Fly 测试超时'
        if len(worlds) != 1:
            return
        world = worlds[0]
        now = unreal.GameplayStatics.get_time_seconds(world)
        if scope['phase'] == 'boot':
            source = unreal.GameplayStatics.get_player_pawn(world, 0)
            if not isinstance(source, unreal.HodgeHeroCharacter):
                return
            asc = source.get_hodge_ability_system_component()
            if not asc or not any(asc.find_ability_definition(h) == definition for h in asc.get_all_abilities()):
                return
            cls = unreal.load_asset('/Game/CodexText/HitReactionValidation/Dummy/BP_HitReactionDummy').generated_class()
            targets = unreal.GameplayStatics.get_all_actors_of_class(world, cls)
            if len(targets) != 1 or not targets[0].initialize_enemy_ability_system():
                return
            target = targets[0]
            reaction = target.get_component_by_class(unreal.HodgeHitReactionComponent)
            if not reaction.get_profile():
                return
            source.set_actor_location(unreal.Vector(0, 0, 95), False, True)
            source.set_actor_rotation(unreal.Rotator(), False)
            source.get_controller().set_control_rotation(unreal.Rotator())
            target.set_actor_location(unreal.Vector(180, 0, 95), False, True)
            target.set_actor_rotation(unreal.Rotator(yaw=180), False)
            system = unreal.get_default_object(unreal.load_class(None, '/Script/Engine.SubsystemBlueprintLibrary'))
            inputs = system.call_method('GetLocalPlayerSubSystemFromPlayerController',
                                        (source.get_controller(), unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
            health = target.get_component_by_class(unreal.HodgeHealthComponent)
            profile = reaction.get_profile()
            if profile.get_editor_property('bUseCompactAnimations'):
                # 当前布局不读隐藏旧数组；测试也按实际 Fly 家族构造预期片段。
                family = profile.get_editor_property('Compact').get_editor_property('Fly')
                launch = unreal.HodgeHitReactionAnimation()
                launch.set_editor_property('Montage', family.get_editor_property('Start'))
                launch.set_editor_property('AirLoopMontage', family.get_editor_property('Loop'))
                launch.set_editor_property('LandingMontage', family.get_editor_property('Landing'))
            else:
                launch = next(e for e in profile.get_editor_property('Animations') if e.type == unreal.HodgeImpactType.LAUNCH)
            report['root_motion_migration'] = launch.landing_motion_source == unreal.HodgeReactionMotionSource.ANIMATION_ROOT_MOTION
            scope.update(source=source, asc=asc, target=target, reaction=reaction, input=inputs, health=health,
                         before_health=health.get_health(), launch=launch, max_z=target.get_actor_location().z,
                         origin_z=target.get_actor_location().z, phase='fill')
            energy = unreal.HodgeCombatValidationLibrary.inspect_resonance_energy(source)
            check('正式 R 已授予且容量有效', energy.y > 0, list(energy.to_tuple()))
            assert unreal.HodgeCombatValidationLibrary.queue_resonance_energy_delta(source, energy.y)
            return
        if scope['phase'] == 'fill':
            energy = unreal.HodgeCombatValidationLibrary.inspect_resonance_energy(scope['source'])
            if energy.x < energy.y:
                return
            scope.update(phase='press', pressed_at=now)
        if scope['phase'] == 'press':
            scope['input'].inject_input_vector_for_action(action, unreal.Vector(1, 0, 0), [], [])
            if now - scope['pressed_at'] > .12:
                scope['input'].inject_input_vector_for_action(action, unreal.Vector(), [], [])
                scope['phase'] = 'observe'
        target, reaction = scope['target'], scope['reaction']
        state = reaction.get_reaction_state()
        location = target.get_actor_location()
        scope['max_z'] = max(scope['max_z'], location.z)
        scope['seen_falling'] |= target.character_movement.is_falling()
        selected = state.montage
        active = state.phase != unreal.HodgeHitReactionPhase.NONE
        if selected == scope['launch'].montage and active:
            scope['seen_start'] = True
        if selected == scope['launch'].air_loop_montage and active:
            scope['seen_loop'] = True
        if selected == scope['launch'].landing_montage and active and not scope['seen_landing']:
            check('Fly_Fall 仅在真实落地后进入', scope['seen_falling'] and target.character_movement.is_moving_on_ground())
            scope['seen_landing'] = True
        result = reaction.get_last_result()
        if result.actual_damage > 0:
            scope['hit_ids'].add(result.hit_id.export_text())
            scope['accepted_damage'] = result.actual_damage
            scope['outcome'] = str(result.outcome)
            scope['applied_launch'] = unreal.HodgeImpactType.LAUNCH in list(result.applied_impacts)
        if now - scope['last_sample'] > .04:
            row = {'time': now, 'phase': str(state.phase), 'montage': selected.get_path_name() if selected else None,
                   'grounded': target.character_movement.is_moving_on_ground(), 'falling': target.character_movement.is_falling(),
                   'z': location.z, 'health': scope['health'].get_health(), 'controlled': reaction.is_controlled()}
            if selected:
                pose = unreal.HodgeCombatValidationLibrary.inspect_montage_state(target, selected)
                row.update(montage_position=pose.position, montage_weight=pose.weight)
            report['samples'].append(row)
            scope['last_sample'] = now
        if scope['seen_landing'] and not reaction.is_controlled():
            energy = unreal.HodgeCombatValidationLibrary.inspect_resonance_energy(scope['source'])
            loss = scope['before_health'] - scope['health'].get_health()
            check('R 经正式消耗后释放', energy.x < energy.y, list(energy.to_tuple()))
            check('一次真实伤害结算包含 Launch', len(scope['hit_ids']) == 1 and scope.get('applied_launch', False),
                  {'hits': len(scope['hit_ids']), 'outcome': scope.get('outcome')})
            check('实际扣血与接受伤害一致', loss > 0 and abs(loss - scope['accepted_damage']) < .1, loss)
            check('Fly_Start 与 Fly_Loop 均实际进入', scope['seen_start'] and scope['seen_loop'])
            weights = {}
            for field in ['montage', 'air_loop_montage', 'landing_montage']:
                path = getattr(scope['launch'], field).get_path_name()
                weights[field] = max((row.get('montage_weight', 0) for row in report['samples'] if row['montage'] == path), default=0)
            check('Fly 三段均存在实际 Montage 权重', all(value > .01 for value in weights.values()), weights)
            check('胶囊真实离地', scope['max_z'] - scope['origin_z'] > 20, scope['max_z'] - scope['origin_z'])
            check('落地恢复后无控制残留', target.character_movement.is_moving_on_ground() and not reaction.is_controlled())
            finish()
    except Exception:
        finish(traceback.format_exc())


# 使用项目既有入口启动真正的游戏 PIE，不进入模拟模式。
scope['callback'] = unreal.register_slate_post_tick_callback(tick)
editor.editor_request_begin_play()
print('RESONANCE_LAUNCH_SMOKE_STARTED')
