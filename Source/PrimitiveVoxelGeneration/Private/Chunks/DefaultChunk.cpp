// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunks/DefaultChunk.h"

#include "FastNoiseWrapper.h"
#include "Chunks/ChunkActor.h"
#include "Chunks/ChunkGridData.h"
#include "Chunks/ChunkSettings.h"
#include "Voxels/ChunkFace.h"

void UDefaultChunk::AddToGrid(const TSharedPtr<FChunkGridData> chunkGridData, FIntVector& chunkGridPos)
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

bool UDefaultChunk::ChunkCull(int32 chunkIndex, FIntVector& neighborChunkCoords) const
{
	auto chunk = ChunkGridData->GetChunkPtr(neighborChunkCoords);
	return chunk != nullptr && chunk->VoxelAt(chunkIndex) == 0;
}

bool UDefaultChunk::VoxelCull(int32 forwardVoxelIndex)
{
	return Voxels.IsValidIndex(forwardVoxelIndex) && Voxels[forwardVoxelIndex] == 0;
}

bool UDefaultChunk::CrossChunkCullMin(int min, int32 forwardVoxelIndex, int32 chunkIndex, FIntVector& neighborChunkCoords)
{
	// min == 0 is first face voxel in chunk, needs to be always shown or cross chunk compared
	if(min == 0)
	{
		return ChunkCull(chunkIndex, neighborChunkCoords);
	}
	
	return VoxelCull(forwardVoxelIndex);
}

bool UDefaultChunk::CrossChunkCullMax(int max, int32 forwardVoxelIndex, int32 chunkIndex,
	FIntVector& neighborChunkCoords)
{
	auto chunkSize = ChunkSettings->GetChunkSideSizeInVoxels();

	if(max == chunkSize - 1)
	{
		return ChunkCull(chunkIndex, neighborChunkCoords);
	}

	// chunkSize == 0 is last face voxel in chunk, needs to be always shown or cross chunk compared
	return VoxelCull(forwardVoxelIndex);
}

