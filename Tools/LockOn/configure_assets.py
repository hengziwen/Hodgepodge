"""安装正式玩家锁定配置；修改前备份，仅增补自己拥有的条目。"""
from pathlib import Path
import json, shutil
import unreal
ROOT=Path('E:/Project/Git/Hodgepodge');WORK=ROOT/'Saved/LockOn'
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
assert all(p.get_path_name().startswith('/Game/Main/Character/Hero/Targeting/') or (WORK/'BeforeAssets'/(p.get_path_name().removeprefix('/Game/')+'.uasset')).exists() or p.get_path_name()=='/Game/Main/Input/InputAction/IA_LockOn' for p in unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages())
tools=unreal.AssetToolsHelpers.get_asset_tools()
pawn=unreal.load_asset('/Game/Main/Data/PawnData/DA_Dafult_PawnData')
assert pawn
input_config=pawn.get_editor_property('InputConfig')
contexts=[unreal.load_asset(path) for path in unreal.EditorAssetLibrary.list_assets('/Game/Main/Input',True,False) if unreal.load_asset(path) and isinstance(unreal.load_asset(path),unreal.InputMappingContext)]
assert contexts
# 正式 Hero 的默认 IMC 通过已有移动/攻击条目识别，不给菜单或实验场景增加按键。
contexts=[c for c in contexts if any(str(m.get_editor_property('key')) and m.get_editor_property('action') and m.get_editor_property('action').get_name()=='IA_Attack' for m in c.get_editor_property('mappings'))]
assert len(contexts)==1,[c.get_path_name() for c in contexts]
imc=contexts[0]
hud=unreal.load_asset('/Game/Main/UI/HUD/WBP_HodgeHUD')
changed=[pawn,input_config,imc,hud]
for a in changed:
 path=a.get_path_name().split('.')[0]; source=ROOT/'Content'/(path.removeprefix('/Game/')+'.uasset')
 backup=WORK/'BeforeAssets'/source.relative_to(ROOT/'Content')
 if not backup.exists():backup.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(source,backup)

def data(path,cls):
 value=unreal.load_asset(path)
 if not value:
  parent,name=path.rsplit('/',1);f=unreal.DataAssetFactory();f.set_editor_property('data_asset_class',cls)
  value=tools.create_asset(name,parent,cls,f)
 assert value
 return value
profile=data('/Game/Main/Character/Hero/Targeting/DA_HeroLockOn',unreal.HodgeLockOnProfile)
ability_set=data('/Game/Main/Character/Hero/Targeting/AS_HeroLockOn',unreal.HodgeAbilitySet)
tag=unreal.GameplayTag();assert tag.import_text('(TagName="InputTag.Ability.LockOn")')
entry=unreal.HodgeAbilitySet_GameplayAbility();assert entry.import_text('(Ability="/Script/CoreUObject.Class\'/Script/Hodgepodge.HodgeGameplayAbility_LockOn\'",AbilityLevel=1,InputTag=(TagName="InputTag.Ability.LockOn"))')
ability_set.set_editor_property('GrantedGameplayAbilities',[entry])
sets=list(pawn.get_editor_property('AbilitySets'))
if ability_set not in sets:sets.append(ability_set)
pawn.set_editor_property('AbilitySets',sets);pawn.set_editor_property('LockOnProfile',profile)
path='/Game/Main/Input/InputAction/IA_LockOn';action=unreal.load_asset(path)
if not action:action=tools.create_asset('IA_LockOn','/Game/Main/Input/InputAction',unreal.InputAction,unreal.InputAction_Factory())
action.set_editor_property('value_type',unreal.InputActionValueType.BOOLEAN)
rows=list(input_config.get_editor_property('AbilityInputActions'))
assert sum(str(r.get_editor_property('InputTag').get_editor_property('TagName'))=='InputTag.Ability.LockOn' for r in rows)<=1
if not any(r.get_editor_property('InputAction')==action for r in rows):
 row=unreal.HodgeInputAction();assert row.import_text('(InputAction="'+action.get_path_name()+'",InputTag=(TagName="InputTag.Ability.LockOn"))');rows.append(row)
input_config.set_editor_property('AbilityInputActions',rows)
existing=[m for m in imc.get_editor_property('mappings') if m.get_editor_property('action')==action]
if existing and any(m.get_editor_property('Key').export_text()!='MiddleMouseButton' for m in existing):
 imc.unmap_all_keys_from_action(action);existing=[]
if not existing:
 key=unreal.Key();assert key.import_text('MiddleMouseButton');imc.map_key(action,key)
assert unreal.HodgeUIAuthoringLibrary.configure_lock_on_hud(hud)
for a in changed+[profile,ability_set,action]:assert unreal.EditorAssetLibrary.save_loaded_asset(a,False),a.get_path_name()
# 正式蓝图及动画继承新原生布局，编译检查但不重存无关资源。
compiled=[]
for path in ['/Game/Main/Character/Hero/BP_Pover','/Game/Main/Character/Enemies/Guardian/BP_Enemy_GuardianEquipped','/Game/Main/Character/Enemies/Guardian/BP_Enemy_Guardian','/Game/Main/Character/Hero/Anim/ABP_Pover_Base','/Game/Main/Character/Hero/Anim/Layer/ABP_ItemAnimLayers_Pover_Base']:
 bp=unreal.load_asset(path)
 if bp:unreal.BlueprintEditorLibrary.compile_blueprint(bp);compiled.append(path)
result={'assets':[a.get_path_name() for a in changed+[profile,ability_set,action]],'compiled':compiled,'profile':profile.get_path_name(),'input_config':input_config.get_path_name(),'mapping':imc.get_path_name()}
(WORK/'configuration.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps(result,ensure_ascii=False))
