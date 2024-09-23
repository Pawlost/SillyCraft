// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunks/DefaultChunk.h"

#include "FastNoiseWrapper.h"
#include "Chunks/ChunkActor.h"
#include "Chunks/ChunkGridData.h"
#include "Chunks/ChunkSettings.h"
#include "Voxels/ChunkFace.h"
#include "Voxels/Voxel.h"
#include "Voxels/VoxelType.h"

void UDefaultChunk::AddToGrid(const TWeakObjectPtr<UChunkGridData> chunkGridData, FIntVector& chunkGridPos)
{
	ChunkSettings = chunkGridData->GetChunkSettings();
	Voxels.SetNum(ChunkSettings->GetChunkSideSizeInVoxels() * ChunkSettings->GetChunkPlaneSizeInVoxels());
	Super::AddToGrid(chunkGridData, chunkGridPos);
}

UDefaultChunk::UDefaultChunk()
{
	Noise = CreateDefaultSubobject<UFastNoiseWrapper>("NoiseGenerator");
}

void UDefaultChunk::AddNaiveMeshedFace(FChunkFace& face, TArray<FChunkFace>& faces)
{
	
	faces.Add(face);

  // Naive Greedy Meshing
	
	//	int32 prevVoxelId = 0;

	//	if(Voxels.IsValidIndex(voxelIndex - prevVoxelPos))
		{
	//		prevVoxelId = Voxels[voxelIndex - prevVoxelPos];
		}
		/*
		if(prevVoxelId == voxelId)
		{
			auto prevface =  faces.Last();
			faces.RemoveAt(faces.Num());
	
			face.BeginVertexDown = prevface.BeginVertexDown;
			face.BeginVertexUp = prevface.BeginVertexUp;
		}*/

		
	//}
}

bool UDefaultChunk::ChunkCull(int32 chunkIndex, FIntVector& neighborChunkDistance) const
{
	FIntVector neighborChunkCoords = ChunkGridPos + neighborChunkDistance;
	auto chunk = ChunkGridData->GetChunkPtr(neighborChunkCoords);
	return chunk != nullptr && chunk->VoxelAt(chunkIndex).IsEmptyVoxel();
}

bool UDefaultChunk::VoxelCull(int32 forwardVoxelIndex)
{
	return Voxels.IsValidIndex(forwardVoxelIndex) && Voxels[forwardVoxelIndex].IsEmptyVoxel();
}

bool UDefaultChunk::CrossChunkCullMin(int min, int32 forwardVoxelIndex, int32 chunkIndex, FIntVector neighborChunkDistance)
{
	// min == 0 is first face voxel in chunk, needs to be always shown or cross chunk compared
	if(min == 0)
	{
		return ChunkCull(chunkIndex, neighborChunkDistance);
	}
	
	return VoxelCull(forwardVoxelIndex);
}

bool UDefaultChunk::CrossChunkCullMax(int max, int32 forwardVoxelIndex, int32 chunkIndex,
	FIntVector neighborChunkDistance)
{
	auto chunkSize = ChunkSettings->GetChunkSideSizeInVoxels();

	if(max == chunkSize - 1)
	{
		return ChunkCull(chunkIndex, neighborChunkDistance);
	}

	// chunkSize == 0 is last face voxel in chunk, needs to be always shown or cross chunk compared
	return VoxelCull(forwardVoxelIndex);
}

