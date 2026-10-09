import unreal
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
profile=unreal.load_asset('/Game/Main/Character/Hero/Anim/HitReactions/DA_Hero_HitReaction')
entry=next(e for e in profile.get_editor_property('Animations') if e.get_editor_property('Type')==unreal.HodgeImpactType.HIT_STUN)
montage=entry.get_editor_property('Montage')
duration=entry.get_editor_property('DefaultDuration')
assert unreal.HodgeAnimationAuthoringLibrary.configure_hero_hit_stun_duration(montage,duration)
assert abs(montage.get_play_length()-duration)<.001
assert unreal.EditorAssetLibrary.save_loaded_asset(montage,only_if_is_dirty=False)
print('HitStun montage segment duration '+str(montage.get_play_length())+', blend-out '+str(montage.get_default_blend_out_time()))
