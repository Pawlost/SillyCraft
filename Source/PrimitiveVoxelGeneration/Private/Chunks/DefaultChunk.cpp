// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunks/DefaultChunk.h"

#include "FastNoiseWrapper.h"
#include "Chunks/ChunkActor.h"
#include "Chunks/ChunkGridData.h"
#include "Chunks/ChunkSettings.h"
#include "Chunks/MeshingStructs/StaticNaiveMeshingData.h"
#include "Voxels/Voxel.h"
#include "Voxels/VoxelType.h"

void UDefaultChunk::AddToGrid(const TWeakObjectPtr<UChunkGridData> chunkGridData, FIntVector& chunkGridPos)
{
	ChunkSettings = chunkGridData->GetChunkSettings();
	ChunkLenght = ChunkSettings->GetChunkSideSizeInVoxels();
	Voxels.SetNum(ChunkLenght * ChunkSettings->GetChunkPlaneSizeInVoxels());
	UpdateAllFacesParams();
	Super::AddToGrid(chunkGridData, chunkGridPos);
}

void UDefaultChunk::UpdateAllFacesParams()
{
	//Axis X
	UpdateFaceParams(FrontFaceTemplate, FIntVector(-1,0,0),
	FIntVector(ChunkLenght - 1, 0, 0),
	FIntVector(0,-1,0));
	
	UpdateFaceParams(BackFaceTemplate, FIntVector(1,0,0),
	FIntVector(0, 0, 0),
	FIntVector(0,-1,0));

	//Axis Y
	UpdateFaceParams(RightFaceTemplate, FIntVector(0,-1,0),
	FIntVector(0, ChunkLenght - 1, 0),
	FIntVector(-1,0,0));
	
	UpdateFaceParams(LeftFaceTemplate, FIntVector(0,1,0),
	FIntVector(0, 0, 0),
	FIntVector(-1,0,0));
	
	// Axis Z
	UpdateFaceParams(BottomFaceTemplate, FIntVector(0,0,-1),
	FIntVector(0, 0, ChunkLenght - 1),
	FIntVector(0,-1,0));

	UpdateFaceParams(TopFaceTemplate, FIntVector(0,0,1),
	FIntVector(0, 0, 0),
	FIntVector(0,-1,0));
}

void UDefaultChunk::UpdateFaceParams(FNaiveMeshingData& face, FIntVector forwardVoxelIndexVector,
	FIntVector chunkBorderIndexVector,
	FIntVector previousVoxelIndexVector) const
{
	face.ForwardVoxelIndex = ChunkSettings->GetVoxelIndex(forwardVoxelIndexVector);
	face.PreviousVoxelIndex = ChunkSettings->GetVoxelIndex(previousVoxelIndexVector);
	face.ChunkBorderIndex = ChunkSettings->GetVoxelIndex(chunkBorderIndexVector);
}

UDefaultChunk::UDefaultChunk()
{
	Noise = CreateDefaultSubobject<UFastNoiseWrapper>("NoiseGenerator");
}

void UDefaultChunk::GenerateMesh()
{
	if(Empty)
	{
		return;
	}

#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Mesh generation")
#endif
	
	TSharedPtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>> faces[FACE_COUNT];

	InitFaceContainers(faces);
	FaceGeneration(faces);
	GreedyMeshAllFaces(faces);
	GenerateMeshFromFaces(faces);
}

void UDefaultChunk::InitFaceContainers(TSharedPtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces)
{
	int32 chunkPlane = ChunkSettings->GetChunkPlaneSizeInVoxels();
	for(int i = 0; i < FACE_COUNT; i++)
	{
		faces[i] = MakeShared<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>();
		
		for (auto voxelId : voxelIdsInMesh)
		{
			if(!faces[i]->Contains(voxelId))
			{
				auto faceArray = MakeShared<TArray<FChunkFace>>();
				faceArray->Reserve(chunkPlane);
				faces[i]->Add(voxelId, faceArray);
			}
		}
	}
}

