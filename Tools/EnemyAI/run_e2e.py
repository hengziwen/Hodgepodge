"""Verify real preplaced enemies, navigation, GAS attacks and remote animation while PIE ticks."""
import unreal
import builtins
import json
import time
import traceback
from pathlib import Path

worlds = unreal.EditorLevelLibrary.get_pie_worlds(False)
world = next(w for w in worlds if unreal.SystemLibrary.is_server(w))
clients = [w for w in worlds if w != world]
actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.HodgeEnemyCharacter)
enemy = next(a for a in actors if 'Equipped' not in a.get_editor_property('EnemyPawnData').get_name())
equipped = next(a for a in actors if 'Equipped' in a.get_editor_property('EnemyPawnData').get_name())
heroes = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.HodgeHeroCharacter)
assert heroes
hero = heroes[0]
hero_equipment = hero.get_component_by_class(unreal.HodgeEquipmentManagerComponent)
probe_grant = hero_equipment.equip_item(unreal.load_asset('/Game/CodexText/EnemyAIValidation/BP_PlayerAttackGrant').generated_class())
assert probe_grant, hero_equipment.get_last_equip_error()
controller = enemy.get_controller()
init = enemy.get_enemy_init_component()
asc = enemy.get_hodge_ability_system_component()
attack = unreal.load_asset('/Game/Main/Character/Enemies/Guardian/Anim/AM_Attack')
death = unreal.load_asset('/Game/Main/Character/Enemies/Guardian/Anim/AM_Death')
bridge = unreal.get_default_object(unreal.load_class(None, '/Script/GameplayAbilities.AbilitySystemBlueprintLibrary'))
output = Path('E:/Project/Git/Hodgepodge/Saved/EnemyAIImplementation')
name = globals().get('ENEMY_OUTPUT_NAME', 'network' if clients else 'single')
samples = []
results = []
state = {'phase': 'ready', 'start_wall': time.monotonic(), 'start': unreal.GameplayStatics.get_time_seconds(world),
         'instances': set(), 'control_seen': False, 'remote_control_seen': False, 'death_seen': False, 'remote_death_seen': False}
remote = None
if clients:
    remote = next(a for a in unreal.GameplayStatics.get_all_actors_of_class(clients[0], unreal.HodgeEnemyCharacter)
                  if a.get_editor_property('EnemyPawnData') == enemy.get_editor_property('EnemyPawnData'))


def hp(actor):
    return actor.get_component_by_class(unreal.HodgeHealthComponent).get_health()


def clock():
    return unreal.GameplayStatics.get_time_seconds(world)


def tag(value):
    result = unreal.GameplayTag()
    result.import_text('(TagName="' + value + '")')
    return result


def control(add):
    tags = unreal.GameplayTagContainer()
    tags.import_text('(GameplayTags=((TagName="State.Combat.HitReaction.Controlled")))')
    method = 'AddLooseGameplayTags' if add else 'RemoveLooseGameplayTags'
    assert bridge.call_method(method, (enemy, tags, False))


def record(case, passed, **detail):
    results.append({'case': case, 'passed': bool(passed), **detail})
    assert passed, (case, detail)


def move_player(position):
    player_id = hero.get_editor_property('PlayerState').get_editor_property('PlayerId')
    for w in worlds:
        for p in unreal.GameplayStatics.get_all_actors_of_class(w, unreal.HodgeHeroCharacter):
            ps = p.get_editor_property('PlayerState')
            if ps and ps.get_editor_property('PlayerId') == player_id:
                p.character_movement.stop_movement_immediately()
                p.set_actor_location(position, False, True)
                p.set_actor_rotation(unreal.Rotator(0, 0, 0), False)


def finish(error=None):
    unreal.unregister_slate_post_tick_callback(state['callback'])
    if unreal.SystemLibrary.is_valid(probe_grant):
        hero_equipment.unequip_item(probe_grant)
    report = {'name': name, 'network': bool(clients), 'error': error, 'results': results, 'samples': samples,
              'attack_instances': sorted(state['instances']), 'elapsed_wall_s': time.monotonic() - state['start_wall']}
    (output / (name + '-e2e.json')).write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
    builtins.HODGE_ENEMY_LAST_REPORT = report
    print(json.dumps({'report': name, 'passed': sum(r['passed'] for r in results), 'error': error}))


