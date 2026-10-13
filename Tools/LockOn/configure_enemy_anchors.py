import unreal,json,shutil
from pathlib import Path
root=Path('E:/Project/Git/Hodgepodge');work=root/'Saved/LockOn'
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
rows=[]
for name in ['BP_Enemy_Guardian','BP_Enemy_GuardianEquipped']:
 path='/Game/Main/Character/Enemies/Guardian/'+name
 bp=unreal.load_asset(path);cdo=unreal.get_default_object(bp.generated_class());mesh=cdo.mesh.get_skeletal_mesh_asset();skeleton=mesh.get_editor_property('Skeleton')
 bones=[str(b) for b in unreal.HodgeAnimationAuthoringLibrary.get_skeleton_bone_names(skeleton)]
 priorities=['b_MF_Spine_03','spine_03','spine_02','spine_01','spine','chest','Bip001Spine2']
 chosen=next((b for wanted in priorities for b in bones if b.lower()==wanted.lower()),None)
 assert chosen,(path,bones)
 existing=str(cdo.get_editor_property('LockOnSocket'))
 if existing in ['None','']:
  source=root/'Content/Main/Character/Enemies/Guardian'/(name+'.uasset');backup=work/'BeforeAssets'/source.relative_to(root/'Content')
  if not backup.exists():backup.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(source,backup)
  cdo.set_editor_property('LockOnSocket',chosen);unreal.BlueprintEditorLibrary.compile_blueprint(bp);assert unreal.EditorAssetLibrary.save_loaded_asset(bp,False)
 rows.append({'asset':path,'anchor':str(cdo.get_editor_property('LockOnSocket')),'preserved_previous':existing not in ['None','']})
(work/'enemy-anchors.json').write_text(json.dumps(rows,ensure_ascii=False,indent=2),encoding='utf-8');print(json.dumps(rows))
