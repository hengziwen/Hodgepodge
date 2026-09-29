"""Temporarily configure authority event edges, keeping the default combo assets on disk unchanged."""
import json
import unreal

root = '/Game/CodexText/DefinitionCombo/'
backup = '/Game/CodexText/Backups/DefinitionCombo_20260928/'
table = unreal.load_asset(root + 'DT_LightCombo')
timeline = unreal.load_asset(root + 'DA_Attack_1_Timeline')
for asset in [table, timeline]:
    destination = backup + asset.get_name() + '_BeforeEventTest'
    if not unreal.EditorAssetLibrary.does_asset_exist(destination):
        assert unreal.EditorAssetLibrary.duplicate_asset(asset.get_path_name(), destination)
rows = json.loads(unreal.DataTableFunctionLibrary.export_data_table_to_json_string(table))
for row in rows:
    if row['Name'] == 'Combo.Light.01':
        row['Transitions'].append('(TriggerEventTag=(TagName="GameplayEvent.Attack.Test"),TargetComboTag=(TagName="Combo.Light.03"),TransitionPriority=100)')
    if row['Name'] == 'Combo.Light.03':
        row['Transitions'].append('(TriggerEventTag=(TagName="GameplayEvent.Attack.Timeline.End"),TargetComboTag=(TagName="Combo.Light.04"),TransitionPriority=100)')
assert unreal.DataTableFunctionLibrary.fill_data_table_from_json_string(table, json.dumps(rows))
events = list(timeline.get_editor_property('events'))
point = unreal.HodgeTimelineEvent()
assert point.import_text('(Kind=Point,EventID="AuthorityBranchTest",StartTime=%.9f,PointEventTag=(TagName="GameplayEvent.Attack.Test"),NetPolicy=AuthorityOnly)' % globals().get('POINT_TIME', .8))
events.append(point)
timeline.set_editor_property('events', events)
unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_begin_play()
print('PREPARED_AUTHORITY_EVENT_TEST_WITHOUT_SAVE')
