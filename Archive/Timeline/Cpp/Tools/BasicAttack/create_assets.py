"""Author the requested five-step GA. Run through the Unreal MCP Python capability."""
import unreal

ROOT = '/Game/CodexText/BasicAttack'
BACKUP = '/Game/CodexText/Backups/BasicAttack_20260924/DA_Pover'
assets = unreal.AssetToolsHelpers.get_asset_tools()

def tag(name):
    value = unreal.GameplayTag()
    value.import_text('(TagName="' + name + '")')
    return value

def create(name, cls, factory):
    path = ROOT + '/' + name
    assert not unreal.EditorAssetLibrary.does_asset_exist(path), 'Inspect existing asset before overwriting: ' + path
    result = assets.create_asset(name, ROOT, cls, factory)
    assert result
    return result

steps = []
for index in range(1, 6):
    montage = unreal.load_asset('/Game/CodexText/Montage/AM_Attack%02d_Montage' % index)
    assert montage
    factory = unreal.DataAssetFactory()
    factory.set_editor_property('data_asset_class', unreal.HodgeAbilityTimeline)
    timeline = create('DA_Attack%02d_Timeline' % index, unreal.HodgeAbilityTimeline, factory)
    duration = montage.get_play_length()
    timeline.set_editor_property('duration', duration)
    events = []
    windows = [('Recovery', 'Status.Attack.Recovery', 0), ('MoveCancel', 'Status.Attack.Cancel.Move', 10)]
    if index < 5:
        windows.insert(0, ('NextAttack', 'Status.Attack.Cancel.NextAttack', -10))
    for event_id, window_tag, priority in windows:
        event = unreal.HodgeTimelineEvent()
        assert event.import_text('(Kind=Window,EventID="%s",StartTime=%.9f,EndTime=%.9f,Priority=%d,WindowTag=(TagName="%s"))' % (event_id, duration * 0.6, duration, priority, window_tag))
        events.append(event)
    timeline.set_editor_property('events', events)
    assert unreal.EditorAssetLibrary.save_loaded_asset(timeline)
    step = unreal.HodgeBasicAttackStep()
    assert step.import_text('(Montage="%s",Timeline="%s")' % (montage.get_path_name(), timeline.get_path_name()))
    steps.append(step)

factory = unreal.BlueprintFactory()
factory.set_editor_property('parent_class', unreal.HodgeGameplayAbility_BasicAttack)
bp = create('GA_BasicAttack', unreal.Blueprint, factory)
cdo = unreal.get_default_object(bp.generated_class())
cdo.set_editor_property('attack_steps', steps)
print('CREATED', bp.get_path_name(), 'steps', len(cdo.get_editor_property('attack_steps')))
print('Compile GA_BasicAttack with saveAfterCompile before updating grant.')
