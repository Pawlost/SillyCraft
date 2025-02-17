// Fill out your copyright notice in the Description page of Project Settings.

#include "Meshers/RunLengthMesher.h"

#include "RealtimeMeshSimple.h"
#include "Mesh/RealtimeMeshBuilder.h"

const URunLengthMesher::FNormalsAndTangents URunLengthMesher::FaceNormalsAndTangents[] = {
	{ FVector3f(-1.0f, 0.0f, 0.0f),  FVector3f(0.0, 1.0, 0.0)},//Front
	{ FVector3f(1.0f, 0.0f, 0.0f),  FVector3f(0.0, 1.0, 0.0)},//Back
	{ FVector3f(0.0f, -1.0f, 0.0f),  FVector3f(1.0f, 0.0f, 0.0f)},// Right 
	{ FVector3f(0.0f, 1.0f, 0.0f),  FVector3f(1.0, 0.0, 0.0)},// Left
	{ FVector3f(0.0f, 0.0f, -1.0f),  FVector3f(1.0f, 0.0f, 0.0f)}, //Bottom
	{ FVector3f(0.0f, 0.0f, 1.0f),  FVector3f(1.0f, 0.0f, 0.0f)}//Top
};

void URunLengthMesher::UpdateAllFacesParams()
{
	auto chunkLenght = VoxelGenerator->GetVoxelCountY();
	//Axis X
	UpdateFaceParams(FrontFaceTemplate, FIntVector(-1,0,0),
	FIntVector(chunkLenght - 1, 0, 0),
	FIntVector(0,-1,0));
	
	UpdateFaceParams(BackFaceTemplate, FIntVector(1,0,0),
	FIntVector(0, 0, 0),
	FIntVector(0,-1,0));

	//Axis Y
	UpdateFaceParams(RightFaceTemplate, FIntVector(0,-1,0),
	FIntVector(0, chunkLenght - 1, 0),
	FIntVector(-1,0,0));
	
	UpdateFaceParams(LeftFaceTemplate, FIntVector(0,1,0),
	FIntVector(0, 0, 0),
	FIntVector(-1,0,0));
	
	// Axis Z
	UpdateFaceParams(BottomFaceTemplate, FIntVector(0,0,-1),
	FIntVector(0, 0, chunkLenght - 1),
	FIntVector(0,-1,0));

	UpdateFaceParams(TopFaceTemplate, FIntVector(0,0,1),
	FIntVector(0, 0, 0),
	FIntVector(0,-1,0));
}

void URunLengthMesher::UpdateFaceParams(FNaiveMeshingData& face, FIntVector forwardVoxelIndexVector,
	FIntVector chunkBorderIndexVector,
	FIntVector previousVoxelIndexVector) const
{
	face.ForwardVoxelIndex = VoxelGenerator->GetVoxelIndex(forwardVoxelIndexVector); 
	face.PreviousVoxelIndex = VoxelGenerator->GetVoxelIndex(previousVoxelIndexVector); 
	face.ChunkBorderIndex = VoxelGenerator->GetVoxelIndex(chunkBorderIndexVector);
}

void URunLengthMesher::GenerateMesh(FChunkStruct& chunk)
{
	if(chunk.IsEmpty)
	{
		return;
	}

#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Mesh generation")
#endif

	// Voxel faces need to be hidden under multiple layers of abstraction because Realtime Mesh Component requires it
	FChunkFaceParams faceParams;
	
	InitFaceContainers(faceParams);
	FaceGeneration(faceParams);
	DirectionalGreedyMeshing(faceParams);
	GenerateMeshFromFaces(faceParams);
}

void URunLengthMesher::InitFaceContainers(FChunkFaceParams& faceParams) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Mesh generation intialization")
#endif
	
	int32 chunkPlane = VoxelGenerator->GetVoxelCountXYZ();
	for(int f = 0; f < FACE_COUNT; f++)
	{
		faceParams.Faces[f].SetNum(faceParams.ChunkStruct.ChunkVoxelTypeTable.Num());
		for (auto voxelId : faceParams.ChunkStruct.ChunkVoxelTypeTable)
		{
			auto faceArray = MakeShared<TArray<FChunkFace>>();
			faceArray->Reserve(chunkPlane );
			faceParams.Faces[f][voxelId.Value] = faceArray;
		}
	}
}

