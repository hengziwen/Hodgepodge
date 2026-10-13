"""Exercise real player input, wing bone motion and cancellation in single/listen PIE."""
import json
import time
import traceback
from pathlib import Path

import unreal

worlds = unreal.EditorLevelLibrary.get_pie_worlds(True)
server = next(w for w in worlds if unreal.SystemLibrary.is_server(w))
clients = [w for w in worlds if w != server]
hero_class = unreal.HodgeHeroCharacter
local = next(a for a in unreal.GameplayStatics.get_all_actors_of_class(clients[0] if clients else server, hero_class)
             if not clients or a.get_local_role() == unreal.NetRole.ROLE_AUTONOMOUS_PROXY)
player_id = local.get_editor_property('PlayerState').get_editor_property('PlayerId')
hero = next(a for a in unreal.GameplayStatics.get_all_actors_of_class(server, hero_class)
            if a.get_editor_property('PlayerState').get_editor_property('PlayerId') == player_id)
peers = [a for w in worlds for a in unreal.GameplayStatics.get_all_actors_of_class(w, hero_class)
         if a.get_editor_property('PlayerState').get_editor_property('PlayerId') == player_id]
for w in worlds:
    for actor in unreal.GameplayStatics.get_all_actors_of_class(w, hero_class):
        if actor not in peers:
            actor.set_actor_location(unreal.Vector(-2000, -2000, 100), False, True)
for enemy in unreal.GameplayStatics.get_all_actors_of_class(server, unreal.HodgeEnemyCharacter):
    if enemy.get_controller() and enemy.get_controller().brain_component:
        enemy.get_controller().brain_component.stop_logic('Wing validation')
    enemy.set_actor_location(unreal.Vector(2300, 2300, 100), False, True)

