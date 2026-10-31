// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TeleportStealth : ModuleRules
{
	public TeleportStealth(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"NavigationSystem",
			"GameplayTasks"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"TeleportStealth",
			"TeleportStealth/Variant_Platforming",
			"TeleportStealth/Variant_Platforming/Animation",
			"TeleportStealth/Variant_Combat",
			"TeleportStealth/Variant_Combat/AI",
			"TeleportStealth/Variant_Combat/Animation",
			"TeleportStealth/Variant_Combat/Gameplay",
			"TeleportStealth/Variant_Combat/Interfaces",
			"TeleportStealth/Variant_Combat/UI",
			"TeleportStealth/Variant_SideScrolling",
			"TeleportStealth/Variant_SideScrolling/AI",
			"TeleportStealth/Variant_SideScrolling/Gameplay",
			"TeleportStealth/Variant_SideScrolling/Interfaces",
			"TeleportStealth/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
