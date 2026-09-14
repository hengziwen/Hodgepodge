import unreal
for o in unreal.ObjectIterator(unreal.Button):
 if '/Game/CodexText/WBP_MainMenu.' in o.get_path_name() and 'RogueliteButton' in o.get_path_name():
  print(o.get_path_name())
  o.set_background_color(unreal.LinearColor(.08,.4,.55,1))
unreal.BlueprintEditorLibrary.compile_blueprint(unreal.load_asset('/Game/CodexText/WBP_MainMenu'))
unreal.EditorAssetLibrary.save_asset('/Game/CodexText/WBP_MainMenu',False)
