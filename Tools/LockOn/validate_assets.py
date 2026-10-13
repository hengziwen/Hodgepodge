"""只读核查正式资产和继承字段，结果可追溯到 LOCK 需求。"""
import unreal,json
from pathlib import Path
checks=[]
def check(name,passed,detail=None):
 checks.append({'check':name,'passed':bool(passed),'detail':detail});assert passed,(name,detail)
pawn=unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData');profile=unreal.load_asset('/Game/Main/Character/Hero/Targeting/DA_HeroLockOn');ability=unreal.load_asset('/Game/Main/Character/Hero/Targeting/AS_HeroLockOn')
check('LOCK-03_PawnData引用正式Profile',pawn.get_editor_property('LockOnProfile')==profile)
check('LOCK-03_AbilitySet授予唯一',sum(a==ability for a in pawn.get_editor_property('AbilitySets'))==1)
rows=ability.get_editor_property('GrantedGameplayAbilities')
check('LOCK-03_原生命令GA等级与输入',len(rows)==1 and rows[0].get_editor_property('Ability')==unreal.HodgeGameplayAbility_LockOn.static_class() and rows[0].get_editor_property('AbilityLevel')==1 and str(rows[0].get_editor_property('InputTag').get_editor_property('TagName'))=='InputTag.Ability.LockOn')
action=unreal.load_asset('/Game/Main/Input/InputAction/IA_LockOn');mapping=unreal.load_asset('/Game/Main/Input/IMC_Default');inputs=pawn.get_editor_property('InputConfig')
check('LOCK-01_输入动作Boolean',action.get_editor_property('ValueType')==unreal.InputActionValueType.BOOLEAN)
rows=[r for r in mapping.get_editor_property('Mappings') if r.get_editor_property('Action')==action]
check('LOCK-01_唯一中键映射',len(rows)==1 and 'MiddleMouseButton' in rows[0].get_editor_property('Key').export_text())
rows=[r for r in inputs.get_editor_property('AbilityInputActions') if str(r.get_editor_property('InputTag').get_editor_property('TagName'))=='InputTag.Ability.LockOn']
check('LOCK-01_唯一输入标签入口',len(rows)==1 and rows[0].get_editor_property('InputAction')==action)
hud=unreal.load_asset('/Game/Main/UI/HUD/WBP_HodgeHUD');tree=unreal.find_object(hud,'WidgetTree');layer=unreal.find_object(tree,'LockOnIndicatorLayer')
check('LOCK-05_HUD投影层存在',bool(layer))
check('LOCK-05_标记类有效',str(profile.get_editor_property('IndicatorClass')).find('HodgeLockOnIndicatorWidget')>=0)
for name in ['BP_Enemy_Guardian','BP_Enemy_GuardianEquipped']:
 bp=unreal.load_asset('/Game/Main/Character/Enemies/Guardian/'+name);cdo=unreal.get_default_object(bp.generated_class())
 check('LOCK-01_胸口锚点_'+name,str(cdo.get_editor_property('LockOnSocket'))=='b_MF_Spine_03')
result={'passed':all(c['passed'] for c in checks),'checks':checks}
Path('E:/Project/Git/Hodgepodge/Saved/LockOn/asset-validation.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8');print(json.dumps(result,ensure_ascii=False))
