// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DFNLeafNode.h"
#include "DFNNode.h"
#include "OctreeData.generated.h"


USTRUCT()
struct DUCKIESFLIGHTNAVIGATION_API FDFNOctreeData
{
	GENERATED_BODY()

	TArray<TArray<FDFNNode>> layers;
	TArray<FDFNLeafNode> leafNodes;

	//TArray<TSet<uint64_t>> mortonCodes; //Used during the first rasterize pass
	//TArray<uint32_t> mortonCodes; //Storing voxels that have collision
	TArray<uint64> mortonCodes; //Storing voxels that have collision

	uint8 NumberLayers = 0;

	void Reset()
	{
		layers.Empty();
		leafNodes.Empty();
		mortonCodes.Empty();
	}

	//int32 GetNodeAmountInLayer()
};

