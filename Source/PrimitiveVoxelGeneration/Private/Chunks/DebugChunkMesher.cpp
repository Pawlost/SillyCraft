// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunks/DebugChunkMesher.h"

#include <Mesh/RealtimeMeshBuilder.h>

#include "RealtimeMeshSimple.h"
#include "Chunks/ChunkGridData.h"
#include "Voxels/VoxelType.h"

class URealtimeMeshSimple;

void UDebugChunkMesher::GenerateMesh()
{
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		URealtimeMeshSimple* RealtimeMesh = ChunkActor->GetRealtimeMeshComponent()->InitializeRealtimeMesh<URealtimeMeshSimple>();
		FRealtimeMeshStreamSet StreamSet;
		
		TRealtimeMeshBuilderLocal<int32> Builder(StreamSet);

		Builder.EnableTexCoords();
		Builder.EnableColors();
	
		Builder.EnablePolyGroups();

			int32 V0 = Builder.AddVertex(FVector3f(0, 0, 0))
				.SetColor(FColor::White)
				.SetTexCoord(FVector2f(0, 0));

			int32 V1 = Builder.AddVertex(FVector3f(100, 0, 0))
				.SetColor(FColor::White)
				.SetTexCoord(FVector2f(1, 0));

			int32 V2 = Builder.AddVertex(FVector3f(100, 100, 0))
				.SetColor(FColor::White)
				.SetTexCoord(FVector2f(1, 1));
				
			int32 V3 = Builder.AddVertex(FVector3f(0, 100, 0))
			.SetColor(FColor::White)
			.SetTexCoord(FVector2f(0, 1));

		Builder.AddTriangle(V0, V1, V2, 0);
		Builder.AddTriangle(V2, V3, V0, 0);

		//Second quad
		int32 V02 = Builder.AddVertex(FVector3f(0, 100, 0))
			.SetColor(FColor::White)
			.SetTexCoord(FVector2f(0, 0));

		int32 V12 = Builder.AddVertex(FVector3f(100, 100, 0))
			.SetColor(FColor::White)
			.SetTexCoord(FVector2f(1, 0));

		int32 V22= Builder.AddVertex(FVector3f(100, 200, 0))
			.SetColor(FColor::White)
			.SetTexCoord(FVector2f(1, 1));
				
		int32 V32 = Builder.AddVertex(FVector3f(0, 200, 0))
			.SetColor(FColor::White)
			.SetTexCoord(FVector2f(1, 1));
				
		Builder.AddTriangle(V02, V12, V22, 1);
		Builder.AddTriangle(V22, V32, V02, 1);
		
			const FRealtimeMeshSectionGroupKey GroupKey = FRealtimeMeshSectionGroupKey::Create(0, FName("Chunk Mesh"));

			// Now we create the section group, since the stream set has polygroups, this will create the sections as well
			RealtimeMesh->CreateSectionGroup(GroupKey, StreamSet);

			FRealtimeMeshSectionKey PolyGroup0SectionKey = FRealtimeMeshSectionKey::CreateForPolyGroup(GroupKey, 0);
			RealtimeMesh->UpdateSectionConfig(PolyGroup0SectionKey, FRealtimeMeshSectionConfig(ERealtimeMeshSectionDrawType::Static, 0), true);

			FVoxelType voxelType = ChunkGridData->GetVoxelTypeById(0);
			RealtimeMesh->SetupMaterialSlot(0, voxelType.BlockName, voxelType.Material);
		
		 PolyGroup0SectionKey = FRealtimeMeshSectionKey::CreateForPolyGroup(GroupKey, 1);
		RealtimeMesh->UpdateSectionConfig(PolyGroup0SectionKey, FRealtimeMeshSectionConfig(ERealtimeMeshSectionDrawType::Static, 1), true);

		voxelType = ChunkGridData->GetVoxelTypeById(1);
		RealtimeMesh->SetupMaterialSlot(1, voxelType.BlockName, voxelType.Material);
	}
	);
}
