import unreal
asset=unreal.load_asset('/Game/Wuwa/Wing/EDarkNanzhuWingMd10011/Model/EDarkNanzhuWingMd10011')
assert asset
unreal.get_editor_subsystem(unreal.AssetEditorSubsystem).open_editor_for_assets([asset])
print('Opened wing mesh editor.')
