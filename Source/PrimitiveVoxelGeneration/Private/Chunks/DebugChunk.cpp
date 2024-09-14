// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunks/DebugChunk.h"
#include "ProceduralMeshComponent.h"

void UDebugChunk::GenerateMesh(UProceduralMeshComponent* ProcMesh, FIntVector& chunkGridPos)
{
	TSharedPtr<TArray<FVector>> Vertice = MakeShared<TArray<FVector>>();
	Vertice->Add(FVector(-100, -100, 0));     // Front-left
	Vertice->Add(FVector(100, -100, 0));      // Front-right
	Vertice->Add(FVector(100, 100, 0));        // Back-right
	Vertice->Add(FVector(-100, 100, 0));        // Back-left

	// Define the cube triangles

	TSharedPtr<TArray<int32>> Triangles = MakeShared<TArray<int32>>();
	Triangles->Add(0); Triangles->Add(1); Triangles->Add(2);
	Triangles->Add(2); Triangles->Add(3); Triangles->Add(0);

	// Define the cube UV coordinates
	TSharedPtr<TArray<FVector2D>> UVs = MakeShared<TArray<FVector2D>>();
	UVs->Add(FVector2D(0, 0));
	UVs->Add(FVector2D(1, 0));
	UVs->Add(FVector2D(1, 1));
	UVs->Add(FVector2D(0, 1));

	TSharedPtr<TArray<FLinearColor>> Colors = MakeShared<TArray<FLinearColor>>();
//	Colors.Get()->Append(TArray<FLinearColor>(&FLinearColor::Green, vertice->Num()*3));

	AsyncTask(ENamedThreads::GameThread, [this, ProcMesh, Vertice, Triangles, Colors]()
	{
	 if(IsValid(ProcMesh) &&  Vertice.IsValid() && Triangles.IsValid() && Colors.IsValid()){
			ProcMesh->CreateMeshSection_LinearColor(0,*Vertice.Get(), *Triangles.Get(), TArray<FVector>(), TArray<FVector2D>(),*Colors.Get(), TArray<FProcMeshTangent>(), true);
			ProcMesh->SetMeshSectionVisible(0, true);
		}
	});
}