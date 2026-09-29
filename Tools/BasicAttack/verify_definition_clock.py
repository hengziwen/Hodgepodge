"""Run on the listen host with DA_Attack_1 temporarily set to PlayRate=2 (do not save)."""
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
montage = unreal.load_asset('/Game/CodexText/Montage/AM_Attack01_Montage')
action = unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack')
window = unreal.GameplayTag()
window.import_text('(TagName="Status.Attack.Cancel.NextAttack")')
external = unreal.GameplayTagContainer()
external.import_text('(GameplayTags=((TagName="Status.Attack.Cancel.NextAttack")))')
state = {'phase': 'rate', 'checked': [], 'external': False}
path = Path(unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_saved_dir())) / 'Tests/definition_combo_clock.json'


def now(): return unreal.GameplayStatics.get_time_seconds(world)
def click(): sub.inject_input_vector_for_action(action, unreal.Vector(1, 0, 0), [], [])
def node(): return combo.get_current_combo_tag().export_text().split('"')[1]


def check(label, result):
    if label in state['checked']: return
    assert result, label
    state['checked'].append(label)


def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    if state['external']: unreal.AbilitySystemLibrary.remove_loose_gameplay_tags(pawn, external)
    anim.montage_resume(montage)
    state['passed'] = error is None
    if error: state['error'] = str(error)
    path.write_text(json.dumps(state, indent=2), encoding='utf-8')
    unreal.log('DEFINITION_CLOCK_TEST ' + json.dumps(state))


def start(phase):
    state['phase'] = phase
    state['start'] = now()
    click()


def tick(delta):
    try:
        elapsed = now() - state['start']
        phase = state['phase']
        if phase == 'rate':
            if elapsed > .7 and 'rate_before_window' not in state['checked']:
                check('rate_before_window', asc.get_gameplay_tag_count(window) == 0 and node() == 'Combo.Light.01')
                check('rate_source_position', abs(anim.montage_get_position(montage) - elapsed * 2) < .12)
            if elapsed > 1.05 and 'rate_window_open' not in state['checked']:
                check('rate_window_open', asc.get_gameplay_tag_count(window) == 1)
            if elapsed > 1.9:
                check('rate_natural_end', node() == 'Combo.Entry' and asc.get_gameplay_tag_count(window) == 0)
                start('pause')
        elif phase == 'pause':
            if elapsed > .3 and 'pause_position' not in state:
                anim.montage_pause(montage)
                state['pause_position'] = anim.montage_get_position(montage)
            if elapsed > 1.1 and 'pause_holds_clock' not in state['checked']:
                check('pause_holds_clock', abs(anim.montage_get_position(montage) - state['pause_position']) < .01)
                check('pause_no_window', node() == 'Combo.Light.01' and asc.get_gameplay_tag_count(window) == 0)
                anim.montage_resume(montage)
            if elapsed > 2.8:
                check('resume_finishes', node() == 'Combo.Entry')
                start('foreign')
        elif phase == 'foreign':
            if elapsed > .15 and not state['external'] and 'foreign_no_transition' not in state['checked']:
                unreal.AbilitySystemLibrary.add_loose_gameplay_tags(pawn, external)
                state['external'] = True
                click()
            if elapsed > .65 and 'foreign_no_transition' not in state['checked']:
                check('foreign_no_transition', node() == 'Combo.Light.01')
                check('foreign_tag_present', asc.get_gameplay_tag_count(window) == 1)
                unreal.AbilitySystemLibrary.remove_loose_gameplay_tags(pawn, external)
                state['external'] = False
            if elapsed > 1.1 and 'expired_foreign_input' not in state['checked']:
                check('expired_foreign_input', node() == 'Combo.Light.01')
            if elapsed > 1.9:
                check('foreign_cleanup', node() == 'Combo.Entry' and asc.get_gameplay_tag_count(window) == 0)
                finish()
    except Exception as error: finish(error)


start('rate')
callback = unreal.register_slate_post_tick_callback(tick)
print('STARTED_CLOCK_TEST')
