"""Verify saved import contracts, pose evaluation, and shader compilation."""
import hashlib,json,math,os
from pathlib import Path
import unreal
ROOT='/Game/Wuwa/Wing'
PROJECT=Path('E:/Project/Git/Hodgepodge')
REPORT=PROJECT/'Saved/WingImport'
RESULTS=json.loads((REPORT/'import-results.json').read_text(encoding='utf-8'))
SOURCES=json.loads((REPORT/'sources.json').read_text(encoding='utf-8'))
source_index={r['source']:r for r in SOURCES}
channel_index={r['source']:r['channels'] for r in json.loads((REPORT/'source-channels.json').read_text(encoding='utf-8'))}
checks=[];animations=[];mesh_results=[]
def check(condition,label,detail=None):
    checks.append({'check':label,'passed':bool(condition),'detail':detail})
def source_bones(relative):
    return {m['name'].split('::')[0] for m in source_index[relative]['models'] if m['type']=='LimbNode'}
for row in RESULTS['imported']+RESULTS['materials']:
    asset=unreal.load_asset(row['path'])
    check(asset and asset.get_class().get_name()==row['type'],'Load saved asset',row['path'])
    if not asset:continue
    if row['type']=='SkeletalMesh':
        mesh=asset;names={str(n) for n in unreal.HodgeAnimationAuthoringLibrary.get_skeleton_bone_names(mesh.skeleton)}
        check(names==source_bones(row['source']),'Preserve complete source bone names',row['path'])
        check(len(mesh.materials)==len(source_index[row['source']]['materials']),'Preserve material slot count',row['path'])
        expected=[m['name'].split('::')[0] for m in source_index[row['source']]['materials']]
        check([str(m.material_slot_name) for m in mesh.materials]==expected,'Preserve material slot order',row['path'])
        data=mesh.get_editor_property('asset_import_data')
        check(abs(data.import_uniform_scale-.01)<1e-7 and data.convert_scene_unit,'Mesh unit conversion',row['path'])
        bounds=mesh.get_bounds();size=[getattr(bounds.box_extent,a)*2 for a in ['x','y','z']]
        check(max(size)>100 and max(size)<400,'Centimeter-scale model dimensions',size)
        mesh_results.append({'mesh':row['path'],'size_cm':size,'bones':len(names)})
    elif row['type']=='AnimSequence':
        weapon=row['source'].startswith('MB1')
        mesh=unreal.load_asset(ROOT+('/MB1ZhenwuguanzheMd601_Weapon1/Model/MB1ZhenwuguanzheMd601_Weapon1' if weapon else '/EDarkNanzhuWingMd10011/Model/EDarkNanzhuWingMd10011'))
        check(asset.get_editor_property('skeleton')==mesh.skeleton,'Animation skeleton identity',row['path'])
        data=asset.get_editor_property('asset_import_data')
        check(abs(data.import_uniform_scale-.01)<1e-7 and data.convert_scene_unit,'Animation unit conversion',row['path'])
        duration=asset.get_play_length()
        expected=source_index[row['source']]['stacks'][0]['properties']['LocalStop'][0]/46186158000.0
        check(abs(duration-expected)<.034 and duration>0,'Animation duration preserved',{'path':row['path'],'seconds':duration,'source_seconds':expected})
        check(not asset.get_editor_property('enable_root_motion'),'No accidental actor root motion',row['path'])
        options=unreal.AnimPoseEvaluationOptions()
        options.optional_skeletal_mesh=mesh
        options.should_retarget=False
        poses=[unreal.AnimPoseExtensions.get_anim_pose_at_time(asset,duration*f,options) for f in [0,.25,.5,.75,1]]
        names=[str(n) for n in unreal.AnimPoseExtensions.get_bone_names(poses[0])]
        check(set(names)==source_bones(row['source']),'All source bones evaluate',row['path'])
        changed=[];valid=True
        for name in names:
            values=[unreal.AnimPoseExtensions.get_bone_pose(pose,name,unreal.AnimPoseSpaces.WORLD).export_text() for pose in poses]
            if len(set(values))>1:changed.append(name)
            for pose in poses:
                transform=unreal.AnimPoseExtensions.get_bone_pose(pose,name,unreal.AnimPoseSpaces.WORLD)
                valid=valid and all(math.isfinite(getattr(transform.translation,a)) and math.isfinite(getattr(transform.scale3d,a)) for a in ['x','y','z'])
        check(valid,'Finite evaluated bone transforms',row['path'])
        authored_motion=any(c['max']-c['min']>.001 for c in channel_index[row['source']])
        check(bool(changed) if authored_motion else not changed,'Match source motion or authored held pose',{'path':row['path'],'authored_motion':authored_motion,'moving_bones':len(changed)})
        anchor='Root' if weapon else 'Bone_Wing000'
        pose_t=unreal.AnimPoseExtensions.get_bone_pose(poses[0],anchor,unreal.AnimPoseSpaces.WORLD)
        ref_t=unreal.AnimPoseExtensions.get_ref_bone_pose(poses[0],anchor,unreal.AnimPoseSpaces.WORLD)
        animations.append({'path':row['path'],'seconds':duration,'moving_bones':len(changed),'authored_motion':authored_motion,'anchor_world':str(pose_t.translation),'reference_anchor_world':str(ref_t.translation)})
    elif row['type']=='Texture2D':
        packed=row['source'].endswith('_N.png')
        check(asset.srgb==(not packed),'Texture color space',row['path'])
    elif row['type']=='Material':
        stats=unreal.MaterialEditingLibrary.get_statistics(asset)
        check(stats.num_pixel_shader_instructions>0 and stats.num_vertex_shader_instructions>0,'Compiled material shaders',{'path':row['path'],'pixel':stats.num_pixel_shader_instructions,'vertex':stats.num_vertex_shader_instructions})

