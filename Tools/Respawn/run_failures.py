"""仅单人 PIE 临时阻挡出生点，验证真实生成失败、有界重试和取消；退出时恢复内存配置。"""
from pathlib import Path
from datetime import datetime
import gc
import json
import re
import time
import traceback
import unreal

WORK = Path(globals()['LOCK_ON_WORK'])
world = next(w for w in unreal.EditorLevelLibrary.get_pie_worlds(True) if unreal.SystemLibrary.is_server(w))
pc = unreal.GameplayStatics.get_player_controller(world, 0)
ps = pc.get_editor_property('PlayerState')
data = unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData')
cdo = unreal.get_default_object(data.get_editor_property('PawnClass'))
original_handling = cdo.get_editor_property('SpawnCollisionHandlingMethod')
original_enabled = data.get_editor_property('bEnableAutoRespawn')
log_file = WORK.parent / 'RuntimeEditor.log'
checks, samples = [], []
state = {'phase': 'warmup', 'at': 0., 'wall': time.monotonic(), 'blocker': None, 'case': None}


def check(name, passed, detail=None):
    """故障必须产生真实失败与停止证据，不能只看最后是否有新 Pawn。"""
    checks.append({'name': name, 'passed': bool(passed), 'detail': detail})
    assert passed, (name, detail)


def restore():
    """只恢复本用例的内存覆盖，不保存或重编任何正式资产。"""
    cdo.set_editor_property('SpawnCollisionHandlingMethod', original_handling)
    data.set_editor_property('bEnableAutoRespawn', original_enabled)
    if state['blocker'] and unreal.SystemLibrary.is_valid(state['blocker']):
        state['blocker'].destroy_actor()
    state['blocker'] = None


def records():
    """完整读取本轮原生日志；保留失败日志，不过滤掉引擎的生成拒绝。"""
    text = log_file.read_text(encoding='utf-8-sig', errors='replace')
    tail = text.partition(state['marker'])[2] if state.get('marker') else ''
    matches = re.findall(r'\[([0-9.:-]+)\].*AutoRespawn attempt .*attempt=(\d+)/(\d+)', tail)
    rows = [(datetime.strptime(stamp, '%Y.%m.%d-%H.%M.%S:%f').timestamp(), int(index), int(limit))
            for stamp, index, limit in matches]
    return rows, tail.count('Game mode was unable to spawn Pawn of class'), tail


def step(phase, now):
    state['phase'], state['at'] = phase, now


def begin(case, now):
    """每个故障独立标记后走真实 Damage/Death GA，不能主动结束死亡表现。"""
    state['case'] = case
    state['marker'] = 'AUTO_RESPAWN_FAULT_CASE_' + case + '_' + str(time.time_ns())
    unreal.log(state['marker'])
    pawn = pc.get_controlled_pawn()
    state['old'] = pawn.get_path_name()
    if case == 'cancel':
        data.set_editor_property('bEnableAutoRespawn', False)
    else:
        cdo.set_editor_property('SpawnCollisionHandlingMethod', unreal.SpawnActorCollisionHandlingMethod.DONT_SPAWN_IF_COLLIDING)
    assert unreal.HodgeCombatValidationLibrary.queue_number_pop_probe(pawn, pawn, 1000000., False)
    step('dying', now)


def block_starts():
    """简单物理盒完全覆盖验证地图，确保 DontSpawnIfColliding 走实际 Spawn 失败。"""
    blocker = unreal.HodgeCombatValidationLibrary.spawn_hit_reaction_validation_actor(world, unreal.StaticMeshActor, unreal.Vector())
    assert blocker
    mesh = blocker.static_mesh_component
    mesh.set_mobility(unreal.ComponentMobility.MOVABLE)
    assert mesh.set_static_mesh(unreal.load_asset('/Engine/BasicShapes/Cube'))
    mesh.set_collision_profile_name('BlockAll')
    mesh.set_collision_enabled(unreal.CollisionEnabled.QUERY_AND_PHYSICS)
    blocker.set_actor_scale3d(unreal.Vector(10000, 10000, 10000))
    state['blocker'] = blocker


