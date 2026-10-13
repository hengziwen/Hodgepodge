"""Import equipment without renaming sources, then rebuild exact PMX material references."""
import unreal,json
from pathlib import Path
ROOT='/Game/Wuwa/PoverEquipment'
DISK=Path('E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment')
REPORT=Path('E:/Project/Git/Hodgepodge/Saved/PoverEquipmentImport')
models=json.loads((REPORT/'conversion.json').read_text(encoding='utf-8'))
lib=unreal.MaterialEditingLibrary
tools=unreal.AssetToolsHelpers.get_asset_tools()
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
dirty=unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
assert all(p.get_path_name().startswith(ROOT+'/') for p in dirty),[p.get_path_name() for p in dirty]
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
textures={};imported=[];repaired=[]
def package(relative):
 p=Path(relative);return ROOT+'/'+p.with_suffix('').as_posix()
def task(filename,path,options=None):
 t=unreal.AssetImportTask();t.filename=str(filename);t.destination_path=path;t.destination_name=filename.stem;t.automated=True;t.replace_existing=False;t.save=True
 if options:t.options=options;t.factory=unreal.FbxFactory()
 return t
tex_tasks=[]
for f in sorted(DISK.rglob('*')):
 if f.suffix.lower() not in ['.png','.jpg','.jpeg']:continue
 relative=f.relative_to(DISK).as_posix();path=package(relative)
 existing=unreal.load_asset(path)
 if not existing:tex_tasks.append(task(f,ROOT+'/'+f.relative_to(DISK).parent.as_posix()))
 textures[relative]=path
if tex_tasks:tools.import_asset_tasks(tex_tasks)
for relative,path in textures.items():
 texture=unreal.load_asset(path);assert isinstance(texture,unreal.Texture2D),path
 packed=relative.startswith('Tex_0/') and (Path(relative).stem.endswith('_N') or Path(relative).stem.endswith('_ID'))
 texture.set_editor_property('srgb',not packed)
 texture.set_editor_property('compression_settings',unreal.TextureCompressionSettings.TC_MASKS if packed else unreal.TextureCompressionSettings.TC_DEFAULT)
 assert unreal.EditorAssetLibrary.save_loaded_asset(texture)
 imported.append({'path':texture.get_path_name(),'type':'Texture2D','source':relative})
