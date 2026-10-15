"""将现有翼窗口迁为通知内蒙太奇配置，保留用户窗口、挂接和命中参数。"""
import json
import re
from pathlib import Path
import unreal

ROOT = '/Game/Main/Character/Hero/Wing'
WORK = Path('E:/Project/Git/Hodgepodge/Saved/WingMontage')
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
# 失败重试只接受本工具已创建的目标包，不保存其他用户工作。
owned = {ROOT + '/ABP_PoverWing_Anzhu', ROOT + '/DA_PoverWing_Anzhu',
         ROOT + '/Montages/AM_Wing_Attack04', ROOT + '/Montages/AM_Wing_Attack05',
         '/Game/Main/Character/Hero/Anim/Montages/AM_Attack04_Montage',
         '/Game/Main/Character/Hero/Anim/Montages/AM_Attack05_Montage',
         '/Game/Wuwa/Anim/AM_Attack04', '/Game/Main/WingText'}
assert all(package.get_path_name() in owned for package in unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages())
tools = unreal.AssetToolsHelpers.get_asset_tools()
profile = unreal.load_asset(ROOT + '/DA_PoverWing_Anzhu')
written = []

# 新 AnimBP 仅包含参考姿势和 DefaultSlot，旧程序化 AnimBP 保持原样。
blueprint = unreal.load_asset(ROOT + '/ABP_PoverWing_Anzhu')
if not blueprint:
    factory = unreal.AnimBlueprintFactory()
    factory.set_editor_property('ParentClass', unreal.AnimInstance)
    factory.set_editor_property('TargetSkeleton', profile.mesh.skeleton)
    blueprint = tools.create_asset('ABP_PoverWing_Anzhu', ROOT, unreal.AnimBlueprint, factory)
    assert unreal.HodgeAnimationAuthoringLibrary.configure_wing_montage_graph(blueprint)
    diagnostic = unreal.HodgeAnimationAuthoringLibrary.compile_animation_blueprint(blueprint)
    assert diagnostic.startswith('ERRORS=0'), diagnostic
written.append(blueprint)

montages = {}
for number in ['04', '05']:
    name = 'AM_Wing_Attack' + number
    montage = unreal.load_asset(ROOT + '/Montages/' + name)
    if not montage:
        factory = unreal.AnimMontageFactory()
        factory.set_editor_property('TargetSkeleton', profile.mesh.skeleton)
        montage = tools.create_asset(name, ROOT + '/Montages', unreal.AnimMontage, factory)
        sequence = unreal.load_asset('/Game/Wuwa/Wing/BaseAnim/B_Attack' + number)
        assert unreal.HodgeAnimationAuthoringLibrary.configure_wing_montage_asset(montage, sequence)
    montages[number] = montage
    written.append(montage)

report = {'windows': [], 'blueprint': blueprint.get_path_name()}
paths = [('/Game/Main/Character/Hero/Anim/Montages/AM_Attack04_Montage', '04'),
         ('/Game/Main/Character/Hero/Anim/Montages/AM_Attack05_Montage', '05'),
         ('/Game/Wuwa/Anim/AM_Attack04', '04')]
for path, number in paths:
    source = unreal.load_asset(path)
    windows = [event for event in unreal.AnimationLibrary.get_animation_notify_events(source)
               if isinstance(event.notify_state_class, unreal.HodgeAnimNotifyState_WingAction)]
    assert windows, path
    for event in windows:
        notify = event.notify_state_class
        if not notify.wing_montage:
            # 旧第四号动作按绝对源时间同步，起播位置沿用当前通知起点。
            begin = float(re.findall(r'LinkValue=([\d.eE+-]+)', event.export_text())[-1])
            notify.set_editor_property('WingMontage', montages[number])
            notify.set_editor_property('PlayRate', 1.0)
            notify.set_editor_property('StartPosition', begin if number == '04' else 0.0)
            notify.set_editor_property('FadeOutSeconds', -1.0)
        report['windows'].append({'source': path, 'montage': notify.wing_montage.get_path_name(),
                                 'start': notify.start_position, 'rate': notify.play_rate,
                                 'event': event.export_text()})
    written.append(source)
profile.set_editor_property('AnimClass', blueprint.generated_class())
# 旧隐藏映射仅保留序列化兼容，运行时已不读取，Python 不修改受保护字段。
written.append(profile)
preview = unreal.load_asset('/Game/Main/WingText')
assert unreal.HodgeAnimationAuthoringLibrary.configure_wing_attack_preview(preview, profile)
unreal.BlueprintEditorLibrary.compile_blueprint(preview)
written.append(preview)
for asset in written:
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset, False), asset.get_path_name()
report['written'] = [asset.get_path_name() for asset in written]
(WORK / 'configuration.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps(report, ensure_ascii=False))
