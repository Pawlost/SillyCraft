// Fill out your copyright notice in the Description page of Project Settings.

#include "ChunkMesher.h"

ChunkMesher::ChunkMesher(const std::shared_ptr<BlockRegistry>& registry, const int& maxElevation) : m_registry(registry), m_maxElevation(maxElevation)
{
}

bool ChunkMesher::MeshChunk(AChunk& meshchunk, std::array<AChunk*, 6> chunkArray) const
{
	std::unique_ptr<std::array<Voxel, Constants::ChunkSize3D>> voxels = std::make_unique<std::array<Voxel, Constants::ChunkSize3D>>();

	if (NaiveGreedyMeshing(voxels, meshchunk, chunkArray))
	{
		TMultiMap<int, Face*> meshedFaces;
		GreedyMeshing(*voxels, meshedFaces);
		if (meshedFaces.Num() > 0)
		{
			TArray<FVector>* vectors;
			TArray<int32>* indice;
			TArray<FLinearColor>* color;

			TMap<int, TArray<FVector>> vectorsMap;
			TMap<int, TArray<int32>> indiceMap;
			TMap<int, TArray<FLinearColor>> colorMap;
			TMap<int, int> indexes;

			int* index;

			for (TPair<int, Face*> pair : meshedFaces) {
				const Block& currentBlock = *m_registry->GetBlock(pair.Key);
				Face* face = pair.Value;

				if (face->visible) {
					IntVector* v1 = &face->v1;
					IntVector* v2 = &face->v2;
					IntVector* v3 = &face->v3;
					IntVector* v4 = &face->v4;

					if (!vectorsMap.Contains(pair.Key))
					{
						vectorsMap.Add(pair.Key, TArray<FVector>());
						indiceMap.Add(pair.Key, TArray<int32>());
						colorMap.Add(pair.Key, TArray<FLinearColor>());
						indexes.Add(pair.Key, 0);
					}

					index = &indexes[pair.Key];
					vectors = &vectorsMap[pair.Key];
					indice = &indiceMap[pair.Key];
					color = &colorMap[pair.Key];

					vectors->Add(v1->GetVertice());
					vectors->Add(v2->GetVertice());
					vectors->Add(v3->GetVertice());
					vectors->Add(v4->GetVertice());

					AddIndice(*index, *indice);
					*index += 4;

					FLinearColor blockColor = currentBlock.Color;

					if (!face->placed)
					{
						FVector chunkPos = meshchunk.GetActorLocation();

						float r = (chunkPos.Z + v3->Z * Constants::ChunkScale) / (Constants::ChunkLenght + m_maxElevation);
						float g = (chunkPos.Z + v3->Z * Constants::ChunkScale) / (Constants::ChunkLenght + m_maxElevation);
						float b = (chunkPos.Z + v3->Z * Constants::ChunkScale) / (Constants::ChunkLenght + m_maxElevation);

						r = std::max(std::min(blockColor.R + r, 1.0f), 0.0f);
						g = std::max(std::min(blockColor.G + g, 1.0f), 0.0f);
						b = std::max(std::min(blockColor.B + b, 1.0f), 0.0f);

						blockColor.R = r;
						blockColor.G = g;
						blockColor.B = b;
					}

					for (int c = 0; c < 4; c++)
					{
						color->Add(blockColor);
					}
				}
			}

			int i = 0;
			for (TPair<int, TArray<FVector>> pair : vectorsMap)
			{
				vectors = &pair.Value;
				indice = &indiceMap[pair.Key];
				color = &colorMap[pair.Key];
				meshchunk.SetMesh(i, *vectors, *indice, *color);
				i++;
			}
		}
		return true;
	}

	return false;
}

FVector ChunkMesher::IntVector::GetVertice() const
{
	return FVector(X * Constants::ChunkScale, Y * Constants::ChunkScale, Z * Constants::ChunkScale);
}

