"""冷启动双客户端夹具，复用项目现有授予与原生调度入口，不修改正式资产。"""
from pathlib import Path
import builtins
import re
import time
import traceback
import unreal

assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
root = Path(unreal.Paths.project_dir())
settings = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.LevelEditorPlaySettings'))
performance = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.EditorPerformanceSettings'))
saved = {name: settings.get_editor_property(name) for name in ['PlayNumberOfClients', 'RunUnderOneProcess']}
try:
    mode = int(settings.get_editor_property('PlayNetMode').value)
except TypeError as error:
    mode = int(re.search(r"Cannot pythonize '(\d+)'", str(error)).group(1))
builtins.HODGE_ACT_PREFERENCES = {'values': saved, 'mode': mode,
                                'throttle': performance.get_editor_property('bThrottleCPUWhenNotForeground')}
assert unreal.HodgeCombatValidationLibrary.configure_validation_pie(2, True)
performance.set_editor_property('bThrottleCPUWhenNotForeground', False)
assert unreal.EditorLoadingAndSavingUtils.load_map('/Game/Tests/Combat/L_HitReactionDummy')
state = {'started': time.monotonic(), 'ready_at': None}


def tick(delta):
    """等待所有真实 Avatar 与测试授予复制就绪，有界失败不无限轮询。"""
    try:
        assert time.monotonic() - state['started'] < 60, '玩家夹具初始化超时'
        worlds = unreal.EditorLevelLibrary.get_pie_worlds(True)
        if len(worlds) != 3:
            return
        if not state.get('actors'):
            cls = unreal.load_asset('/Game/Main/Character/Hero/BP_Hero_Pover').generated_class()
            for world in worlds:
                actors = unreal.GameplayStatics.get_all_actors_of_class(world, cls)
                if len(actors) != 2 or not all(p.get_hodge_ability_system_component() and p.get_component_by_class(unreal.HodgeHitReactionComponent).get_profile() for p in actors):
                    return
            exec((root / 'Tools/HitReaction/setup_two_clients.py').read_text(encoding='utf-8'), {})
            exec((root / 'Tools/HitReaction/setup_actors.py').read_text(encoding='utf-8'), {})
            state['actors'] = True
            state['ready_at'] = time.monotonic() + 1.
        if time.monotonic() < state['ready_at']:
            return
        s = builtins.HODGE_REACTION_TEST
        # 隔离受击运动时三端采用同一 Pawn 碰撞响应，避免客户端单独被另一角色推开。
        for peer_world in worlds:
            for pawn in unreal.GameplayStatics.get_all_actors_of_class(peer_world, unreal.HodgeHeroCharacter):
                pawn.capsule_component.set_collision_response_to_channel(unreal.CollisionChannel.ECC_PAWN, unreal.CollisionResponseType.ECR_OVERLAP)
        for pawn in [s['source_owner'], s['owned_target']]:
            asc = pawn.get_hodge_ability_system_component()
            if not any(asc.find_ability_definition(h) == s['definition'] for h in asc.get_all_abilities()):
                return
        unreal.unregister_slate_post_tick_callback(state['callback'])
        s['act_fixture_ready'] = True
        print('ACT_FIXTURE_READY')
    except Exception:
        unreal.unregister_slate_post_tick_callback(state['callback'])
        builtins.HODGE_ACT_FIXTURE_ERROR = traceback.format_exc()
        unreal.log_error(builtins.HODGE_ACT_FIXTURE_ERROR)


state['callback'] = unreal.register_slate_post_tick_callback(tick)
unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_begin_play()
