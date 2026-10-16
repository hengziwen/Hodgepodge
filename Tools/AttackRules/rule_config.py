"""编辑器作者与验证共用的攻击规则配置入口，不执行任何保存或玩法逻辑。"""
import unreal


def find_attack_rule(asset, rule_type):
    """同类型配置只能有一份，配置冲突必须显式报告。"""
    matches = [rule for rule in asset.get_editor_property('AttackRules') if isinstance(rule, rule_type)]
    assert len(matches) <= 1, (asset.get_path_name(), rule_type)
    return matches[0] if matches else None


def migrate_attack_rules(asset):
    """读取适配仅用于迁移；新配置优先，迁移后清空旧收益值避免双事实来源。"""
    rules = list(asset.get_editor_property('AttackRules'))
    gain = asset.get_editor_property('ResonanceEnergyGainPerPhase')
    has_resonance = find_attack_rule(asset, unreal.HodgeResonanceAttackRuleConfig)
    has_havoc = find_attack_rule(asset, unreal.HodgeHavocAttackRuleConfig)
    # 非默认曲线也属于有效旧配置，不能只比较常量 Value。
    has_legacy_gain = gain.export_text() != unreal.ScalableFloat().export_text()
    if not has_resonance and has_legacy_gain:
        rule = unreal.new_object(unreal.HodgeResonanceAttackRuleConfig, outer=asset)
        rule.set_editor_property('GainPerPhase', gain)
        rule.set_editor_property('GainEffect', asset.get_editor_property('ResonanceEnergyGainEffect'))
        rules.insert(0, rule)
    flow = asset.get_editor_property('DarkFlowGainPerPhase')
    cast = asset.get_editor_property('DarkFlowGainOnCast')
    enhanced = asset.get_editor_property('bEnhancedNormalAttack')
    if not has_havoc and (flow or cast or enhanced):
        rule = unreal.new_object(unreal.HodgeHavocAttackRuleConfig, outer=asset)
        rule.set_editor_property('DarkFlowGainPerPhase', flow)
        rule.set_editor_property('DarkFlowGainOnCast', cast)
        rule.set_editor_property('bEnhancedNormalAttack', enhanced)
        rules.append(rule)
    asset.set_editor_property('AttackRules', rules)
    asset.set_editor_property('ResonanceEnergyGainPerPhase', unreal.ScalableFloat())
    asset.set_editor_property('DarkFlowGainPerPhase', 0.)
    asset.set_editor_property('DarkFlowGainOnCast', 0.)
    asset.set_editor_property('bEnhancedNormalAttack', False)


def ensure_attack_rule(asset, rule_type):
    """作者工具只写新配置，先迁移该资产原有其他规则避免丢失既有参数。"""
    migrate_attack_rules(asset)
    rule = find_attack_rule(asset, rule_type)
    if not rule:
        rule = unreal.new_object(rule_type, outer=asset)
        asset.set_editor_property('AttackRules', list(asset.get_editor_property('AttackRules')) + [rule])
    return rule
