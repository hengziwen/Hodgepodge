"""MCP PIE regression: use the granted ability and real Enhanced Input actions."""
import json
from pathlib import Path
import unreal

world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world()
side = globals().get('TEST_SIDE', 'standalone')
if side in ('client', 'server'):
    worlds = [x.get_world() for x in unreal.ObjectIterator(unreal.EnhancedInputLocalPlayerSubsystem)
              if 'Default__' not in x.get_name() and x.get_world() is not None]
    world = next(w for w in worlds if unreal.GameplayStatics.get_player_controller(w, 0).has_authority() == (side == 'server'))
pc = unreal.GameplayStatics.get_player_controller(world, 0)
pawn = pc.get_controlled_pawn()
asc = unreal.AbilitySystemLibrary.get_ability_system_component(pawn)
sub = next(x for x in unreal.ObjectIterator(unreal.EnhancedInputLocalPlayerSubsystem)
           if 'Default__' not in x.get_name() and x.get_world() == world)
attack = unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack')
move = unreal.load_asset('/Game/Main/Input/InputAction/IA_Move')
ability = None
for handle in asc.get_all_abilities():
    candidate, instance = unreal.AbilitySystemLibrary.get_gameplay_ability_from_spec_handle(asc, handle)
    if isinstance(candidate, unreal.HodgeGameplayAbility_BasicAttack):
        assert instance
        ability = candidate
assert ability is not None, 'DefaultPawnData did not grant GA_BasicAttack'
anim = pawn.get_component_by_class(unreal.SkeletalMeshComponent).get_anim_instance()
result_path = Path(unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_saved_dir())) / ('Tests/basic_attack_' + side + '.json')
state = {'phase': 'natural', 'start': unreal.GameplayStatics.get_time_seconds(world), 'rows': [], 'checks': [], 'clicked': [], 'last_step': 0}

def now(): return unreal.GameplayStatics.get_time_seconds(world)
def active(): return unreal.AbilitySystemLibrary.is_gameplay_ability_active(ability)
def click(): sub.inject_input_vector_for_action(attack, unreal.Vector(1,0,0), [], [])
peer_ability = None
peer_anim = None
if side == 'client':
    player_id = pc.player_state.get_editor_property('PlayerId')
    peer = next(x for x in unreal.ObjectIterator(unreal.HodgePlayerState)
                if x.get_world() is not None and x.has_authority()
                and x.get_editor_property('PlayerId') == player_id)
    peer_asc = peer.get_component_by_class(unreal.AbilitySystemComponent)
    for h in peer_asc.get_all_abilities():
        candidate, instance = unreal.AbilitySystemLibrary.get_gameplay_ability_from_spec_handle(peer_asc, h)
        if isinstance(candidate, unreal.HodgeGameplayAbility_BasicAttack): peer_ability = candidate
    assert peer_ability is not None
    peer_pawn = peer.get_pawn()
    peer_anim = peer_pawn.get_component_by_class(unreal.SkeletalMeshComponent).get_anim_instance()

def check(name, passed):
    state['checks'].append({'name': name, 'passed': bool(passed)})
    assert passed, name
    if peer_ability is not None:
        peer_ok = peer_ability.current_attack_step == ability.current_attack_step
        state['checks'].append({'name': name + '_authority_step', 'passed': peer_ok})
        assert peer_ok, name + '_authority_step'
        if ability.current_attack_step:
            montage = peer_anim.get_current_active_montage()
            montage_ok = montage is not None and montage.get_name() == 'AM_Attack%02d_Montage' % ability.current_attack_step
            state['checks'].append({'name': name + '_authority_montage', 'passed': montage_ok})
            assert montage_ok, name + '_authority_montage'
    if ability.current_attack_step == 0:
        clean = all(asc.get_gameplay_tag_count(t) == 0 for t in cleanup_tags)
        state['checks'].append({'name': name + '_tags_cleared', 'passed': clean})
        assert clean, name + '_tags_cleared'


def row():
    montage = anim.get_current_active_montage()
    return {'time': now(), 'phase': state['phase'], 'step': ability.current_attack_step,
            'active': active(), 'montage': montage.get_name() if montage else None}

def switch(phase):
    state['phase'] = phase
    state['start'] = now()
    state['last_step'] = 0
    click()

def finish(error=None):
    state['passed'] = error is None and all(x['passed'] for x in state['checks'])
    if error: state['error'] = str(error)
    unreal.unregister_slate_post_tick_callback(callback)
    result_path.write_text(json.dumps(state, indent=2), encoding='utf-8')
    unreal.log('BASIC_ATTACK_PIE ' + json.dumps(state))

cleanup_tags = []
for name in ['Status.Attack', 'Status.Attack.Recovery', 'Status.Attack.Cancel.Move', 'Status.Attack.Cancel.NextAttack']:
    t = unreal.GameplayTag(); t.import_text('(TagName="%s")' % name); cleanup_tags.append(t)
click()
def tick(delta):
    try:
        elapsed = now() - state['start']
        step = ability.current_attack_step
        if step != state['last_step']:
            state['rows'].append(row())
            state['last_step'] = step
            state['step_start'] = now()
        phase = state['phase']
        if phase == 'natural':
            if elapsed > 0.4 and not state.get('natural_checked'):
                check('click_activates_first_montage', active() and step == 1 and anim.get_current_active_montage().get_name() == 'AM_Attack01_Montage')
                state['natural_checked'] = True
            if elapsed > 3.5:
                check('single_click_ends_without_auto_combo', not active() and step == 0 and anim.get_current_active_montage() is None)
                switch('combo')
        elif phase == 'combo':
            if step and step not in state['clicked'] and now() - state['step_start'] > 0.25:
                check('combo_montage_' + str(step), anim.get_current_active_montage().get_name() == 'AM_Attack%02d_Montage' % step)
                state['clicked'].append(step)
                if step < 5: click()
            if len(state['clicked']) == 5 and not active() and now() - state['step_start'] > 0.2:
                check('five_steps_finish', step == 0)
                switch('move')
            if elapsed > 18: raise AssertionError('combo timed out')
        elif phase == 'move':
            if elapsed > 0.25:
                sub.inject_input_vector_for_action(move, unreal.Vector(0,1,0), [], [])
            if elapsed > 0.7 and not state.get('early_move_checked'):
                check('movement_before_window_keeps_attack', active() and step == 1)
                state['early_move_checked'] = True
            if elapsed > 2.2:
                check('held_movement_cancels_at_window', not active() and step == 0 and anim.get_current_active_montage() is None)
                state['phase'] = 'release_move'
                state['start'] = now()
        elif phase == 'release_move' and elapsed > 0.25:
            switch('restart')
        elif phase == 'restart' and elapsed > 0.4:
            check('after_cancel_restarts_first', active() and step == 1)
            state['phase'] = 'restart_finish'
        elif phase == 'restart_finish' and elapsed > 3.5:
            check('restart_cleans_up', not active() and step == 0)
            finish()
        if elapsed > 25: raise AssertionError('phase timed out')
    except Exception as exc:
        finish(repr(exc))
callback = unreal.register_slate_post_tick_callback(tick)
print('BASIC_ATTACK_PIE_STARTED', world.get_path_name(), ability.get_path_name())
