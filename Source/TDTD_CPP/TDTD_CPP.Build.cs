// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TDTD_CPP : ModuleRules
{
	public TDTD_CPP(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
	        "Core",
	        "CoreUObject",
	        "Engine",
	        "InputCore",
	        "HeadMountedDisplay",
	        "NavigationSystem",
	        "AIModule"/*,
	        "ProceduralMeshComponent"*/
        });
    }
}
