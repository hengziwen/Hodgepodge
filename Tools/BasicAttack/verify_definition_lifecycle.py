"""Host PIE: activation precheck, execution failure, interruption and respawn cleanup."""
import json
from pathlib import Path
import unreal

sub = next(x for x in unreal.ObjectIterator(unreal.EnhancedInputLocalPlayerSubsystem)
           if 'Default__' not in x.get_name() and x.get_world() is not None
           and unreal.GameplayStatics.get_player_controller(x.get_world(), 0).has_authority())
world = sub.get_world()
pc = unreal.GameplayStatics.get_player_controller(world, 0)
pawn = pc.get_controlled_pawn()
asc = unreal.AbilitySystemLibrary.get_ability_system_component(pawn)
combo = pc.player_state.get_component_by_class(unreal.HodgeComboComponent)
anim = pawn.get_component_by_class(unreal.SkeletalMeshComponent).get_anim_instance()
action = unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack')
definition = unreal.load_asset('/Game/CodexText/DefinitionCombo/DA_Attack_2')
original_config = definition.get_editor_property('execution_config').export_text()
tags = []
for name in ['Status.Attack', 'Status.Attack.Cancel.Move', 'Status.Attack.Cancel.NextAttack']:
    tag = unreal.GameplayTag()
    tag.import_text('(TagName="%s")' % name)
    tags.append(tag)
state = {'checks': [], 'done': []}
path = Path(unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_saved_dir())) / 'Tests/definition_combo_lifecycle.json'


def now(): return unreal.GameplayStatics.get_time_seconds(world)
def click(): sub.inject_input_vector_for_action(action, unreal.Vector(1, 0, 0), [], [])
def node(): return combo.get_current_combo_tag().export_text().split('"')[1]
def clean(): return all(asc.get_gameplay_tag_count(t) == 0 for t in tags)


def check(label, value):
    assert value, label
    state['checks'].append(label)


def restore_config():
    config = definition.get_editor_property('execution_config')
    assert config.import_text(original_config)
    definition.set_editor_property('execution_config', config)


def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    asc.set_user_ability_activation_inhibited(False)
    restore_config()
    state['passed'] = error is None
    if error: state['error'] = str(error)
    path.write_text(json.dumps(state, indent=2), encoding='utf-8')
    unreal.log('DEFINITION_LIFECYCLE_TEST ' + json.dumps(state))


def start(phase):
    state['phase'] = phase
    state['start'] = now()
    state['done'] = []
    click()


def tick(delta):
    global pawn, anim
    try:
        elapsed = now() - state['start']
        phase = state['phase']
        if phase == 'precheck':
            if elapsed > 2 and not state['done']:
                asc.set_user_ability_activation_inhibited(True)
                click()
                state['done'].append('input')
            if elapsed > 2.45 and 'checked' not in state['done']:
                check('failed_precheck_keeps_source', node() == 'Combo.Light.01')
                asc.set_user_ability_activation_inhibited(False)
                state['done'].append('checked')
            if elapsed > 3.7:
                check('failed_precheck_cleans_naturally', node() == 'Combo.Entry' and clean())
                start('activation_failure')
        elif phase == 'activation_failure':
            if elapsed > 2 and not state['done']:
                config = definition.get_editor_property('execution_config')
                assert config.import_text('(Montage=None)')
                definition.set_editor_property('execution_config', config)
                click()
                state['done'].append('input')
            if elapsed > 2.5:
                check('failure_after_source_end_returns_entry', node() == 'Combo.Entry' and clean())
                restore_config()
                start('interrupt')
        elif phase == 'interrupt':
            if elapsed > 2.1 and not state['done']:
                check('window_active_before_interrupt', asc.get_gameplay_tag_count(tags[1]) == 1)
                anim.montage_stop(.1)
                state['done'].append('stop')
            if elapsed > 2.5:
                check('external_interrupt_cleans_session', node() == 'Combo.Entry' and clean())
                start('respawn')
        elif phase == 'respawn':
            if elapsed > 2.1 and not state['done']:
                state['old_pawn'] = pawn.get_path_name()
                pawn.destroy_actor()
                unreal.GameplayStatics.get_game_mode(world).restart_player(pc)
                state['done'].append('spawned')
            if elapsed > 3 and 'checked' not in state['done']:
                pawn = pc.get_controlled_pawn()
                check('respawn_has_new_avatar', pawn is not None and pawn.get_path_name() != state['old_pawn'])
                check('respawn_clears_old_session', node() == 'Combo.Entry' and clean())
                check('respawn_preserves_playerstate_asc', unreal.AbilitySystemLibrary.get_ability_system_component(pawn) == asc)
                anim = pawn.get_component_by_class(unreal.SkeletalMeshComponent).get_anim_instance()
                click()
                state['done'].append('checked')
            if elapsed > 3.5:
                check('respawn_can_attack_from_first', node() == 'Combo.Light.01' and asc.get_gameplay_tag_count(tags[0]) == 1)
                finish()
    except Exception as error: finish(error)


start('precheck')
callback = unreal.register_slate_post_tick_callback(tick)
print('STARTED_LIFECYCLE_TEST')
