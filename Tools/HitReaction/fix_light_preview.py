"""Give the additive montage a matching constant preview base; leave runtime reaction settings intact."""
import unreal, math, json
from pathlib import Path
root = '/Game/Main/Character/Hero/Anim/HitReactions'
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
source = unreal.load_asset(root + '/A_Hero_LightFeedback')
montage = unreal.load_asset(root + '/AM_Hero_LightFeedback')
assert source and montage
assert source.get_editor_property('RefPoseType') == unreal.AdditiveBasePoseType.ABPT_LOCAL_ANIM_FRAME
assert source.get_editor_property('RefFrameIndex') == 0
path = root + '/A_Hero_LightFeedback_PreviewBase'
base = unreal.load_asset(path)
if not base: base = unreal.EditorAssetLibrary.duplicate_asset(root + '/A_Hero_LightFeedback', path)
assert base
base.set_editor_property('AdditiveAnimType', unreal.AdditiveAnimationType.AAT_NONE)
names = unreal.AnimationLibrary.get_animation_track_names(source)
poses = {str(name): unreal.AnimationLibrary.get_bone_pose_for_frame(base, name, 0, False) for name in names}
controller = base.controller
controller.open_bracket('Create frame-zero static additive preview base')
try:
    controller.set_number_of_frames(unreal.FrameNumber(1))
    for name, pose in poses.items():
        assert controller.set_bone_track_keys(name, [pose.translation, pose.translation],
            [pose.rotation, pose.rotation], [pose.scale3d, pose.scale3d]), name
finally:
    controller.close_bracket()
montage.set_editor_property('PreviewBasePose', base)
for asset in [base, montage]: assert unreal.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=False)
maximum_angle = 0.0
maximum_translation = 0.0
for name, pose in poses.items():
    for t in [0.0, base.get_play_length()]:
        actual = unreal.AnimationLibrary.get_bone_pose_for_time(base, name, t, False)
        a = actual.rotation; b = pose.rotation
        dot = abs(a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w)
        maximum_angle = max(maximum_angle, math.degrees(2*math.acos(min(1.,dot))))
        v = actual.translation-pose.translation
        maximum_translation = max(maximum_translation, math.sqrt(v.x*v.x+v.y*v.y+v.z*v.z))
assert maximum_angle < .01 and maximum_translation < .001
report = {'montage': montage.get_path_name(), 'preview_base': base.get_path_name(), 'tracks': len(names),
    'max_static_rotation_error_degrees': maximum_angle, 'max_static_position_error_cm': maximum_translation,
    'runtime_additive_and_blend_unchanged': True}
out = Path(unreal.Paths.project_saved_dir())/'LightFeedbackInspection'
out.mkdir(parents=True,exist_ok=True)
(out/'preview-fix.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
unreal.get_editor_subsystem(unreal.AssetEditorSubsystem).close_all_editors_for_asset(montage)
unreal.get_editor_subsystem(unreal.AssetEditorSubsystem).open_editor_for_assets([montage])
print(json.dumps(report))
