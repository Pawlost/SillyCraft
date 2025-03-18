#include "Mesher/RunDirectionalMesher.h"

#include "RealtimeMeshComponent.h"
#include "RealtimeMeshSimple.h"
#include "Mesh/RealtimeMeshBuilder.h"
#include "Mesher/MeshingUtils/MesherVariables.h"
#include "Spawner/ChunkSpawnerBase.h"

class URealtimeMeshSimple;

void URunDirectionalMesher::GenerateMesh(FMesherVariables& MeshVars)
{
	MeshVars.ChunkParams.OriginalChunk->bHasMesh = false;
	
	if (MeshVars.ChunkParams.OriginalChunk->ChunkVoxelIdTable.IsEmpty())
	{
		if (MeshVars.ChunkParams.OriginalChunk->ChunkMeshActor.IsValid())
		{
			// If chunk is full of empty voxels but actor was pulled from pool, clear its mesh
			MeshVars.ChunkParams.OriginalChunk->ChunkMeshActor->ClearMesh();
		}
		return;
	}

#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Mesh generation")
#endif
	
	InitFaceContainers(MeshVars);
	FaceGeneration(MeshVars);
	DirectionalGreedyMeshing(MeshVars);
	GenerateMeshFromFaces(MeshVars);
}

void URunDirectionalMesher::InitFaceContainers(FMesherVariables& MeshVars) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Mesh generation intialization")
#endif

	MeshVars.VoxelIdToLocalVoxelMap.Reserve(MeshVars.ChunkParams.OriginalChunk->ChunkVoxelIdTable.Num());
	MeshVars.VoxelIdToLocalVoxelMap.Empty();

	for (const auto VoxelId : MeshVars.ChunkParams.OriginalChunk->ChunkVoxelIdTable)
	{
		const auto LocalVoxelId = MeshVars.VoxelIdToLocalVoxelMap.Num();
		MeshVars.VoxelIdToLocalVoxelMap.Add(VoxelId.Key, LocalVoxelId);
	}

	for (uint8 f = 0; f < CHUNK_FACE_COUNT; f++)
	{
		// Voxel faces need to be sorted to different arrays by Id because Realtime Mesh Component requires it
		for (const auto Voxel : MeshVars.VoxelIdToLocalVoxelMap)
		{
			TMap<int32, uint32>& VoxelTable = MeshVars.ChunkParams.OriginalChunk->ChunkVoxelIdTable;
			MeshVars.Faces[f].SetNum(VoxelTable.Num());
			
			auto FaceArray = MeshVars.Faces[f][Voxel.Value];
			if (FaceArray == nullptr || !FaceArray.IsValid())
			{
				// In case voxel table is not available this code needs to be rewritten to add local voxels id dynamically during voxel grid traversal
				FaceArray = MakeShared<TArray<FChunkFace>>();
				MeshVars.Faces[f][Voxel.Value] = FaceArray;
			}
			else
			{
				// If array was pulled from a pool, just empty it 
				FaceArray->Empty();
			}
			
			// Preallocate memory needed for meshing
			const uint32 Count = VoxelGenerator->GetVoxelCountPerChunk();
			FaceArray->Reserve(Count);
		}
	}
}

void URunDirectionalMesher::FaceGeneration(FMesherVariables& faceParams) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Run lenght meshing generation")
#endif

	const auto ChunkDimension = VoxelGenerator->GetVoxelCountPerChunkDimension();
	
	// Traverse through voxel grid
	for (uint32 x = 0; x < ChunkDimension; x++)
	{
		// Border is necessary to know if voxels from neighboring chunk are needed.
		const auto bMinBorder = IsMinBorder(x);
		const auto bMaxBorder = IsMaxBorder(x);

		// Get last voxel coordinate which is different for each face
		const auto XAxisIndex = VoxelGenerator->CalculateVoxelIndex(x, 0, 0);
		const auto YAxisIndex = VoxelGenerator->CalculateVoxelIndex(0, x, 0);
		const auto ZAxisIndex = VoxelGenerator->CalculateVoxelIndex(0, 0, x);

		for (uint32 z = 0; z < ChunkDimension; z++)
		{
			for (uint32 y = 0; y < ChunkDimension; y++)
			{
				/*
				* Increment run for each chunk axis
				* Coordinates are different in order to create a sorted array of quads/faces.
				* Sorted array is then used in Run Direction Greedy meshing, because it is sorted the array needs to traversed only once.
				* Grid is only read from, it doesn't matter which coordinate is first
				* Each voxel needs to be visited only once per face.
				* Indices are same for face and reversed face.
				*/
				IncrementRun(x, y, z, XAxisIndex, bMinBorder, bMaxBorder, FrontFaceTemplate, BackFaceTemplate,
				             faceParams);
				IncrementRun(y, x, z, YAxisIndex, bMinBorder, bMaxBorder, RightFaceTemplate, LeftFaceTemplate,
				             faceParams);
				IncrementRun(z, y, x, ZAxisIndex, bMinBorder, bMaxBorder, BottomFaceTemplate, TopFaceTemplate,
				             faceParams);
			}
		}
	}
}

