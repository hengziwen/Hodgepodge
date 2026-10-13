"""Check editor state before the scoped wing import."""
import unreal, json
print(json.dumps({'pie': len(unreal.EditorLevelLibrary.get_pie_worlds(False)),
 'dirty_content': [p.get_path_name() for p in unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()],
 'dirty_maps': [p.get_path_name() for p in unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()]}, ensure_ascii=False))
