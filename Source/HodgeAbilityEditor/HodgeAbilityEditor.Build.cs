/** @file HodgeAbilityEditor.Build.cs
 * @brief 配置能力编辑器构建相关的类型、接口与现有处理流程。
 */
using UnrealBuildTool;
// 定义当前模块或构建目标的UBT规则。
public class HodgeAbilityEditor : ModuleRules
{
    // 根据传入目标信息设置本构建规则；Target为UBT提供的平台与配置。
    public HodgeAbilityEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        // 设置预编译头使用方式，沿用当前项目配置。
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        // 登记当前依赖可见性对应的模块列表。
        PrivateDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "Hodgepodge", "GameplayTags", "GameplayAbilities",
            "UnrealEd", "AssetTools", "AssetRegistry", "PropertyEditor", "Slate", "SlateCore",
            "InputCore", "EditorFramework", "AdvancedPreviewScene", "AnimGraph", "AnimGraphRuntime", "ToolMenus", "AIModule", "AIGraph", "BehaviorTreeEditor", "NavigationSystem",
            "BlueprintGraph", "KismetCompiler", "UMG", "UMGEditor", "CommonUI", "CommonInput", "Json", "ApplicationCore", "GameFeatures", "MotionWarping"
        });
    }
}
