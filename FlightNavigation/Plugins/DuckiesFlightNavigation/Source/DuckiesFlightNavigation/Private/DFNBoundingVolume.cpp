// Fill out your copyright notice in the Description page of Project Settings.


#include "DFNBoundingVolume.h"
#include "DrawDebugHelpers.h"
#include "libmorton/morton.h"
#include "Algo/Unique.h"
#include "Math/Color.h"

const TArray<FColor> ADFNBoundingVolume::layerColors = {
		FColor(230, 25, 75),   // Red
		FColor(60, 180, 75),   // Green
		FColor(255, 225, 25),  // Yellow
		FColor(0, 130, 200),   // Blue
		FColor(246, 167, 49),  // Orange
		FColor(145, 30, 180),  // Purple
		FColor(70, 240, 240),  // Cyan
		FColor(240, 50, 230),  // Magenta
		FColor(210, 245, 60),  // Lime
		FColor(250, 190, 212), // Pink
		FColor(0, 128, 128),   // Teal
		FColor(220, 190, 255), // Lavender
		FColor(170, 110, 40),  // Brown
		FColor(128, 0, 0),     // Maroon
		FColor(170, 255, 195), // Mint
		FColor(0, 0, 128)      // Navy
};

// Sets default values
ADFNBoundingVolume::ADFNBoundingVolume()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADFNBoundingVolume::BeginPlay()
{
	Super::BeginPlay();
	Generate();
}

//Formula found in the link below.
//https://stackoverflow.com/questions/1322510/given-an-integer-how-do-i-find-the-next-largest-power-of-two-using-bit-twiddlin/1322548#1322548
unsigned int ADFNBoundingVolume::RoundUp2Pow2(unsigned int value)
{
	unsigned int v;

	v = value;

	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;

	return v;
}

bool ADFNBoundingVolume::GetNodePosition(float nodeSize, uint_fast64_t mCode, FVector& position) const
{
	uint_fast32_t X, Y, Z;
	morton3D_64_decode(mCode, X, Y, Z);
	//position = origin - (newVolumeSize * 0.5f) + (FVector(X, Y, Z) * nodeSize) + FVector(nodeSize * 0.5f);
	position = origin + (FVector(X, Y, Z) * nodeSize) + FVector(nodeSize * 0.5f);

	return true;
}

uint64 ADFNBoundingVolume::GetParentCode(uint64 childCode)
{
	return childCode >> 3;
}

uint64 ADFNBoundingVolume::GetChildCode(uint64 parentCode, unsigned int index)
{
	return (parentCode << 3) | index;
}

int32 ADFNBoundingVolume::GetNodeAmountInLayer(uint8 layer) const
{
	return FMath::Pow(FMath::Pow(2.0f, ((octD.NumberLayers - 1) - layer)), 3);
}

float ADFNBoundingVolume::GetVoxelSize(uint8 layer) const
{
	return ((newVolumeSize * 0.5) / FMath::Pow(2.f, (octD.NumberLayers - 1))) * (FMath::Pow(2.0f, layer + 1));
}

bool ADFNBoundingVolume::GetIndexFromCode(uint8 layer, uint64 mCode, int32& cIndex) const
{
	const TArray<FDFNNode>& nLayer = octD.layers[layer];

	int low = 0;
	int high = nLayer.Num() - 1;

	while (low <= high)
	{
		int mid = low + (high - low) / 2;
		uint64 midCode = nLayer[mid].mortonCode;

		if (midCode == mCode)
		{
			cIndex = mid;
			return true;
		}

		if (midCode < mCode)
			low = mid + 1;
		else
			high = mid - 1;
	}

	return false;
}

bool ADFNBoundingVolume::CheckIfNodeIsBlocked(uint8 layer, uint64 mCode)
{
	if (layer == octD.NumberLayers - 1)
		return true;

	return octD.mortonCodes[layer].Contains(GetParentCode(mCode));
}

FVector ADFNBoundingVolume::GetFaceDirection(int faceIdx)
{
	switch (faceIdx)
	{
	case 0: return FVector(1, 0, 0);   // +X
	case 1: return FVector(-1, 0, 0);  // -X
	case 2: return FVector(0, 1, 0);   // +Y
	case 3: return FVector(0, -1, 0);  // -Y
	case 4: return FVector(0, 0, 1);   // +Z
	case 5: return FVector(0, 0, -1);  // -Z
	default: return FVector(0, 0, 0);
	}
}

int32 ADFNBoundingVolume::GetNodeAmountOfSide(uint8 layer)
{
	return FMath::Pow(2.f, (octD.NumberLayers - (layer)));
}

