import unreal
from pathlib import Path

assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
assert unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).load_level('/Game/ThirdPerson/Maps/ThirdPersonMap')
root = Path(unreal.Paths.project_dir())
exec(compile((root / 'Tools/HitReaction/start_two_clients.py').read_text(encoding='utf-8'), 'start_two_clients.py', 'exec'))
