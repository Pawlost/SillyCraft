// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunks/DebugChunk.h"
#include "ProceduralMeshComponent.h"

void UDebugChunk::GenerateMesh(UProceduralMeshComponent* ProcMesh)
{
	TSharedPtr<TArray<FVector>> Vertices = MakeShared<TArray<FVector>>();
	Vertices.Get()->Add(FVector(-100, -100, 0));     // Front-left
	Vertices.Get()->Add(FVector(100, -100, 0));      // Front-right
	Vertices.Get()->Add(FVector(100, 100, 0));        // Back-right
	Vertices.Get()->Add(FVector(-100, 100, 0));        // Back-left

	// Define the cube triangles

	TSharedPtr<TArray<int32>> Triangles = MakeShared<TArray<int32>>();
	Triangles.Get()->Add(0); Triangles.Get()->Add(1); Triangles.Get()->Add(2);
	Triangles.Get()->Add(2); Triangles.Get()->Add(3); Triangles.Get()->Add(0);

	// Define the cube UV coordinates
	TSharedPtr<TArray<FVector2D>> UVs = MakeShared<TArray<FVector2D>>();
	UVs.Get()->Add(FVector2D(0, 0));
	UVs.Get()->Add(FVector2D(1, 0));
	UVs.Get()->Add(FVector2D(1, 1));
	UVs.Get()->Add(FVector2D(0, 1));

	TSharedPtr<TArray<FLinearColor>> Colors = MakeShared<TArray<FLinearColor>>();
	Colors.Get()->Append(TArray<FLinearColor>(&FLinearColor::Green, Vertices.Get()->Num()));
	
	AsyncTask(ENamedThreads::GameThread, [this, ProcMesh, Vertices, Triangles, Colors]()
	{
	 if(IsValid(ProcMesh) &&  Vertices.IsValid() && Triangles.IsValid() && Colors.IsValid()){
	 		auto test = this->ChunkSettings.Get();
	 		int32 test2 = test->GetSeed();
			ProcMesh->CreateMeshSection_LinearColor(0, *Vertices.Get(), *Triangles.Get(), TArray<FVector>(), TArray<FVector2D>(), *Colors.Get(), TArray<FProcMeshTangent>(), true);
			ProcMesh->SetMeshSectionVisible(0, true);
	    }
	});
	
	Super::GenerateMesh(ProcMesh);
}
