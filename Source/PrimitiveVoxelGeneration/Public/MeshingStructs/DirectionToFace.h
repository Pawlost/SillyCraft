#pragma once
#include "FaceDirection.h"

struct FDirectionToFace
{
	const EFaceDirection FaceSide;
	const FIntVector Direction;
	FDirectionToFace(const EFaceDirection faceSide, const FIntVector& direction)
		:
		FaceSide(faceSide),
		Direction(direction)
	{
	}

	static FDirectionToFace FrontDirection;
	static FDirectionToFace BackDirection;
	static FDirectionToFace LeftDirection;
	static FDirectionToFace RightDirection;
	static FDirectionToFace TopDirection;
	static FDirectionToFace BottomDirection;
};