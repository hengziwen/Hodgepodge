/**
 * @file Hodgepodge.Build.cs
 * @brief Hodgepodge 模块的构建配置文件
 *
 * 定义了模块的编译依赖、预编译头配置等:
 * - PCHUsage:使用显式或共享 PCH
 * - PublicDependencyModuleNames:公共依赖模块(Core、CoreUObject、Engine、GAS 等)
 * - PrivateDependencyModuleNames:私有依赖模块(按需添加 Slate、OnlineSubsystem 等)
 */

using UnrealBuildTool;

public class Hodgepodge : ModuleRules
{
	public Hodgepodge(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core",
			"CoreUObject", 
			"Engine", 
			"InputCore",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"ALS", 
			"GameFeatures"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			// ALSCamera: 提供 UAlsCameraComponent 相机组件。
			"ALSCamera",
			// EnhancedInput: 增强输入系统,用于角色输入绑定。
			"EnhancedInput"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}