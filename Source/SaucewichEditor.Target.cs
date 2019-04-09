// Copyright (c) 2019, Team Sosweet. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SaucewichEditorTarget : TargetRules
{
	public SaucewichEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		ExtraModuleNames.Add("Saucewich");
	}
}