bool ChunkMesher::NaiveGreedyMeshing(const std::unique_ptr<std::array<Voxel, Constants::ChunkSize3D>>& voxels, const AChunk& chunk, std::array<AChunk*, 6> chunkArray) const
{
	bool meshed = false;
	IntVector* v1 = nullptr;
	IntVector* v2 = nullptr;
	IntVector* v3 = nullptr;
	IntVector* v4 = nullptr;
	Face* face = nullptr;
	Face* prevface = nullptr;
	std::shared_ptr<Block> otherBlock = nullptr;
	std::shared_ptr<Block> currentBlock;
	Voxel* voxel;

	int index = 0;
	int x, y, z, id;
	bool placed;

	for (int i = 0; i < Constants::ChunkSize3D; i++)
	{
		id = chunk.GetBlockID(i);
		placed = chunk.IsPlaceByPlayer(i);
		currentBlock = m_registry->GetBlock(id);
		voxel = &(*voxels)[i];
		voxel->ID = id;

		if ((*currentBlock).BlockHardness != Block::Hardness::Empty)
		{
			meshed = true;
			y = i % Constants::ChunkSize;
			z = (i / Constants::ChunkSize) % Constants::ChunkSize;
			x = i / Constants::ChunkSize2D;

			//Front
			face = &voxel->face[Faces::Front];
			face->placed = placed;
			otherBlock = m_registry->GetBlock(m_registry->AirID);

			if (x > 0) {
				index = i + Constants::MakeIndex(0, 0, -1);
				otherBlock = m_registry->GetBlock(chunk.GetBlockID(index));
			}
			else
			{
				index = Constants::MakeIndex(y, z, Constants::ChunkSize - 1);
				otherBlock = m_registry->GetBlock(chunkArray[Faces::Front]->GetBlockID(index));
			}

			if ((*otherBlock).BlockHardness == Block::Hardness::Empty)
			{
				face->visible = true;

				v1 = &face->v1;
				v2 = &face->v2;
				v3 = &face->v3;
				v4 = &face->v4;

				v2->Y = y + 1;
				v2->Z = z;
				v2->X = x;

				v3->Y = y + 1;
				v3->Z = z + 1;
				v3->X = x;

				id = m_registry->AirID;

				if (y > 0) {
					index = i + Constants::MakeIndex(-1, 0, 0);
					id = chunk.GetBlockID(index);
				}

				prevface = &(*voxels)[index].face[Faces::Front];

				if (id == voxel->ID && prevface->visible && face->placed == prevface->placed)
				{
					ChunkMesher::Join(face, prevface);
				}
				else
				{
					v1->Y = y;
					v1->Z = z;
					v1->X = x;

					v4->Y = y;
					v4->Z = z + 1;
					v4->X = x;
				}
			}
			else
			{
				face->visible = false;
			}

			//Back
			face = &voxel->face[Faces::Back];
			face->placed = placed;
			otherBlock = m_registry->GetBlock(m_registry->AirID);

			if (x < Constants::ChunkSize - 1) {
				index = i + Constants::MakeIndex(0, 0, 1);
				otherBlock = m_registry->GetBlock(chunk.GetBlockID(index));
			}
			else
			{
				index = Constants::MakeIndex(y, z, 0);
				otherBlock = m_registry->GetBlock(chunkArray[Faces::Back]->GetBlockID(index));
			}

			if ((*otherBlock).BlockHardness == Block::Hardness::Empty)
			{
				face->visible = true;

				v1 = &face->v1;
				v2 = &face->v2;
				v3 = &face->v3;
				v4 = &face->v4;

				v1->Y = y + 1;
				v1->Z = z;
				v1->X = x + 1;

				v4->Y = y + 1;
				v4->Z = z + 1;
				v4->X = x + 1;

				id = m_registry->AirID;
				if (y > 0) {
					index = i + Constants::MakeIndex(-1, 0, 0);
					id = chunk.GetBlockID(index);
				}

				prevface = &(*voxels)[index].face[Faces::Back];

				if (id == voxel->ID && prevface->visible && face->placed == prevface->placed)
				{
					ChunkMesher::JoinReversed(face, prevface);
				}
				else
				{
					v2->Y = y;
					v2->Z = z;
					v2->X = x + 1;

					v3->Y = y;
					v3->Z = z + 1;
					v3->X = x + 1;
				}
			}
			else
			{
				face->visible = false;
			}

			//Left
			face = &voxel->face[Faces::Left];
			face->placed = placed;
			otherBlock = m_registry->GetBlock(m_registry->AirID);

			if (y > 0) {
				index = i + Constants::MakeIndex(-1, 0, 0);
				otherBlock = m_registry->GetBlock(chunk.GetBlockID(index));
			}
			else
			{
				index = Constants::MakeIndex(Constants::ChunkSize - 1, z, x);
				otherBlock = m_registry->GetBlock(chunkArray[Faces::Left]->GetBlockID(index));
			}

			if ((*otherBlock).BlockHardness == Block::Hardness::Empty)
			{
				face->visible = true;

				v1 = &face->v1;
				v2 = &face->v2;
				v3 = &face->v3;
				v4 = &face->v4;

				v1->Y = y;
				v1->Z = z;
				v1->X = x + 1;

				v4->Y = y;
				v4->Z = z + 1;
				v4->X = x + 1;

				id = m_registry->AirID;
				if (x > 0) {
					index = i + Constants::MakeIndex(0, 0, -1);
					id = chunk.GetBlockID(index);
				}

				prevface = &(*voxels)[index].face[Faces::Left];

				if (id == voxel->ID && prevface->visible && face->placed == prevface->placed)
				{
					ChunkMesher::JoinReversed(face, prevface);
				}
				else
				{
					v2->Y = y;
					v2->Z = z;
					v2->X = x;

					v3->Y = y;
					v3->Z = z + 1;
					v3->X = x;
				}
			}
			else
			{
				face->visible = false;
			}

			//Right
			face = &voxel->face[Faces::Right];
			face->placed = placed;
			otherBlock = m_registry->GetBlock(m_registry->AirID);

			if (y < Constants::ChunkSize - 1) {
				index = i + Constants::MakeIndex(1, 0, 0);
				otherBlock = m_registry->GetBlock(chunk.GetBlockID(index));
			}
			else
			{
				index = Constants::MakeIndex(0, z, x);
				otherBlock = m_registry->GetBlock(chunkArray[Faces::Right]->GetBlockID(index));
			}

			if ((*otherBlock).BlockHardness == Block::Hardness::Empty)
			{
				face->visible = true;

				v1 = &face->v1;
				v2 = &face->v2;
				v3 = &face->v3;
				v4 = &face->v4;

				v2->Y = y + 1;
				v2->Z = z;
				v2->X = x + 1;

				v3->Y = y + 1;
				v3->Z = z + 1;
				v3->X = x + 1;

				id = m_registry->AirID;
				if (x > 0)
				{
					index = i + Constants::MakeIndex(0, 0, -1);
					id = chunk.GetBlockID(index);
				}

				prevface = &(*voxels)[index].face[Faces::Right];

				if (id == voxel->ID && prevface->visible && face->placed == prevface->placed)
				{
					ChunkMesher::Join(face, prevface);
				}
				else
				{
					v1->Y = y + 1;
					v1->Z = z;
					v1->X = x;

					v4->Y = y + 1;
					v4->Z = z + 1;
					v4->X = x;
				}
			}
			else
			{
				face->visible = false;
			}

			//Bottom
			face = &voxel->face[Faces::Bottom];
			face->placed = placed;
			otherBlock = m_registry->GetBlock(m_registry->AirID);

			if (z > 0) {
				index = i + Constants::MakeIndex(0, -1, 0);
				otherBlock = m_registry->GetBlock(chunk.GetBlockID(index));
			}
			else
			{
				index = Constants::MakeIndex(y, Constants::ChunkSize - 1, x);
				otherBlock = m_registry->GetBlock(chunkArray[Faces::Bottom]->GetBlockID(index));
			}

			if ((*otherBlock).BlockHardness == Block::Hardness::Empty)
			{
				face->visible = true;

				v1 = &face->v1;
				v2 = &face->v2;
				v3 = &face->v3;
				v4 = &face->v4;

				v1->Y = y + 1;
				v1->Z = z;
				v1->X = x;

				v4->Y = y + 1;
				v4->Z = z;
				v4->X = x + 1;

				id = m_registry->AirID;
				if (y > 0) {
					index = i + Constants::MakeIndex(-1, 0, 0);
					id = chunk.GetBlockID(index);
				}

				prevface = &(*voxels)[index].face[Faces::Bottom];

				if (id == voxel->ID && prevface->visible && face->placed == prevface->placed)
				{
					ChunkMesher::JoinReversed(face, prevface);
				}
				else
				{
					v2->Y = y;
					v2->Z = z;
					v2->X = x;

					v3->Y = y;
					v3->Z = z;
					v3->X = x + 1;
				}
			}
			else
			{
				face->visible = false;
			}

			//Top
			face = &voxel->face[Faces::Top];
			face->placed = placed;
			otherBlock = m_registry->GetBlock(m_registry->AirID);

			if (z < Constants::ChunkSize - 1) {
				index = i + Constants::MakeIndex(0, 1, 0);
				otherBlock = m_registry->GetBlock(chunk.GetBlockID(index));
			}
			else
			{
				index = Constants::MakeIndex(y, 0, x);
				otherBlock = m_registry->GetBlock(chunkArray[Faces::Top]->GetBlockID(index));
			}

			if ((*otherBlock).BlockHardness == Block::Hardness::Empty)
			{
				face->visible = true;

				v1 = &face->v1;
				v2 = &face->v2;
				v3 = &face->v3;
				v4 = &face->v4;

				v2->Y = y + 1;
				v2->Z = z + 1;
				v2->X = x;

				v3->Y = y + 1;
				v3->Z = z + 1;
				v3->X = x + 1;

				id = m_registry->AirID;

				if (y > 0) {
					index = i + Constants::MakeIndex(-1, 0, 0);
					id = chunk.GetBlockID(index);
				}

				prevface = &(*voxels)[index].face[Faces::Top];

				if (id == voxel->ID && prevface->visible && face->placed == prevface->placed)
				{
					ChunkMesher::Join(face, prevface);
				}
				else
				{
					v1->Y = y;
					v1->Z = z + 1;
					v1->X = x;

					v4->Y = y;
					v4->Z = z + 1;
					v4->X = x + 1;
				}
			}
			else
			{
				face->visible = false;
			}
		}
	}
	return  meshed;
}