void UDefaultChunk::FaceGeneration(const TSharedPtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Naive greedy mesh generation")
#endif
	
	// culling and naive greedy meshing
	for(int x = 0; x < ChunkLenght; x++)
	{
		auto minBorder = IsMinBorder(x);
		auto maxBorder = IsMaxBorder(x);
		
		auto xAxisIndex = ChunkSettings->GetVoxelIndex(x, 0, 0);
		auto yAxisIndex = ChunkSettings->GetVoxelIndex(0, x, 0);
		auto zAxisIndex = ChunkSettings->GetVoxelIndex(0, 0, x);
		
		for(int z = 0; z < ChunkLenght; z++)
		{
			for(int y = 0; y < ChunkLenght; y++)
			{
				GenerateFacesInAxis(x, y, z, xAxisIndex, minBorder, maxBorder, FrontFaceTemplate, BackFaceTemplate, faces[0].ToSharedRef(), faces[1].ToSharedRef());
				GenerateFacesInAxis(y, x, z, yAxisIndex, minBorder, maxBorder, RightFaceTemplate, LeftFaceTemplate, faces[2].ToSharedRef(), faces[3].ToSharedRef());
				GenerateFacesInAxis(z, y, x, zAxisIndex, minBorder, maxBorder, BottomFaceTemplate, TopFaceTemplate, faces[4].ToSharedRef(), faces[5].ToSharedRef());
			}
		}
	}
}

void UDefaultChunk::GenerateFacesInAxis(int x, int y, int z, int32 axisVoxelIndex, bool isMinBorder, bool isMaxBorder,
                                        const FNaiveMeshingData& faceTemplate, const FNaiveMeshingData& reversedFaceTemplate,
                                        const TSharedRef<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>& faceContainer,
                                        const TSharedRef<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>& reversedFaceContainer)
{
	auto index = ChunkSettings->GetVoxelIndex(x, y, z);
	auto voxel = Voxels[index];
				
	if(!voxel.IsEmptyVoxel())
	{
#if CPUPROFILERTRACE_ENABLED
		TRACE_CPUPROFILER_EVENT_SCOPE("Face and Reverse face generation")
	#endif
		
		auto position = FIntVector(x, y, z);
		CreateFace(faceTemplate, isMinBorder, index, position, voxel, axisVoxelIndex, faceContainer);
		CreateFace(reversedFaceTemplate, isMaxBorder, index, position, voxel, axisVoxelIndex, reversedFaceContainer);
	}
}

void UDefaultChunk::CreateFace(const FNaiveMeshingData& faceTemplate, bool isBorder,
	const int32& index, const FIntVector& position, const FVoxel& voxel, const int32& axisVoxelIndex,
	const TSharedRef<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>& faceContainer)
{
	auto chunkFaces =  faceContainer->Find(voxel.VoxelId)->ToSharedRef();

	VoxelIndexParams voxelIndexParams =
	{
		isBorder,
		faceTemplate.ForwardVoxelIndex + index,
		faceTemplate.PreviousVoxelIndex + index,
		index - axisVoxelIndex + faceTemplate.ChunkBorderIndex,
		faceTemplate.StaticMeshingData.borderChunkDirection
	};

	if((IsBorderVoxelVisible(voxelIndexParams) || IsVoxelVisible(voxelIndexParams)))
	{
		FChunkFace newFace =
		{
			voxel,
			faceTemplate.StaticMeshingData.faceTemplate.StartVertexDown + position,
			faceTemplate.StaticMeshingData.faceTemplate.EndVertexDown + position,
			faceTemplate.StaticMeshingData.faceTemplate.EndVertexUp + position,
			faceTemplate.StaticMeshingData.faceTemplate.StartVertexUp + position,
		};

		if(!chunkFaces->IsEmpty())
		{
			FChunkFace& prevFace = chunkFaces->Last();
			if(voxel == prevFace.Voxel && faceTemplate.StaticMeshingData.naiveFaceMerge(prevFace,newFace)){
				//return when new face was merged
				return;
			}
		}

		chunkFaces->Push(newFace);
	}
}

bool UDefaultChunk::IsBorderVoxelVisible(const VoxelIndexParams& faceData) const
{
	if(faceData.isBorder)
	{
		FIntVector neighborChunkCoords = ChunkGridPos + faceData.borderChunkDirection;
		auto chunk = ChunkGridData->GetChunkPtr(neighborChunkCoords);
		return chunk != nullptr && (VisibleChunkBorders && !chunk->HasMesh() || chunk->VoxelAt(faceData.currentVoxelIndex).IsEmptyVoxel());
	}
	return false;
}

bool UDefaultChunk::IsVoxelVisible(const VoxelIndexParams& faceData)
{
	return !faceData.isBorder && Voxels.IsValidIndex(faceData.forwardVoxelIndex) && Voxels[faceData.forwardVoxelIndex].IsEmptyVoxel();
}


