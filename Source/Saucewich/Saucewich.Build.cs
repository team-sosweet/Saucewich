// Copyright 2019 Team Sosweet. All Rights Reserved.

using UnrealBuildTool;

public class Saucewich : ModuleRules
{
	public Saucewich(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Http", "Json", "JsonUtilities", "GameLiftServerSDK" });
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
	}
}
