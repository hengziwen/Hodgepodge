"""只读检查硬直动画与控制时间，不再隐式裁剪或保存正式动画。"""
import unreal
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
profile=unreal.load_asset('/Game/Main/Character/Hero/Anim/HitReactions/DA_Hero_HitReaction')
entry=next(e for e in profile.get_editor_property('Animations') if e.get_editor_property('Type')==unreal.HodgeImpactType.HIT_STUN)
montage=entry.get_editor_property('Montage')
duration=entry.get_editor_property('DefaultDuration')
print('HitStun control duration '+str(duration)+', montage length '+str(montage.get_play_length())+', blend-out '+str(montage.get_default_blend_out_time())+'; read-only, no trimming or saving')