void URunDirectionalMesher::IncrementRun(const int X, const int Y, const int Z, const int32 AxisVoxelIndex, const bool bIsMinBorder, const bool bIsMaxBorder,
                                    const FMeshingDirections& FaceTemplate,
                                    const FMeshingDirections& ReversedFaceTemplate,
                                    FMesherVariables& MeshVars) const
{
	// Get voxel at current position of the run.
	const auto Position = FIntVector(X, Y, Z);
	const int32 Index = VoxelGenerator->CalculateVoxelIndex(Position);
	const FVoxel Voxel = MeshVars.ChunkParams.OriginalChunk->VoxelGrid[Index];

	// If voxel is empty, no mesh should be generated
	if (!Voxel.IsEmptyVoxel())
	{
		// Get correct face containers
		auto OriginalChunk = MeshVars.ChunkParams.OriginalChunk;
		const auto LocalVoxelId = MeshVars.VoxelIdToLocalVoxelMap[Voxel.VoxelId];
		const auto FaceContainerIndex = static_cast<uint8>(FaceTemplate.StaticMeshingData.FaceSide);
		const auto FaceContainerVoxelIndex = static_cast<uint8>(ReversedFaceTemplate.StaticMeshingData.FaceSide);

		// Generate face for each direction
		AddFace(FaceTemplate, bIsMinBorder, Index, Position, Voxel, AxisVoxelIndex,
		        MeshVars.Faces[FaceContainerIndex][LocalVoxelId], MeshVars.ChunkParams);
		AddFace(ReversedFaceTemplate, bIsMaxBorder, Index, Position, Voxel, AxisVoxelIndex,
		        MeshVars.Faces[FaceContainerVoxelIndex][LocalVoxelId], MeshVars.ChunkParams);
	}
}

void URunDirectionalMesher::AddFace(const FMeshingDirections& FaceTemplate, const bool bIsBorder,
                               const int32& Index, const FIntVector& Position, const FVoxel& Voxel,
                               const int32& AxisVoxelIndex,
                               const TSharedPtr<TArray<FChunkFace>>& ChunkFaces, const FChunkParams& ChunkParams)
{
	// Calculate indices need to check if face should be generated
	const FVoxelIndexParams VoxelIndexParams =
	{
		bIsBorder,
		FaceTemplate.ForwardVoxelIndex + Index,
		FaceTemplate.PreviousVoxelIndex + Index,
		Index - AxisVoxelIndex + FaceTemplate.ChunkBorderIndex,
		Voxel,
		FaceTemplate.StaticMeshingData.FaceSide
	};

	// Check if face should be generated
	if (IsBorderVoxelVisible(VoxelIndexParams, ChunkParams) || IsVoxelVisible(VoxelIndexParams, ChunkParams))
	{
		// Generate new face with coordinates
		const FChunkFace NewFace =
		{
			Voxel,
			FaceTemplate.StaticMeshingData.FaceTemplate.StartVertexDown + Position,
			FaceTemplate.StaticMeshingData.FaceTemplate.EndVertexDown + Position,
			FaceTemplate.StaticMeshingData.FaceTemplate.EndVertexUp + Position,
			FaceTemplate.StaticMeshingData.FaceTemplate.StartVertexUp + Position,
		};

		if (!ChunkFaces->IsEmpty())
		{
			// Tries to merge face coordinates into previous face. Because faces are sorted, the last one is always the correct one.
			FChunkFace& PrevFace = ChunkFaces->Last();
			
			if (FaceTemplate.StaticMeshingData.RunDirectionFaceMerge(PrevFace, NewFace))
			{
				// Return when new face was merged
				return;
			}
		}

		ChunkFaces->Push(NewFace);
	}
}

bool URunDirectionalMesher::IsBorderVoxelVisible(const FVoxelIndexParams& FaceData, const FChunkParams& ChunkStruct)
{
	if (FaceData.IsBorder)
	{
		// Check voxel visibility in side chunk (crosschunk)
		const auto FaceContainerIndex = static_cast<uint8>(FaceData.FaceDirection);
		const auto SideChunk = ChunkStruct.SideChunks[FaceContainerIndex];
		if (SideChunk != nullptr)
		{
			const auto NextVoxel = SideChunk->VoxelGrid[FaceData.CurrentVoxelIndex];
			return NextVoxel.IsTransparent() && NextVoxel != FaceData.CurrentVoxel;
		}

		return SideChunk == nullptr && ChunkStruct.ShowBorders;
	}
	return false;
}

