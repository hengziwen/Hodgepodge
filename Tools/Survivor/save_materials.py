import unreal
for p in ['/Game/CodexText/Survivor/M_EliteAmber','/Game/CodexText/Survivor/M_HeroIvory']:
 print(p,unreal.EditorAssetLibrary.save_asset(p,False))
