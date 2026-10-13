"""Check both network roles have no wing and ordinary five-hit input still works."""
import json
import time
import traceback
from pathlib import Path
import unreal

worlds = unreal.EditorLevelLibrary.get_pie_worlds(True)
server = next(w for w in worlds if unreal.SystemLibrary.is_server(w))
clients = [w for w in worlds if w != server]
local = next(a for a in unreal.GameplayStatics.get_all_actors_of_class(clients[0] if clients else server, unreal.HodgeHeroCharacter)
             if not clients or a.get_local_role() == unreal.NetRole.ROLE_AUTONOMOUS_PROXY)
lib = unreal.get_default_object(unreal.load_class(None, '/Script/Engine.SubsystemBlueprintLibrary'))
inputs = lib.call_method('GetLocalPlayerSubSystemFromPlayerController', (local.get_controller(), unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
action = unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack')
for enemy in unreal.GameplayStatics.get_all_actors_of_class(server, unreal.HodgeEnemyCharacter):
    if enemy.get_controller():
        enemy.get_controller().brain_component.stop_logic('Wing disabled verification')
    enemy.set_actor_location(unreal.Vector(2300, 2300, 100), False, True)
checks = []
for world in worlds:
    for hero in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.HodgeHeroCharacter):
        no_component = not hero.get_components_by_class(unreal.HodgeWingComponent)
        no_mesh = not any(str(c.get_fname()) == 'WingMesh' for c in hero.get_components_by_class(unreal.SkeletalMeshComponent))
        assert no_component and no_mesh, hero.get_path_name()
        checks.append({'actor': hero.get_path_name(), 'no_wing_component': no_component, 'no_wing_mesh': no_mesh})
state = {'wall': time.monotonic(), 'start': unreal.GameplayStatics.get_time_seconds(server), 'last_input': -100., 'seen': set()}


def finish(error=None):
    unreal.unregister_slate_post_tick_callback(state['callback'])
    result = {'checks': checks, 'combo_nodes': sorted(state['seen']), 'error': error, 'worlds': len(worlds)}
    Path('E:/Project/Git/Hodgepodge/Saved/WingDisable/pie.json').write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
    print(json.dumps(result, ensure_ascii=False))


def tick(delta):
    try:
        assert time.monotonic() - state['wall'] < 30, 'Five-hit input verification timed out'
        now = unreal.GameplayStatics.get_time_seconds(server)
        node = str(local.get_component_by_class(unreal.HodgeCombatComponentBase).get_current_combo_tag().get_editor_property('TagName'))
        state['seen'].add(node)
        expected = {'Combo.Light.' + n for n in ['01', '02', '03', '05', '04']}
        if expected.issubset(state['seen']) and now - state['start'] > 5:
            finish()
            return
        if now - state['last_input'] > .12:
            inputs.inject_input_vector_for_action(action, unreal.Vector(1, 0, 0), [], [])
            state['last_input'] = now
    except Exception:
        finish(traceback.format_exc())


state['callback'] = unreal.register_slate_post_tick_callback(tick)
print('Started disabled-wing network and normal combo verification')
