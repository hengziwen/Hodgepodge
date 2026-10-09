"""Create a native-enemy fixture with a Pawn-owned ASC, real mesh, AnimBP and combat component."""
import unreal
ROOT = '/Game/CodexText/HitReactionValidation'
tools = unreal.AssetToolsHelpers.get_asset_tools()
data = unreal.load_asset(ROOT + '/DA_EnemyPawnData')
if not data:
    factory = unreal.DataAssetFactory()
    factory.set_editor_property('DataAssetClass', unreal.HodgePawnData)
    data = tools.create_asset('DA_EnemyPawnData', ROOT, unreal.HodgePawnData, factory)
data.set_editor_property('StatProfile', unreal.load_asset('/Game/Main/Data/CharacterStats/DA_Pover_Stats'))
data.set_editor_property('HitReactionProfile', unreal.load_asset(ROOT + '/DA_ReactionProfile'))
data.set_editor_property('AbilitySets', [unreal.load_asset(ROOT + '/AS_ReactionProbe')])
bp = unreal.load_asset(ROOT + '/BP_EnemyProbe')
if not bp:
    factory = unreal.BlueprintFactory()
    factory.set_editor_property('ParentClass', unreal.HodgeEnemyCharacter)
    bp = tools.create_asset('BP_EnemyProbe', ROOT, unreal.Blueprint, factory)
subsystem = unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
handles = subsystem.k2_gather_subobject_data_for_blueprint(bp)
objects = [unreal.SubobjectDataBlueprintFunctionLibrary.get_object(unreal.SubobjectDataBlueprintFunctionLibrary.get_data(h)) for h in handles]
if not any(isinstance(obj, unreal.HodgeCombatComponentBase) for obj in objects):
    params = unreal.AddNewSubobjectParams(parent_handle=handles[0], new_class=unreal.HodgeCombatComponentBase, blueprint_context=bp)
    component, reason = subsystem.add_new_subobject(params)
    assert not str(reason), str(reason)
unreal.BlueprintEditorLibrary.compile_blueprint(bp)
cdo = unreal.get_default_object(bp.generated_class())
cdo.set_editor_property('EnemyPawnData', data)
cdo.mesh.set_skeletal_mesh_asset(unreal.load_asset('/Game/Main/Character/Hero/Anim/Model/SKM_Pover_LyraLab'))
cdo.mesh.set_anim_instance_class(unreal.load_asset('/Game/Main/Character/Hero/Anim/ABP_Pover_Base').generated_class())
cdo.mesh.set_relative_location(unreal.Vector(0, 0, -90), False, False)
cdo.mesh.set_relative_rotation(unreal.Rotator(0, -90, 0), False, False)
data.set_editor_property('PawnClass', bp.generated_class())
unreal.BlueprintEditorLibrary.compile_blueprint(bp)
if not unreal.EditorLevelLibrary.get_pie_worlds(False):
    for path in [ROOT + '/BP_EnemyProbe', ROOT + '/DA_EnemyPawnData']:
        assert unreal.EditorAssetLibrary.save_asset(path, only_if_is_dirty=False), path
print('Created native EnemyPawnData / BP_EnemyProbe fixtures')
