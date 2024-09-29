// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunks/DefaultChunk.h"

#include "FastNoiseWrapper.h"
#include "Chunks/ChunkActor.h"
#include "Chunks/ChunkGridData.h"
#include "Chunks/ChunkSettings.h"
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

void UDefaultChunk::AddNaiveMeshedFace(FChunkFace& face,
	TMap<int32, TArray<FChunkFace>>& faces, int32 previousVoxelDirection,
	FChunkFace::EMergeMethod mergeMethod, FChunkFace::EUnstableAxis unstableAxis)
{
	if(!faces.Contains(face.Voxel.VoxelId))
	{
		faces.Add(face.Voxel.VoxelId, TArray<FChunkFace>());
	}
	
	auto chunkFace = faces.Find(face.Voxel.VoxelId);

	if(Voxels.IsValidIndex(previousVoxelDirection) &&
		face.Voxel == Voxels[previousVoxelDirection] &&
		!chunkFace->IsEmpty())
	{
		const auto prevface = chunkFace->Last();
		
		if(face.MergeFace(prevface, mergeMethod, unstableAxis))
		{
		//TODO: merge prevface
			chunkFace->Pop();
		}
	}
	
	chunkFace->Add(face);
}

bool UDefaultChunk::ChunkCull(int32 chunkIndex, FIntVector& neighborChunkDistance) const
{
	FIntVector neighborChunkCoords = ChunkGridPos + neighborChunkDistance;
	auto chunk = ChunkGridData->GetChunkPtr(neighborChunkCoords);
	return chunk != nullptr && (VisibleChunkBorders && !chunk->HasMesh() || chunk->VoxelAt(chunkIndex).IsEmptyVoxel());
}

bool UDefaultChunk::VoxelCull(int32 forwardVoxelIndex)
{
	return Voxels.IsValidIndex(forwardVoxelIndex) && Voxels[forwardVoxelIndex].IsEmptyVoxel();
}

bool UDefaultChunk::CrossChunkCullInNegativeDirection(int min, int32 forwardVoxelIndex, int32 chunkIndex, FIntVector neighborChunkDistance)
{
	// min == 0 is first face voxel in chunk, needs to be always shown or cross chunk compared
	if(min == 0)
	{
		return ChunkCull(chunkIndex, neighborChunkDistance);
	}
	
	return VoxelCull(forwardVoxelIndex);
}

