"""Reload only the imported packages and verify persistent mesh/material/texture references."""
import hashlib
import json
import os
from pathlib import Path

import unreal

ROOT = '/Game/Wuwa/PoverEquipment'
PROJECT = Path('E:/Project/Git/Hodgepodge')
REPORT = PROJECT / 'Saved/PoverEquipmentImport'
SOURCE = Path(r'D:\下载\EdgeDownloads\漂子装备_by_微波叮桃子_414004e836adf6e38d5275515289704d\PoverEquipment')
DESTINATION = PROJECT / 'Content/Wuwa/PoverEquipment'

results = json.loads((REPORT / 'import-results.json').read_text(encoding='utf-8'))
models = json.loads((REPORT / 'source-models.json').read_text(encoding='utf-8'))
manifest = json.loads((REPORT / 'source-files.json').read_text(encoding='utf-8'))
checks = []


def check(condition, name, detail=None):
    checks.append({'check': name, 'passed': bool(condition), 'detail': detail})


assert not unreal.EditorLevelLibrary.get_pie_worlds(False), 'Stop PIE before asset reload.'
assets = [unreal.load_asset(item['path']) for item in results['imported']]
assert all(assets), 'An imported asset is missing before reload.'
if os.environ.get('HODGE_IMPORT_FRESH') == '1':
    check(True, 'Fresh commandlet loads saved packages', {'assets': len(assets)})
else:
    editors = unreal.get_editor_subsystem(unreal.AssetEditorSubsystem)
    for asset in assets:
        editors.close_all_editors_for_asset(asset)
        assert unreal.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=True)
    packages = list({asset.get_outer().get_path_name(): asset.get_outer() for asset in assets}.values())
    reloaded, error = unreal.EditorLoadingAndSavingUtils.reload_packages(
        packages, unreal.ReloadPackagesInteractionMode.ASSUME_NEGATIVE)
    check(reloaded and not str(error), 'Reload saved packages', str(error))

for item in results['imported']:
    asset = unreal.load_asset(item['path'])
    check(asset and asset.get_class().get_name() == item['type'], 'Load asset', item['path'])

material_results = []
lib = unreal.MaterialEditingLibrary
for repaired in results['repairs']:
    material = unreal.load_asset(repaired['material'])
    actual_textures = {texture.get_path_name() for texture in lib.get_used_textures(material) if texture}
    intended = {ref['resolved_texture'] for ref in repaired['references']}
    check(actual_textures == intended, 'Material texture dependencies', repaired['material'])
    for ref in repaired['references']:
        actual = lib.get_material_default_texture_parameter_value(material, ref['role'])
        check(actual and actual.get_path_name() == ref['resolved_texture'],
              'Texture parameter', repaired['material'] + ':' + ref['role'])
    check(material.get_editor_property('two_sided') == repaired['two_sided'],
          'Two-sided material', repaired['material'])
    blend = unreal.BlendMode.BLEND_TRANSLUCENT if repaired['transparent'] else unreal.BlendMode.BLEND_OPAQUE
    check(material.get_editor_property('blend_mode') == blend, 'Material transparency', repaired['material'])
    check(material.get_editor_property('used_with_skeletal_mesh'), 'Skeletal mesh material usage', repaired['material'])
    stats = lib.get_statistics(material)
    check(stats.num_pixel_shader_instructions > 0 and stats.num_vertex_shader_instructions > 0,
          'Compiled shader instructions', repaired['material'])
    material_results.append({'material': repaired['material'], 'textures': sorted(actual_textures),
                             'vertex_instructions': stats.num_vertex_shader_instructions,
                             'pixel_instructions': stats.num_pixel_shader_instructions})

mesh_results = []
for source in models:
    path = ROOT + '/' + Path(source['file']).stem
    mesh = unreal.load_asset(path)
    slots = list(mesh.get_editor_property('materials'))
    check(len(slots) == len(source['materials']), 'Material slot count', path)
    for slot, original in zip(slots, source['materials']):
        check(str(slot.material_slot_name) == original['name'], 'Original slot identity', path + ':' + original['name'])
        check(slot.material_interface and slot.material_interface.get_path_name() ==
              ROOT + '/Materials/' + original['name'] + '.' + original['name'],
              'Mesh material assignment', path + ':' + original['name'])
    skeleton = mesh.get_editor_property('skeleton')
    names = [str(name) for name in unreal.HodgeAnimationAuthoringLibrary.get_skeleton_bone_names(skeleton)]
    missing = [bone['name'] for bone in source['bones'] if bone['name'] not in names]
    check(not missing and len(names) == len(source['bones']) + 1, 'Preserved source skeleton', {'mesh': path, 'missing': missing})
    check(unreal.get_editor_subsystem(unreal.SkeletalMeshEditorSubsystem).get_lod_count(mesh) >= 1, 'Renderable mesh LOD', path)
    mesh_results.append({'mesh': path, 'source_bones': len(source['bones']), 'imported_bones': len(names),
                         'source_triangles': source['faces'], 'materials': [slot.material_interface.get_path_name() for slot in slots]})

for relative, path in results['textures'].items():
    texture = unreal.load_asset(path)
    packed = relative.startswith('Tex_0/') and Path(relative).stem.endswith(('_N', '_ID'))
    check(texture.get_editor_property('srgb') == (not packed), 'Texture color space', path)
    if packed:
        check(texture.get_editor_property('compression_settings') == unreal.TextureCompressionSettings.TC_MASKS,
              'Preserved packed channels', path)

for entry in manifest:
    for label, directory in [('Original source unchanged', SOURCE), ('Copied source unchanged', DESTINATION)]:
        file = directory / entry['relative']
        check(file.is_file() and hashlib.sha256(file.read_bytes()).hexdigest() == entry['sha256'], label, entry['relative'])

discovered = set(unreal.EditorAssetLibrary.list_assets(ROOT, recursive=True, include_folder=False))
expected = {item['path'] for item in results['imported']}
check(discovered == expected, 'Exact imported asset inventory', {'expected': len(expected), 'actual': len(discovered)})
dirty = [package.get_path_name() for package in unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
         if package.get_path_name().startswith(ROOT + '/')]
check(not dirty, 'All imported packages saved', dirty)
report = {'passed': all(item['passed'] for item in checks), 'checks': checks,
          'meshes': mesh_results, 'materials': material_results}
(REPORT / 'validation-results.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps({'passed': report['passed'], 'checks': len(checks),
                  'failed': [item for item in checks if not item['passed']]}, ensure_ascii=False))
assert report['passed'], 'Imported asset validation failed; see validation-results.json.'
