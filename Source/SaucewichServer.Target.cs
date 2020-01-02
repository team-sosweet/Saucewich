// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SaucewichServerTarget : TargetRules
{
	public SaucewichServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		bUseLoggingInShipping = true;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "Saucewich" } );
	}
}
