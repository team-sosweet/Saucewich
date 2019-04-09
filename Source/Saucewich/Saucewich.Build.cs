// Copyright (c) 2019, Team Sosweet. All rights reserved.

using UnrealBuildTool;

public class Saucewich : ModuleRules
{
	public Saucewich(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
