// Fill out your copyright notice in the Description page of Project Settings.

#include "Mesher/MesherBase.h"

void UMesherBase::SetVoxelGenerator(const TObjectPtr<UVoxelGeneratorBase>& VoxelGeneratorBase)
{
	this->VoxelGenerator = VoxelGeneratorBase;
	UpdateAllFacesParams();
}
const UMesherBase::FNormalsAndTangents UMesherBase::FaceNormalsAndTangents[] = {
	{FVector3f(-1.0f, 0.0f, 0.0f), FVector3f(0.0, 1.0, 0.0)}, //Front
	{FVector3f(1.0f, 0.0f, 0.0f), FVector3f(0.0, 1.0, 0.0)}, //Back
	{FVector3f(0.0f, -1.0f, 0.0f), FVector3f(1.0f, 0.0f, 0.0f)}, // Right 
	{FVector3f(0.0f, 1.0f, 0.0f), FVector3f(1.0, 0.0, 0.0)}, // Left
	{FVector3f(0.0f, 0.0f, -1.0f), FVector3f(1.0f, 0.0f, 0.0f)}, //Bottom
	{FVector3f(0.0f, 0.0f, 1.0f), FVector3f(1.0f, 0.0f, 0.0f)} //Top
};

void UMesherBase::UpdateAllFacesParams()
{
	const auto ChunkDimension = VoxelGenerator->GetVoxelCountPerChunkDimension();
	//Axis X
	UpdateFaceParams(FrontFaceTemplate, FIntVector(-1, 0, 0),
					 FIntVector(ChunkDimension - 1, 0, 0),
					 FIntVector(0, -1, 0));

	UpdateFaceParams(BackFaceTemplate, FIntVector(1, 0, 0),
					 FIntVector(0, 0, 0),
					 FIntVector(0, -1, 0));

	//Axis Y
	UpdateFaceParams(RightFaceTemplate, FIntVector(0, -1, 0),
					 FIntVector(0, ChunkDimension - 1, 0),
					 FIntVector(-1, 0, 0));

	UpdateFaceParams(LeftFaceTemplate, FIntVector(0, 1, 0),
					 FIntVector(0, 0, 0),
					 FIntVector(-1, 0, 0));

	// Axis Z
	UpdateFaceParams(BottomFaceTemplate, FIntVector(0, 0, -1),
					 FIntVector(0, 0, ChunkDimension - 1),
					 FIntVector(0, -1, 0));

	UpdateFaceParams(TopFaceTemplate, FIntVector(0, 0, 1),
					 FIntVector(0, 0, 0),
					 FIntVector(0, -1, 0));
}

void UMesherBase::UpdateFaceParams(FMeshingDirections& Face, const FIntVector ForwardVoxelIndexVector,
										const FIntVector ChunkBorderIndexVector,
										const FIntVector PreviousVoxelIndexVector) const
{
	Face.ForwardVoxelIndex = VoxelGenerator->CalculateVoxelIndex(ForwardVoxelIndexVector);
	Face.PreviousVoxelIndex = VoxelGenerator->CalculateVoxelIndex(PreviousVoxelIndexVector);
	Face.ChunkBorderIndex = VoxelGenerator->CalculateVoxelIndex(ChunkBorderIndexVector);
}

