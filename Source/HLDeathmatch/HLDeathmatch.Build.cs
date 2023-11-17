// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.Linq;
using UnrealBuildTool;

public class HLDeathmatch : ModuleRules
{
	public HLDeathmatch(ReadOnlyTargetRules Target) : base(Target)
	{
		// This disables Code Optimisation. Helpful for debugging
		bool bUnoptimisedDebugging = false;

		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		string userName = Environment.UserName;

		if(Target.Configuration != UnrealTargetConfiguration.Shipping && bUnoptimisedDebugging)
		{
			OptimizeCode = CodeOptimization.Never;
		}

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "NetCore" });
	}
}