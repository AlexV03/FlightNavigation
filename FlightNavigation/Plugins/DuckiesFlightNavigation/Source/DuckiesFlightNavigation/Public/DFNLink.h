#pragma once

#include "CoreMinimal.h"
#include "DFNLink.generated.h"


USTRUCT()
struct DUCKIESFLIGHTNAVIGATION_API FDFNLink
{
	GENERATED_BODY()

	int layer : 4;
	int nodeIndex : 22;
	int subNodeIndex : 6;

	//GetLayer()
	//GetNodeIndex()
	//GetSubnodeIndex()

	//subnodeIndexToCoord()
	//CoordToSubnode()
};