bool URunDirectionalMesher::IsVoxelVisible(const FVoxelIndexParams& FaceData, const FChunkParams& ChunkStruct)
{
	if (!FaceData.IsBorder && ChunkStruct.OriginalChunk->VoxelGrid.IsValidIndex(FaceData.ForwardVoxelIndex))
	{
		// Check if next voxel is visible based on calculated index
		const auto NextVoxel = ChunkStruct.OriginalChunk->VoxelGrid[FaceData.ForwardVoxelIndex];
		return NextVoxel.IsTransparent() && NextVoxel != FaceData.CurrentVoxel;
	}
	return false;
}

void URunDirectionalMesher::DirectionalGreedyMeshing(const FMesherVariables& MeshVars)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Run directional greedy mesh generation")
#endif

	// Merge faces in sorted arrays
	for (uint8 f = 0; f < CHUNK_FACE_COUNT; f++)
	{
		for (const auto VoxelId : MeshVars.VoxelIdToLocalVoxelMap)
		{
			auto FaceContainer = MeshVars.Faces[f][VoxelId.Value];
			const int LastElementIndex = FaceContainer->Num() - 1;

			// Iterate from last face
			for (int32 i = LastElementIndex - 1; i >= 0; i--)
			{
				FChunkFace& NextFace = (*FaceContainer)[i + 1];

				int BackTrackIndex = i;

				/*
				 * Face may be last in the coordinate row.
				 * It is necessary to iterate through the current coordinate row to reach previous coordinate row.
				 * It is necessary to go only -1 coordinate because if there is a merge, it will accumulate.
				 */
				while (FaceContainer->IsValidIndex(BackTrackIndex))
				{
					FChunkFace& Face = (*FaceContainer)[BackTrackIndex];

					if (Face.StartVertexUp.Z < NextFace.StartVertexDown.Z)
					{
						// Break the iteration if coordinate row is too far.
						break;
					}

					if (FChunkFace::MergeFaceUp(Face, NextFace))
					{
						// Break the iteration if merge was found
						FaceContainer->RemoveAt(i + 1, EAllowShrinking::No);
						break;
					}

					BackTrackIndex--;
				}
			}
		}
	}
}

void URunDirectionalMesher::GenerateMeshFromFaces(const FMesherVariables& MeshVars) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Mesh stream generation")
#endif

	auto StreamSet = MakeShared<FRealtimeMeshStreamSet>();

	TRealtimeMeshBuilderLocal<int32> Builder(*StreamSet.ToWeakPtr().Pin());

	Builder.EnableTexCoords();
	Builder.EnableColors();
	Builder.EnableTangents();
	Builder.EnablePolyGroups();

	if (!IsValid(VoxelGenerator))
	{
		return;
	}

	auto VoxelSize = VoxelGenerator->GetVoxelSize();

	// Local voxel table 
	TMap<uint32, uint16> LocalVoxelTable;

	// Iterate through merged faces
	for (auto VoxelId : MeshVars.VoxelIdToLocalVoxelMap)
	{
		for (uint8 FaceIndex = 0; FaceIndex < CHUNK_FACE_COUNT; FaceIndex++)
		{
			auto FaceContainer = MeshVars.Faces[FaceIndex];

			auto SideFaces = FaceContainer[VoxelId.Value];

			auto [Normal, Tangent] = FaceNormalsAndTangents[FaceIndex];

			// Create quad foreach face
			for (auto Face : *SideFaces)
			{
				int32 V0 = Builder.AddVertex(Face.GetFinalStartVertexDown(VoxelSize))
				                  .SetColor(FColor::White)
				                  .SetTexCoord(FVector2f(0, 0))
				                  .SetNormalAndTangent(Normal, Tangent);
				
				int32 V1 = Builder.AddVertex(Face.GetFinalEndVertexDown(VoxelSize))
				                  .SetColor(FColor::White)
				                  .SetTexCoord(FVector2f(1, 0))
				                  .SetNormalAndTangent(Normal, Tangent);


				int32 V2 = Builder.AddVertex(Face.GetFinalEndVertexUp(VoxelSize))
				                  .SetColor(FColor::White)
				                  .SetTexCoord(FVector2f(1, 1))
				                  .SetNormalAndTangent(Normal, Tangent);

				int32 V3 = Builder.AddVertex(Face.GetFinalStartVertexUp(VoxelSize))
				                  .SetColor(FColor::White)
				                  .SetTexCoord(FVector2f(0, 1))
				                  .SetNormalAndTangent(Normal, Tangent);

				if (!LocalVoxelTable.Contains(VoxelId.Key))
				{
					// Keep track of how many voxel quads are actually displayed
					LocalVoxelTable.Add(VoxelId.Key, LocalVoxelTable.Num());
				}

				// Create quad from 2 triangles
				Builder.AddTriangle(V0, V1, V2, LocalVoxelTable[VoxelId.Key]);
				Builder.AddTriangle(V2, V3, V0, LocalVoxelTable[VoxelId.Key]);
			}
		}
	}

	if (!MeshVars.ChunkParams.OriginalChunk.IsValid() || LocalVoxelTable.IsEmpty())
	{
		return;
	}

	auto Spawner = MakeShared<FChunkParams>(MeshVars.ChunkParams);

	if (!MeshVars.ChunkParams.ExecutedOnMainThread)
	{
		// Synchronize Mesh generation with game thread.
		AsyncTask(ENamedThreads::GameThread, [this, LocalVoxelTable, StreamSet, Spawner]()
		{
			GenerateActorMesh(LocalVoxelTable, *StreamSet, Spawner);
		});
	}
	else
	{
		//Creating AsyncTask from main thread will cause deadlock
		GenerateActorMesh(LocalVoxelTable, *StreamSet, Spawner);
	}

	MeshVars.ChunkParams.OriginalChunk->bHasMesh = true;
}