void ADFNBoundingVolume::FindNeighborInParents(uint8 layer, int32 nodeIndex, uint8 faceIndex, uint8& parentLayer, int32& neighborNodeIndex)
{
	UWorld* world = GetWorld();
	FVector originPos;
	GetNodePosition(GetVoxelSize(layer), octD.layers[layer][nodeIndex].mortonCode, originPos);

	uint8 currentLayer = layer;
	int32 currentIndex = nodeIndex;

	while (true)
	{
		FDFNLink& parentLink = octD.layers[currentLayer][currentIndex].parent;

		//Reached root, set invalid and return
		if (parentLink.layer == 15)
		{
			parentLayer = 15;
			neighborNodeIndex = 0;
			return;
		}

		currentLayer = parentLink.layer;
		currentIndex = parentLink.nodeIndex;

		uint_fast32_t x, y, z;
		morton3D_64_decode(octD.layers[currentLayer][currentIndex].mortonCode, x, y, z);

		FVector neighborPosition = FVector(x, y, z) + GetFaceDirection(faceIndex);
		int32 boundsSideSize = GetNodeAmountOfSide(currentLayer);

		//Check bounds of neighbor
		if (neighborPosition.X < 0 || neighborPosition.X >= boundsSideSize ||
			neighborPosition.Y < 0 || neighborPosition.Y >= boundsSideSize ||
			neighborPosition.Z < 0 || neighborPosition.Z >= boundsSideSize)
		{//Node is out of bounds, return
			continue;
		}

		uint64 neighborMCode = morton3D_64_encode(neighborPosition.X, neighborPosition.Y, neighborPosition.Z);

		int32 foundIndex = 0;
		if (GetIndexFromCode(currentLayer, neighborMCode, foundIndex))
		{
			//Node found, set variables, return
			parentLayer = currentLayer;
			neighborNodeIndex = currentIndex;

			//Draw debug line between node and its neighbor. Meaning no direct neighbor
			if (world && showNeighborLinks)
			{
				FVector neighborPos;
				GetNodePosition(GetVoxelSize(currentLayer), octD.layers[currentLayer][foundIndex].mortonCode, neighborPos);

				//Color = redish
				DrawDebugLine(world, originPos, neighborPos, FColor(255, 0, 127), true, -1.0f, 0, 2.0f);

				UE_LOG(LogTemp, Warning,
					TEXT("Neighbor (climbed): orig layer=%d node=%d face=%d -> found layer=%d node=%d"),
					layer, nodeIndex, faceIndex, currentLayer, foundIndex);
			}

			return;
		}
	}
}

// Called every frame
void ADFNBoundingVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if WITH_EDITOR
	UWorld* world = GetWorld();
	if (world && !world->IsGameWorld())
	{
		FVector origin2 = GetActorLocation();
		FVector extent = volumeSize * 0.5f * GetActorScale3D();
		DrawDebugBox(world, origin2, extent, FQuat::Identity, GetColorAt(0), false, 0.0f, 0, 2.0f);
	}
#endif
}

void ADFNBoundingVolume::RasterizeLayer(uint8 layer)
{
	UWorld* world = GetWorld();
	int32 nodeAmount = GetNodeAmountInLayer(layer);
	float nodeSize = GetVoxelSize(layer);

	if (layer == 0)
	{
		for (int32 i = 0; i < nodeAmount; i++)
		{
			if (CheckIfNodeIsBlocked(layer, i))
			{
				FDFNNode node;
				FDFNLink link;
				link.layer = 0;
				link.nodeIndex = i;
				link.subNodeIndex = 0;
				node.firstChild = link;
				node.mortonCode = i;

				if (world)
				{
					FVector position;
					GetNodePosition(nodeSize, i, position);
					DrawDebugBox(world, position, FVector(nodeSize * 0.5f), FQuat::Identity, GetColorAt(layer), true, -1.0f, layer, 2.0f);
				}

				octD.layers[0].Add(node);
			}
		}
	}
	else if (layer > 0)
	{
		for (int32 i = 0; i < nodeAmount; i++)
		{
			//If statement to check if this node is within a child node that has morton code(collision)
			if (CheckIfNodeIsBlocked(layer, i))
			{
				int32 nodeIndex = octD.layers[layer].Emplace();

				//do the layer caluclations
				FDFNNode& node = octD.layers[layer][nodeIndex];
				FDFNLink link;
				int32 childIndex = 0;
				//Parent -> child. Giving parent node its child node
				node.mortonCode = i;

				//If GetChildNode
				if (GetIndexFromCode(layer - 1, node.mortonCode << 3, childIndex))
				{
					link.layer = layer - 1;
					link.nodeIndex = childIndex;
					link.subNodeIndex = 0;
					node.firstChild = link;

					//Child -> parent. Go through 8 children and setting there parent link
					for (int ci = 0; ci < 8; ci++)
					{
						octD.layers[node.firstChild.layer][node.firstChild.nodeIndex + ci].parent.layer = layer;
						octD.layers[node.firstChild.layer][node.firstChild.nodeIndex + ci].parent.nodeIndex = nodeIndex;
					}

					if (world && showRootNode)
					{
						FVector position;
						GetNodePosition(nodeSize, i, position);
						DrawDebugBox(world, position, FVector(nodeSize * 0.5f), FQuat::Identity, GetColorAt(layer), true, -1.0f, layer, 2.0f);
					}
				}
			}
		}
	}
}

