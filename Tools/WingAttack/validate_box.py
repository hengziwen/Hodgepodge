"""只读核对正式翼 Box 的尺寸、窗口、身份去重与服务器开关。"""
import json
from pathlib import Path
import unreal

WORK = Path('E:/Project/Git/Hodgepodge/Saved/WingBox')
profile = unreal.load_asset('/Game/Main/Character/Hero/Wing/DA_PoverWing_Anzhu')
montage = profile.attack_montage
events = unreal.AnimationLibrary.get_animation_notify_events(montage)
hits = [e.notify_state_class.hit for e in events if isinstance(e.notify_state_class, unreal.HodgeAnimNotifyState_HitCheck)]
wing = next(h for h in hits if h.source == unreal.HodgeAnimHitSource.NAMED_COMPONENT and str(h.component_name) == 'WingMesh')
primary = next(h for h in hits if h.source == unreal.HodgeAnimHitSource.CHARACTER_MESH_SOCKET)
cal = json.loads((WORK / 'configuration.json').read_text(encoding='utf-8'))
checks = []

def check(name, value):
    checks.append({'check': name, 'passed': bool(value)})
    assert value, name

check('翅膀使用 Box', wing.shape == unreal.HodgeHitShape.BOX)
check('跟随真实翼根骨骼', str(wing.bone_or_socket) == 'Bone_Wing000' and wing.transform_policy == unreal.HodgeHitTransformPolicy.FOLLOW)
check('三轴半尺寸为正', min(wing.box_half_extent.x, wing.box_half_extent.y, wing.box_half_extent.z) > 0)
check('偏移不重复缩放', wing.local_transform.scale3d == unreal.Vector(1, 1, 1))
check('开启服务器翼姿势采样', profile.enable_authority_hit_sampling)
check('沿用同刀去重', str(wing.hit_group) == str(primary.hit_group) != 'None' and wing.attack_phase == primary.attack_phase)
check('沿用默认伤害及受击', wing.use_default_damage and wing.use_default_reaction)
check('不是重复伤害', wing.repeat_hit_interval == 0.0)
check('没有新增或删除通知', len(events) == cal['notify_count'])
check('材质淡出保留', abs(profile.fade_out_seconds - .35) < .0001 and len(profile.materials) == 3)
check('真实实体顶点参与测量', cal['vertices'] > 0 and cal['samples'] >= 2 and cal['margin'] == 4.0)
check('锚点存在于新骨架', 'Bone_Wing000' in [str(n) for n in unreal.HodgeAnimationAuthoringLibrary.get_skeleton_bone_names(profile.mesh.skeleton)])
result = {'passed': all(c['passed'] for c in checks), 'checks': checks}
(WORK / 'asset-validation.json').write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps(result, ensure_ascii=False))
