import unreal,json
from pathlib import Path
ROOT='/Game/Main/Character/Enemies/Guardian/Validation'
MAIN='/Game/Main/Character/Enemies/Guardian'
tools=unreal.AssetToolsHelpers.get_asset_tools()
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
written=[]
def tag(name):
 r=unreal.GameplayTag();r.import_text('(TagName="'+name+'")');return r
def dup(name,src):
 p=ROOT+'/'+name;a=unreal.load_asset(p)
 if not a:a=unreal.EditorAssetLibrary.duplicate_asset(src,p)
 assert a,p
 written.append(p);return a
def create(name,cls,factory):
 p=ROOT+'/'+name;a=unreal.load_asset(p)
 if not a:a=tools.create_asset(name,ROOT,cls,factory)
 assert a,p
 written.append(p);return a
attack=dup('DA_ComboAttack',MAIN+'/DA_Guardian_Attack')
attack.set_editor_property('ExecutionRoute',unreal.HodgeAbilityExecutionRoute.COMBO_COORDINATED)
combo=dup('DA_EnemyCombo','/Game/Main/Data/Combo/DA_LightCombo')
intent=combo.get_editor_property('InputBindings')[0].get_editor_property('IntentTag')
factory=unreal.DataTableFactory();factory.set_editor_property('Struct',unreal.load_object(None,'/Script/Hodgepodge.HodgeComboRow'))
table=create('DT_EnemyCombo',unreal.DataTable,factory)
rows=[{'Name':'Combo.Entry','ComboTag':'Combo.Entry','Transitions':[{'TriggerInputIntentTag':str(intent.get_editor_property('TagName')),'TargetComboTag':'Combo.Light.01'}]},
      {'Name':'Combo.Light.01','ComboTag':'Combo.Light.01','AbilityTag':'Ability.Type.Action.Melee','Transitions':[]}]
assert unreal.DataTableFunctionLibrary.fill_data_table_from_json_string(table,json.dumps(rows)),rows
combo.set_editor_property('ComboTable',table);combo.set_editor_property('EntryComboTag',tag('Combo.Entry'));combo.set_editor_property('InputBindings',[])
aset=dup('AS_EnemyCombo',MAIN+'/AS_Guardian_Basic')
entry=unreal.HodgeAbilitySet_Definition();entry.import_text('(Definition="/Script/Hodgepodge.HodgeAbilityDefinition\''+attack.get_path_name()+'\'",AbilityLevel=1)')
aset.set_editor_property('GrantedAbilityDefinitions',[entry])
bb=create('BB_EnemyCombo',unreal.BlackboardData,unreal.BlackboardDataFactory())
bt=create('BT_EnemyCombo',unreal.BehaviorTree,unreal.BehaviorTreeFactory())
assert unreal.HodgeAnimationAuthoringLibrary.configure_enemy_behavior_tree(bt,bb,intent,True)
aic=dup('AIC_EnemyCombo',MAIN+'/AIC_Guardian');unreal.BlueprintEditorLibrary.compile_blueprint(aic)
unreal.get_default_object(aic.generated_class()).set_editor_property('BehaviorTree',bt)
data=dup('DA_EnemyComboPawnData',MAIN+'/DA_Guardian_PawnData');data.set_editor_property('ComboDefinition',combo);data.set_editor_property('AbilitySets',[aset])
bp=dup('BP_EnemyCombo',MAIN+'/BP_Enemy_Guardian');unreal.BlueprintEditorLibrary.compile_blueprint(bp)
cdo=unreal.get_default_object(bp.generated_class());cdo.set_editor_property('EnemyPawnData',data);cdo.set_editor_property('AIControllerClass',aic.generated_class())
data.set_editor_property('PawnClass',bp.generated_class())
for obj in [aic,bp]:
 result=unreal.HodgeAnimationAuthoringLibrary.compile_animation_blueprint(obj);assert result.startswith('ERRORS=0'),result
for p in sorted(set(written)):assert unreal.EditorAssetLibrary.save_asset(p,only_if_is_dirty=False)
summary={'assets':sorted(set(written)),'intent':intent.export_text(),'combo_table':unreal.DataTableFunctionLibrary.export_data_table_to_json_string(table)}
Path('E:/Project/Git/Hodgepodge/Saved/CombatActionRequests/assets.json').write_text(json.dumps(summary,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps(summary,ensure_ascii=False))
