// Copyright (c) 2019, Team Sosweet. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SaucewichTarget : TargetRules
{
	public SaucewichTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		ExtraModuleNames.Add("Saucewich");
	}
}
