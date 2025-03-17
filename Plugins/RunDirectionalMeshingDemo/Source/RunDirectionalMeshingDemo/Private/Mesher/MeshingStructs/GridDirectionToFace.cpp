#include "Mesher/MeshingStructs/GridDirectionToFace.h"

FGridDirectionToFace FGridDirectionToFace::TopDirection = FGridDirectionToFace(EFaceDirection::Top, FIntVector(0, 0,1));
FGridDirectionToFace FGridDirectionToFace::BottomDirection = FGridDirectionToFace(EFaceDirection::Bottom, FIntVector(0, 0,-1));
FGridDirectionToFace FGridDirectionToFace::BackDirection = FGridDirectionToFace(EFaceDirection::Back, FIntVector(1, 0,0));
FGridDirectionToFace FGridDirectionToFace::FrontDirection = FGridDirectionToFace(EFaceDirection::Front, FIntVector(-1, 0,0));
FGridDirectionToFace FGridDirectionToFace::LeftDirection = FGridDirectionToFace(EFaceDirection::Left, FIntVector(0, 1,0));
FGridDirectionToFace FGridDirectionToFace::RightDirection = FGridDirectionToFace(EFaceDirection::Right, FIntVector(0, -1,0));