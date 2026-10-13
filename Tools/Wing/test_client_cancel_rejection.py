"""Verify that allowing authority termination does not authorize client combo termination."""
import json
import time
import traceback
from pathlib import Path
import unreal

worlds = unreal.EditorLevelLibrary.get_pie_worlds(True)
server = next(w for w in worlds if unreal.SystemLibrary.is_server(w))
client = next(w for w in worlds if not unreal.SystemLibrary.is_server(w))
local = next(a for a in unreal.GameplayStatics.get_all_actors_of_class(client, unreal.HodgeHeroCharacter)
             if a.get_local_role() == unreal.NetRole.ROLE_AUTONOMOUS_PROXY)
pid = local.get_editor_property('PlayerState').get_editor_property('PlayerId')
hero = next(a for a in unreal.GameplayStatics.get_all_actors_of_class(server, unreal.HodgeHeroCharacter)
            if a.get_editor_property('PlayerState').get_editor_property('PlayerId') == pid)
profile = local.get_component_by_class(unreal.HodgeWingComponent).profile
definition = unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_4')
asc = local.get_hodge_ability_system_component()
handle = next(h for h in asc.get_all_abilities() if asc.find_ability_definition(h) == definition)
lib = unreal.get_default_object(unreal.load_class(None, '/Script/Engine.SubsystemBlueprintLibrary'))
inputs = lib.call_method('GetLocalPlayerSubSystemFromPlayerController', (local.get_controller(), unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
action = unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack')
state = {'wall': time.monotonic(), 'last_input': -100., 'requested': False}


def finish(result):
    unreal.unregister_slate_post_tick_callback(state['callback'])
    hero.mesh.get_anim_instance().montage_stop(.1, profile.attack_montage)
    Path('E:/Project/Git/Hodgepodge/Saved/WingImplementation/client-cancel-rejection.json').write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
    print(json.dumps(result, ensure_ascii=False))


def tick(delta):
    try:
        assert time.monotonic() - state['wall'] < 30, 'Client cancellation validation timed out'
        now = unreal.GameplayStatics.get_time_seconds(server)
        server_info = unreal.HodgeCombatValidationLibrary.inspect_montage_state(hero, profile.attack_montage)
        local_info = unreal.HodgeCombatValidationLibrary.inspect_montage_state(local, profile.attack_montage)
        if not state['requested']:
            if local_info.instance_id < 0 and now - state['last_input'] > .12:
                inputs.inject_input_vector_for_action(action, unreal.Vector(1, 0, 0), [], [])
                state['last_input'] = now
            if server_info.instance_id >= 0 and local_info.instance_id >= 0 and .2 < server_info.position < .6:
                assert unreal.HodgeCombatValidationLibrary.queue_ability_action(asc, handle, True)
                state.update({'requested': True, 'request_time': now, 'instance': server_info.instance_id})
        elif now - state['request_time'] > .2:
            passed = server_info.instance_id == state['instance'] and not server_info.stopped
            finish({'passed': passed, 'case': 'client_cannot_end_authority_combo', 'server_instance': server_info.instance_id,
                    'server_position': server_info.position, 'server_stopped': server_info.stopped})
    except Exception:
        finish({'passed': False, 'error': traceback.format_exc()})


state['callback'] = unreal.register_slate_post_tick_callback(tick)
print('Started unauthorized client termination validation')
