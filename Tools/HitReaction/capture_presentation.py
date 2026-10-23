"""通过正式连段意图记录同场景动作序列；截图帧率按实际时间报告。"""
import json
import re
import sys
import time
import traceback
from pathlib import Path

import unreal

STAGE = globals().get('CAPTURE_STAGE', 'Before')
DENSE = globals().get('CAPTURE_FIXED_STEP', False)
BASELINE = globals().get('CAPTURE_BASELINE_ASSETS', False)
WALL_TIMEOUT = globals().get('CAPTURE_WALL_TIMEOUT', 180)
OUT = (Path(unreal.Paths.project_saved_dir()) / 'HitPresentation' / STAGE).resolve()
OUT.mkdir(parents=True, exist_ok=True)
sys.path.insert(0, str(Path(unreal.Paths.project_dir()) / 'Tools' / 'HitReactionDummy'))
import control

editor = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
play = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.LevelEditorPlaySettings'))
perf = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.EditorPerformanceSettings'))
state = {'boot': time.monotonic(), 'rows': [], 'frames': [], 'last_input': -1.0, 'last_capture': -1.0,
         'settings': {n: play.get_editor_property(n) for n in ['PlayNumberOfClients', 'RunUnderOneProcess']},
         'throttle': perf.get_editor_property('bThrottleCPUWhenNotForeground')}
try:
    state['mode_before'] = int(play.get_editor_property('PlayNetMode').value)
except TypeError as error:
    state['mode_before'] = int(re.search(r"Cannot pythonize '(\d+)'", str(error)).group(1))
state['cvars'] = {n: unreal.SystemLibrary.get_console_variable_int_value(n) for n in ['Slate.bAllowThrottling', 't.IdleWhenNotForeground']}
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(1, False)
perf.set_editor_property('bThrottleCPUWhenNotForeground', False)
editor_world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
for name in state['cvars']:
    unreal.SystemLibrary.execute_console_command(editor_world, name + ' 0')
assert unreal.EditorLoadingAndSavingUtils.load_map('/Game/Tests/Combat/L_HitReactionDummy')
state['restore_assets'] = []
if BASELINE:
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages(), '先保留已有未保存资产，禁止在其上运行基线覆盖'
    # 仅在内存还原备份配置；运行时代码仍为最新版本，不能标为旧代码版本录像。
    for number in range(1, 6):
        name = 'DA_Attack_' + str(number)
        asset = unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/' + name)
        old = unreal.load_asset('/Game/Tests/Combat/HitPresentationBackups/B20261016/' + name)
        state['restore_assets'].append((asset, 'DefaultHitConfig', asset.get_editor_property('DefaultHitConfig').export_text()))
        asset.set_editor_property('DefaultHitConfig', old.get_editor_property('DefaultHitConfig'))
    for path in ['/Game/Main/Character/Hero/Anim/HitReactions/DA_Hero_HitReaction',
                 '/Game/CodexText/HitReactionValidation/Dummy/DA_DummyHitReaction']:
        asset = unreal.load_asset(path)
        for key in ['DirectionalLightFeedback', 'ContactEffect', 'ContactSound']:
            stored = [e.export_text() for e in asset.get_editor_property(key)] if key == 'DirectionalLightFeedback' else asset.get_editor_property(key)
            state['restore_assets'].append((asset, key, stored))
            asset.set_editor_property(key, [] if key == 'DirectionalLightFeedback' else None)


