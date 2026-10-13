"""Convert PMX equipment in an isolated Blender runtime; never edit the source files."""
import sys,json,shutil,hashlib
from pathlib import Path
ROOT=Path('E:/Project/Git/Hodgepodge')
RUNTIME=ROOT/'Saved/PoverEquipmentImport/Runtime'
sys.path.insert(0,str(RUNTIME/'python'))
import bpy,addon_utils
sys.path.insert(0,str(RUNTIME/'blender_mmd_tools-main'))
import mmd_tools
mmd_tools.register()
from mmd_tools.core.pmx.importer import PMXImporter
from mmd_tools.core import pmx
addon_utils.enable('io_scene_fbx',default_set=False,persistent=False)
SOURCE=Path(r'D:\下载\EdgeDownloads\漂子装备_by_微波叮桃子_414004e836adf6e38d5275515289704d\PoverEquipment')
DEST=ROOT/'Content/Wuwa/PoverEquipment'
for p in SOURCE.rglob('*'):
 if not p.is_file():continue
 target=DEST/p.relative_to(SOURCE)
 if target.exists():assert hashlib.sha256(target.read_bytes()).digest()==hashlib.sha256(p.read_bytes()).digest(),str(target)
 else:target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(p,target)
results=[]
for source in sorted(SOURCE.glob('*.pmx')):
 bpy.ops.wm.read_factory_settings(use_empty=True)
 model=pmx.load(str(source))
 PMXImporter().execute(filepath=str(source),types={'MESH','ARMATURE','MORPHS'},scale=.08,clean_model=False,remove_doubles=False,rename_LR_bones=False,use_underscore=False,translator=None)
 meshes=[o for o in bpy.context.scene.objects if o.type=='MESH']
 arms=[o for o in bpy.context.scene.objects if o.type=='ARMATURE']
 assert len(arms)==1 and meshes,source.name
 # Preserve material identities and replace importer-only shader groups with FBX-readable texture nodes.
 material_info=[]
 for i,mat in enumerate(model.materials):
  material=bpy.data.materials.get(mat.name)
  assert material,mat.name
  material.use_nodes=True;material.node_tree.nodes.clear()
  principled=material.node_tree.nodes.new('ShaderNodeBsdfPrincipled');output=material.node_tree.nodes.new('ShaderNodeOutputMaterial')
  material.node_tree.links.new(principled.outputs['BSDF'],output.inputs['Surface'])
  if mat.texture>=0:
   path=Path(model.textures[mat.texture].path)
   assert path.is_file(),path
   node=material.node_tree.nodes.new('ShaderNodeTexImage');node.image=bpy.data.images.load(str(path),check_existing=True)
   material.node_tree.links.new(node.outputs['Color'],principled.inputs['Base Color'])
   material.node_tree.links.new(node.outputs['Alpha'],principled.inputs['Alpha'])
  material_info.append({'index':i,'name':mat.name,'base_texture':str(Path(model.textures[mat.texture].path).relative_to(SOURCE)).replace('\\','/') if mat.texture>=0 else None,
    'sphere_texture':str(Path(model.textures[mat.sphere_texture].path).relative_to(SOURCE)).replace('\\','/') if mat.sphere_texture>=0 else None,
    'sphere_mode':mat.sphere_texture_mode,'toon_texture':str(Path(model.textures[mat.toon_texture].path).relative_to(SOURCE)).replace('\\','/') if not mat.is_shared_toon_texture and mat.toon_texture>=0 else None,
    'diffuse':list(mat.diffuse),'two_sided':mat.is_double_sided,'indices':mat.vertex_count})
 bpy.ops.object.select_all(action='DESELECT')
 for obj in meshes+arms:obj.select_set(True)
 bpy.context.view_layer.objects.active=arms[0]
 meshes[0].name=source.stem;arms[0].name=source.stem+'_Armature'
 out=DEST/(source.stem+'.fbx')
 bpy.ops.export_scene.fbx(filepath=str(out),use_selection=True,object_types={'MESH','ARMATURE'},add_leaf_bones=False,use_armature_deform_only=False,bake_anim=False,
   use_mesh_modifiers=False,mesh_smooth_type='FACE',axis_forward='-Y',axis_up='Z',path_mode='ABSOLUTE',apply_unit_scale=True)
 results.append({'source':source.name,'mesh_name':source.stem,'fbx':str(out),'vertices':len(model.vertices),'triangles':len(model.faces),'source_bones':len(model.bones),'export_bones':len(arms[0].data.bones),
   'blender_dimensions':[list(o.dimensions) for o in meshes],'materials':material_info})
 print('CONVERTED',source.name,len(arms[0].data.bones),len(material_info),flush=True)
 (ROOT/'Saved/PoverEquipmentImport/conversion.json').write_text(json.dumps(results,ensure_ascii=False,indent=2),encoding='utf-8')
print('Converted all four models',flush=True)
