// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SillyCraftEditorTarget : TargetRules
{
	public SillyCraftEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.Add("SillyCraft");
	}
}
