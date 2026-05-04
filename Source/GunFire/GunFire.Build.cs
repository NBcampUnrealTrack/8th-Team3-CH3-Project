// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GunFire : ModuleRules
{
	public GunFire(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
