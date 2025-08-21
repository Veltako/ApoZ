// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AZ_Core : ModuleRules
{
	public AZ_Core(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[]
			{
				// ... add public include paths required here ...
			}
		);


		PrivateIncludePaths.AddRange(
			new string[]
			{
				// ... add other private include paths required here ...
			}
		);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"GameplayAbilities", // Pour GAS
				"GameplayTags",		// Pour les tags GAS
				"GameplayTasks"		// Pour les tâches GAS
			}
		);


		PrivateDependencyModuleNames.AddRange(new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"ToolMenus",
				"EditorStyle",
				"EditorWidgets",
				"UnrealEd"
			}
		);


		DynamicallyLoadedModuleNames.AddRange(new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
		
	}
}