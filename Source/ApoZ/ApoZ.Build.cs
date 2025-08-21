// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ApoZ : ModuleRules
{
	public ApoZ(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",        // Nouveau système d'input UE5
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",                  // Widget UI
            "SlateCore",            // UI Core
            "Slate",                // UI
            "GameplayAbilities",    // GAS pour les compétences
            "GameplayTags",         // Tags pour GAS
            "GameplayTasks",        // Tasks pour GAS
            "Networking",           // Réseau
            "OnlineSubsystem",      // Steam
            "OnlineSubsystemSteam"  // Steam spécifique
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"ToolMenus",
			"UnrealEd",
			"EditorStyle",
			"EditorWidgets"
		 });

		PublicIncludePaths.AddRange(new string[]
		{
			"ApoZ",
			"ApoZ/Variant_Platforming",
			"ApoZ/Variant_Platforming/Animation",
			"ApoZ/Variant_Combat",
			"ApoZ/Variant_Combat/AI",
			"ApoZ/Variant_Combat/Animation",
			"ApoZ/Variant_Combat/Gameplay",
			"ApoZ/Variant_Combat/Interfaces",
			"ApoZ/Variant_Combat/UI",
			"ApoZ/Variant_SideScrolling",
			"ApoZ/Variant_SideScrolling/AI",
			"ApoZ/Variant_SideScrolling/Gameplay",
			"ApoZ/Variant_SideScrolling/Interfaces",
			"ApoZ/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

		OptimizeCode = CodeOptimization.InShippingBuildsOnly;
        
        // Définir nos macros
        PublicDefinitions.Add("APOZ_LOGGING=1");
        
        if (Target.Configuration == UnrealTargetConfiguration.Development ||
            Target.Configuration == UnrealTargetConfiguration.Debug)
        {
            PublicDefinitions.Add("APOZ_DEBUG=1");
        }
	}
}
