"""通过编辑器 MCP 创建独立近战测试配置，不切换正式 Experience 或启动 PIE。"""
import json
from pathlib import Path

import unreal


ROOT = '/Game/CodexText/CombatHitWindows/Fixture'
EXPECTED = 'e:/project/git/hodgepodge/hodgepodge.uproject'
actual = unreal.Paths.convert_relative_path_to_full(unreal.Paths.get_project_file_path())
assert actual.replace('\\', '/').casefold() == EXPECTED, actual
assert unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world() is None
assets = []


def duplicate(source, name):
    destination = ROOT + '/' + name
    assert not unreal.EditorAssetLibrary.does_asset_exist(destination), destination
    asset = unreal.EditorAssetLibrary.duplicate_asset(source, destination)
    assert asset, destination
    assets.append(asset)
    return asset


def source(tag, component='', sockets=(), offset=(0, 0, 0), radius=25):
    entry = unreal.HodgeHitSource()
    entry.import_text('(SourceTag=(TagName="' + tag + '"))')
    entry.set_editor_property('component_name', component)
    entry.set_editor_property('sockets', list(sockets))
    entry.set_editor_property('local_offset', unreal.Vector(*offset))
    entry.set_editor_property('radius', radius)
    return entry


# 测试角色保留原角色动画，只新增测试来源及一个不参与实体碰撞的 Box。
hero = duplicate('/Game/Main/Character/Hero/BP_Hero_Pover', 'BP_MeleeTestHero')
subsystem = unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
library = unreal.SubobjectDataBlueprintFunctionLibrary
handles = subsystem.k2_gather_subobject_data_for_blueprint(hero)
parent = next(h for h in handles if isinstance(library.get_object(library.get_data(h)), unreal.CapsuleComponent))
handle, failure = subsystem.add_new_subobject(unreal.AddNewSubobjectParams(
    parent_handle=parent, new_class=unreal.BoxComponent, blueprint_context=hero))
assert not str(failure), str(failure)
assert subsystem.rename_subobject(handle, 'MeleeTestHitBox')
box = library.get_object(library.get_data(handle))
box.set_editor_property('relative_location', unreal.Vector(120, 0, 0))
box.set_box_extent(unreal.Vector(50, 40, 60), False)
box.set_collision_enabled(unreal.CollisionEnabled.NO_COLLISION)
unreal.BlueprintEditorLibrary.compile_blueprint(hero)
hero_class = unreal.EditorAssetLibrary.load_blueprint_class(ROOT + '/BP_MeleeTestHero')
hero_default = unreal.get_default_object(hero_class)
# Mesh 有固定旋转和高度偏移，把角色前方位置转换为 Mesh 局部坐标。
body_offset = unreal.MathLibrary.inverse_transform_location(
    hero_default.get_editor_property('mesh').get_relative_transform(), unreal.Vector(120, 0, 0))
hit_sources = [
    source('Combat.Source.Body.Origin', offset=(body_offset.x, body_offset.y, body_offset.z), radius=55),
    source('Combat.Source.Hitbox.Chest', component='MeleeTestHitBox'),
]
# 来源参数交给 Experience 添加的组件蓝图，不再写入角色默认子对象。
source_path = Path(unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir())) / 'Tools/Combat/melee_fixture_hit_sources.json'
source_path.write_text(json.dumps([entry.export_text() for entry in hit_sources], ensure_ascii=False, indent=2), encoding='utf-8')

# 使用已核实存在的武器骨骼；此配置仅验证来源路由，不代表完整刀身覆盖。
weapon = duplicate('/Game/Main/Weapon/BP_WeaponInstance_Sword', 'BP_MeleeTestWeaponInstance')
weapon_class = unreal.EditorAssetLibrary.load_blueprint_class(ROOT + '/BP_MeleeTestWeaponInstance')
unreal.get_default_object(weapon_class).set_editor_property('hit_sources', [
    source('Combat.Source.Weapon.MainHand', component='SkeletalMesh', sockets=('Sword_Bone01',)),
])
equipment = duplicate('/Game/Main/Data/Equipments/BP_Equipment_Sword', 'BP_MeleeTestEquipment')
equipment_class = unreal.EditorAssetLibrary.load_blueprint_class(ROOT + '/BP_MeleeTestEquipment')
unreal.get_default_object(equipment_class).set_editor_property('instance_type', weapon_class)

