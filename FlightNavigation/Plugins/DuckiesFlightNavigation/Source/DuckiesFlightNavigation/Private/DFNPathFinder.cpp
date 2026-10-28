// Fill out your copyright notice in the Description page of Project Settings.

#include "DFNPathFinder.h"
#include "Algo/Reverse.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

bool DFNPathFinder::FindPath(const FVector& _startPos, const FVector& _targetPos, const float _agentSize)
{
	//Clear the data
	Reset();

	//Store agent position to use later for Line of sight check
	agentPos = _startPos;

	FDFNLink startLink;
	FDFNLink targetLink;

	if (!boundingVolume.CalculateLinkFromPosition(_startPos, startLink)) return false;
	if (!boundingVolume.CalculateLinkFromPosition(_targetPos, targetLink)) return false;

	FDFNPathNode startPathNode;

	startPathNode.nodeLink = startLink;
	startPathNode.gScore = 0.0f;
	startPathNode.fScore = CalculateHeuristic(startLink, targetLink);

	int iteration = 0;

	open.Add(startPathNode);
	GScore.Add(startLink, 0.0f);

	//while open has more than 0 elements
	while (!open.IsEmpty())
	{
		//Loop through all open nodes to find node with lowest f score
		int32 bestIdx = 0;
		for (int32 i = 1; i < open.Num(); i++)
		{
			if (open[i].fScore < open[bestIdx].fScore)
				bestIdx = i;
		}

		//pop open
		FDFNPathNode node = open[bestIdx];
		open.RemoveAtSwap(bestIdx);

		if (closed.Contains(node.nodeLink))
			continue;

		closed.Add(node.nodeLink);

		if (node.nodeLink == targetLink)
		{
			BuildPath(node.nodeLink, _agentSize);
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("I HAVE BEEN BUILDS!"));

			return true;
		}

		TArray<FDFNLink> neighbors = boundingVolume.GetNeighborLinks(node.nodeLink);

		for (const FDFNLink& neighborLink : neighbors)
		{
			if (neighborLink.layer == 15 || closed.Contains(neighborLink))
				continue;

			FVector currentPosition;
			FVector neighborPosition;
			boundingVolume.CalculatePositionFromLink(node.nodeLink, currentPosition);
			boundingVolume.CalculatePositionFromLink(neighborLink, neighborPosition);

			//Calculate cost between two nodes based on distance
			float stepCost = FVector::Dist(currentPosition, neighborPosition);
			float tentativeG = node.gScore + stepCost;
			float* existingG = GScore.Find(neighborLink);

			//add new node when current node's Gcost is lower than existing Gcost
			if (!existingG || tentativeG < *existingG)
			{
				GScore.Add(neighborLink, tentativeG);
				cameFrom.Add(neighborLink, node.nodeLink);

				FDFNPathNode neighborNode;
				neighborNode.nodeLink = neighborLink;
				neighborNode.gScore = tentativeG;
				neighborNode.fScore = tentativeG + CalculateHeuristic(neighborLink, targetLink);

				open.Add(neighborNode);
			}
		}

		if (++iteration > maxIterations)
			return false;
	}

	return false;
}

void DFNPathFinder::DrawPath()
{
	if (!finalPath.IsEmpty())
	{
		if (UWorld* world = boundingVolume.GetWorld())
		{
			//Draw line between 2 points
			for (int32 i = 0; i < finalPath.Num() - 1; i++)
			{
				DrawDebugLine(world, finalPath[i], finalPath[i + 1], FColor::Cyan, false, -1.0f, 0, 4.0f);
			}

			//Draw spheres for start, goal and inbetween locations
			for (int32 i = 0; i < finalPath.Num(); i++)
			{
				FColor pointColor = (i == 0) ? FColor::Green        // start
					: (i == finalPath.Num() - 1) ? FColor::Red      // goal
					: FColor::Yellow;                               // intermediate waypoint
				DrawDebugSphere(world, finalPath[i], 15.0f, 8, pointColor, false, -1.0f, 0, 2.0f);
			}
		}
	}
}

float DFNPathFinder::CalculateHeuristic(const FDFNLink& _fromLink, const FDFNLink& _targetLink)
{
	FVector startPos;
	FVector targetPos;

	boundingVolume.CalculatePositionFromLink(_fromLink, startPos);
	boundingVolume.CalculatePositionFromLink(_targetLink, targetPos);

	//Euclidian distance
	return (startPos - targetPos).Size();
}

void DFNPathFinder::BuildPath(const FDFNLink& _targetLink, const float _agentSize)
{
	TArray<FDFNLink> path;

	//Add target link to path
	FDFNLink current = _targetLink;
	path.Add(current);

	//Walk back through cameFrom until fully walked through
	while (const FDFNLink* parent = cameFrom.Find(current))
	{
		current = *parent;
		path.Add(current);
	}

	//Flip the path so it becomes goal -> ... -> target
	Algo::Reverse(path);

	FVector lastNode = agentPos;
	FVector pos;

	//Pathnode pruning
	//Loop through the path and do line-of-sight check
	for (int32 i = 1; i < path.Num(); i++)
	{
		boundingVolume.CalculatePositionFromLink(path[i], pos);
		if (HasLineOfSight(lastNode, pos, _agentSize))
		{
			boundingVolume.CalculatePositionFromLink(path[i - 1], pos);
			lastNode = pos;
			finalPath.Add(lastNode);
		}
	}

	if (!path.IsEmpty())
	{
		boundingVolume.CalculatePositionFromLink(path.Last(), pos);
		finalPath.Add(pos);
	}
}

void DFNPathFinder::Reset()
{
	open.Empty();
	closed.Empty();
	cameFrom.Empty();
	GScore.Empty();
	finalPath.Empty();
}

bool DFNPathFinder::HasLineOfSight(const FVector& _fromPos, const FVector& _toPos, const float _agentSize)
{
	FHitResult Hit;
	bool bHit = UKismetSystemLibrary::SphereTraceSingle(
		boundingVolume.GetWorld(), _fromPos, _toPos, _agentSize,
		UEngineTypes::ConvertToTraceType(ECC_WorldStatic), false, {},
		EDrawDebugTrace::None, Hit, true);

	return bHit;
}
