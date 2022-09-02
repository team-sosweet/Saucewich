// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

using UnrealBuildTool;

public class Saucewich : ModuleRules
{
	public Saucewich(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HTTP", "Json", "JsonUtilities", "OnlineSubsystem" });
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		//PrivateDependencyModuleNames.Add("GameLiftServerSDK");

		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			DynamicallyLoadedModuleNames.Add("OnlineSubsystemGooglePlay");
		}
	}
}
