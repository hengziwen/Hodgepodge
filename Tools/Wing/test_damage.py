"""Place an initialized enemy at the real wing tip during the formal finisher hit window."""
import json
import time
import traceback
from pathlib import Path

import unreal

world = next(w for w in unreal.EditorLevelLibrary.get_pie_worlds(False) if unreal.SystemLibrary.is_server(w))
hero = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.HodgeHeroCharacter)[0]
enemy = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.HodgeEnemyCharacter)[0]
enemy.character_movement.stop_movement_immediately()
if enemy.get_controller():
    enemy.get_controller().stop_movement()
    enemy.get_controller().brain_component.stop_logic('Wing damage validation')
wing = hero.get_component_by_class(unreal.HodgeWingComponent)
profile = wing.profile
health = enemy.get_component_by_class(unreal.HodgeHealthComponent)
lib = unreal.get_default_object(unreal.load_class(None, '/Script/Engine.SubsystemBlueprintLibrary'))
inputs = lib.call_method('GetLocalPlayerSubSystemFromPlayerController', (hero.get_controller(), unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
action = unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack')
state = {'start': time.monotonic(), 'phase': 'combo', 'last_input': -100., 'hp_before': health.get_health()}
samples = []
checks = []
assert state['hp_before'] > 0


def finish(error=None):
    unreal.unregister_slate_post_tick_callback(state['callback'])
    hero.mesh.get_anim_instance().montage_resume(profile.attack_montage)
    result = {'checks': checks, 'samples': samples, 'error': error}
    Path('E:/Project/Git/Hodgepodge/Saved/WingImplementation/damage.json').write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
    print(json.dumps({'checks': checks, 'error': error}, ensure_ascii=False))


def tick(delta):
    try:
        assert time.monotonic() - state['start'] < 35, 'Damage validation timed out'
        now = unreal.GameplayStatics.get_time_seconds(world)
        info = unreal.HodgeCombatValidationLibrary.inspect_montage_state(hero, profile.attack_montage)
        samples.append({'time': now, 'position': wing.source_position, 'hp': health.get_health(), 'phase': state['phase']})
        if state['phase'] == 'combo':
            if info.instance_id < 0 and now - state['last_input'] > .12:
                inputs.inject_input_vector_for_action(action, unreal.Vector(1, 0, 0), [], [])
                state['last_input'] = now
            if info.instance_id >= 0 and profile.strike_start + .04 < wing.source_position < profile.strike_peak:
                hero.mesh.get_anim_instance().montage_pause(profile.attack_montage)
                state.update({'phase': 'settle', 'pause_time': now, 'position': wing.source_position})
        elif state['phase'] == 'settle' and now - state['pause_time'] > .08:
            tip = wing.get_wing_mesh().get_socket_location('WingHitTip')
            state['tip'] = [tip.x, tip.y, tip.z]
            enemy.set_actor_location(tip, False, True)
            state.update({'phase': 'hit', 'hit_time': now})
        elif state['phase'] == 'hit' and now - state['hit_time'] > .25:
            checks.append({'case': 'paused_montage_holds_wing_clock', 'passed': abs(wing.source_position - state['position']) < .01})
            checks.append({'case': 'wing_tip_server_damage', 'passed': health.get_health() < state['hp_before'],
                           'before': state['hp_before'], 'after': health.get_health(), 'tip': state['tip']})
            changes = [b['hp'] - a['hp'] for a, b in zip(samples, samples[1:]) if b['hp'] < a['hp']]
            checks.append({'case': 'persistent_overlap_not_repeated', 'passed': len(changes) == 1, 'damage_changes': changes})
            assert all(c['passed'] for c in checks), checks
            finish()
    except Exception:
        finish(traceback.format_exc())


state['callback'] = unreal.register_slate_post_tick_callback(tick)
print('Started actual wing-tip damage validation')
