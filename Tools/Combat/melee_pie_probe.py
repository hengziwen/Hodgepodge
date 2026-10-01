"""通过 MCP 在已启动的 PIE 中记录近战验收证据；不保存正式资产。"""
import builtins
import json
import time
from pathlib import Path

import unreal


class MeleePIEProbe:
    def __init__(self, state):
        self.state = state
        self.handle = None
        self.samples = []

    def reset_health(self, actor):
        # 测试复位也通过 GAS 修改属性，不直接改 HealthSet 内存。
        effect = unreal.get_default_object(unreal.GameplayEffect)
        original = list(effect.get_editor_property('modifiers'))
        assert effect.get_editor_property('duration_policy') == unreal.GameplayEffectDurationType.INSTANT
        modifier = unreal.GameplayModifierInfo()
        modifier.import_text('(Attribute=(AttributeName="Health",Attribute=/Script/Hodgepodge.HodgeHealthSet:Health,'
                             'AttributeOwner="/Script/Hodgepodge.HodgeHealthSet"),ModifierOp=Override,'
                             'ModifierMagnitude=(MagnitudeCalculationType=ScalableFloat,ScalableFloatMagnitude=(Value=100.0)))')
        # 基础 GE 默认对象仅在同步复位期间借用，立即恢复且不保存。
        try:
            effect.set_editor_property('modifiers', [modifier])
            spec = unreal.AbilitySystemLibrary.make_spec_handle(effect, actor, actor, 1.0)
            unreal.AbilitySystemLibrary.get_ability_system_component(actor).apply_gameplay_effect_spec_to_self(spec)
        finally:
            effect.set_editor_property('modifiers', original)
        assert actor.get_component_by_class(unreal.HodgeHealthComponent).get_health() == 100.0

    def start(self, name, expected_health, offset=120, cancel_at=None, enter_at=None, follow_weapon=False, replace_at=None):
        assert self.handle is None
        s = self.state
        self.reset_health(s['target'])
        self.samples = []
        self.name = name
        self.expected_health = expected_health
        self.start_time = time.monotonic()
        self.cancel_at = cancel_at
        self.enter_at = enter_at
        self.did_cancel = False
        self.did_enter = False
        self.follow_weapon = follow_weapon
        self.cancel_health = None
        self.replace_at = replace_at
        self.did_replace = False
        s['attacker'].set_actor_location(unreal.Vector(1000, 1000, 300), False, False)
        s['attacker'].set_actor_rotation(unreal.Rotator(0, 0, 0), False)
        s['pc'].set_control_rotation(unreal.Rotator(0, 0, 0))
        s['target'].set_actor_location(unreal.Vector(1000 + offset, 1000, 300), False, False)
        self.handle = unreal.register_slate_post_tick_callback(self.tick)
        print('PIE probe started: ' + name)

    def tick(self, delta):
        s = self.state
        try:
            elapsed = time.monotonic() - self.start_time
            anim = s['attacker'].get_editor_property('mesh').get_anim_instance()
            montage = anim.get_current_active_montage()
            position = anim.montage_get_position(montage) if montage else None
            if self.replace_at is not None and position is not None and position >= self.replace_at and not self.did_replace:
                self.did_replace = True
                s['attacker'].destroy_actor()
                unreal.GameplayStatics.get_game_mode(s['world']).restart_player(s['pc'])
                s['attacker'] = s['pc'].get_controlled_pawn()
                assert s['attacker'], 'GameMode failed to respawn Pawn'
                s['attacker'].get_component_by_class(unreal.CharacterMovementComponent).set_movement_mode(unreal.MovementMode.MOVE_NONE)
                s['attacker'].set_actor_location(unreal.Vector(1000, 1000, 300), False, False)
                s['attacker'].set_actor_rotation(unreal.Rotator(0, 0, 0), False)
            if self.follow_weapon:
                # 路由验收让靶保持在实际刀骨附近，完整刀身范围另行精调。
                weapons = [a for a in unreal.GameplayStatics.get_all_actors_of_class(s['world'], unreal.Actor)
                           if a.get_name().startswith('BP_Weapon_Sword') and a.get_owner() == s['attacker']]
                if weapons:
                    component = weapons[0].get_component_by_class(unreal.SkeletalMeshComponent)
                    s['target'].set_actor_location(component.get_socket_location('Sword_Bone01'), False, False)
            if self.enter_at is not None and position is not None and position >= self.enter_at and not self.did_enter:
                self.did_enter = True
                s['target'].set_actor_location(s['attacker'].get_actor_location() + unreal.Vector(120, 0, 0), False, False)
            if self.cancel_at is not None and position is not None and position >= self.cancel_at and not self.did_cancel:
                self.did_cancel = True
                self.cancel_health = s['target_health'].get_health()
                abilities = [ability for ability in unreal.ObjectIterator(unreal.HodgeGameplayAbility_Melee)
                             if ability.get_outer() == s['source_asc'].get_owner()
                             and unreal.AbilitySystemLibrary.is_gameplay_ability_active(ability)]
                assert abilities, 'No active melee ability instance found for cancellation'
                for ability in abilities:
                    ability.cancel_ability()
            tags = unreal.GameplayTagLibrary.get_owned_gameplay_tags(s['source_asc'])
            self.samples.append({'elapsed': round(elapsed, 5), 'health': s['target_health'].get_health(),
                                 'source_health': s['attacker'].get_component_by_class(unreal.HodgeHealthComponent).get_health(),
                                 'montage': montage.get_name() if montage else None, 'montage_time': position,
                                 'tags': unreal.GameplayTagLibrary.get_debug_string_from_gameplay_tag_container(tags)})
            if elapsed >= 5:
                self.finish()
        except Exception as error:
            self.finish(str(error))

    def finish(self, error=None):
        unreal.unregister_slate_post_tick_callback(self.handle)
        self.handle = None
        health = self.state['target_health'].get_health()
        result = {'case': self.name, 'expected_health': self.expected_health, 'actual_health': health,
                  'passed': not error and health == self.expected_health and any(row['montage'] for row in self.samples),
                  'cancelled': self.did_cancel, 'health_at_cancel': self.cancel_health,
                  'entered_late': self.did_enter, 'replaced_pawn': self.did_replace, 'error': error, 'samples': self.samples}
        folder = Path(unreal.Paths.project_saved_dir()) / 'Tests/MeleePIE'
        (folder / (self.name + '.json')).write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
        self.state['report'].append({key: value for key, value in result.items() if key != 'samples'})
        (folder / 'results.json').write_text(json.dumps(self.state['report'], ensure_ascii=False, indent=2), encoding='utf-8')
        print(json.dumps({key: value for key, value in result.items() if key != 'samples'}))


builtins.HODGE_PIE_PROBE = MeleePIEProbe(builtins.HODGE_PIE_TEST)
