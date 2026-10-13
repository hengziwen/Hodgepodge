"""只读导出当前正式首招配置，不创建通知或保存资产。"""
import json
from pathlib import Path
import unreal

work = Path(unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_saved_dir())) / 'AttackWarp'
work.mkdir(parents=True, exist_ok=True)
definition = unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_1')
montage = definition.get_editor_property('ExecutionConfig').get_editor_property('Montage')
config = definition.get_editor_property('FacingWarp')
hero = unreal.get_default_object(unreal.EditorAssetLibrary.load_blueprint_class('/Game/Main/Character/Hero/BP_Hero_Pover'))
result = {
    'definition': definition.get_path_name(),
    'montage': montage.get_path_name(),
    'warp_component': bool(hero.get_component_by_class(unreal.HodgeAttackMotionWarpingComponent)),
    'config': {name: config.get_editor_property(name) for name in (
        'bEnabled', 'MaxCorrectionAngle', 'MaxRotationRate', 'RotationTimeMultiplier',
        'TargetDataTimeout', 'StartYawTolerance', 'TargetYawTolerance')},
    'other_attacks_enabled': [unreal.load_asset('/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_' + str(i)).get_editor_property('FacingWarp').get_editor_property('bEnabled') for i in range(2, 6)],
}
result['target_name'] = str(config.get_editor_property('WarpTargetName'))
# UE Python 不公开原生 FAnimNotifyEvent 时间接口；窗口由原生校验及制作记录验证。
result['window_source'] = 'native WindowContract test and ConfigureAttackFacingWarp(0, 0.1)'
(work / 'configuration-final.json').write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps(result, ensure_ascii=False))
