import json
import traceback
from pathlib import Path
import unreal

# 先启动双人 Listen 或 Client PIE；可预先设置 NetEmulation.PktLag。
worlds = unreal.EditorLevelLibrary.get_pie_worlds(True)
server = next(w for w in worlds if unreal.GameplayStatics.get_game_mode(w))
client = next(w for w in worlds if w != server)
pc = unreal.GameplayStatics.get_player_controller(client, 0)
owner = pc.get_controlled_pawn()
pid = owner.get_editor_property('player_state').get_editor_property('PlayerId')
remote = next(h for h in unreal.GameplayStatics.get_all_actors_of_class(server, owner.get_class())
              if h.get_editor_property('player_state') and h.get_editor_property('player_state').get_editor_property('PlayerId') == pid)
owner_asc = owner.get_editor_property('player_state').get_hodge_ability_system_component()
server_asc = remote.get_editor_property('player_state').get_hodge_ability_system_component()
lib = unreal.get_default_object(unreal.load_class(None, '/Script/GameplayAbilities.AbilitySystemBlueprintLibrary'))
sub = unreal.get_default_object(unreal.load_class(None, '/Script/Engine.SubsystemBlueprintLibrary')).call_method(
    'GetLocalPlayerSubSystemFromPlayerController', (pc, unreal.EnhancedInputLocalPlayerSubsystem.static_class()))
attack = unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack')
expected = ['AM_Attack01_Montage', 'AM_Attack02_Montage', 'AM_Attack03_Montage', 'AM_Attack05_Montage', 'AM_Attack04_Montage'] * 2 + ['AM_Attack01_Montage']
state = {'start': unreal.GameplayStatics.get_time_seconds(client), 'started': False, 'pressed': False,
         'begin': 0, 'owner': [], 'server': [], 'cancel': False}

def active(asc):
    items = []
    for handle in asc.get_all_abilities():
        definition = asc.find_ability_definition(handle)
        if not definition or definition.get_editor_property('ExecutionRoute') != unreal.HodgeAbilityExecutionRoute.COMBO_COORDINATED:
            continue
        ga = lib.call_method('GetGameplayAbilityFromSpecHandle', (asc, handle))[0]
        if lib.call_method('IsGameplayAbilityActive', (ga,)):
            items.append((ga, definition.get_editor_property('ExecutionConfig').get_editor_property('Montage'), handle))
    assert len(items) <= 1, items
    return items

def finish(error=None):
    unreal.unregister_slate_post_tick_callback(callback)
    output = Path(unreal.Paths.project_saved_dir()) / 'ClientComboFix' / 'authoritative-combo.json'
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps({'error': error, 'expected': expected, 'state': state}, ensure_ascii=False, indent=2), encoding='utf-8')

def tick(delta):
    try:
        now = unreal.GameplayStatics.get_time_seconds(client)
        oa, sa = active(owner_asc), active(server_asc)
        for name, items in [('owner', oa), ('server', sa)]:
            if not items:
                continue
            montage = items[0][1].get_name()
            if not state[name] or state[name][-1]['montage'] != montage:
                state[name].append({'montage': montage, 'time': now - state['start']})
                assert [r['montage'] for r in state[name]] == expected[:len(state[name])], state
                if name == 'owner':
                    state['begin'], state['pressed'] = now, False
        if not state['started']:
            sub.inject_input_vector_for_action(attack, unreal.Vector(1, 0, 0), [], [])
            state['started'] = True
        elif len(state['owner']) == len(expected) and len(state['server']) == len(expected):
            if not state['cancel'] and oa:
                assert unreal.HodgeCombatValidationLibrary.queue_ability_action(owner_asc, oa[0][2], True)
                state['cancel'] = True
            if state['cancel'] and not oa and not sa:
                for pawn in [owner, remote]:
                    assert unreal.HodgeCombatValidationLibrary.inspect_pose_leases(pawn) == 0
                    assert unreal.HodgeCombatValidationLibrary.inspect_hit_sessions(pawn) == 0
                finish()
                return
        elif oa and len(state['owner']) < len(expected) and not state['pressed']:
            starts = [unreal.AnimationLibrary.get_anim_notify_event_trigger_time(e)
                      for e in unreal.AnimationLibrary.get_animation_notify_events(oa[0][1])
                      if isinstance(e.get_editor_property('NotifyStateClass'), unreal.HodgeAnimNotifyState_GameplayTag)
                      and 'Status.Attack.Cancel.NextAttack' in e.get_editor_property('NotifyStateClass').get_editor_property('StateTag').export_text()]
            assert starts, oa[0][1].get_name()
            if now - state['begin'] >= max(0, min(starts) - .15):
                sub.inject_input_vector_for_action(attack, unreal.Vector(1, 0, 0), [], [])
                state['pressed'] = True
        assert now - state['start'] < 25, state
    except Exception:
        finish(traceback.format_exc())

output = Path(unreal.Paths.project_saved_dir()) / 'ClientComboFix' / 'authoritative-combo.json'
output.parent.mkdir(parents=True, exist_ok=True)
output.write_text('{"error":"RUNNING"}', encoding='utf-8')
callback = unreal.register_slate_post_tick_callback(tick)
print('Started two full buffered combo cycles; owner and authoritative server sequences must match.')
