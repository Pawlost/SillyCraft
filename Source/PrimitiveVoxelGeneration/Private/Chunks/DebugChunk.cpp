// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunks/DebugChunk.h"

#include <Mesh/RealtimeMeshBuilder.h>

#include "RealtimeMeshSimple.h"

class URealtimeMeshSimple;

void UDebugChunk::GenerateMesh()
{
	AsyncTask(ENamedThreads::GameThread, [this]()
{
	URealtimeMeshSimple* RealtimeMesh = ChunkActor->GetRealtimeMeshComponent()->InitializeRealtimeMesh<URealtimeMeshSimple>();
	FRealtimeMeshStreamSet StreamSet;
		
	TRealtimeMeshBuilderLocal<int32> Builder(StreamSet);

	Builder.EnableTexCoords();
	Builder.EnableColors();
	
	Builder.EnablePolyGroups();

		int32 V0 = Builder.AddVertex(FVector3f(-100, -100, 0))
			.SetColor(FColor::White)
			.SetTexCoord(FVector2f(0, 0));

		// Add our second vertex
		int32 V1 = Builder.AddVertex(FVector3f(100, -100, 0))
			.SetColor(FColor::White)
			.SetTexCoord(FVector2f(1, 0));

		// Add our third vertex
		int32 V2 = Builder.AddVertex(FVector3f(100, 100, 0))
			.SetColor(FColor::White)
			.SetTexCoord(FVector2f(1, 1));
				
		int32 V3 = Builder.AddVertex(FVector3f(-100, 100, 0))
		.SetColor(FColor::White)
		.SetTexCoord(FVector2f(0, 1));
				
		Builder.AddTriangle(V0, V1, V2, 0);
		Builder.AddTriangle(V2, V3, V0, 0);
		
		const FRealtimeMeshSectionGroupKey GroupKey = FRealtimeMeshSectionGroupKey::Create(0, FName("Chunk Mesh"));

		// Now we create the section group, since the stream set has polygroups, this will create the sections as well
		RealtimeMesh->CreateSectionGroup(GroupKey, StreamSet);
	});
}