void ChunkMesher::GreedyMeshing(std::array<Voxel, Constants::ChunkSize3D>& voxels, TMultiMap<int, Face*>& meshedFaces) const
{
	int index = 0;
	Voxel* otherVoxel;
	Voxel* voxel;
	Face* face;
	int x, y, z, f;

	for (int i = 0; i < Constants::ChunkSize3D; i++)
	{
		y = i % Constants::ChunkSize;
		z = (i / Constants::ChunkSize) % Constants::ChunkSize;
		x = i / Constants::ChunkSize2D;

		voxel = &voxels[i];

		for (f = 0; f < 6; f++)
		{
			face = &voxel->face[f];

			if (face->visible) {
				switch (f)
				{
				case Faces::Front:
				case Faces::Back:
				case Faces::Right:
				case Faces::Left:
					//Commented because of color change per elevation
				/*	if (z < Constants::ChunkSize - 1) {
						index = i + Constants::MakeIndex(0, 1, 0);
						otherVoxel = &voxels[index];
						Face* otherface = &otherVoxel->face[f];
						if (otherVoxel->ID == voxel->ID && ChunkMesher::GreedyJoin(face, otherface))
						{
							continue;
						}
					}*/
					break;
				case Faces::Top:
				case Faces::Bottom:
					if (x < Constants::ChunkSize - 1) {
						index = i + Constants::MakeIndex(0, 0, 1);
						otherVoxel = &voxels[index];
						Face* otherface = &otherVoxel->face[f];
						if (otherVoxel->ID == voxel->ID && ChunkMesher::GreedyJoin(face, otherface))
						{
							continue;
						}
					}
					break;
				}
				meshedFaces.Add(voxel->ID, face);
			}
		}
	}
}

