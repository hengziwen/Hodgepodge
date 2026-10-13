/** @file Hodgepodge.Build.cs
 * @brief 配置podge构建相关的类型、接口与现有处理流程。
 */
using UnrealBuildTool;

// 定义当前模块或构建目标的UBT规则。
public class Hodgepodge : ModuleRules
{
	// 根据传入目标信息设置本构建规则；Target为UBT提供的平台与配置。
	public Hodgepodge(ReadOnlyTargetRules Target) : base(Target)
	{
		// 设置预编译头使用方式，沿用当前项目配置。
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		// 保留项目当前使用的头文件包含顺序版本。
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;

		// 启用当前生成代码相关的构建检查设置。
		bEnableNonInlinedGenCppWarnings = true;
		// UnsafeTypeCastWarningLevel = WarningLevel.Warning;

		// 登记当前依赖可见性对应的模块列表。
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"ModularGameplay",
			"GameFeatures",
			"AIModule",
			"EngineSettings",
			"NetCore",
			"AnimGraphRuntime",
			"RigVM",
			"ControlRig",
			"MotionWarping",
			"UMG",
			"SlateCore",
			"CommonUI",
			"CommonInput",
			"Slate",
			"Niagara",
		});

		// 登记当前依赖可见性对应的模块列表。
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"EnhancedInput",
			"PhysicsCore",
			"SignificanceManager",
			// IPlatformInputDeviceMapper 定义在 ApplicationCore（GenericPlatformInputDeviceMapper.h）。
			"ApplicationCore"
		});

		// 沿用引擎提供的Iris构建支持设置。
		SetupIrisSupport(Target);

		if (Target.bBuildEditor)
		{
			// 登记当前依赖可见性对应的模块列表。
			PrivateDependencyModuleNames.AddRange(new string[]
				{ "UnrealEd", "AnimGraph", "BlueprintGraph", "AnimationWarpingRuntime", "AnimationWarpingEditor" });
		}

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
