// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class SaucewichServerTarget : TargetRules
{
	public SaucewichServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;

		ExtraModuleNames.AddRange( new string[] { "Saucewich" } );
	}
}
