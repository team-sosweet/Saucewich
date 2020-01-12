// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SaucewichTarget : TargetRules
{
	public SaucewichTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		bUseLoggingInShipping = true;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "Saucewich" } );
	}
}