for repair in RESULTS['repairs']:
    mesh=unreal.load_asset(repair['mesh']);instance=unreal.load_asset(repair['material']);master=unreal.load_asset(repair['parent'])
    check(mesh.materials[repair['slot_index']].material_interface==instance,'Mesh material assignment',repair['mesh']+':'+repair['slot'])
    check(instance.parent==master,'Material instance parent',repair['material'])
    if repair['base_texture']:
        tex=unreal.MaterialEditingLibrary.get_material_instance_texture_parameter_value(instance,'BaseTexture')
        check(tex and tex.get_path_name()==repair['base_texture'],'Material texture reference',repair['material'])
    if repair['effect']:
        check(master.get_editor_property('blend_mode')==unreal.BlendMode.BLEND_TRANSLUCENT and master.get_editor_property('shading_model')==unreal.MaterialShadingModel.MSM_UNLIT and master.get_editor_property('two_sided'),'Effect transparency, emissive, and two-sided settings',repair['material'])
        opacity=unreal.MaterialEditingLibrary.get_material_instance_scalar_parameter_value(instance,'EffectOpacity')
        check(0<opacity<.5,'Controlled effect opacity',opacity)
for source in json.loads((REPORT/'source-files.json').read_text(encoding='utf-8')):
    for directory in [Path('D:/下载/BaiduwangpanDownloads/Anzhu'),PROJECT/'Content/Wuwa/Wing']:
        path=directory/source['relative']
        check(path.is_file() and hashlib.sha256(path.read_bytes()).hexdigest()==source['sha256'],'Original/copy integrity',str(path))
discovered=set(unreal.EditorAssetLibrary.list_assets(ROOT,True,False))
expected={r['path'] for r in RESULTS['imported']+RESULTS['materials']}
check(discovered==expected,'Exact asset inventory',{'expected':len(expected),'actual':len(discovered),'unexpected':sorted(discovered-expected)})
report={'passed':all(c['passed'] for c in checks),'checks':checks,'meshes':mesh_results,'animations':animations,'fresh_process':os.environ.get('HODGE_WING_FRESH')=='1'}
name='validation-fresh.json' if report['fresh_process'] else 'validation-editor.json'
(REPORT/name).write_text(json.dumps(report,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps({'passed':report['passed'],'checks':len(checks),'failed':[c for c in checks if not c['passed']],'meshes':mesh_results},ensure_ascii=False))
assert report['passed'],'See validation report.'
