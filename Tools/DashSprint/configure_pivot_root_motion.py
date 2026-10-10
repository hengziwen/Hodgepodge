"""Author only Main's dedicated Pivot root-motion resources and profile binding."""
import unreal, json, math, statistics
from pathlib import Path

assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
tools = unreal.AssetToolsHelpers.get_asset_tools()
folder = '/Game/Main/Character/Hero/Anim/Movement'
path = folder + '/A_Hero_Sprint_Pivot_RootMotion'
sequence = unreal.load_asset(path)
if not sequence:
    sequence = tools.duplicate_asset('A_Hero_Sprint_Pivot_RootMotion', folder,
                                    unreal.load_asset(folder + '/A_Hero_Sprint_Turn'))
sequence.set_editor_property('enable_root_motion', True)
sequence.set_editor_property('force_root_lock', True)
sequence.set_editor_property('root_motion_root_lock', unreal.RootMotionRootLock.ANIM_FIRST_FRAME)
montage = unreal.load_asset(folder + '/AM_Hero_Sprint_Pivot')
if not montage:
    factory = unreal.AnimMontageFactory()
    factory.set_editor_property('target_skeleton', sequence.get_editor_property('skeleton'))
    factory.set_editor_property('source_animation', sequence)
    montage = tools.create_asset('AM_Hero_Sprint_Pivot', folder, unreal.AnimMontage, factory)
assert unreal.HodgeAnimationAuthoringLibrary.set_hero_reaction_montage_slot(montage, 'FullBody')
blend_in = unreal.AlphaBlend(); blend_in.set_editor_property('blend_time', .08)
blend_out = unreal.AlphaBlend(); blend_out.set_editor_property('blend_time', .12)
montage.set_editor_property('blend_in', blend_in); montage.set_editor_property('blend_out', blend_out)
profile = unreal.load_asset('/Game/Main/Character/Hero/Ability/Sprint/DA_Hero_SprintAbility')
profile.set_editor_property('SprintPivotMontage', montage)
cycle = profile.get_editor_property('SprintCycle')
first = unreal.AnimationLibrary.get_bone_pose_for_time(cycle, 'Root', 0., False).translation
last = unreal.AnimationLibrary.get_bone_pose_for_time(cycle, 'Root', cycle.get_play_length(), False).translation
reference_speed = ((last.x-first.x)**2 + (last.y-first.y)**2)**.5 / cycle.get_play_length()
reference_source = 'root trajectory'
if reference_speed <= 1.:
    # 原地素材从近地、垂直速度较小的脚部区间估计步幅速度。
    tracks = [[], []]
    for i in range(121):
        time = i * cycle.get_play_length() / 120.
        pose = unreal.AnimPoseExtensions.get_anim_pose_at_time(cycle, time, unreal.AnimPoseEvaluationOptions())
        for index, bone in enumerate(['Bip001LFoot', 'Bip001RFoot']):
            point = unreal.AnimPoseExtensions.get_bone_pose(pose, bone, unreal.AnimPoseSpaces.WORLD).translation
            tracks[index].append((time, point.x, point.y, point.z))
    estimates = []
    for track in tracks:
        minimum_z = min(row[3] for row in track)
        speeds = [math.dist(a[1:3], b[1:3]) / (b[0]-a[0]) for a, b in zip(track, track[1:])
                  if max(a[3], b[3]) < minimum_z + 3. and abs(b[3]-a[3]) / (b[0]-a[0]) < 30.]
        if len(speeds) >= 3:
            estimates.append(statistics.median(speeds))
    assert estimates, 'In-place loop needs a configured stride reference or measurable stance samples'
    reference_speed = statistics.mean(estimates)
    reference_source = 'in-place stance foot velocity estimate'
profile.set_editor_property('SprintCycleReferenceSpeed', reference_speed)
main = unreal.load_asset('/Game/Main/Character/Hero/Anim/ABP_Pover_Base')
assert unreal.HodgeAnimationAuthoringLibrary.set_animation_default(main, 'RootMotionMode', 'RootMotionFromMontagesOnly')
diagnostic = unreal.HodgeAnimationAuthoringLibrary.compile_animation_blueprint(main)
assert diagnostic.startswith('ERRORS=0'), diagnostic
for asset in [sequence, montage, profile, main]:
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset, False)
result = {'sequence': sequence.get_path_name(), 'montage': montage.get_path_name(), 'length': montage.get_play_length(), 'cycle_reference_speed': reference_speed, 'cycle_reference_source': reference_source, 'diagnostic': diagnostic}
Path(unreal.Paths.project_saved_dir()).joinpath('PivotFootSlide/configuration.json').write_text(json.dumps(result, indent=2), encoding='utf-8')
print(json.dumps(result))
