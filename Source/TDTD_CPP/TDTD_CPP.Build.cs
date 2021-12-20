// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TDTD_CPP : ModuleRules
{
	public TDTD_CPP(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				"TDTD_CPP/Camera",
				"TDTD_CPP/GridWorld",
				"TDTD_CPP/Units"
				// ... add public include paths required here ...
			}
		);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"TDTD_CPP/Camera",
				"TDTD_CPP/GridWorld",
				"TDTD_CPP/Units"
				// ... add other private include paths required here ...
			}
		);
        PublicDependencyModuleNames.AddRange(new string[]
        {
	        "Core",
	        "CoreUObject",
	        "Engine",
	        "InputCore",
	        "HeadMountedDisplay",
	        "NavigationSystem",
	        "AIModule", 
	        "VarDump"/*,
	        "ProceduralMeshComponent"*/
        });
    }
}
