"""Open a real source wing animation for native visual preview."""
import unreal
asset=unreal.load_asset('/Game/Wuwa/Wing/BaseAnim/G_Attack05')
assert asset
unreal.get_editor_subsystem(unreal.AssetEditorSubsystem).open_editor_for_assets([asset])
print('Opened imported G_Attack05 with its own wing preview mesh.')