void URunLengthMesher::FaceGeneration(FChunkFaceParams& faceParams) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Run lenght meshing generation")
#endif

	auto chunkLenght = VoxelGenerator->GetVoxelCountY();
	// culling and run meshing
	for(int32 x = 0; x < chunkLenght; x++)
	{
		auto minBorder = IsMinBorder(x);
		auto maxBorder = IsMaxBorder(x);
		
		auto xAxisIndex = VoxelGenerator->GetVoxelIndex(x, 0, 0);
		auto yAxisIndex = VoxelGenerator->GetVoxelIndex(0, x, 0);
		auto zAxisIndex = VoxelGenerator->GetVoxelIndex(0, 0, x);
		
		for(int32 z = 0; z < chunkLenght; z++)
		{
			for(int32 y = 0; y < chunkLenght; y++)
			{
				IncrementRun(x, y, z, xAxisIndex, minBorder, maxBorder, FrontFaceTemplate, BackFaceTemplate, FRONT_FACE_INDEX, BACK_FACE_INDEX, faceParams);
				IncrementRun(y, x, z, yAxisIndex, minBorder, maxBorder, RightFaceTemplate, LeftFaceTemplate, RIGHT_FACE_INDEX, LEFT_FACE_INDEX, faceParams);
				IncrementRun(z, y, x, zAxisIndex, minBorder, maxBorder, BottomFaceTemplate, TopFaceTemplate, BOTTOM_FACE_INDEX, TOP_FACE_INDEX, faceParams);
			}
		}
	}
}

void URunLengthMesher::IncrementRun(int x, int y, int z, int32 axisVoxelIndex, bool isMinBorder, bool isMaxBorder,
                                        const FNaiveMeshingData& faceTemplate, const FNaiveMeshingData& reversedFaceTemplate,
                                        const int32 faceContainerIndex,
                                        const int32 reversedFaceContainerIndex,
                                        FChunkFaceParams& chunkParams) const
{
	auto index = VoxelGenerator->GetVoxelIndex(x, y, z);
	auto voxel = chunkParams.ChunkStruct.Voxels[index];
				
	if(!voxel.IsEmptyVoxel())
	{
		auto position = FIntVector(x, y, z);
		AddFace(faceTemplate, isMinBorder, index, position, voxel, axisVoxelIndex, chunkParams.Faces[faceContainerIndex][voxel.VoxelId], chunkParams.ChunkStruct);
		AddFace(reversedFaceTemplate, isMaxBorder, index, position, voxel, axisVoxelIndex, chunkParams.Faces[reversedFaceContainerIndex][voxel.VoxelId], chunkParams.ChunkStruct);
	}
}

void URunLengthMesher::AddFace(const FNaiveMeshingData& faceTemplate, bool isBorder,
	const int32& index, const FIntVector& position, const FVoxel& voxel, const int32& axisVoxelIndex,
	const TSharedPtr<TArray<FChunkFace>>& chunkFaces, FChunkStruct& chunkStruct)
{
	FVoxelIndexParams voxelIndexParams =
	{
		isBorder,
		faceTemplate.ForwardVoxelIndex + index,
		faceTemplate.PreviousVoxelIndex + index,
		index - axisVoxelIndex + faceTemplate.ChunkBorderIndex,
		faceTemplate.StaticMeshingData.borderChunkDirection
	};

	if(IsBorderVoxelVisible(voxelIndexParams, chunkStruct) || IsVoxelVisible(voxelIndexParams, chunkStruct))
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
			if(faceTemplate.StaticMeshingData.naiveFaceMerge(prevFace,newFace)){
				//return when new face was merged
				return;
			}
		}

		chunkFaces->Push(newFace);
	}
}

bool URunLengthMesher::IsBorderVoxelVisible(const FVoxelIndexParams& faceData, FChunkStruct& chunkStruct)
{
	if(faceData.IsBorder)
	{
		FIntVector neighborChunkCoords = chunkStruct.GridPosition + faceData.BorderChunkDirection;
		// auto chunk = ChunkGridData->GetChunkPtr(neighborChunkCoords);
		// return chunk == nullptr || chunk->VoxelAt(faceData.CurrentVoxelIndex).IsEmptyVoxel();
	}
	return false;
}

bool URunLengthMesher::IsVoxelVisible(const FVoxelIndexParams& faceData, FChunkStruct& chunkStruct)
{
	return !faceData.IsBorder && chunkStruct.Voxels.IsValidIndex(faceData.ForwardVoxelIndex) && chunkStruct.Voxels[faceData.ForwardVoxelIndex].IsEmptyVoxel();
}

void URunLengthMesher::GenerateVoxels(FChunkStruct& chunk)
{
	VoxelGenerator->GenerateVoxels(chunk);
}

void URunLengthMesher::BeginPlay()
{
	if (VoxelGeneratorClass)
	{
		// Create the component dynamically
		VoxelGenerator = NewObject<UVoxelGeneratorBase>(this, VoxelGeneratorClass);
        
		if (VoxelGenerator)
		{
			VoxelGenerator->RegisterComponent();
		}
	}
	UpdateAllFacesParams();
	
	Super::BeginPlay();
}

