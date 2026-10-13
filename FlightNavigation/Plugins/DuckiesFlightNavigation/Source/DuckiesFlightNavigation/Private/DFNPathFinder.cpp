// Fill out your copyright notice in the Description page of Project Settings.

#include "DFNPathFinder.h"
#include "Algo/Reverse.h"
#include "DrawDebugHelpers.h"

void DFNPathFinder::FindPath(const FVector& startPos, const FVector& targetPos)
{
	//Clear the data
	Reset();

	//Calculate startLink from position
	//FDFNLink startLink;
	//boundingVolume.CalculateLinkFromPosition(startPos, startLink)

	//Calculate targetLink from position
	//FDFNLink targetLink;
	//boundingVolume.CalculateLinkFromPosition(startPos, targetLink)

	FDFNLink startLink;
	FDFNLink targetLink;

	if (!boundingVolume.CalculateLinkFromPosition(startPos, startLink))
	{
		return;
	}
	if (!boundingVolume.CalculateLinkFromPosition(targetPos, targetLink)) return;

	FDFNPathNode startPathNode;
	//FDFNPathNode targetPathNode;

	//Set startPathNode.nodeLink
	//Set targetPathNode.nodeLink
	startPathNode.nodeLink = startLink;
	startPathNode.gScore = 0.0f;
	startPathNode.fScore = CalculateHeuristic(startLink, targetLink);

	int iteration = 0;

	//Add node to each array
	open.Add(startPathNode);
	//openLookUp.Add(startPathNode);
	//cameFrom.Add(startPathNode, startPathNode);
	GScore.Add(startLink, 0.0f);
	//FScore.Add(startPathNode, CalculateHeuristic(startPathNode, targetPathNode));

	//while openheap has more than 0 elements
	while (!open.IsEmpty())
	{
		//Loop through all open nodes to find node with lowest f score
		int32 bestIdx = 0;
		for (int32 i = 1; i < open.Num(); i++)
		{
			if (open[i].fScore < open[bestIdx].fScore)
				bestIdx = i;
		}


		//open.pop
		FDFNPathNode node = open[bestIdx];
		open.RemoveAtSwap(bestIdx);

		if (closed.Contains(node.nodeLink))
			continue;
		//Add node to closedlist
		closed.Add(node.nodeLink);
		//check if current link is target link
		//if (node.nodeLink == targetLink)

		if (node.nodeLink.nodeIndex == targetLink.nodeIndex)
		{
			UE_LOG(LogTemp, Warning, TEXT("layersAAAAAAAA.Num()="));
		}
		
		//Skip subnodes for now. TODO: implement way to check if target is inside a subnode to have precies navigation
		if (node.nodeLink.layer == targetLink.layer && node.nodeLink.nodeIndex == targetLink.nodeIndex)
		{
			BuildPath(node.nodeLink);
			return;
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

			float stepCost = FVector::Dist(currentPosition, neighborPosition);

			float tentativeG = node.gScore + stepCost;
			float* existingG = GScore.Find(neighborLink);

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

		//const FDFNNode& currentNode = octD.GetNode(node.nodeLink);
		//TArray<FDFNLink> neighbors;

		//if layer == 0
			//loop through the leafneighbors
			// do the F G H calculations, add them to the proper arrays
		//else
			//loop through normal neighbors
			// do the F G H calculations, add them to the proper arrays

		//if(node.nodeLink.layer == 0 && currentNode.firstChild.layer != 15)
			//Get leafneighbors
			//Loop through each neighbor
			//if closed doesnt contain this neighbor, then calculate F G H, add to proper arrays
		//else
			//Get neighbors
			//Loop through neihbors
			//if closed doesnt contain this neighbor, then calculate F G H, add to proper arrays

		//increase iteration
		//iteration++;
		//if max iterations reached, stop
		if (++iteration > maxIterations)//Needed?
			return;
	}
}

float DFNPathFinder::CalculateHeuristic(const FDFNLink& fromLink, const FDFNLink& targetLink)
{
	FVector startPos;
	FVector targetPos;

	boundingVolume.CalculatePositionFromLink(fromLink, startPos);
	boundingVolume.CalculatePositionFromLink(targetLink, targetPos);

	//Euclidian distance
	return (startPos - targetPos).Size();
}

void DFNPathFinder::BuildPath(const FDFNLink& targetLink)
{
	TArray<FDFNLink> path;

	//Add target link to path
	FDFNLink current = targetLink;
	path.Add(current);

	//Walk back through cameFrom until fully walked through
	while (const FDFNLink* parent = cameFrom.Find(current))
	{
		current = *parent;
		path.Add(current);
	}

	//Flip the path so it becomes goal -> ... -> target
	Algo::Reverse(path);

	//Convert links to position
	for (const FDFNLink& link : path)
	{
		FVector pos;
		boundingVolume.CalculatePositionFromLink(link, pos);
		finalPath.Add(pos);
	}

	//UE_LOG(LogTemp, Warning, TEXT("layers.Num()=%d"), finalPath.Num());

	if (UWorld* world = boundingVolume.GetWorld())
	{
		//Draw line between 2 points
		for (int32 i = 0; i < finalPath.Num() - 1; i++)
		{
			DrawDebugLine(world, finalPath[i], finalPath[i + 1], FColor::Cyan, true, -1.0f, 0, 4.0f);
		}

		//Draw spheres for start, goal and inbetween locations
		for (int32 i = 0; i < finalPath.Num(); i++)
		{
			FColor pointColor = (i == 0) ? FColor::Green        // start
				: (i == finalPath.Num() - 1) ? FColor::Red      // goal
				: FColor::Yellow;                               // intermediate waypoint
			DrawDebugSphere(world, finalPath[i], 15.0f, 8, pointColor, true, -1.0f, 0, 2.0f);
		}
	}
}

void DFNPathFinder::Reset()
{
	open.Empty();
	closed.Empty();
	cameFrom.Empty();
	GScore.Empty();
	finalPath.Empty();

	if (UWorld* world = boundingVolume.GetWorld())
	{
		FlushPersistentDebugLines(world);
	}
}
