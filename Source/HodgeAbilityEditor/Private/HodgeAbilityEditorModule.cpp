/** @file HodgeAbilityEditorModule.cpp
 * @brief 实现技能编辑器模块注册相关的类型、接口与现有处理流程。
 */
#include "Modules/ModuleManager.h"
#include "AssetToolsModule.h"
#include "AssetTypeActions_Base.h"
#include "Data/HodgeAbilityDefinition.h"
#include "Toolkits/SimpleAssetEditor.h"
#include "HodgeNotifyDamageCustomization.h"
#include "HodgeCombatAssetCustomization.h"
#include "HodgeCombatPropertyCustomization.h"
#include "PropertyEditorModule.h"
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
    // 身份对象必须与注销时配对，避免影响其他上下文的同类型详情。
    TSharedPtr<IPropertyTypeIdentifier> NotifyDamageIdentifier;
public:
    /** 在模块加载时注册其编辑器或运行入口。 */
    virtual void StartupModule() override
    {
        Actions = MakeShared<FHodgeAbilityDefinitionActions>();
        FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get().RegisterAssetTypeActions(Actions.ToSharedRef());
        NotifyDamageIdentifier = FHodgeNotifyDamageCustomization::MakeIdentifier();
        auto& Properties = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
        Properties.RegisterCustomPropertyTypeLayout(TEXT("HodgeHitEffectConfig"),
            FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FHodgeNotifyDamageCustomization::MakeInstance), NotifyDamageIdentifier);
        // 复用既有资产与结构，不建立新的配置副本或运行时入口。
        Properties.RegisterCustomClassLayout(TEXT("HodgeAbilityDefinition"),
            FOnGetDetailCustomizationInstance::CreateStatic(&HodgeCombatDetails::MakeAbilityDefinitionCustomization));
        Properties.RegisterCustomClassLayout(TEXT("HodgePawnData"),
            FOnGetDetailCustomizationInstance::CreateStatic(&HodgeCombatDetails::MakePawnDataCustomization));
        Properties.RegisterCustomClassLayout(TEXT("HodgeHitReactionProfile"),
            FOnGetDetailCustomizationInstance::CreateStatic(&HodgeCombatDetails::MakeHitReactionProfileCustomization));
        Properties.RegisterCustomPropertyTypeLayout(TEXT("HodgeHitReactionConfig"),
            FOnGetPropertyTypeCustomizationInstance::CreateStatic(&HodgeCombatDetails::MakeHitReactionConfigCustomization));
        Properties.RegisterCustomPropertyTypeLayout(TEXT("HodgePoiseSettings"),
            FOnGetPropertyTypeCustomizationInstance::CreateStatic(&HodgeCombatDetails::MakePoiseSettingsCustomization));
        Properties.RegisterCustomPropertyTypeLayout(TEXT("HodgeAnimHitConfig"),
            FOnGetPropertyTypeCustomizationInstance::CreateStatic(&HodgeCombatDetails::MakeAnimHitConfigCustomization));
        Properties.RegisterCustomPropertyTypeLayout(TEXT("HodgeHitReactionAnimation"),
            FOnGetPropertyTypeCustomizationInstance::CreateStatic(&HodgeCombatDetails::MakeLegacyReactionAnimationCustomization));
        Properties.NotifyCustomizationModuleChanged();
    }
    /** 在模块卸载时注销其注册项。 */
    virtual void ShutdownModule() override
    {
        if (NotifyDamageIdentifier && FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
        {
            auto& Properties = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
            Properties.UnregisterCustomPropertyTypeLayout(TEXT("HodgeHitEffectConfig"), NotifyDamageIdentifier);
            // 使用与注册一致的类型名，卸载后不留下指向本模块的委托。
            Properties.UnregisterCustomClassLayout(TEXT("HodgeAbilityDefinition"));
            Properties.UnregisterCustomClassLayout(TEXT("HodgePawnData"));
            Properties.UnregisterCustomClassLayout(TEXT("HodgeHitReactionProfile"));
            Properties.UnregisterCustomPropertyTypeLayout(TEXT("HodgeHitReactionConfig"));
            Properties.UnregisterCustomPropertyTypeLayout(TEXT("HodgePoiseSettings"));
            Properties.UnregisterCustomPropertyTypeLayout(TEXT("HodgeAnimHitConfig"));
            Properties.UnregisterCustomPropertyTypeLayout(TEXT("HodgeHitReactionAnimation"));
            Properties.NotifyCustomizationModuleChanged();
        }
        NotifyDamageIdentifier.Reset();
        if (Actions && FModuleManager::Get().IsModuleLoaded("AssetTools"))
        { FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get().UnregisterAssetTypeActions(Actions.ToSharedRef()); }
        Actions.Reset();
    }
};
IMPLEMENT_MODULE(FHodgeAbilityEditorModule, HodgeAbilityEditor)
