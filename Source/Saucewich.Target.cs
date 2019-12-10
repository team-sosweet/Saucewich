// Copyright 2019 Othereum. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SaucewichTarget : TargetRules
{
	public SaucewichTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "Saucewich" } );
	}
}
