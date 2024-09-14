// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunks/DefaultChunk.h"

#include "FastNoiseWrapper.h"
#include "Chunks/ChunkSettings.h"
#include "Voxels/ChunkFace.h"

UDefaultChunk::UDefaultChunk()
{
	Noise = CreateDefaultSubobject<UFastNoiseWrapper>("NoiseGenerator");
}

void UDefaultChunk::AddNaiveMeshedFace(int32 frontVoxelPos,
	FChunkFace& face, TArray<FChunkFace>& faces)
{

	int32 forwardVoxelId = 0;
	if(Voxels.IsValidIndex(frontVoxelPos))
	{
		 forwardVoxelId = Voxels[frontVoxelPos];
	}
	
	// Culling
	if(forwardVoxelId != 0)
	{
		return;
	}
	
	faces.Add(face);


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

bool UDefaultChunk::CheckBorderMin(int32 min, int32 forwardChunkIndex)
{
	// TODO: cross chunk
	return min > 0; //|| otherChunk != 0;
}

bool UDefaultChunk::CheckBorderMax(int32 max, int32 forwardChunkIndex)
{
	// TODO: cross chunk
	auto chunkSize = ChunkSettings->GetChunkSideSizeInVoxels();
	return max < chunkSize - 1;
}

void UDefaultChunk::GenerateMesh(UProceduralMeshComponent* procMesh, FIntVector& chunkGridPos)
{
	auto chunkWidth = ChunkSettings->GetChunkSideSizeInVoxels();
	constexpr auto faceNumber = 6;
	
	TUniquePtr<TArray<FChunkFace>> faces[faceNumber];

	for(int i = 0; i < faceNumber; i++)
	{
		faces[i] = MakeUnique<TArray<FChunkFace>>();
	}
	
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
					FVector position = FVector(x, y, z);

					FChunkFace face;

					face = FChunkFace::FrontFace;
					face += position;
					// Front
					if(CheckBorderMin(x, 0))
					{
						AddNaiveMeshedFace(ChunkSettings->GetVoxelIndex(x-1,y,z), face, *faces[0]);
					}
					else
					{
						faces[0]->Add(face);
					}
					
					// Back
					face = FChunkFace::BackFace;
					face += position;
					if(CheckBorderMax(x, 0))
					{
						AddNaiveMeshedFace(ChunkSettings->GetVoxelIndex(x+1,y,z), face, *faces[1]);
					}
					else
					{
						faces[1]->Add(face);
					}
					
					// Left
					face = FChunkFace::LeftFace;
					face += position;
					if(CheckBorderMin(y, 0))
					{
						AddNaiveMeshedFace(ChunkSettings->GetVoxelIndex(x,y-1,z), face, *faces[2]);
					}
					else
					{
						faces[2]->Add(face);
					}
					
					// Right
					face = FChunkFace::RightFace;
					face += position;
					if(CheckBorderMax(y, 0))
					{
						AddNaiveMeshedFace(ChunkSettings->GetVoxelIndex(x,y+1,z), face, *faces[3]);
					}
					else
					{
						faces[3]->Add(face);
					}
					
					// Top
					face = FChunkFace::TopFace;
					face += position;
					if(CheckBorderMax(z, 0))
					{
						AddNaiveMeshedFace(ChunkSettings->GetVoxelIndex(x,y,z + 1), face, *faces[4]);
					}
					else
					{
						faces[4]->Add(face);
					}

					// Bottom
					face = FChunkFace::BackFace;
					face += position;
					if(CheckBorderMin(z, 0))
					{
						AddNaiveMeshedFace(ChunkSettings->GetVoxelIndex(x,y,z-1), face, *faces[5]);
					}
					else
					{
						faces[5]->Add(face);
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

	AsyncTask(ENamedThreads::GameThread, [this, procMesh, Vertice, Triangles, Colors]()
	{
	 if(IsValid(procMesh) &&  Vertice.IsValid() && Triangles.IsValid() && Colors.IsValid()){
			procMesh->CreateMeshSection_LinearColor(0,*Vertice.Get(), *Triangles.Get(), TArray<FVector>(), TArray<FVector2D>(),*Colors.Get(), TArray<FProcMeshTangent>(), true);
			procMesh->SetMeshSectionVisible(0, true);
		}
	});
}

void UDefaultChunk::GenerateVoxels(FIntVector& chunkGridPos)
{
	Noise->SetSeed(ChunkSettings->GetSeed());
	Noise->SetupFastNoise(EFastNoise_NoiseType::ValueFractal, ChunkSettings->GetSeed(),  ChunkSettings->GetNoiseFrequency());
	
	auto chunkSize = ChunkSettings->GetChunkSideSizeInVoxels();
	auto maxElevation = ChunkSettings->GetMaximumElevation();;

	auto gridPos = chunkGridPos * chunkSize;
	
	for (int x = 0; x < chunkSize; x++) 
	{
		for (int y = 0; y < chunkSize; y++) 
		{
			float elevation = Noise->GetNoise2D(x + gridPos.X, y + gridPos.Y) * maxElevation;
			
			for (int z = 0; z < chunkSize; z++) 
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

void UDefaultChunk::SetChunkSettings(const TSharedPtr<FChunkSettings> chunkSettings)
{
	Voxels.SetNum(chunkSettings->GetChunkSideSizeInVoxels() * chunkSettings->GetChunkPlaneSizeInVoxels());
	Super::SetChunkSettings(chunkSettings);
}

int32 UDefaultChunk::VoxelAt(int32 index)
{
	return Voxels[index];
}
