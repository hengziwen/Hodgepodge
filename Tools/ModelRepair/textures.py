import unreal
p='/Game/CodexText/Model/'
e=unreal.EditorAssetLibrary
for path in e.list_assets(p,recursive=True,include_folder=False):
 a=e.load_asset(path)
 print(a.get_name(),a.get_class().get_name())
 if isinstance(a,unreal.Texture2D) and (a.get_name().endswith('_D') or a.get_name().endswith('_N')):
  t=unreal.AssetExportTask();t.object=a;t.filename='D:/CodexUE55Lab/ModelRepairBackup/'+a.get_name()+'.tga';t.automated=True;t.prompt=False;t.replace_identical=True
  print('EXPORT',unreal.Exporter.run_asset_export_task(t))
