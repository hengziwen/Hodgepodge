using UnrealBuildTool;

public class Hodgepodge : ModuleRules
{
	public Hodgepodge(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;

		bEnableNonInlinedGenCppWarnings = true;
		// UnsafeTypeCastWarningLevel = WarningLevel.Warning;

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
			"UMG",
			"SlateCore",
			"CommonUI",
			"CommonInput",
			"Slate",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"EnhancedInput",
			"PhysicsCore",
			"Niagara",
			"SignificanceManager"
		});

		SetupIrisSupport(Target);

		if (Target.bBuildEditor)
		{
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