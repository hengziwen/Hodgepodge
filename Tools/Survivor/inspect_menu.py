import unreal
for o in unreal.ObjectIterator(unreal.EdGraph):
 if '/Game/CodexText/WBP_MainMenu.' in o.get_path_name():print(o.get_path_name())