void ADFNBoundingVolume::RasterizeLeafNode()
{
	UWorld* world = GetWorld();

	float leafSize = GetVoxelSize(0);
	int32 leafIndex = 0;

	for (int i = 0; i < octD.leafNodes.Num(); i++)
	{
		FDFNNode& node = octD.layers[0][i];

		FVector leafCenter;
		GetNodePosition(leafSize, node.mortonCode, leafCenter);
		FVector lOri = leafCenter - FVector(leafSize * 0.5f);

		if (CheckCollisionOverlap(leafCenter, ECC_WorldStatic, leafSize))
		{
			//do 64 loop
			for (int v = 0; v < 64; v++)
			{
				uint_fast32_t x, y, z;
				morton3D_64_decode(v, x, y, z);//Position of node
				FVector position = lOri + FVector(x * voxelResolution, y * voxelResolution, z * voxelResolution) + FVector(voxelResolution * 0.5f);

				if (CheckCollisionOverlap(position, ECC_WorldStatic, voxelResolution))
				{
					//set voxel bit in the 64bit var

					octD.leafNodes[leafIndex].SetVoxelBit(v);

					//Debug render voxel
					if (world && showSubNodes)
					{
						DrawDebugBox(world, position, FVector(voxelResolution * 0.5f), FQuat::Identity, GetColorAt(9), true, -1.0f, 0, 2.0f);
					}
				}
			}

			leafIndex++;
		}
	}
}

//Idea: Box needs to be power of 2, could make it that the bounding box is still the real navmesh bounds. so everything outside the
//bounding box will be "blocked" and everything inside will be calculated normally.


