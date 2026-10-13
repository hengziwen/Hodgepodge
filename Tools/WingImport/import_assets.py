"""Import the wing, its variant, and bundled weapon with explicitly matched skeletons."""
import json
from pathlib import Path
import unreal

ROOT='/Game/Wuwa/Wing'
PROJECT=Path('E:/Project/Git/Hodgepodge')
DISK=PROJECT/'Content/Wuwa/Wing'
REPORT=PROJECT/'Saved/WingImport'
SCALE=0.01
TOOLS=unreal.AssetToolsHelpers.get_asset_tools()
SOURCES=json.loads((REPORT/'sources.json').read_text(encoding='utf-8'))
MODEL='EDarkNanzhuWingMd10011/Model/EDarkNanzhuWingMd10011.fbx'
WEAPON='MB1ZhenwuguanzheMd601_Weapon1/Model/MB1ZhenwuguanzheMd601_Weapon1.fbx'
VARIANT='EDarkNanzhuWingMd10011/Model/test/EDarkNanzhuWingMd10011.fbx'
assert not unreal.EditorLevelLibrary.get_pie_worlds(False)
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
assert all(p.get_path_name().startswith(ROOT+'/') for p in unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages())
IMPORTED=[]

def package(relative):
    return ROOT+'/'+Path(relative).with_suffix('').as_posix()

def import_task(relative,options=None):
    file=DISK/relative
    t=unreal.AssetImportTask()
    t.filename=str(file);t.destination_path=ROOT+'/'+Path(relative).parent.as_posix()
    t.destination_name=file.stem;t.automated=True;t.replace_existing=False;t.save=True
    if options:
        t.options=options;t.factory=unreal.FbxFactory()
    TOOLS.import_asset_tasks([t])
    asset=unreal.load_asset(package(relative))
    assert asset,(relative,list(t.imported_object_paths))
    return asset

def fbx_options(animation=False,skeleton=None):
    o=unreal.FbxImportUI()
    o.automated_import_should_detect_type=False
    o.mesh_type_to_import=unreal.FBXImportType.FBXIT_ANIMATION if animation else unreal.FBXImportType.FBXIT_SKELETAL_MESH
    o.import_as_skeletal=True;o.import_mesh=not animation
    o.import_materials=False;o.import_textures=False;o.import_animations=animation;o.create_physics_asset=False
    if skeleton:o.skeleton=skeleton
    d=o.anim_sequence_import_data if animation else o.skeletal_mesh_import_data
    d.import_uniform_scale=SCALE;d.convert_scene=True;d.convert_scene_unit=True
    if animation:
        d.set_editor_property('use_default_sample_rate',True)
        d.set_editor_property('animation_length',unreal.FBXAnimationLengthImportType.FBXALIT_EXPORTED_TIME)
        d.set_editor_property('remove_redundant_keys',False)
    else:
        d.normal_import_method=unreal.FBXNormalImportMethod.FBXNIM_IMPORT_NORMALS
        d.set_editor_property('update_skeleton_reference_pose',False)
        d.set_editor_property('use_t0_as_ref_pose',False)
    return o

def record(asset,relative):
    IMPORTED.append({'path':asset.get_path_name(),'type':asset.get_class().get_name(),'source':relative})
    (REPORT/'import-progress.json').write_text(json.dumps(IMPORTED,ensure_ascii=False,indent=2),encoding='utf-8')

def load_or_import(relative,options,cls):
    asset=unreal.load_asset(package(relative)) or import_task(relative,options)
    assert isinstance(asset,cls),(relative,asset.get_class().get_name())
    record(asset,relative)
    return asset

wing=load_or_import(MODEL,fbx_options(),unreal.SkeletalMesh)
wing_skeleton=wing.skeleton;assert wing_skeleton
record(wing_skeleton,MODEL)
weapon=load_or_import(WEAPON,fbx_options(),unreal.SkeletalMesh)
weapon_skeleton=weapon.skeleton;assert weapon_skeleton
record(weapon_skeleton,WEAPON)
variant=load_or_import(VARIANT,fbx_options(skeleton=wing_skeleton),unreal.SkeletalMesh)
assert variant.skeleton==wing_skeleton

for source in SOURCES:
    relative=source['source']
    if relative in [MODEL,WEAPON,VARIANT]:continue
    skeleton=weapon_skeleton if relative.startswith('MB1ZhenwuguanzheMd601_Weapon1/') else wing_skeleton
    asset=load_or_import(relative,fbx_options(True,skeleton),unreal.AnimSequence)
    assert asset.get_editor_property('skeleton')==skeleton and asset.get_play_length()>0,relative
    asset.set_editor_property('enable_root_motion',False)
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset,False)

for file in sorted(DISK.rglob('*.png')):
    relative=file.relative_to(DISK).as_posix()
    tex=unreal.load_asset(package(relative)) or import_task(relative)
    assert isinstance(tex,unreal.Texture2D)
    packed=file.stem.endswith('_N')
    tex.srgb=not packed
    tex.compression_settings=unreal.TextureCompressionSettings.TC_MASKS if packed else unreal.TextureCompressionSettings.TC_DEFAULT
    assert unreal.EditorAssetLibrary.save_loaded_asset(tex,False)
    record(tex,relative)

(REPORT/'import-results.json').write_text(json.dumps({'imported':IMPORTED,'scale':SCALE},ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps({'imported':len(IMPORTED),'wing_bones':len(unreal.HodgeAnimationAuthoringLibrary.get_skeleton_bone_names(wing_skeleton)),
 'wing_slots':[str(m.material_slot_name) for m in wing.materials],'weapon_bones':len(unreal.HodgeAnimationAuthoringLibrary.get_skeleton_bone_names(weapon_skeleton))},ensure_ascii=False))