void UDefaultChunk::GenerateMesh()
{
	auto chunkLenght = ChunkSettings->GetChunkSideSizeInVoxels();
	constexpr auto faceNumber = 6;
	
	TUniquePtr<TArray<FChunkFace>> faces[faceNumber];

	for(int i = 0; i < faceNumber; i++)
	{
		faces[i] = MakeUnique<TArray<FChunkFace>>();
	}
	
	for(int x = 0; x < chunkLenght; x++)
	{
		for(int y = 0; y < chunkLenght; y++)
		{
			for(int z = 0; z < chunkLenght; z++)
			{
				int32 index = ChunkSettings->GetVoxelIndex(x, y, z);

				auto voxelId = Voxels[index];

				if(voxelId != 0)
				{
					FVector position = FVector(x, y, z);

					FChunkFace face;
					FIntVector neighborChunkCoords;
			
					// Front
					face = FChunkFace::FrontFace;
					face += position;
					
					neighborChunkCoords = ChunkGridPos + FIntVector(-1, 0, 0);
					if(CrossChunkCullMin(x, index + ChunkSettings->GetVoxelIndex(-1,0,0),
						ChunkSettings->GetVoxelIndex(chunkLenght - 1, y, z), neighborChunkCoords))
					{
						AddNaiveMeshedFace(face, *faces[0]);
					}
					
					// Back
					face = FChunkFace::BackFace;
					face += position;
					
					neighborChunkCoords = ChunkGridPos + FIntVector(1, 0, 0);
					if(CrossChunkCullMax(x, index + ChunkSettings->GetVoxelIndex(1,0,0),
						ChunkSettings->GetVoxelIndex(0, y, z), neighborChunkCoords))
					{
						AddNaiveMeshedFace(face, *faces[1]);
					}
					
					// Left
					face = FChunkFace::LeftFace;
					face += position;
					
					neighborChunkCoords = ChunkGridPos + FIntVector(0, -1, 0);
					if(CrossChunkCullMin(y, index + ChunkSettings->GetVoxelIndex(0,-1,0),
						ChunkSettings->GetVoxelIndex(x, chunkLenght - 1, z), neighborChunkCoords))
					{
						AddNaiveMeshedFace(face, *faces[2]);
					}
					
					// Right
					face = FChunkFace::RightFace;
					face += position;
					
					neighborChunkCoords = ChunkGridPos + FIntVector(0, 1, 0);
					if(CrossChunkCullMax(y, index + ChunkSettings->GetVoxelIndex(0,1,0),
						ChunkSettings->GetVoxelIndex(x, 0,z ), neighborChunkCoords))
					{
						AddNaiveMeshedFace(face, *faces[3]);
					}

					// Bottom
					face = FChunkFace::BottomFace;
					face += position;
					
					neighborChunkCoords = ChunkGridPos + FIntVector(0, 0, -1);
					if(CrossChunkCullMin(z, index + ChunkSettings->GetVoxelIndex(0,0,-1),
						ChunkSettings->GetVoxelIndex(x, y, chunkLenght - 1), neighborChunkCoords))
					{
						AddNaiveMeshedFace(face, *faces[4]);
					}

					// Top
					face = FChunkFace::TopFace;
					face += position;
					
					neighborChunkCoords = ChunkGridPos + FIntVector(0, 0, 1);
					if(CrossChunkCullMax(z, index + ChunkSettings->GetVoxelIndex(0,0,1),
						ChunkSettings->GetVoxelIndex(x, y,0), neighborChunkCoords))
					{
						AddNaiveMeshedFace(face, *faces[5]);
					}
				}
			}
		}	
	}

	int32 index = 0;
	
	TSharedPtr<TArray<FVector>> Vertice = MakeShared<TArray<FVector>>();

	TSharedPtr<TArray<int32>> Triangles = MakeShared<TArray<int32>>();
	
	// Define the cube UV coordinates
	TSharedPtr<TArray<FVector2D>> UVs = MakeShared<TArray<FVector2D>>();

	for(int i = 0; i < faceNumber; i++){
		for (auto Face : *faces[i])
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
	}

	TSharedPtr<TArray<FLinearColor>> Colors = MakeShared<TArray<FLinearColor>>();

	
	AsyncTask(ENamedThreads::GameThread, [this, Vertice, Triangles, Colors]()
	{

		auto procMesh = ChunkActor->GetProceduralMeshComponent();

	 if(procMesh.IsValid(false,true) &&  Vertice.IsValid() && Triangles.IsValid() && Colors.IsValid()){
			procMesh->CreateMeshSection_LinearColor(0,*Vertice.Get(), *Triangles.Get(), TArray<FVector>(), TArray<FVector2D>(),*Colors.Get(), TArray<FProcMeshTangent>(), true);
			procMesh->SetMeshSectionVisible(0, true);
		}
	});
}

void UDefaultChunk::GenerateVoxels()
{
	Noise->SetSeed(ChunkSettings->GetSeed());
	Noise->SetupFastNoise(EFastNoise_NoiseType::ValueFractal, ChunkSettings->GetSeed(),  ChunkSettings->GetNoiseFrequency());
	
	auto chunkLenght = ChunkSettings->GetChunkSideSizeInVoxels();
	auto maxElevation = ChunkSettings->GetMaximumElevation();;

	auto gridPos = ChunkGridPos * chunkLenght;
	
	for (int x = 0; x < chunkLenght; x++) 
	{
		for (int y = 0; y < chunkLenght; y++) 
		{
			float elevation = Noise->GetNoise2D(x + gridPos.X, y + gridPos.Y) * maxElevation;
			
			for (int z = 0; z < chunkLenght; z++) 
			{
				int32 index = ChunkSettings->GetVoxelIndex(x, y, z);
				int32 voxelId = 0;
				
				if (gridPos.Z + z <= elevation) 
				{
					voxelId = 1;
				}

				Voxels[index] = voxelId;
			}
		}
	}
}

int32 UDefaultChunk::VoxelAt(int32 index)
{
	return Voxels[index];
}
