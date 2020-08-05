// Fill out your copyright notice in the Description page of Project Settings.

#include "ChunkMesher.h"

ChunkMesher::ChunkMesher()
{

}

void ChunkMesher::MeshChunk(AChunk* chunk) {

	TArray<FVector> vectors;
	TArray<int32> indice;
	TArray<FLinearColor> color;

	int index = 0;

	for (int i = 0; i < Constants::ChunkSize3D; i++) {
		int x = (i % Constants::ChunkSize) * Constants::ChunkScale;
		int y = (i / Constants::ChunkSize2D) * Constants::ChunkScale;
		int z = ((i / Constants::ChunkSize) % Constants::ChunkSize) * Constants::ChunkScale;

		//Front
		vectors.Add(FVector(x, y, z));
		vectors.Add(FVector(x + Constants::ChunkScale, y, z));
		vectors.Add(FVector(x + Constants::ChunkScale, y + Constants::ChunkScale, z));
		vectors.Add(FVector(x, y + Constants::ChunkScale, z));


		indice.Add(index);
		indice.Add(index +1);
		indice.Add(index +2);


		indice.Add(index +2);
		indice.Add(index +3);
		indice.Add(index);

		index += 4;

		//Back

		vectors.Add(FVector(x + Constants::ChunkScale, y, z + Constants::ChunkScale));
		vectors.Add(FVector(x, y, z + Constants::ChunkScale));
		vectors.Add(FVector(x, y + Constants::ChunkScale, z + Constants::ChunkScale));
		vectors.Add(FVector(x + Constants::ChunkScale, y + Constants::ChunkScale, z + Constants::ChunkScale));


		indice.Add(index);
		indice.Add(index + 1);
		indice.Add(index + 2);


		indice.Add(index + 2);
		indice.Add(index + 3);
		indice.Add(index);

		index += 4;

		//Left

		vectors.Add(FVector(x, y, z + Constants::ChunkScale));
		vectors.Add(FVector(x, y, z));
		vectors.Add(FVector(x, y + Constants::ChunkScale, z));
		vectors.Add(FVector(x, y + Constants::ChunkScale, z + Constants::ChunkScale));


		indice.Add(index);
		indice.Add(index + 1);
		indice.Add(index + 2);


		indice.Add(index + 2);
		indice.Add(index + 3);
		indice.Add(index);

		index += 4;

		//Right

		vectors.Add(FVector(x + Constants::ChunkScale, y, z));
		vectors.Add(FVector(x + Constants::ChunkScale, y, z + Constants::ChunkScale));
		vectors.Add(FVector(x + Constants::ChunkScale, y + Constants::ChunkScale, z + Constants::ChunkScale));
		vectors.Add(FVector(x + Constants::ChunkScale, y + Constants::ChunkScale, z));


		indice.Add(index);
		indice.Add(index + 1);
		indice.Add(index + 2);


		indice.Add(index + 2);
		indice.Add(index + 3);
		indice.Add(index);

		index += 4;

		//Top

		vectors.Add(FVector(x, y + Constants::ChunkScale, z));
		vectors.Add(FVector(x + Constants::ChunkScale, y + Constants::ChunkScale, z));
		vectors.Add(FVector(x + Constants::ChunkScale, y + Constants::ChunkScale, z + Constants::ChunkScale));
		vectors.Add(FVector(x, y + Constants::ChunkScale, z + Constants::ChunkScale));

		indice.Add(index);
		indice.Add(index + 1);
		indice.Add(index + 2);


		indice.Add(index + 2);
		indice.Add(index + 3);
		indice.Add(index);

		index += 4;
		
		//Bottom		
		vectors.Add(FVector(x + Constants::ChunkScale, y, z));
		vectors.Add(FVector(x, y, z));
		vectors.Add(FVector(x, y, z + Constants::ChunkScale));
		vectors.Add(FVector(x + Constants::ChunkScale, y, z+ Constants::ChunkScale));


		indice.Add(index);
		indice.Add(index + 1);
		indice.Add(index + 2);

		indice.Add(index + 2);
		indice.Add(index + 3);
		indice.Add(index);
		
		index += 4;

		color.Add(FLinearColor::Black);
		color.Add(FLinearColor::Black);
		color.Add(FLinearColor::Black);
		color.Add(FLinearColor::Black);
		color.Add(FLinearColor::Black);
		color.Add(FLinearColor::Black);
		color.Add(FLinearColor::Black);
		color.Add(FLinearColor::Black);
	}

	chunk->Mesh->CreateMeshSection_LinearColor(int32(0), vectors, indice, TArray<FVector>(), TArray<FVector2D>(), color, TArray<FProcMeshTangent>(), false);
}

ChunkMesher::~ChunkMesher()
{
}