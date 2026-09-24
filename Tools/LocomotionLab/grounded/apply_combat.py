import unreal
E=unreal.EditorAssetLibrary
bp=E.load_asset('/Game/CodexText/AnimInstance/ALS/ABP_ALS_Grounded')
a=E.load_asset('/Game/CodexText/Anim/Stand2')
has_layer=any(n.get_class().get_name()=='AnimGraphNode_LayeredBoneBlend' and n.get_path_name().startswith(bp.get_path_name()+':') for n in unreal.ObjectIterator(unreal.EdGraphNode))
if not has_layer:assert unreal.HodgeGroundedAuthoring.add_combat_layer(bp,a)
assert unreal.HodgeGroundedAuthoring.refine_grounded_transitions(bp)
cdo=unreal.get_default_object(bp.generated_class());cdo.set_editor_property('overlay_pose',a)
assert E.save_loaded_asset(bp,False)
print('COMBAT_LAYER_SAVED',cdo.get_editor_property('overlay_pose'))
