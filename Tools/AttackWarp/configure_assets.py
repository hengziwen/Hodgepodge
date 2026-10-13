"""只为首招配置旋转 Warp，备份在本轮开始时已保存。"""
import unreal,json
from pathlib import Path
work=Path('E:/Project/Git/Hodgepodge/Saved/AttackWarp')
data=unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_1')
assert unreal.HodgeAnimationAuthoringLibrary.configure_attack_facing_warp(data,0.,.1)=='OK'
montage=data.get_editor_property('ExecutionConfig').get_editor_property('Montage')
for p in ['/Game/Main/Character/Hero/BP_Hero_Pover','/Game/Main/Character/Hero/Ability/BasicAttack/GA_Attack_1']:
 unreal.BlueprintEditorLibrary.compile_blueprint(unreal.load_asset(p))
assert unreal.EditorAssetLibrary.save_loaded_asset(montage,False)
assert unreal.EditorAssetLibrary.save_loaded_asset(data,False)
hero=unreal.get_default_object(unreal.EditorAssetLibrary.load_blueprint_class('/Game/Main/Character/Hero/BP_Hero_Pover'))
config=data.get_editor_property('FacingWarp')
r={'definition':data.get_path_name(),'montage':montage.get_path_name(),'warp_component':bool(hero.get_component_by_class(unreal.HodgeAttackMotionWarpingComponent)),'enabled':config.get_editor_property('bEnabled'),'window':[0.,.1],'max_angle':config.get_editor_property('MaxCorrectionAngle'),'max_rate':config.get_editor_property('MaxRotationRate'),'other_attacks_enabled':[unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_'+str(i)).get_editor_property('FacingWarp').get_editor_property('bEnabled') for i in range(2,6)]}
(work/'configuration.json').write_text(json.dumps(r,indent=2),encoding='utf-8')
print(json.dumps(r))
