"""Create the procedural wing assets and modify only the actual five-hit combo finisher."""
import json
import re
import shutil
from pathlib import Path

import unreal

assert globals().get('ENABLE_POVER_WING', False), 'Wing is disabled by user request; explicitly enable it before re-authoring.'

PROJECT = Path('E:/Project/Git/Hodgepodge')
WORK = PROJECT / 'Saved/WingImplementation'
ROOT = '/Game/Main/Character/Hero/Wing'
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
assert all(p.get_path_name().startswith(ROOT + '/') or p.get_path_name() == '/Game/Wuwa/PoverEquipment/残象之翼'
           for p in unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages())
tools = unreal.AssetToolsHelpers.get_asset_tools()
lib = unreal.HodgeAnimationAuthoringLibrary
written = []


def asset(name, cls, factory):
    path = ROOT + '/' + name
    value = unreal.load_asset(path)
    if not value:
        value = tools.create_asset(name, ROOT, cls, factory)
    assert value, path
    written.append(value.get_path_name())
    return value


def backup(package):
    source = PROJECT / 'Content' / (package.removeprefix('/Game/') + '.uasset')
    target = WORK / 'BeforeAssets' / source.relative_to(PROJECT)
    if source.exists() and not target.exists():
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, target)


def tag_name(value):
    if isinstance(value, dict):
        return value.get('TagName', '')
    if isinstance(value, str):
        match = re.search(r'TagName="([^"]+)"', value)
        return match.group(1) if match else ('' if value in ('None', '(TagName="")') else value)
    return value or ''


combo = unreal.load_asset('/Game/Main/Data/Combo/DA_LightCombo')
rows = json.loads(unreal.DataTableFunctionLibrary.export_data_table_to_json_string(combo.combo_table))
(WORK / 'combo-rows.json').write_text(json.dumps(rows, ensure_ascii=False, indent=2), encoding='utf-8')
by_tag = {tag_name(row['ComboTag']): row for row in rows}
entry = str(combo.entry_combo_tag.get_editor_property('TagName'))
node = entry
seen = set()
chain = []
while node not in seen:
    seen.add(node)
    row = by_tag[node]
    if tag_name(row['AbilityTag']):
        chain.append(tag_name(row['AbilityTag']))
    edges = []
    for edge in row['Transitions']:
        if isinstance(edge, str):
            trigger = re.search(r'TriggerInputIntentTag=\(TagName="([^"]*)"\)', edge)
            target = re.search(r'TargetComboTag=\(TagName="([^"]*)"\)', edge)
            edge = {'TriggerInputIntentTag': trigger.group(1) if trigger else '',
                    'TargetComboTag': target.group(1) if target else ''}
        if tag_name(edge['TriggerInputIntentTag']):
            edges.append(edge)
    if not edges:
        break
    node = tag_name(edges[0]['TargetComboTag'])
assert len(chain) == 5, chain
definitions = [unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_' + str(i)) for i in range(1, 6)]
finisher = next(d for d in definitions if str(d.ability_tag.get_editor_property('TagName')) == chain[-1])
montage = finisher.execution_config.montage
assert montage
backup(montage.get_path_name().split('.')[0])
backup('/Game/Main/Experiences/Exp_HodgeDefaultExperience')
backup('/Game/Wuwa/PoverEquipment/残象之翼')

mesh = unreal.load_asset('/Game/Wuwa/PoverEquipment/残象之翼')
for name, bone in [('WingHitRoot', 'Wing000'), ('WingHitMid', 'Wing003'), ('WingHitTip', 'Wing006')]:
    socket = mesh.find_socket(name)
    assert socket, 'Run Tools/Wing/create_sockets.py through the native socket capability first.'
    assert str(socket.bone_name) == bone

factory = unreal.AnimBlueprintFactory()
factory.set_editor_property('ParentClass', unreal.HodgeWingAnimInstance)
factory.set_editor_property('TargetSkeleton', mesh.skeleton)
anim = asset('ABP_PoverWing', unreal.AnimBlueprint, factory)
assert lib.configure_wing_animation(anim)
for index, variable in enumerate(['WingRootRotation', 'WingMidRotation', 'WingTipRotation']):
    assert lib.set_animation_node_property(anim, anim.get_path_name() + ':AnimGraph.WingRotation' + str(index),
                                           'VariableReference', '(MemberParent=None,MemberName="' + variable + '",bSelfContext=True)')
diagnostic = lib.compile_animation_blueprint(anim)
assert diagnostic.startswith('ERRORS=0'), diagnostic

factory = unreal.DataAssetFactory()
factory.set_editor_property('DataAssetClass', unreal.HodgeWingProfile)
profile = asset('DA_PoverWing', unreal.HodgeWingProfile, factory)
profile.set_editor_properties({'mesh': mesh, 'anim_class': anim.generated_class(),
                              'attach_socket': 'WingAttach', 'mesh_offset': unreal.Transform(),
                              'cancel_return_seconds': .18})
windup = unreal.HodgeWingPose()
windup.set_editor_properties({'root': unreal.Rotator(pitch=-30, yaw=0, roll=0),
                             'mid': unreal.Rotator(pitch=0, yaw=-12, roll=0),
                             'tip': unreal.Rotator(pitch=0, yaw=-8, roll=0)})
strike = unreal.HodgeWingPose()
strike.set_editor_properties({'root': unreal.Rotator(pitch=65, yaw=0, roll=0),
                             'mid': unreal.Rotator(pitch=0, yaw=18, roll=0),
                             'tip': unreal.Rotator(pitch=0, yaw=12, roll=0)})
profile.set_editor_properties({'windup_pose': windup, 'strike_pose': strike})
assert lib.configure_wing_finisher(montage, profile)

factory = unreal.BlueprintFactory()
factory.set_editor_property('ParentClass', unreal.HodgeWingComponent)
component = asset('BP_PoverWingComponent', unreal.Blueprint, factory)
unreal.get_default_object(component.generated_class()).set_editor_property('Profile', profile)
diagnostic_component = lib.compile_animation_blueprint(component)
assert diagnostic_component.startswith('ERRORS=0'), diagnostic_component
experience = unreal.load_asset('/Game/Main/Experiences/Exp_HodgeDefaultExperience')
hero = unreal.load_asset('/Game/Main/Character/Hero/BP_Hero_Pover')
assert lib.configure_wing_experience(experience, hero.generated_class(), component.generated_class())
unreal.BlueprintEditorLibrary.compile_blueprint(experience)
for value in [anim, profile, component, experience, mesh, montage]:
    assert unreal.EditorAssetLibrary.save_loaded_asset(value)
result = {'assets': written, 'combo_chain': chain, 'finisher': finisher.get_path_name(),
          'montage': montage.get_path_name(), 'windup_start': profile.windup_start,
          'strike_start': profile.strike_start, 'strike_peak': profile.strike_peak,
          'recovery_end': profile.recovery_end, 'anim_compile': diagnostic,
          'component_compile': diagnostic_component, 'preserved_user_socket': 'WingAttach'}
WORK.mkdir(exist_ok=True)
(WORK / 'assets.json').write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps(result, ensure_ascii=False))
