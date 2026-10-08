#include "Modules/ModuleManager.h"
#include "AssetToolsModule.h"
#include "AssetTypeActions_Base.h"
#include "Data/HodgeAbilityDefinition.h"
#include "Toolkits/SimpleAssetEditor.h"
class FHodgeAbilityDefinitionActions : public FAssetTypeActions_Base
{
public:
    virtual FText GetName() const override { return NSLOCTEXT("HodgeAbilityEditor", "AssetName", "Hodge Ability Definition"); }
    virtual FColor GetTypeColor() const override { return FColor(45,160,180); }
    virtual UClass* GetSupportedClass() const override { return UHodgeAbilityDefinition::StaticClass(); }
    virtual uint32 GetCategories() override { return EAssetTypeCategories::Gameplay; }
    virtual void OpenAssetEditor(const TArray<UObject*>& Objects, TSharedPtr<IToolkitHost> Host) override
    {
        for (UObject* Object : Objects)
        {
            if (auto* Definition = Cast<UHodgeAbilityDefinition>(Object))
            { FSimpleAssetEditor::CreateEditor(Host.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone, Host, Definition); }
        }
    }
};
class FHodgeAbilityEditorModule : public IModuleInterface
{
    TSharedPtr<IAssetTypeActions> Actions;
public:
    virtual void StartupModule() override
    {
        Actions = MakeShared<FHodgeAbilityDefinitionActions>();
        FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get().RegisterAssetTypeActions(Actions.ToSharedRef());
    }
    virtual void ShutdownModule() override
    {
        if (Actions && FModuleManager::Get().IsModuleLoaded("AssetTools"))
        { FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get().UnregisterAssetTypeActions(Actions.ToSharedRef()); }
        Actions.Reset();
    }
};
IMPLEMENT_MODULE(FHodgeAbilityEditorModule, HodgeAbilityEditor)
