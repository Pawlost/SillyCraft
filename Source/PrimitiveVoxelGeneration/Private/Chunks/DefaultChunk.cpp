// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunks/DefaultChunk.h"

#include "FastNoiseWrapper.h"
#include "Voxels/Voxel.h"

UDefaultChunk::UDefaultChunk()
{
	Noise = CreateDefaultSubobject<UFastNoiseWrapper>("NoiseGenerator");
}

void UDefaultChunk::NaiveMeshing(
	int32 index, int32 prevIndexDifference, int32 frontIndex,
	FVoxelFace& face,
	TArray<FVoxelFace>& faces)
{
	auto voxelId = Voxels[index];

	int32 forwardVoxelId = 0;
	
	if(Voxels.IsValidIndex(index - frontIndex))
	{
		forwardVoxelId = Voxels[index - frontIndex];
	}

	/*else
	{
		// TODO: cross chunk

		/*
		*				index = Constants::MakeIndex(y, z, Constants::ChunkSize - 1);
				otherBlock = m_registry->GetBlock(chunkArray[Faces::Front]->GetBlockID(index));
		 * 
		 */
//	}

	if(forwardVoxelId == 0){

		int32 prevVoxelId = 0;

		if(Voxels.IsValidIndex(index - prevIndexDifference))
		{
			prevVoxelId = Voxels[index - prevIndexDifference];
		}
		/*
		if(prevVoxelId == voxelId)
		{
			auto prevface =  faces.Last();
			faces.RemoveAt(faces.Num());
	
			face.BeginVertexDown = prevface.BeginVertexDown;
			face.BeginVertexUp = prevface.BeginVertexUp;
		}*/

		faces.Add(face);
	}
}

void UDefaultChunk::GenerateMesh(UProceduralMeshComponent* ProcMesh)
{
	auto chunkWidth = ChunkSettings->GetChunkSideSizeInVoxels();
	TArray<FVoxelFace> frontFaces = TArray<FVoxelFace>();
	
	for(int x = 0; x < chunkWidth; x++)
	{
		for(int y = 0; y < chunkWidth; y++)
		{
			for(int z = 0; z < chunkWidth; z++)
			{
				int32 index = ChunkSettings->GetVoxelIndex(x, y, z);

				auto voxelId = Voxels[index];

				if(voxelId != 0)
				{
					// Front
					FVoxelFace face;

					face.BeginVertexDown = FVector(x, y, z);
					face.BeginVertexUp = FVector(x, y + 1, z);
					face.EndVertexDown = FVector(x, y + 1, z + 1);
					face.EndVertexUp = FVector(x, y, z + 1);

					NaiveMeshing(index, ChunkSettings->GetVoxelIndex(0,-1,0),
						ChunkSettings->GetVoxelIndex(1,0,0), face, frontFaces);

				}
			}
		}	
	}

	int32 index = 0;
	
	TSharedPtr<TArray<FVector>> Vertice = MakeShared<TArray<FVector>>();

	TSharedPtr<TArray<int32>> Triangles = MakeShared<TArray<int32>>();
	
	// Define the cube UV coordinates
	TSharedPtr<TArray<FVector2D>> UVs = MakeShared<TArray<FVector2D>>();
	
	for (auto FrontFace : frontFaces)
	{
		auto voxelSize = ChunkSettings->GetVoxelSize();
		Vertice->Push(FrontFace.BeginVertexDown * voxelSize);
		Vertice->Push(FrontFace.BeginVertexUp* voxelSize);
		Vertice->Push(FrontFace.EndVertexDown* voxelSize);
		Vertice->Push(FrontFace.EndVertexUp* voxelSize);

		Triangles->Add(index); Triangles->Add(index + 1); Triangles->Add(index + 2);
		Triangles->Add(index + 2); Triangles->Add(index + 3); Triangles->Add(index);

		index += 4;
	}

	TSharedPtr<TArray<FLinearColor>> Colors = MakeShared<TArray<FLinearColor>>();

	AsyncTask(ENamedThreads::GameThread, [this, ProcMesh, Vertice, Triangles, Colors]()
	{
	 if(IsValid(ProcMesh) &&  Vertice.IsValid() && Triangles.IsValid() && Colors.IsValid()){
			ProcMesh->CreateMeshSection_LinearColor(0,*Vertice.Get(), *Triangles.Get(), TArray<FVector>(), TArray<FVector2D>(),*Colors.Get(), TArray<FProcMeshTangent>(), true);
			ProcMesh->SetMeshSectionVisible(0, true);
		}
	});
}

void UDefaultChunk::SetChunkSettings(const TSharedPtr<FGenerationSettings> chunkSettings)
{
	Voxels.SetNum(chunkSettings->GetChunkSideSizeInVoxels() * chunkSettings->GetChunkPlaneSizeInVoxels());
	Super::SetChunkSettings(chunkSettings);
}

int32 UDefaultChunk::VoxelAt(int32 index)
{
	return Voxels[index];
}

void UDefaultChunk::GenerateVoxels(FVector& origin)
{
	Noise->SetSeed(ChunkSettings->GetSeed());
	Noise->SetupFastNoise(EFastNoise_NoiseType::ValueFractal, ChunkSettings->GetSeed(), 0.0001f);
	
	auto chunkSize = ChunkSettings->GetChunkSideSizeInVoxels();
	auto maxElevation = ChunkSettings->GetMaximumElevation();;
	
	for (int x = 0; x < chunkSize; x++) 
	{
		for (int y = 0; y < chunkSize; y++) 
		{
			float elevation = Noise->GetNoise2D(x + origin.X, y + origin.Y) * maxElevation;
			
			for (int z = 0; z < chunkSize; z++) 
			{
				int32 index = ChunkSettings->GetVoxelIndex(x, y, z);
				int32 voxelId = 0;
				
				if (origin.Z + z <= elevation) 
				{
					voxelId = 1;
				}

				Voxels[index] = voxelId;
			}
		}
	}
}
