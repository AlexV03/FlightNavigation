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
	
	//GetLayer()
	//GetNodeIndex()
	//GetSubnodeIndex()

	//subnodeIndexToCoord()
	//CoordToSubnode()
};


