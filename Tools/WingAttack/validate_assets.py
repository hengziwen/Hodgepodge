"""只读核对 WING-02/03/07/11 的正式与预览资产契约。"""
import unreal,json
from pathlib import Path
WORK=Path('E:/Project/Git/Hodgepodge/Saved/WingAttack')
ROOT='/Game/Main/Character/Hero/Wing'
profile=unreal.load_asset(ROOT+'/DA_PoverWing_Anzhu')
component=unreal.load_asset(ROOT+'/BP_PoverWing_Anzhu')
old=unreal.load_asset(ROOT+'/BP_PoverWingComponent')
experience=unreal.load_asset('/Game/Main/Experiences/Exp_HodgeDefaultExperience')
checks=[]
def check(req,name,condition):
    checks.append({'requirement':req,'check':name,'passed':bool(condition)})
check('WING-07','动画模式',profile.playback_mode==unreal.HodgeWingPlaybackMode.ANIMATION_ASSET)
check('WING-02','仅已校准源映射',len(profile.animation_bindings)==2)
check('WING-07','统一校准缩放',abs(profile.mesh_offset.scale3d.x-.8)<.0001)
for binding in profile.animation_bindings:
    events=unreal.AnimationLibrary.get_animation_notify_events(binding.source_animation)
    windows=[e for e in events if isinstance(e.notify_state_class,unreal.HodgeAnimNotifyState_WingAction)]
    check('WING-03','源显示窗口唯一:'+binding.source_animation.get_name(),len(windows)==1)
    check('WING-07','独立翼骨架:'+binding.source_animation.get_name(),binding.wing_animation.get_editor_property('skeleton')==profile.mesh.skeleton)
new_entries=[];old_entries=[]
for action in unreal.get_default_object(experience.generated_class()).get_editor_property('Actions'):
    if action.get_class().get_name()!='GameFeatureAction_AddComponents':continue
    for entry in action.get_editor_property('ComponentList'):
        cls=entry.get_editor_property('ComponentClass')
        if cls==component.generated_class():new_entries.append(entry)
        if cls==old.generated_class():old_entries.append(entry)
check('WING-12','新组件唯一且各端注入',len(new_entries)==1 and all(e.get_editor_property('bClientComponent') and e.get_editor_property('bServerComponent') for e in new_entries))
check('WING-07','旧组件仍关闭',bool(old_entries) and all(not e.get_editor_property('bClientComponent') and not e.get_editor_property('bServerComponent') for e in old_entries))
for comp in unreal.ObjectIterator(unreal.HodgeWingComponent):
    if comp.get_path_name().startswith('/Game/Main/WingText.'):
        check('WING-11','预览复用现有网格',comp.existing_mesh_component_name=='SkeletalMesh' and comp.profile==profile)
result={'passed':all(c['passed'] for c in checks),'checks':checks}
(WORK/'asset-validation.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps(result,ensure_ascii=False));assert result['passed']