void ADFNBoundingVolume::RasterizeFirstLayer()
{
	octD.mortonCodes.Emplace();

	float leafSize = 4 * voxelResolution;
	float nodeSize = leafSize * 2;

	FVector layerOneNodes = volumeSize / nodeSize;

	//TArray<uint64> mortonCodes; //Storing voxels that have collision
	//mortonCodes.Emplace();

	UWorld* world = GetWorld();
	//Modify to make it cover the whole octree eventually
	//if (world)
	//{
	//	FVector origin = GetActorLocation();
	//	FVector extent = nodeSize * GetActorScale3D();
	//	DrawDebugBox(world, origin, extent, FQuat::Identity, FColor::Blue, true, -1.0f, 0, 2.0f);
	//}

	unsigned int X = RoundUp2Pow2(ceil(layerOneNodes.X));
	unsigned int Y = RoundUp2Pow2(ceil(layerOneNodes.Y));
	unsigned int Z = RoundUp2Pow2(ceil(layerOneNodes.Z));

	int32 layerOneDim = FMath::Max3(X, Y, Z);
	//int32 nodeAmount = layerOneDim * 3;

	newVolumeSize = nodeSize * layerOneDim;
	origin = GetActorLocation() - FVector(newVolumeSize * 0.5f);

	octD.NumberLayers = 2 + log2(layerOneDim);
	//UE_LOG(LogTemp, Warning, TEXT("My integer value idddddddddds: %d"), newVolumeSize);
	UE_LOG(LogTemp, Warning, TEXT("!VOLUME size: %d"), newVolumeSize);
	//int32 nodeAmount = 0;

	int32 na = GetNodeAmountInLayer(1);
	for (int i = 0; i < na; i++)
	{
		FVector position;
		//int32 numNode = xi + yi + zi;
		GetNodePosition(nodeSize, i, position);

		//UE_LOG(LogTemp, Warning, TEXT("My integer value is: %d"), nodeAmount);
		//UE_LOG(LogTemp, Warning, TEXT("My POSITION is: %s"), *position.ToString());
		if (CheckCollisionOverlap(position, ECC_WorldStatic, nodeSize))
		{
			octD.mortonCodes[0].Add(i);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("FINAL: My codes amount are: %d"), octD.mortonCodes.Num());
}

void ADFNBoundingVolume::BuildNeighborLinks(uint8 layer)
{
	UWorld* world = GetWorld();

	TArray<FDFNNode>& nodeLayer = octD.layers[layer];

	for (int i = 0; i < nodeLayer.Num(); i++)
	{
		FDFNNode& node = nodeLayer[i];

		uint_fast32_t x, y, z;
		morton3D_64_decode(node.mortonCode, x, y, z);
		float nodeSize = GetVoxelSize(layer);
		FVector nodePosition;
		GetNodePosition(nodeSize, node.mortonCode, nodePosition);

		//For each direction
		for (int d = 0; d < 6; d++)
		{
			FDFNLink& neighborLink = node.neighbors[d];

			FVector neighborCoords = FVector(x, y, z) + GetFaceDirection(d);
			int32 layerSize = GetNodeAmountOfSide(layer);

			//If node is outside the bounds
			if (neighborCoords.X < 0 || neighborCoords.X >= layerSize ||
				neighborCoords.Y < 0 || neighborCoords.Y >= layerSize ||
				neighborCoords.Z < 0 || neighborCoords.Z >= layerSize)
			{
				neighborLink.layer = 15;
				continue;
			}

			uint_fast32_t nx, ny, nz;
			nx = neighborCoords.X;
			ny = neighborCoords.Y;
			nz = neighborCoords.Z;
			uint64_t neightborMCode = morton3D_64_encode(nx, ny, nz);

			int32 nIndex = 0;
			if (GetIndexFromCode(layer, neightborMCode, nIndex))
			{
				//Found neightbor, set link
				neighborLink.layer = layer;
				neighborLink.nodeIndex = nIndex;

				//Draw debug line between direct neighbors
				if (world && showNeighborLinks)
				{
					FVector neighborPos;
					GetNodePosition(nodeSize, octD.layers[layer][nIndex].mortonCode, neighborPos);

					//Color = prupleish
					DrawDebugLine(world, nodePosition, neighborPos, FColor(127, 0, 255), true, -1.0f, 0, 2.0f);
				}
			}
			else//No neighbor found
			{
				uint8 newLayer = 0;
				int32 nnIndex = 0;
				FindNeighborInParents(layer, i, d, newLayer, nnIndex);
				neighborLink.layer = newLayer;
				neighborLink.nodeIndex = nnIndex;
			}
		}
	}
}

bool ADFNBoundingVolume::CheckCollisionOverlap(const FVector& position, ECollisionChannel colChannel, const float voxelSize)
{
	bool occupied = GetWorld()->OverlapAnyTestByChannel(position, FQuat::Identity, colChannel, FCollisionShape::MakeBox(FVector(voxelSize * 0.5f)));
	return occupied;
}

void ADFNBoundingVolume::Generate()
{
	octD.Reset();//Clear data upon generation

	//first pass rasterizer.
	RasterizeFirstLayer();

	//Creates for each layer, morton codes
	CreateLayerMortonCodes();

	//clear leafnodes
	//allocate leafnodes
	//octD.leafNodes.Empty();

	octD.leafNodes.AddDefaulted(octD.mortonCodes[0].Num() * 8);

	for (int i = 0; i < octD.NumberLayers; i++)
	{
		//allocate amount of layers into octree
		octD.layers.Emplace();
	}

	//for (int i = 0; i < octD.NumberLayers - 1; i++)
	for (int i = 0; i < octD.NumberLayers; i++)
	{
		//Rasterrize all layers(down to up)
		RasterizeLayer(i);
	}

	RasterizeLeafNode();

	for (int i = octD.NumberLayers - 2; i >= 0; i--)
	{
		//creating neighbore links(up to down)
		BuildNeighborLinks(i);
	}
}

const FColor& ADFNBoundingVolume::GetColorAt(int32 index) const
{
	if (layerColors.IsValidIndex(index))
	{
		return layerColors[index];
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Index out of range : layerColors"));
		return FColor::Black;
	}
}

void ADFNBoundingVolume::CreateLayerMortonCodes()
{
	int index = 0;

	while (octD.mortonCodes[index].Num() > 1)
	{
		// Add a new morton code layer
		octD.mortonCodes.Emplace();
		// Add any parent morton codes to the new layer
		for (uint64& code : octD.mortonCodes[index])
		{
			octD.mortonCodes[index + 1].Add(code >> 3);
		}
		index++;
	}
}
