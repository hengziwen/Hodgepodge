"""记录双玩家 Listen Server 的真实输入、服务器结算及客户端复制。"""
import builtins
import json
import time
from pathlib import Path

import unreal


class MeleeNetworkProbe:
    def __init__(self, state, direction):
        self.state = state
        self.direction = direction
        self.samples = []
        self.started = time.monotonic()
        self.host_id = state['host_pc'].get_editor_property('player_state').get_editor_property('player_id')
        self.remote_id = state['remote_server_pc'].get_editor_property('player_state').get_editor_property('player_id')
        self.handle = unreal.register_slate_post_tick_callback(self.tick)
        action = unreal.load_asset('/Game/Main/Input/InputAction/IA_Attack')
        state[direction + '_input'].inject_input_vector_for_action(action, unreal.Vector(1, 0, 0), [], [])

    def tick(self, delta):
        try:
            row = {'elapsed': round(time.monotonic() - self.started, 5), 'worlds': []}
            for world in self.state['net_worlds']:
                pawns = []
                for pawn in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.HodgeCombatCharacter):
                    ps = pawn.get_editor_property('player_state')
                    asc = unreal.AbilitySystemLibrary.get_ability_system_component(pawn)
                    if not ps or not asc:
                        continue
                    health = pawn.get_component_by_class(unreal.HodgeHealthComponent)
                    anim = pawn.get_editor_property('mesh').get_anim_instance()
                    montage = anim.get_current_active_montage()
                    origin = unreal.MathLibrary.transform_location(pawn.get_editor_property('mesh').get_world_transform(), unreal.Vector(0, 120, 88))
                    attribute = next(a for a in asc.get_all_attributes() if a.get_editor_property('attribute_name') == 'BaseDamage')
                    pawns.append({'id': ps.get_editor_property('player_id'), 'role': str(pawn.get_local_role()),
                                  'health': health.get_health(), 'montage': montage.get_name() if montage else None,
                                  'location': [pawn.get_actor_location().x, pawn.get_actor_location().y, pawn.get_actor_location().z],
                                  'detection_origin': [origin.x, origin.y, origin.z],
                                  'base_damage': unreal.AbilitySystemLibrary.get_float_attribute_from_ability_system_component(asc, attribute),
                                  'tags': unreal.GameplayTagLibrary.get_debug_string_from_gameplay_tag_container(
                                      unreal.GameplayTagLibrary.get_owned_gameplay_tags(asc))})
                row['worlds'].append({'world': world.get_path_name(), 'pawns': pawns})
            self.samples.append(row)
            if row['elapsed'] >= 6:
                self.finish()
        except Exception as error:
            self.finish(str(error))

    def finish(self, error=None):
        unreal.unregister_slate_post_tick_callback(self.handle)
        target_id = self.remote_id if self.direction == 'host' else self.host_id
        final = [{p['id']: p['health'] for p in world['pawns']} for world in self.samples[-1]['worlds']] if self.samples else []
        activated = any(p['montage'] for row in self.samples for world in row['worlds'] for p in world['pawns']
                        if p['id'] != target_id)
        passed = not error and activated and len(final) == 2 and all(values.get(target_id) == 70 for values in final)
        suffix = getattr(builtins, 'HODGE_NETWORK_CASE_SUFFIX', '')
        result = {'case': 'ListenServer_' + self.direction + '_attack' + suffix, 'passed': passed,
                  'target_id': target_id, 'final_health_by_world': final, 'error': error, 'samples': self.samples}
        folder = Path(unreal.Paths.project_saved_dir()) / 'Tests/MeleePIE'
        (folder / (result['case'] + '.json')).write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
        self.state['report'].append({key: value for key, value in result.items() if key != 'samples'})
        (folder / 'results.json').write_text(json.dumps(self.state['report'], ensure_ascii=False, indent=2), encoding='utf-8')
        print(json.dumps({key: value for key, value in result.items() if key != 'samples'}))


builtins.HODGE_NETWORK_PROBE = MeleeNetworkProbe(builtins.HODGE_PIE_TEST, builtins.HODGE_NETWORK_DIRECTION)
