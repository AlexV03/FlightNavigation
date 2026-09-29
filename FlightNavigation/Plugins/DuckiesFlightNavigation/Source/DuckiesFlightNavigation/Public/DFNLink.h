#pragma once

#include "CoreMinimal.h"
#include "DFNLink.generated.h"


USTRUCT()
struct DUCKIESFLIGHTNAVIGATION_API FDFNLink
{
	GENERATED_BODY()

	unsigned int layer : 4;			//0 - 15
	unsigned int nodeIndex : 22;	//0 - 4,194,303
	unsigned int subNodeIndex : 6;	//0 - 63(only used for indexing voxels inside leaf nodes)
	
	FDFNLink() :
		layer(15), nodeIndex(0), subNodeIndex(0) {}

	//GetLayer()
	//GetNodeIndex()
	//GetSubnodeIndex()

	//subnodeIndexToCoord()
	//CoordToSubnode()
};