def ready():
    """生成成功与初始化就绪分别判定。"""
    pawn = pc.get_controlled_pawn()
    value = ps.get_attribute_ready_state()
    return pawn and pawn.get_path_name() != state.get('old') and value.ready and value.avatar == pawn


def finish(error=None):
    """异常和正常结束都恢复覆盖、销毁盒并释放世界引用。"""
    restore()
    unreal.unregister_slate_post_tick_callback(state['callback'])
    result = {'mode': 'single-fault-injection', 'passed': error is None, 'checks': checks,
              'samples': samples, 'error': error, 'elapsed': time.monotonic() - state['wall'], 'saved_assets': False}
    (WORK / 'single-respawn-e2e.json').write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
    state.clear()
    for name in ['world', 'pc', 'ps', 'data', 'cdo']:
        globals()[name] = None
    gc.collect()


def tick(delta):
    """以原生尝试日志和真实 Pawn 状态验证，而非重复模拟生产调度算法。"""
    try:
        assert time.monotonic() - state['wall'] < 75., '故障专项超时：' + state['phase']
        now = unreal.GameplayStatics.get_time_seconds(world)
        phase, age = state['phase'], now - state['at']
        if phase == 'warmup':
            if state['at'] == 0.: state['at'] = now
            if age > .5: begin('recover', now)
        elif phase == 'dying':
            pawn = pc.get_controlled_pawn()
            if pawn and pawn.get_component_by_class(unreal.HodgeHealthComponent).is_dead_or_dying():
                if state['case'] != 'cancel': block_starts()
                step('attempts', now)
        elif phase == 'attempts':
            rows, failures, tail = records()
            samples.append({'time': now, 'case': state['case'], 'attempts': [row[1] for row in rows], 'spawn_failures': failures,
                            'pawn': pc.get_controlled_pawn().get_path_name() if pc.get_controlled_pawn() else None})
            if state['case'] == 'recover' and failures == 1:
                check('第一刀死亡后的生成确实被物理盒拒绝', len(rows) == 1)
                restore()
                step('recovered', now)
            elif state['case'] == 'exhaust' and len(rows) >= 3:
                check('总尝试计数明确为1、2、3', [row[1] for row in rows] == [1, 2, 3] and all(row[2] == 3 for row in rows), rows)
                check('两次重试间隔不短于配置', all(rows[i][0] - rows[i-1][0] >= .48 for i in [1, 2]), rows)
                step('stopped', now)
            elif state['case'] == 'cancel' and age > 5.:
                check('配置停用使已绑定旧段不能生成', not pc.get_controlled_pawn() and len(rows) == 0, rows)
                restore()
                finish()
        elif phase == 'recovered' and ready():
            rows, failures, tail = records()
            # 原生日志异步刷盘可能晚于同帧 Possess/Ready，等待证据到齐而不跳过任何尝试。
            if len(rows) < 2 and age < 3.:
                return
            check('第二次受检尝试恢复生成', [row[1] for row in rows] == [1, 2] and failures == 1, rows)
            check('恢复仍等到原0.5秒重试窗口', rows[1][0] - rows[0][0] >= .48, rows)
            begin('exhaust', now)
        elif phase == 'stopped' and age > 1.3:
            rows, failures, tail = records()
            check('耗尽后停止且没有原GameMode每帧重试', len(rows) == 3 and failures == 3 and not pc.get_controlled_pawn() and 'AutoRespawn exhausted' in tail,
                  {'attempts': rows, 'spawn_failures': failures})
            restore()
            # 只用既有外部入口恢复夹具，证明停止后仍可被明确操作重新生成。
            unreal.GameplayStatics.get_game_mode(world).restart_player(pc)
            step('manual_restore', now)
        elif phase == 'manual_restore' and ready():
            check('既有外部Restart入口仍可用', True)
            begin('cancel', now)
    except Exception:
        finish(traceback.format_exc())


state['callback'] = unreal.register_slate_post_tick_callback(tick)
