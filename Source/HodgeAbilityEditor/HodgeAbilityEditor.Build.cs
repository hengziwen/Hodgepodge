using UnrealBuildTool;
public class HodgeAbilityEditor : ModuleRules
{
    public HodgeAbilityEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivateDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "Hodgepodge", "GameplayTags", "GameplayAbilities",
            "UnrealEd", "AssetTools", "AssetRegistry", "PropertyEditor", "Slate", "SlateCore",
            "InputCore", "EditorFramework", "AdvancedPreviewScene", "AnimGraph", "ToolMenus",
            "BlueprintGraph", "KismetCompiler"
        });
    }
}
