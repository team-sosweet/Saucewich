// Copyright 2019 Seokjin Lee. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SaucewichServerTarget : TargetRules
{
	public SaucewichServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		bUseLoggingInShipping = true;
		ExtraModuleNames.AddRange( new string[] { "Saucewich" } );
	}
}
