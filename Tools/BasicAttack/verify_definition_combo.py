"""PIE regression through real Enhanced Input; writes results under Saved/Tests."""
import json
from pathlib import Path
import unreal

side = globals().get('TEST_SIDE', 'standalone')
subsystems = [x for x in unreal.ObjectIterator(unreal.EnhancedInputLocalPlayerSubsystem)
              if 'Default__' not in x.get_name() and x.get_world() is not None]
sub = next(x for x in subsystems if side == 'standalone' or
           unreal.GameplayStatics.get_player_controller(x.get_world(), 0).has_authority() == (side == 'server'))
world = sub.get_world()
pc = unreal.GameplayStatics.get_player_controller(world, 0)
pawn = pc.get_controlled_pawn()
asc = unreal.AbilitySystemLibrary.get_ability_system_component(pawn)
combo = pc.player_state.get_component_by_class(unreal.HodgeComboComponent)
anim = pawn.get_component_by_class(unreal.SkeletalMeshComponent).get_anim_instance()
attack = unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack')
move = unreal.load_asset('/Game/Main/Input/InputAction/IA_Move')
tags = []
for name in ['Status.Attack', 'Status.Attack.Cancel.Move', 'Status.Attack.Cancel.NextAttack']:
    tag = unreal.GameplayTag()
    tag.import_text('(TagName="%s")' % name)
    tags.append(tag)
result = {'side': side, 'checks': [], 'changes': [], 'phase': 'natural', 'last': '', 'pressed': []}
peer_combo = None
peer_asc = None
peer_anim = None
if side != 'standalone':
    player_id = pc.player_state.get_editor_property('PlayerId')
    peer = next(x for x in unreal.ObjectIterator(unreal.HodgePlayerState)
                if x.get_world() is not None and x.get_world() != world
                and x.get_editor_property('PlayerId') == player_id)
    peer_combo = peer.get_component_by_class(unreal.HodgeComboComponent)
    peer_asc = peer.get_component_by_class(unreal.AbilitySystemComponent)
    peer_anim = peer.get_pawn().get_component_by_class(unreal.SkeletalMeshComponent).get_anim_instance()
path = Path(unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_saved_dir())) / ('Tests/definition_combo_' + side + '.json')
path.parent.mkdir(parents=True, exist_ok=True)


def now(): return unreal.GameplayStatics.get_time_seconds(world)
def node(): return combo.get_current_combo_tag().export_text().split('"')[1]
def click(): sub.inject_input_vector_for_action(attack, unreal.Vector(1, 0, 0), [], [])
def check(name, passed):
    result['checks'].append({'name': name, 'passed': bool(passed)})
    assert passed, name
    if peer_asc and name in ('node_tag_count_one', 'natural_tags_clean', 'combo_tags_clean', 'movement_tags_clean'):
        expected = 1 if name == 'node_tag_count_one' else 0
        peer_ok = peer_asc.get_gameplay_tag_count(tags[0]) == expected
        result['checks'].append({'name': name + '_peer', 'passed': peer_ok})
        assert peer_ok, name + '_peer'
    if peer_combo and side == 'client' and name in ('single_click_first_node', 'natural_end_entry', 'five_chain_finishes', 'valid_combo_precedes_movement'):
        peer_ok = peer_combo.get_current_combo_tag().export_text() == combo.get_current_combo_tag().export_text()
        result['checks'].append({'name': name + '_authority', 'passed': peer_ok})
        assert peer_ok, name + '_authority'
    if peer_anim and name == 'first_montage':
        peer_montage = peer_anim.get_current_active_montage()
        peer_ok = peer_montage and peer_montage.get_name() == 'AM_Attack01_Montage'
        result['checks'].append({'name': 'first_montage_peer', 'passed': bool(peer_ok)})
        assert peer_ok, 'first_montage_peer'


def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    result['passed'] = error is None
    if error: result['error'] = str(error)
    path.write_text(json.dumps(result, indent=2), encoding='utf-8')
    unreal.log('DEFINITION_COMBO_PIE ' + json.dumps(result))


def start(phase):
    result['phase'] = phase
    result['start'] = now()
    result['pressed'] = []
    click()


def tick(delta):
    try:
        elapsed = now() - result['start']
        current = node()
        montage = anim.get_current_active_montage()
        position = anim.montage_get_position(montage) if montage else 0
        if current != result['last']:
            result['changes'].append({'phase': result['phase'], 'node': current, 'time': now(),
                                      'montage': montage.get_name() if montage else None})
            result['last'] = current
        phase = result['phase']
        if phase == 'natural':
            if elapsed > .4 and 'active' not in result['pressed']:
                check('single_click_first_node', current == 'Combo.Light.01')
                check('first_montage', montage and montage.get_name() == 'AM_Attack01_Montage')
                check('node_tag_count_one', asc.get_gameplay_tag_count(tags[0]) == 1)
                result['pressed'].append('active')
            if elapsed > 3.7:
                check('natural_end_entry', current == 'Combo.Entry')
                check('natural_tags_clean', all(asc.get_gameplay_tag_count(t) == 0 for t in tags))
                start('expired')
        elif phase == 'expired':
            if elapsed > .3 and not result['pressed']:
                click()
                result['pressed'].append('early')
            if elapsed > 2.2 and 'checked' not in result['pressed']:
                check('expired_input_does_not_chain', current == 'Combo.Light.01')
                result['pressed'].append('checked')
            if elapsed > 3.7: start('combo')
        elif phase == 'combo':
            if montage and current.startswith('Combo.Light.'):
                index = int(current.rsplit('.', 1)[1])
                if index < 5 and position >= montage.get_play_length() * .6 - .15 and index not in result['pressed']:
                    click()
                    result['pressed'].append(index)
            if elapsed > 14:
                visited = [x['node'] for x in result['changes'] if x['phase'] == 'combo']
                check('all_five_nodes', all('Combo.Light.%02d' % i in visited for i in range(1, 6)))
                check('five_chain_finishes', current == 'Combo.Entry')
                check('combo_tags_clean', all(asc.get_gameplay_tag_count(t) == 0 for t in tags))
                start('movement')
        elif phase == 'movement':
            sub.inject_input_vector_for_action(move, unreal.Vector(1, 0, 0), [], [])
            if elapsed > .5 and 'checked' not in result['pressed']:
                check('movement_cannot_cancel_early', current == 'Combo.Light.01')
                result['pressed'].append('checked')
            if elapsed > 2.4:
                check('movement_cancels_in_recovery', current == 'Combo.Entry')
                check('movement_tags_clean', all(asc.get_gameplay_tag_count(t) == 0 for t in tags))
                start('priority')
        elif phase == 'priority':
            sub.inject_input_vector_for_action(move, unreal.Vector(1, 0, 0), [], [])
            if current == 'Combo.Light.01' and montage and position >= montage.get_play_length() * .6 - .15 and not result['pressed']:
                click()
                result['pressed'].append('next')
            if elapsed > 2.2 and 'checked' not in result['pressed']:
                check('valid_combo_precedes_movement', current == 'Combo.Light.02')
                result['pressed'].append('checked')
            if elapsed > 4.5:
                check('subsequent_movement_returns_entry', current == 'Combo.Entry')
                finish()
        if elapsed > 20: raise RuntimeError('Phase timeout')
    except Exception as error:
        finish(error)


start('natural')
callback = unreal.register_slate_post_tick_callback(tick)
print('STARTED', side, str(path))