bool UDefaultChunk::CrossChunkCullInPositiveDirection(int max, int32 forwardVoxelIndex, int32 chunkIndex,
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
	if(Empty)
	{
		return;
	}
	
	auto chunkLenght = ChunkSettings->GetChunkSideSizeInVoxels();
	
	auto voxelIdsInMesh = TSet<int32>();

	constexpr int faceNumber = 6;
	TUniquePtr<TMap<int32, TArray<FChunkFace>>> faces[faceNumber];

	for(int i = 0; i < faceNumber; i++)
	{
		faces[i] = MakeUnique<TMap<int32, TArray<FChunkFace>>>();
	}

	FChunkFace face;
	FIntVector position;
	int32 index;
	FVoxel voxel;
	
	for(int x = 0; x < chunkLenght; x++)
	{
		for(int z = 0; z < chunkLenght; z++)
		{
			for(int y = 0; y < chunkLenght; y++)
			{
				index = ChunkSettings->GetVoxelIndex(x, y, z);
				voxel = Voxels[index];
				
				if(!voxel.IsEmptyVoxel())
				{
					voxelIdsInMesh.FindOrAdd(voxel.VoxelId);
					position = FIntVector(x, y, z);
					
					// Front
					if(CrossChunkCullInNegativeDirection(x, index + ChunkSettings->GetVoxelIndex(-1,0,0),
						ChunkSettings->GetVoxelIndex(chunkLenght - 1, y, z), FIntVector(-1, 0, 0)))
					{
						face = FChunkFace::CreateFrontFace(position, voxel);
						AddNaiveMeshedFace(face, *faces[0], index + ChunkSettings->GetVoxelIndex(0,-1,0), FChunkFace::EMergeMethod::Begin,  FChunkFace::EUnstableAxis::Y);
					}
						
					// Back
					if(CrossChunkCullInPositiveDirection(x, index + ChunkSettings->GetVoxelIndex(1,0,0),
						ChunkSettings->GetVoxelIndex(0, y, z), FIntVector(1, 0, 0)))
					{
						face = FChunkFace::CreateBackFace(position, voxel);
						AddNaiveMeshedFace(face, *faces[1], index + ChunkSettings->GetVoxelIndex(0,-1,0), FChunkFace::EMergeMethod::End,  FChunkFace::EUnstableAxis::Y);
					}
				
					// Bottom
					if(CrossChunkCullInNegativeDirection(z, index + ChunkSettings->GetVoxelIndex(0,0,-1),
						ChunkSettings->GetVoxelIndex(x, y, chunkLenght - 1), FIntVector(0, 0, -1)))
					{
						face = FChunkFace::CreateBottomFace(position, voxel);
						AddNaiveMeshedFace(face, *faces[4], index + ChunkSettings->GetVoxelIndex(0,-1,0), FChunkFace::EMergeMethod::End,  FChunkFace::EUnstableAxis::Y);
					}
			
					// Top
					if(CrossChunkCullInPositiveDirection(z, index + ChunkSettings->GetVoxelIndex(0,0,1),
						ChunkSettings->GetVoxelIndex(x, y,0), FIntVector(0, 0, 1)))
					{
						face = FChunkFace::CreateTopFace(position, voxel);
						AddNaiveMeshedFace(face, *faces[5], index + ChunkSettings->GetVoxelIndex(0,-1,0), FChunkFace::EMergeMethod::Begin,  FChunkFace::EUnstableAxis::Y);
					}
				}
				
				index = ChunkSettings->GetVoxelIndex(y, x, z);
				voxel = Voxels[index];
				
				if(!voxel.IsEmptyVoxel())
				{
					voxelIdsInMesh.FindOrAdd(voxel.VoxelId);
					
					position = FIntVector(y, x, z);

					// Right
					if(CrossChunkCullInNegativeDirection(x, index + ChunkSettings->GetVoxelIndex(0,-1,0),
						ChunkSettings->GetVoxelIndex(y, chunkLenght-1, z), FIntVector(0, -1, 0)))
					{
						face = FChunkFace::CreateRightFace(position, voxel);
						AddNaiveMeshedFace(face, *faces[3], index + ChunkSettings->GetVoxelIndex(-1,0,0), FChunkFace::EMergeMethod::End, FChunkFace::EUnstableAxis::X);
					}
					
					// Left
					if(CrossChunkCullInPositiveDirection(x, index + ChunkSettings->GetVoxelIndex(0,1,0),
						ChunkSettings->GetVoxelIndex(y,0, z), FIntVector(0, 1, 0)))
					{
						face = FChunkFace::CreateLeftFace(position, voxel);
						AddNaiveMeshedFace(face, *faces[2], index + ChunkSettings->GetVoxelIndex(-1,0,0), FChunkFace::EMergeMethod::Begin,  FChunkFace::EUnstableAxis::X);
					}
				}
			}
		}
	}
	
	for (auto voxelId : voxelIdsInMesh)
	{
		int32 faceIndex = 0;
	
		TSharedPtr<TArray<FVector>> Vertice = MakeShared<TArray<FVector>>();
		TSharedPtr<TArray<FVector2D>> UVs = MakeShared<TArray<FVector2D>>();
		TSharedPtr<TArray<int32>> Triangles = MakeShared<TArray<int32>>();
		
		FVoxelType voxelType = ChunkGridData->GetVoxelTypeById(voxelId);

		for(int i = 0; i < faceNumber; i++)
		{
			auto sideFaces = faces[i]->Find(voxelId);
			
			if(sideFaces == nullptr)
			{
				continue;
			}
	
			for (auto Face : *sideFaces)
			{
				auto voxelSize = ChunkSettings->GetVoxelSize();

				Face *= voxelSize;
			
				Vertice->Push(static_cast<FVector>(Face.BeginVertexDown));
				Vertice->Push(static_cast<FVector>(Face.EndVertexDown));
				Vertice->Push(static_cast<FVector>(Face.EndVertexUp));
				Vertice->Push(static_cast<FVector>(Face.BeginVertexUp));

				UVs->Add(FVector2D(0, 0));
				UVs->Add(FVector2D(1, 0));
				UVs->Add(FVector2D(1, 1));
				UVs->Add(FVector2D(0, 1));
				
				Triangles->Add(faceIndex); Triangles->Add(faceIndex + 1); Triangles->Add(faceIndex + 2);
				Triangles->Add(faceIndex + 2); Triangles->Add(faceIndex + 3); Triangles->Add(faceIndex);

				faceIndex += 4;
			}
		}

		AsyncTask(ENamedThreads::GameThread, [this, voxelId, Vertice, Triangles, UVs, voxelType]()
		{

			auto procMesh = ChunkActor->GetProceduralMeshComponent();

		 if(procMesh.IsValid() && procMesh.IsValid(false,true) &&  Vertice.IsValid() && Triangles.IsValid()){
			 procMesh->CreateMeshSection_LinearColor(voxelId,*Vertice, *Triangles, TArray<FVector>(), *UVs, TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true);
		 	procMesh->SetMaterial(voxelId, voxelType.Material);
		 	procMesh->SetMeshSectionVisible(voxelId, true);
		 }});
	}
}

void UDefaultChunk::GenerateVoxels()
{
	auto chunkLenght = ChunkSettings->GetChunkSideSizeInVoxels();
	auto maxElevation = ChunkSettings->GetMaximumElevation();

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
				auto elevation = Noise->GetNoise2D(x + gridPos.X,  y + gridPos.Y) * maxElevation;
				
				for (int z = 0; z < chunkLenght; z++) 
				{
					auto index = ChunkSettings->GetVoxelIndex(x, y, z);
					
					if (gridPos.Z + z <= elevation) 
					{
						Voxels[index] = voxel;
						Empty = false;
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

double UDefaultChunk::GetHighestElevationAtPosition(double posX, double posY)
{
	double maxElevation = 0.0;

	auto voxelSize = ChunkSettings->GetVoxelSize();
	
	int voxelIds = ChunkGridData->GetVoxelIdCount();

	auto voxelPosX = posX / voxelSize;
	auto voxelPosY = posY / voxelSize;
	
	for(int voxelId = 0; voxelId < voxelIds; voxelId++)
	{
		FVoxelType voxelType = ChunkGridData->GetVoxelTypeById(voxelId);
		Noise->SetupFastNoise(EFastNoise_NoiseType::ValueFractal, voxelType.Seed,  voxelType.NoiseFrequency);
		double elevation = Noise->GetNoise2D(voxelPosX, voxelPosY) * ChunkSettings->GetMaximumElevation();

		if(elevation > maxElevation)
		{
			maxElevation = elevation;
		}
	}
	
	return maxElevation * voxelSize;
}