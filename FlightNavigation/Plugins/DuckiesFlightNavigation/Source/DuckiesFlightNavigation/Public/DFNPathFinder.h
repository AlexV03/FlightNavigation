// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DFNLink.h"
#include "DFNBoundingVolume.h"
#include "DFNPathFinder.generated.h"

//A* implementation
//Path finding settings, weights, score number, unit cost, distacne


//navagent component
//Having some settings,
//able to add flymovecomponent
//Agent radius, 

//Research how ue5 does agent now,
//and how they work with componenets
//do you create a agent and then add movemement component/pathfinder component, or something else

USTRUCT()
struct DUCKIESFLIGHTNAVIGATION_API FDFNPathNode
{
	GENERATED_BODY()

	//FDFNLink nextNode;
	//FDFNLink previousNode;
	//Hcost
	//Fcost
	FDFNLink nodeLink;
	float fScore = 0;
	float gScore = 0;

	bool operator==(const FDFNPathNode& other) const;
};

//Used in heap to sort pathnode by Fscore
//struct SortFScoreHeap
//{
//	bool operator()(const FDFNPathNode& A, const FDFNPathNode& B) const
//	{
//		return A.fScore < B.fScore;
//	}
//};
 
class DUCKIESFLIGHTNAVIGATION_API DFNPathFinder
{
public:
	DFNPathFinder(const ADFNBoundingVolume& _boundingVolume)
		: boundingVolume(_boundingVolume) {};

	bool FindPath(const FVector& startPos, const FVector& targetPos, const float agentSize);
	const TArray<FVector>& GetPath() const { return finalPath; }
	void DrawPath();

	//How many times a* can iterate before being terminated
	int maxIterations = 50000;
private:
	//Store nodes to visit
	TArray<FDFNPathNode> open;
	//Quick lookup to see if node is stored.??
	//TSet<FDFNPathNode> openLookUp;
	//Stores the nodes that have been visited
	TSet<FDFNLink> closed;
	//TArray<FDFNPathNode> cameFrom;//Store the nodes 
	
	//Store the node from which we arrived
	TMap<FDFNLink, FDFNLink> cameFrom;
	//Cost of the path from startNode to NextNode
	TMap<FDFNLink, float> GScore;
	//Netx node Gcost + H next node
	//TMap<FDFNPathNode, float> FScore;

	FVector agentPos;

	//Store currentLink, startLink, goalLink?? maybe instead of link a FDFNPathNode?

	//Distance between nextNode and targetGoal
	float CalculateHeuristic(const FDFNLink& fromLink, const FDFNLink& targetLink);
	//float CalculateCost(const FDFNPathNode& startNode, const FDFNPathNode& targetNode);
	void BuildPath(const FDFNLink& targetLink, const float agentSize);
	void Reset();
	bool HasLineOfSight(const FVector& fromPos, const FVector& toPos, const float agentSize);

	//The octree
	//const FDFNOctreeData& octD;
	const ADFNBoundingVolume& boundingVolume;

	TArray<FVector> finalPath;
};

//CreatePath(startPoint, endPoint)
//Open list