profile = unreal.load_asset('/Game/Main/Character/Hero/Wing/DA_PoverWing')
montage = profile.attack_montage
server_asc = hero.get_hodge_ability_system_component()
finisher_definition = unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_4')
finisher_handle = next(h for h in server_asc.get_all_abilities() if server_asc.find_ability_definition(h) == finisher_definition)
action = unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack')
subsystem_lib = unreal.get_default_object(unreal.load_class(None, '/Script/Engine.SubsystemBlueprintLibrary'))
input_system = subsystem_lib.call_method('GetLocalPlayerSubSystemFromPlayerController',
    (local.get_controller(), unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
samples = []
checks = []
name = globals().get('WING_REPORT_NAME', 'listen' if clients else 'single')
state = {'start': unreal.GameplayStatics.get_time_seconds(server), 'wall': time.monotonic(),
         'last_input': -100., 'phase': 'combo', 'seen': set(), 'wing_moved': set(),
         'peak_pitch': 0., 'canceled': False, 'finisher_started': False}
state['bone_points'] = set()


def check(case, passed, **detail):
    checks.append({'case': case, 'passed': bool(passed), **detail})
    assert passed, (case, detail)


def finish(error=None):
    unreal.unregister_slate_post_tick_callback(state['callback'])
    result = {'mode': name, 'error': error, 'checks': checks, 'samples': samples}
    Path('E:/Project/Git/Hodgepodge/Saved/WingImplementation/' + name + '-e2e.json').write_text(
        json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
    print(json.dumps({'mode': name, 'error': error, 'checks': len(checks)}, ensure_ascii=False))


for peer in peers:
    wings = peer.get_components_by_class(unreal.HodgeWingComponent)
    check('one_wing_component_' + str(peer.get_local_role()), len(wings) == 1)
    mesh = wings[0].get_wing_mesh()
    check('wing_registered_' + str(peer.get_local_role()), bool(mesh) and str(mesh.get_fname()) == 'WingMesh')
    check('wing_attach_socket_' + str(peer.get_local_role()), str(mesh.get_attach_socket_name()) == 'WingAttach')
    check('wing_no_blocking_collision_' + str(peer.get_local_role()), mesh.get_collision_enabled() == unreal.CollisionEnabled.NO_COLLISION)
    check('independent_wing_anim_' + str(peer.get_local_role()), isinstance(mesh.get_anim_instance(), unreal.HodgeWingAnimInstance))
    check('idempotent_initialization_' + str(peer.get_local_role()), wings[0].initialize_wing() and wings[0].get_wing_mesh() == mesh)


def tick(delta):
    try:
        now = unreal.GameplayStatics.get_time_seconds(server)
        elapsed = now - state['start']
        assert time.monotonic() - state['wall'] < 65, 'Wing validation timeout'
        wing = hero.get_component_by_class(unreal.HodgeWingComponent)
        local_wing = local.get_component_by_class(unreal.HodgeWingComponent)
        node = local.get_component_by_class(unreal.HodgeCombatComponentBase).get_current_combo_tag().get_editor_property('TagName')
        node = str(node)
        state['seen'].add(node)
        info = unreal.HodgeCombatValidationLibrary.inspect_montage_state(local, montage)
        sample = {'time': elapsed, 'node': node, 'position': wing.source_position,
                  'server_pitch': wing.pose.root.pitch, 'owner_pitch': local_wing.pose.root.pitch,
                  'instances': [p.get_component_by_class(unreal.HodgeWingComponent).source_instance_id for p in peers]}
        samples.append(sample)
        point = wing.get_wing_mesh().get_socket_transform('Wing006', unreal.RelativeTransformSpace.RTS_COMPONENT).translation
        state['bone_points'].add((round(point.x, 1), round(point.y, 1), round(point.z, 1)))
        for index, peer in enumerate(peers):
            current = peer.get_component_by_class(unreal.HodgeWingComponent)
            if abs(current.pose.root.pitch) > 8:
                state['wing_moved'].add(index)
        if state['phase'] == 'combo':
            if info.instance_id < 0 and now - state['last_input'] > .12:
                input_system.inject_input_vector_for_action(action, unreal.Vector(1, 0, 0), [], [])
                state['last_input'] = now
            if info.instance_id >= 0 and info.weight > .02:
                state['finisher_started'] = True
                state['peak_pitch'] = max(state['peak_pitch'], abs(wing.pose.root.pitch))
            if state['finisher_started'] and elapsed > 6 and info.instance_id < 0:
                check('all_five_combo_nodes', all('Combo.Light.' + n in state['seen'] for n in ['01', '02', '03', '05', '04']))
                check('server_wing_pose_moves', state['peak_pitch'] > 20, peak=state['peak_pitch'])
                check('actual_wing_bones_deform', len(state['bone_points']) > 5, positions=len(state['bone_points']))
                check('all_peers_receive_wing_motion', len(state['wing_moved']) == len(peers), peers=len(peers))
                state.update({'phase': 'cancel', 'start': now, 'last_input': -100., 'finisher_started': False})
        elif state['phase'] == 'cancel':
            if info.instance_id < 0 and not state['canceled'] and now - state['last_input'] > .12:
                input_system.inject_input_vector_for_action(action, unreal.Vector(1, 0, 0), [], [])
                state['last_input'] = now
            if info.instance_id >= 0 and wing.source_position > profile.strike_start and abs(wing.pose.root.pitch) > 12 and not state['canceled']:
                assert unreal.HodgeCombatValidationLibrary.queue_ability_action(server_asc, finisher_handle, True)
                state.update({'canceled': True, 'cancel_time': now, 'cancel_pitch': abs(wing.pose.root.pitch)})
            if state['canceled'] and now - state['cancel_time'] > profile.cancel_return_seconds + .3:
                check('cancel_returns_reference', abs(wing.pose.root.pitch) < .1)
                check('cancel_invalidates_source', wing.source_instance_id == -1)
                check('cancel_returns_all_peers', all(abs(p.get_component_by_class(unreal.HodgeWingComponent).pose.root.pitch) < .1 for p in peers))
                check('cancel_closes_hit_sessions', unreal.HodgeCombatValidationLibrary.inspect_hit_sessions(hero) == 0)
                finish()
    except Exception:
        finish(traceback.format_exc())


state['callback'] = unreal.register_slate_post_tick_callback(tick)
print('Started wing ' + name + ' E2E')
