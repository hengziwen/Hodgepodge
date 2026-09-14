import unreal
for p in ['/Game/CodexText/Survivor/BP_SurvivorMode','/Game/CodexText/Survivor/WBP_SurvivorHUD']:
 bp=unreal.load_asset(p);cls=bp.generated_class();cdo=unreal.get_default_object(cls)
 print(p,cls,cdo)
 if 'Mode' in p:
  for n in ['interface_class','enemy_material','default_pawn_class']:print(n,cdo.get_editor_property(n))
 else: print('NATIVEHUD',isinstance(cdo,unreal.HodgeSurvivorHUD))
