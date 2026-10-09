import unreal
root = '/Game/CodexText/HitReactionValidation'
asset = unreal.load_asset(root + '/AS_HeroPresentationProbe')
if not asset: asset = unreal.EditorAssetLibrary.duplicate_asset(root + '/AS_ReactionProbe', root + '/AS_HeroPresentationProbe')
assert asset
asset.set_editor_property('GrantedGameplayAbilities', [])
bp = unreal.load_asset(root + '/BP_HeroPresentationProbe')
if not bp:
    factory = unreal.BlueprintFactory(); factory.set_editor_property('ParentClass', unreal.HodgeEquipmentDefinition)
    bp = unreal.AssetToolsHelpers.get_asset_tools().create_asset('BP_HeroPresentationProbe', root, unreal.Blueprint, factory)
unreal.BlueprintEditorLibrary.compile_blueprint(bp)
unreal.get_default_object(bp.generated_class()).set_editor_property('AbilitySetsToGrant', [asset])
unreal.BlueprintEditorLibrary.compile_blueprint(bp)
for a in [asset, bp]: assert unreal.EditorAssetLibrary.save_loaded_asset(a, only_if_is_dirty=False)
print('Created probe definitions grant without a second reaction GA')
