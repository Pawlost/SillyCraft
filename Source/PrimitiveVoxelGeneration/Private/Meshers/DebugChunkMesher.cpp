// Fill out your copyright notice in the Description page of Project Settings.


#include "Meshers/DebugChunkMesher.h"
#include "ProceduralMeshComponent.h"

void UDebugChunkMesher::GenerateMesh(UProceduralMeshComponent* ProcMesh)
{
	TArray<FVector> Vertices;
	Vertices.Add(FVector(-100, -100, 0));     // Front-left
	Vertices.Add(FVector(100, -100, 0));      // Front-right
	Vertices.Add(FVector(100, 100, 0));        // Back-right
	Vertices.Add(FVector(-100, 100, 0));        // Back-left

	// Define the cube triangles
	TArray<int32> Triangles;
	Triangles.Add(0); Triangles.Add(1); Triangles.Add(2);
	Triangles.Add(2); Triangles.Add(3); Triangles.Add(0);

	// Define the cube UV coordinates
	TArray<FVector2D> UVs;
	UVs.Add(FVector2D(0, 0));
	UVs.Add(FVector2D(1, 0));
	UVs.Add(FVector2D(1, 1));
	UVs.Add(FVector2D(0, 1));

	TArray<FLinearColor> Colors = TArray<FLinearColor>(&FLinearColor::Green, Vertices.Num());

	ProcMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), Colors, TArray<FProcMeshTangent>(), true);
	ProcMesh->SetMeshSectionVisible(0, true);
}