def tick(delta):
    """只操作 PIE 副本；真实动作每帧记录，渲染截图记录自身时间。"""
    if state.get('capturing'):
        return
    try:
        worlds = unreal.EditorLevelLibrary.get_pie_worlds(True)
        if state.get('paused_for_capture'):
            if Path(state['pending_frame']).exists():
                unreal.GameplayStatics.set_game_paused(state['world'], False)
                state['paused_for_capture'] = False
            elif time.monotonic() - state['capture_wall'] > 15:
                raise RuntimeError('渲染帧未写入，停止离线采集')
            return
        if state.get('closing'):
            if worlds:
                return
            restored = []
            for asset, key, stored in state['restore_assets']:
                if key == 'DefaultHitConfig':
                    value = unreal.HodgeHitEffectConfig()
                    value.import_text(stored)
                elif key == 'DirectionalLightFeedback':
                    value = []
                    for text in stored:
                        entry = unreal.HodgeDirectionalHitFeedback()
                        entry.import_text(text)
                        value.append(entry)
                else:
                    value = stored
                asset.set_editor_property(key, value)
                if key == 'DefaultHitConfig':
                    assert asset.get_editor_property(key).export_text() == stored
                if asset not in restored:
                    restored.append(asset)
            # 基线只改内存，禁止为恢复临时覆盖而重存正式包或争抢另一编辑器文件锁。
            if restored:
                packages = [asset.get_outermost() for asset in restored]
                changed, error = unreal.EditorLoadingAndSavingUtils.reload_packages(packages, unreal.ReloadPackagesInteractionMode.ASSUME_POSITIVE)
                assert changed and not str(error), str(error)
            state['restore_assets'] = []
            assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(1 if state['mode_before'] == 0 else 2, state['mode_before'] == 2)
            for name, value in state['settings'].items():
                play.set_editor_property(name, value)
            perf.set_editor_property('bThrottleCPUWhenNotForeground', state['throttle'])
            unreal.unregister_slate_post_tick_callback(state['callback'])
            return
        if time.monotonic() - state['boot'] > WALL_TIMEOUT:
            raise RuntimeError('动作采集超时')
        if not worlds:
            return
        world = worlds[0]
        if 'world' not in state:
            source = unreal.GameplayStatics.get_player_pawn(world, 0)
            if not isinstance(source, unreal.HodgeHeroCharacter) or not source.get_hodge_ability_system_component():
                return
            targets = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.load_asset(control.ROOT + '/BP_HitReactionDummy').generated_class())
            if len(targets) != 1 or not targets[0].initialize_enemy_ability_system():
                return
            control.start()
            control.reset()
            state.update(world=world, source=source, target=targets[0], start=unreal.GameplayStatics.get_time_seconds(world))
            # 复用正式玩家镜头，前后采集保持同一默认相机配置。
            cameras = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.CameraActor)
            if cameras:
                unreal.GameplayStatics.get_player_controller(world, 0).set_view_target_with_blend(cameras[0], 0)
            for cvar in ['Hodge.Combat.DebugDraw.Weapon', 'Hodge.Combat.DebugDraw.Body', 'Hodge.Combat.DebugDraw.HitBox']:
                state.setdefault('cvars', {})[cvar] = unreal.SystemLibrary.get_console_variable_int_value(cvar)
                unreal.SystemLibrary.execute_console_command(world, cvar + ' 0')
            if DENSE:
                state['override_fps'] = unreal.SystemLibrary.get_console_variable_float_value('t.OverrideFPS')
                unreal.SystemLibrary.execute_console_command(world, 't.OverrideFPS 60')
        elapsed = unreal.GameplayStatics.get_time_seconds(world) - state['start']
        if elapsed > 7.0:
            finish()
            return
        combat = state['source'].get_component_by_class(unreal.HodgeCombatComponentBase)
        if elapsed > 0.5 and elapsed - state['last_input'] >= 0.16 and elapsed < 5.2:
            intent = unreal.GameplayTag()
            intent.import_text('(TagName="InputIntent.Attack.Light")')
            combat.submit_combat_intent(intent)
            state['last_input'] = elapsed
        anim = state['source'].mesh.get_anim_instance()
        montage = anim.get_current_active_montage()
        row = control.snapshot()
        row.update(elapsed=elapsed, combo=combat.get_current_combo_tag().export_text(), source_montage=montage.get_name() if montage else None)
        reaction = control._state['reaction']
        reaction_state = reaction.get_reaction_state()
        contact = reaction.get_last_presentation()
        target_montage = state['target'].mesh.get_anim_instance().get_current_active_montage()
        target_montage = target_montage or state.get('last_target_montage')
        observed = unreal.HodgeCombatValidationLibrary.inspect_montage_state(state['target'], target_montage)
        if observed.instance_id < 0:
            target_montage = None
        state['last_target_montage'] = target_montage
        result = reaction.get_last_result()
        row.update(presentation_count=reaction.get_presentation_count(), hit_id=contact.hit_id.export_text(),
                   precise=contact.precise_contact if hasattr(contact, 'precise_contact') else contact.get_editor_property('bPreciseContact'),
                   target_montage=target_montage.get_name() if target_montage else None,
                   started_at=reaction_state.started_at, ends_at=reaction_state.ends_at,
                   outcome=str(result.outcome), attack_rank=result.attack_rank, body_rank=result.target_body_rank,
                   attack_phase=str(contact.attack_phase), strength=contact.strength, execution_id=contact.execution_id.export_text(),
                   impact_point=[contact.location.x, contact.location.y, contact.location.z],
                   target_position=observed.position, target_weight=observed.weight, target_instance=observed.instance_id)
        state['rows'].append(row)
        if elapsed - state['last_capture'] >= ((1.0 / 60.0 - 0.001) if DENSE else 0.08):
            filename = str(OUT / ('frame_%04d.png' % len(state['frames'])))
            state['frames'].append({'elapsed': elapsed, 'wall': time.monotonic(), 'path': filename})
            state['last_capture'] = elapsed
            # 高分辨率截图会泵入 Slate Tick，先登记身份并阻止回调重入。
            state['capturing'] = True
            try:
                if DENSE:
                    unreal.GameplayStatics.set_game_paused(world, True)
                    state['paused_for_capture'] = True
                state['pending_frame'] = filename
                state['capture_wall'] = time.monotonic()
                unreal.SystemLibrary.execute_console_command(world, 'HighResShot 960x540 filename="' + filename.replace('\\', '/') + '"')
            finally:
                state['capturing'] = False
    except Exception:
        if state.get('closing'):
            # 收尾失败不再次访问已销毁的 PIE World，也不覆盖已完整采集的帧记录。
            (OUT / 'cleanup-error.txt').write_text(traceback.format_exc(), encoding='utf-8')
            unreal.unregister_slate_post_tick_callback(state['callback'])
        else:
            finish(traceback.format_exc())


def finish(error=None):
    """结束运行并保存证据，不保存测试世界中的站位与相机。"""
    state['closing'] = True
    state['paused_for_capture'] = False
    report = {k: state[k] for k in ['rows', 'frames']}
    report.update(error=error, input='formal SubmitCombatIntent; not physical input', stage=STAGE)
    report['fixed_simulation_step'] = 1.0 / 60.0 if DENSE else None
    report['baseline_assets_latest_runtime'] = BASELINE
    (OUT / 'capture.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
    control.stop()
    world = state.get('world') or unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    for name, value in state.get('cvars', {}).items():
        unreal.SystemLibrary.execute_console_command(world, name + ' ' + str(value))
    if 'override_fps' in state:
        unreal.SystemLibrary.execute_console_command(state['world'], 't.OverrideFPS ' + str(state['override_fps']))
        unreal.GameplayStatics.set_game_paused(state['world'], False)
    editor.editor_request_end_play()


state['callback'] = unreal.register_slate_post_tick_callback(tick)
editor.editor_request_begin_play()
print('CAPTURE_STARTED ' + STAGE)