void UDefaultChunk::GenerateMesh()
{
	if(IsEmpty)
	{
		return;
	}
	
	auto chunkLenght = ChunkSettings->GetChunkSideSizeInVoxels();
	
	TMap<int32, TUniquePtr<TArray<FChunkFace>>> faces;
	
	for(int x = 0; x < chunkLenght; x++)
	{
		for(int y = 0; y < chunkLenght; y++)
		{
			for(int z = 0; z < chunkLenght; z++)
			{
				int32 index = ChunkSettings->GetVoxelIndex(x, y, z);

				FVoxel voxelId = Voxels[index];
				
				if(!voxelId.IsEmptyVoxel())
				{

					if(!faces.Contains(voxelId.VoxelId))
					{
						faces.Add(voxelId.VoxelId, MakeUnique<TArray<FChunkFace>>());
					}
					
					FVector position = FVector(x, y, z);

					FChunkFace face;
			
					// Front
					if(CrossChunkCullMin(x, index + ChunkSettings->GetVoxelIndex(-1,0,0),
						ChunkSettings->GetVoxelIndex(chunkLenght - 1, y, z), FIntVector(-1, 0, 0)))
					{
						face = FChunkFace::CreateFrontFace(position, voxelId);
						AddNaiveMeshedFace(face, **faces.Find(voxelId.VoxelId));
					}
					
					// Back
					if(CrossChunkCullMax(x, index + ChunkSettings->GetVoxelIndex(1,0,0),
						ChunkSettings->GetVoxelIndex(0, y, z), FIntVector(1, 0, 0)))
					{
						face = FChunkFace::CreateBackFace(position, voxelId);
						AddNaiveMeshedFace(face, **faces.Find(voxelId.VoxelId));
					}
					
					// Left
					if(CrossChunkCullMin(y, index + ChunkSettings->GetVoxelIndex(0,-1,0),
						ChunkSettings->GetVoxelIndex(x, chunkLenght - 1, z), FIntVector(0, -1, 0)))
					{
						face = FChunkFace::CreateLeftFace(position, voxelId);
						AddNaiveMeshedFace(face, **faces.Find(voxelId.VoxelId));
					}
					
					// Right
					if(CrossChunkCullMax(y, index + ChunkSettings->GetVoxelIndex(0,1,0),
						ChunkSettings->GetVoxelIndex(x, 0,z ), FIntVector(0, 1, 0)))
					{
						face = FChunkFace::CreateRightFace(position, voxelId);
						AddNaiveMeshedFace(face, **faces.Find(voxelId.VoxelId));
					}

					// Bottom
					if(CrossChunkCullMin(z, index + ChunkSettings->GetVoxelIndex(0,0,-1),
						ChunkSettings->GetVoxelIndex(x, y, chunkLenght - 1), FIntVector(0, 0, -1)))
					{
						face = FChunkFace::CreateBottomFace(position, voxelId);
						AddNaiveMeshedFace(face, **faces.Find(voxelId.VoxelId));
					}

					// Top
					if(CrossChunkCullMax(z, index + ChunkSettings->GetVoxelIndex(0,0,1),
						ChunkSettings->GetVoxelIndex(x, y,0), FIntVector(0, 0, 1)))
					{
						face = FChunkFace::CreateTopFace(position, voxelId);
						AddNaiveMeshedFace(face, **faces.Find(voxelId.VoxelId));
					}
				}
			}
		}	
	}

	auto voxelIdsInMesh = TArray<int32>();
	faces.GetKeys(voxelIdsInMesh);
	
	for (auto voxelId : voxelIdsInMesh)
	{
		int32 index = 0;
	
		TSharedPtr<TArray<FVector>> Vertice = MakeShared<TArray<FVector>>();

		TSharedPtr<TArray<int32>> Triangles = MakeShared<TArray<int32>>();
		
		FVoxelType voxelType = ChunkGridData->GetVoxelTypeById(voxelId);

		for (auto Face : **faces.Find(voxelId))
		{
			auto voxelSize = ChunkSettings->GetVoxelSize();

			Face *= voxelSize;
			
			Vertice->Push(Face.BeginVertexDown);
			Vertice->Push(Face.BeginVertexUp);
			Vertice->Push(Face.EndVertexDown);
			Vertice->Push(Face.EndVertexUp);
			
			Triangles->Add(index); Triangles->Add(index + 1); Triangles->Add(index + 2);
			Triangles->Add(index + 2); Triangles->Add(index + 3); Triangles->Add(index);

			index += 4;
		}
	
		AsyncTask(ENamedThreads::GameThread, [this, voxelId, Vertice, Triangles, voxelType]()
		{

			auto procMesh = ChunkActor->GetProceduralMeshComponent();

		 if(procMesh.IsValid() && procMesh.IsValid(false,true) &&  Vertice.IsValid() && Triangles.IsValid()){
			 procMesh->CreateMeshSection_LinearColor(voxelId,*Vertice, *Triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true);
		 	procMesh->SetMaterial(voxelId, voxelType.Material);
		 	procMesh->SetMeshSectionVisible(voxelId, true);
		 }});
	}
}

void UDefaultChunk::GenerateVoxels()
{
	auto chunkLenght = ChunkSettings->GetChunkSideSizeInVoxels();
	auto maxElevation = ChunkSettings->GetMaximumElevation();;

	auto gridPos = ChunkGridPos * chunkLenght;
	int voxelIds = ChunkGridData->GetVoxelIdCount(); 
	
	for(int voxelId = 0; voxelId < voxelIds; voxelId++)
	{
		FVoxelType voxelType = ChunkGridData->GetVoxelTypeById(voxelId);
		
		auto voxel = FVoxel(voxelId);
		
		Noise->SetupFastNoise(EFastNoise_NoiseType::ValueFractal, voxelType.Seed,  voxelType.NoiseFrequency);
		
		for (int x = 0; x < chunkLenght; x++) 
		{
			for (int y = 0; y < chunkLenght; y++) 
			{
				float elevation = Noise->GetNoise2D(x + gridPos.X, y + gridPos.Y) * maxElevation;
			
				for (int z = 0; z < chunkLenght; z++) 
				{
					int32 index = ChunkSettings->GetVoxelIndex(x, y, z);
				
					if ((gridPos.Z + z) * ChunkSettings->GetVoxelSize() <= elevation) 
					{
						Voxels[index] = voxel;
						IsEmpty = false;
				}
			}
		}
	}
	}
}

FVoxel UDefaultChunk::VoxelAt(int32 index)
{
	return Voxels[index];
}
