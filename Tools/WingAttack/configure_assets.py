"""配置攻击限定的新翅膀，保留旧模式关闭和用户挂接校准。"""
import json
from pathlib import Path
import unreal

PROJECT=Path('E:/Project/Git/Hodgepodge')
WORK=PROJECT/'Saved/WingAttack'
ROOT='/Game/Main/Character/Hero/Wing'
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
owned={ROOT+'/DA_PoverWing_Anzhu',ROOT+'/BP_PoverWing_Anzhu','/Game/Main/WingText','/Game/Main/Experiences/Exp_HodgeDefaultExperience','/Game/Wuwa/Anim/AM_Attack04','/Game/Main/Character/Hero/Anim/Montages/AM_Attack04_Montage'}
assert all(p.get_path_name() in owned for p in unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages())
cal=json.loads((WORK/'calibration.json').read_text(encoding='utf-8'))
tools=unreal.AssetToolsHelpers.get_asset_tools()
source=unreal.load_asset('/Game/Wuwa/Anim/AM_Attack04')
montage=unreal.load_asset('/Game/Main/Character/Hero/Anim/Montages/AM_Attack04_Montage')
animation=unreal.load_asset('/Game/Wuwa/Wing/BaseAnim/B_Attack04')
mesh=unreal.load_asset('/Game/Wuwa/Wing/EDarkNanzhuWingMd10011/Model/EDarkNanzhuWingMd10011')
experience=unreal.load_asset('/Game/Main/Experiences/Exp_HodgeDefaultExperience')
preview=unreal.load_asset('/Game/Main/WingText')
before={asset.get_path_name():len(unreal.AnimationLibrary.get_animation_notify_events(asset)) for asset in [source,montage]}
for asset in [source,montage]:
    assert unreal.HodgeAnimationAuthoringLibrary.configure_wing_action_window(asset,.05,asset.get_play_length()-.05)

profile=unreal.load_asset(ROOT+'/DA_PoverWing_Anzhu')
if not profile:
    factory=unreal.DataAssetFactory();factory.set_editor_property('data_asset_class',unreal.HodgeWingProfile)
    profile=tools.create_asset('DA_PoverWing_Anzhu',ROOT,unreal.HodgeWingProfile,factory)
offset=unreal.Transform();assert offset.import_text(cal['offset'])
profile.set_editor_property('playback_mode',unreal.HodgeWingPlaybackMode.ANIMATION_ASSET)
profile.set_editor_property('mesh',mesh)
profile.set_editor_property('attach_socket',cal['bone'])
profile.set_editor_property('mesh_offset',offset)
profile.set_editor_property('attack_montage',montage)
bindings=[]
for asset in [montage,source]:
    binding=unreal.HodgeWingAnimationBinding()
    binding.set_editor_property('source_animation',asset)
    binding.set_editor_property('wing_animation',animation)
    bindings.append(binding)
profile.set_editor_property('animation_bindings',bindings)

component=unreal.load_asset(ROOT+'/BP_PoverWing_Anzhu')
if not component:
    factory=unreal.BlueprintFactory();factory.set_editor_property('parent_class',unreal.HodgeWingComponent)
    component=tools.create_asset('BP_PoverWing_Anzhu',ROOT,unreal.Blueprint,factory)
unreal.get_default_object(component.generated_class()).set_editor_property('Profile',profile)
unreal.BlueprintEditorLibrary.compile_blueprint(component)
assert unreal.HodgeAnimationAuthoringLibrary.configure_wing_experience(experience,unreal.HodgeHeroCharacter,component.generated_class())
assert unreal.HodgeAnimationAuthoringLibrary.configure_wing_attack_preview(preview,profile)
for asset in [component,preview,experience]:unreal.BlueprintEditorLibrary.compile_blueprint(asset)
for asset in [source,montage,profile,component,preview,experience]:
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset,False),asset.get_path_name()

result={'profile':profile.get_path_name(),'component':component.get_path_name(),'preview':preview.get_path_name(),
        'calibration':cal,'notify_counts_before':before,'notify_counts_after':{a.get_path_name():len(unreal.AnimationLibrary.get_animation_notify_events(a)) for a in [source,montage]}}
(WORK/'assets.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps(result,ensure_ascii=False))
