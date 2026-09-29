"""Migrate only the five Definition combo timelines; keep a byte-for-byte backup."""
import json
import shutil
from pathlib import Path
import unreal

root = '/Game/CodexText/DefinitionCombo'
project = Path(unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir()))
backup = project / 'Saved/Backups/TimelineMontageDuration_20260928'
backup.mkdir(parents=True, exist_ok=True)
validator = unreal.get_editor_subsystem(unreal.EditorValidatorSubsystem)
records = []
for index in range(1, 6):
    definition = unreal.load_asset(root + '/DA_Attack_%d' % index)
    assert definition
    config = definition.get_editor_property('execution_config')
    timeline = config.timeline_task_config.timeline
    assert timeline
    package = timeline.get_path_name().split('.')[0]
    assert package.startswith('/Game/CodexText/'), package
    relative = package.removeprefix('/Game/') + '.uasset'
    source = project / 'Content' / relative
    destination = backup / relative
    destination.parent.mkdir(parents=True, exist_ok=True)
    if not destination.exists():
        shutil.copy2(source, destination)
    before = timeline.get_editor_property('duration')
    events = [event.export_text() for event in timeline.get_editor_property('events')]
    timeline.set_editor_property('use_montage_duration', True)
    for asset in (timeline, definition):
        result, errors, warnings = validator.is_object_valid(asset, unreal.DataValidationUsecase.SCRIPT)
        assert result == unreal.DataValidationResult.VALID, (asset.get_path_name(), errors)
    assert timeline.get_editor_property('duration') == before
    assert [event.export_text() for event in timeline.get_editor_property('events')] == events
    assert unreal.EditorAssetLibrary.save_loaded_asset(timeline)
    records.append({'timeline': timeline.get_path_name(), 'montage_length': config.montage.get_play_length(),
                    'ignored_legacy_duration': before, 'use_montage_duration': True})
output = project / 'Saved/Tests/timeline_montage_duration_migration.json'
output.parent.mkdir(parents=True, exist_ok=True)
output.write_text(json.dumps(records, indent=2), encoding='utf-8')
print('TIMELINE_MONTAGE_DURATION_MIGRATED', json.dumps(records))