# 只给测试 AbilitySet 添加基础伤害效果，不再添加第二份 CombatSet。
factory = unreal.BlueprintFactory()
factory.set_editor_property('parent_class', unreal.GameplayEffect)
assert not unreal.EditorAssetLibrary.does_asset_exist(ROOT + '/GE_MeleeTestAttributes')
effect = unreal.AssetToolsHelpers.get_asset_tools().create_asset('GE_MeleeTestAttributes', ROOT, None, factory)
assert effect
assets.append(effect)
effect_class = unreal.EditorAssetLibrary.load_blueprint_class(ROOT + '/GE_MeleeTestAttributes')
effect_default = unreal.get_default_object(effect_class)
effect_default.set_editor_property('duration_policy', unreal.GameplayEffectDurationType.INFINITE)
modifier = unreal.GameplayModifierInfo()
modifier.import_text('(Attribute=(AttributeName="BaseDamage",Attribute=/Script/Hodgepodge.HodgeCombatSet:BaseDamage,'
                     'AttributeOwner="/Script/Hodgepodge.HodgeCombatSet"),'
                     'ModifierOp=Override,ModifierMagnitude=(MagnitudeCalculationType=ScalableFloat,'
                     'ScalableFloatMagnitude=(Value=20.0)))')
assert 'HodgeCombatSet:BaseDamage' in modifier.export_text(), modifier.export_text()
effect_default.set_editor_property('modifiers', [modifier])

original_set = unreal.load_asset('/Game/CodexText/DefinitionCombo/AS_LightCombo')
original_combo = unreal.load_asset('/Game/CodexText/DefinitionCombo/DA_LightCombo')
combo_table = duplicate(original_combo.get_editor_property('combo_table').get_path_name(), 'DT_MeleeTestCombo')
rows = json.loads(unreal.DataTableFunctionLibrary.export_data_table_to_json_string(combo_table))
rows = [row for row in rows if row['Name'] in ('Combo.Entry', 'Combo.Light.01')]
assert len(rows) == 2
next(row for row in rows if row['Name'] == 'Combo.Light.01')['Transitions'] = []
assert unreal.DataTableFunctionLibrary.fill_data_table_from_json_string(combo_table, json.dumps(rows))
combo = duplicate(original_combo.get_path_name(), 'DA_MeleeTestCombo')
combo.set_editor_property('combo_table', combo_table)
pawn_path = '/Game/Main/Data/PawnData/DA_Dafult_PawnData'
for mode in ('Body', 'Weapon', 'HitBox'):
    ability_set = duplicate(original_set.get_path_name(), 'AS_MeleeTest_' + mode)
    definitions = list(ability_set.get_editor_property('granted_ability_definitions'))
    assert definitions
    assert definitions[0].get_editor_property('definition').get_name() == 'DA_Attack_1'
    old_path = definitions[0].get_editor_property('definition').get_path_name()
    new_path = unreal.load_asset('/Game/CodexText/CombatHitWindows/DA_Test_' + mode).get_path_name()
    replacement = unreal.HodgeAbilitySet_Definition()
    replacement.import_text(definitions[0].export_text().replace(old_path, new_path))
    definitions[0] = replacement
    ability_set.set_editor_property('granted_ability_definitions', definitions[:1])
    effect_entry = unreal.HodgeAbilitySet_GameplayEffect()
    effect_entry.import_text('(GameplayEffect="' + effect_class.get_path_name() + '",EffectLevel=1.0)')
    effects = list(ability_set.get_editor_property('granted_gameplay_effects'))
    effects.append(effect_entry)
    ability_set.set_editor_property('granted_gameplay_effects', effects)
    attributes = ability_set.get_editor_property('granted_attributes')
    assert all(x.get_editor_property('attribute_set') != unreal.HodgeCombatSet.static_class() for x in attributes)
    pawn = duplicate(pawn_path, 'DA_MeleeTestPawn_' + mode)
    sets = list(pawn.get_editor_property('ability_sets'))
    assert original_set in sets
    pawn.set_editor_property('ability_sets', [ability_set if x == original_set else x for x in sets])
    pawn.set_editor_property('pawn_class', hero_class)
    pawn.set_editor_property('default_weapon_definition', equipment_class)
    pawn.set_editor_property('combo_definition', combo)

# 编译结果由 MCP 原生 compile 接口复查，数据资产只保存本次创建的文件。
for asset in assets:
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=False), asset.get_path_name()
report = {'assets': [a.get_path_name() for a in assets], 'main_configuration_changed': False, 'pie_started': False}
output = Path(unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_saved_dir())) / 'Tests/MeleeMigration/fixture_assets.json'
output.write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps(report, ensure_ascii=False))
print('Next: python Tools/Combat/configure_melee_fixture_experience.py')
