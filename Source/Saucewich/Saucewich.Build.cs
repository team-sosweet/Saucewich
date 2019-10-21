// Copyright 2019 Team Sosweet. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class Saucewich : ModuleRules
{
	public Saucewich(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Http", "Json", "JsonUtilities", "GameLiftServerSDK" });
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        LoadLibraries(new string[] { "aws-cpp-sdk-core", "aws-cpp-sdk-gamelift" });
	}

	private void LoadLibraries(string[] Libraries)
	{
		string ThirdPartyPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../ThirdParty/"));

		foreach (string Library in Libraries)
		{
			string LibraryPath = ThirdPartyPath + Library + "/";
			string BinPath = LibraryPath + "bin/";
			string IncPath = LibraryPath + "include/";

			PublicLibraryPaths.Add(BinPath);
			PublicIncludePaths.Add(IncPath);
			PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, Library + ".lib"));

			string DLLName = Library + ".dll";
			PublicDelayLoadDLLs.Add(DLLName);
			RuntimeDependencies.Add(Path.Combine(LibraryPath, DLLName));
		}
	}
}