void URunLengthMesher::DirectionalGreedyMeshing(const FChunkFaceParams& faceParams)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Run directional greedy mesh generation")
#endif
	
	for(int8 f = 0; f < FACE_COUNT; f++)
	{
		for (auto voxelId : faceParams.ChunkStruct.ChunkVoxelTypeTable)
		{
			auto faceContainer = faceParams.Faces[f][voxelId.Value];
			auto lastElementIndex = faceContainer->Num() - 1;

			for (int32 i = lastElementIndex - 1; i >= 0; i--)
			{
				FChunkFace& face = (*faceContainer)[i];
				FChunkFace& nextFace = (*faceContainer)[i + 1];

				if(FChunkFace::MergeFaceUp(face, nextFace))
				{
					faceContainer->RemoveAt(i + 1, EAllowShrinking::No);
				}
			}
		}
	}
}

void URunLengthMesher::GenerateMeshFromFaces(const FChunkFaceParams& faceParams) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Mesh stream generation")
#endif
	
	auto StreamSet = MakeShared<RealtimeMesh::FRealtimeMeshStreamSet>();

	RealtimeMesh::TRealtimeMeshBuilderLocal<int32> Builder(*StreamSet.ToWeakPtr().Pin());

	Builder.EnableTexCoords();
	Builder.EnableColors();
	Builder.EnableTangents();
	
	Builder.EnablePolyGroups();

	auto voxelSize = VoxelGenerator->GetVoxelSize();
	
	// Because of RealTimeMesh component voxelId needs to be first
	for (auto voxelId : faceParams.ChunkStruct.ChunkVoxelTypeTable)
	{
		for(int faceIndex = 0; faceIndex < FACE_COUNT; faceIndex++)
		{
			auto faceContainer = faceParams.Faces[faceIndex];

			auto voxelIndex = voxelId.Value;
			auto sideFaces = faceContainer[voxelIndex];

			auto faceNormalAndTangent = FaceNormalsAndTangents[faceIndex];
			for (auto Face : *sideFaces)
			{
				// Add our first vertex
				int32 V0 = Builder.AddVertex(Face.GetFinalStartVertexDown(voxelSize))
					.SetColor(FColor::White)
					.SetTexCoord(FVector2f(0, 0))
					.SetNormalAndTangent(faceNormalAndTangent.Normal, faceNormalAndTangent.Tangent);

				// Add our second vertex
				int32 V1 = Builder.AddVertex(Face.GetFinalEndVertexDown(voxelSize))
					.SetColor(FColor::White)
					.SetTexCoord(FVector2f(1, 0))
					.SetNormalAndTangent(faceNormalAndTangent.Normal, faceNormalAndTangent.Tangent);

				// Add our third vertex
				int32 V2 = Builder.AddVertex(Face.GetFinalEndVertexUp(voxelSize))
					.SetColor(FColor::White)
					.SetTexCoord(FVector2f(1, 1))
					.SetNormalAndTangent(faceNormalAndTangent.Normal, faceNormalAndTangent.Tangent);
				
				int32 V3 = Builder.AddVertex(Face.GetFinalStartVertexUp(voxelSize))
				.SetColor(FColor::White)
				.SetTexCoord(FVector2f(0, 1))
				.SetNormalAndTangent(faceNormalAndTangent.Normal, faceNormalAndTangent.Tangent);
				
				Builder.AddTriangle(V0, V1, V2, voxelIndex);
				Builder.AddTriangle(V2, V3, V0, voxelIndex);
			}
		}
	}
	
	AsyncTask(ENamedThreads::GameThread, [this, StreamSet, faceParams]()
	{
#if CPUPROFILERTRACE_ENABLED
		TRACE_CPUPROFILER_EVENT_SCOPE("Procedural mesh generation")
#endif
		
		URealtimeMeshSimple* RealtimeMesh = faceParams.ChunkStruct.ChildChunk->RealtimeMeshComponent->InitializeRealtimeMesh<URealtimeMeshSimple>();
			
		const FRealtimeMeshSectionGroupKey GroupKey = FRealtimeMeshSectionGroupKey::Create(0, FName("Chunk Mesh"));

		// Now we create the section group, since the stream set has polygroups, this will create the sections as well
		RealtimeMesh->CreateSectionGroup(GroupKey, *StreamSet);

		for (auto voxelId : faceParams.ChunkStruct.ChunkVoxelTypeTable)
		{
			auto voxelIndex = voxelId.Value;
			const FRealtimeMeshSectionKey PolyGroup0SectionKey = FRealtimeMeshSectionKey::CreateForPolyGroup(GroupKey, voxelIndex);
			RealtimeMesh->UpdateSectionConfig(PolyGroup0SectionKey, FRealtimeMeshSectionConfig(ERealtimeMeshSectionDrawType::Static, voxelIndex), true);

			FVoxelType voxelType = VoxelGenerator->GetVoxelTypeById(voxelId.Key);
			RealtimeMesh->SetupMaterialSlot(voxelIndex, voxelType.BlockName, voxelType.Material);
		}
	});
}

bool URunLengthMesher::IsMinBorder(const int x)
{
	return x == 0;
}

bool URunLengthMesher::IsMaxBorder(const int x) const
{
 	return x == VoxelGenerator->GetVoxelCountY() - 1;
}