void ChunkMesher::Join(Face* face, Face* prevface)
{
	if (prevface->prevFace)
	{
		prevface->visible = false;
		prevface = prevface->prevFace;
	}

	face->v1 = prevface->v1;
	face->v4 = prevface->v4;

	prevface->visible = false;
	face->prevFace = prevface;
}

void ChunkMesher::JoinReversed(Face* face, Face* prevface)
{
	if (prevface->prevFace)
	{
		prevface->visible = false;
		prevface = prevface->prevFace;
	}

	face->v2 = prevface->v2;
	face->v3 = prevface->v3;

	prevface->visible = false;
	face->prevFace = prevface;
}

bool ChunkMesher::GreedyJoin(Face* face, Face* otherface)
{
	if (face->v3 == otherface->v2 && face->v4 == otherface->v1 && otherface->visible)
	{
		otherface->v1 = face->v1;
		otherface->v2 = face->v2;
		face->visible = false;
		return true;
	}
	return false;
}

void ChunkMesher::AddIndice(const int& index, TArray<int>& indice) const
{
	indice.Add(index);
	indice.Add(index + 1);
	indice.Add(index + 2);


	indice.Add(index + 2);
	indice.Add(index + 3);
	indice.Add(index);
}

ChunkMesher::~ChunkMesher()
{
}

