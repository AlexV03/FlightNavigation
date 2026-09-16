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

	TArray<TArray<FDFNNode>> levels;
	TArray<FDFNLeafNode> leafNodes;

	TArray<TSet<uint32_t>> mortonCodes; //Used during the first rasterize pass
};

