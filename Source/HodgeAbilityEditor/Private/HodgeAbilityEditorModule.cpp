/** @file HodgeAbilityEditorModule.cpp
 * @brief 实现技能编辑器模块注册相关的类型、接口与现有处理流程。
 */
#include "Modules/ModuleManager.h"
#include "AssetToolsModule.h"
#include "AssetTypeActions_Base.h"
#include "Data/HodgeAbilityDefinition.h"
#include "Toolkits/SimpleAssetEditor.h"
/** 封装能力定义Actions的状态与操作接口。 */
class FHodgeAbilityDefinitionActions : public FAssetTypeActions_Base
{
public:
    /** 获取名称。
     * @return 名称的当前值或引用。
     */
    virtual FText GetName() const override { return NSLOCTEXT("HodgeAbilityEditor", "AssetName", "Hodge Ability Definition"); }
    /** 获取类型颜色。
     * @return 类型颜色的当前值或引用。
     */
    virtual FColor GetTypeColor() const override { return FColor(45,160,180); }
    /** 获取Supported类型。
     * @return Supported类型的当前值或引用。
     */
    virtual UClass* GetSupportedClass() const override { return UHodgeAbilityDefinition::StaticClass(); }
    /** 获取Categories。
     * @return Categories的当前值或引用。
     */
    virtual uint32 GetCategories() override { return EAssetTypeCategories::Gameplay; }
    /** 打开资产编辑器。
     * @param Objects 对象，由调用方传入。
     * @param Host Host，由调用方传入。
     */
    virtual void OpenAssetEditor(const TArray<UObject*>& Objects, TSharedPtr<IToolkitHost> Host) override
    {
        for (UObject* Object : Objects)
        {
            if (auto* Definition = Cast<UHodgeAbilityDefinition>(Object))
            { FSimpleAssetEditor::CreateEditor(Host.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone, Host, Definition); }
        }
    }
};
/** 封装技能编辑器模块注册的状态与操作接口。 */
class FHodgeAbilityEditorModule : public IModuleInterface
{
    // Actions的数据或对象引用。
    TSharedPtr<IAssetTypeActions> Actions;
public:
    /** 在模块加载时注册其编辑器或运行入口。 */
    virtual void StartupModule() override
    {
        Actions = MakeShared<FHodgeAbilityDefinitionActions>();
        FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get().RegisterAssetTypeActions(Actions.ToSharedRef());
    }
    /** 在模块卸载时注销其注册项。 */
    virtual void ShutdownModule() override
    {
        if (Actions && FModuleManager::Get().IsModuleLoaded("AssetTools"))
        { FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get().UnregisterAssetTypeActions(Actions.ToSharedRef()); }
        Actions.Reset();
    }
};
IMPLEMENT_MODULE(FHodgeAbilityEditorModule, HodgeAbilityEditor)
