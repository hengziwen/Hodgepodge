import unreal

ROOT='/Game/CodexText/Survivor'
EAL=unreal.EditorAssetLibrary
tools=unreal.AssetToolsHelpers.get_asset_tools()
assert not EAL.does_asset_exist(ROOT+'/L_Survivor'), 'Arena already exists; inspect before rebuilding'
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
EAL.make_directory(ROOT)

def material(name,color,glow=False):
    m=tools.create_asset(name,ROOT,unreal.Material,unreal.MaterialFactoryNew())
    c=unreal.MaterialEditingLibrary.create_material_expression(m,unreal.MaterialExpressionConstant3Vector,0,0)
    c.set_editor_property('constant',unreal.LinearColor(*color,1))
    unreal.MaterialEditingLibrary.connect_material_property(c,'',unreal.MaterialProperty.MP_BASE_COLOR)
    if glow:unreal.MaterialEditingLibrary.connect_material_property(c,'',unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    rough=unreal.MaterialEditingLibrary.create_material_expression(m,unreal.MaterialExpressionConstant,0,120)
    rough.set_editor_property('r',.7)
    unreal.MaterialEditingLibrary.connect_material_property(rough,'',unreal.MaterialProperty.MP_ROUGHNESS)
    unreal.MaterialEditingLibrary.recompile_material(m)
    EAL.save_loaded_asset(m,False)
    return m

floor=material('M_ArenaSlate',(.055,.085,.12))
tile=material('M_ArenaTile',(.075,.115,.15))
enemy=material('M_EnemyCrimson',(.55,.035,.085))
elite=material('M_EliteAmber',(1,.38,.035),True)
energy=material('M_EnergyCyan',(.03,.8,1.8),True)

def blueprint(name,parent):
    factory=unreal.BlueprintFactory();factory.set_editor_property('parent_class',parent)
    bp=tools.create_asset(name,ROOT,unreal.Blueprint,factory)
    assert bp
    return bp

hero=blueprint('BP_SurvivorHero',unreal.HodgeSurvivorHero)
unreal.BlueprintEditorLibrary.compile_blueprint(hero)
EAL.save_loaded_asset(hero,False)
widget=EAL.duplicate_asset('/Game/CodexText/WBP_SurvivalHUD',ROOT+'/WBP_SurvivorHUD')
unreal.BlueprintEditorLibrary.reparent_blueprint(widget,unreal.HodgeSurvivorHUD)
unreal.BlueprintEditorLibrary.compile_blueprint(widget)
EAL.save_loaded_asset(widget,False)
mode=blueprint('BP_SurvivorMode',unreal.HodgeSurvivorMode)
unreal.BlueprintEditorLibrary.compile_blueprint(mode)
cdo=unreal.get_default_object(mode.generated_class())
cdo.set_editor_property('default_pawn_class',hero.generated_class())
cdo.set_editor_property('interface_class',widget.generated_class())
cdo.set_editor_property('enemy_material',enemy)
cdo.set_editor_property('elite_material',elite)
cdo.set_editor_property('energy_material',energy)
unreal.BlueprintEditorLibrary.compile_blueprint(mode)
EAL.save_loaded_asset(mode,False)

world=unreal.EditorLoadingAndSavingUtils.new_blank_map(False)
actors=unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
cube=unreal.load_asset('/Engine/BasicShapes/Cube')
def block(label,p,scale,mat):
    a=actors.spawn_actor_from_class(unreal.StaticMeshActor,unreal.Vector(*p))
    a.set_actor_label(label)
    a.static_mesh_component.set_static_mesh(cube)
    a.static_mesh_component.set_material(0,mat)
    a.set_actor_scale3d(unreal.Vector(*scale))
    return a

for x in range(-5,6):
    for y in range(-5,6):
        block('ArenaTile_%d_%d'%(x,y),(x*500,y*500,-30),(4.98,4.98,.5),floor if (x+y)%2 else tile)
for sign in [-1,1]:
    block('BoundaryRail',(sign*2700,0,45),(.16,54,1),energy)
    block('BoundaryRail',(0,sign*2700,45),(54,.16,1),energy)
    block('CenterMark',(sign*160,0,-1),(.12,4,.08),energy)
    block('CenterMark',(0,sign*200,-1),(3.2,.12,.08),energy)
    for c in [-2200,-1100,0,1100,2200]:
        block('PerimeterPillar',(sign*2600,c,90),(.8,.8,1.8),tile)
        block('PillarLight',(sign*2600,c,184),(.82,.82,.08),energy)
light=actors.spawn_actor_from_class(unreal.DirectionalLight,unreal.Vector(0,0,1000),unreal.Rotator(-55,-25,0))
light.light_component.set_editor_property('intensity',4)
fill=actors.spawn_actor_from_class(unreal.DirectionalLight,unreal.Vector(0,0,1200),unreal.Rotator(-35,150,0))
fill.light_component.set_editor_property('intensity',1.5)
fill.light_component.set_editor_property('light_color',unreal.Color(140,190,255,255))
start=actors.spawn_actor_from_class(unreal.PlayerStart,unreal.Vector(0,0,150))
start.set_actor_label('SurvivorPlayerStart')
world.get_world_settings().set_editor_property('default_game_mode',mode.generated_class())
assert unreal.EditorLoadingAndSavingUtils.save_map(world,ROOT+'/L_Survivor')
print('CREATED',ROOT+'/L_Survivor',hero.get_path_name(),widget.get_path_name(),mode.get_path_name())
