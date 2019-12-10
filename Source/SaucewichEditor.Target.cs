// Copyright 2019 Othereum. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SaucewichEditorTarget : TargetRules
{
	public SaucewichEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "Saucewich" } );
	}
}
