"""只读验证通知直接配置、AnimBP、窗口保留与现有 Box 契约。"""
import json
from pathlib import Path
import unreal

WORK = Path('E:/Project/Git/Hodgepodge/Saved/WingMontage')
profile = unreal.load_asset('/Game/Main/Character/Hero/Wing/DA_PoverWing_Anzhu')
checks = []

def check(name, condition):
    """记录并断言实际资产属性，失败时留下具体条目。"""
    checks.append({'requirement': 'WING-14', 'check': name, 'passed': bool(condition)})
    assert condition, name

check('翼 AnimBP 已配置', profile.anim_class and 'ABP_PoverWing_Anzhu' in profile.anim_class.get_path_name())
check('权威采样保留', profile.enable_authority_hit_sampling)
check('挂接缩放保留', profile.attach_socket == 'WingCase' and abs(profile.mesh_offset.scale3d.x - .8) < .0001)
check('材质和默认淡出保留', len(profile.materials) == 3 and abs(profile.fade_out_seconds - .35) < .0001)
before = json.loads((WORK / 'editor-before.json').read_text(encoding='utf-8'))
check('用户挂接偏移保留', profile.mesh_offset.export_text() == before['profile'])
for number in ['04', '05']:
    source = unreal.load_asset('/Game/Main/Character/Hero/Anim/Montages/AM_Attack' + number + '_Montage')
    events = unreal.AnimationLibrary.get_animation_notify_events(source)
    windows = [event for event in events if isinstance(event.notify_state_class, unreal.HodgeAnimNotifyState_WingAction)]
    check('已有窗口保留_' + number, len(windows) == 1)
    window = windows[0].notify_state_class
    check('通知直接选择翼蒙太奇_' + number, window.wing_montage and window.wing_montage.get_name() == 'AM_Wing_Attack' + number)
    check('独立翼骨架_' + number, window.wing_montage.get_editor_property('Skeleton') == profile.mesh.skeleton)
    check('默认倍率和淡出_' + number, window.play_rate == 1 and window.fade_out_seconds == -1)
    previous = next(row for row in before['windows'] if row['path'] == source.get_path_name().split('.')[0])
    check('窗口时间轨道原样保留_' + number, windows[0].export_text() == previous['event'])
    if number == '04':
        hits = [event.notify_state_class.hit for event in events if isinstance(event.notify_state_class, unreal.HodgeAnimNotifyState_HitCheck)]
        box = next(hit for hit in hits if hit.source == unreal.HodgeAnimHitSource.NAMED_COMPONENT and str(hit.component_name) == 'WingMesh')
        check('Box 和真实翼根保留', box.shape == unreal.HodgeHitShape.BOX and str(box.bone_or_socket) == 'Bone_Wing000')
result = {'passed': all(row['passed'] for row in checks), 'checks': checks}
(WORK / 'asset-validation.json').write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps(result, ensure_ascii=False))