def tick(_delta):
    try:
        if time.monotonic() - state['start_wall'] > 100:
            raise RuntimeError('EnemyAI E2E timed out: ' + state['phase'])
        now = clock()
        phase = state['phase']
        if phase == 'ready':
            if not init.is_gameplay_ready() or not equipped.get_enemy_init_component().is_gameplay_ready():
                assert now - state['start'] < 12, (init.get_initialization_diagnostic(), equipped.get_enemy_init_component().get_initialization_diagnostic())
                return
            record('preplaced_experience_ready', init.has_valid_combat_dependencies(), diagnostic=init.get_initialization_diagnostic())
            record('pawn_owned_asc', asc.get_owner() == enemy and init.has_valid_combat_dependencies())
            record('unarmed_ready', enemy.get_editor_property('EnemyPawnData').get_editor_property('DefaultWeaponDefinition') is None)
            manager = equipped.get_component_by_class(unreal.HodgeEquipmentManagerComponent)
            items = manager.get_equipment_instances_of_type(unreal.HodgeEquipmentInstance)
            record('equipped_ready', len(items) == 1 and len(items[0].get_spawned_actors()) == 1)
            if remote:
                record('remote_presentation_ready', remote.get_enemy_init_component().is_gameplay_ready() and remote.mesh.get_anim_instance() is not None)
            state['abilities'] = len(asc.get_all_abilities())
            state['health'] = hp(enemy)
            for _ in range(5):
                enemy.initialize_enemy_ability_system()
                controller.try_start_brain()
            record('idempotent_callbacks', len(asc.get_all_abilities()) == state['abilities'] and hp(enemy) == state['health'] and controller.get_brain_start_count() == 1)
            move_player(unreal.Vector(0, 0, 100))
            for p in heroes[1:]:
                p.set_actor_location(unreal.Vector(-2400, 2200, 100), False, True)
            enemy.character_movement.stop_movement_immediately()
            enemy.set_actor_location(unreal.Vector(850, 0, 100), False, True)
            enemy.set_actor_rotation(unreal.Rotator(0, 180, 0), False)
            state['distance'] = (enemy.get_actor_location() - hero.get_actor_location()).length()
            state['hero_hp'] = hp(hero)
            state['start'] = now
            state['phase'] = 'chase'
            return
        if phase in ('chase', 'attack'):
            info = unreal.HodgeCombatValidationLibrary.inspect_montage_state(enemy, attack)
            if info.instance_id >= 0 and info.weight > .02:
                state['instances'].add(info.instance_id)
            sample = {'time': now, 'phase': phase, 'distance': (enemy.get_actor_location() - hero.get_actor_location()).length(),
                      'enemy_hp': hp(enemy), 'hero_hp': hp(hero), 'instance': info.instance_id, 'position': info.position,
                      'velocity': enemy.get_velocity().length(), 'brain': str(controller.get_enemy_brain_state())}
            if remote:
                ri = unreal.HodgeCombatValidationLibrary.inspect_montage_state(remote, attack)
                sample['remote_instance'] = ri.instance_id
                sample['remote_weight'] = ri.weight
            samples.append(sample)
            if phase == 'chase' and sample['distance'] < state['distance'] - 200:
                record('navigation_chase', True, initial_distance=state['distance'], distance=sample['distance'])
                state['phase'] = 'attack'
            assert now - state['start'] < 16, ('No chase/attack progress', sample)
            if state['phase'] == 'attack' and len(state['instances']) >= 2 and hp(hero) < state['hero_hp']:
                record('gas_attack_damage', True, initial_hp=state['hero_hp'], current_hp=hp(hero))
                record('task_completion_repeats', True, instances=sorted(state['instances']), duration=now - state['start'])
                if remote:
                    record('remote_attack_animation', any(s.get('remote_weight', 0) > .1 for s in samples))
                control(True)
                move_player(enemy.get_actor_location() + unreal.Vector(-120, 0, 0))
                state['start'] = now
                state['phase'] = 'prepare_hit'
            return
        if phase == 'prepare_hit':
            if now - state['start'] < .8:
                return
            definition = unreal.load_asset('/Game/CodexText/EnemyAIValidation/DA_PlayerAttack')
            ha = hero.get_hodge_ability_system_component()
            handles = [h for h in ha.get_all_abilities() if ha.find_ability_definition(h) == definition]
            assert len(handles) == 1, len(handles)
            state['enemy_hp_before'] = hp(enemy)
            assert unreal.HodgeCombatValidationLibrary.queue_ability_action(ha, handles[0], False)
            state['start'] = now
            state['phase'] = 'hit'
            return
        if phase == 'hit':
            rs = enemy.get_component_by_class(unreal.HodgeHitReactionComponent).get_reaction_state()
            state['control_seen'] |= rs.phase != unreal.HodgeHitReactionPhase.NONE
            if remote:
                state['remote_control_seen'] |= remote.get_component_by_class(unreal.HodgeHitReactionComponent).get_reaction_state().phase != unreal.HodgeHitReactionPhase.NONE
            if now - state['start'] < 1.5:
                return
            record('actual_hit_reaction', hp(enemy) < state['enemy_hp_before'] and state['control_seen'], hp=hp(enemy))
            record('controlled_actions_rejected', not init.can_execute_ai_action())
            if remote:
                record('remote_hit_reaction', state['remote_control_seen'])
            damaged_hp = hp(enemy)
            for _ in range(4):
                enemy.initialize_enemy_ability_system()
            record('repeated_init_does_not_heal', hp(enemy) == damaged_hp, health=damaged_hp)
            control(False)
            state['start'] = now
            state['phase'] = 'recover'
            return
        if phase == 'recover':
            if now - state['start'] < 1.0:
                return
            record('control_recovery', init.can_execute_ai_action() and controller.get_enemy_brain_state() == unreal.HodgeEnemyBrainState.RUNNING)
            assert unreal.HodgeCombatValidationLibrary.queue_gameplay_event(asc, tag('GameplayEvent.Death'))
            state['start'] = now
            state['phase'] = 'death'
            return
        if phase == 'death':
            if unreal.SystemLibrary.is_valid(enemy):
                state['death_seen'] |= controller.get_enemy_brain_state() == unreal.HodgeEnemyBrainState.STOPPED and not init.can_execute_ai_action()
            if remote and unreal.SystemLibrary.is_valid(remote):
                di = unreal.HodgeCombatValidationLibrary.inspect_montage_state(remote, death)
                state['remote_death_seen'] |= di.weight > .05
            if now - state['start'] < 3.0:
                return
            record('death_stops_normal_ai', state['death_seen'])
            record('death_finishes_lifecycle', not unreal.SystemLibrary.is_valid(enemy))
            if remote:
                record('remote_death_animation', state['remote_death_seen'])
            finish()
    except Exception:
        finish(traceback.format_exc())


state['callback'] = unreal.register_slate_post_tick_callback(tick)
print('Started EnemyAI E2E ' + name)
