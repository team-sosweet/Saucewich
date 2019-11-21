// Copyright 2019 Seokjin Lee. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SaucewichClientTarget : TargetRules
{
	public SaucewichClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Client;

		ExtraModuleNames.AddRange( new string[] { "Saucewich" } );
	}
}
