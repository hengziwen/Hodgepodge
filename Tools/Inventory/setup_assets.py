"""制作数据层所需定义并接入正式 Experience；先备份现有资产，不制作界面。"""
import json
import shutil
from pathlib import Path
import unreal

ROOT = Path(unreal.Paths.project_dir()).resolve()
WORK = ROOT / 'Saved/Inventory'
BACKUP = WORK / 'BeforeInventory'
BACKUP.mkdir(parents=True, exist_ok=True)
assert unreal.Paths.get_project_file_path().endswith('Hodgepodge.uproject')
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
assets = unreal.AssetToolsHelpers.get_asset_tools()


def backup(asset):
    """保留首次修改前的实际包字节，不覆盖已经保存的基线备份。"""
    package = asset.get_path_name().split('.')[0]
    source = ROOT / 'Content' / (package.removeprefix('/Game/') + '.uasset')
    destination = BACKUP / source.relative_to(ROOT)
    destination.parent.mkdir(parents=True, exist_ok=True)
    if not destination.exists():
        shutil.copy2(source, destination)
    return {'asset': package, 'backup': str(destination)}


def definition(name, stack, equipment=None):
    """只创建本任务物品类，已有类读取校验而不覆盖手工配置。"""
    path = '/Game/Main/Data/Inventory/' + name
    existing = unreal.load_asset(path) if unreal.EditorAssetLibrary.does_asset_exist(path) else None
    if not existing:
        factory = unreal.BlueprintFactory()
        factory.set_editor_property('ParentClass', unreal.HodgeInventoryItemDefinition)
        existing = assets.create_asset(name, '/Game/Main/Data/Inventory', unreal.Blueprint, factory)
        assert existing
        cdo = unreal.get_default_object(existing.generated_class())
        cdo.set_editor_property('DisplayName', '测试材料' if not equipment else '长剑')
        cdo.set_editor_property('MaxStackCount', stack)
        if equipment:
            fragment = unreal.new_object(unreal.HodgeInventoryFragment_EquippableItem, outer=cdo)
            fragment.set_editor_property('EquipmentDefinition', equipment)
            cdo.set_editor_property('Fragments', [fragment])
        unreal.BlueprintEditorLibrary.compile_blueprint(existing)
        assert unreal.EditorAssetLibrary.save_loaded_asset(existing)
    cdo = unreal.get_default_object(existing.generated_class())
    assert cdo.get_editor_property('MaxStackCount') == stack
    return existing.generated_class()


pawn = unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData')
experience_asset = unreal.load_asset('/Game/Main/Experiences/Exp_HodgeDefaultExperience')
assert pawn and experience_asset
experience = unreal.get_default_object(experience_asset.generated_class()) if isinstance(experience_asset, unreal.Blueprint) else experience_asset
# 先读取实际默认武器和 Action，继承现有手工配置，不按文档猜测当前引用。
weapon = pawn.get_editor_property('DefaultWeaponDefinition')
assert weapon
actions = list(experience.get_editor_property('Actions'))
records = [backup(pawn), backup(experience_asset)]
sword = definition('ID_Sword', 1, weapon)
material = definition('ID_TestMaterial', 20)
if not pawn.get_editor_property('bUseInventory'):
    grant = unreal.HodgeInventoryDefaultItem()
    grant.set_editor_property('Definition', sword)
    grant.set_editor_property('Count', 1)
    grant.set_editor_property('Level', 1)
    grant.set_editor_property('bEquip', True)
    pawn.set_editor_property('DefaultInventoryItems', [grant])
    pawn.set_editor_property('InventoryMaxEntries', 40)
    pawn.set_editor_property('bUseInventory', True)
component_actions = [a for a in actions if a.get_class().get_name() == 'GameFeatureAction_AddComponents']
assert component_actions, '正式 Experience 必须已有组件注入 Action'
for component in [unreal.HodgeInventoryManagerComponent, unreal.HodgeEquipmentSelectionComponent]:
    found = any(any(
        component.static_class().get_path_name() in str(row.get_editor_property('ComponentClass'))
        for row in a.get_editor_property('ComponentList')) for a in component_actions)
    if found:
        continue
    # 复用实际存在的 Action 与反射结构，不依赖未暴露给 Python 的 UCLASS 名称。
    action = component_actions[0]
    entries = list(action.get_editor_property('ComponentList'))
    assert entries
    row = entries[0].copy()
    row.set_editor_property('ActorClass', unreal.HodgePlayerState)
    row.set_editor_property('ComponentClass', component)
    row.set_editor_property('bClientComponent', True)
    row.set_editor_property('bServerComponent', True)
    entries.append(row)
    action.set_editor_property('ComponentList', entries)
experience.set_editor_property('Actions', actions)
assert unreal.EditorAssetLibrary.save_loaded_asset(pawn)
if isinstance(experience_asset, unreal.Blueprint):
    unreal.BlueprintEditorLibrary.compile_blueprint(experience_asset)
assert unreal.EditorAssetLibrary.save_loaded_asset(experience_asset)
report = {'backups': records, 'sword': sword.get_path_name(), 'material': material.get_path_name(),
          'default_weapon': weapon.get_path_name(), 'actions': len(actions), 'ui_created': False}
(WORK / 'asset-setup.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps(report, ensure_ascii=False))
