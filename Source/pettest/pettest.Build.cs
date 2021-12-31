// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class pettest : ModuleRules
{
	public pettest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Niagara", "PhysicsCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		PublicIncludePaths.AddRange(new string[] 
		{
			"pettest",
			"pettest/Public/Components",
			"pettest/Public/UI",
			"pettest/Public/Player",
			"pettest/Public/Weapon",
			"pettest/Public/Pickups",
			"pettest/Public/Sounds",
			});
		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
