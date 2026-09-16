#pragma once

#include "CoreMinimal.h"
#include "DFNLink.h"
#include "DFNNode.generated.h"


USTRUCT()
struct DUCKIESFLIGHTNAVIGATION_API FDFNNode
{
	GENERATED_BODY()

	FDFNLink firstChild;
	FDFNLink parent;
	FDFNLink neighbors[6];
	uint32_t mortonCode;
	//Bool isEmpty??
};


