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

	inline bool IsSubnodeSolid(uint8 _subIndex) const
	{
		return (voxel & (1ull << _subIndex)) != 0;
	}

	//Set specific bit to index
	void SetVoxelBit(uint8 _index)
	{
		voxel |= 1ULL << _index;
	}
};