void UDefaultChunk::GreedyMeshAllFaces(const TSharedPtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Greedy mesh generation")
#endif
	for (auto voxelId : voxelIdsInMesh)
	{
		// Front
		GreedyMeshing(voxelId,*faces[0], FChunkFace::MergeFaceDown);
		
		// Back
		GreedyMeshing(voxelId,*faces[1], FChunkFace::MergeFaceDown);
	
		// Right
		GreedyMeshing(voxelId,*faces[2],FChunkFace::MergeFaceDown);

		// Left
		GreedyMeshing(voxelId,*faces[3],FChunkFace::MergeFaceDown);

		// Bottom
		GreedyMeshing(voxelId,*faces[4], FChunkFace::MergeFaceDown);

		// Top
		GreedyMeshing(voxelId,*faces[5],FChunkFace::MergeFaceDown);
	}
}

void UDefaultChunk::GreedyMeshing(int32 voxelId, TMap<int32, TSharedPtr<TArray<FChunkFace>>>& faces,
                                  const TFunctionRef<bool(FChunkFace& prevFace, const FChunkFace& newFace)>& mergeFaces)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Face greedy mesh generation")
#endif
	
	auto voxelFaces = faces.Find(voxelId);

	if(voxelFaces == nullptr || (*voxelFaces)->Num() <= 1)
	{
		return;
	}

	auto voxelFacesArray = voxelFaces->ToSharedRef();
	auto faceArraySize = voxelFacesArray->Num();
	
	auto meshedFaces = MakeShared<TArray<FChunkFace>>();
	meshedFaces->Reserve(faceArraySize);
		
	for (int i = 1; i < faceArraySize; i++)
	{
		FChunkFace& prevFace = (*voxelFacesArray)[i-1];
		FChunkFace& nextFace = (*voxelFacesArray)[i];

		if(!mergeFaces(nextFace, prevFace))
		{
			meshedFaces->Push(prevFace);
		}
	}

	meshedFaces->Push(voxelFacesArray->Last());

	faces[voxelId] = meshedFaces;
}


void UDefaultChunk::GenerateMeshFromFaces(const TSharedPtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Mesh section generation")
#endif
	
	for (auto voxelId : voxelIdsInMesh)
	{
		int32 faceIndex = 0;
	
		TSharedPtr<TArray<FVector>> Vertice = MakeShared<TArray<FVector>>();
		TSharedPtr<TArray<FVector2D>> UVs = MakeShared<TArray<FVector2D>>();
		TSharedPtr<TArray<int32>> Indice = MakeShared<TArray<int32>>();
		
		FVoxelType voxelType = ChunkGridData->GetVoxelTypeById(voxelId);

		for(int i = 0; i < FACE_COUNT; i++)
		{
			auto sideFaces = faces[i]->Find(voxelId);
			
			if(sideFaces == nullptr)
			{
				continue;
			}
	
			for (auto Face : *sideFaces->Get())
			{
				auto voxelSize = ChunkSettings->GetVoxelSize();

				Vertice->Push(Face.GetFinalStartVertexDown(voxelSize));
				Vertice->Push(Face.GetFinalEndVertexDown(voxelSize));
				Vertice->Push(Face.GetFinalEndVertexUp(voxelSize));
				Vertice->Push(Face.GetFinalStartVertexUp(voxelSize));

				UVs->Emplace(0, 0);
				UVs->Emplace(1, 0);
				UVs->Emplace(1, 1);
				UVs->Emplace(0, 1);
				
				Indice->Append({faceIndex, faceIndex + 1, faceIndex + 2, faceIndex + 2, faceIndex + 3, faceIndex});
				faceIndex += 4;
			}
		}

		AsyncTask(ENamedThreads::GameThread, [this, voxelId, Vertice, Indice, UVs, voxelType]()
		{
#if CPUPROFILERTRACE_ENABLED
			TRACE_CPUPROFILER_EVENT_SCOPE("Procedural mesh generation")
#endif
			auto procMesh = ChunkActor->GetProceduralMeshComponent();

		 if(procMesh.IsValid() && procMesh.IsValid(false,true) &&  Vertice.IsValid() && Indice.IsValid()){
			 procMesh->CreateMeshSection_LinearColor(voxelId,*Vertice, *Indice, TArray<FVector>(), *UVs, TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true);
			 procMesh->SetMaterial(voxelId, voxelType.Material);
			 procMesh->SetMeshSectionVisible(voxelId, true);
		 }});
	}
}

bool UDefaultChunk::IsMinBorder(const int x)
{
	return x == 0;
}

bool UDefaultChunk::IsMaxBorder(const int x) const
{
	return x == ChunkLenght - 1;
}

void UDefaultChunk::GenerateVoxels()
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Voxel generation")
#endif
	
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
						voxelIdsInMesh.FindOrAdd(voxel.VoxelId);
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