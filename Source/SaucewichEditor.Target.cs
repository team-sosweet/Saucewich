// Copyright 2019 Team Sosweet. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SaucewichEditorTarget : TargetRules
{
	public SaucewichEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "Saucewich" } );
	}
}
