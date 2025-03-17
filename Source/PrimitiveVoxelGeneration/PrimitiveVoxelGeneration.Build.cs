using UnrealBuildTool;

public class PrimitiveVoxelGeneration : ModuleRules
{
    public PrimitiveVoxelGeneration(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "ProceduralMeshComponent",
                "RealtimeMeshComponent",
                "FastNoise",
                "FastNoiseGenerator",
                "RunDirectionalMeshingDemo"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
            }
        );
    }
}