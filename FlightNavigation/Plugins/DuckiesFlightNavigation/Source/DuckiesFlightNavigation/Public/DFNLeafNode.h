#pragma once

#include "CoreMinimal.h"
#include "DFNLeafNode.generated.h"

USTRUCT()
struct DUCKIESFLIGHTNAVIGATION_API FDFNLeafNode
{
	GENERATED_BODY()

	uint64 voxel = 0;

	inline bool isEmpty() const { return voxel == 0; }
	inline bool isFullyBlocked() const { return voxel == 0xFFFFFFFFFFFFFFFFull; }
	inline bool isOccupied() const { return voxel != 0; }

	//Set specific bit to index
	void SetVoxelBit(uint8 index)
	{
		voxel |= 1ULL << index;
	}

	//Get specific bit
	/*uint8 GetVoxelBit(uint64 mCode)
	{

	}*/
};
