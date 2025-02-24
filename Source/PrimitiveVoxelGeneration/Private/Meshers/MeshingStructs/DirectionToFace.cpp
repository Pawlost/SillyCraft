#include "MeshingStructs/DirectionToFace.h"

FDirectionToFace FDirectionToFace::TopDirection = FDirectionToFace(EFaceDirection::Top, FIntVector(0, 0,1));
FDirectionToFace FDirectionToFace::BottomDirection = FDirectionToFace(EFaceDirection::Bottom, FIntVector(0, 0,-1));
FDirectionToFace FDirectionToFace::BackDirection = FDirectionToFace(EFaceDirection::Back, FIntVector(1, 0,0));
FDirectionToFace FDirectionToFace::FrontDirection = FDirectionToFace(EFaceDirection::Front, FIntVector(-1, 0,0));
FDirectionToFace FDirectionToFace::LeftDirection = FDirectionToFace(EFaceDirection::Left, FIntVector(0, 1,0));
FDirectionToFace FDirectionToFace::RightDirection = FDirectionToFace(EFaceDirection::Right, FIntVector(0, -1,0));