bool ChunkMesher::IntVector:: operator ==(const IntVector& a) 
{
	return X == a.X && Y == a.Y && Z == a.Z;
}
bool ChunkMesher::IntVector:: operator !=(const IntVector& a)
{
	return !(*this == a);
}

void ChunkMesher::CreateFastCube(AFastCube& cube, const FLinearColor& color) const
{
	int index = 0;
	TArray<FVector> vectors;
	TArray<int32> indice;
	TArray<FLinearColor> colors;

	//Front
	vectors.Add(FVector(0, 0, 0));
	vectors.Add(FVector(Constants::ChunkScale, 0, 0));
	vectors.Add(FVector(Constants::ChunkScale, Constants::ChunkScale, 0));
	vectors.Add(FVector(0, Constants::ChunkScale, 0));

	AddIndice(index, indice);
	index += 4;

	for (int c = 0; c < 4; c++)
	{
		colors.Add(color);
	}

	//Back
	vectors.Add(FVector(Constants::ChunkScale, 0, Constants::ChunkScale));
	vectors.Add(FVector(0, 0, Constants::ChunkScale));
	vectors.Add(FVector(0, Constants::ChunkScale, Constants::ChunkScale));
	vectors.Add(FVector(Constants::ChunkScale, Constants::ChunkScale, Constants::ChunkScale));

	AddIndice(index, indice);
	index += 4;

	for (int c = 0; c < 4; c++)
	{
		colors.Add(color);
	}

	//Left
	vectors.Add(FVector(0, 0, Constants::ChunkScale));
	vectors.Add(FVector(0, 0, 0));
	vectors.Add(FVector(0, Constants::ChunkScale, 0));
	vectors.Add(FVector(0, Constants::ChunkScale, Constants::ChunkScale));

	AddIndice(index, indice);
	index += 4;

	for (int c = 0; c < 4; c++)
	{
		colors.Add(color);
	}

	//Right
	vectors.Add(FVector(Constants::ChunkScale, 0, 0));
	vectors.Add(FVector(Constants::ChunkScale, 0, Constants::ChunkScale));
	vectors.Add(FVector(Constants::ChunkScale, Constants::ChunkScale, Constants::ChunkScale));
	vectors.Add(FVector(Constants::ChunkScale, Constants::ChunkScale, 0));

	AddIndice(index, indice);
	index += 4;

	for (int c = 0; c < 4; c++)
	{
		colors.Add(color);
	}

	//Bottom
	vectors.Add(FVector(Constants::ChunkScale, 0, 0));
	vectors.Add(FVector(0, 0, 0));
	vectors.Add(FVector(0, 0, Constants::ChunkScale));
	vectors.Add(FVector(Constants::ChunkScale, 0, Constants::ChunkScale));

	AddIndice(index, indice);
	index += 4;

	for (int c = 0; c < 4; c++)
	{
		colors.Add(color);
	}

	//Top
	vectors.Add(FVector(0, Constants::ChunkScale, 0));
	vectors.Add(FVector(Constants::ChunkScale, Constants::ChunkScale, 0));
	vectors.Add(FVector(Constants::ChunkScale, Constants::ChunkScale, Constants::ChunkScale));
	vectors.Add(FVector(0, Constants::ChunkScale, Constants::ChunkScale));

	AddIndice(index, indice);
	index += 4;

	for (int c = 0; c < 4; c++)
	{
		colors.Add(color);
	}

	cube.SetMesh(0, vectors, indice, colors);
}
