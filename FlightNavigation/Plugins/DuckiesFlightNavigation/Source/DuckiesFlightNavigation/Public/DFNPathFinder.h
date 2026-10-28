// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DFNLink.h"
#include "DFNBoundingVolume.h"
#include "DFNPathFinder.generated.h"

USTRUCT()
struct DUCKIESFLIGHTNAVIGATION_API FDFNPathNode
{
	GENERATED_BODY()

	FDFNLink nodeLink;
	float fScore = 0;
	float gScore = 0;

	bool operator==(const FDFNPathNode& other) const;
};
 
class DUCKIESFLIGHTNAVIGATION_API DFNPathFinder
{
public:
	DFNPathFinder(const ADFNBoundingVolume& _boundingVolume)
		: boundingVolume(_boundingVolume) {};

	bool FindPath(const FVector& _startPos, const FVector& _targetPos, const float _agentSize);
	const TArray<FVector>& GetPath() const { return finalPath; }
	void DrawPath();

	//How many times A* can iterate before being terminated
	int maxIterations = 50000;
private:
	//Store nodes to visit
	TArray<FDFNPathNode> open;
	//Stores the nodes that have been visited
	TSet<FDFNLink> closed;
	
	//Store the node from which we arrived
	TMap<FDFNLink, FDFNLink> cameFrom;
	//Cost of the path from startNode to NextNode
	TMap<FDFNLink, float> GScore;

	FVector agentPos;

	//Distance between nextNode and targetGoal
	float CalculateHeuristic(const FDFNLink& _fromLink, const FDFNLink& _targetLink);
	//float CalculateCost(const FDFNPathNode& startNode, const FDFNPathNode& targetNode);
	void BuildPath(const FDFNLink& _targetLink, const float _agentSize);
	void Reset();
	bool HasLineOfSight(const FVector& _fromPos, const FVector& _toPos, const float _agentSize);

	const ADFNBoundingVolume& boundingVolume;

	TArray<FVector> finalPath;
};
