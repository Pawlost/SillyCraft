#pragma once
#include "FaceDirection.h"

struct FGridDirectionToFace
{
	const EFaceDirection FaceSide;
	const FIntVector Direction;
	FGridDirectionToFace(const EFaceDirection faceSide, const FIntVector& direction)
		:
		FaceSide(faceSide),
		Direction(direction)
	{
	}

	static FGridDirectionToFace FrontDirection;
	static FGridDirectionToFace BackDirection;
	static FGridDirectionToFace LeftDirection;
	static FGridDirectionToFace RightDirection;
	static FGridDirectionToFace TopDirection;
	static FGridDirectionToFace BottomDirection;
};