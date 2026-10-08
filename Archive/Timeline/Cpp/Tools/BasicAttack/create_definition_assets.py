"""Create the new combo assets without migrating the default PawnData."""
import json
import unreal

ROOT = '/Game/CodexText/DefinitionCombo'
tools = unreal.AssetToolsHelpers.get_asset_tools()


def create(name, cls, factory):
    path = ROOT + '/' + name
    assert not unreal.EditorAssetLibrary.does_asset_exist(path), path
    result = tools.create_asset(name, ROOT, cls, factory)
    assert result, path
    return result


def data(name, cls):
    factory = unreal.DataAssetFactory()
    factory.set_editor_property('data_asset_class', cls)
    return create(name, cls, factory)


def tag(name):
    value = unreal.GameplayTag()
    assert value.import_text('(TagName="%s")' % name)
    return value


definitions = []
rows = []
for i in range(1, 6):
    montage = unreal.load_asset('/Game/CodexText/Montage/AM_Attack%02d_Montage' % i)
    assert montage
    duration = montage.get_play_length()
    timeline = data('DA_Attack_%d_Timeline' % i, unreal.HodgeAbilityTimeline)
    timeline.set_editor_property('use_montage_duration', True)
    events = []
    windows = [('MoveCancel', 'Status.Attack.Cancel.Move', 10)]
    if i < 5:
        windows.insert(0, ('NextAttack', 'Status.Attack.Cancel.NextAttack', -10))
    for name, window, priority in windows:
        event = unreal.HodgeTimelineEvent()
        assert event.import_text('(Kind=Window,EventID="%s",StartTime=%.9f,EndTime=%.9f,Priority=%d,WindowTag=(TagName="%s"))'
                                 % (name, duration * .6, duration, priority, window))
        events.append(event)
    timeline.set_editor_property('events', events)
    assert unreal.EditorAssetLibrary.save_loaded_asset(timeline)
    factory = unreal.BlueprintFactory()
    factory.set_editor_property('parent_class', unreal.HodgeGameplayAbility_Definition)
    bp = create('GA_Attack_%d' % i, unreal.Blueprint, factory)
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    assert unreal.EditorAssetLibrary.save_loaded_asset(bp)
    definition = data('DA_Attack_%d' % i, unreal.HodgeAbilityDefinition)
    definition.set_editor_property('ability_class', bp.generated_class())
    definition.set_editor_property('ability_tag', tag('Ability.Attack.Light.%02d' % i))
    config = unreal.HodgeAbilityExecutionConfig()
    assert config.import_text('(Montage="%s",PlayRate=1.0,BlendIn=(Time=0.1),NaturalBlendOut=(Time=0.2),StopBlendOut=(Time=0.1),TimelineTaskConfig=(Timeline="%s"))'
                              % (montage.get_path_name(), timeline.get_path_name()))
    definition.set_editor_property('execution_config', config)
    assert unreal.EditorAssetLibrary.save_loaded_asset(definition)
    definitions.append(definition)
    rows.append({'Name': 'Combo.Light.%02d' % i, 'ComboTag': 'Combo.Light.%02d' % i,
                 'AbilityTag': 'Ability.Attack.Light.%02d' % i, 'GrantedTags': '(GameplayTags=((TagName="Status.Attack")))',
                 'Transitions': [] if i == 5 else [{'TriggerInputIntentTag': 'InputIntent.Attack.Light',
                    'TargetComboTag': 'Combo.Light.%02d' % (i + 1),
                    'RequiredWindowTags': '(GameplayTags=((TagName="Status.Attack.Cancel.NextAttack")))', 'TransitionPriority': 10}]})

rows.insert(0, {'Name': 'Combo.Entry', 'ComboTag': 'Combo.Entry', 'Transitions': [
    {'TriggerInputIntentTag': 'InputIntent.Attack.Light', 'TargetComboTag': 'Combo.Light.01'}]})
factory = unreal.DataTableFactory()
factory.set_editor_property('struct', unreal.load_object(None, '/Script/Hodgepodge.HodgeComboRow'))
table = create('DT_LightCombo', unreal.DataTable, factory)
assert unreal.DataTableFunctionLibrary.fill_data_table_from_json_string(table, json.dumps(rows))
assert unreal.EditorAssetLibrary.save_loaded_asset(table)
combo = data('DA_LightCombo', unreal.HodgeComboDefinition)
combo.set_editor_property('combo_table', table)
combo.set_editor_property('entry_combo_tag', tag('Combo.Entry'))
combo.set_editor_property('move_cancel_window_tag', tag('Status.Attack.Cancel.Move'))
binding = unreal.HodgeComboInputBinding()
assert binding.import_text('(InputTag=(TagName="InputTag.Ability.Melee"),IntentTag=(TagName="InputIntent.Attack.Light"))')
combo.set_editor_property('input_bindings', [binding])
assert unreal.EditorAssetLibrary.save_loaded_asset(combo)
ability_set = data('AS_LightCombo', unreal.HodgeAbilitySet)
entries = []
for definition in definitions:
    entry = unreal.HodgeAbilitySet_Definition()
    assert entry.import_text('(Definition="%s",AbilityLevel=1)' % definition.get_path_name())
    entries.append(entry)
ability_set.set_editor_property('granted_ability_definitions', entries)
assert unreal.EditorAssetLibrary.save_loaded_asset(ability_set)
print('CREATED_DEFINITION_COMBO', ROOT, len(definitions), 'definitions')