void URunDirectionalMesher::GenerateActorMesh(const TMap<uint32, uint16>& LocalVoxelTable,
                                         const FRealtimeMeshStreamSet& StreamSet,
                                         const TSharedPtr<FChunkParams>& ChunkParams) const
{
	const auto World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	//Spawn actor
	const auto Chunk = ChunkParams->OriginalChunk;
	TWeakObjectPtr<AChunkRMCActor> ActorPtr = Chunk->ChunkMeshActor;
	const auto SpawnLocation = FVector(Chunk->GridPosition) * VoxelGenerator->GetChunkAxisSize();

	FAttachmentTransformRules ActorAttachmentRules = FAttachmentTransformRules::KeepWorldTransform;
	if (!ChunkParams->WorldTransform)
	{
		ActorAttachmentRules = FAttachmentTransformRules::KeepRelativeTransform;
	}

	if (ActorPtr == nullptr)
	{
		// If there is no actor spawn new one.
		ActorPtr = World->SpawnActor<AChunkRMCActor>(AChunkRMCActor::StaticClass(), SpawnLocation,
		                                             FRotator::ZeroRotator);

		if (!ActorPtr.IsValid() || !ChunkParams->SpawnerPtr.IsValid())
		{
			return;
		}

		ActorPtr->AttachToActor(ChunkParams->SpawnerPtr.Get(), ActorAttachmentRules);
	}
	else
	{
		if (!ActorPtr.IsValid())
		{
			return;
		}
		
		// If actor exists, ensure correct location
		if (!ChunkParams->WorldTransform)
		{
			ActorPtr->SetActorRelativeLocation(SpawnLocation);
		}
		else
		{
			ActorPtr->SetActorLocation(SpawnLocation);
		}
	}
	
	Chunk->ChunkMeshActor = ActorPtr;


	ActorPtr->PrepareMesh();
	const auto RealtimeMesh = ActorPtr->RealtimeMeshComponent->GetRealtimeMeshAs<
		URealtimeMeshSimple>();

	// Fill actor with mesh
	// Now we create the section group, since the stream set has polygroups, this will create the sections as well
	RealtimeMesh->CreateSectionGroup(ActorPtr->GroupKey, StreamSet);

	for (const auto VoxelId : LocalVoxelTable)
	{
		// Add voxel materials to mesh
		const auto MaterialId = VoxelId.Value;
		const auto VoxelType = VoxelGenerator->GetVoxelTypeById(VoxelId.Key);
		RealtimeMesh->SetupMaterialSlot(MaterialId, VoxelType.Key, VoxelType.Value.Material);

		const auto Key = FRealtimeMeshSectionKey::CreateForPolyGroup(ActorPtr->GroupKey, MaterialId);

		/**
		* This code may cause debugger trigger when closing editor while collision is still generating.
		* It seems that collider is created on async thread, but currently there is no way to wait for its creation or kill it.
		* Async thread becomes invalid and wrong memory is access which causes the trigger.
		* It does not break the execution, you can continue after breakpoint trigger, it will only log to console.
		* For futher details you may join discord channel: https://discord.gg/KGvBBTv 
		*/

		// Generate collider
		RealtimeMesh->UpdateSectionConfig(Key, FRealtimeMeshSectionConfig(
			                                  ERealtimeMeshSectionDrawType::Static, MaterialId),
		                                  true);
	}
}
