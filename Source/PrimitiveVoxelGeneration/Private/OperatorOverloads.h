#pragma once
#include <vector>
#include <vector>

inline bool operator<(const FIntVector& vectorA, const FIntVector& vectorB)
{	
	return vectorA.X < vectorB.X || vectorA.Y < vectorB.Y || vectorA.Z < vectorB.Z;
}

inline bool operator>(const FIntVector& vectorA, const FIntVector& vectorB)
{	
	return vectorA.X > vectorB.X || vectorA.Y > vectorB.Y || vectorA.Z > vectorB.Z;
}

inline bool operator<=(const FIntVector& vectorA, const FIntVector& vectorB)
{	
	return vectorA.X <= vectorB.X || vectorA.Y <= vectorB.Y || vectorA.Z <= vectorB.Z;
}

inline bool operator>=(const FIntVector& vectorA, const FIntVector& vectorB)
{	
	return vectorA.X >= vectorB.X || vectorA.Y >= vectorB.Y || vectorA.Z >= vectorB.Z;
}