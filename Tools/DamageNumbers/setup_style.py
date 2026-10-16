"""仅创建 Hodge 跳字样式；迁入资源保持 Lyra 原始字节，实际依赖另行记录。"""
import json
from pathlib import Path
import unreal

ROOT = Path(unreal.Paths.project_dir())
PACKAGE = '/Game/Main/UI/Feedback/DA_DamageNumberStyle'
assert unreal.Paths.get_project_file_path().endswith('Hodgepodge.uproject')
asset = unreal.load_asset(PACKAGE) if unreal.EditorAssetLibrary.does_asset_exist(PACKAGE) else None
if not asset:
    factory = unreal.DataAssetFactory()
    factory.set_editor_property('DataAssetClass', unreal.HodgeDamagePopStyleNiagara)
    asset = unreal.AssetToolsHelpers.get_asset_tools().create_asset('DA_DamageNumberStyle', '/Game/Main/UI/Feedback', unreal.HodgeDamagePopStyleNiagara, factory)
    assert asset
    asset.set_editor_property('TextNiagara', unreal.load_asset('/Game/Effects/Particles/Impacts/NS_DamageNumbers'))
    asset.set_editor_property('NiagaraArrayName', 'DamageInfo')
    asset.set_editor_property('MaxBufferedRequests', 64)
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset)
assert asset.get_editor_property('TextNiagara')
registry = unreal.AssetRegistryHelpers.get_asset_registry()
options = unreal.AssetRegistryDependencyOptions(True, True, False, False, False)
pending, seen, rows = ['/Game/Effects/Particles/Impacts/NS_DamageNumbers'], set(), []
while pending:
    package = pending.pop()
    if package in seen:
        continue
    seen.add(package)
    dependencies = [str(v) for v in registry.get_dependencies(package, options)]
    rows.append({'package': package, 'dependencies': dependencies})
    for dependency in dependencies:
        if dependency.startswith('/Game/'):
            assert unreal.EditorAssetLibrary.does_asset_exist(dependency), dependency
            pending.append(dependency)
    assert unreal.load_asset(package), package
report = {'style': PACKAGE, 'dependencies': rows, 'loaded': sorted(seen)}
(ROOT / 'Saved/DamageNumbers/asset-audit.json').write_text(json.dumps(report, indent=2), encoding='utf-8')
print(json.dumps(report))