for model in models:
 path=ROOT+'/'+model['mesh_name']
 mesh=unreal.load_asset(path)
 if not mesh:
  options=unreal.FbxImportUI();options.set_editor_property('automated_import_should_detect_type',False)
  options.set_editor_property('mesh_type_to_import',unreal.FBXImportType.FBXIT_SKELETAL_MESH)
  options.set_editor_property('import_as_skeletal',True);options.set_editor_property('import_mesh',True)
  options.set_editor_property('import_materials',False);options.set_editor_property('import_textures',False);options.set_editor_property('import_animations',False);options.set_editor_property('create_physics_asset',False)
  data=options.get_editor_property('skeletal_mesh_import_data');data.set_editor_property('import_morph_targets',True);data.set_editor_property('normal_import_method',unreal.FBXNormalImportMethod.FBXNIM_IMPORT_NORMALS)
  data.set_editor_property('convert_scene',True);data.set_editor_property('convert_scene_unit',True)
  tools.import_asset_tasks([task(Path(model['fbx']),ROOT,options)])
  mesh=unreal.load_asset(path)
 assert isinstance(mesh,unreal.SkeletalMesh),path
 imported.append({'path':mesh.get_path_name(),'type':'SkeletalMesh','source':model['source']})
 slots=list(mesh.get_editor_property('materials'))
 assert len(slots)==len(model['materials']),(path,len(slots),len(model['materials']))
 for index,source_mat in enumerate(model['materials']):
  name=source_mat['name'];material_path=ROOT+'/Materials/'+name
  material=unreal.load_asset(material_path)
  if not material:material=tools.create_asset(name,ROOT+'/Materials',unreal.Material,unreal.MaterialFactoryNew())
  assert material,material_path
  lib.delete_all_material_expressions(material)
  material.set_editor_property('two_sided',source_mat['two_sided'])
  material.set_editor_property('used_with_skeletal_mesh',True)
  material.set_editor_property('shading_model',unreal.MaterialShadingModel.MSM_UNLIT)
  has_alpha=source_mat['base_texture'] and Path(source_mat['base_texture']).stem.endswith('A')
  material.set_editor_property('blend_mode',unreal.BlendMode.BLEND_TRANSLUCENT if has_alpha else unreal.BlendMode.BLEND_OPAQUE)
  refs=[]
  def node(cls,**props):
   n=lib.create_material_expression(material,cls,-500,len(refs)*150)
   for key,value in props.items():n.set_editor_property(key,value)
   return n
  def link(a,out,b,inp):
   if inp=='Input':inp=''
   assert lib.connect_material_expressions(a,out,b,inp),(b.get_class().get_name(),inp,lib.get_material_expression_input_names(b))
  def sample(role,relative,uv=None):
   assert relative in textures,(material_path,role,relative)
   tex=unreal.load_asset(textures[relative]);n=node(unreal.MaterialExpressionTextureSampleParameter2D,parameter_name=role,texture=tex,sampler_type=unreal.MaterialSamplerType.SAMPLERTYPE_COLOR)
   if uv:link(uv,'',n,'UVs')
   refs.append({'role':role,'original_reference':relative,'resolved_texture':tex.get_path_name()});return n
  if source_mat['base_texture']:
   base=sample('BaseTexture',source_mat['base_texture']);color=base;out='RGB'
  else:color=node(unreal.MaterialExpressionConstant3Vector,constant=unreal.LinearColor(*source_mat['diffuse'][:3],1));out=''
  if source_mat['sphere_texture'] and source_mat['sphere_mode'] in [1,2]:
   normal=node(unreal.MaterialExpressionVertexNormalWS)
   view=node(unreal.MaterialExpressionTransform,transform_source_type=unreal.MaterialVectorCoordTransformSource.TRANSFORMSOURCE_WORLD,transform_type=unreal.MaterialVectorCoordTransform.TRANSFORM_VIEW);link(normal,'',view,'Input')
   mask=node(unreal.MaterialExpressionComponentMask,r=True,g=True);link(view,'',mask,'Input')
   scale=node(unreal.MaterialExpressionMultiply);factor=node(unreal.MaterialExpressionConstant2Vector,r=.5,g=-.5);link(mask,'',scale,'A');link(factor,'',scale,'B')
   uv=node(unreal.MaterialExpressionAdd,const_b=.5);link(scale,'',uv,'A')
   sphere=sample('SphereTexture',source_mat['sphere_texture'],uv)
   blend=node(unreal.MaterialExpressionMultiply if source_mat['sphere_mode']==1 else unreal.MaterialExpressionAdd);link(color,out,blend,'A');link(sphere,'RGB',blend,'B');color=blend;out=''
  if source_mat['toon_texture']:
   uv=node(unreal.MaterialExpressionConstant2Vector,r=.5,g=.5);toon=sample('ToonTexture',source_mat['toon_texture'],uv)
   blend=node(unreal.MaterialExpressionMultiply);link(color,out,blend,'A');link(toon,'RGB',blend,'B');color=blend;out=''
  assert lib.connect_material_property(color,out,unreal.MaterialProperty.MP_EMISSIVE_COLOR)
  if has_alpha:assert lib.connect_material_property(base,'A',unreal.MaterialProperty.MP_OPACITY)
  lib.layout_material_expressions(material);lib.recompile_material(material)
  for ref in refs:assert lib.get_material_default_texture_parameter_value(material,ref['role']).get_path_name()==ref['resolved_texture']
  stats=lib.get_statistics(material)
  assert unreal.EditorAssetLibrary.save_loaded_asset(material)
  old=slots[index].material_interface
  slots[index].material_interface=material
  repaired.append({'mesh':mesh.get_path_name(),'slot':str(slots[index].material_slot_name),'source_material':name,'material':material.get_path_name(),'previous_material':old.get_path_name() if old else None,'references':refs,'transparent':has_alpha,'two_sided':source_mat['two_sided'],'statistics':str(stats)})
  imported.append({'path':material.get_path_name(),'type':'Material','source':model['source']+'::'+name})
 mesh.set_editor_property('materials',slots);assert unreal.EditorAssetLibrary.save_loaded_asset(mesh)
 skeleton=mesh.get_editor_property('skeleton');assert skeleton
 imported.append({'path':skeleton.get_path_name(),'type':'Skeleton','source':model['source']})
 assert len(unreal.HodgeAnimationAuthoringLibrary.get_skeleton_bone_names(skeleton))>=model['source_bones']
 result={'imported':imported,'repairs':repaired,'textures':textures}
 (REPORT/'import-results.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps({'imported':len(imported),'materials_repaired':len(repaired),'texture_links':sum(len(r['references']) for r in repaired)},ensure_ascii=False))
